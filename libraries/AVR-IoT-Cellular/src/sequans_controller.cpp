#include "sequans_controller.h"

#include "log.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <string.h>
#include <util/delay.h>

#include <cryptoauthlib.h>

#include <Arduino.h>
#include <pins_arduino.h>

#ifdef __AVR_AVR128DB48__ // MINI

#define TX_PIN PIN_PC0

#define CTS_PIN     PIN_PC4
#define CTS_PIN_bm  PIN4_bm
#define CTS_INT_bm  PORT_INT4_bm
#define RING_PIN    PIN_PC6
#define RING_INT_bm PORT_INT6_bm
#define RTS_PORT    PORTC
#define RTS_PIN     PIN_PC7
#define RTS_PIN_bm  PIN7_bm
#define RESET_PIN   PIN_PC5
#define HWSERIALAT  USART1

#else

#ifdef __AVR_AVR128DB64__ // Non-Mini

#define TX_PIN      PIN_PC0
#define CTS_PIN     PIN_PC6
#define CTS_PIN_bm  PIN6_bm
#define CTS_INT_bm  PORT_INT6_bm
#define RING_PIN    PIN_PC4
#define RING_INT_bm PORT_INT4_bm
#define RTS_PORT    PORTC
#define RTS_PIN     PIN_PC7
#define RTS_PIN_bm  PIN7_bm
#define RESET_PIN   PIN_PE1
#define HWSERIALAT  USART1

#else
#error "INCOMPATIBLE_DEVICE_SELECTED"
#endif
#endif

#define SEQUANS_MODULE_BAUD_RATE 115200

// Sizes for the circular buffers
#define RX_BUFFER_SIZE        512
#define TX_BUFFER_SIZE        512
#define RX_BUFFER_ALMOST_FULL (RX_BUFFER_SIZE - 2)

#define MAX_URC_CALLBACKS          8
#define URC_IDENTIFIER_BUFFER_SIZE 28

// Specifies the valid bits for the index in the buffers
#define RX_BUFFER_MASK (RX_BUFFER_SIZE - 1)
#define TX_BUFFER_MASK (TX_BUFFER_SIZE - 1)

#define RX_ELEMENTS_MASK (RX_BUFFER_SIZE * 2 - 1)

// CTS, control line for the MCU sending the LTE module data
#define sequansModuleIsReadyForData() (!(VPORTC.IN & CTS_PIN_bm))

#define LINE_FEED          '\n'
#define CARRIAGE_RETURN    '\r'
#define SPACE_CHARACTER    ' '
#define RESPONSE_DELIMITER ','

static bool initialized = false;

static const char OK_TERMINATION[] = "\r\nOK\r\n";
static const char ERROR_TERMINATION[] = "\r\nERROR\r\n";

static uint8_t rx_buffer[RX_BUFFER_SIZE];
static volatile uint16_t rx_head_index = 0;
static volatile uint16_t rx_tail_index = 0;
static volatile uint16_t rx_num_elements = 0;

static uint8_t tx_buffer[TX_BUFFER_SIZE];
static volatile uint16_t tx_head_index = 0;
static volatile uint16_t tx_tail_index = 0;
static volatile uint16_t tx_num_elements = 0;

// We keep two buffers for identifier and data so that data won't be overwritten
// whilst we are looking for a new URC. In that way the data buffer will only be
// overwritten if we find an URC we are looking for.
static volatile uint8_t urc_identifier_buffer[URC_IDENTIFIER_BUFFER_SIZE];
static volatile char urc_data_buffer[URC_DATA_BUFFER_SIZE];
static volatile uint16_t urc_identifier_buffer_length = 0;
static volatile uint16_t urc_data_buffer_length = 0;

typedef enum {
    URC_PARSING_IDENTIFIER,
    URC_EVALUATING_IDENTIFIER,
    URC_PARSING_DATA,
    URC_NOT_PARSING
} UrcParseState;

static UrcParseState urc_parse_state = URC_NOT_PARSING;

static volatile uint8_t urc_index = 0;
static volatile char urc_lookup_table[MAX_URC_CALLBACKS]
                                     [URC_IDENTIFIER_BUFFER_SIZE];
static volatile uint8_t urc_lookup_table_length[MAX_URC_CALLBACKS];
static volatile bool urc_lookup_table_clear_data[MAX_URC_CALLBACKS];
void (*urc_callbacks[MAX_URC_CALLBACKS])(char *);

