/*
 * EepromAddress.c
 *
 *  Created on: Jun 26, 2021
 *      Author: developer
 */


#include "TaskCore.h"
#include "EepromAddress.h"
#include "stdint.h"
#include "TaskRF.h"
#include "SignalProcessing.h"


static uint8_t EA_GetMostLikely_U8(uint8_t val1, uint8_t val2, uint8_t val3);
static uint32_t EA_GetMostLikely_U32(uint32_t val1, uint32_t val2, uint32_t val3);

/**
 *
 */
void EA_SetSystemState(eStateCoreSystem state)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_MACHINE_STATE, (uint8_t) state);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_MACHINE_STATE, (uint8_t) state);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_MACHINE_STATE, (uint8_t) state);
	EepromStop();
}

/**
 *
 */
eStateCoreSystem EA_GetSystemState(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_MACHINE_STATE,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_MACHINE_STATE,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_MACHINE_STATE,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);

	return (eStateCoreSystem) read[0];
}


/**
 *
 */
void EA_SetRfFreq(uint32_t freq)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,EEPROM_RF_FREQUENCY, freq);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,SH1_EEPROM_RF_FREQUENCY, freq);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,SH2_EEPROM_RF_FREQUENCY, freq);
	EepromStop();
}


/**
 * vraci f zaokrouhlenou na khz
 */
uint32_t EA_GetRfFreq(void)
{
	uint32_t read[3];
	uint32_t temp32;

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_RF_FREQUENCY,4);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_RF_FREQUENCY,4);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_RF_FREQUENCY,4);
	EepromStop();

	read[0] = EA_GetMostLikely_U32(read[0], read[1], read[2]);

	if (read[0] < ALLOWED_RF_FREQ_MIN) read[0] = 869525000;
	else if (read[0] > ALLOWED_RF_FREQ_MAX) read[0] = 869525000;

	// zaokrohlime
	temp32 =(read[0]%1000);
	read[0] = read[0]/1000;
	read[0] = read[0]*1000;
	if(temp32>=500)
	{
		read[0] = read[0]+1000;
	}

	return  read[0];
}
/**
 *
 */
uint32_t EA_GetSystemMAC(void)
{
	uint32_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_MY_TX_EUI,4);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_MY_TX_EUI,4);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_MY_TX_EUI,4);
	EepromStop();

	read[0] = EA_GetMostLikely_U32(read[0], read[1], read[2]);
	return read[0];
}

/**
 *
 */
eSelectedDog EA_GetSelectedDog(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_SELECTED_DOG,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_SELECTED_DOG,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_SELECTED_DOG,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);

	return (eSelectedDog) read[0];

}

/**
 *
 */
void EA_SetSelectedDog(eSelectedDog dog)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_SELECTED_DOG, (uint8_t)dog);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_SELECTED_DOG, (uint8_t)dog);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_SELECTED_DOG, (uint8_t)dog);
	EepromStop();
}


/**
 *
 */
void EA_SetShockA(uint8_t shock)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_DOG_A_SHOCK, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_DOG_A_SHOCK, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_DOG_A_SHOCK, shock);
	EepromStop();
}

/**
 *
 */
uint8_t EA_GetShockA(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_DOG_A_SHOCK,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_DOG_A_SHOCK,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_DOG_A_SHOCK,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);

	return read[0];
}


/**
 *
 */
void EA_SetShockB(uint8_t shock)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_DOG_B_SHOCK, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_DOG_B_SHOCK, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_DOG_B_SHOCK, shock);
	EepromStop();
}

/**
 *
 */
uint8_t EA_GetShockB(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_DOG_B_SHOCK,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_DOG_B_SHOCK,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_DOG_B_SHOCK,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);
	return read[0];
}

#if (VER_4DOGS == 1)

/**
 *
 */
void EA_SetShockC(uint8_t shock)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_DOG_C_SHOCK, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_DOG_C_SHOCK, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_DOG_C_SHOCK, shock);
	EepromStop();
}

