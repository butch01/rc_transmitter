/*
 * Config.cpp
 *
 *  Created on: 25.10.2021
 *      Author: butch
 */

#include "RCConfig.h"

#include <Arduino.h>
#include <EEPROM.h> // to read and write to eeprom
#include "hw_config.h" // to have information about number of sticks available. Infos for all Sticks needs to be stored
#include "defaultValues.h"



RCConfig::RCConfig() {
	currentModelId = DEFAULT_CONFIG_MODEL_ID;
	memset(configModelNames,0,sizeof(configModelNames));

	// currently static setting of model names
//	configModelNames={'Bully','Porsche','K989','Lambo'};
	readConfigModelNamesFromEEPROM();

}

RCConfig::~RCConfig() {
	// TODO Auto-generated destructor stub
}


/**
 * returns true if a specific model config is marked as EEPROM_CONFIG_MODEL_STATUS_STATUS_SUCCESSFULLY_SAVED, otherwise false
 */
bool RCConfig::isModelConfigSuccessfullySaved(uint8_t modelId)
{
	if (EEPROM.read(EEPROM_CONFIG_MODEL_STATUS_START_POSITION + modelId) == EEPROM_CONFIG_MODEL_STATUS_STATUS_SUCCESSFULLY_SAVED)
	{
		return true;
	}
	else
	{
		return false;
	}
}


/**
 * returns true if the CURRENT model config is marked as EEPROM_CONFIG_MODEL_STATUS_STATUS_SUCCESSFULLY_SAVED, otherwise false
 */
bool RCConfig::isCurrentModelConfigSuccessfullySaved()
{
	return isModelConfigSuccessfullySaved(currentModelId);
}



/**
 * reads the model names from eeprom
 */
void RCConfig::readConfigModelNamesFromEEPROM()
{

	// iterate through models
	for (unsigned char model=0; model< CONFIG_MODEL_COUNT; model++)
	{
		if (isModelConfigSuccessfullySaved(model))
		{
			uint16_t eepromModelStart = calculateEEPROMStartOfModel(model);
			for (unsigned char c=0; c<CONFIG_MODEL_NAME_MAX_LENGTH; c++)
			{
				// read model name byte by byte
				configModelNames[model][c]=EEPROM.read(eepromModelStart + c);
			}
		}
		else
		{
			configModelNames[model][0] = "Model ";
			configModelNames[model][6] = model + 48; // 48 is the ascii offset for numbers. decimal 0 = ascii 48
		}
	}
}

/**
 * reads the current model configuration (all axis, etc.) and saves it to the objects like rcstick, etc.
 */
void RCConfig::loadModelConfig()
{
	// get the start of the current model configuration
	uint16_t eepromStart = calculateEEPROMStartOfCurrentModel();

	// iterate over sticks
	for (unsigned char stick=0; stick < HW_NUMBER_OF_STICKS; stick++)
	{
		// 2 axes per stick
		for (unsigned char axis=0; axis<2; axis++)
		{
			// read from eeprom
			axisData myAxisData;

			// if model is marked as saved in eeprom, read from there, otherwise use defaults.
			if (isCurrentModelConfigSuccessfullySaved())
			{
				// read data from eeprom
				EEPROM.get(calculateEEPROMAxisId(stick, axis), myAxisData);
			}
			else
			{
				// use default data
				myAxisData.axisDeadZone = DEFAULT_DEAD_ZONE;
				myAxisData.axisExpo = DEFAULT_EXPO;
				myAxisData.axisIsReverse = DEFAULT_REVERSE;
				myAxisData.axisTrim = DEFAULT_TRIM;
				myAxisData.axisLimitMax = DEFAULT_LIMIT_MAX;
				myAxisData.axisLimitMin = DEFAULT_LIMIT_MIN;
			}

			// assign to configuration
			// assign trim
			mySticks[stick].getAxis(axis)->setTrim(myAxisData.axisTrim);

			// assign deadZone
			mySticks[stick].getAxis(axis)->setDeadZone(myAxisData.axisDeadZone);

			// assign reverse
			mySticks[stick].getAxis(axis)->setReverse(myAxisData.axisIsReverse);

			// assign expo
			mySticks[stick].getAxis(axis)->setExpo(myAxisData.axisExpo);

		}
	}
}


/**
 * saves the current model configuration to EEPROM
 */