// Used to keep a pointer to the URC we are processing and found to be matching,
// which will fire after we are finished processing the URC.
void (*urc_current_callback)(char *);

// Default values
static uint8_t number_of_retries = 5;
static double sleep_between_retries_ms = 200;

// Power save modes
static volatile uint8_t power_save_mode = 0;
static void (*ring_line_callback)(void);

static bool critical_section_enabled = false;

// Singleton. Defined for use of rest of library
SequansControllerClass SequansController = SequansControllerClass::instance();

/** @brief Flow control update for the UART interface with the LTE modules
 *
 * Updates RTS line based on space available in receive buffer. If the buffer
 * is close to full the RTS line is de-asserted (set high) to signal to the
 * target that no more data should be sent
 */
static void flowControlUpdate(void) {
    // If we are in a power save mode, flow control is disabled until we get a
    // RING0 ack
    if (power_save_mode == 1) {
        return;
    }

    if (critical_section_enabled) {
        return;
    }

    // We prefer to not use arduino's digitalWrite here to reduce code in the
    // ISR
    if (rx_num_elements < RX_BUFFER_ALMOST_FULL) {
        // Space for more data, assert RTS line (active low)
        VPORTC.OUT &= (~RTS_PIN_bm);
    } else {
        // Buffer is filling up, tell the target to stop sending data
        // for now by de-asserting RTS
        VPORTC.OUT |= RTS_PIN_bm;
    }
}

void CTSInterrupt(void) {
    if (PORTC.INTFLAGS & CTS_INT_bm) {

        if (PORTC.IN & CTS_PIN_bm) {
            // CTS is not asserted (active low) so disable USART Data Register
            // Empty Interrupt where the logic is to send more data
            HWSERIALAT.CTRLA &= ~USART_DREIE_bm;
        } else {
            // CTS is asserted check if there is data to transmit
            // before we enable interrupt
            HWSERIALAT.CTRLA |= USART_DREIE_bm;
        }

        PORTC.INTFLAGS = CTS_INT_bm;
    }
}

void RingInterrupt(void) {
    if (PORTC.INTFLAGS & RING_INT_bm) {
        if (PORTC.IN & RING_PIN) {
            if (ring_line_callback != NULL) {
                ring_line_callback();
            }
        }

        PORTC.INTFLAGS = RING_INT_bm;
    }
}

// RX complete
ISR(USART1_RXC_vect) {
    uint8_t data = USART1.RXDATAL;

    // We do an logical AND here as a means of allowing the index to wrap
    // around since we have a circular buffer
    rx_head_index = (rx_head_index + 1) & RX_BUFFER_MASK;
    rx_buffer[rx_head_index] = data;
    rx_num_elements++;

    // Here we keep track of the length of the URC when it starts and
    // compare it against the look up table of lengths of the strings we are
    // looking for. We compare against them first in order to save some
    // cycles in the ISR and if the lengths match, we compare the string for
    // the URC and against the buffer. If they match, call the callback
    switch (urc_parse_state) {

    case URC_NOT_PARSING:

        if (data == URC_IDENTIFIER_START_CHARACTER) {
            urc_identifier_buffer_length = 0;
            urc_parse_state = URC_EVALUATING_IDENTIFIER;
        }

        break;

    case URC_EVALUATING_IDENTIFIER:

        // Some commands return a '+' followed by numbers which can be mistaken
        // for an URC, so we check against that and disregard the data if that
        // is the case
        if (data >= '0' && data <= '9') {
            urc_parse_state = URC_NOT_PARSING;
        } else {
            urc_identifier_buffer[urc_identifier_buffer_length++] = data;
            urc_parse_state = URC_PARSING_IDENTIFIER;
        }

        break;

    case URC_PARSING_IDENTIFIER:

        if (data == URC_IDENTIFIER_END_CHARACTER) {

            // We set this as the initial condition and if we find a match
            // for the URC we go on parsing the data
            urc_parse_state = URC_NOT_PARSING;

            for (uint8_t i = 0; i < MAX_URC_CALLBACKS; i++) {

                if (urc_lookup_table_length[i] ==
                    urc_identifier_buffer_length) {

                    if (memcmp((const void *)urc_identifier_buffer,
                               (const void *)urc_lookup_table[i],
                               urc_lookup_table_length[i]) == 0) {

                        urc_index = i;
                        urc_current_callback = urc_callbacks[i];
                        urc_parse_state = URC_PARSING_DATA;

                        // Clear data if requested and if the data hasn't
                        // already been read
                        if (urc_lookup_table_clear_data[urc_index] &&
                            rx_num_elements >= urc_identifier_buffer_length) {

                            rx_head_index =
                                (rx_head_index - urc_identifier_buffer_length) &
                                RX_BUFFER_MASK;

                            rx_num_elements = (rx_num_elements -
                                               urc_identifier_buffer_length) &
                                              RX_ELEMENTS_MASK;
                        }

                        // Reset the index in order to prepare the URC
                        // buffer for data
                        urc_data_buffer_length = 0;

                        break;
                    }
                }
            }

            urc_identifier_buffer_length = 0;

        } else if (urc_identifier_buffer_length == URC_IDENTIFIER_BUFFER_SIZE) {
            urc_parse_state = URC_NOT_PARSING;
        } else {
            urc_identifier_buffer[urc_identifier_buffer_length++] = data;
        }

        break;

    case URC_PARSING_DATA:

        if (data == CARRIAGE_RETURN) {

            // Add termination since we're done
            urc_data_buffer[urc_data_buffer_length] = 0;

            // Clear the buffer for the URC if requested and if it already
            // hasn't been read
            if (urc_lookup_table_clear_data[urc_index] &&
                rx_num_elements >= urc_data_buffer_length) {

                rx_head_index =
                    (rx_head_index - urc_data_buffer_length) & RX_BUFFER_MASK;
                rx_num_elements = (rx_num_elements - urc_data_buffer_length) &
                                  RX_ELEMENTS_MASK;
            }

            if (urc_current_callback != NULL) {
                urc_current_callback((char *)urc_data_buffer);
                urc_current_callback = NULL;
            }

            urc_parse_state = URC_NOT_PARSING;
            urc_data_buffer_length = 0;

        } else if (urc_data_buffer_length == URC_DATA_BUFFER_SIZE) {
            // This is just a failsafe
            urc_parse_state = URC_NOT_PARSING;
        } else {
            urc_data_buffer[urc_data_buffer_length++] = data;
        }
        break;

    default:
        break;
    }

    flowControlUpdate();
}

