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
	static unsigned char enrichValue (unsigned int inputValue, unsigned int analogResolution, bool isReverse, signed char trim, unsigned char deadZone, float expo, unsigned char calMin, unsigned char calMax);

	// applies reverse
	static unsigned char applyReverse(unsigned char value, bool isReverse);

	// applies the axis calibration
	static unsigned char applyCalibration(unsigned char value, unsigned char calMin, unsigned char calMax);

	// applies deadzone
	static unsigned char applyDeadZone (unsigned char inputValue, unsigned char deadZone);

	// applies trim
	static unsigned char applyTrim(unsigned char inputValue, signed char trim);



	// applies expo
	static unsigned char applyExpo(unsigned char inputValue, float expo);

};

#endif /* RCSTICKAXISFUNCTIONS_H_ */
