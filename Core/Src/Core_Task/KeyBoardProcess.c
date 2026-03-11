/*
 * KeyBoardProcess.c
 *
 *  Created on: 27. 6. 2021
 *      Author: developer
 */

#include "TaskCore.h"
#include "KeyBoardProcess.h"
#include "ProcessCoreTask.h"
#include "EepromAddress.h"
#include "SignalProcessing.h"

extern osTimerId TimerBacklightHandle;
extern osMessageQId QueueRFHandle;
extern osMessageQId QueueDisplayHandle;
extern osMessageQId QueueCoreHandle;
extern osTimerId TimerLockShockHandle;
extern osTimerId TimerBeepHandle;
extern osTimerId TimerVibratDuringShockHandle;
extern osTimerId TimerChargerHandle;
extern osTimerId TimerUnpairringHandle;
extern osTimerId Timer_AscendingModeHandle;


uint64_t UnmaskKeyBit(uint64_t value, uint64_t mask)
{
	return value & (~mask);
}

/**
 *
 * @param
 * @param LocalStatStruct
 */
void KP_KeyBoardActivated(tDataQueue receiveData,tCoreGlobalData *glData,tStateCoreAutomat* StateAutomat)
{
	if(receiveData.data==DATA_TO_CORE_SHORT_PRESS_EVENT)	/* short Press*/
	{
		if((glData->ShortPressLowLevelMask==ALL_KEY_PINS)&&(receiveData.RFU_64!=ALL_KEY_PINS))
		{	/*first - falling edge came*/
			osTimerStop(TimerLockShockHandle);

			if(StateAutomat->ActualState != STATE_CORE_OFF)
			{
				if(KP_KeyboardFallingEdge((uint32_t)receiveData.RFU_64, glData) == true)	return;
			}

			glData->repeatCounter=0;
		}

		switch (receiveData.RFU_64)
		{
			case ALL_KEY_PINS:
				/*jiz neni nic zmacknuto*/
				if(glData->btns_disabled == true)
				{
					KP_KeyboarRisingEdge(glData);
					break;
				}

				KP_KeyboarRisingEdge(glData);

				if((glData->ShortPressLowLevelMask == PRESS_SW2_PLUS ) || (glData->ShortPressLowLevelMask == PRESS_SW3_MINUS )
						|| (glData->ShortPressLowLevelMask == PRESS_SW4_1_LEFT) ||(glData->ShortPressLowLevelMask == PRESS_SW5_2_RIGHT)
						|| (glData->ShortPressLowLevelMask == PRESS_SW1_ON_OFF))
				{
					if ((glData->sharedData.activeSettingsNumberOfDogs)&&((glData->ShortPressLowLevelMask == PRESS_SW4_1_LEFT)||(glData->ShortPressLowLevelMask == PRESS_SW5_2_RIGHT)))
					{
						//no nothing
						// if is in number of dogs settings - left or right do not exit this settings
					}
					else
					{
						KP_ShortPress(glData->ShortPressLowLevelMask, glData);
					}
				}

				glData->ShortPressLowLevelMask=ALL_KEY_PINS;

				break;

			case PRESS_SW1_ON_OFF:
			case PRESS_SW2_PLUS:
			case PRESS_SW3_MINUS:
			case PRESS_SW4_1_LEFT:
			case PRESS_SW5_2_RIGHT:
			case PRESS_SW6_BEEP:
			case PRESS_SW7_PULSE:
			case PRESS_SW8_BOOST:
				glData->ShortPressLowLevelMask=receiveData.RFU_64;
				break;

			default:
				break;
		}

	}
	else if(receiveData.data==DATA_TO_CORE_MED_LONG_PRESS_EVENT)	/* Med Long event */
	{
		glData->ShortPressLowLevelMask=ALL_KEY_PINS;

		#if (VERSION_WITH_RING == 1)
				// zadny dlouhy stisk pri parovani neni povolen
				if (glData->sharedData.ringInfo.isPairingMenuActive == true)	return;
		#endif

		if(receiveData.RFU_64 == PRESS_SW1_ON_OFF)
		{
			KP_KeyBoardLight(false,false,false,glData);
			KP_LongPressPress(receiveData.RFU_64,glData,StateAutomat);
		}
	}
	else if(receiveData.data==DATA_TO_CORE_LONG_PRESS_EVENT)	/* Long event */
	{
		glData->ShortPressLowLevelMask=ALL_KEY_PINS;

#if (VERSION_WITH_RING == 1)
		// zadny dlouhy stisk pri parovani neni povolen
		if (glData->sharedData.ringInfo.isPairingMenuActive == true)	return;
#endif

		if((receiveData.RFU_64 == PRESS_SW4_1_LEFT ) || (receiveData.RFU_64 == PRESS_SW5_2_RIGHT))
		{
			if((glData->sharedData.lockBottomKeys == false) && (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].onlyVibration == false)
					&& (glData->sharedData.activeSettingsNumberOfDogs == false))
			{
				KP_KeyBoardLight(false,false,false,glData);
				KP_LongPressPress(receiveData.RFU_64,glData,StateAutomat);
			}
		}

	}
	else if(receiveData.data==DATA_TO_CORE_REPEATE_PRESS_EVENT)	/* Repeat */
	{
		glData->ShortPressLowLevelMask=ALL_KEY_PINS;

#if (VERSION_WITH_RING == 1)
		// zakazujeme veskere super long press v parovacim rezimu
		if (glData->sharedData.ringInfo.isPairingMenuActive == true)	return;
#endif
		// Super long press
		if (((UnmaskKeyBit(receiveData.RFU_64, PRESS_SW4_1_LEFT)==KEY_PRESSED) && (UnmaskKeyBit(receiveData.RFU_64, PRESS_SW5_2_RIGHT)==KEY_PRESSED))
			|| ((UnmaskKeyBit(receiveData.RFU_64, PRESS_SW2_PLUS)==KEY_PRESSED) && (UnmaskKeyBit(receiveData.RFU_64, PRESS_SW3_MINUS)==KEY_PRESSED))
			|| ((UnmaskKeyBit(receiveData.RFU_64, PRESS_SW4_1_LEFT)==KEY_PRESSED) && (UnmaskKeyBit(receiveData.RFU_64, PRESS_SW3_MINUS)==KEY_PRESSED))
			|| ((UnmaskKeyBit(receiveData.RFU_64, PRESS_SW5_2_RIGHT)==KEY_PRESSED) && (UnmaskKeyBit(receiveData.RFU_64, PRESS_SW2_PLUS)==KEY_PRESSED)))
		{
			glData->repeatCounter++;

			if(glData->repeatCounter==SUPER_LONG_PRESS){
				if(glData->sharedData.lockBottomKeys == false)
				{
					//KP_KeyBoardLight(false,false,false,glData);
					KP_LongPressPress(receiveData.RFU_64,glData,StateAutomat);
				}
			}
		}
		else
		{
			glData->repeatCounter=0;
		}
		// ----------------

		if(((receiveData.RFU_64 == PRESS_SW2_PLUS) ||(receiveData.RFU_64 == PRESS_SW3_MINUS))
				&& (glData->sharedData.lockBottomKeys == false))
		{

			if(KP_ApplicablePressBOTTOMKEYS(receiveData.RFU_64, glData) == true)
			{
				KP_KeyBoardLight(true,false,false,glData);
				KP_ShortPress(receiveData.RFU_64,glData);
			}

			if ((glData->sharedData.dogsInfo[glData->sharedData.selectedDog].activeBooter == false) &&
					(glData->sharedData.activeSettingsNumberOfDogs == false))
			{
				/* prechod z ran na vibraci */
				if(receiveData.RFU_64 == PRESS_SW3_MINUS)
				{
					if(glData->sharedData.dogsInfo[glData->sharedData.selectedDog].onlyVibration)
					{
						if((glData->sharedData.dogsInfo[glData->sharedData.selectedDog].shockValue == VIBRATION_MIN))
						{
							if(xTimerIsTimerActive(TimerLockShockHandle) == pdFALSE)
							{
								osTimerStart(TimerLockShockHandle,LOCK_SHOCK_TIMEOUT);
							}
						}
					}
					else
					{
						if((glData->sharedData.dogsInfo[glData->sharedData.selectedDog].shockValue == SHOCK_MIN))
						{
							if(xTimerIsTimerActive(TimerLockShockHandle) == pdFALSE)
							{
								osTimerStart(TimerLockShockHandle,LOCK_SHOCK_TIMEOUT);
							}
						}
					}

				}
			}

		}

	}
}