/**
 *
 */
uint8_t EA_GetShockC(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_DOG_C_SHOCK,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_DOG_C_SHOCK,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_DOG_C_SHOCK,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);
	return read[0];
}

/**
 *
 */
void EA_SetShockD(uint8_t shock)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_DOG_D_SHOCK, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_DOG_D_SHOCK, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_DOG_D_SHOCK, shock);
	EepromStop();
}

/**
 *
 */
uint8_t EA_GetShockD(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_DOG_D_SHOCK,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_DOG_D_SHOCK,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_DOG_D_SHOCK,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);
	return read[0];
}

/**
 *
 */
void EA_SetShockE(uint8_t shock)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_DOG_E_SHOCK, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_DOG_E_SHOCK, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_DOG_E_SHOCK, shock);
	EepromStop();
}

/**
 *
 */
uint8_t EA_GetShockE(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_DOG_E_SHOCK,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_DOG_E_SHOCK,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_DOG_E_SHOCK,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);
	return read[0];
}

/**
 *
 */
void EA_SetShockF(uint8_t shock)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_DOG_F_SHOCK, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_DOG_F_SHOCK, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_DOG_F_SHOCK, shock);
	EepromStop();
}

/**
 *
 */
uint8_t EA_GetShockF(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_DOG_F_SHOCK,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_DOG_F_SHOCK,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_DOG_F_SHOCK,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);
	return read[0];
}
#endif

/**
 *
 */
void EA_SetBoosterA(uint8_t shock)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_DOG_A_SHOCK_BOOSTER, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_DOG_A_SHOCK_BOOSTER, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_DOG_A_SHOCK_BOOSTER, shock);
	EepromStop();
}

/**
 *
 */
uint8_t EA_GetBoosterA(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_DOG_A_SHOCK_BOOSTER,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_DOG_A_SHOCK_BOOSTER,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_DOG_A_SHOCK_BOOSTER,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);
	return read[0];
}




/**
 *
 */
void EA_SetBoosterB(uint8_t shock)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_DOG_B_SHOCK_BOOSTER, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_DOG_B_SHOCK_BOOSTER, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_DOG_B_SHOCK_BOOSTER, shock);
	EepromStop();
}

/**
 *
 */
uint8_t EA_GetBoosterB(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_DOG_B_SHOCK_BOOSTER,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_DOG_B_SHOCK_BOOSTER,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_DOG_B_SHOCK_BOOSTER,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);
	return read[0];
}

#if (VER_4DOGS == 1)
/**
 *
 */
void EA_SetBoosterC(uint8_t shock)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_DOG_C_SHOCK_BOOSTER, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_DOG_C_SHOCK_BOOSTER, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_DOG_C_SHOCK_BOOSTER, shock);
	EepromStop();
}

/**
 *
 */
uint8_t EA_GetBoosterC(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_DOG_C_SHOCK_BOOSTER,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_DOG_C_SHOCK_BOOSTER,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_DOG_C_SHOCK_BOOSTER,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);

	return read[0];
}

/**
 *
 */
void EA_SetBoosterD(uint8_t shock)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_DOG_D_SHOCK_BOOSTER, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_DOG_D_SHOCK_BOOSTER, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_DOG_D_SHOCK_BOOSTER, shock);
	EepromStop();
}

/**
 *
 */
uint8_t EA_GetBoosterD(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_DOG_D_SHOCK_BOOSTER,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_DOG_D_SHOCK_BOOSTER,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_DOG_D_SHOCK_BOOSTER,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);

	return read[0];
}


/**
 *
 */
void EA_SetBoosterE(uint8_t shock)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_DOG_E_SHOCK_BOOSTER, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_DOG_E_SHOCK_BOOSTER, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_DOG_E_SHOCK_BOOSTER, shock);
	EepromStop();
}

/**
 *
 */
uint8_t EA_GetBoosterE(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_DOG_E_SHOCK_BOOSTER,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_DOG_E_SHOCK_BOOSTER,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_DOG_E_SHOCK_BOOSTER,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);

	return read[0];
}

