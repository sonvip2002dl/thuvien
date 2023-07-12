/**
 * Copyright (c) 2011-2021 Bill Greiman
 * This file is part of the SdFat library for SD memory cards.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
/**
 * \file
 * \brief SdSpiCard class for V2 SD/SDHC cards
 */
#ifndef SdSpiCard_h
#define SdSpiCard_h
#include <stddef.h>
#include "../common/SysCall.h"
#include "SdCardInfo.h"
#include "SdCardInterface.h"
#include "../SpiDriver/SdSpiDriver.h"

#ifdef HOST_MOCK
extern uint64_t _sdCardSizeB;
extern uint8_t *_sdCard;
#endif

//==============================================================================
/**
 * \class SharedSpiCard
 * \brief Raw access to SD and SDHC flash memory cards via shared SPI port.
 */
#if HAS_SDIO_CLASS
class SharedSpiCard : public SdCardInterface {
#elif USE_BLOCK_DEVICE_INTERFACE
class SharedSpiCard : public BlockDeviceInterface {
#else  // HAS_SDIO_CLASS
class SharedSpiCard {
#endif  // HAS_SDIO_CLASS
 public:
  /** SD is in idle state */
  static const uint8_t IDLE_STATE = 0;
  /** SD is in multi-sector read state. */
  static const uint8_t READ_STATE = 1;
  /** SD is in multi-sector write state. */
  static const uint8_t WRITE_STATE = 2;
  /** Construct an instance of SharedSpiCard. */
  SharedSpiCard() {}
  /** Initialize the SD card.
   * \param[in] spiConfig SPI card configuration.
   * \return true for success or false for failure.
   */
  bool begin(SdSpiConfig spiConfig);
  /** Erase a range of sectors.
   *
   * \param[in] firstSector The address of the first sector in the range.
   * \param[in] lastSector The address of the last sector in the range.
   *
   * \note This function requests the SD card to do a flash erase for a
   * range of sectors.  The data on the card after an erase operation is
   * either 0 or 1, depends on the card vendor.  The card must support
   * single sector erase.
   *
   * \return true for success or false for failure.
   */
  bool erase(uint32_t firstSector, uint32_t lastSector);
  /** Determine if card supports single sector erase.
   *
   * \return true is returned if single sector erase is supported.
   * false is returned if single sector erase is not supported.
   */
  bool eraseSingleSectorEnable();
  /**
   *  Set SD error code.
   *  \param[in] code value for error code.
   */
  void error(uint8_t code) {
//    (void)code;
    m_errorCode = code;
  }
  /**
   * \return code for the last error. See SdCardInfo.h for a list of error codes.
   */
  uint8_t errorCode() const {
    return m_errorCode;
  }
  /** \return error data for last error. */
  uint32_t errorData() const {
    return m_status;
  }
  /**
   * Check for busy.  MISO low indicates the card is busy.
   *
   * \return true if busy else false.
   */
  bool isBusy();
  /**
   * Read a card's CID register. The CID contains card identification
   * information such as Manufacturer ID, Product name, Product serial
   * number and Manufacturing date.
   *
   * \param[out] cid pointer to area for returned data.
   *
   * \return true for success or false for failure.
   */
  bool readCID(cid_t* cid) {
    return readRegister(CMD10, cid);
  }
  /**
   * Read a card's CSD register. The CSD contains Card-Specific Data that
   * provides information regarding access to the card's contents.
   *
   * \param[out] csd pointer to area for returned data.
   *
   * \return true for success or false for failure.
   */
  bool readCSD(csd_t* csd) {
    return readRegister(CMD9, csd);
  }
  /** Read one data sector in a multiple sector read sequence
   *
   * \param[out] dst Pointer to the location for the data to be read.
   *
   * \return true for success or false for failure.
   */
  bool readData(uint8_t* dst);
  /** Read OCR register.
   *
   * \param[out] ocr Value of OCR register.
   * \return true for success or false for failure.
   */
  bool readOCR(uint32_t* ocr);
  /**
   * Read a 512 byte sector from an SD card.
   *
   * \param[in] sector Logical sector to be read.
   * \param[out] dst Pointer to the location that will receive the data.
   * \return true for success or false for failure.
   */
  bool readSector(uint32_t sector, uint8_t* dst);
  /**
   * Read multiple 512 byte sectors from an SD card.
   *
   * \param[in] sector Logical sector to be read.
   * \param[in] ns Number of sectors to be read.
   * \param[out] dst Pointer to the location that will receive the data.
   * \return true for success or false for failure.
   */
  bool readSectors(uint32_t sector, uint8_t* dst, size_t ns);

