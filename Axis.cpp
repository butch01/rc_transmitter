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
	myAxisCalibrationMinValue=0;
	myAxisCalibrationMaxValue=255;
	myDeadZone=0;
	myIsReverse=false;


}

Axis::~Axis() {
	// TODO Auto-generated destructor stub
}



/**
 * gets value from arduino pin and enriches it.
 */
unsigned char Axis::getCalculatedValueFromPin()
{
	return RCStickAxisFunctions::enrichValue(
			analogRead(myArduinoPin), // inputValueRaw,
			1024, // analogResolution,
			myIsReverse, // isReverse,
			myTrim, // trim,
			myDeadZone, // deadZone,
			myExpo, //expo,
			myAxisCalibrationMinValue, // calMin,
			myAxisCalibrationMaxValue  // calMax
			);

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
unsigned char Axis::getCalibrationMinValue()
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
unsigned char Axis::getCalibrationMaxValue()
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
signed char Axis::getTrim()
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
unsigned char Axis::getDeadZone()
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
