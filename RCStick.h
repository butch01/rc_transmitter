/*
 * RCStick.h
 *
 *  Created on: 29.06.2019
 *      Author: butch
 */

#ifndef RCSTICK_H_
#define RCSTICK_H_
#include "RCStickAxis.h"
#include <ArduinoLog.h>

#define AXIS_X 0
#define AXIS_Y 1

class RCStick {
public:
	RCStick();
	virtual ~RCStick();
	RCStickAxis myAxes[2];
	RCStickAxis * getAxis(unsigned char axis); // returns pointer to rc axis


};

#endif /* RCSTICK_H_ */
