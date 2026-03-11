/*
 * EepromAddress.h
 *
 *  Created on: 12. 3. 2020
 *      Author: developer
 */

#ifndef INC_EEPROMADDRESS_H_
#define INC_EEPROMADDRESS_H_

#include "cmsis_os.h"
#include "stm32l083xx.h"
#include "stdint.h"
#include "stdbool.h"
#include "TaskCore.h"


#define EEPROM_MACHINE_STATE			(DATA_EEPROM_BASE)
#define EEPROM_MY_TX_EUI				(EEPROM_MACHINE_STATE+sizeof(uint32_t))
#define EEPROM_RF_FREQUENCY				(EEPROM_MY_TX_EUI+sizeof(uint32_t))
#define EEPROM_DOG_A_SHOCK				(EEPROM_RF_FREQUENCY + sizeof(uint32_t))
#define EEPROM_DOG_A_SHOCK_BOOSTER		(EEPROM_DOG_A_SHOCK + sizeof(uint32_t))
#define EEPROM_DOG_B_SHOCK				(EEPROM_DOG_A_SHOCK_BOOSTER + sizeof(uint32_t))
#define EEPROM_DOG_B_SHOCK_BOOSTER		(EEPROM_DOG_B_SHOCK + sizeof(uint32_t))
#define EEPROM_DOG_C_SHOCK				(EEPROM_DOG_B_SHOCK_BOOSTER + sizeof(uint32_t))
#define EEPROM_DOG_C_SHOCK_BOOSTER		(EEPROM_DOG_C_SHOCK + sizeof(uint32_t))
#define EEPROM_DOG_D_SHOCK				(EEPROM_DOG_C_SHOCK_BOOSTER + sizeof(uint32_t))
#define EEPROM_DOG_D_SHOCK_BOOSTER		(EEPROM_DOG_D_SHOCK + sizeof(uint32_t))
#define EEPROM_DOG_E_SHOCK				(EEPROM_DOG_D_SHOCK_BOOSTER + sizeof(uint32_t))
#define EEPROM_DOG_E_SHOCK_BOOSTER		(EEPROM_DOG_E_SHOCK + sizeof(uint32_t))
#define EEPROM_DOG_F_SHOCK				(EEPROM_DOG_E_SHOCK_BOOSTER + sizeof(uint32_t))
#define EEPROM_DOG_F_SHOCK_BOOSTER		(EEPROM_DOG_F_SHOCK + sizeof(uint32_t))
#define EEPROM_DOG_A_MODE				(EEPROM_DOG_F_SHOCK_BOOSTER + sizeof(uint32_t))	// NICK/Continues je pouze u 2 psu
#define EEPROM_DOG_B_MODE				(EEPROM_DOG_A_MODE + sizeof(uint32_t))
#define EEPROM_DOG_C_MODE				(EEPROM_DOG_B_MODE + sizeof(uint32_t))
#define EEPROM_DOG_D_MODE				(EEPROM_DOG_C_MODE + sizeof(uint32_t))
#define EEPROM_DOG_E_MODE				(EEPROM_DOG_D_MODE + sizeof(uint32_t))
#define EEPROM_DOG_F_MODE				(EEPROM_DOG_E_MODE + sizeof(uint32_t))
#define EEPROM_SELECTED_DOG				(EEPROM_DOG_F_MODE + sizeof(uint32_t))
#define EEPROM_LOCK_ACTIVE_A			(EEPROM_SELECTED_DOG + sizeof(uint32_t))
#define EEPROM_LOCK_ACTIVE_B			(EEPROM_LOCK_ACTIVE_A + sizeof(uint32_t))
#define EEPROM_LOCK_ACTIVE_C			(EEPROM_LOCK_ACTIVE_B + sizeof(uint32_t))
#define EEPROM_LOCK_ACTIVE_D			(EEPROM_LOCK_ACTIVE_C + sizeof(uint32_t))
#define EEPROM_LOCK_ACTIVE_E			(EEPROM_LOCK_ACTIVE_D + sizeof(uint32_t))
#define EEPROM_LOCK_ACTIVE_F			(EEPROM_LOCK_ACTIVE_E + sizeof(uint32_t))
#define EEPROM_LOCK_BTM_KEYS			(EEPROM_LOCK_ACTIVE_F + sizeof(uint32_t))
#define EEPROM_NUMBER_OF_DOGS			(EEPROM_LOCK_BTM_KEYS + sizeof(uint32_t))
#define EEPROM_FEEDBACK_ON				(EEPROM_NUMBER_OF_DOGS + sizeof(uint32_t))
#define EEPROM_RING_1_MAC				(EEPROM_FEEDBACK_ON + sizeof(uint32_t))
#define EEPROM_RING_1_DOG				(EEPROM_RING_1_MAC + sizeof(uint32_t))
#define EEPROM_RING_1_FCE				(EEPROM_RING_1_DOG + sizeof(uint32_t))
#define EEPROM_RING_2_MAC				(EEPROM_RING_1_FCE + sizeof(uint32_t))
#define EEPROM_RING_2_DOG				(EEPROM_RING_2_MAC + sizeof(uint32_t))
#define EEPROM_RING_2_FCE				(EEPROM_RING_2_DOG + sizeof(uint32_t))
#define EEPROM_RING_3_MAC				(EEPROM_RING_2_FCE + sizeof(uint32_t))
#define EEPROM_RING_3_DOG				(EEPROM_RING_3_MAC + sizeof(uint32_t))
#define EEPROM_RING_3_FCE				(EEPROM_RING_3_DOG + sizeof(uint32_t))
#define EEPROM_RING_4_MAC				(EEPROM_RING_3_FCE + sizeof(uint32_t))
#define EEPROM_RING_4_DOG				(EEPROM_RING_4_MAC + sizeof(uint32_t))
#define EEPROM_RING_4_FCE				(EEPROM_RING_4_DOG + sizeof(uint32_t))
#define EEPROM_RING_5_MAC				(EEPROM_RING_4_FCE + sizeof(uint32_t))
#define EEPROM_RING_5_DOG				(EEPROM_RING_5_MAC + sizeof(uint32_t))
#define EEPROM_RING_5_FCE				(EEPROM_RING_5_DOG + sizeof(uint32_t))
#define EEPROM_RING_6_MAC				(EEPROM_RING_5_FCE + sizeof(uint32_t))
#define EEPROM_RING_6_DOG				(EEPROM_RING_6_MAC + sizeof(uint32_t))
#define EEPROM_RING_6_FCE				(EEPROM_RING_6_DOG + sizeof(uint32_t))
#define EEPROM_RING_7_MAC				(EEPROM_RING_6_FCE + sizeof(uint32_t))
#define EEPROM_RING_7_DOG				(EEPROM_RING_7_MAC + sizeof(uint32_t))
#define EEPROM_RING_7_FCE				(EEPROM_RING_7_DOG + sizeof(uint32_t))
#define EEPROM_RING_8_MAC				(EEPROM_RING_7_FCE + sizeof(uint32_t))
#define EEPROM_RING_8_DOG				(EEPROM_RING_8_MAC + sizeof(uint32_t))
#define EEPROM_RING_8_FCE				(EEPROM_RING_8_DOG + sizeof(uint32_t))
#define EEPROM_NORDIC_STATE				(EEPROM_RING_8_FCE + sizeof(uint32_t))
#define EEPROM_ASCENDING_1				(EEPROM_NORDIC_STATE + sizeof(uint32_t))
#define EEPROM_ASCENDING_2				(EEPROM_ASCENDING_1 + sizeof(uint32_t))
#define EEPROM_ASCENDING_3				(EEPROM_ASCENDING_2 + sizeof(uint32_t))
#define EEPROM_ASCENDING_4				(EEPROM_ASCENDING_3 + sizeof(uint32_t))
#define EEPROM_ASCENDING_5				(EEPROM_ASCENDING_4 + sizeof(uint32_t))
#define EEPROM_ASCENDING_6				(EEPROM_ASCENDING_5 + sizeof(uint32_t))