  /** Start a read multiple sector sequence.
   *
   * \param[in] sector Address of first sector in sequence.
   *
   * \note This function is used with readData() and readStop() for optimized
   * multiple sector reads.  SPI chipSelect must be low for the entire sequence.
   *
   * \return true for success or false for failure.
   */
  bool readStart(uint32_t sector);
  /** Return the 64 byte card status
   * \param[out] status location for 64 status bytes.
   * \return true for success or false for failure.
   */
  bool readStatus(uint8_t* status);
  /** End a read multiple sectors sequence.
   *
   * \return true for success or false for failure.
   */
  bool readStop();
  /** \return SD multi-sector read/write state */
  uint8_t sdState() {return m_state;}
  /**
   * Determine the size of an SD flash memory card.
   *
   * \return The number of 512 byte data sectors in the card
   *         or zero if an error occurs.
   */
  uint32_t sectorCount();
#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // Use sectorCount(). cardSize() will be removed in the future.
  uint32_t __attribute__((error("use sectorCount()"))) cardSize();
#endif  // DOXYGEN_SHOULD_SKIP_THIS
  /** end a mult-sector transfer.
   *
   * \return true for success or false for failure.
   */
  bool stopTransfer();
  /** \return success if sync successful. Not for user apps. */
  bool syncDevice();
  /** Return the card type: SD V1, SD V2 or SDHC/SDXC
   * \return 0 - SD V1, 1 - SD V2, or 3 - SDHC/SDXC.
   */
  uint8_t type() const {
    return m_type;
  }
  /**
   * Write a 512 byte sector to an SD card.
   *
   * \param[in] sector Logical sector to be written.
   * \param[in] src Pointer to the location of the data to be written.
   * \return true for success or false for failure.
   */
  bool writeSector(uint32_t sector, const uint8_t* src);
  /**
   * Write multiple 512 byte sectors to an SD card.
   *
   * \param[in] sector Logical sector to be written.
   * \param[in] ns Number of sectors to be written.
   * \param[in] src Pointer to the location of the data to be written.
   * \return true for success or false for failure.
   */
  bool writeSectors(uint32_t sector, const uint8_t* src, size_t ns);
  /** Write one data sector in a multiple sector write sequence.
   * \param[in] src Pointer to the location of the data to be written.
   * \return true for success or false for failure.
   */
  bool writeData(const uint8_t* src);
  /** Start a write multiple sectors sequence.
   *
   * \param[in] sector Address of first sector in sequence.
   *
   * \note This function is used with writeData() and writeStop()
   * for optimized multiple sector writes.
   *
   * \return true for success or false for failure.
   */
  bool writeStart(uint32_t sector);

  /** End a write multiple sectors sequence.
   *
   * \return true for success or false for failure.
   */
  bool writeStop();

 private:
  // private functions
  uint8_t cardAcmd(uint8_t cmd, uint32_t arg) {
    cardCommand(CMD55, 0);
    return cardCommand(cmd, arg);
  }
  uint8_t cardCommand(uint8_t cmd, uint32_t arg);
  bool readData(uint8_t* dst, size_t count);
  bool readRegister(uint8_t cmd, void* buf);
  void spiSelect() {
    sdCsWrite(m_csPin, false);
  }
  void spiStart();
  void spiStop();
  void spiUnselect() {
    sdCsWrite(m_csPin, true);
  }
  void type(uint8_t value) {
    m_type = value;
  }
  bool waitReady(uint16_t ms);
  bool writeData(uint8_t token, const uint8_t* src);

#if SPI_DRIVER_SELECT < 2
  void spiActivate() {
    m_spiDriver.activate();
  }
  void spiBegin(SdSpiConfig spiConfig) {
    m_spiDriver.begin(spiConfig);
  }
  void spiDeactivate() {
    m_spiDriver.deactivate();
  }
  uint8_t spiReceive() {
    return m_spiDriver.receive();
  }
  uint8_t spiReceive(uint8_t* buf, size_t n) {
    return m_spiDriver.receive(buf, n);
  }
  void spiSend(uint8_t data) {
    m_spiDriver.send(data);
  }
  void spiSend(const uint8_t* buf, size_t n) {
    m_spiDriver.send(buf, n);
  }
  void spiSetSckSpeed(uint32_t maxSck) {
    m_spiDriver.setSckSpeed(maxSck);
  }
  SdSpiDriver m_spiDriver;
#else  // SPI_DRIVER_SELECT < 2
  void spiActivate() {
    m_spiDriverPtr->activate();
  }
  void spiBegin(SdSpiConfig spiConfig) {
    m_spiDriverPtr->begin(spiConfig);
  }
  void spiDeactivate() {
    m_spiDriverPtr->deactivate();
  }
  uint8_t spiReceive() {
    return m_spiDriverPtr->receive();
  }
  uint8_t spiReceive(uint8_t* buf, size_t n) {
    return m_spiDriverPtr->receive(buf, n);
  }
  void spiSend(uint8_t data) {
    m_spiDriverPtr->send(data);
  }
  void spiSend(const uint8_t* buf, size_t n) {
    m_spiDriverPtr->send(buf, n);
  }
  void spiSetSckSpeed(uint32_t maxSck) {
    m_spiDriverPtr->setSckSpeed(maxSck);
  }
  SdSpiDriver* m_spiDriverPtr;
#endif  // SPI_DRIVER_SELECT < 2