/**
 * @brief Data register empty. Allows us to keep track of when the data has
 * been transmitted on the line and set up new data to be transmitted from
 * the ring buffer.
 */
ISR(USART1_DRE_vect) {
    if (tx_num_elements != 0) {
        // We do an logical AND here as a means of allowing the index to
        // wrap around since we have a circular buffer
        tx_tail_index = (tx_tail_index + 1) & TX_BUFFER_MASK;

        // Fill the transmit buffer since our ring buffer isn't empty
        // yet
        HWSERIALAT.TXDATAL = tx_buffer[tx_tail_index];
        tx_num_elements--;
    } else {
        // Disable TX interrupt until we want to send more data
        HWSERIALAT.CTRLA &= ~USART_DREIE_bm;
    }
}

void SequansControllerClass::begin(void) {

    // PIN SETUP
    pinConfigure(TX_PIN, PIN_DIR_OUTPUT | PIN_PULLUP_ON);

    // Request to send (RTS) and clear to send (CTS) are the control lines
    // on the UART line. From the configuration the MCU and the LTE modem is
    // in, we control the RTS line from the MCU to signalize if we can
    // process more data or not from the LTE modem. The CTS line is
    // controlled from the LTE modem and gives us the ability to know
    // whether the LTE modem can receive more data or if we have to wait.
    //
    // Both pins are active low.

    pinConfigure(RTS_PIN, PIN_DIR_OUTPUT);
    digitalWrite(RTS_PIN, HIGH);

    // Clear to send is input and we want interrupts on both edges to know
    // when the LTE modem has changed the state of the line.
    pinConfigure(CTS_PIN, PIN_DIR_INPUT | PIN_PULLUP_ON | PIN_INT_CHANGE);

    // We use attach interrupt here instead of the ISR directly as other
    // libraries might use the same ISR and we don't want to override it to
    // create a linker issue
    attachInterrupt(CTS_PIN, CTSInterrupt, CHANGE);

    // Set reset low to reset the LTE modem
    pinConfigure(RESET_PIN, PIN_DIR_OUTPUT);
    digitalWrite(RESET_PIN, HIGH);
    delay(10);
    digitalWrite(RESET_PIN, LOW);

    // SERIAL INTERFACE SETUP

    // LTE modules has set baud rate of 115200 for its UART0 interface
    USART1.BAUD = (uint16_t)(
        ((float)F_CPU * 64 / (16 * (float)SEQUANS_MODULE_BAUD_RATE)) + 0.5);

    // Interrupt on receive completed
    USART1.CTRLA = USART_RXCIE_bm;

    USART1.CTRLB = USART_RXEN_bm | USART_TXEN_bm;

    // LTE module interface requires 8 data bits with one stop bit
    USART1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_SBMODE_1BIT_gc |
                   USART_CHSIZE_8BIT_gc;

    flowControlUpdate();

    initialized = true;
}

