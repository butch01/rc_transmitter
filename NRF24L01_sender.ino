/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
*
* by Dejan Nedelkovski, www.HowToMechatronics.com
*
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include "RCStick.h"
#include "MyRemoteControlProtocolV2.h"
#include <ArduinoLog.h>


#define ANALOG_IN_MAX 1024



#define HW_NUMBER_OF_STICKS 1
#define HW_HAS_STATUS_LED 1

#if HW_NUMBER_OF_STICKS > 0
	#define STICK_LX 0
	#define STICK_LY 1
	#define STICK_LEFT 0

	// TRIM
	#define STICK_LX_TRIM 0
	#define STICK_LY_TRIM 0

	// DEADZONE
	#define STICK_LX_DEADZONE 2
	#define STICK_LY_DEADZONE 2

	// PIN ASSIGNMENT
	#define STICK_LX_PIN A0
	#define STICK_LY_PIN A1
#endif

#if HW_NUMBER_OF_STICKS >1
	#define STICK_RX 2
	#define STICK_RY 3
	#define STICK_RIGHT 1

	// TRIM
	#define STICK_RX_TRIM 0
	#define STICK_RY_TRIM 0

	// DEADZONE
	#define STICK_RX_DEADZONE 2
	#define STICK_RY_DEADZONE 10

	// PIN ASSIGNMENT
	#define STICK_RX_PIN A2
	#define STICK_RY_PIN A3
#endif


// STATUS LED
#if HW_HAS_STATUS_LED
	#define STATUS_LED_PIN 7
#endif

// add RC Sticks (which owns 2 axis each)
RCStick sticks[HW_NUMBER_OF_STICKS];


#define RADIO_SERIAL 0
#define RADIO_INTERFACE_HM10 3
#define RADIO_INTERFACE_NRF24L01 1

#define COMPILE_RADIO_INTERFACE_SERIAL 0
#if COMPILE_RADIO_INTERFACE_SERIAL == 1
// todo
#endif

// confiure hm10 device
#define COMPILE_RADIO_INTERFACE_HM10 0
#if COMPILE_RADIO_INTERFACE_HM10 == 1
	// todo
#endif

// configure nrf24l01 radio
#define COMPILE_RADIO_INTERFACE_NRF24L01 1
#if COMPILE_RADIO_INTERFACE_NRF24L01 == 1
	#include <SPI.h>
	#include <nRF24L01.h>
	#include <RF24.h>

	// PIN ASSIGNEMNT
	#define CE_PIN 10
	#define CSN_PIN 9

	RF24 nrf24l01Radio(CE_PIN, CSN_PIN); // CE, CSN
	const byte address[6] = "00001";

	// TRANSMISSION LEVEL
	#define TRANSMISSION_LEVEL RF24_PA_MIN
	#define TRANSMISSION_LEVEL RF24_PA_LOW
#endif


// define detail logging
#define LOGGING_RADIO_SENDING 0

// RC Protocol
MyRemoteControlProtocolV2 rcProtocol;

unsigned char seletedRadio = RADIO_INTERFACE_NRF24L01;


#define MIN_SEND_INTERVAL_MS 25
//#define MIN_SEND_INTERVAL_MS 500
unsigned long lastSendTime=0;

/**
 * to include in setup()
 *
 */
void setupInterfaceRadioNrf24L01()
{
	Log.notice(F("in NRF24L01"));
	#if COMPILE_RADIO_INTERFACE_NRF24L01 == 1
		Log.notice(F("setting up NRF24L01"));
		nrf24l01Radio.begin();

		nrf24l01Radio.setDataRate( RF24_250KBPS );

		nrf24l01Radio.setAutoAck(false);
		//nrf24l01Radio.disableDynamicPayloads();
		//nrf24l01Radio.setPayloadSize(rcProtocol.getProtocolLength());


		nrf24l01Radio.openWritingPipe(address);
		nrf24l01Radio.setPALevel(TRANSMISSION_LEVEL);
		nrf24l01Radio.stopListening();
		nrf24l01Radio.setRetries(0, 0);
		Serial.print("CRC: ");
		Serial.println(nrf24l01Radio.getCRCLength());
		Log.notice(F("is chip connected %b" CR), nrf24l01Radio.isChipConnected());



	#endif
}



void setup() {
	Serial.begin(115200);
	//Serial.println(F("starting sender ;-)"));
	Log.begin   (LOG_LEVEL_VERBOSE, &Serial);
	Log.notice(F("starting sender with logging\n"));

	// setup status LED
	#if HW_HAS_STATUS_LED
		pinMode(STATUS_LED_PIN, OUTPUT);
	#endif


	// setup interface nrf24L01
	setupInterfaceRadioNrf24L01();

	#if HW_NUMBER_OF_STICKS > 0
	// sticks, set trim
	sticks[STICK_LEFT].getAxis(AXIS_X)->setTrim(STICK_LX_TRIM);
	sticks[STICK_LEFT].getAxis(AXIS_Y)->setTrim(STICK_LY_TRIM);

	// set deadzone
	sticks[STICK_LEFT].getAxis(AXIS_X)->setDeadZone(STICK_LX_DEADZONE);
	sticks[STICK_LEFT].getAxis(AXIS_Y)->setDeadZone(STICK_LY_DEADZONE);

	// set reverse
	//sticks[STICK_LEFT].getAxis(AXIS_Y)->setReverse(true);
	#endif

	#if HW_NUMBER_OF_STICKS > 1
		sticks[STICK_RIGHT].getAxis(AXIS_X)->setTrim(STICK_RX_TRIM);
		sticks[STICK_RIGHT].getAxis(AXIS_Y)->setTrim(STICK_RY_TRIM);

		// set deadzone
		sticks[STICK_RIGHT].getAxis(AXIS_X)->setDeadZone(STICK_RX_DEADZONE);
		sticks[STICK_RIGHT].getAxis(AXIS_Y)->setDeadZone(STICK_RY_DEADZONE);

	#endif
	// set deadzone



	// initialize values (without sticks) of protocol. Sticks are stored at channel 0-3.
	for (unsigned char i=4;i<PROT_ARRAY_LENGTH;i++)
	{
		rcProtocol.setChannelValue(0, i);
	}
}