  SdCsPin_t m_csPin;
  uint8_t m_errorCode = SD_CARD_ERROR_INIT_NOT_CALLED;
  bool    m_spiActive;
  uint8_t m_state;
  uint8_t m_status;
  uint8_t m_type = 0;
};

//==============================================================================
/**
 * \class DedicatedSpiCard
 * \brief Raw access to SD and SDHC flash memory cards via dedicate SPI port.
 */
class DedicatedSpiCard : public SharedSpiCard {
#ifndef HOST_MOCK
 public:
  /** Construct an instance of DedicatedSpiCard. */
  DedicatedSpiCard() {}
  /** Initialize the SD card.
   * \param[in] spiConfig SPI card configuration.
   * \return true for success or false for failure.
   */
  bool begin(SdSpiConfig spiConfig);
  /**
   * Read a 512 byte sector from an SD card.
   *
   * \param[in] sector Logical sector to be read.
   * \param[out] dst Pointer to the location that will receive the data.
   * \return true for success or false for failure.
   */
  bool readSector(uint32_t sector, uint8_t* dst);
  /**
   * Read multiple 512 byte sectors from an SD card.
   *
   * \param[in] sector Logical sector to be read.
   * \param[in] ns Number of sectors to be read.
   * \param[out] dst Pointer to the location that will receive the data.
   * \return true for success or false for failure.
   */
  bool readSectors(uint32_t sector, uint8_t* dst, size_t ns);
  /**
   * Write a 512 byte sector to an SD card.
   *
   * \param[in] sector Logical sector to be written.
   * \param[in] src Pointer to the location of the data to be written.
   * \return true for success or false for failure.
   */
  bool writeSector(uint32_t sector, const uint8_t* src);
  /**
   * Write multiple 512 byte sectors to an SD card.
   *
   * \param[in] sector Logical sector to be written.
   * \param[in] ns Number of sectors to be written.
   * \param[in] src Pointer to the location of the data to be written.
   * \return true for success or false for failure.
   */
  bool writeSectors(uint32_t sector, const uint8_t* src, size_t ns);

 private:
  uint32_t m_curSector;
  bool m_sharedSpi = true;
#else // HOST_MOCK
 public:
  DedicatedSpiCard() : m_errorCode(SD_CARD_ERROR_INIT_NOT_CALLED), m_type(0) {
  }
  ~DedicatedSpiCard() { }
  bool begin(SdSpiConfig spiConfig) {
    m_errorCode = 0;
    m_status = 0;
    (void)spiConfig;
    return true;
  }
  uint32_t cardSize() { return _sdCardSizeB / 512LL; }
  bool erase(uint32_t firstBlock, uint32_t lastBlock) {
    memset(_sdCard + firstBlock * 512, 0, (lastBlock - firstBlock) * 512);
    return true;
  }
  bool eraseSingleBlockEnable() { return true; }
  void error(uint8_t code) { m_errorCode = code; }
  int errorCode() const { return m_errorCode; }
  int errorData() const { return m_status; }
  bool isBusy() { return false; }
  bool readSector(uint32_t sector, uint8_t* dst) {
    return readSectors(sector, dst, 1);
  }
  bool readSectors(uint32_t sector, uint8_t* dst, size_t ns) {
    if ((int)(sector + ns) > (int) (_sdCardSizeB / 512LL)) return false;
    memcpy(dst, _sdCard + sector * 512, 512 * ns);
    return true;
  }

  bool readCID(cid_t* cid) { return true; }
  bool readCSD(csd_t* csd) { return true; }
  bool readOCR(uint32_t* ocr) { return true; }
  bool readStatus(uint8_t* status) { return true; }
  bool readStop() { return true; }
  bool syncBlocks() { return true; }
  int type() const { return m_type; }
  bool writeSector(uint32_t sector, const uint8_t* src) {
    return writeSectors(sector, src, 1);
  }
  bool writeSectors(uint32_t sector, const uint8_t* src, size_t ns) {
    if ((int)(sector + ns) > (int) (_sdCardSizeB / 512LL)) return false;
    memcpy(_sdCard + sector * 512, src, 512 * ns);
    return true;
  }

  uint32_t sectorCount() { return _sdCardSizeB / 512LL; }
  bool syncDevice() { return true; }

  bool writeStop() { return true; }
  void spiStart() { }
  void spiStop() { }

private:
  int m_errorCode;
  int m_status;
  int m_type;
#endif
};
//==============================================================================
#if ENABLE_DEDICATED_SPI
/** typedef for dedicated SPI. */
typedef DedicatedSpiCard SdSpiCard;
#else
/** typedef for shared SPI. */
typedef SharedSpiCard SdSpiCard;
#endif
#endif  // SdSpiCard_h