bool SequansControllerClass::isInitialized(void) { return initialized; }

void SequansControllerClass::end(void) {
    USART1.CTRLA = 0;
    USART1.CTRLB = 0;
    USART1.CTRLC = 0;

    pinConfigure(CTS_PIN, PIN_ISC_DISABLE);
    digitalWrite(CTS_PIN, HIGH);

    // Set RTS high to halt the modem
    pinConfigure(RTS_PIN, PIN_DIR_OUTPUT);
    digitalWrite(RTS_PIN, HIGH);

    detachInterrupt(RING_PIN);
    detachInterrupt(CTS_PIN);

    initialized = false;
}

void SequansControllerClass::setRetryConfiguration(const uint8_t num_retries,
                                                   const double sleep_ms) {

    number_of_retries = num_retries;
    sleep_between_retries_ms = sleep_ms;
}

bool SequansControllerClass::isTxReady(void) {
    return tx_num_elements != TX_BUFFER_SIZE;
}

bool SequansControllerClass::isRxReady(void) { return rx_num_elements > 0; }

bool SequansControllerClass::writeByte(const uint8_t data) {

    uint8_t retry_count = 0;
    while (!isTxReady()) {
        retry_count++;

        if (retry_count == number_of_retries) {
            return false;
        }

        _delay_ms(sleep_between_retries_ms);
    }

    cli();
    tx_head_index = (tx_head_index + 1) & TX_BUFFER_MASK;
    tx_buffer[tx_head_index] = data;
    tx_num_elements++;
    sei();

    // Enable TX interrupt if CTS (active low) is asserted
    // (i.e. device is ready for data)
    if (sequansModuleIsReadyForData()) {
        USART1.CTRLA |= (1 << USART_DREIE_bp);
    }

    return true;
}

bool SequansControllerClass::writeCommand(const char *command) {

    Log.debugf("Sending AT command: %s\r\n", command);
    return writeBytes((uint8_t *)command, strlen(command));
}

bool SequansControllerClass::retryCommand(const char *command,
                                          char *out_buffer,
                                          const size_t size,
                                          uint8_t retries) {
    uint8_t retry_count = 0;
    ResponseResult response;

    do {
        writeCommand(command);

        // Allow some time for the response
        _delay_ms(100);

        if (out_buffer != NULL && size != 0) {
            response = SequansController.readResponse(out_buffer, size);
        } else {
            response = SequansController.readResponse();
        }
    } while (response != ResponseResult::OK && retry_count++ < retries);

    char response_string[18];
    responseResultToString(response, response_string);

    Log.debugf("Command response: %s\r\n", response_string);

    return retry_count < retries;
}

bool SequansControllerClass::writeBytes(const uint8_t *data,
                                        const size_t buffer_size) {

    for (size_t i = 0; i < buffer_size; i++) {
        if (!writeByte(data[i])) {
            return false;
        }
    }

    return writeByte('\r');
}

int16_t SequansControllerClass::readByte(void) {
    if (!isRxReady()) {
        return -1;
    }

    // Disable interrupts temporarily here to prevent being interleaved
    // in the middle of updating the tail index
    cli();
    const uint16_t next_tail_index = (rx_tail_index + 1) & RX_BUFFER_MASK;
    rx_tail_index = next_tail_index;
    rx_num_elements--;
    sei();

    flowControlUpdate();

    return rx_buffer[next_tail_index];
}

