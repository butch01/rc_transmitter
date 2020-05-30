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

#define FIRST_MENU_LINE 2
#define DISPLAY_NUMBER_OF_LINES 8


// define of the menu ids
#define MENU_ID_NONE 0 // no menu
#define MENU_ID_MAIN 1 // main menu
#define MENU_ID_AXIS_CHOOSER 10 // menu where to choose a specific stick to open the details
#define MENU_ID_AXIS 11 // detail configuration of a single stick's axis
#define MENU_ID_RECEIVER 5 // menu where to choose the reviever
#define MENU_ID_BUTTONS 20 // menu where to choose buttons for config


class Menu {

private:
	unsigned char menuCursorY;
	unsigned char currentMenuNumberOfEntries;
	U8X8_SH1106_128X64_NONAME_HW_I2C *pDisplay;
	void printHeadline(char *headline); // prints the headline to display

	unsigned char currentMenuId; // saves the current ID of the menu
	unsigned char getSelectedMenuEntryId();

public:
	Menu(U8X8_SH1106_128X64_NONAME_HW_I2C *display);
	virtual ~Menu();

	void processCursorNavigation();
	void moveCursorUp();
	void moveCursorDown();

	void printMenuCursor (); // print display cursor selection

	void printMenuTrimStatic(); // print trim menu (static captions, no values)
	void printMenuTrimValues(signed char *values); // prints the values for the trim menu. Values to print will be given by array

	unsigned char Menu::getMenuElementId(); // returns the selected menu element
	void printUpdateSingleValue(unsigned char x, unsigned char y, unsigned char length, signed int intValue); // prints a single int value. clears spaces before.
	int debugDisplayAddress();

	void printMenuMain(); // Prints the main menu


	void printMenuAxisChooser(); // prints all available sticks
	unsigned char getCurrentMenuId();

	unsigned char getMenuLevelUpId(); // returns the menu of the upper (in most cases previous) menu id.
	void printSubmenu(); //prints the submenu

	void printMenuId(unsigned char menuIdToPrint);

	void printMenuReceiver();
	void printMenuButtons();
	void printMenuAxisDetails(unsigned char axisId);


};




#endif /* MENU_H_ */
