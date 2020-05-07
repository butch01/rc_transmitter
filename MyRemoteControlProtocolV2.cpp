/*
 * MyRemoteControlProtocolV2.cpp
 *
 *  Created on: 27.06.2019
 *      Author: butch
 */

#include "MyRemoteControlProtocolV2.h"


//define logging
#define DEBUG_UPDATE_CRC 0
#define DEBUG_SET_CHANNEL_VALUE 0

MyRemoteControlProtocolV2::MyRemoteControlProtocolV2() {
	// initialize values array
	memset(values, 0, sizeof (values));


}

MyRemoteControlProtocolV2::~MyRemoteControlProtocolV2() {
	// TODO Auto-generated destructor stub
}


/**
 * updates the CRC byte
 */
void MyRemoteControlProtocolV2::updateCRC()
{
	#if DEBUG_UPDATE_CRC==1
		Log.trace(F("in MyRemoteControlProtocolV2::updateCRC\n"));
	#endif
	// calculate crc. values[sizeof (values) is CRC-Flag, so DO NOT CALCULATE FOR THIS ENTRY!)
	unsigned char myCRC = CRC8.smbus(values, sizeof(values-1));

	#if DEBUG_UPDATE_CRC==1
		Log.trace(F("  calculated CRC: %X\n"), myCRC);
	#endif
	// update CRC element in values
	values[(sizeof (values))-1] = myCRC;

	#if DEBUG_UPDATE_CRC==1
		Log.trace(F("  complete values: "));
	#endif
	for (int i=0; i< sizeof (values); i++)
	{
		if (i==sizeof (values)-1)
		{
			#if DEBUG_UPDATE_CRC==1
				Log.trace("%x\n",values[i]);
			#endif
		}
		else
		{
			#if DEBUG_UPDATE_CRC==1
				Log.trace("%x ",values[i]);
			#endif
		}
	}

	#if DEBUG_UPDATE_CRC==1
		Log.trace(F("end of MyRemoteControlProtocolV2::updateCRC\n"));
	#endif
}

/**
 * sets / updates a value for a specific channel
 */
void MyRemoteControlProtocolV2::setChannelValue(unsigned char value, unsigned char channel)
{
	#if DEBUG_SET_CHANNEL_VALUE==1
		Log.trace(F("in MyRemoteControlProtocolV2::setChannelValue\n"));
	#endif

	// first check if we are in bounds
	if ((signed int)channel < 0 || (signed int)channel > (int) (sizeof (values)-1)) // last value[sizeof(value)] is crc byte which cannot be set
	{
		// out of bounds
		Log.error(F("  channel is out of bounds. Channel requested: %d, available Channels: %d to %d\n"), channel, 0, (sizeof(values)-1));
	}
	else
	{
		// in bounds, so process it -> update the channel
		#if DEBUG_SET_CHANNEL_VALUE==1
			Log.trace (F("  setting value %X to channel %d\n"), value, channel);
		#endif
		values[channel]=value;
	}
	#if DEBUG_SET_CHANNEL_VALUE==1
		Log.trace(F("end of MyRemoteControlProtocolV2::setChannelValue\n"));
	#endif
}

/**
 * updates crc and then returns a pointer to array values (contains channels + crc)
 */
unsigned char * MyRemoteControlProtocolV2::getValueArray()
{
	updateCRC();
	return values;
}

unsigned char MyRemoteControlProtocolV2::getProtocolLength()
{
	return sizeof (values);
}

