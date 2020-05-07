/*
 * RCStick.cpp
 *
 *  Created on: 29.06.2019
 *      Author: butch
 */

#include "RCStick.h"

RCStick::RCStick() {
	// create to axis for this stick
	RCStickAxis myAxes={};
}

RCStick::~RCStick() {
	// TODO Auto-generated destructor stub
}

/**
 * returns pointer to axis. If out of bounds, returning nullptr.
 */
RCStickAxis * RCStick::getAxis(unsigned char axis)
{
	// check bounds
	if (axis >=0 && axis < sizeof (myAxes))
	{
		// we are in bounds
		return &myAxes[axis];
	}
	else
	{
		Log.fatal(F("RCStick::getAxis(unsigned char axis) requests stick which is out of bounds. Requested: %d, valid is 0 to %d. Returning NULL!\n"), axis, sizeof(myAxes)-1);

		 return nullptr;
		// don't know how to get it work with nullpointer. So return myAxes[0];
		//return myAxes[0];
	}
}