/**
 *
 */
void KP_ShortPress(uint32_t button,tCoreGlobalData *glData)
{
#if (VERSION_WITH_RING == 1)
	if(glData->sharedData.ringInfo.isPairingMenuActive == true)
	{
		PCT_RestartUnpairTimer();
	}
#endif

	switch (button)
	{
		case PRESS_SW2_PLUS:
			KP_KeyPLUS(glData);
			break;

		case PRESS_SW3_MINUS:
			KP_KeyMinus(glData);
			break;

		case PRESS_SW4_1_LEFT:
			KP_KeyLeft(glData);
			break;

		case PRESS_SW5_2_RIGHT:
			KP_KeyRight(glData);
			break;

		case PRESS_SW6_BEEP:
#if (VERSION_WITH_RING == 1)
			if (glData->sharedData.ringInfo.isPairingMenuActive == true)
			{
				glData->sharedData.ringInfo.selectedBtn = RING_FCE_BEEP;
				PCT_RefreshLCD(glData);
			}
			else
#endif
			{
#if (VERSION_WITH_RING == 1)
				if(glData->sharedData.ringInfo.txActiveBy == TX_BY_NORDIC) break;
				glData->sharedData.ringInfo.txActiveBy = TX_BY_EDGE;
#endif
				PCT_SendBeep(glData,glData->sharedData.selectedDog,true);
			}


			break;

		case PRESS_SW7_PULSE:
#if (VERSION_WITH_RING == 1)
			if (glData->sharedData.ringInfo.isPairingMenuActive == true)
			{
				glData->sharedData.ringInfo.selectedBtn = RING_FCE_SHOCK;
				PCT_RefreshLCD(glData);
			}else
#endif
			{

#if (VERSION_WITH_RING == 1)
				if(glData->sharedData.ringInfo.txActiveBy == TX_BY_NORDIC) break;
				glData->sharedData.ringInfo.txActiveBy = TX_BY_EDGE;
#endif
				PCT_SendShock(glData,glData->sharedData.selectedDog,true);
			}

			break;

		case PRESS_SW8_BOOST:

#if (ASCENDING_MODE_ENABLED==true)
			glData->sharedData.dogsInfo[glData->sharedData.selectedDog].ascendingStarted = false;
#endif

#if (VERSION_WITH_RING == 1)
			if (glData->sharedData.ringInfo.isPairingMenuActive == true)
			{
				glData->sharedData.ringInfo.selectedBtn = RING_FCE_BOOSTER;
				PCT_RefreshLCD(glData);
			}
			else
#endif
			{

#if (VERSION_WITH_RING == 1)
				if(glData->sharedData.ringInfo.txActiveBy == TX_BY_NORDIC) break;
				glData->sharedData.ringInfo.txActiveBy = TX_BY_EDGE;
#endif

				uint8_t valueTosend;
#if ASCENDING_MODE_ENABLED == true
				if(EA_GetAscendingMode(glData->sharedData.selectedDog) == true
						&& glData->sharedData.dogsInfo[glData->sharedData.selectedDog].onlyVibration == false)
				{
					glData->sharedData.dogsInfo[glData->sharedData.selectedDog].ascendingMode = true;
					if(ASCENDING_VALUE_ZERO_OR_SHOCK == true)
					{
						valueTosend = glData->sharedData.dogsInfo[glData->sharedData.selectedDog].ascendingShockValue = 0;
					}
					else
					{
						valueTosend = glData->sharedData.dogsInfo[glData->sharedData.selectedDog].ascendingShockValue = glData->sharedData.dogsInfo[glData->sharedData.selectedDog].shockValue;
					}

					osTimerStart(Timer_AscendingModeHandle, ASCENDING_INCREASING_PERIOD_MS);
				}
				else
#endif
				{
					valueTosend = glData->sharedData.dogsInfo[glData->sharedData.selectedDog].boosterValue;
				}

				PCT_SendBooster(glData,valueTosend,glData->sharedData.selectedDog,true);

			}


			break;

		case PRESS_SW1_ON_OFF:
#if (VERSION_WITH_RING == 1)
			if(glData->sharedData.ringInfo.isPairingMenuActive == true)
			{
				glData->sharedData.ringInfo.isPairingMenuActive = false;
				osTimerStop(TimerUnpairringHandle);
				PCT_RefreshLCD(glData);

			}else
#endif
			{
				PCT_LockPulseEdit(glData);
			}

			break;

		default:
			break;
	}


}

