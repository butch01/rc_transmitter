/*
 * Menu.cpp
 *
 *  Created on: 28.05.2020
 *      Author: butch
 */

#include "Menu.h"
#include <ArduinoLog.h>

Menu::Menu(U8X8_SH1106_128X64_NONAME_HW_I2C *display, RCStick *sticks)
{
	pDisplay = display;
	menuCursorY=(unsigned char) FIRST_MENU_LINE;
	currentMenuNumberOfEntries=0;
	currentMenuId=0;
	mySticks=sticks;

}

Menu::~Menu() {
	// TODO Auto-generated destructor stub

}



int Menu::debugDisplayAddress()
{
	return &pDisplay;
}

/**
 * moves the menu corser 1 line up, but stay in bounds
 */
void Menu::moveCursorUp()
{
	if (menuCursorY > FIRST_MENU_LINE)
	{
		menuCursorY--;
		printMenuCursor();
	}
	Log.verbose(F("y after press: %d, number of elements: %d"), menuCursorY, currentMenuNumberOfEntries);
}


/**
 * moves the menu corser 1 line down, but stay in bounds
 */
void Menu::moveCursorDown()
{
	if ((menuCursorY < DISPLAY_NUMBER_OF_LINES -1 ) && (menuCursorY < 2+ currentMenuNumberOfEntries - 1))
	{
		menuCursorY++;
		printMenuCursor();
	}
	Log.verbose(F("y after press: %d, number of elements: %d"), menuCursorY, currentMenuNumberOfEntries);
}

/**
 * draw MenuCursor
 */
void Menu::printMenuCursor ()
{
	for (unsigned char i=2; i< DISPLAY_NUMBER_OF_LINES; i++)
	{

		pDisplay->setCursor(0, i);
		if (i != menuCursorY)
		{
			pDisplay->print(' ');
		}
		else
		{
			pDisplay->print('>');
		}
	}
}

void Menu::printHeadline(char *headline)
{
	pDisplay->setCursor(0,0);
	pDisplay->print(headline);
}


/**
 * prints the trim menu for all analog sticks without values.
 */
void Menu::printMenuTrimStatic()
{
	pDisplay->clear();
	printHeadline("Trim");

	for (unsigned char element=0;element<(unsigned char) HW_NUMBER_OF_STICKS; element++)
	{
		pDisplay->setCursor(2, FIRST_MENU_LINE + element*2);
		//pDisplay.print("S" + (char) element + " X");

		pDisplay->setCursor(2, FIRST_MENU_LINE + element*2 +1);
		//pDisplay.print("S" + (char) element + " Y");
	}

	menuCursorY = FIRST_MENU_LINE;
	printMenuCursor();
}

/**
 * prints the values for the trim menu.
 */
void Menu::printMenuTrimValues(signed char *values)
{

}


/**
 * prints a single value at a specific position. Alignment left (don't know how to align right).
 * Positions between x,y and x+length,y will be wiped with spaces first. Then intValue will be written at x,y
 */
void Menu::printUpdateSingleValue(unsigned char x, unsigned char y, unsigned char length, signed int intValue)
{
	pDisplay->setCursor(x, y);
	for (unsigned char i=0; i<length;i++)
	{
		pDisplay->print(" ");
	}
	pDisplay->setCursor(x, y);
	pDisplay->print(intValue);
}

/**
 * returns the id of the selected menu element
 */
unsigned char Menu::getMenuElementId()
{
	return menuCursorY-(unsigned char) FIRST_MENU_LINE;
}


/**
 * returns the current menu id
 */
unsigned char Menu::getCurrentMenuId()
{
	return currentMenuId;
}

unsigned char Menu::getMenuLevelUpId()
{
	switch (currentMenuId)
	{
		case MENU_ID_MAIN:
			return MENU_ID_NONE;

		case MENU_ID_AXIS_CHOOSER:
			return MENU_ID_MAIN;

		case MENU_ID_AXIS:
			return MENU_ID_AXIS_CHOOSER;

		default:
			return MENU_ID_MAIN;
	}
}

void Menu::printMenuId(unsigned char menuIdToPrint)
{
	switch (menuIdToPrint)
	{
		case MENU_ID_NONE:
			currentMenuId=MENU_ID_NONE;
			pDisplay->clearDisplay();
			break;

		case MENU_ID_MAIN:
			printMenuMain();
			break;

		case MENU_ID_AXIS_CHOOSER:
			printMenuAxisChooser();
			break;

		default:
			printMenuMain();
			break;



	}
}



void Menu::printMenuMain()
{
	currentMenuId=MENU_ID_MAIN;
	pDisplay->clear();
	printHeadline("MAIN MENU");

	unsigned char y=FIRST_MENU_LINE;
	pDisplay->setCursor(2, y);
	pDisplay->print("RECEIVER");
	y++;
	pDisplay->setCursor(2, y);
	pDisplay->print("STICKS");
	y++;
	pDisplay->setCursor(2, y);
	pDisplay->print("BUTTONS");

	currentMenuNumberOfEntries=y-FIRST_MENU_LINE+1;

	menuCursorY = FIRST_MENU_LINE;
	printMenuCursor();
}
/**
 * prints all available sticks
 */
