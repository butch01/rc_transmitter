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
// #include "RCStickAxisFunctions.h"
#include "Axis.h"
#include "hw_config.h"
#include "Menu.h"

//#include "RCConfig.h"
#include "defaultValues.h"
#include "loggingConfig.h"

#define VERSIONSTRING "0.2"




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
	#define STICK_LX_PIN A6
	#define STICK_LY_PIN A7
#endif

#if HW_NUMBER_OF_STICKS >1
	#define STICK_RX 2
	#define STICK_RY 3
	#define STICK_RIGHT 1

	// RANGE LIMITATIONS (calibration of poti)
	// to save memory this calibration can be done by define statement. It won't change until hardware changes
	#define STICK_RX_MIN_RANGE 15
	#define STICK_RX_MAX_RANGE 240
	#define STICK_RY_MIN_RANGE 15
	#define STICK_RY_MAX_RANGE 240

	// TRIM
	#define STICK_RX_TRIM 0
	#define STICK_RY_TRIM 0

	// DEADZONE
	#define STICK_RX_DEADZONE 2
	#define STICK_RY_DEADZONE 10

	// PIN ASSIGNMENT
	#define STICK_RX_PIN A6
	#define STICK_RY_PIN A7
#endif


// STATUS LED
#if HW_HAS_STATUS_LED
	#define STATUS_LED_PIN 7
	#define STATUS_LED_BLINK_INTERVAL_NOT_CONNECTED_MS 500
	unsigned long statusLedLastChange=0;
	bool statusLedBlinkStatus=false;
#endif


#if HW_HAS_DISPLAY
	// define the display config
	#include <U8x8lib.h>
	#ifdef U8X8_HAVE_HW_SPI
	#include <SPI.h>
	#endif
	U8X8_SH1106_128X64_NONAME_HW_I2C display(/* reset=*/ U8X8_PIN_NONE);
	unsigned char displayContrast = 10;



	// define the menu (without display the menu makes no sense
	Menu menu(&display, sticks);
#endif

#if HW_HAS_NATIVE_BUTTONS
	#include <PushButton.h>
	PushButton pushButtonBack((char) 1, (int) BUTTON_BACK_PIN);
	PushButton pushButtonUp((char) 1, (int) BUTTON_UP_PIN);
	PushButton pushButtonDown(1, BUTTON_DOWN_PIN);
	PushButton pushButtonLeft((char) 1, (int) BUTTON_LEFT_PIN);
	PushButton pushButtonRight(1, BUTTON_RIGHT_PIN);
	PushButton pushButtonEnter(1, BUTTON_ENTER_PIN); 
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
	//#define TRANSMISSION_LEVEL RF24_PA_MIN
	#define TRANSMISSION_LEVEL RF24_PA_MAX
#endif




// RC Protocol
MyRemoteControlProtocolV2 rcProtocol;

unsigned char seletedRadio = RADIO_INTERFACE_NRF24L01;


#define MIN_SEND_INTERVAL_MS 10
//#define MIN_SEND_INTERVAL_MS 500
unsigned long lastSendTime=0;


// RC Config
//RCConfig rcConfig;

// model store
#define CONFIG_MODEL_NAME_MAX_LENGTH 10
#define CONFIG_MODEL_COUNT 4
// currently with static naming. Will be changeable and stored / read from eeprom later
char configModelNames[CONFIG_MODEL_COUNT][CONFIG_MODEL_NAME_MAX_LENGTH] = {{'B','U','L','L','Y'},{'K','9','8','9'},{'P','o','r','s','c','h','e'},{'D','B','R','9'}};



bool negateBoolean(bool boolToNegate)
{
	if (boolToNegate)
	{
		return false;
	}
	else
	{
		return true;
	}
}


bool isButtonBackPressedOnce()
{
	#if HW_HAS_NATIVE_BUTTONS
		pushButtonBack.processButtonState();
		return pushButtonBack.getButtonPressedOnlyOnce();
	#endif
}

/**
 * checks if the Button has been pressed once (Menu mode. Do not flip logical state of Pusbutton)
 */
bool isButtonMenuEnterPressedOnce()
{
	#if HW_HAS_NATIVE_BUTTONS
		// in menu mode do not flip logical button state of push button
		pushButtonEnter.processButtonState(false);
		return pushButtonEnter.getButtonPressedOnlyOnce();
	#endif
}

