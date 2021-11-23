/*
 * RCStickAxisFunctions.cpp
 *
 *  Created on: 07.05.2020
 *      Author: butch
 */

#define IS_DEBUG_TRIM 0
#define IS_DEBUG_ENRICH 0

#include "RCStickAxisFunctions.h"
RCStickAxisFunctions::RCStickAxisFunctions() {


}

RCStickAxisFunctions::~RCStickAxisFunctions() {
	// TODO Auto-generated destructor stub
}

/**
 * add all supported enrichments (reverse, trim, expo, fload, calibration)
 * returns unsigned char
 */
static unsigned char RCStickAxisFunctions::enrichValue (uint16_t inputValueRaw, uint16_t analogResolution, bool isReverse, int8_t trim, uint8_t deadZone, float expo, uint8_t calMin, uint8_t calMax, uint8_t limitMin, uint8_t limitMax)

{
	// set to input value
	unsigned char value = map (inputValueRaw,0,analogResolution, 0,255);
	#if IS_DEBUG_ENRICH
		Serial.print("map: ");
		Serial.print(value);
	#endif
	// apply calibration and limit
	value = applyCalibrationAndLimit(value, calMin, calMax, limitMin, limitMax);

	#if IS_DEBUG_ENRICH
		Serial.print(" cal: ");
		Serial.print(value);
	#endif
	// apply reverse
	value = applyReverse(value, isReverse);
	#if IS_DEBUG_ENRICH
		Serial.print(" rev: ");
		Serial.print(value);
	#endif
	// apply deadzone
	value = applyDeadZone(value, deadZone);
	#if IS_DEBUG_ENRICH
		Serial.print(" dead: ");
		Serial.print(value);
	#endif

	// apply trim

		value = applyTrim(value, trim);
	#if IS_DEBUG_ENRICH
		Serial.print(" trim: ");
		Serial.print(value);
	#endif
	// apply expo
	value = applyExpo(value, expo);
	#if IS_DEBUG_ENRICH
		Serial.print("exp: ");
		Serial.println(value);
	#endif
	return value;

}


/**
 * adds Trim to given Value
 */
static unsigned char RCStickAxisFunctions::applyTrim(unsigned char inputValue, signed char trim)
{
	signed int value = (signed int) inputValue;
	// apply trim AFTER deadZone
	value = value + trim;

//	// cut to 0 or 255 if
//	if (value < 0)
//	{
//		value = 0;
//	}
//	else if (value > 255)
//	{
//		value = 255;
//	}


	signed int max_in= 255 + trim;
	signed int min_in= 0 + trim;

	#if IS_DEBUG_TRIM
		Serial.print(" with_trim:");
		Serial.print(value);
		Serial.print(" min_in:");
		Serial.print(min_in);
		Serial.print(" max_in:");
		Serial.print(max_in);
	#endif


	// stretch the half input ranges
	if (value < 255/2 + trim)
	{
		value = map(value, min_in, 255/2 + trim, 0, 255/2 + trim);
	}
	else if (value > 255/2 + trim)
	{
		value = map(value, 255/2 + trim, max_in, 255/2 + trim, 255);
	}

	#if IS_DEBUG_TRIM
		Serial.print(" aft trim:");
		Serial.print(value);
		Serial.print(" ");
	#endif


	return (unsigned char) value;
}



/**
 * applies the deadZone
 */
unsigned char RCStickAxisFunctions::applyDeadZone (unsigned char inputValue, unsigned char deadZone)
{
	signed int value=0;

	// apply deadZone
	if ( (inputValue > 127 - deadZone) && (inputValue < 127 +  deadZone))
	{
		value = 127;
	}
	else if (inputValue > 127)
	{
		value = map (inputValue, 127 + deadZone, 255, 128, 255);
	}
	else if (value < 127)
	{
		value = map (inputValue, 0, 127 - deadZone,0, 126);
	}
	return value;
}




/**
 * applies expo to given value
 * expects 0 to 255 as input range.
 * returns range 0 to 255.
 */
static unsigned char RCStickAxisFunctions::applyExpo(unsigned char inputValue, float expo)
{

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
 * assignes the calibration of the hardware poti
 */
static unsigned char RCStickAxisFunctions::applyCalibrationAndLimit(uint8_t value, uint8_t calMin, uint8_t calMax, uint8_t limitMin, uint8_t limitMax)
{
	unsigned char calculatedValue = value;

	// if calibration values are bad, prevent from out of bounds. limit to calibration settings if value is off limits
	if (calculatedValue < calMin)
	{
		calculatedValue = calMin;
	}
	else
	{
		if (calculatedValue > calMax)
		{
			calculatedValue = calMax;
		}
	}

	// map the value based on limits
	calculatedValue = map (calculatedValue, calMin, calMax, limitMin, limitMax);

	return calculatedValue;
}



/**
 * if isReverse is set, it inverts the direction.
 * input expects range 0 to 255.
 * returns range 0 to 255.
 */
static unsigned char RCStickAxisFunctions::applyReverse(unsigned char value, bool isReverse)
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
