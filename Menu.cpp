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
	currentMenuNumberOfEntries = 0;
	currentMenuId = 0;
	mySticks = sticks;
	myIsEditMode = false;
	myCurrentAxisMenuId=-1;
	lastEditChangeValueStartTime=0;

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
 * no moving if in edit mode
 */
void Menu::moveCursorUp()
{

	if (!myIsEditMode)
	{

		// only do if not in edit mode
		if (menuCursorY > FIRST_MENU_LINE)
		{
			menuCursorY--;
			printMenuCursor();
		}
		Log.verbose(F("y after press: %d, number of elements: %d"), menuCursorY, currentMenuNumberOfEntries);
	}

}


/**
 * moves the menu corser 1 line down, but stay in bounds
 * no moving if in edit mode
 */
void Menu::moveCursorDown()
{
	if (!myIsEditMode)
	{

		// only do if not in edit mode
		if ((menuCursorY < DISPLAY_NUMBER_OF_LINES -1 ) && (menuCursorY < 2+ currentMenuNumberOfEntries - 1))
		{
			menuCursorY++;
			printMenuCursor();
		}
		Log.verbose(F("y after press: %d, number of elements: %d"), menuCursorY, currentMenuNumberOfEntries);
	}
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

/**
 * prints a Menu.
 * no printing if in edit mode
 */
void Menu::printMenuId(unsigned char menuIdToPrint)
{
	if (!myIsEditMode)
	{
		// do only if not in edit mode
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
 * prints the submenu or changes to edit mode, base on the current cursor position and current menu
 */
void Menu::actionOnEnter()
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
			// a stick has two axis
			printMenuAxisDetails(getSelectedMenuEntryId());
			Log.verbose(F("Menu: MENU_ID_AXIS_CHOOSER -> call printMenuAxisDetails with id %d \n"),getSelectedMenuEntryId());
			break;

		case MENU_ID_AXIS:

			if (myIsEditMode)
			{
				// disable edit mode
				myIsEditMode = false;
				pDisplay-> setInverseFont(false);

			}
			else
			{
				// enable edit mode
				myIsEditMode = true;
				pDisplay-> setInverseFont(true);
			}
			switch (getSelectedMenuEntryId())
			{
				case MENU_AXIS_DEAD_Y_POS:
					printAxisDeadzoneValue();
					break;

				case MENU_AXIS_EXPO_Y_POS:
					printAxisExpoValue();
					break;

				case MENU_AXIS_REVERSE_Y_POS:
					printAxisReverseValue();
					break;

				case MENU_AXIS_TRIM_Y_POS:
					printAxisTrimValue();
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
	myCurrentAxisMenuId = axisMenuId;


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
	pDisplay->setCursor(MENU_COL_0_X_POS, MENU_AXIS_DEAD_Y_POS + FIRST_MENU_LINE);
	pDisplay->print("DEAD: ");
	printAxisDeadzoneValue( );

	pDisplay->setCursor(MENU_COL_0_X_POS, MENU_AXIS_TRIM_Y_POS + FIRST_MENU_LINE);
	pDisplay->print("TRIM:");
	printAxisTrimValue();

	pDisplay->setCursor(MENU_COL_0_X_POS, MENU_AXIS_EXPO_Y_POS + FIRST_MENU_LINE);
	pDisplay->print(" EXP:");
	printAxisExpoValue();

	pDisplay->setCursor(MENU_COL_0_X_POS, MENU_AXIS_REVERSE_Y_POS + FIRST_MENU_LINE);
	pDisplay->print(" REV:");
	printAxisReverseValue();
	menuCursorY = FIRST_MENU_LINE;
	currentMenuNumberOfEntries=4;
	printMenuCursor();

}


/**
 * calculates the current stick id of the mySticks array.
 * returns -1 on error
 */
signed char Menu::calculateStickId()
{
	if (myCurrentAxisMenuId == -1)
	{
		return -1;
	}
	else
	{
		return (signed char) (myCurrentAxisMenuId /2);
	}
}


/**
 * calculates the current axis of of the stick in myStick array.
 * returns -1 on error.
 */
signed char Menu::calculateAxisId()
{
	if (myCurrentAxisMenuId == -1)
	{
		return -1;
	}
	else
	{
		return (signed char) (myCurrentAxisMenuId %2);
	}
}



/**
 * prints the deadzone value in axis detail menu
 */
void Menu::printAxisDeadzoneValue()
{
	printUpdateSingleValue(MENU_AXIS_VALUE_X_POS, MENU_AXIS_DEAD_Y_POS + FIRST_MENU_LINE, 4, mySticks[calculateStickId()].getAxis(calculateAxisId())->getDeadZone());
}

/**
 * prints the Trim value in axis detail menu
 */
void Menu::printAxisTrimValue()
{
	printUpdateSingleValue(MENU_AXIS_VALUE_X_POS, MENU_AXIS_TRIM_Y_POS + FIRST_MENU_LINE, 4, mySticks[calculateStickId()].getAxis(calculateAxisId())->getTrim());
}


/**
 * prints the deadzone value in axis detail menu
 */
void Menu::printAxisReverseValue()
{
	printUpdateSingleValue(MENU_AXIS_VALUE_X_POS, MENU_AXIS_REVERSE_Y_POS + FIRST_MENU_LINE, 4, mySticks[calculateStickId()].getAxis(calculateAxisId())->getReverse());
}


/**
 * prints the Expo value in axis detail menu
 */
void Menu::printAxisExpoValue()
{
	printUpdateSingleValue(MENU_AXIS_VALUE_X_POS, MENU_AXIS_EXPO_Y_POS + FIRST_MENU_LINE, 4, mySticks[calculateStickId()].getAxis(calculateAxisId())->getExpo());
}

/**
 * returns info if menu is in edit mode
 */
bool Menu::isEditMode()
{
	return myIsEditMode;
}


/**
 * - checks if edit mode is active. If so:
 * - get Value which is edited by shown menu and cursor position
 * - increment the value
 * - modifier can be set to -1 to turn the increase to a decrease (value is multiplied with -1)
 */
void Menu::editIncreaseCurrentMenuValue(signed char valueModifier=1)
{
	// do only if we are in edit mode, otherwise do nothing here
	if (myIsEditMode)
	{

		bool triggerIncrease = false;
		unsigned long currentTime = millis();

		Log.verbose(F("edit: %l - %l = %l\n"), currentTime, lastEditChangeValueLastTime, currentTime - lastEditChangeValueLastTime);

		// apply timeout
		if (currentTime - lastEditChangeValueLastTime > (unsigned long) MENU_MODIFY_TIMEOUT)
		{
			Log.verbose(F("edit: timeout -> %l\n"), MENU_MODIFY_TIMEOUT);
			// reset timer
			lastEditChangeValueStartTime = 0;
			lastEditChangeValueLastTime= 0 ;

		}

		// check update interval
		if (lastEditChangeValueStartTime == 0)
		{
			triggerIncrease = true;
			lastEditChangeValueStartTime = currentTime;
			lastEditChangeValueLastTime = currentTime;
			Log.verbose(F("edit: %l - %l = %l - trigger new\n"), currentTime, lastEditChangeValueLastTime, currentTime - lastEditChangeValueLastTime);
		}
		else
		{

			// not the first edit -> check the time to decide if we trigger the action now or not.
			if (currentTime - lastEditChangeValueStartTime < (unsigned long) MENU_MODIFY_SLOW_TIME_RANGE)
			{
				Log.verbose(F("in slow interval\n"));
				// use slow interval
				// check if we need to trigger now
				if (currentTime - lastEditChangeValueLastTime > (unsigned long) MENU_MODIFY_SLOW_INTERVAL_MS)
				{
					triggerIncrease = true;
					lastEditChangeValueLastTime = currentTime;
					Log.verbose(F("edit: %l - %l = %l - trigger slow\n"), currentTime, lastEditChangeValueLastTime, currentTime - lastEditChangeValueLastTime);
				}
			}
			else
			{
				// use fast interval
				// check if we need to trigger now
				Log.verbose(F("in fast interval\n"));
				if (currentTime - lastEditChangeValueLastTime > (unsigned long) MENU_MOFIFY_FAST_INTERVAL_MS)
				{
					triggerIncrease = true;
					lastEditChangeValueLastTime = currentTime;
					Log.verbose(F("edit: %l - %l = %l - trigger fast\n"), currentTime, lastEditChangeValueLastTime, currentTime - lastEditChangeValueLastTime);
				}
			}
		}


		// do the following only if triggerIncrease == true.
		if (triggerIncrease)
		{
			unsigned char increment = DEFAULT_INCREMENT_CHAR;
			float incrementFloat = (float) DEFAULT_INCREMENT_FLOAT;

			if (currentMenuId == MENU_ID_AXIS)
			{
				// select the field which needs to be increased
				switch (menuCursorY - (unsigned char) FIRST_MENU_LINE)
				{
					case MENU_AXIS_DEAD_Y_POS:

						mySticks[calculateStickId()].getAxis(calculateAxisId())->setDeadZone(unsignedCharLimiter((unsigned int) mySticks[calculateStickId()].getAxis(calculateAxisId())->getDeadZone() + increment * valueModifier));
						printAxisDeadzoneValue();
						Log.verbose(F("Menu::editIncreaseCurrentMenuValue, deadzone, increment: %d\n"), increment * valueModifier);
						break;

					case MENU_AXIS_EXPO_Y_POS:
						mySticks[calculateStickId()].getAxis(calculateAxisId())->setExpo(mySticks[calculateStickId()].getAxis(calculateAxisId())->getExpo() + incrementFloat * valueModifier);
						printAxisExpoValue();
						break;

					case MENU_AXIS_REVERSE_Y_POS:
						// CAUTION: this will not work if value modifier is other than 1 or -1
						if (valueModifier == 1)
						{
							mySticks[calculateStickId()].getAxis(calculateAxisId())->setReverse(true);
						}
						if (valueModifier == -1)
						{
							mySticks[calculateStickId()].getAxis(calculateAxisId())->setReverse(false);
						}
						printAxisReverseValue();
						break;

					case MENU_AXIS_TRIM_Y_POS:
						Log.verbose("%d + %d\n", (unsigned int) mySticks[calculateStickId()].getAxis(calculateAxisId())->getTrim(),  (signed int) increment * valueModifier);
						mySticks[calculateStickId()].getAxis(calculateAxisId())->setTrim(signedCharLimiter((signed int) mySticks[calculateStickId()].getAxis(calculateAxisId())->getTrim() + (signed int) increment * valueModifier));
						printAxisTrimValue();
						break;
				}
			}
		}
	}
}

/**
 * decrease the current limit
 * under the hood it calls editIncreaseCurrentValue(-1)
 */
void Menu::editDecreaseCurrentValue()
{
	editIncreaseCurrentMenuValue(-1);
}


/**
 * limits an unsigned char to values boundaries of unsigned char (0 to 255).
 * if value is out of bounds, the nearest boundary is returned.
 */
unsigned char Menu::unsignedCharLimiter(signed int unsignedCharToLimit)
{
	Log.verbose(F("Menu::unsignedCharLimiter (%d)\n"), unsignedCharToLimit);
	if (unsignedCharToLimit > 255)
	{
		return 255;
	}
	else
	{
		if (unsignedCharToLimit < 0)
		{
			return 0;
		}
		else
		{
			return unsignedCharToLimit;
		}
	}

}

/**
 * limits an unsigned char to values boundaries of unsigned char (-127 to 127).
 * if value is out of bounds, the nearest boundary is returned.
 */

signed char Menu::signedCharLimiter(signed int signedCharToLimit)
{
	if (signedCharToLimit > 127)
	{
		return 127;
	}
	else
	{
		if (signedCharToLimit < -127)
		{
			return -127;
		}
		else
		{
			return signedCharToLimit;
		}
	}
}