/**
 *
 * @param KeyLevelMask
 * @param GlobalData
 * @param LocalStatStruct
 */
void KP_LongPressPress(uint32_t button,tCoreGlobalData *glData,tStateCoreAutomat* StateAutomat)
{
	tDataQueue sendData,sendData2;
	sendData.pointer = NULL;
	sendData2.pointer = NULL;
	bool lcdRefresh = false;

	switch (button)
	{
#if (VER_4DOGS != 1)
		case PRESS_SW4_1_LEFT:
			glData->sharedData.selectedDog = DOG_A;
			glData->sharedData.dogsInfo[glData->sharedData.selectedDog].activeBooter = true;
			lcdRefresh = true;
			break;

		case PRESS_SW5_2_RIGHT:
			glData->sharedData.selectedDog = DOG_B;
			glData->sharedData.dogsInfo[glData->sharedData.selectedDog].activeBooter = true;
			lcdRefresh = true;
			break;
#else	// 4 psy
		case PRESS_SW4_1_LEFT:
		case PRESS_SW5_2_RIGHT:
			if(glData->sharedData.lockBottomKeys == true)	break;

			if(glData->sharedData.dogsInfo[glData->sharedData.selectedDog].activeBooter == false)
			{
				glData->sharedData.dogsInfo[glData->sharedData.selectedDog].activeBooter = true;
				lcdRefresh = true;
			}

			break;
#endif

		case PRESS_SW1_ON_OFF:

			if(StateAutomat->ActualState == STATE_CORE_ON)
			{
				sendData.address = ADDR_TO_CORE_START_SHUTDOWN;
				xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);

			}
			else
			{
				PCT_StartMeasureBatt(glData,false);

				if(glData->sharedData.needChargerToLive == false)
				{
					sendData.address = ADDR_TO_RF_CHANGE_STATE;
					sendData.data = DATA_TO_RF_START_ON;

					sendData2.address = ADDR_TO_LCD_CHANGE_STATE;
					sendData2.data = DATA_TO_LCD_START_ON;

					xQueueSend(QueueRFHandle,&sendData, portMAX_DELAY);
					xQueueSend(QueueDisplayHandle,&sendData2, portMAX_DELAY);

					StateAutomat->PreviousState = StateAutomat->ActualState;
					StateAutomat->ActualState = STATE_CORE_START_ON;

					/* baterie se nemuze zotavit po zapnuti */
//					if(glData->sharedData.iUseLipol == false)
//					{
//						/* povolime zotaveni bateri po zapnuti*/
//						glData->sharedData.stateBattery=BATTERY_STATUS_HIGH;
//					}

					EA_ClearResetCounter(); // vynuluj resetcounter

				}
				else
				{
					/* rozsvitime prazdnou baterku */
					sendData2.address = ADDR_TO_LCD_CHANGE_STATE;
					sendData2.data = DATA_TO_LCD_START_ON;
					glData->sharedData.stateBattery = BATTERY_STATUS_CRITICAL;
					/* po chvili zhasneme symbol batt v Offu */
					osTimerStart(TimerChargerHandle, CHARGER_UNPLGED_LCD_TIMEOUT);
				}
			}
			lcdRefresh = true;
			break;

		default:
			break;
	}

