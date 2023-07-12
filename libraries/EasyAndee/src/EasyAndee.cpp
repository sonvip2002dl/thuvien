/* EasyAndee.h - Annikken Andee Library for Arduino
Annikken Pte Ltd
Author: Muhammad Hasif

The Easy Andee Library is to allow users to use a feature similar to Arduino's Firmata to control their Arduino using their Bluetooth enabled phone. Users no longer need to code the UI and logic. With the Easy Andee feature on the Andee App, users can create their UI and immediately control their electronics.

This library is free software*/

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <SPI.h>
#include <EasyAndee.h>

#define RX_DELAY 200
#define RX_MAX 20
#define TX_MAX 20
 
unsigned int SS_PIN = 8;

char txBuffer[TX_MAX];
char rxBuffer[RX_MAX];

/****************************************************************************
 * EasyAndee Functions
 *****************************************************************************/

void EasyAndeeBegin()
{
	pinMode(SS_PIN,OUTPUT);
	digitalWrite(SS_PIN,HIGH);
	#if defined(__SAM3X8E__)
		SPI.begin(SS_PIN);
		SPI.setClockDivider(SS_PIN, 21);
	#else		
		SPI.begin();	
	#endif 	
	
	delay(800);
}
void EasyAndeeBegin(int pin)
{
	SS_PIN = pin;
	EasyAndeeBegin();
}
void setName(const char* name)
{
	char limit[33];
	if(strlen(name) > 32)
	{
		memcpy(limit,name,32);
		limit[32] = '\0';
		sendAndee(SET_EZANDEE_NAME,(char*)limit);//
	}
	else
	{
		sendAndee(SET_EZANDEE_NAME,(char*)name);//
	}
}


void processDOut()
{	
	if(rxBuffer[2] >= 111 )//'o'
	{
		switch(rxBuffer[2])
		{
			case 'o':
				pinMode(A0,OUTPUT);
				digitalWrite(A0,rxBuffer[3]-48);
			break;
			
			case 'p':
				pinMode(A1,OUTPUT);
				digitalWrite(A1,rxBuffer[3]-48);
			break;
			
			case 'q':
				pinMode(A2,OUTPUT);
				digitalWrite(A2,rxBuffer[3]-48);
				break;
			
			case 'r':
				pinMode(A3,OUTPUT);
				digitalWrite(A3,rxBuffer[3]-48);
				break;
			
			case 's':
				pinMode(A4,OUTPUT);
				digitalWrite(A4,rxBuffer[3]-48);
				break;
			
			case 't':
				pinMode(A5,OUTPUT);
				digitalWrite(A5,rxBuffer[3]-48);
				break;			
		}
	}
	else
	{
		pinMode(rxBuffer[2] - 97,OUTPUT);
		digitalWrite(rxBuffer[2] - 97,rxBuffer[3]-48);
	}	
}

void processAOut()
{
	char analogBuffer[4];
	analogBuffer[0] = rxBuffer[3];
	analogBuffer[1] = rxBuffer[4];
	analogBuffer[2] = rxBuffer[5];
	analogBuffer[3] = '\0';

	int analogValue = strtol(analogBuffer,NULL,10);	 
	if( rxBuffer[2] == 'd' ||
		rxBuffer[2] == 'f' ||
		rxBuffer[2] == 'g' ||
		rxBuffer[2] == 'j' )
		{
			pinMode(rxBuffer[2] - 97,OUTPUT);
			analogWrite(rxBuffer[2] - 97,analogValue);
		}
}

void processDIn()
{
	int value = 0;
	char temp[5];
	
	if(rxBuffer[2] >= 111 )//'o'
	{
		switch(rxBuffer[2])
		{
			case 'o':
				pinMode(A0,INPUT);
				value = digitalRead(A0);
			break;
			
			case 'p':
				pinMode(A1,INPUT);
				value = digitalRead(A1);
			break;
			
			case 'q':
				pinMode(A2,INPUT);
				value = digitalRead(A2);
				break;
			
			case 'r':
				pinMode(A3,INPUT);
				value = digitalRead(A3);
				break;
			
			case 's':
				pinMode(A4,INPUT);
				value = digitalRead(A4);
				break;
			
			case 't':
				pinMode(A5,INPUT);
				value = digitalRead(A5);
				break;			
		}
	}
	else
	{
		pinMode(rxBuffer[2] - 97,INPUT);
		value = digitalRead(rxBuffer[2] - 97);
	}	
	snprintf(temp,5,"%c%c%c%c", rxBuffer[0], EASYANDEE_D_IN_TO_PHONE, rxBuffer[2], value+48);
	sendAndee(EASYANDEE_REPLY_D,temp);
}

