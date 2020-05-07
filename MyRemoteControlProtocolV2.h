/*
 * MyRemoteControlProtocolV2.h
 *
 *  Created on: 27.06.2019
 *      Author: butch
 */

#ifndef MYREMOTECONTROLPROTOCOLV2_H_
#define MYREMOTECONTROLPROTOCOLV2_H_

#include <arduino.h>
#include <FastCRC.h>
#include <ArduinoLog.h>





// Protocol definition
#define PROT_ARRAY_LENGTH 17
#define PROT_STICK_LX 0
#define PROT_STICK_LY 1
#define PROT_STICK_RX 2
#define PROT_STICK_RY 3
#define PROT_BTN_SQUARE 4
#define PROT_BTN_CROSS 5
#define PROT_BTN_TRIANGLE 6
#define PROT_BTN_CIRCLE 7
#define PROT_BTN_L1 8
#define PROT_BTN_L2 9
#define PROT_BTN_R1 10
#define PROT_BTN_R2 11
#define PROT_UP 12
#define PROT_DONW 13
#define PROT_LEFT 14
#define PROT_RIGHT 15
#define PROT_DIGITAL_BUTTONS 16
#define PROT_DIGITAL_START 0
#define PROT_DIGITAL_SELECT 1
#define PROT_DIGITAL_STICK_BTN_L 2
#define PROT_DIGITAL_STICK_BTN_R 3


class MyRemoteControlProtocolV2 {
public:



	void setChannelValue(unsigned char value, unsigned char channel);
	void setChannelValues(unsigned char * sourceValues, unsigned char dataLength);

	unsigned char * getValueArray();

	MyRemoteControlProtocolV2();
	virtual ~MyRemoteControlProtocolV2();
	unsigned char getProtocolLength();


private:
	void updateCRC();
	unsigned char values[PROT_ARRAY_LENGTH+1];
	FastCRC8 CRC8; // CRC Object




};

#endif /* MYREMOTECONTROLPROTOCOLV2_H_ */