/* shadow 1 */
#define SH1_EEPROM_MACHINE_STATE			(EEPROM_ASCENDING_6+ sizeof(uint32_t))
#define SH1_EEPROM_MY_TX_EUI				(SH1_EEPROM_MACHINE_STATE+sizeof(uint32_t))
#define SH1_EEPROM_RF_FREQUENCY				(SH1_EEPROM_MY_TX_EUI+sizeof(uint32_t))
#define SH1_EEPROM_DOG_A_SHOCK				(SH1_EEPROM_RF_FREQUENCY + sizeof(uint32_t))
#define SH1_EEPROM_DOG_A_SHOCK_BOOSTER		(SH1_EEPROM_DOG_A_SHOCK + sizeof(uint32_t))
#define SH1_EEPROM_DOG_B_SHOCK				(SH1_EEPROM_DOG_A_SHOCK_BOOSTER + sizeof(uint32_t))
#define SH1_EEPROM_DOG_B_SHOCK_BOOSTER		(SH1_EEPROM_DOG_B_SHOCK + sizeof(uint32_t))
#define SH1_EEPROM_DOG_C_SHOCK				(SH1_EEPROM_DOG_B_SHOCK_BOOSTER + sizeof(uint32_t))
#define SH1_EEPROM_DOG_C_SHOCK_BOOSTER		(SH1_EEPROM_DOG_C_SHOCK + sizeof(uint32_t))
#define SH1_EEPROM_DOG_D_SHOCK				(SH1_EEPROM_DOG_C_SHOCK_BOOSTER + sizeof(uint32_t))
#define SH1_EEPROM_DOG_D_SHOCK_BOOSTER		(SH1_EEPROM_DOG_D_SHOCK + sizeof(uint32_t))
#define SH1_EEPROM_DOG_E_SHOCK				(SH1_EEPROM_DOG_D_SHOCK_BOOSTER + sizeof(uint32_t))
#define SH1_EEPROM_DOG_E_SHOCK_BOOSTER		(SH1_EEPROM_DOG_E_SHOCK + sizeof(uint32_t))
#define SH1_EEPROM_DOG_F_SHOCK				(SH1_EEPROM_DOG_E_SHOCK_BOOSTER + sizeof(uint32_t))
#define SH1_EEPROM_DOG_F_SHOCK_BOOSTER		(SH1_EEPROM_DOG_F_SHOCK + sizeof(uint32_t))
#define SH1_EEPROM_DOG_A_MODE				(SH1_EEPROM_DOG_F_SHOCK_BOOSTER + sizeof(uint32_t))	// NICK/Continues je pouze u 2 psu
#define SH1_EEPROM_DOG_B_MODE				(SH1_EEPROM_DOG_A_MODE + sizeof(uint32_t))
#define SH1_EEPROM_DOG_C_MODE				(SH1_EEPROM_DOG_B_MODE + sizeof(uint32_t))
#define SH1_EEPROM_DOG_D_MODE				(SH1_EEPROM_DOG_C_MODE + sizeof(uint32_t))
#define SH1_EEPROM_DOG_E_MODE				(SH1_EEPROM_DOG_D_MODE + sizeof(uint32_t))
#define SH1_EEPROM_DOG_F_MODE				(SH1_EEPROM_DOG_E_MODE + sizeof(uint32_t))
#define SH1_EEPROM_SELECTED_DOG				(SH1_EEPROM_DOG_F_MODE + sizeof(uint32_t))
#define SH1_EEPROM_LOCK_ACTIVE_A			(SH1_EEPROM_SELECTED_DOG + sizeof(uint32_t))
#define SH1_EEPROM_LOCK_ACTIVE_B			(SH1_EEPROM_LOCK_ACTIVE_A + sizeof(uint32_t))
#define SH1_EEPROM_LOCK_ACTIVE_C			(SH1_EEPROM_LOCK_ACTIVE_B + sizeof(uint32_t))
#define SH1_EEPROM_LOCK_ACTIVE_D			(SH1_EEPROM_LOCK_ACTIVE_C + sizeof(uint32_t))
#define SH1_EEPROM_LOCK_ACTIVE_E			(SH1_EEPROM_LOCK_ACTIVE_D + sizeof(uint32_t))
#define SH1_EEPROM_LOCK_ACTIVE_F			(SH1_EEPROM_LOCK_ACTIVE_E + sizeof(uint32_t))
#define SH1_EEPROM_LOCK_BTM_KEYS			(SH1_EEPROM_LOCK_ACTIVE_F + sizeof(uint32_t))
#define SH1_EEPROM_NUMBER_OF_DOGS			(SH1_EEPROM_LOCK_BTM_KEYS + sizeof(uint32_t))
#define SH1_EEPROM_FEEDBACK_ON				(SH1_EEPROM_NUMBER_OF_DOGS + sizeof(uint32_t))
#define SH1_EEPROM_RING_1_MAC				(SH1_EEPROM_FEEDBACK_ON + sizeof(uint32_t))
#define SH1_EEPROM_RING_1_DOG				(SH1_EEPROM_RING_1_MAC + sizeof(uint32_t))
#define SH1_EEPROM_RING_1_FCE				(SH1_EEPROM_RING_1_DOG + sizeof(uint32_t))
#define SH1_EEPROM_RING_2_MAC				(SH1_EEPROM_RING_1_FCE + sizeof(uint32_t))
#define SH1_EEPROM_RING_2_DOG				(SH1_EEPROM_RING_2_MAC + sizeof(uint32_t))
#define SH1_EEPROM_RING_2_FCE				(SH1_EEPROM_RING_2_DOG + sizeof(uint32_t))
#define SH1_EEPROM_RING_3_MAC				(SH1_EEPROM_RING_2_FCE + sizeof(uint32_t))
#define SH1_EEPROM_RING_3_DOG				(SH1_EEPROM_RING_3_MAC + sizeof(uint32_t))
#define SH1_EEPROM_RING_3_FCE				(SH1_EEPROM_RING_3_DOG + sizeof(uint32_t))
#define SH1_EEPROM_RING_4_MAC				(SH1_EEPROM_RING_3_FCE + sizeof(uint32_t))
#define SH1_EEPROM_RING_4_DOG				(SH1_EEPROM_RING_4_MAC + sizeof(uint32_t))
#define SH1_EEPROM_RING_4_FCE				(SH1_EEPROM_RING_4_DOG + sizeof(uint32_t))
#define SH1_EEPROM_RING_5_MAC				(SH1_EEPROM_RING_4_FCE + sizeof(uint32_t))
#define SH1_EEPROM_RING_5_DOG				(SH1_EEPROM_RING_5_MAC + sizeof(uint32_t))
#define SH1_EEPROM_RING_5_FCE				(SH1_EEPROM_RING_5_DOG + sizeof(uint32_t))
#define SH1_EEPROM_RING_6_MAC				(SH1_EEPROM_RING_5_FCE + sizeof(uint32_t))
#define SH1_EEPROM_RING_6_DOG				(SH1_EEPROM_RING_6_MAC + sizeof(uint32_t))
#define SH1_EEPROM_RING_6_FCE				(SH1_EEPROM_RING_6_DOG + sizeof(uint32_t))
#define SH1_EEPROM_RING_7_MAC				(SH1_EEPROM_RING_6_FCE + sizeof(uint32_t))
#define SH1_EEPROM_RING_7_DOG				(SH1_EEPROM_RING_7_MAC + sizeof(uint32_t))
#define SH1_EEPROM_RING_7_FCE				(SH1_EEPROM_RING_7_DOG + sizeof(uint32_t))
#define SH1_EEPROM_RING_8_MAC				(SH1_EEPROM_RING_7_FCE + sizeof(uint32_t))
#define SH1_EEPROM_RING_8_DOG				(SH1_EEPROM_RING_8_MAC + sizeof(uint32_t))
#define SH1_EEPROM_RING_8_FCE				(SH1_EEPROM_RING_8_DOG + sizeof(uint32_t))
#define SH1_EEPROM_NORDIC_STATE				(SH1_EEPROM_RING_8_FCE + sizeof(uint32_t))
#define SH1_EEPROM_ASCENDING_1				(SH1_EEPROM_NORDIC_STATE + sizeof(uint32_t))
#define SH1_EEPROM_ASCENDING_2				(SH1_EEPROM_ASCENDING_1 + sizeof(uint32_t))
#define SH1_EEPROM_ASCENDING_3				(SH1_EEPROM_ASCENDING_2 + sizeof(uint32_t))
#define SH1_EEPROM_ASCENDING_4				(SH1_EEPROM_ASCENDING_3 + sizeof(uint32_t))
#define SH1_EEPROM_ASCENDING_5				(SH1_EEPROM_ASCENDING_4 + sizeof(uint32_t))
#define SH1_EEPROM_ASCENDING_6				(SH1_EEPROM_ASCENDING_5 + sizeof(uint32_t))