#if VER_4DOGS==1
	if ((UnmaskKeyBit(button, PRESS_SW4_1_LEFT)==KEY_PRESSED) && (UnmaskKeyBit(button, PRESS_SW5_2_RIGHT)==KEY_PRESSED))
	{
		// activate settings number of dogs
		glData->sharedData.activeSettingsNumberOfDogs = true;
		lcdRefresh = true;
	}
	else if ((UnmaskKeyBit(button, PRESS_SW2_PLUS)==KEY_PRESSED) && (UnmaskKeyBit(button, PRESS_SW3_MINUS)==KEY_PRESSED))
	{
		PCT_ToggleFeedbackON(glData);

		glData->ShortPressLowLevelMask=ALL_KEY_PINS;

#if (KEYBOARD_VIBRATION == 1)
		LL_GPIO_SetOutputPin(VIBRATION_GPIO_Port, VIBRATION_Pin);
		osDelay(2*VIBRAT_SHORT_PRESS_TIMEOUT);
		LL_GPIO_ResetOutputPin(VIBRATION_GPIO_Port, VIBRATION_Pin);
#endif
	}
#endif

#if (VERSION_WITH_RING == 1)
	/* zapinani parovani prstynku */
	if ((UnmaskKeyBit(button, PRESS_SW4_1_LEFT)==KEY_PRESSED) && (UnmaskKeyBit(button, PRESS_SW3_MINUS)==KEY_PRESSED))
	{
		if(StateAutomat->ActualState == STATE_CORE_ON)	/* pouze v zaplem stavu */
		{
			PCT_ChangeNORDICState(true,true,glData);
			PCT_EnterInPairMenu(glData);
			lcdRefresh = true;

			glData->ShortPressLowLevelMask=ALL_KEY_PINS;
		}
	}
	else /* zapinani a vypinani prstynku */
	if ((UnmaskKeyBit(button, PRESS_SW5_2_RIGHT)==KEY_PRESSED) && (UnmaskKeyBit(button, PRESS_SW2_PLUS)==KEY_PRESSED))
	{
		if(StateAutomat->ActualState == STATE_CORE_ON)	/* pouze v zaplem stavu */
		{

			glData->ShortPressLowLevelMask=ALL_KEY_PINS;

			if(glData->sharedData.ringInfo.ringIsOn == true)
			{
				PCT_ChangeNORDICState(false,true, glData);
			}
			else
			{
				PCT_ChangeNORDICState(true,true, glData);
			}

			glData->btns_disabled = true;
			lcdRefresh = true;
		}
	}
