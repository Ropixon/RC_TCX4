/*
 * TaskDisplay.c
 *
 *  Created on: 25. 3. 2021
 *      Author: jirik
 */

/************************************************************************/
/* include header												   		*/
/************************************************************************/
#include "cmsis_os.h"
#include "main.h"
#include "TaskDisplay.h"
#include "LCD_LowLevel.h"
#include "ProcessLCDTask.h"
#include "lcd.h"

/************************************************************************/
/* Declaration importing objects                                        */
/************************************************************************/
extern osMessageQId 	QueueCoreHandle;
extern osMessageQId		QueueDisplayHandle;
extern osTimerId 		TimerLCDHandle;
extern osTimerId 		TimerChargAnimationHandle;
/************************************************************************/
/* Definition global variables                                          */
/************************************************************************/

/************************************************************************/
/* Local #DEFINE														*/
/************************************************************************/

/************************************************************************/
/* Local TYPEDEF												   		*/
/************************************************************************/


typedef struct
{
	bool tempValue;
} DataStateTaskDISPLAY_t;

/************************************************************************/
/* Definition local variables										   	*/
/************************************************************************/

/************************************************************************/
/* Declaration functions											   	*/
/************************************************************************/
static uint8_t DISPLAY_StateINIT(tDataQueue receiveData,
		DataStateTaskDISPLAY_t* globalData, StructstateAutomatDisplay_t* stateAutomat,
		void** pointerMalloc);

static uint8_t DISPLAY_StateOFF(tDataQueue receiveData,
		DataStateTaskDISPLAY_t* globalData, StructstateAutomatDisplay_t* stateAutomat,
		void** pointerMalloc);

static uint8_t DISPLAY_StateStartON(tDataQueue receiveData,
		DataStateTaskDISPLAY_t* globalData, StructstateAutomatDisplay_t* stateAutomat,
		void** pointerMalloc);

static uint8_t DISPLAY_StateON(tDataQueue receiveData,
		DataStateTaskDISPLAY_t* globalData, StructstateAutomatDisplay_t* stateAutomat,
		void** pointerMalloc);

static uint8_t DISPLAY_StateStartOFF(tDataQueue receiveData,
		DataStateTaskDISPLAY_t* globalData, StructstateAutomatDisplay_t* stateAutomat,
		void** pointerMalloc);

static uint8_t (*StateDISPLAY[])(tDataQueue, DataStateTaskDISPLAY_t*,
		StructstateAutomatDisplay_t*, void**) =
		{DISPLAY_StateINIT, DISPLAY_StateOFF, DISPLAY_StateStartON , DISPLAY_StateON, DISPLAY_StateStartOFF};



/************************************************************************/
/* Definition functions                                                 */
/************************************************************************/

void BlinkingSegment(bool rstDiv1)
{
	tSharedDataLCD_Core LCDinfo;
	static uint8_t div1;

	if(rstDiv1 == true)
	{
		div1 = 0;
		return;
	}

	PLT_LoadLCDInfo(&LCDinfo);

#if (VERSION_WITH_RING == 1)
	if(LCDinfo.ringInfo.isPairingMenuActive == true)			PLT_ToogleRingBtnFunction(div1,&LCDinfo);
#endif
	if(LCDinfo.dogsInfo[LCDinfo.selectedDog].activeBooter == true)	PLT_ToogleSetBooster(div1,&LCDinfo);
	else if(LCDinfo.activeSettingsNumberOfDogs == true)	PLT_ToogleSettingsNumberOfDogs(div1,&LCDinfo);

	if(div1<1)	div1++;
	else		div1=0;

}

/*
 *
 */
void CallbackChargeAnimation(void const * argument)
{
	static uint8_t div;
	tSharedDataLCD_Core LCDinfo;

	PLT_LoadLCDInfo(&LCDinfo);

	if(LCDinfo.chargerState == CHARGER_RUNNING)	LCD_ShowBattLevels(div+1);

	if(div<3)	div++;
	else		div=0;

}

/**
 *
 */
void LCDRefreschWatchdog(void)
{
	tDataQueue	sendData;
	sendData.pointer=NULL;

	sendData.address=ADDR_TO_CORE_WATCHDOG_ACTIONS;
	sendData.data=WD_LCD_IS_ALIVE;
	xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);
	portYIELD();

}


/**
 *
 */
static uint8_t DISPLAY_StateINIT(tDataQueue receiveData,
		DataStateTaskDISPLAY_t* globalData, StructstateAutomatDisplay_t* stateAutomat,
		void** pointerMalloc){

	stateAutomat->actualState = STATE_LCD_OFF;

	return 0;
}

/**
 *
 */
static uint8_t DISPLAY_StateOFF(tDataQueue receiveData,
		DataStateTaskDISPLAY_t* globalData, StructstateAutomatDisplay_t* stateAutomat,
		void** pointerMalloc){

	tDataQueue 		sendData;
	sendData.pointer=NULL;

	switch (receiveData.address)
	{
		case ADDR_TO_LCD_CHANGE_STATE:
			if (receiveData.data == DATA_TO_LCD_START_ON)
			{
				 /* Enable the peripheral */
				LCD->CR|=LCD_CR_LCDEN;

				LCD_ShowAllFrames();
				sendData.address = ADDR_TO_CORE_TASK_STATE_CHANGED;
				sendData.data = DATA_TO_CORE_LCD_IS_ON;
				xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);

				stateAutomat->previousState = stateAutomat->actualState;
				stateAutomat->actualState = STATE_LCD_ON;

			}
			else if (receiveData.data == DATA_TO_LCD_START_OFF)
			{
				osTimerStop(TimerLCDHandle);
				osTimerStop(TimerChargAnimationHandle);
				LCD_TurnOffAllSegments();
				LCD->CR&=~LCD_CR_LCDEN;

				sendData.address = ADDR_TO_CORE_TASK_STATE_CHANGED;
				sendData.data = DATA_TO_CORE_LCD_IS_OFF;
				xQueueSend(QueueCoreHandle, &sendData,portMAX_DELAY);

			}
			break;
		default:
			break;
	}
	return 0;
}