/* shadow 2 */
#define SH2_EEPROM_MACHINE_STATE			(SH1_EEPROM_ASCENDING_6+ sizeof(uint32_t))
#define SH2_EEPROM_MY_TX_EUI				(SH2_EEPROM_MACHINE_STATE+sizeof(uint32_t))
#define SH2_EEPROM_RF_FREQUENCY				(SH2_EEPROM_MY_TX_EUI+sizeof(uint32_t))
#define SH2_EEPROM_DOG_A_SHOCK				(SH2_EEPROM_RF_FREQUENCY + sizeof(uint32_t))
#define SH2_EEPROM_DOG_A_SHOCK_BOOSTER		(SH2_EEPROM_DOG_A_SHOCK + sizeof(uint32_t))
#define SH2_EEPROM_DOG_B_SHOCK				(SH2_EEPROM_DOG_A_SHOCK_BOOSTER + sizeof(uint32_t))
#define SH2_EEPROM_DOG_B_SHOCK_BOOSTER		(SH2_EEPROM_DOG_B_SHOCK + sizeof(uint32_t))
#define SH2_EEPROM_DOG_C_SHOCK				(SH2_EEPROM_DOG_B_SHOCK_BOOSTER + sizeof(uint32_t))
#define SH2_EEPROM_DOG_C_SHOCK_BOOSTER		(SH2_EEPROM_DOG_C_SHOCK + sizeof(uint32_t))
#define SH2_EEPROM_DOG_D_SHOCK				(SH2_EEPROM_DOG_C_SHOCK_BOOSTER + sizeof(uint32_t))
#define SH2_EEPROM_DOG_D_SHOCK_BOOSTER		(SH2_EEPROM_DOG_D_SHOCK + sizeof(uint32_t))
#define SH2_EEPROM_DOG_E_SHOCK				(SH2_EEPROM_DOG_D_SHOCK_BOOSTER + sizeof(uint32_t))
#define SH2_EEPROM_DOG_E_SHOCK_BOOSTER		(SH2_EEPROM_DOG_E_SHOCK + sizeof(uint32_t))
#define SH2_EEPROM_DOG_F_SHOCK				(SH2_EEPROM_DOG_E_SHOCK_BOOSTER + sizeof(uint32_t))
#define SH2_EEPROM_DOG_F_SHOCK_BOOSTER		(SH2_EEPROM_DOG_F_SHOCK + sizeof(uint32_t))
#define SH2_EEPROM_DOG_A_MODE				(SH2_EEPROM_DOG_F_SHOCK_BOOSTER + sizeof(uint32_t))	// NICK/Continues je pouze u 2 psu
#define SH2_EEPROM_DOG_B_MODE				(SH2_EEPROM_DOG_A_MODE + sizeof(uint32_t))
#define SH2_EEPROM_DOG_C_MODE				(SH2_EEPROM_DOG_B_MODE + sizeof(uint32_t))
#define SH2_EEPROM_DOG_D_MODE				(SH2_EEPROM_DOG_C_MODE + sizeof(uint32_t))
#define SH2_EEPROM_DOG_E_MODE				(SH2_EEPROM_DOG_D_MODE + sizeof(uint32_t))
#define SH2_EEPROM_DOG_F_MODE				(SH2_EEPROM_DOG_E_MODE + sizeof(uint32_t))
#define SH2_EEPROM_SELECTED_DOG				(SH2_EEPROM_DOG_F_MODE + sizeof(uint32_t))
#define SH2_EEPROM_LOCK_ACTIVE_A			(SH2_EEPROM_SELECTED_DOG + sizeof(uint32_t))
#define SH2_EEPROM_LOCK_ACTIVE_B			(SH2_EEPROM_LOCK_ACTIVE_A + sizeof(uint32_t))
#define SH2_EEPROM_LOCK_ACTIVE_C			(SH2_EEPROM_LOCK_ACTIVE_B + sizeof(uint32_t))
#define SH2_EEPROM_LOCK_ACTIVE_D			(SH2_EEPROM_LOCK_ACTIVE_C + sizeof(uint32_t))
#define SH2_EEPROM_LOCK_ACTIVE_E			(SH2_EEPROM_LOCK_ACTIVE_D + sizeof(uint32_t))
#define SH2_EEPROM_LOCK_ACTIVE_F			(SH2_EEPROM_LOCK_ACTIVE_E + sizeof(uint32_t))
#define SH2_EEPROM_LOCK_BTM_KEYS			(SH2_EEPROM_LOCK_ACTIVE_F + sizeof(uint32_t))
#define SH2_EEPROM_NUMBER_OF_DOGS			(SH2_EEPROM_LOCK_BTM_KEYS + sizeof(uint32_t))
#define SH2_EEPROM_FEEDBACK_ON				(SH2_EEPROM_NUMBER_OF_DOGS + sizeof(uint32_t))
#define SH2_EEPROM_RING_1_MAC				(SH2_EEPROM_FEEDBACK_ON + sizeof(uint32_t))
#define SH2_EEPROM_RING_1_DOG				(SH2_EEPROM_RING_1_MAC + sizeof(uint32_t))
#define SH2_EEPROM_RING_1_FCE				(SH2_EEPROM_RING_1_DOG + sizeof(uint32_t))
#define SH2_EEPROM_RING_2_MAC				(SH2_EEPROM_RING_1_FCE + sizeof(uint32_t))
#define SH2_EEPROM_RING_2_DOG				(SH2_EEPROM_RING_2_MAC + sizeof(uint32_t))
#define SH2_EEPROM_RING_2_FCE				(SH2_EEPROM_RING_2_DOG + sizeof(uint32_t))
#define SH2_EEPROM_RING_3_MAC				(SH2_EEPROM_RING_2_FCE + sizeof(uint32_t))
#define SH2_EEPROM_RING_3_DOG				(SH2_EEPROM_RING_3_MAC + sizeof(uint32_t))
#define SH2_EEPROM_RING_3_FCE				(SH2_EEPROM_RING_3_DOG + sizeof(uint32_t))
#define SH2_EEPROM_RING_4_MAC				(SH2_EEPROM_RING_3_FCE + sizeof(uint32_t))
#define SH2_EEPROM_RING_4_DOG				(SH2_EEPROM_RING_4_MAC + sizeof(uint32_t))
#define SH2_EEPROM_RING_4_FCE				(SH2_EEPROM_RING_4_DOG + sizeof(uint32_t))
#define SH2_EEPROM_RING_5_MAC				(SH2_EEPROM_RING_4_FCE + sizeof(uint32_t))
#define SH2_EEPROM_RING_5_DOG				(SH2_EEPROM_RING_5_MAC + sizeof(uint32_t))
#define SH2_EEPROM_RING_5_FCE				(SH2_EEPROM_RING_5_DOG + sizeof(uint32_t))
#define SH2_EEPROM_RING_6_MAC				(SH2_EEPROM_RING_5_FCE + sizeof(uint32_t))
#define SH2_EEPROM_RING_6_DOG				(SH2_EEPROM_RING_6_MAC + sizeof(uint32_t))
#define SH2_EEPROM_RING_6_FCE				(SH2_EEPROM_RING_6_DOG + sizeof(uint32_t))
#define SH2_EEPROM_RING_7_MAC				(SH2_EEPROM_RING_6_FCE + sizeof(uint32_t))
#define SH2_EEPROM_RING_7_DOG				(SH2_EEPROM_RING_7_MAC + sizeof(uint32_t))
#define SH2_EEPROM_RING_7_FCE				(SH2_EEPROM_RING_7_DOG + sizeof(uint32_t))
#define SH2_EEPROM_RING_8_MAC				(SH2_EEPROM_RING_7_FCE + sizeof(uint32_t))
#define SH2_EEPROM_RING_8_DOG				(SH2_EEPROM_RING_8_MAC + sizeof(uint32_t))
#define SH2_EEPROM_RING_8_FCE				(SH2_EEPROM_RING_8_DOG + sizeof(uint32_t))
#define SH2_EEPROM_NORDIC_STATE				(SH2_EEPROM_RING_8_FCE + sizeof(uint32_t))
#define SH2_EEPROM_ASCENDING_1				(SH2_EEPROM_NORDIC_STATE + sizeof(uint32_t))
#define SH2_EEPROM_ASCENDING_2				(SH2_EEPROM_ASCENDING_1 + sizeof(uint32_t))
#define SH2_EEPROM_ASCENDING_3				(SH2_EEPROM_ASCENDING_2 + sizeof(uint32_t))
#define SH2_EEPROM_ASCENDING_4				(SH2_EEPROM_ASCENDING_3 + sizeof(uint32_t))
#define SH2_EEPROM_ASCENDING_5				(SH2_EEPROM_ASCENDING_4 + sizeof(uint32_t))
#define SH2_EEPROM_ASCENDING_6				(SH2_EEPROM_ASCENDING_5 + sizeof(uint32_t))
#define EEPROM_RESET_COUNTER				(SH2_EEPROM_ASCENDING_6 + sizeof(uint32_t))