ResponseResult SequansControllerClass::readResponse(char *out_buffer,
                                                    const size_t buffer_size) {

    memset(out_buffer, '\0', buffer_size);

    uint8_t retry_count = 0;

    for (size_t i = 0; i < buffer_size; i++) {
        if (!isRxReady()) {
            retry_count++;
            _delay_ms(sleep_between_retries_ms);

            i--;

            if (retry_count == number_of_retries) {
                return ResponseResult::TIMEOUT;
            }

            continue;
        }

        // Reset since we get a valid value
        retry_count = 0;
        out_buffer[i] = (uint8_t)readByte();

        // We won't check for the buffer having a termination until at least
        // 2 bytes are in it
        if (i == 0) {
            continue;
        }

        // For AT command responses from the LTE module, "\r\nOK\r\n" or
        // "\r\nERROR\r\n" signifies the end of a response, so we look
        // "\r\n".
        if (out_buffer[i - 1] == CARRIAGE_RETURN &&
            out_buffer[i] == LINE_FEED) {

            char *ok_index = strstr(out_buffer, OK_TERMINATION);

            if (ok_index != NULL) {
                // Terminate and omit the rest from the OK index.
                memset(ok_index, '\0', 1);
                return ResponseResult::OK;
            }

            char *error_index = strstr(out_buffer, ERROR_TERMINATION);

            if (error_index != NULL) {
                // Terminate and omit the rest from the ERROR index
                memset(error_index, 0, 1);
                return ResponseResult::ERROR;
            }
        }
    }

    // Didn't find the end marker within the number of bytes given for the
    // response. Caller should increase the buffer size.
    return ResponseResult::BUFFER_OVERFLOW;
}

ResponseResult SequansControllerClass::readResponse(void) {

    // Just an arbitrary value, enough to hold the default terminations
    char termination_buffer[16];
    uint8_t retry_count = 0;
    ResponseResult response = ResponseResult::NONE;

    do {
        response = readResponse(termination_buffer, sizeof(termination_buffer));

        // Keep looping until response is OK or ERROR or no retries left
    } while (response == ResponseResult::TIMEOUT &&
             retry_count++ < number_of_retries);

    return response;
}

void SequansControllerClass::clearReceiveBuffer(void) {
    cli();
    rx_num_elements = 0;
    rx_tail_index = rx_head_index;
    sei();

    flowControlUpdate();
}

bool SequansControllerClass::extractValueFromCommandResponse(
    char *response,
    const uint8_t index,
    char *buffer,
    const size_t buffer_size,
    const char start_character) {

    // We need a copy in order to not modify the original
    size_t rcp_size = strlen(response) + 1;
    char response_copy[rcp_size];
    strncpy(response_copy, response, rcp_size);

    // Enforce non buffer overflow
    response_copy[rcp_size - 1] = '\0';

    char *data;

    if (start_character != 0) {

        // Find the first occurrence of the data start character and move
        // pointer to there
        data = strchr(response_copy, start_character);

        if (data == NULL) {
            return false;
        }

        // Increment pointer to skip the data start character (and the
        // following space in the start sequence of the data if it is there)
        while (*data == start_character || *data == SPACE_CHARACTER) { data++; }
    } else {
        // If no start character is given, just set data start to string
        // start
        data = response_copy;
    }

    // Now we split the string by the response delimiter and search for the
    // index we're interested in
    //
    // We refrain from using strtok to split the string here as it will not
    // take into account empty strings between the delimiter, which can be
    // the case with certain command responses

    // These keep track of the contens between the delimiter
    char *start_value_ptr = data;
    char *end_value_ptr = strchr(data, RESPONSE_DELIMITER);

    // We did not find the delimiter at all, abort if the index we request
    // is > 0. If it is 0, the command might only consist of one entry and
    // not have a delimiter
    if (end_value_ptr == NULL && index > 0) {
        return false;
    }

    uint8_t value_index = 1;

    while (end_value_ptr != NULL && value_index <= index) {
        // Find next occurrence and update accordingly
        start_value_ptr = end_value_ptr + 1;
        end_value_ptr = strchr(start_value_ptr, RESPONSE_DELIMITER);
        value_index++;
    }

    // If we got all the way to the end, set the end_value_ptr to the end of
    // the data ptr
    if (end_value_ptr == NULL) {
        end_value_ptr = data + strlen(data);
    }
    end_value_ptr[0] = 0; // Add null termination

    // If found, set termination to the carriage return. If not, leave the
    // string be as it is
    char *first_carriage_return = strchr(start_value_ptr, '\r');
    if (first_carriage_return != NULL) {
        *first_carriage_return = 0;
    }

    size_t value_length = strlen(start_value_ptr);
    // We compare inclusive for value length as we want to take the null
    // termination into consideration. So the buffer size has be
    // value_length + 1
    if (value_length >= buffer_size) {
        Log.error("Buffer was too small for value when extracting value for "
                  "command response, increase the buffer size");
        return false;
    }

    strcpy(buffer, start_value_ptr);

    return true;
}