/**
 *
 */
void EA_SetBoosterF(uint8_t shock)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_DOG_F_SHOCK_BOOSTER, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_DOG_F_SHOCK_BOOSTER, shock);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_DOG_F_SHOCK_BOOSTER, shock);
	EepromStop();
}

/**
 *
 */
uint8_t EA_GetBoosterF(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_DOG_F_SHOCK_BOOSTER,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_DOG_F_SHOCK_BOOSTER,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_DOG_F_SHOCK_BOOSTER,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);

	return read[0];
}


#endif


#if (NC_MODE_AVAILABLE == 1)
/**
 *
 */
void EA_SetMode(uint8_t dog, eMode mode)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_DOG_A_MODE + dog*sizeof(uint32_t), (uint8_t) mode);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_DOG_A_MODE+ dog*sizeof(uint32_t), (uint8_t) mode);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_DOG_A_MODE+ dog*sizeof(uint32_t), (uint8_t) mode);
	EepromStop();
}


/**
 *
 */
eMode EA_GetMode(uint8_t dog)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_DOG_A_MODE+ dog*sizeof(uint32_t),1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_DOG_A_MODE+ dog*sizeof(uint32_t),1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_DOG_A_MODE+ dog*sizeof(uint32_t),1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);

	return (eMode) (read[0]);
}

/**
 *
 */
uint8_t EA_GetNumOfDogs(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_NUMBER_OF_DOGS,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_NUMBER_OF_DOGS,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_NUMBER_OF_DOGS,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);

	return read[0];
}

/**
 *
 */
uint8_t EA_GetFeedbackON(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)EEPROM_FEEDBACK_ON,1);
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_FEEDBACK_ON,1);
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_FEEDBACK_ON,1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);

	return read[0];
}

/**
 *
 */
void EA_SetFeedbackON(uint8_t value)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_FEEDBACK_ON, value);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_FEEDBACK_ON, value);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_FEEDBACK_ON, value);
	EepromStop();
}

/**
 *
 */
void EA_SetNumOfDogs(uint8_t numberOfDogs)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_NUMBER_OF_DOGS, (uint8_t) numberOfDogs);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_NUMBER_OF_DOGS, (uint8_t) numberOfDogs);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_NUMBER_OF_DOGS, (uint8_t) numberOfDogs);
	EepromStop();
}


#endif // mode nc available
/**
 *
 */
void EA_SetLockActive(eSelectedDog dog ,bool lockActive)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_LOCK_ACTIVE_A + dog*sizeof(uint32_t), (uint8_t) lockActive);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_LOCK_ACTIVE_A + dog*sizeof(uint32_t), (uint8_t) lockActive);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_LOCK_ACTIVE_A + dog*sizeof(uint32_t), (uint8_t) lockActive);
	EepromStop();
}

/**
 *
 */
bool EA_GetLockActive(eSelectedDog dog)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *) (EEPROM_LOCK_ACTIVE_A + dog*sizeof(uint32_t)),1);
	memcpy(&read[1],(uint8_t *) (SH1_EEPROM_LOCK_ACTIVE_A + dog*sizeof(uint32_t)),1);
	memcpy(&read[2],(uint8_t *) (SH2_EEPROM_LOCK_ACTIVE_A + dog*sizeof(uint32_t)),1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[3]);

	return (bool) (read[0]);
}

/**
 *
 */
void EA_SetSystemMAC(uint32_t mac)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,EEPROM_MY_TX_EUI,mac);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,SH1_EEPROM_MY_TX_EUI,mac);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,SH2_EEPROM_MY_TX_EUI,mac);
	EepromStop();
}

/**
 *
 */
bool EA_GetLockBtmKeys(void)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy(&read[0],(uint8_t *) (EEPROM_LOCK_BTM_KEYS),1);
	memcpy(&read[1],(uint8_t *) (SH1_EEPROM_LOCK_BTM_KEYS),1);
	memcpy(&read[2],(uint8_t *) (SH2_EEPROM_LOCK_BTM_KEYS),1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1],read[2]);

	return (bool) (read[0]);
}

