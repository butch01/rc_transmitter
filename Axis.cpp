/*
 * Axis.cpp
 *
 *  Created on: 07.05.2020
 *      Author: butch
 */

#include "Axis.h"
#include "RCStickAxisFunctions.h"
#include <ArduinoLog.h>

Axis::Axis() {
	// TODO Auto-generated constructor stub
	myArduinoPin = 0;
	myExpo = 1;
	myTrim = 0;
	myAxisCalibrationMinValue = 0;
	myAxisCalibrationMaxValue = 255;
	myDeadZone = 0;
	myIsReverse = false;
	myLimitMin = 0;
	myLimitMax = 255;


}

Axis::~Axis() {
	// TODO Auto-generated destructor stub
}



/**
 * sets the LimitMin, but only if it is <= myAxisCalibrationMaxValue and is > myLimitMin
 * returns the current value of myLimitMax. So you can check directly if the set function was successful.
 */
uint8_t Axis::setLimitMin(uint8_t limitMin)
{
	if (limitMin >= myAxisCalibrationMinValue && limitMin < myLimitMax)
	{
		myLimitMin = limitMin;
	}
	return myLimitMin;
}


/**
 * returns myLimitMin
 */
uint8_t Axis::getLimitMin()
{
	return myLimitMin;
}


/**
 * sets the LimitMax, but only if it is <= myAxisCalibrationMaxValue and is > myLimitMin
 * returns the current value of myLimitMax. So you can check directly if the set function was successful.
 */
uint8_t Axis::setLimitMax(uint8_t limitMax)
{
	if (limitMax <= myAxisCalibrationMaxValue && limitMax > myLimitMin)
	{
		myLimitMax = limitMax;
	}
	return myLimitMax;
}


/**
 * returns myLimitMin
 */
uint8_t Axis::getLimitMax()
{
	return myLimitMax;
}



/**
 * gets value from arduino pin and enriches it.
 */
uint8_t Axis::getCalculatedValueFromPin()
{
	return RCStickAxisFunctions::enrichValue(
			analogRead(myArduinoPin), // inputValueRaw,
			1024, // analogResolution,
			myIsReverse, // isReverse,
			myTrim, // trim,
			myDeadZone, // deadZone,
			myExpo, //expo,
			myAxisCalibrationMinValue, // calMin,
			myAxisCalibrationMaxValue,  // calMax
			myLimitMin,
			myLimitMax);

}



/**
 * set axis calibration value min
 */
void Axis::setCalibrationMinValue(unsigned char axisCalibrationMinValue)
{
	this -> myAxisCalibrationMinValue = axisCalibrationMinValue;
}

/**
 * get axis calibration value min
 */
uint8_t Axis::getCalibrationMinValue()
{
	return this -> myAxisCalibrationMinValue;
}




/**
 * set axis calibration value max
 */
void Axis::setCalibrationMaxValue(unsigned char axisCalibrationMaxValue)
{
	this -> myAxisCalibrationMaxValue = axisCalibrationMaxValue;
}



/**
 * get axis calibration value max
 */
uint8_t Axis::getCalibrationMaxValue()
{
	return this -> myAxisCalibrationMaxValue;
}


/**
 * sets trim Value
 */
void Axis::setTrim(signed char trimValue)
{
	this -> myTrim = trimValue;
}


/**
 * get trim
 */
int8_t Axis::getTrim()
{
	return this -> myTrim;
}



/**
 * set deadzone
 */
void Axis::setDeadZone(unsigned char deadZone)
{
	this -> myDeadZone = deadZone;

}


/**
 * get deadzone
 */
uint8_t Axis::getDeadZone()
{
	return this -> myDeadZone;

}


/**
 * set Arduino Pin
 */
void Axis::setArduinoPin(unsigned char arduinoAnalogPin)
{
	this -> myArduinoPin = arduinoAnalogPin;
}


/**
 * sets the expo value
 */
void Axis::setExpo(float expoValue)
{
	myExpo = expoValue;
}


/**
 * gets the expo value
 */
float Axis::getExpo()
{
	return myExpo;
}

/**
 * sets reverse state for the axis
 */
void Axis::setReverse(bool isReverse)
{
	myIsReverse = isReverse;
}

/**
 * returns if the axis is reverse or not
 */
bool Axis::getReverse()
{
	return myIsReverse;
}