/**
 * converts analog read of given pin, mapped to range 0-255
 */
unsigned char getAnalogValue255(unsigned char arduinoAnalogPin)
{
	return map(analogRead(arduinoAnalogPin),0,ANALOG_IN_MAX,0,255);
}

/**
 * sends the data, based on the selected Interface
 */
void mySend(unsigned char dataToSend[], unsigned char dataLenth, unsigned char radioInterface)
{
	char function[]="mySend";
	if (millis() - (unsigned long) MIN_SEND_INTERVAL_MS > lastSendTime)
	{
		// need to send data
		#if COMPILE_RADIO_INTERFACE_NRF24L01
			if (radioInterface == RADIO_INTERFACE_NRF24L01)
			{
				#if LOGGING_RADIO_SENDING == 1
					Log.notice(F("%l - %s - snd NRF24L01: %X size %d" CR), millis(), function,  &dataToSend, dataLenth );

					//Log.notice(F("sending\n"));
					#endif

					// trying to debug the array
					for (int i=0; i<dataLenth;i++)
					{
						Serial.print((int) dataToSend[i],HEX);
						Serial.print(" ");
					}
					Serial.print("\n");

				nrf24l01Radio.write(dataToSend, dataLenth);


				//  const char text[] = "Hello World!\n";
				//  Serial.print(F("sending: "));
				//  Serial.println(i);
				//
				//
//				const char text[] = "Hello World";
//				Log.notice(F("sending %s"), text);
//				nrf24l01Radio.write(&text, sizeof(text));
				lastSendTime=millis();
			}
		#endif
	}
	else
	{
		// no sending yet
		#if LOGGING_RADIO_SENDING == 1
			Log.notice(F("%l - %s - not sending now" CR), millis(), function);
		#endif
	}
}


/**
 * for testing only. Is sending static packet
 */
void nrfSendStaticDummyData()
{
	unsigned char dataToSend[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
	//char dataToSend[] = "hello";
	nrf24l01Radio.write(dataToSend, sizeof (dataToSend));
}


void loop() {

	// Log.notice(F("reading stick\n"));
	// read input of analog sticks
	#if HW_NUMBER_OF_STICKS > 0
		#if HW_NUMBER_OF_STICKS == 1
			rcProtocol.setChannelValue(sticks[STICK_LEFT].getAxis(AXIS_X)->applyAll(getAnalogValue255(STICK_LX_PIN)), PROT_STICK_RX);
		#else
			rcProtocol.setChannelValue(sticks[STICK_LEFT].getAxis(AXIS_X)->applyAll(getAnalogValue255(STICK_LX_PIN)), PROT_STICK_LX);
		#endif
		rcProtocol.setChannelValue(sticks[STICK_LEFT].getAxis(AXIS_Y)->applyAll(getAnalogValue255(STICK_LY_PIN)), PROT_STICK_LY);
	#endif

	#if HW_NUMBER_OF_STICKS > 1
		rcProtocol.setChannelValue(sticks[STICK_RIGHT].getAxis(AXIS_X)->applyAll(getAnalogValue255(STICK_RX_PIN)), PROT_STICK_RX);
		rcProtocol.setChannelValue(sticks[STICK_RIGHT].getAxis(AXIS_Y)->applyAll(getAnalogValue255(STICK_RY_PIN)), PROT_STICK_RY);
	#endif

	// build protocol value incl. CRC and send it
	// not sure here what to do with the pointer.


//	unsigned char *t[PROT_ARRAY_LENGTH];
//	t= *rcProtocol.getValueArray();
//	Log.verbose(F("length of array: %d\n"), sizeof (rcProtocol.getValueArray()));
//	for (int i=0; i<PROT_ARRAY_LENGTH;i++)
//	{
//		Serial.println(t[i]);
//	}

	// Serial.println("MY_SIZE");
	// Serial.println((int) rcProtocol.getProtocolLength());
	mySend(rcProtocol.getValueArray(), rcProtocol.getProtocolLength(), seletedRadio);
	//nrfSendStaticDummyData();
	//delay (1000);




//  const char text[] = "Hello World!\n";
//  Serial.print(F("sending: "));
//  Serial.println(i);
//
//
//  radio.write(&i, sizeof(text));
//  i++;
  //delay(5);
	//delay (500);

#if HW_HAS_STATUS_LED
	#if COMPILE_RADIO_INTERFACE_NRF24L01
		if (nrf24l01Radio.isChipConnected())
		{
			digitalWrite(STATUS_LED_PIN, true);
		}
		else
		{
			digitalWrite(STATUS_LED_PIN, false);
		}
	#endif
#endif
}
