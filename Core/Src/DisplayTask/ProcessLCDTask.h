/*
 * ProcessLCDTask.h
 *
 *  Created on: 28. 6. 2021
 *      Author: developer
 */

#ifndef SRC_DISPLAYTASK_PROCESSLCDTASK_H_
#define SRC_DISPLAYTASK_PROCESSLCDTASK_H_

#include "TaskDisplay.h"

void PLT_SaveNewData(tSharedDataLCD_Core * data);
bool PLT_AnythingIsBlinking(void);
void PLT_LoadLCDInfo(tSharedDataLCD_Core *data);
void PLT_ChangeScreen(tSharedDataLCD_Core * pageInfo);
void PLT_CareAboutCharger(StructstateAutomatDisplay_t *stateAutomat);
void PLT_ToogleSetBooster(uint8_t visible, tSharedDataLCD_Core *data);
void PLT_ToogleSettingsNumberOfDogs(uint8_t visible, tSharedDataLCD_Core *data);
#if (VERSION_WITH_RING == 1)
uint8_t PLT_GetAllRingFunctions(tSharedDataLCD_Core *data);
#endif

#if (VERSION_WITH_RING == 1)

void PLT_ToogleRingBtnFunction(uint8_t visible,tSharedDataLCD_Core *data);

#endif
#endif /* SRC_DISPLAYTASK_PROCESSLCDTASK_H_ */