#endif

	if(lcdRefresh)	PCT_RefreshLCD(glData);

}

/**
 *
 */
void KP_KeyPLUS(tCoreGlobalData *glData) {
	bool lcdRefresh;

	if (glData->sharedData.lockBottomKeys == true)
		return;


#if (VERSION_WITH_RING == 1)
	if(glData->sharedData.ringInfo.isPairingMenuActive == true)
	{
		lcdRefresh = PCT_IncreaseRingNumber(glData);
	}
	else
#endif
	{
		if (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].activeBooter)
		{
		/* prechod pres Ascending */
#ifdef ASCENDING_MODE_ENABLED
			if(glData->sharedData.dogsInfo[glData->sharedData.selectedDog].boosterValue == 0
							&& glData->sharedData.dogsInfo[glData->sharedData.selectedDog].ascendingMode == false )
			{
				//zapnuti Ascending mode
				PCT_SetAscendingMode(glData->sharedData.selectedDog, true, glData);
				lcdRefresh = true;
			}
			else if(glData->sharedData.dogsInfo[glData->sharedData.selectedDog].ascendingMode == true)
			{
				/* vypnuti ascending */
				PCT_SetAscendingMode(glData->sharedData.selectedDog, false, glData);
				glData->sharedData.dogsInfo[glData->sharedData.selectedDog].boosterValue = 1;
				lcdRefresh = true;
			}
			else
			{
				PCT_SetAscendingMode(glData->sharedData.selectedDog, false, glData);
				lcdRefresh = true;
				PCT_IncreaseBooster(glData->sharedData.selectedDog, 1, glData);
			}

#else
			lcdRefresh = PCT_IncreaseBooster(glData->sharedData.selectedDog, 1, glData);
#endif


		}else if (glData->sharedData.activeSettingsNumberOfDogs) {
			lcdRefresh = PCT_IncreaseNumberOfDogs(glData);
		} else {
			lcdRefresh = PCT_IncreaseShock(glData->sharedData.selectedDog, 1, glData);
		}
	}


	if (lcdRefresh)
		PCT_RefreshLCD(glData);
}

/**
 *
 */