void Menu::printMenuAxisChooser()
{
	currentMenuId=MENU_ID_AXIS_CHOOSER;

	pDisplay->clear();
	printHeadline("CHOOSE AXIS");

	for (unsigned char element=0;element<(unsigned char) HW_NUMBER_OF_STICKS; element++)
	{
		pDisplay->setCursor(2, FIRST_MENU_LINE + element*2);
		pDisplay->print("STICK ");
		pDisplay->print(element);
		pDisplay->print("X");

		pDisplay->setCursor(2, FIRST_MENU_LINE + element*2 +1);
		pDisplay->print("STICK ");
		pDisplay->print(element);
		pDisplay->print("Y");
	}

	menuCursorY = FIRST_MENU_LINE;
	currentMenuNumberOfEntries=(unsigned char) HW_NUMBER_OF_STICKS * 2 ;
	printMenuCursor();
}


/**
 * calculates the menu id based on menuCursorUserY and FIRST_MENU_LINE
 */
unsigned char Menu::getSelectedMenuEntryId()
{
	return menuCursorY - (unsigned char) FIRST_MENU_LINE;
}

/**
 * prints the submenu, base on the current cursor position and current menu
 */
void Menu::printSubmenu()
{
	switch (currentMenuId)
	{
		case MENU_ID_MAIN:
			switch (getSelectedMenuEntryId())
			{
				case 0:
					printMenuReceiver();
					Log.verbose(F("Menu: call printMenuReceiver\n"));
					break;
				case 1:
					printMenuAxisChooser();
					Log.verbose(F("Menu: call printMenuStickChooser\n"));
					break;
				case 2:
					printMenuButtons();
					Log.verbose(F("Menu: MENU_ID_MAIN -> call printMenuButtons\n"));
					break;
			}
		break;
		case MENU_ID_AXIS_CHOOSER:
		{
			// a stick has two axis
			printMenuAxisDetails(getSelectedMenuEntryId());
			Log.verbose(F("Menu: MENU_ID_AXIS_CHOOSER -> call printMenuAxisDetails with id %d \n"),getSelectedMenuEntryId());
			break;


		}
	}
}


void Menu::printMenuReceiver()
{
	currentMenuId=MENU_ID_RECEIVER;
	pDisplay->clear();
	printHeadline("RECEIVER");
	currentMenuNumberOfEntries=0;
}

void Menu::printMenuButtons()
{
	currentMenuId=MENU_ID_BUTTONS;
	pDisplay->clear();
	printHeadline("BUTTONS");
	currentMenuNumberOfEntries=0;
}

/**
 * shows details to an axis
 */
void Menu::printMenuAxisDetails(unsigned char axisMenuId)
{
	currentMenuId=MENU_ID_AXIS;
	pDisplay->clear();
	unsigned char axisId = 0;


	// dirty workaround
	// this part needs to be generalized. But How do I modify the stings / chars
	if ((unsigned char) axisMenuId /2 == 0)
	{
		if (axisId %2 == 0)
		{
			printHeadline("STICK 0X");
		}
		else
		{
			printHeadline("STICK 0Y");
		}
	}
	else
	{
		if ((unsigned char) axisMenuId /2 == 1)
		{
			if (axisId %2 == 0)
			{
				printHeadline("STICK 1X");
			}
			else
			{
				printHeadline("STICK 1Y");
			}
		}


	}


	unsigned char y=FIRST_MENU_LINE;
	pDisplay->setCursor(MENU_COL_0_X_POS, MENU_AXIS_DEAD_POS);
	pDisplay->print("DEAD: ");
	printUpdateSingleValue(MENU_AXIS_VALUE_X_POS, MENU_COL_0_X_POS, 4, mySticks[axisMenuId /2].getAxis(axisMenuId %2)->getDeadZone());

	pDisplay->setCursor(MENU_COL_0_X_POS, MENU_AXIS_TRIM_POS);
	pDisplay->print("TRIM:");
	printUpdateSingleValue(MENU_AXIS_VALUE_X_POS, MENU_COL_0_X_POS, 4, mySticks[axisMenuId /2].getAxis(axisMenuId %2)->getTrim());

	pDisplay->setCursor(MENU_COL_0_X_POS, MENU_AXIS_EXP_POS);
	pDisplay->print(" EXP:");
	printUpdateSingleValue(MENU_AXIS_VALUE_X_POS, MENU_COL_0_X_POS, 4, mySticks[axisMenuId /2].getAxis(axisMenuId %2)->getExpo());

	pDisplay->setCursor(MENU_COL_0_X_POS, MENU_AXIS_EXP_REVERSE);
	pDisplay->print(" EXP:");
	printUpdateSingleValue(MENU_AXIS_VALUE_X_POS, MENU_AXIS_EXP_REVERSE, 4, mySticks[axisMenuId /2].getAxis(axisMenuId %2)->getReverse());
	menuCursorY = FIRST_MENU_LINE;
	currentMenuNumberOfEntries=3;
	printMenuCursor();

}


