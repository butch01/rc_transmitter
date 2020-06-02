/*
 * Axis.h
 *
 *  Created on: 07.05.2020
 *      Author: butch
 */

#ifndef AXIS_H_
#define AXIS_H_

class Axis {

	public:
		Axis();
		virtual ~Axis();

		void setTrim(signed char trimValue);
		signed char getTrim();

		void setDeadZone(unsigned char deadZone);
		unsigned char getDeadZone();

		void setReverse(bool isReverse);
		bool getReverse();


		void setExpo(float expoValue); // sets the expo value
		float getExpo(); // gets the exp value


		// returns the current value
		unsigned char getCalculatedValueFromPin();

		// Calibration
		void setCalibrationMinValue(unsigned char axisCalibrationMinValue);
		unsigned char Axis::getCalibrationMinValue();
		unsigned char Axis::getCalibrationMaxValue();
		void setCalibrationMaxValue(unsigned char axisCalibrationMaxValue);

		// Arduino Pin
		void setArduinoPin(unsigned char arduinoAnalogPin);


	private:

		signed char myTrim;
		unsigned char myDeadZone;
		bool myIsReverse;
		float myExpo;

		unsigned char myAxisCalibrationMinValue;
		unsigned char myAxisCalibrationMaxValue;

		unsigned char  myArduinoPin;



};

#endif /* AXIS_H_ */
