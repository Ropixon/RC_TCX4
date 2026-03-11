/*
 * ProcessCoreTask.h
 *
 *  Created on: Jun 25, 2021
 *      Author: developer
 */

#ifndef INC_FREERTOS_TASKS_CORE_TASK_PROCESSCORETASK_H_
#define INC_FREERTOS_TASKS_CORE_TASK_PROCESSCORETASK_H_

#include "UartProcess.h"


typedef enum
{
	ADC_MEAS_WITHOUT_LOAD = 0,
	ADC_MEAS_WITH_LOAD = 1,

	ADC_MEAS_CNT,
}eADCMeasType;

//#define ADC_MEAS_WITHOUT_LOAD  	0
//#define ADC_MEAS_WITH_LOAD  	1

typedef struct
{
	uint16_t  volBatt_mv;
	uint16_t  volAccu_mv;
	int16_t  volTemp_mv;
}__attribute__((packed)) adcData_t;


void PCT_RefreshLCD(tCoreGlobalData * glData);
bool PCT_ToggleFeedbackON(tCoreGlobalData *glData);
bool PCT_IncreaseBooster(eSelectedDog dog, uint8_t value, tCoreGlobalData *glData);
bool PCT_IncreaseShock(eSelectedDog dog, uint8_t value, tCoreGlobalData *glData);
bool PCT_DecreaseShock(eSelectedDog dog, uint8_t value, tCoreGlobalData *glData);
bool PCT_DecreaseBooster(eSelectedDog dog, uint8_t value, tCoreGlobalData *glData);
bool PCT_IncreaseNumberOfDogs(tCoreGlobalData *glData);
bool PCT_DecreaseNumberOfDogs(tCoreGlobalData *glData);
bool PCT_ExitFromSettings(tCoreGlobalData *glData);
bool PCT_ExitFromBoosterSettings(tCoreGlobalData *glData);
void PCT_LockPulseEdit(tCoreGlobalData *glData);
void PCT_SendBeep(tCoreGlobalData *glData, eSelectedDog selectedDog, bool lcdRefresh);
void PCT_SendShock( tCoreGlobalData *glData,eSelectedDog selectedDog, bool lcdRefresh);
void PCT_LockShockToogle(tCoreGlobalData *glData);
void PCT_StopTX(tCoreGlobalData *glData);
void PCT_SendBooster( tCoreGlobalData *glData,uint8_t shockValue,eSelectedDog selectedDog, bool lcdRefresh);
void PCT_StartMeasureBatt(tCoreGlobalData * glData,bool measAfterCharge);
void PCT_WatchDogProcedure(uint32_t	data);
void PCT_TimerStartWatchdog();
uint16_t CRC_Calculation(uint8_t Data[], uint16_t len);
void PCT_ChargerProcedure(uint32_t data, tStateCoreAutomat* stateAutomat,tCoreGlobalData *glData);
void PCT_SetAscendingMode(eSelectedDog dog, bool enable, tCoreGlobalData *glData);
void PCT_ChargerInit(void);
void PCT_StartShutdown(tStateCoreAutomat* stateAutomat);

#if(VERSION_WITH_RING == 1)
void PCT_ChangeNORDICState(bool state /* true = on */,bool writeToEeprom,tCoreGlobalData *glData);
void PCT_ResetRing(tCoreGlobalData *glData);
void PCT_EnterInPairMenu(tCoreGlobalData *glData);
void PCT_RestartUnpairTimer();
void PCT_ResetRing(tCoreGlobalData *glData);
bool PCT_DecreaseRingNumber(tCoreGlobalData *glData);
bool PCT_IncreaseRingNumber(tCoreGlobalData *glData);
uint8_t PCT_Ring_Is_MAC_Whitelisted(uint32_t mac);
void PCT_UnpairRing(tCoreGlobalData *glData);
void PCT_EraseRing(uint8_t ring);
uint8_t PCT_DecodeRingPacket(uart_payloads_u *packet,tCoreGlobalData *glData);
#endif

#endif /* INC_FREERTOS_TASKS_CORE_TASK_PROCESSCORETASK_H_ */