void KP_KeyMinus(tCoreGlobalData *glData)
{
	bool lcdRefresh = false;

	if (glData->sharedData.lockBottomKeys == true)
		return;

#if (VERSION_WITH_RING == 1)
	if(glData->sharedData.ringInfo.isPairingMenuActive == true)
	{
		lcdRefresh = PCT_DecreaseRingNumber(glData);
	}
	else
#endif
	{
		if (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].activeBooter)
		{
/* prechod pres Ascending */
#if ASCENDING_MODE_ENABLED == true
			if(glData->sharedData.dogsInfo[glData->sharedData.selectedDog].boosterValue == 1
				&& glData->sharedData.dogsInfo[glData->sharedData.selectedDog].ascendingMode == false )
			{
				//zapnuti Ascending mode
				PCT_SetAscendingMode(glData->sharedData.selectedDog, true, glData);
				lcdRefresh = true;
			}
//			else if(glData->sharedData.dogsInfo[glData->sharedData.selectedDog].ascendingMode == true)
//			{
//				/* vypnuti ascending */
//				PCT_SetAscendingMode(glData->sharedData.selectedDog, false, glData);
//				lcdRefresh = true;
//			}
			else
			{
				/*vypnuti ascending */
				PCT_SetAscendingMode(glData->sharedData.selectedDog, false, glData);
				lcdRefresh =true;
				PCT_DecreaseBooster(glData->sharedData.selectedDog, 1, glData);
			}
#else
			lcdRefresh = PCT_DecreaseBooster(glData->sharedData.selectedDog, 1, glData);
#endif


		} else if (glData->sharedData.activeSettingsNumberOfDogs) {
			lcdRefresh = PCT_DecreaseNumberOfDogs(glData);
		} else {
			/* prepneme nick/cont */
			if ((glData->sharedData.dogsInfo[glData->sharedData.selectedDog].shockValue == 0)
					&& (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].onlyVibration == false)) {
				glData->sharedData.dogsInfo[glData->sharedData.selectedDog].modeNC = !glData->sharedData.dogsInfo[glData->sharedData.selectedDog].modeNC;
				EA_SetMode(glData->sharedData.selectedDog,glData->sharedData.dogsInfo[glData->sharedData.selectedDog].modeNC);
				lcdRefresh = true;
				KP_KeyBoardLight(true, false, false, glData);
			} else {
				lcdRefresh = PCT_DecreaseShock(glData->sharedData.selectedDog, 1, glData);
			}
		}
	}

	if (lcdRefresh)
		PCT_RefreshLCD(glData);

}

/**
 *
 */
void KP_KeyLeft(tCoreGlobalData *glData)
{
	if(glData->sharedData.lockBottomKeys == true)	return;

	if(PCT_ExitFromSettings(glData) == true) return;

//#if (VER_4DOGS != true)
//	switch (glData->sharedData.selectedDog)
//	{
//		case DOG_A:
//			glData->sharedData.dogsInfo[DOG_A].modeNC = !glData->sharedData.dogsInfo[DOG_A].modeNC;
//			EA_SetModeA(glData->sharedData.dogsInfo[DOG_A].modeNC);
//			break;
//
//		case DOG_B:
//			glData->sharedData.dogsInfo[DOG_A].modeNC = EA_GetModeA();
//			glData->sharedData.selectedDog = DOG_A;
//			EA_SetSelectedDog(glData->sharedData.selectedDog);
//			break;
//
//		default:
//			break;
//	}
//#else
	glData->sharedData.selectedDog = SP_ConstrainINT(glData->sharedData.selectedDog-1, DOG_A, glData->sharedData.numberOfDogs-1);
	EA_SetSelectedDog(glData->sharedData.selectedDog);
//#endif

	PCT_RefreshLCD(glData);

}

/**
 *
 */
void KP_KeyRight(tCoreGlobalData *glData)
{
	if(glData->sharedData.lockBottomKeys == true)	return;

	if(PCT_ExitFromSettings(glData) == true) return;

//#if (VER_4DOGS != true)
//	switch (glData->sharedData.selectedDog)
//	{
//		case DOG_B:
//			glData->sharedData.dogsInfo[DOG_B].modeNC = !glData->sharedData.dogsInfo[DOG_B].modeNC;
//			EA_SetModeB(glData->sharedData.dogsInfo[DOG_B].modeNC);
//
//			break;
//
//		case DOG_A:
//			glData->sharedData.dogsInfo[DOG_B].modeNC = EA_GetModeB();
//			glData->sharedData.selectedDog = DOG_B;
//			EA_SetSelectedDog(glData->sharedData.selectedDog);
//
//			break;
//
//		default:
//			break;
//	}
//#else
	glData->sharedData.selectedDog = SP_ConstrainINT(glData->sharedData.selectedDog+1, DOG_A, glData->sharedData.numberOfDogs-1);
	EA_SetSelectedDog(glData->sharedData.selectedDog);

//#endif

	PCT_RefreshLCD(glData);

}

/**
 * stisknuti tlacitek
 */