/**
 * checks if the Button has been pressed once (Menu mode. Do not flip logical state of Pusbutton)
 */
bool isButtonMenuLeftPressedOnce()
{
	#if HW_HAS_NATIVE_BUTTONS
		// in menu mode do not flip logical button state of push button
		pushButtonLeft.processButtonState(false);
		return pushButtonLeft.getButtonPressedOnlyOnce();
	#endif
}

/**
 * checks if the Button has been pressed once (Menu mode. Do not flip logical state of Pusbutton)
 */
bool isButtonMenuLeftPressed()
{
	#if HW_HAS_NATIVE_BUTTONS
		// in menu mode do not flip logical button state of push button
		pushButtonLeft.processButtonState(false);
		return pushButtonLeft.getButtonStateRawDebouncedInputPullup();
	#endif
}

/**
 * checks if the Button has been pressed once (Menu mode. Do not flip logical state of Pusbutton)
 */
bool isButtonMenuRightPressedOnce()
{
	#if HW_HAS_NATIVE_BUTTONS
		// in menu mode do not flip logical button state of push button
		pushButtonRight.processButtonState(false);
		return pushButtonRight.getButtonPressedOnlyOnce();
	#endif
}

/**
 * checks if the Button has been pressed once (Menu mode. Do not flip logical state of Pusbutton)
 */
bool isButtonMenuRightPressed()
{
	#if HW_HAS_NATIVE_BUTTONS
		// in menu mode do not flip logical button state of push button
		pushButtonRight.processButtonState(false);
		return pushButtonRight.getButtonStateRawDebouncedInputPullup();
	#endif
}

/**
 * checks if the Button has been pressed once (Menu mode. Do not flip logical state of Pusbutton)
 */
bool isButtonMenuUpPressedOnce()
{
	#if HW_HAS_NATIVE_BUTTONS
		// in menu mode do not flip logical button state of push button
		pushButtonUp.processButtonState(false);
		return pushButtonUp.getButtonPressedOnlyOnce();
	#endif
}

/**
 * checks if the Button has been pressed once (Menu mode. Do not flip logical state of Pusbutton)
 */
bool isButtonMenuDownPressedOnce()
{
	#if HW_HAS_NATIVE_BUTTONS
		// in menu mode do not flip logical button state of push button
		pushButtonDown.processButtonState(false);
		return pushButtonDown.getButtonPressedOnlyOnce();
	#endif
}


/**
 * returns the Logical State of a pushbutton
 */
bool getButtonAltLeftLogicalState()
{
	#if HW_HAS_NATIVE_BUTTONS
		pushButtonLeft.processButtonState();
		return pushButtonLeft.getButtonStateLogical();
	#endif
}

/**
 * returns the Logical State of a pushbutton
 */
bool getButtonAltRightLogicalState()
{
	#if HW_HAS_NATIVE_BUTTONS
		pushButtonRight.processButtonState();
		return pushButtonRight.getButtonStateLogical();
	#endif
}

/**
 * returns the Logical State of a pushbutton
 */
bool getButtonAltUpLogicalState()
{
	#if HW_HAS_NATIVE_BUTTONS
		pushButtonUp.processButtonState();
		return pushButtonUp.getButtonStateLogical();
	#endif
}

/**
 * returns the Logical State of a pushbutton
 */
bool getButtonAltDownLogicalState()
{
	#if HW_HAS_NATIVE_BUTTONS
		pushButtonDown.processButtonState();
		return pushButtonDown.getButtonStateLogical();
	#endif
}

/**
 * returns the Logical State of a pushbutton
 */
bool getButtonAltEnterLogicalState()
{
	#if HW_HAS_NATIVE_BUTTONS
		pushButtonEnter.processButtonState();
		return pushButtonEnter.getButtonStateLogical();
	#endif
}


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

/**
 * to be included into setup procedure
 */
void setupDisplay()
{
#if HW_HAS_DISPLAY
	display.begin();
	display.setPowerSave(0);
	display.setContrast(displayContrast);
	//display.setFont(u8x8_font_chroma48medium8_r);
	display.setFont(u8x8_font_artossans8_u);
	//display.drawString(2,3,"VERSION: ");
	//display.draw(VERSIONSTRING);
	display.setCursor(2, 3);
	display.print("VERSION: ");
	display.print(VERSIONSTRING);
#endif
}


