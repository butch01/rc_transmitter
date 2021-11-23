/*
 * Axis.h
 *
 *  Created on: 07.05.2020
 *      Author: butch
 */

#ifndef AXIS_H_
#define AXIS_H_
#include <arduino.h>
#include "hw_config.h"

class Axis {

	public:
		Axis();
		virtual ~Axis();

		// Trim
		void setTrim(signed char trimValue);
		int8_t getTrim();

		// Dead Zone
		void setDeadZone(unsigned char deadZone);
		uint8_t getDeadZone();

		// Reverse
		void setReverse(bool isReverse);
		bool getReverse();

		// Limits. Limit will be considered as +/- 100% (0/255).
		uint8_t setLimitMin(uint8_t limitMin);
		uint8_t getLimitMin();

		uint8_t setLimitMax(uint8_t limitMax);
		uint8_t getLimitMax();

		// Expo
		void setExpo(float expoValue); // sets the expo value
		float getExpo(); // gets the exp value


		// returns the current value
		uint8_t getCalculatedValueFromPin();

		// Calibration
		void setCalibrationMinValue(unsigned char axisCalibrationMinValue);
		uint8_t getCalibrationMinValue();
		uint8_t getCalibrationMaxValue();
		void setCalibrationMaxValue(unsigned char axisCalibrationMaxValue);

		// Arduino Pin
		void setArduinoPin(unsigned char arduinoAnalogPin);


	private:

		int8_t myTrim;
		uint8_t myDeadZone;
		bool myIsReverse;
		float myExpo;

		uint8_t myAxisCalibrationMinValue;  // that is the PHYSICAL limit of the controller
		uint8_t myAxisCalibrationMaxValue;  // that is the PHYSICAL limit of the controller

		uint8_t  myArduinoPin;

		uint8_t myLimitMin; // that is the LOGICAL limit of the axis. It can be used to limit the travel of the axis
		uint8_t myLimitMax; // that is the LOGICAL limit of the axis. It can be used to limit the travel of the axis



};

#endif /* AXIS_H_ */