bool KP_KeyboardFallingEdge(uint32_t button,tCoreGlobalData *glData)
{
	bool temp = false;
	bool tempOnlyLight = false;


	if (KP_ApplicablePressBOTTOMKEYS(button,glData) == false)
	{
		tempOnlyLight = true;
	}

	if ((glData->sharedData.activeSettingsNumberOfDogs)&&((UnmaskKeyBit(button, PRESS_SW4_1_LEFT)==KEY_PRESSED) || (UnmaskKeyBit(button, PRESS_SW5_2_RIGHT)==KEY_PRESSED)))
	{
		tempOnlyLight = true;
	}

	KP_KeyBoardLight(true,false,tempOnlyLight,glData);

	if((button != PRESS_SW2_PLUS) && (button != PRESS_SW3_MINUS)
			&& (button != PRESS_SW4_1_LEFT)&& (button != PRESS_SW5_2_RIGHT))
	{
		temp = PCT_ExitFromSettings(glData);

		if((temp == true)&&(button == PRESS_SW1_ON_OFF)) return true;

	}
	else if(button == PRESS_SW3_MINUS)
	{
		if((glData->sharedData.lockBottomKeys == false) &&
				(glData->sharedData.dogsInfo[glData->sharedData.selectedDog].activeBooter == false)
				&& (glData->sharedData.activeSettingsNumberOfDogs == false)
				&& (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].shockValue == 0))
		{
			osTimerStart(TimerLockShockHandle,LOCK_SHOCK_TIMEOUT);
		}
	}

	if((button == PRESS_SW6_BEEP ) || (button == PRESS_SW7_PULSE ) || (button == PRESS_SW8_BOOST ))
	{
		/* reaguj ihned */
		KP_ShortPress(button, glData);
#if (KEYBOARD_VIBRATION == 1)
		osTimerStart(TimerVibratDuringShockHandle, PERIOD_VIBRAT_DURING_SHOCK);
#endif
		//return true;
	}

	return false;
}



/**
 * pusteni tlacitek
 */
void KP_KeyboarRisingEdge(tCoreGlobalData *glData)
{
#if (VERSION_WITH_RING == 1)
		if(glData->sharedData.ringInfo.txActiveBy == TX_BY_NORDIC)
		{
			return;
		}
#endif

		glData->btns_disabled = false;
		PCT_StopTX(glData);


	if(xTimerIsTimerActive(TimerBacklightHandle) == pdFALSE)
	{	/* zhasni pokud podsvit */
		LL_GPIO_ResetOutputPin(BACKLIGHT_GPIO_Port, BACKLIGHT_Pin);
	}

	osTimerStop(TimerLockShockHandle);
}

/**
 *
 */
void KP_KeyBoardLight(bool shortPress,bool impulse,bool onlyLight, tCoreGlobalData *glData )
{
	static uint8_t beep_vib_cnt;

	if(glData->sharedData.txActive != TX_ACTIVE_BEEP ) beep_vib_cnt = 0;

	LL_GPIO_SetOutputPin(BACKLIGHT_GPIO_Port, BACKLIGHT_Pin);
	osTimerStart(TimerBacklightHandle, BACKLIGHT_TIMEOUT);

	if (onlyLight == true)	return;

	if (glData->sharedData.feedbackON)
	{
	#if (KEYBOARD_SOUND == 1)

		LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH4);

		if(shortPress)	osTimerStart(TimerBeepHandle, BEEP_TIMEOUT);
		else			osTimerStart(TimerBeepHandle, 280);

	#endif

	#if (KEYBOARD_VIBRATION == 1)

		float tempTime;

		if(impulse)
		{
			if(glData->sharedData.txActive == TX_ACTIVE_SHOCK )
			{
				tempTime = glData->sharedData.dogsInfo[glData->sharedData.selectedDog].shockValue * 1.87;
			}
			else
			{

#if (ASCENDING_MODE_ENABLED == 1)
				if(glData->sharedData.dogsInfo[glData->sharedData.selectedDog].ascendingMode == true &&
						glData->sharedData.txActive == TX_ACTIVE_BOOSTER)
				{
					tempTime = glData->sharedData.dogsInfo[glData->sharedData.selectedDog].ascendingShockValue * 1.87;

				}
				else
#endif
				{
					// pokud mame na boosteru vibraci
					if(glData->sharedData.dogsInfo[glData->sharedData.selectedDog].boosterValue == 0)
					{
						tempTime = BEEP_VIBRAT_POWER * 1.87;
					}
					else
					{
						tempTime = glData->sharedData.dogsInfo[glData->sharedData.selectedDog].boosterValue * 1.87;
					}
				}

			}

			tempTime += 15;
			tempTime = SP_ConstrainU32(tempTime, 15, PERIOD_VIBRAT_DURING_SHOCK);
			osTimerStart(TimerBeepHandle, tempTime);
		}
		else
		{
			beep_vib_cnt++;

			if(glData->sharedData.txActive == TX_ACTIVE_BEEP )
			{
				if(beep_vib_cnt == 4)
				{
					//beep_vib_cnt = 0;
					return;
				}
				else if(beep_vib_cnt == 5)
				{
					beep_vib_cnt = 0;
					return;
				}
				tempTime = BEEP_VIBRAT_POWER * 1.87;
				tempTime += 15;
				tempTime = SP_ConstrainU32(tempTime, 15, PERIOD_VIBRAT_DURING_SHOCK);
				osTimerStart(TimerBeepHandle, tempTime);
			}
			else
			{
				if(shortPress)	osTimerStart(TimerBeepHandle, VIBRAT_SHORT_PRESS_TIMEOUT);
				else			osTimerStart(TimerBeepHandle, 110);
			}

		}

		LL_GPIO_SetOutputPin(VIBRATION_GPIO_Port, VIBRATION_Pin);
	#endif
	}

}