void setupNativeButtons()
{
	#if HW_HAS_NATIVE_BUTTONS
		pinMode((unsigned char) BUTTON_BACK_PIN,INPUT_PULLUP);
		pinMode((unsigned char) BUTTON_DOWN_PIN, INPUT_PULLUP);
		pinMode((unsigned char) BUTTON_UP_PIN, INPUT_PULLUP);
		pinMode((unsigned char) BUTTON_LEFT_PIN, INPUT_PULLUP);
		pinMode((unsigned char) BUTTON_RIGHT_PIN, INPUT_PULLUP);
		pinMode((unsigned char) BUTTON_ENTER_PIN, INPUT_PULLUP);
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

	#if HW_HAS_DISPLAY
		setupDisplay();

		Log.verbose(F("display address: %d\n"), &display);
		Log.verbose(F("menu display address: %d\n"), menu.debugDisplayAddress());

	#endif

	#if HW_HAS_NATIVE_BUTTONS
		setupNativeButtons();
	#endif


	// setup interface nrf24L01
	setupInterfaceRadioNrf24L01();

	#if HW_NUMBER_OF_STICKS > 0
	// sticks, set arduino pins
	sticks[STICK_LEFT].getAxis(AXIS_X)->setArduinoPin((unsigned char) STICK_LX_PIN);
	sticks[STICK_LEFT].getAxis(AXIS_Y)->setArduinoPin((unsigned char) STICK_LY_PIN);

	// set poti limits
	sticks[STICK_LEFT].getAxis(AXIS_X)-> setCalibrationMinValue(STICK_LX_HW_MIN_RANGE);
	sticks[STICK_LEFT].getAxis(AXIS_X)-> setCalibrationMaxValue(STICK_LX_HW_MAX_RANGE);
	sticks[STICK_LEFT].getAxis(AXIS_Y)-> setCalibrationMinValue(STICK_LY_HW_MIN_RANGE);
	sticks[STICK_LEFT].getAxis(AXIS_Y)-> setCalibrationMaxValue(STICK_LY_HW_MAX_RANGE);

//	// initializing the config. Connecting the sticks
//	rcConfig.init(sticks);
//	// switching to the last known model
//	rcConfig.switchToModel(rcConfig.getLastModelId());


/**
 * commented out, because we now use the eeprom loader
	// sticks, set trim
	sticks[STICK_LEFT].getAxis(AXIS_X)->setTrim(STICK_LX_HW_TRIM);
	sticks[STICK_LEFT].getAxis(AXIS_Y)->setTrim(STICK_LY_HW_TRIM);

	// set deadzone
	sticks[STICK_LEFT].getAxis(AXIS_X)->setDeadZone(STICK_LX_DEADZONE);
	sticks[STICK_LEFT].getAxis(AXIS_Y)->setDeadZone(STICK_LY_DEADZONE);

	// set reverse
	sticks[STICK_LEFT].getAxis(AXIS_Y)->setReverse(true);
*/

	#endif

	#if HW_NUMBER_OF_STICKS > 1
		sticks[STICK_RIGHT].getAxis(AXIS_X)->setTrim(STICK_RX_HW_TRIM);
		sticks[STICK_RIGHT].getAxis(AXIS_Y)->setTrim(STICK_RY_HW_TRIM);

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
	#if LOGGING_RADIO_SENDING == 1
	char function[]="mySend";
	#endif
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

				#if LOGGING_RADIO_SENDING_PACKET_ONLY
					// trying to debug the array
					for (int i=0; i<dataLenth;i++)
					{
						Serial.print((int) dataToSend[i],HEX);
						Serial.print(" ");
					}
					Serial.print("\n");
				#endif
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


/**
 * processes the menu buttons
 */
void processMenuButtons()
{
	if (isButtonMenuUpPressedOnce())
	{
		menu.moveCursorUp();
		Log.verbose(F("current MenuId = %d\n"),menu.getCurrentMenuId());
	}
	if (isButtonMenuDownPressedOnce())
	{
		menu.moveCursorDown();
		Log.verbose(F("current MenuId = %d\n"),menu.getCurrentMenuId());
	}
	if (isButtonMenuEnterPressedOnce())
	{
		menu.actionOnEnter();
		Log.verbose(F("current MenuId = %d\n"),menu.getCurrentMenuId());
	}

	if (isButtonMenuLeftPressed())
	{
		menu.editDecreaseCurrentValue();
	}

	if (isButtonMenuRightPressed())
	{
		menu.editIncreaseCurrentMenuValue();
	}


}


/**
 * checks the alternative buttons. Available are all menu buttons except for back, because back enables the menu.
 * writes the logical state of the pushbuttons left, right, up, down, enter to rc protocol channels.
 */
void processAlternativeButtons()
{
	bool state = false;

	state = getButtonAltLeftLogicalState();
	rcProtocol.setChannelValue(state, PROT_BTN_SQUARE);
	rcProtocol.setChannelValue(getButtonAltRightLogicalState(), PROT_BTN_CIRCLE);
	rcProtocol.setChannelValue(getButtonAltUpLogicalState(), PROT_BTN_TRIANGLE);
	rcProtocol.setChannelValue(getButtonAltDownLogicalState(), PROT_BTN_CROSS);
	rcProtocol.setChannelValue(getButtonAltEnterLogicalState(), PROT_BTN_R1);
	
}

void loop() {

	// Log.notice(F("reading stick\n"));
	// read input of analog sticks
	#if HW_NUMBER_OF_STICKS > 0
		#if HW_NUMBER_OF_STICKS == 1
//		unsigned char x=sticks[STICK_LEFT].getAxis(AXIS_X)-> getCalculatedValueFromPin();
//		unsigned char y=sticks[STICK_LEFT].getAxis(AXIS_X)-> getCalculatedValueFromPin();
//		Log.notice("x=%d, y=%d\n", x,y);

		rcProtocol.setChannelValue(sticks[STICK_LEFT].getAxis(AXIS_X)-> getCalculatedValueFromPin(), PROT_STICK_RX);
		#else
			//rcProtocol.setChannelValue(sticks[STICK_LEFT].getAxis(AXIS_X)->applyAll(getAnalogValue255(STICK_LX_PIN)), PROT_STICK_LX);
			rcProtocol.setChannelValue(sticks[STICK_LEFT].getAxis(AXIS_X)-> getCalculatedValueFromPin(), PROT_STICK_LX);
		#endif

		rcProtocol.setChannelValue(sticks[STICK_LEFT].getAxis(AXIS_Y)-> getCalculatedValueFromPin(), PROT_STICK_LY);
	#endif

//	#if HW_NUMBER_OF_STICKS > 1
//		rcProtocol.setChannelValue(sticks[STICK_RIGHT].getAxis(AXIS_X)->applyAll(getAnalogValue255(STICK_RX_PIN)), PROT_STICK_RX);
//		rcProtocol.setChannelValue(sticks[STICK_RIGHT].getAxis(AXIS_Y)->applyAll(getAnalogValue255(STICK_RY_PIN)), PROT_STICK_RY);
//	#endif



//
//
	if (isButtonBackPressedOnce())
	{
		menu.printMenuId(menu.getMenuLevelUpId());
		Log.verbose(F("current MenuId = %d\n"),menu.getCurrentMenuId());
	}

	// check menu buttons if in menu
	if (menu.getCurrentMenuId() != MENU_ID_NONE)
	{
		// we are in a menu

		// check menu buttons
		processMenuButtons();
	}
	else
	{
		// we are NOT in menu -> check alternative button modes
		processAlternativeButtons();
	}





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
			// always on if transmitter chip is connected
			digitalWrite(STATUS_LED_PIN, true);
		}
		else
		{
			// blink if transmitter chip is NOT connected
			unsigned long currentTime = millis();
			if (currentTime - statusLedLastChange > STATUS_LED_BLINK_INTERVAL_NOT_CONNECTED_MS)
			{
				if (statusLedBlinkStatus)
				{
					digitalWrite(STATUS_LED_PIN, false);
					statusLedBlinkStatus=false;
				}
				else
				{
					digitalWrite(STATUS_LED_PIN, true);
					statusLedBlinkStatus=true;
				}
				statusLedLastChange = currentTime;
			}



		}
	#endif
#endif
}
