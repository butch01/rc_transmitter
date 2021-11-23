/*
 * Config.h
 *
 *  Created on: 25.10.2021
 *      Author: butch
 */

#ifndef RCCONFIG_H_
#define RCCONFIG_H_


#include <arduino.h>
#include "RCStick.h"

// model store
#define CONFIG_MODEL_NAME_MAX_LENGTH 10
#define CONFIG_MODEL_COUNT 4
#define EEPROM_CONFIG_MODEL_STATUS_START_POSITION 50
#define EEPROM_CONFIG_MODEL_STATUS_STATUS_SUCCESSFULLY_SAVED 1	// if status is one, use saved config from eeprom, otherwise use defaults
#define EEPROM_CONFIG_MODEL_STATUS_STATUS_IGNORE_SAVED 255 // same as default
#define EEPROM_CONFIG_START_POSITION_OF_FIRST_MODEL 200 //

#define EEPROM_LAST_CURRENT_MODEL_ID 0	// saves the last current Model Id

/**
 * configuration is stored in eeprom in the following way:
 *
 * model name (CONFIG_MODEL_NAME_MAX_LENGTH chars)
 * axis configuration (trim, deadzone, expo, limit min, limit max)
 *
 *
 * all models in loop
 */


class RCConfig {
public:
	RCConfig();
	virtual ~RCConfig();

	void loadModelConfig(); // loads the model config from eeprom and puts updates the current configuration objects like rcstick, ...
	void saveModelConfig(); // saves the model config to eeprom persistently
	void init(RCStick *sticks); // give pointer to RCStick
	bool isModelConfigSuccessfullySaved(uint8_t modelId); // checks if a specific model config is marked as successfully saved in eeprom
	bool isCurrentModelConfigSuccessfullySaved(); // checks if the current model is marked as successfully saved in eeprom
	void markModelConfigAsIgnored(unsigned char modelId);
	void markCurrentModelConfigAsIgnored();

	void switchToModel(uint8_t modelId); // sets current model Id and loads the config
	uint8_t getLastModelId(); // saves the last active ModelId



private:
	unsigned char currentModelId; // id of the configuration which is selected and active

//	// currently with static naming. Will be changeable and stored / read from eeprom later
	char configModelNames[CONFIG_MODEL_NAME_MAX_LENGTH][CONFIG_MODEL_COUNT];

	void readConfigModelNamesFromEEPROM(); // reads the Model list from eeprom

	struct axisData {
		signed char 	axisTrim;
		unsigned char 	axisDeadZone;
		float 			axisExpo;
		bool			axisIsReverse;
		unsigned char 	axisLimitMin;
		unsigned char	axisLimitMax;
	};

	uint16_t calculateEEPROMStartOfCurrentModel(); // get eeprom address of the start of the current model
	uint16_t calculateEEPROMStartOfModel(unsigned char modelId); // get eeprom addess of the start of a specific model id
	uint16_t calculateEEPROMAxisId(unsigned char stickId, unsigned char axisId); // returns the eeprom id of the specified axis
	void updateLastCurrentModelId(); // updates the current modelId in eeprom. So the last selected model will load automatically

	RCStick *mySticks; // pointer to sticks
};

#endif /* RCCONFIG_H_ */