/**
 *
 */
void EA_SetLockBtmKeys(bool locked)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,EEPROM_LOCK_BTM_KEYS,locked);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,SH1_EEPROM_LOCK_BTM_KEYS,locked);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,SH2_EEPROM_LOCK_BTM_KEYS,locked);
	EepromStop();
}

/**
 *
 */
void EA_IncResetCounter()
{
	uint32_t counter;

	EepromStart(true);

	memcpy(&counter,(uint8_t *)EEPROM_RESET_COUNTER,sizeof(counter));

	if (counter<NUMBER_OF_RESET_MAX)
	{
		counter++;
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,EEPROM_RESET_COUNTER,counter);
	}

	EepromStop();
}

/**
 *
 */
void EA_ClearResetCounter()
{
	uint32_t counter;

	EepromStart(true);

	memcpy(&counter,(uint8_t *)EEPROM_RESET_COUNTER,sizeof(counter));

	if (counter>0)
	{
		counter=0;
		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,EEPROM_RESET_COUNTER,counter);
	}

	EepromStop();
}

/**
 *
 */
bool EA_IsResetCounterFull()
{
	uint32_t counter;

	EepromStart(false);
	memcpy(&counter,(uint8_t *)EEPROM_RESET_COUNTER,sizeof(counter));
	EepromStop();

	return counter==NUMBER_OF_RESET_MAX?true:false;
}

#if ASCENDING_MODE_ENABLED == true
void EA_SetAscendingMode(uint8_t dog, bool ascending)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_ASCENDING_1+dog*sizeof(uint32_t), ascending);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_ASCENDING_1+dog*sizeof(uint32_t), ascending);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_ASCENDING_1+dog*sizeof(uint32_t), ascending);
	EepromStop();
}

/**
 *
 */
bool EA_GetAscendingMode(uint8_t dog)
{
	uint8_t read[3];

	EepromStart(false);
	memcpy((uint8_t*)&read[0],(uint8_t *)(EEPROM_ASCENDING_1+dog*sizeof(uint32_t)),1);
	memcpy((uint8_t*)&read[1],(uint8_t *)(SH1_EEPROM_ASCENDING_1+dog*sizeof(uint32_t)),1);
	memcpy((uint8_t*)&read[2],(uint8_t *)(SH2_EEPROM_ASCENDING_1+dog*sizeof(uint32_t)),1);
	EepromStop();

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[2]);
	return (bool) read[0];
}
#endif

#if (VERSION_WITH_RING == 1)

void EA_SaveRing(uint8_t ring,uint8_t selectedDog, uint8_t function, uint32_t mac)
{
	uint32_t addr;

	addr = EEPROM_RING_1_MAC + (ring-1)*3*sizeof(uint32_t);
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,addr, mac);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,addr+sizeof(uint32_t), selectedDog);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,addr+2*sizeof(uint32_t), function);

	addr = SH1_EEPROM_RING_1_MAC + (ring-1)*3*sizeof(uint32_t);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,addr, mac);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,addr+sizeof(uint32_t), selectedDog);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,addr+2*sizeof(uint32_t), function);

	addr = SH2_EEPROM_RING_1_MAC + (ring-1)*3*sizeof(uint32_t);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,addr, mac);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,addr+sizeof(uint32_t), selectedDog);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,addr+2*sizeof(uint32_t), function);

	EepromStop();
}

/*
 *
 */
uint8_t EA_GetRingBtnFction(uint8_t ring)
{
	uint32_t addr;
	uint8_t read[3];
	EepromStart(false);

	addr = EEPROM_RING_1_FCE + (ring-1)*3*sizeof(uint32_t);

	memcpy(&read[0],(uint8_t *)addr,sizeof(uint8_t));
	addr = SH1_EEPROM_RING_1_FCE + (ring-1)*3*sizeof(uint32_t);
	memcpy(&read[1],(uint8_t *)addr,sizeof(uint8_t));
	addr = SH2_EEPROM_RING_1_FCE + (ring-1)*3*sizeof(uint32_t);
	memcpy(&read[2],(uint8_t *)addr,sizeof(uint8_t));

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[3]);

	EepromStop();

	return read[0] ;

}