/**
 *
 */
static uint8_t DISPLAY_StateStartON(tDataQueue receiveData,
		DataStateTaskDISPLAY_t* globalData, StructstateAutomatDisplay_t* stateAutomat,
		void** pointerMalloc){

	return 0;
}

/**
 *
 */
static uint8_t DISPLAY_StateON(tDataQueue receiveData,
		DataStateTaskDISPLAY_t* globalData, StructstateAutomatDisplay_t* stateAutomat,
		void** pointerMalloc)
{

	tDataQueue 		sendData;
	sendData.pointer=NULL;
	tSharedDataLCD_Core pageInfo;

	switch (receiveData.address)
	{
		case ADDR_TO_LCD_REFRESH_PAGE:
			if (mcuIsLocked() == false)	break;

			PLT_SaveNewData(receiveData.pointer);

			PLT_LoadLCDInfo(&pageInfo);

			if(PLT_AnythingIsBlinking())
			{
				osTimerStart(TimerLCDHandle, BLINKING_LCD_PERIOD);
				BlinkingSegment(true);
			}
			else
			{
				osTimerStop(TimerLCDHandle);
			}

			/* blikani pro nabijeni je zvlast - aby animace pusobila plynule */
			if((pageInfo.chargerState == CHARGER_RUNNING))
			{
				if(xTimerIsTimerActive(TimerChargAnimationHandle) != true)	osTimerStart(TimerChargAnimationHandle, BLINKING_LCD_PERIOD);
			}
			else	osTimerStop(TimerChargAnimationHandle);


			PLT_ChangeScreen(&pageInfo);

			break;

		case ADDR_TO_LCD_CHANGE_STATE:
			if (mcuIsLocked() == false)	break;

			PLT_LoadLCDInfo(&pageInfo);

			if ((receiveData.data == DATA_TO_LCD_START_OFF))
			{
				osTimerStop(TimerLCDHandle);
				LCD_TurnOffAllSegments();

				if((pageInfo.chargerState == CHARGER_RUNNING))
				{

				}
				else
				{
					osTimerStop(TimerChargAnimationHandle);
					LCD->CR&=~LCD_CR_LCDEN;
				}

				sendData.address = ADDR_TO_CORE_TASK_STATE_CHANGED;
				sendData.data = DATA_TO_CORE_LCD_IS_OFF;
				xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);
				stateAutomat->previousState = stateAutomat->actualState;
				stateAutomat->actualState = STATE_LCD_OFF;
			}
			break;

		case ADDR_TO_LCD_LCD_TEST:
			if(mcuIsLocked()==false)
			{
				LCD_TurnOffAllSegments();

				uint16_t  values[] = {0, 2, 4, 6};

				if (receiveData.data >= 0 && receiveData.data <= 3)
				{
					LCD_Write(values[receiveData.data],LCD_MASK_ALL,receiveData.RFU_32);
					LCD_UpdateDisplayRequest();
				}
				else if((receiveData.data == 0xFF) && (receiveData.RFU_32 == 0xFFF))
				{
					LCD_TurnOnAllSegments();
				}

				LL_GPIO_SetOutputPin(BACKLIGHT_GPIO_Port, BACKLIGHT_Pin);

			}
			break;

		default:
			break;
	}
	return 0;
}

/**
 *
 */
static uint8_t DISPLAY_StateStartOFF(tDataQueue receiveData,
		DataStateTaskDISPLAY_t* globalData, StructstateAutomatDisplay_t* stateAutomat,
		void** pointerMalloc){

	return 0;
}

/**
 *
 * @param argument
 */
void TaskDisplay(void const * argument)
{
    static tDataQueue receiveData;
    static DataStateTaskDISPLAY_t dataTaskDisplay;
    static StructstateAutomatDisplay_t stateAutomat = {STATE_LCD_INIT, STATE_LCD_INIT};
    static void* pointerToMalloc = NULL;
    static portBASE_TYPE returnValue;

#if RTT_PRINT == 1
    SEGGER_RTT_printf(0, "%sDISPLAY_START\r\n", RTT_CTRL_BG_BLACK );
#endif

    DISPLAY_StateINIT(receiveData, &dataTaskDisplay, &stateAutomat, &pointerToMalloc);

    for (;;)
    {
    	returnValue = xQueueReceive(QueueDisplayHandle, &receiveData, portMAX_DELAY);
    	//LL_GPIO_SetOutputPin(TP204_SCOPE_GPIO_Port, TP204_SCOPE_Pin);

		if (returnValue == pdPASS)
		{
			if(receiveData.address == ADDR_TO_LCD_CHECK_LCD_IS_ALIVE)
			{
				LCDRefreschWatchdog();
			}
			else
			{
				if((stateAutomat.actualState != STATE_LCD_ON) && (receiveData.address == ADDR_TO_LCD_REFRESH_PAGE))
				{
					PLT_SaveNewData(receiveData.pointer);
					PLT_CareAboutCharger(&stateAutomat);
				}
				else
				{
					StateDISPLAY[stateAutomat.actualState](receiveData, &dataTaskDisplay, &stateAutomat,
						&pointerToMalloc);
				}

			}

			/* Clear malloc */
			vPortFree(receiveData.pointer);
			receiveData.pointer=NULL;

		}
    }
}