bool SequansControllerClass::registerCallback(const char *urc_identifier,
                                              void (*urc_callback)(char *),
                                              const bool clear_data) {

    // Check if we can override first
    uint8_t urc_identifier_length = strlen(urc_identifier);
    for (uint8_t i = 0; i < MAX_URC_CALLBACKS; i++) {
        if (urc_lookup_table_length[i] == urc_identifier_length &&
            strcmp(urc_identifier, (const char *)urc_lookup_table[i]) == 0) {
            urc_callbacks[i] = urc_callback;
            urc_lookup_table_clear_data[i] = clear_data;
            return true;
        }
    }

    // Look for empty spot
    for (uint8_t i = 0; i < MAX_URC_CALLBACKS; i++) {
        if (urc_lookup_table_length[i] == 0) {
            strcpy((char *)urc_lookup_table[i], urc_identifier);
            urc_lookup_table_length[i] = strlen(urc_identifier);
            urc_callbacks[i] = urc_callback;
            urc_lookup_table_clear_data[i] = clear_data;
            return true;
        }
    }

    Log.error("Max amount of URC callbacks for Sequans controller reached");

    return false;
}

void SequansControllerClass::unregisterCallback(const char *urc_identifier) {
    const uint8_t urc_identifier_length = strlen(urc_identifier);
    for (uint8_t i = 0; i < MAX_URC_CALLBACKS; i++) {
        if (memcmp((const void *)urc_identifier,
                   (const void *)urc_lookup_table[i],
                   urc_identifier_length) == 0) {
            // No need to fill the look up table identifier table, as we
            // override it if a new registration is issued, but the length
            // is used to check if the slot is active or not, so we set that
            // to 0 and reset the callback pointer for house keeping
            urc_lookup_table_length[i] = 0;
            urc_callbacks[i] = NULL;
            break;
        }
    }
}

void SequansControllerClass::setPowerSaveMode(const uint8_t mode,
                                              void (*ring_callback)(void)) {

    Log.debugf("Setting power save mode %d for Sequans Controller\r\n", mode);

    if (mode == 0) {
        ring_line_callback = NULL;
        power_save_mode = 0;

        // Clear interrupt
        pinConfigure(RING_PIN, PIN_DIR_INPUT);
        detachInterrupt(RING_PIN);

        RTS_PORT.OUTCLR |= RTS_PIN_bm;
    } else if (mode == 1) {

        if (ring_callback != NULL) {
            ring_line_callback = ring_callback;

            // We have interrupt on change here since there is sometimes
            // a too small interval for the sensing to sense a rising edge.
            // This is fine as any change will yield that we are out of
            // power save mode.
            pinConfigure(RING_PIN, PIN_DIR_INPUT | PIN_INT_CHANGE);
            attachInterrupt(RING_PIN, RingInterrupt, CHANGE);
        }

        power_save_mode = 1;
        RTS_PORT.OUTSET |= RTS_PIN_bm;
    }
}

void SequansControllerClass::responseResultToString(
    const ResponseResult response_result,
    char *response_string) {

    switch (response_result) {
    case ResponseResult::OK:
        strcpy(response_string, "OK");
        break;
    case ResponseResult::ERROR:
        strcpy(response_string, "ERROR");
        break;
    case ResponseResult::BUFFER_OVERFLOW:
        strcpy(response_string, "BUFFER_OVERFLOW");
        break;
    case ResponseResult::TIMEOUT:
        strcpy(response_string, "TIMEOUT");
        break;
    case ResponseResult::SERIAL_READ_ERROR:
        strcpy(response_string, "SERIAL_READ_ERROR");
        break;
    case ResponseResult::NONE:
        strcpy(response_string, "NONE");
        break;
    }
}

uint8_t SequansControllerClass::waitForByte(const uint8_t byte,
                                            const uint32_t timeout) {
    int16_t read_byte = SequansController.readByte();
    uint32_t start = millis();

    while (read_byte != byte) {
        read_byte = SequansController.readByte();

        if (millis() - start > timeout) {
            return SEQUANS_CONTROLLER_READ_BYTE_TIMEOUT;
        }
    }

    return SEQUANS_CONTROLLER_READ_BYTE_OK;
}

void SequansControllerClass::startCriticalSection(void) {
    critical_section_enabled = true;
    RTS_PORT.OUTSET = RTS_PIN_bm;
}

void SequansControllerClass::stopCriticalSection(void) {
    critical_section_enabled = false;
    RTS_PORT.OUTCLR = RTS_PIN_bm;
}