/*
 *
 */
uint8_t EA_GetRingDogID(uint8_t ring)
{
	uint32_t addr;
	uint8_t read[3];

	EepromStart(false);

	addr = EEPROM_RING_1_DOG + (ring-1)*3*sizeof(uint32_t);
	memcpy(&read[0],(uint8_t *)addr,sizeof(uint8_t));
	addr = SH1_EEPROM_RING_1_DOG + (ring-1)*3*sizeof(uint32_t);
	memcpy(&read[1],(uint8_t *)addr,sizeof(uint8_t));
	addr = SH2_EEPROM_RING_1_DOG + (ring-1)*3*sizeof(uint32_t);
	memcpy(&read[2],(uint8_t *)addr,sizeof(uint8_t));

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[3]);
	EepromStop();

	return read[0] ;

}

/*
 *
 */
void EA_EraseRingMAC( uint8_t ring,uint32_t mac)
{
	uint32_t addr;
	addr = EEPROM_RING_1_MAC + (ring-1)*3*sizeof(uint32_t);

	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,addr, mac);
	addr = SH1_EEPROM_RING_1_MAC + (ring-1)*3*sizeof(uint32_t);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,addr, mac);
	addr = SH2_EEPROM_RING_1_MAC + (ring-1)*3*sizeof(uint32_t);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD,addr, mac);
	EepromStop();
}


uint32_t EA_GetRingMAC(uint8_t ring )
{
	uint32_t addr;
	uint32_t read[3];

	addr = EEPROM_RING_1_MAC + (ring-1)*3*sizeof(uint32_t);

	EepromStart(false);
	memcpy(&read[0],(uint8_t *)addr,sizeof(uint32_t));
	addr = SH1_EEPROM_RING_1_MAC + (ring-1)*3*sizeof(uint32_t);
	memcpy(&read[1],(uint8_t *)addr,sizeof(uint32_t));
	addr = SH2_EEPROM_RING_1_MAC + (ring-1)*3*sizeof(uint32_t);
	memcpy(&read[2],(uint8_t *)addr,sizeof(uint32_t));
	EepromStop();

	read[0] = EA_GetMostLikely_U32(read[0], read[1], read[3]);

	return read[0] ;
}


void EA_SetNordicState(bool state)
{
	EepromStart(true);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,EEPROM_NORDIC_STATE,(uint8_t) state);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH1_EEPROM_NORDIC_STATE,(uint8_t) state);
	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,SH2_EEPROM_NORDIC_STATE,(uint8_t) state);
	EepromStop();
}


bool EA_GetNordicState(void)
{
	bool read[3];

	memcpy(&read[0],(uint8_t *)EEPROM_NORDIC_STATE,sizeof(bool));
	memcpy(&read[1],(uint8_t *)SH1_EEPROM_NORDIC_STATE,sizeof(bool));
	memcpy(&read[2],(uint8_t *)SH2_EEPROM_NORDIC_STATE,sizeof(bool));

	read[0] = EA_GetMostLikely_U8(read[0], read[1], read[3]);

	return read[0] ;

}

#endif

/**
 *
 */
static uint8_t EA_GetMostLikely_U8(uint8_t val1, uint8_t val2, uint8_t val3)
{
	if(val1 == val2)	return val1;

	if(val1 == val3)	return val1;

	if(val2 == val3)	return val2;

	return val1;

}



/**
 *
 */
static uint32_t EA_GetMostLikely_U32(uint32_t val1, uint32_t val2, uint32_t val3)
{
	if(val1 == val2)	return val1;

	if(val1 == val3)	return val1;

	if(val2 == val3)	return val2;

	return val1;

}



