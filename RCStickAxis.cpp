/*
 * RCStick.cpp
 *
 *  Created on: 29.06.2019
 *      Author: butch
 */

#include "RCStickAxis.h"
#include <ArduinoLog.h>


/**
 * Constructor
 */
RCStickAxis::RCStickAxis()
{
	// sets default values for stick private variables
	myTrim = DEFAULT_TRIM;
	myDeadZone = DEFAULT_DEAD_ZONE;
	isReverse = DEFAULT_REVERSE;
	myExpo = DEFAULT_EXPO;

}

/**
 * deconstructor
 */
RCStickAxis::~RCStickAxis()
{
	// TODO Auto-generated destructor stub
}

/**
 * sets the trim value for this stick
 */
void RCStickAxis::setTrim(signed char trimValue)
{
	myTrim = trimValue;
}

/**
 * returns the trim value for this stick
 */
signed char RCStickAxis::getTrim()
{
	return myTrim;
}


/**
 * sets the deadZone for this stick
 */
void RCStickAxis::setDeadZone(unsigned char deadZoneValue)
{
	myDeadZone = deadZoneValue;
}

/**
 * gets the deadZone for this stick
 */
unsigned char RCStickAxis::getDeadZone()
{
	return myDeadZone;
}


/**
 * applies the deadZone
 */
unsigned char RCStickAxis::applyDeadZone (unsigned char inputValue)
{
	signed int value=0;

	// apply deadZone
	if ( (inputValue > 127 - myDeadZone) && (inputValue < 127 +  myDeadZone))
	{
		value = 127;
	}
	else if (inputValue > 127)
	{
		value = map (inputValue, 127 + myDeadZone, 255, 128, 255);
	}
	else if (value < 127)
	{
		value = map (inputValue, 0, 127 - myDeadZone,0, 126);
	}
	return value;
}

/**
 * adds Trim to given Value
 */
unsigned char RCStickAxis::applyTrim(unsigned char inputValue)
{
	signed int value = (signed int) inputValue;
	// apply trim AFTER deadZone
	value = value + (signed char) myTrim;

	// cut to 0 or 255 if
	if (value < 0)
	{
		value = 0;
	}
	else if (value > 255)
	{
		value = 255;
	}
	return (unsigned char) value;
}

/**
 * applies: trim, expo
 * expects Input from 0 to 255 while 127 represents center.
 * returns same range 0 to 255 while 127 represents center.
 */
unsigned char RCStickAxis::applyDeadZoneAndTrim (unsigned char inputValue)
{
	return applyTrim(applyDeadZone(inputValue));
}

/**
 * if isReverse is set, it inverts the direction.
 * input expects range 0 to 255.
 * returns range 0 to 255.
 */
unsigned char RCStickAxis::applyReverse(unsigned char value)
{

	// apply reverse
	if (isReverse)
	{
		return map(value,0,255,255,0);
	}
	else
	{
		return value;
	}

}

/**
 * applies expo to given value
 * expects 0 to 255 as input range.
 * returns range 0 to 255.
 */
unsigned char RCStickAxis::applyExpoValue(unsigned char inputValue)
{
	float expo=1;

	// map is only working with long values, not with float. So we use range from 0 to 1000, which means 3 decimal values;
	float  inputValue1000=0;
	if (inputValue > 127)
	{
		 inputValue1000 = map(inputValue, 128, 255, 0, 1000);
	}
	else if  (inputValue < 127)
	{
		 inputValue1000 = map(inputValue, 126, 0, 0, 1000);
	}

	float withExpo = pow(inputValue1000/1000, expo)*1000;

	if (inputValue > 127)
	{
		inputValue =  map(withExpo, 0,1000,128,255);
	}
	else if (inputValue < 127)
	{
		inputValue =  map(withExpo, 0,1000,126,0);
	}

	return inputValue;

}

/**
 * apples following function in the following order:
 * applyDeadZone, applyExpo, applyTrim, applyReverse
 */
unsigned char RCStickAxis::applyAll(unsigned char inputValue)
{
	return applyReverse(applyTrim(applyExpoValue(applyDeadZone(inputValue))));
}

/**
 * configures reverse state
 */
void RCStickAxis::setReverse(bool isReverse)
{
	this -> isReverse = isReverse;
}

/**
 * returns value of isReverse
 */
bool RCStickAxis::getReverse()
{
	return isReverse;
}


/**
 * set the calibrated min input value which is limited by hardware / poti
 */
void RCStickAxis::setCalibrationMinValue(unsigned char axisCalibrationMinValue)
{
	this -> axisCalibrationMinValue = axisCalibrationMinValue;
}

/**
 * set the calibrated max input value which is limited by hardware / poti
 */
void RCStickAxis::setCalibrationMaxValue(unsigned char axisCalibrationMaxValue)
{
	this -> axisCalibrationMaxValue = axisCalibrationMaxValue;
}

/**
 * applies calibration to the given inputValue
 */
unsigned char RCStickAxis::applyCalibration(unsigned char inputValue)
{

}

