/*
 * Menu.h
 *
 *  Created on: 28.05.2020
 *      Author: butch
 */

#ifndef MENU_H_
#define MENU_H_

#include "hw_config.h"
#include <U8x8lib.h>
#ifdef U8X8_HAVE_HW_SPI
	#include <SPI.h>
#endif

#include "RCStick.h"

#define FIRST_MENU_LINE 2
#define DISPLAY_NUMBER_OF_LINES 8


// define of the menu ids
#define MENU_ID_NONE 0 // no menu
#define MENU_ID_MAIN 1 // main menu
#define MENU_ID_AXIS_CHOOSER 10 // menu where to choose a specific stick to open the details
#define MENU_ID_AXIS 11 // detail configuration of a single stick's axis
#define MENU_ID_RECEIVER 5 // menu where to choose the reviever
#define MENU_ID_BUTTONS 20 // menu where to choose buttons for config

#define MENU_COL_0_X_POS 2

#define MENU_AXIS_VALUE_X_POS 9
#define MENU_AXIS_DEAD_Y_POS 0
#define MENU_AXIS_TRIM_Y_POS 1
#define MENU_AXIS_EXPO_Y_POS 2
#define MENU_AXIS_REVERSE_Y_POS 3


#define DEFAULT_INCREMENT_CHAR 1
#define DEFAULT_INCREMENT_FLOAT 0.1
#define MENU_MODIFY_SLOW_INTERVAL_MS 150 // slow interval (after starting of change button pressed)
#define MENU_MODIFY_SLOW_TIME_RANGE 2000 // time range where slow interval is applied. After that fast interval will be used
#define MENU_MOFIFY_FAST_INTERVAL_MS 50 // fast interval (after holding the pressed change button for a while)
#define MENU_MODIFY_TIMEOUT 250 // after that time after last trigger, we assume that the button is not pressed anymore and we reset edit timer


class Menu {

private:
	unsigned char menuCursorY;
	unsigned char currentMenuNumberOfEntries;
	U8X8_SH1106_128X64_NONAME_HW_I2C *pDisplay; // pointer to display
	RCStick *mySticks; // pointer to sticks

	void printHeadline(char *headline); // prints the headline to display

	unsigned char currentMenuId; // saves the current ID of the menu
	unsigned char getSelectedMenuEntryId();
	bool myIsEditMode; // defines if we are currently in edit mode or not.

	void printAxisDeadzoneValue();
	void printAxisExpoValue();
	void printAxisTrimValue();
	void printAxisReverseValue();

	signed char calculateStickId();
	signed char calculateAxisId();

	signed char myCurrentAxisMenuId; // Id of the axis (Stick 0X -> 0, Stick 0Y -> 1, Stick 1X -> 2, ...   -1 if not set / not applicable)
	unsigned char unsignedCharLimiter(signed int unsignedCharToLimit);
	signed char signedCharLimiter(signed int signedCharToLimit);

	unsigned long lastEditChangeValueStartTime; // 0 if not in edit mode / start changing action -> time where edit started
	unsigned long lastEditChangeValueLastTime; // 0 if not in edit mode / start changing action -> time where edit action was last triggered


public:
	Menu(U8X8_SH1106_128X64_NONAME_HW_I2C *display, RCStick *sticks);
	virtual ~Menu();

	void processCursorNavigation();
	void moveCursorUp();
	void moveCursorDown();

	void printMenuCursor (); // print display cursor selection

	void printMenuTrimStatic(); // print trim menu (static captions, no values)
	void printMenuTrimValues(signed char *values); // prints the values for the trim menu. Values to print will be given by array

	unsigned char getMenuElementId(); // returns the selected menu element
	void printUpdateSingleValue(unsigned char x, unsigned char y, unsigned char length, signed int intValue); // prints a single int value. clears spaces before.
	int debugDisplayAddress();

	void printMenuMain(); // Prints the main menu


	void printMenuAxisChooser(); // prints all available sticks
	unsigned char getCurrentMenuId();

	unsigned char getMenuLevelUpId(); // returns the menu of the upper (in most cases previous) menu id.
	void actionOnEnter(); //prints the submenu

	void printMenuId(unsigned char menuIdToPrint);

	void printMenuReceiver();
	void printMenuButtons();
	void printMenuAxisDetails(unsigned char axisMenuId);

	void editIncreaseCurrentMenuValue(signed char valueModifier=1);
	void editDecreaseCurrentValue();

	bool isEditMode(); // gets info if menu is in edit mode


};




#endif /* MENU_H_ */
