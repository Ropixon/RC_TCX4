/*
 * ProcessLCDTask.c
 *
 *  Created on: 28. 6. 2021
 *      Author: developer
 */

#include "main.h"
#include "TaskCore.h"
#include  "ProcessLCDTask.h"
#include "LCD_LowLevel.h"
#include "lcd.h"
#include "EepromAddress.h"
#include "SignalProcessing.h"

static tSharedDataLCD_Core LCDData;
extern osTimerId TimerLCDHandle;
extern osTimerId TimerChargerHandle;
extern osTimerId TimerChargAnimationHandle;

/**
 *
 */
void PLT_ChangeScreen(tSharedDataLCD_Core * pageInfo)
{
	static uint8_t lastScreen = 0;

#if (VERSION_WITH_RING == 1)
	if(pageInfo->ringInfo.isPairingMenuActive == false)
#endif
	{
		 if(lastScreen != 1)
		 {
			 LCD_TurnOffAllSegments();
			 LCD_Write(LCD_DOG_ICO);
		 }
		 LCD_DrawMainScreen(pageInfo);
		 lastScreen = 1;
	}
#if (VERSION_WITH_RING == 1)
	else
	{

		if(lastScreen != 0) LCD_TurnOffAllSegments();
		LCD_DrawPairingScreen(pageInfo);
		lastScreen = 0;

	}
#endif

}


/**
 *
 */
void PLT_CareAboutCharger(StructstateAutomatDisplay_t *stateAutomat)
{
	tSharedDataLCD_Core pageInfo;

	PLT_LoadLCDInfo(&pageInfo);

	if(stateAutomat->actualState != STATE_LCD_ON)
	{
		switch (pageInfo.chargerState)
		{

			case CHARGER_RUNNING:
				/* Enable the peripheral */
				LCD->CR|=LCD_CR_LCDEN;
				LCD_Write(LCD_BAT_FRAME);
				LCD_UpdateDisplayRequest();
				osTimerStart(TimerChargAnimationHandle, BLINKING_LCD_PERIOD);
				break;

			case CHARGER_BATT_FULL:
				LCD->CR|=LCD_CR_LCDEN;
				LCD_TurnOffAllSegments();
				LCD_ShowBattLevels(pageInfo.stateBattery);

				break;

			case CHARGER_UNPLUGGED:

				if(xTimerIsTimerActive(TimerChargerHandle) == pdFALSE) /* chceme jiz zhasnout symbol batt */
				{
					osTimerStop(TimerChargAnimationHandle);
					LCD->CR|=LCD_CR_LCDEN;
					LCD_TurnOffAllSegments();
					LCD->CR&=~LCD_CR_LCDEN;
				}
				else
				{
					LCD->CR|=LCD_CR_LCDEN;
					LCD_ShowBattLevels(pageInfo.stateBattery);
				}
				break;

			default:
				break;
		}

	}

}

/*
 *
 */
bool PLT_AnythingIsBlinking(void)
{
	tSharedDataLCD_Core data;
	PLT_LoadLCDInfo(&data);

#if (VERSION_WITH_RING == 1)
	if(data.ringInfo.isPairingMenuActive == true)	return true;	//budeme blikat symbolem oznacujici jedno ze 3
																		//povelovych tlaciteck
#endif

	if(data.dogsInfo[data.selectedDog].activeBooter == true)	return true;
	if(data.activeSettingsNumberOfDogs == true)	return true;

	return false;
}

/*
 *
 */
void PLT_SaveNewData(tSharedDataLCD_Core * data)
{
	taskENTER_CRITICAL();
	memcpy(&LCDData,data,sizeof(tSharedDataLCD_Core));
	taskEXIT_CRITICAL();
}

/*
 *
 */
void PLT_LoadLCDInfo(tSharedDataLCD_Core *data)
{
	taskENTER_CRITICAL();
	memcpy(data,&LCDData,sizeof(tSharedDataLCD_Core));
	taskEXIT_CRITICAL();
}

#if (VERSION_WITH_RING == 1)
/*
 *
 */
void PLT_ToogleRingBtnFunction(uint8_t visible,tSharedDataLCD_Core *data)
{

	LCD_Clear_Btn1Function(false);
	LCD_Clear_Btn2Function(false);
	LCD_Clear_Btn3Function(false);

	if(visible == true)
	{
		LCD_ShowRingBtnFunction(data->ringInfo.selectedBtn,true,false);
	}

	LCD_UpdateDisplayRequest();
}

#endif
/**
 *
 */
void PLT_ToogleSetBooster(uint8_t visible, tSharedDataLCD_Core *data)
{
	LCD_Clear_1_All(true);
	LCD_Clear_2_All(true);
	LCD_ClearDogsID(true);

	if(visible == true)
	{
#if ASCENDING_MODE_ENABLED == true
		LCD_ShowNumber(data->dogsInfo[data->selectedDog].boosterValue,data->dogsInfo[data->selectedDog].activeBooter,
				data->dogsInfo[data->selectedDog].ascendingMode);
#else
		LCD_ShowNumber(data->dogsInfo[data->selectedDog].boosterValue,data->dogsInfo[data->selectedDog].activeBooter,false);
#endif
		LCD_ShowDogID(data->selectedDog);
	}

}

/**
 *
 */
void PLT_ToogleSettingsNumberOfDogs(uint8_t visible, tSharedDataLCD_Core *data)
{
	LCD_Clear_1_All(true);
	LCD_Clear_2_All(true);
	LCD_ClearDogsID(true);

	if(visible == true)
	{
		LCD_ShowNumber(data->numberOfDogs, false,false);
		LCD_ShowDogID_Train(data->numberOfDogs);
	}
}

#if (VERSION_WITH_RING == 1)
/**
 *
 */
uint8_t PLT_GetAllRingFunctions(tSharedDataLCD_Core *data)
{
	uint8_t functions=0;

    for (uint8_t ringNumber = 1; ringNumber <= RING_MAX_ALLOWED_RING; ringNumber++)
    {
        uint8_t currentDog = EA_GetRingDogID(ringNumber);
        currentDog = SP_ConstrainU8(currentDog, DOG_A, data->numberOfDogs);

        if (currentDog == data->selectedDog)
        {
        	functions |= (1<<EA_GetRingBtnFction(ringNumber));
        }
    }

    return functions;
}
#endif

