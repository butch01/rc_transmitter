/*
 * RCStickAxisFunctions.h
 *
 *  Created on: 07.05.2020
 *      Author: butch
 */

#ifndef RCSTICKAXISFUNCTIONS_H_
#define RCSTICKAXISFUNCTIONS_H_

#include <arduino.h>

class RCStickAxisFunctions {
public:
	RCStickAxisFunctions();
	virtual ~RCStickAxisFunctions();

	// add all supported enrichments (reverse, trim, expo, fload, calibration). returns unsigned char
	static unsigned char enrichValue (uint16_t inputValueRaw, uint16_t analogResolution, bool isReverse, int8_t trim, uint8_t deadZone, float expo, uint8_t calMin, uint8_t calMax, uint8_t limitMin, uint8_t limitMax);

	// applies reverse
	static unsigned char applyReverse(unsigned char value, bool isReverse);

	// applies the axis calibration  and Limit
	static unsigned char applyCalibrationAndLimit(uint8_t value, uint8_t calMin, uint8_t calMax, uint8_t limitMin, uint8_t limitMax);

	// applies deadzone
	static unsigned char applyDeadZone (unsigned char inputValue, unsigned char deadZone);

	// applies trim
	static unsigned char applyTrim(unsigned char inputValue, signed char trim);



	// applies expo
	static unsigned char applyExpo(unsigned char inputValue, float expo);

};

#endif /* RCSTICKAXISFUNCTIONS_H_ */