/**
 *
 */
static void inline EepromStart(bool write)
{
    osThreadSuspendAll();
    if(write == true)
    {
    	HAL_FLASHEx_DATAEEPROM_Unlock();
    }

}

/**
 *
 */
static void inline EepromStop(void)
{
    HAL_FLASHEx_DATAEEPROM_Lock();
    osThreadResumeAll();
}


eSelectedDog EA_GetSelectedDog(void);
eStateCoreSystem EA_GetSystemState(void);
void EA_SetSelectedDog(eSelectedDog dog);
void EA_SetRfFreq(uint32_t freq);
uint32_t EA_GetRfFreq(void);
uint32_t EA_GetSystemMAC(void);
void EA_SetSystemMAC(uint32_t mac);
void EA_SetSystemState(eStateCoreSystem state);
void EA_SetShockA(uint8_t shock);
void EA_SetShockB(uint8_t shock);
void EA_SetShockC(uint8_t shock);
void EA_SetShockD(uint8_t shock);
void EA_SetShockE(uint8_t shock);
void EA_SetShockF(uint8_t shock);
uint8_t EA_GetShockA(void);
uint8_t EA_GetShockB(void);
uint8_t EA_GetShockC(void);
uint8_t EA_GetShockD(void);
uint8_t EA_GetShockE(void);
uint8_t EA_GetShockF(void);
void EA_SetBoosterA(uint8_t shock);
void EA_SetBoosterB(uint8_t shock);
void EA_SetBoosterC(uint8_t shock);
void EA_SetBoosterD(uint8_t shock);
void EA_SetBoosterE(uint8_t shock);
void EA_SetBoosterF(uint8_t shock);
uint8_t EA_GetBoosterA(void);
uint8_t EA_GetBoosterB(void);
uint8_t EA_GetBoosterC(void);
uint8_t EA_GetBoosterD(void);
uint8_t EA_GetBoosterE(void);
uint8_t EA_GetBoosterF(void);
bool EA_GetLockBtmKeys(void);
void EA_SetLockBtmKeys(bool locked);