/**
 *
 */
bool KP_ApplicablePressBOTTOMKEYS(uint32_t button, tCoreGlobalData *glData)
{
	switch (button)
	{
		case PRESS_SW2_PLUS:
			return KP_ApplicablePressPLUS(glData);
			break;

		case PRESS_SW3_MINUS:
			return KP_ApplicablePressMINUS(glData);
			break;

		//case PRESS_SW2_PLUS:
		case PRESS_SW4_1_LEFT:
			return KP_ApplicablePressLEFT(glData);
			break;

		case PRESS_SW5_2_RIGHT:
			return KP_ApplicablePressRIGHT(glData);
			break;

		default:
			break;
	}

	return true;
}

/**
 *
 */
bool KP_ApplicablePressMINUS(tCoreGlobalData *glData) {
	if (glData->sharedData.lockBottomKeys == true)
		return false;

	if (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].activeBooter == true) {
		if (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].boosterValue > BOOSTER_MIN) {
			return true;
		} else {
			return false;
		}
	} else if (glData->sharedData.activeSettingsNumberOfDogs == true) {
		if (glData->sharedData.numberOfDogs > 1) {
			return true;
		} else {
			return false;
		}
	} else {
		if (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].onlyVibration == false) {
			if (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].shockValue > SHOCK_MIN) {
				return true;
			} else {
				return false;
			}
		} else {
			if (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].shockValue > VIBRATION_MIN) {
				return true;
			} else {
				return false;
			}
		}
	}
}

/**
 *
 */
bool KP_ApplicablePressPLUS(tCoreGlobalData *glData) {
	if (glData->sharedData.lockBottomKeys == true)
		return false;

	if (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].activeBooter == true) {
		if (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].boosterValue < BOOSTER_MAX) {
			return true;
		} else {
			return false;
		}
	} else if (glData->sharedData.activeSettingsNumberOfDogs == true) {
		if (glData->sharedData.numberOfDogs < NUMBER_OF_DOGS_MAX) {
			return true;
		} else {
			return false;
		}
	} else {
		if (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].onlyVibration == false) {
			if (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].shockValue < SHOCK_MAX) {
				return true;
			} else {
				return false;
			}
		} else {
			if (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].shockValue < VIBRATION_MAX) {
				return true;
			} else {
				return false;
			}
		}
	}
}


/**
 *
 */
bool KP_ApplicablePressLEFT(tCoreGlobalData *glData)
{
	if(glData->sharedData.lockBottomKeys == true)	return false;


	if(glData->sharedData.selectedDog > DOG_A)	return true;
	return false;

}


/**
 *
 */
bool KP_ApplicablePressRIGHT(tCoreGlobalData *glData)
{
	if(glData->sharedData.lockBottomKeys == true)	return false;

	if(glData->sharedData.selectedDog < (glData->sharedData.numberOfDogs -1))	return true;

	return false;

}


