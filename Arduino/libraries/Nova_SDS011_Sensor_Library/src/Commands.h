/** 
 * @file Commands.h
 * @brief Commands and replies definitions.
 *
 * @author R. Orecki
 * 12.2019
 */

#pragma once
#include "NovaSDS011.h"

static CommandType REPORT_TYPE_CMD = {
	0xAA, // head
	0xB4, // command id
	0x02, // data byte 1
	0x01, // data byte 2 (0：query the current mode 1：set reporting mode)
	0x00, // data byte 3 (0：report active mode 1：Report query mode)
	0x00, // data byte 4
	0x00, // data byte 5
	0x00, // data byte 6
	0x00, // data byte 7
	0x00, // data byte 8
	0x00, // data byte 9
	0x00, // data byte 10
	0x00, // data byte 11
	0x00, // data byte 12
	0x00, // data byte 13
	0xFF, // data byte 14 (device id byte 1)
	0xFF, // data byte 15 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};

static ReplyType REPORT_TYPE_REPLY = {
	0xAA, // head
	0xC5, // command id
	0x02, // data byte 1
	0x01, // data byte 2 (0：query the current mode 1：set reporting mode)
	0x00, // data byte 3 (0：report active mode 1：Report query mode)
	0x00, // data byte 4
	0x00, // data byte 5 (device id byte 1)
	0x00, // data byte 6 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};

static CommandType QUERY_CMD = {
	0xAA, // head
	0xB4, // command id
	0x04, // data byte 1
	0x00, // data byte 2
	0x00, // data byte 3
	0x00, // data byte 4
	0x00, // data byte 5
	0x00, // data byte 6
	0x00, // data byte 7
	0x00, // data byte 8
	0x00, // data byte 9
	0x00, // data byte 10
	0x00, // data byte 11
	0x00, // data byte 12
	0x00, // data byte 13
	0xFF, // data byte 14 (device id byte 1)
	0xFF, // data byte 15 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};

static ReplyType QUERY_REPLY = {
	0xAA, // head
	0xC0, // command id
	0x00, // data byte 1 (PM2.5 low byte)
	0x00, // data byte 2 (PM2.5 high byte)
	0x00, // data byte 3 (PM10 low byte)
	0x00, // data byte 4 (PM10 high byte)
	0x00, // data byte 5 (device id byte 1)
	0x00, // data byte 6 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};

static CommandType SET_ID_CMD = {
	0xAA, // head
	0xB4, // command id
	0x05, // data byte 1
	0x00, // data byte 2
	0x00, // data byte 3
	0x00, // data byte 4
	0x00, // data byte 5
	0x00, // data byte 6
	0x00, // data byte 7
	0x00, // data byte 8
	0x00, // data byte 9
	0x00, // data byte 10
	0x00, // data byte 11
	0x00, // data byte 12 (new device id byte 1)
	0x00, // data byte 13 (new device id byte 2)
	0xFF, // data byte 14 (device id byte 1)
	0xFF, // data byte 15 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};

static ReplyType SET_ID_REPLY = {
	0xAA, // head
	0xC5, // command id
	0x05, // data byte 1 
	0x00, // data byte 2 
	0x00, // data byte 3 
	0x00, // data byte 4 
	0x00, // data byte 5 (new device id byte 1)
	0x00, // data byte 6 (new device id byte 2)
	0x00, // checksum
	0xAB  // tail
};


static  CommandType WORKING_MODE_CMD = {
	0xAA, // head
	0xB4, // command id
	0x06, // data byte 1
	0x01, // data byte 2 (0：query the current mode 1：set reporting mode)
	0x00, // data byte 3 (0: sleep 1: work)
	0x00, // data byte 4
	0x00, // data byte 5
	0x00, // data byte 6
	0x00, // data byte 7
	0x00, // data byte 8
	0x00, // data byte 9
	0x00, // data byte 10
	0x00, // data byte 11
	0x00, // data byte 12
	0x00, // data byte 13
	0xFF, // data byte 14 (device id byte 1)
	0xFF, // data byte 15 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};

static ReplyType WORKING_MODE_REPLY = {
	0xAA, // head
	0xC5, // command id
	0x06, // data byte 1 
	0x00, // data byte 2 (0：query the current mode 1：set reporting mode)
	0x00, // data byte 3 (0: sleep 1: work)
	0x00, // data byte 4 
	0x00, // data byte 5 (device id byte 1)
	0x00, // data byte 6 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};

static  CommandType DUTY_CYCLE_CMD = {
	0xAA, // head
	0xB4, // command id
	0x08, // data byte 1
	0x01, // data byte 2 (0：query the current mode 1：set new mode)
	0x00, // data byte 3 (0-30: Duty cycle in seconds)
	0x00, // data byte 4
	0x00, // data byte 5
	0x00, // data byte 6
	0x00, // data byte 7
	0x00, // data byte 8
	0x00, // data byte 9
	0x00, // data byte 10
	0x00, // data byte 11
	0x00, // data byte 12
	0x00, // data byte 13
	0xFF, // data byte 14 (device id byte 1)
	0xFF, // data byte 15 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};

static ReplyType DUTY_CYCLE_REPLY = {
	0xAA, // head
	0xC5, // command id
	0x08, // data byte 1 
	0x00, // data byte 2 (0：query the current mode 1：set new mode)
	0x00, // data byte 3 (0-30: Duty cycle in seconds)
	0x00, // data byte 4 
	0x00, // data byte 5 (device id byte 1)
	0x00, // data byte 6 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};

static  CommandType VERSION_CMD = {
	0xAA, // head
	0xB4, // command id
	0x07, // data byte 1
	0x01, // data byte 2
	0x00, // data byte 3
	0x00, // data byte 4
	0x00, // data byte 5
	0x00, // data byte 6
	0x00, // data byte 7
	0x00, // data byte 8
	0x00, // data byte 9
	0x00, // data byte 10
	0x00, // data byte 11
	0x00, // data byte 12
	0x00, // data byte 13
	0xFF, // data byte 14 (device id byte 1)
	0xFF, // data byte 15 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};

static ReplyType VERSION_REPLY = {
	0xAA, // head
	0xC5, // command id
	0x07, // data byte 1 
	0x00, // data byte 2 (Year)
	0x00, // data byte 3 (Month)
	0x00, // data byte 4 (Day)
	0x00, // data byte 5 (device id byte 1)
	0x00, // data byte 6 (device id byte 2)
	0x00, // checksum
	0xAB  // tail
};