eMode EA_GetMode(uint8_t dog);
void EA_SetMode(uint8_t dog,eMode mode);
uint8_t EA_GetNumOfDogs(void);
void EA_SetNumOfDogs(uint8_t numberOfDogs);
uint8_t EA_GetFeedbackON(void);
void EA_SetFeedbackON(uint8_t value);

bool EA_GetAscendingMode(uint8_t dog);
void EA_SetAscendingMode(uint8_t dog, bool ascending);

void EA_IncResetCounter();
void EA_ClearResetCounter();
bool EA_IsResetCounterFull();

bool EA_GetLockActive(eSelectedDog dog);
void EA_SetLockActive(eSelectedDog dog ,bool lockActive);

#if(VERSION_WITH_RING ==1)
void EA_SaveRing(uint8_t ring,uint8_t selectedDog, uint8_t function, uint32_t mac);
void EA_EraseRingMAC( uint8_t ring,uint32_t mac);
uint32_t EA_GetRingMAC(uint8_t ring /*0-7*/);
uint8_t EA_GetRingBtnFction(uint8_t ring);
uint8_t EA_GetRingDogID(uint8_t ring);
bool EA_GetNordicState(void);
void EA_SetNordicState(bool state);
#endif
#endif /* INC_EEPROMADDRESS_H_ */