void processAIn()
{
	int value = 0;
	char temp[8];	
	
	switch(rxBuffer[2])
		{
			case 'o':
				pinMode(A0,INPUT);
				value = analogRead(A0);
			break;
			
			case 'p':
				pinMode(A1,INPUT);
				value = analogRead(A1);
			break;
			
			case 'q':
				pinMode(A2,INPUT);
				value = analogRead(A2);
				break;
			
			case 'r':
				pinMode(A3,INPUT);
				value = analogRead(A3);
				break;
			
			case 's':
				pinMode(A4,INPUT);
				value = analogRead(A4);
				break;
			
			case 't':
				pinMode(A5,INPUT);
				value = analogRead(A5);
				break;			
		}
	snprintf(temp,8,"%c%c%c%04d", rxBuffer[0], EASYANDEE_A_IN_TO_PHONE, rxBuffer[2], value);
	sendAndee(EASYANDEE_REPLY_A,temp);
}

void EasyAndeePoll()
{
	pollRx(rxBuffer);
	switch(rxBuffer[1])
	{
		case EASYANDEE_D_OUT: //done, TODO test
			processDOut();
			break;
		
		case EASYANDEE_D_IN:
			processDIn();
			break;
		
		case EASYANDEE_A_OUT: //done, TODO test
			processAOut();
			break;
		
		case EASYANDEE_A_IN:
			processAIn();
			break;		
		case 0x00:
			//Do Nothing here
			break;
		default:			
			Serial.println("Unknown Command");
			break;
	}
}

/****************************************************************************
 * Function to send and receive data to and from PIC32 using SPI
 *****************************************************************************/
void resetRX()
{
	memset(rxBuffer,0x00,RX_MAX);
}

void spiSendData(char* txBuffer, size_t bufferLength){
	unsigned int txCount = 0;
	
	SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
	
	digitalWrite(SS_PIN,LOW);
	for(txCount = 0;txCount < bufferLength;txCount++)//send whole buffer
	{		
		SPI.transfer(txBuffer[txCount]);//transfer and receive 1 char in SPI
		delayMicroseconds(40);
	}
	digitalWrite(SS_PIN,HIGH);
	
	SPI.endTransaction();
	delay(10);
}

bool pollRx(char* buffer)
{
	unsigned int rxCount = 0;
	unsigned char tempChar;	
	resetRX();
	delay(3);	
	SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
	
	digitalWrite(SS_PIN,LOW);	
	for(int i = 0;i<RX_DELAY;)
	{
		tempChar = SPI.transfer(0x00);
		if(tempChar > 32)
		{				
		if(tempChar == '}')
			{
				Serial.print("pollRx:");Serial.println(buffer);
				buffer[rxCount] = '\0';
				
				digitalWrite(SS_PIN,HIGH);
				SPI.endTransaction();
				
				delay(5);
				return true;			
				break;
			}
			else if(tempChar == 173)
			{
				Serial.println("pollRx: No Reply");
				digitalWrite(SS_PIN,HIGH);
				SPI.endTransaction();
				
				delay(5);
				return false;
				break;
			}
			else
			{
				buffer[rxCount++] = tempChar;	
			}
		}
		else
		{
			i++;
		}
	}	
	digitalWrite(SS_PIN,HIGH);
	
	SPI.endTransaction();
	delay(5);	
	return false;
} 

void sendAndee(unsigned char andeeCommand,char* message){
	memset(txBuffer,0x00,TX_MAX);
	sprintf(txBuffer,"#99#%d#%s;", andeeCommand, message);	
	
	spiSendData( txBuffer,strlen(txBuffer) );	
}
