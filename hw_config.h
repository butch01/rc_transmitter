/*
 * hw_config.h
 *
 *  Created on: 28.05.2020
 *      Author: butch
 */

#ifndef HW_CONFIG_H_
#define HW_CONFIG_H_


#define ANALOG_IN_MAX 1024


#define HW_NUMBER_OF_STICKS 1

#define HW_HAS_STATUS_LED 1

// define if we have a display
#define HW_HAS_DISPLAY 1

// define if we have native buttons directly connected to the arduino pins
#define HW_HAS_NATIVE_BUTTONS 1
#if HW_HAS_NATIVE_BUTTONS
	#define BUTTON_BACK_PIN A3
	#define BUTTON_ENTER_PIN 8
	#define BUTTON_UP_PIN A1
	#define BUTTON_DOWN_PIN 4
	#define BUTTON_LEFT_PIN A0
	#define BUTTON_RIGHT_PIN 6
#endif


#endif /* HW_CONFIG_H_ */
