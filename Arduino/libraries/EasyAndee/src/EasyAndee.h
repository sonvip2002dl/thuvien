/* EasyAndee.h - Annikken Andee Library for Arduino
Annikken Pte Ltd
Author: Muhammad Hasif

The Easy Andee Library is to allow users to use a feature similar to Arduino's Firmata to control their Arduino using their Bluetooth enabled phone. Users no longer need to code the UI and logic. With the Easy Andee feature on the Andee App, users can create their UI and immediately control their electronics.

This library is free software*/

#if defined (__arm__)
#include "itoa.h"
#endif

#define SET_EZANDEE_NAME 119
#define EASYANDEE_D_IN 'E'
#define EASYANDEE_D_OUT 'D'
#define EASYANDEE_A_IN 'B'
#define EASYANDEE_A_OUT 'A'
#define EASYANDEE_D_IN_TO_PHONE 'F'
#define EASYANDEE_A_IN_TO_PHONE 'C'

#define EASYANDEE_REPLY_D 120
#define EASYANDEE_REPLY_A 121


void EasyAndeeBegin();
void EasyAndeeBegin(int);
void setName(const char*);
void EasyAndeePoll();
void processDIn();
void processAIn();
void processAOut();


void resetRX();
void spiSendData(char*, size_t);
bool pollRx(char*);
void sendAndee(unsigned char,char*);