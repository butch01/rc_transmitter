/*
 * RCStick.h
 *
 *  Created on: 29.06.2019
 *      Author: butch
 */

#ifndef RCSTICKAXIS_H_
#define RCSTICKAXIS_H_
#include "defaultValues.h"
#include <arduino.h>

/**
 * represents a single axis of an RC stick (e.g. x OR y axis).
 * It offers functions for storing and applying trim, expo, reverse, deadzone.
 */
class RCStickAxis {
public:
	RCStickAxis();
	virtual ~RCStickAxis();

	void setTrim(signed char trimValue);
	signed char getTrim();

	void setDeadZone(unsigned char deadZone);
	unsigned char getDeadZone();

	unsigned char applyDeadZoneAndTrim (unsigned char inputValue);
	unsigned char applyReverse(unsigned char value);
	void setReverse(bool isReverse);
	bool getReverse();

	void setRawInputValue(unsigned char rawInputValue);
	unsigned char getRawInputValue();

	unsigned char applyDeadZone (unsigned char inputValue);
	unsigned char applyTrim(unsigned char inputValue);

	unsigned char applyAll(unsigned char inputValue);
	unsigned char applyExpoValue(unsigned char inputValue);


	// returns the current value
	unsigned char getCalculatedValue();



	// Calibration
	void setCalibrationMinValue(unsigned char axisCalibrationMinValue);
	void setCalibrationMaxValue(unsigned char axisCalibrationMaxValue);
	unsigned char applyCalibration(unsigned char inputValue);

private:


	signed char myTrim;
	unsigned char myDeadZone;
	bool isReverse;
	float myExpo;

	unsigned char axisCalibrationMinValue;
	unsigned char axisCalibrationMaxValue;

};

#endif /* RCSTICKAXIS_H_ */