void RCConfig::saveModelConfig()
{
	// get the start of current model name in eeprom
	uint16_t eepromStart = calculateEEPROMStartOfCurrentModel();
	for (unsigned char i=0; i< CONFIG_MODEL_NAME_MAX_LENGTH; i++)
	{
		// update name of the model, char by char. Using update to have minimal EEPROM writes.
		EEPROM.update(eepromStart + i, configModelNames[currentModelId][i]);
	}


	// iterate over sticks
	for (unsigned char stick=0; stick < HW_NUMBER_OF_STICKS; stick++)
	{
		// 2 axes per stick
		for (unsigned char axis=0; axis<2; axis++)
		{
			// read from eeprom
			axisData myAxisData;

			// copy current configuration to struct to prepare saving to eeprom
			myAxisData.axisTrim 		= mySticks[stick].getAxis(axis)->getTrim();
			myAxisData.axisDeadZone 	= mySticks[stick].getAxis(axis)->getDeadZone();
			myAxisData.axisIsReverse 	= mySticks[stick].getAxis(axis)->getReverse();
			myAxisData.axisExpo 		= mySticks[stick].getAxis(axis)->getExpo();
			myAxisData.axisLimitMin 	= mySticks[stick].getAxis(axis)->getLimitMin();
			myAxisData.axisLimitMax 	= mySticks[stick].getAxis(axis)->getLimitMax();

			// save values to eeprom
			EEPROM.put(calculateEEPROMAxisId(stick, axis), myAxisData);
		}
	}

	// mark model as saved in eeprom
	EEPROM.update(EEPROM_CONFIG_MODEL_STATUS_START_POSITION + currentModelId, EEPROM_CONFIG_MODEL_STATUS_STATUS_SUCCESSFULLY_SAVED);
}


/**
 * marks a config of a specific model to be ignored in eeprom. Then the default is loaded. (reset model)
 */
void RCConfig::markModelConfigAsIgnored(unsigned char modelId)
{
	EEPROM.update(EEPROM_CONFIG_MODEL_STATUS_START_POSITION + modelId, EEPROM_CONFIG_MODEL_STATUS_STATUS_IGNORE_SAVED);
}


/**
 * marks a config of current model to be ignored in eeprom. Then the default is loaded. (reset model)
 */
void RCConfig::markCurrentModelConfigAsIgnored()
{
	markModelConfigAsIgnored(currentModelId);
}




/**
 * initializes the config. Binding sticks to config object.
 */
void RCConfig::init(RCStick *sticks)
{
	mySticks = sticks;
}

/**
 * you need to set currentModelId first
 */
uint16_t RCConfig::calculateEEPROMAxisId(unsigned char stickId, unsigned char axisId)
{
	// ToDo: do I need to add 1?
	return (calculateEEPROMStartOfCurrentModel() + CONFIG_MODEL_NAME_MAX_LENGTH + sizeof(axisData) * axisId + sizeof(axisData) * stickId * 2);
}

/**
 * returns the eeprom start of the current model
 */
uint16_t RCConfig::calculateEEPROMStartOfCurrentModel()
{
	return calculateEEPROMStartOfModel(currentModelId);

}

/**
 * returns the eeprom start of a specific model id
 */
uint16_t RCConfig::calculateEEPROMStartOfModel(unsigned char modelId)
{
	/**
	 * description of formula:
	 * 		1 stick has 2 axis
	 * 		HW_NUMBER_OF_STICKS defines how many sticks are available in the current hardware
	 * 		Length of static model name is defined by currentConfigIdSelected
	 *
	 */
	return (uint16_t) EEPROM_CONFIG_START_POSITION_OF_FIRST_MODEL + ( CONFIG_MODEL_NAME_MAX_LENGTH + HW_NUMBER_OF_STICKS * 2 * sizeof (axisData)) * modelId;
}

/**
 * switches current ModelConfig and loads the configuration for that model.
 */
void RCConfig::switchToModel(uint8_t modelId)
{
	currentModelId = modelId;
	loadModelConfig();
	updateLastCurrentModelId();
}

/**
 * updates the last current ModelId in eeprom
 */
void RCConfig::updateLastCurrentModelId()
{
	EEPROM.update(EEPROM_LAST_CURRENT_MODEL_ID, currentModelId);
}

/**
 * returns the value of the last current model id, if it is inside the model range. Otherwise it returns the default id.
 */
uint8_t RCConfig::getLastModelId()
{
	uint8_t lastModelId = EEPROM.read(EEPROM_LAST_CURRENT_MODEL_ID);
	if (lastModelId >=0 && lastModelId < CONFIG_MODEL_COUNT)
	{
		return lastModelId;
	}
	else
	{
		// return default model id
		return DEFAULT_CONFIG_MODEL_ID;
	}
}
