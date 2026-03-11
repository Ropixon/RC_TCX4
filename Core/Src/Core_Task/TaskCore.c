/*
 * TaskCore.c
 *
 *  Created on: 25. 3. 2021
 *      Author: jirik
 */

/************************************************************************/
/* include header												   		*/
/************************************************************************/
#include "main.h"
#include "TaskCore.h"
#include "SignalProcessing.h"
#include "KeyBoardProcess.h"
#include "EepromAddress.h"
#include "ProcessCoreTask.h"
#include "RadioUser.h"

#include "FactoryTest.h"

#if (VERSION_WITH_RING==1)
#include "UartProcess.h"
#endif

/************************************************************************/
/* Declaration importing objects                                        */
/************************************************************************/
extern  osMessageQId 	QueueCoreHandle;
extern  osMessageQId 	QueueRFHandle;
extern  osMessageQId	QueueDisplayHandle;
extern  osTimerId 		TimerMeasureBattHandle;
extern  osTimerId 		TimerBacklightHandle;
extern  osTimerId 		TimerStopTXHandle;
extern  osTimerId 		TimerLockShockHandle;
extern  osTimerId 		TimerVibratDuringShockHandle;
extern osTimerId 		TimerChargerHandle;
extern osTimerId		TimerUartCheckHandle;
extern TaskHandle_t 	NotifyADCDone;
extern osTimerId TimerFactoryTestHandle;
extern osTimerId TimerRingHeartBeatHandle;
extern osTimerId TimerStopTxFromRingHandle;
extern osTimerId Timer_AscendingModeHandle;
extern osTimerId timerChargingCheckHandle;
/************************************************************************/
/* Definition global variables                                          */
/************************************************************************/

/************************************************************************/
/* Local #DEFINE														*/
/************************************************************************/

//Define watchdog constants
#define MAX_COUNT_TICK_TIM_WATCH		20

#define SIZE_OF_QUEUE_CORE_TASK			16

/************************************************************************/
/* Local TYPEDEF												   		*/
/************************************************************************/


typedef enum {
	STATE_N = 0,
	STATE_C
} EnumStateShockTime_t;					//State shock time

typedef struct
{
	uint32_t bitMask;
	uint32_t prohibitionBitMask;
	uint16_t counter;
} StructIntKeys_t;



/************************************************************************/
/* Definition local variables										   	*/
/************************************************************************/
static volatile tKeyBoard 			GlKeyboard;
static volatile tChargerStatepin 	GlCharger;
#if(VERSION_WITH_RING)
volatile bool						ringTxStillActive_LOCK;
#endif

/************************************************************************/
/* Declaration functions											   	*/
/************************************************************************/

static uint8_t CORE_StateINIT(tDataQueue receiveData,
		tCoreGlobalData* globalData, tStateCoreAutomat* stateAutomat,
		void** pointerMalloc);

static uint8_t CORE_StateOFF(tDataQueue receiveData,
		tCoreGlobalData* globalData, tStateCoreAutomat* stateAutomat,
		void** pointerMalloc);

static uint8_t CORE_StateStartON(tDataQueue receiveData,
		tCoreGlobalData* globalData, tStateCoreAutomat* stateAutomat,
		void** pointerMalloc);

static uint8_t CORE_StateON(tDataQueue receiveData,
		tCoreGlobalData* globalData, tStateCoreAutomat* stateAutomat,
		void** pointerMalloc);

static uint8_t CORE_StateStartOFF(tDataQueue receiveData,
		tCoreGlobalData* globalData, tStateCoreAutomat* stateAutomat,
		void** pointerMalloc);

static uint8_t (*StateCORE[])(tDataQueue, tCoreGlobalData*,
		tStateCoreAutomat*, void**) =
		{CORE_StateINIT, CORE_StateOFF, CORE_StateStartON , CORE_StateON, CORE_StateStartOFF};

/************************************************************************/
/* Definition functions                                                 */
/************************************************************************/
void Callback_DMA1_Channel1_IRQHandler( DMA_HandleTypeDef * _hdma)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(NotifyADCDone!=NULL)
	{
		LL_GPIO_ResetOutputPin(BATT_LOAD_GPIO_Port,BATT_LOAD_Pin);
		/* Notify the task that the ADC is complete. */
		vTaskNotifyGiveFromISR( NotifyADCDone, &xHigherPriorityTaskWoken );
		/* There are no ADC in progress, so no tasks to notify. */
		NotifyADCDone = NULL;

		portYIELD_FROM_ISR( xHigherPriorityTaskWoken);
	}

}

#if (VERSION_WITH_RING == 1)
void CallbackUartCheck(void const * argument)
{
	tDataQueue sendData;
	sendData.pointer = NULL;

	sendData.address = ADDR_TO_CORE_CHECK_UART_MSG;
	xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);
}

/*
 *
 */
void CallbackRingHB(void const * argument)
{
	tDataQueue sendData;
	sendData.pointer = NULL;

	sendData.address = ADDR_TO_CORE_RING_HB_ELAPSED;
	xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);
}

/*
 *
 */
void CallbackStopTxFromRing(void const * argument)
{
	tDataQueue sendData;
	sendData.pointer = NULL;

	sendData.address = ADDR_TO_CORE_STOP_TX;
	sendData.data = 1;

	xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);
}

#endif

/**
 *
 */
void CallbackBacklight(void const * argument)
{
	if((LL_GPIO_ReadInputPort(SW1_ON_OFF_GPIO_Port) & ALL_KEY_PINS) == ALL_KEY_PINS)
	{	/* jiz neni nic zmacknuto */
		LL_GPIO_ResetOutputPin(BACKLIGHT_GPIO_Port, BACKLIGHT_Pin);
	}

}

/**
 *
 */
void CallbackStartMeasureBatt(void const * argument)
{
	tDataQueue sendData;
	sendData.pointer = NULL;

	sendData.address = ADDR_TO_CORE_START_BAT_MEAS;
	xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);

}

/**
 *
 */
void CallbackStopTX(void const * argument)
{
	tDataQueue sendData;
	sendData.pointer = NULL;

	sendData.address = ADDR_TO_CORE_STOP_TX;
	sendData.data = 0;
	xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);
}


/**
 *
 */
void CallbackLockShock(void const * argument)
{
	tDataQueue sendData;
	sendData.pointer = NULL;

	sendData.address = ADDR_TO_CORE_LOCK_SHOCK_TOOGLE;

	xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);
}

/**
 *
 */
void CallbackChargerState(void)
{
	uint32_t chargerState;

	chargerState = LL_GPIO_ReadInputPort(CHARGER_STAT_GPIO_Port) & CHARGER_STAT_Pin;
//	if(chargerState != GlCharger.pinState)
	{
		GlCharger.pinState = chargerState;
		GlCharger.counter = 0;

		LL_TIM_DisableCounter(TIM22);
		LL_TIM_SetCounter(TIM22, 0);
		LL_TIM_EnableIT_UPDATE(TIM22);
		LL_TIM_EnableCounter(TIM22);
	}

}

/**
 *
 */
void CallbackButtonIRQ(bool init)
{
	uint32_t	TempPort;

	if(init)
	{
		GlKeyboard.BitMask = ALL_KEY_PINS;
		return;
	}

	TempPort = (LL_GPIO_ReadInputPort(SW1_ON_OFF_GPIO_Port) & ALL_KEY_PINS) | GlKeyboard.ProhibitionBitMask;

	if (GlKeyboard.BitMask != TempPort) //na tlacitkach je zmena
	{
		GlKeyboard.BitMask=TempPort;	// ulozim pro dalsi prichod preruseni
		GlKeyboard.Counter = 0;		// reset counteru vzdy pokud je zmena na klavesnici

		LL_TIM_DisableCounter(KEYBOARD_TIMER);
		LL_TIM_SetCounter(KEYBOARD_TIMER, 0);
		LL_TIM_EnableIT_UPDATE(KEYBOARD_TIMER);
		LL_TIM_EnableCounter(KEYBOARD_TIMER);
	}
}


/**
 *
 */
void CallbackBeepEnd(void const * argument)
{
#if (KEYBOARD_SOUND == 1)
	LL_TIM_CC_DisableChannel(TIM3,LL_TIM_CHANNEL_CH4);
#endif

#if (KEYBOARD_VIBRATION == 1)
	LL_GPIO_ResetOutputPin(VIBRATION_GPIO_Port, VIBRATION_Pin);
#endif
}

/**
 *
 */
void CallbackChargerTimeout(void const * argument)
{
	tDataQueue sendData;
	sendData.pointer=NULL;

	sendData.address = ADDR_TO_CORE_CHARGER_CHANGE;
	sendData.data = DATA_TO_CORE_CHARGER_TIMEOUT;

	xQueueSend(QueueCoreHandle, &sendData,portMAX_DELAY);
}


/**
*
*/
void CallbackVibrationDuringShock(void const * argument)
{
	tDataQueue sendData;
	sendData.pointer = NULL;

	sendData.address = ADDRT_TO_CORE_REPEAT_VIBRATION;

	xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);
}


void CallbackUnpairRing(void const * argument)
{
	tDataQueue sendData;
	sendData.pointer = NULL;

	sendData.address = ADDR_TO_CORE_RING_UNPAIR_RING;

	xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);
}

/**
 *
 */
void CallbackCheckChargerStat(void const * argument)
{
	//TODO
}


/**
 *
 */
void CallbackAscendingMode(void const * argument)
{

#if ASCENDING_MODE_ENABLED == true
	tDataQueue sendData;
	sendData.pointer = NULL;
	sendData.address = ADDR_TO_CORE_ASCENDING_BOOST;

	xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);
#endif

}


/*
 *
 */
void KeyboardTimElapsed(TIM_HandleTypeDef *htim)
{
	tDataQueue sendData;
	sendData.pointer=NULL;
	signed portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken=pdFALSE;

	GlKeyboard.Counter++;

	switch (GlKeyboard.Counter)
	{
		case TIMEOUT_SHORT_PRESS:
			if ((GlKeyboard.BitMask&ALL_KEY_PINS) == ALL_KEY_PINS)
			{
				/* jiz neni nic zmacknuto*/
				LL_TIM_DisableCounter(KEYBOARD_TIMER);
				LL_TIM_DisableIT_UPDATE(KEYBOARD_TIMER);
			}

			sendData.address = ADDR_TO_CORE_KEYBOARD_EVENT;
			sendData.data=DATA_TO_CORE_SHORT_PRESS_EVENT;
			sendData.RFU_64 = GlKeyboard.BitMask;

			if(xQueueSendFromISR(QueueCoreHandle,&sendData,&xHigherPriorityTaskWoken)!=pdPASS)
			{
				LogError(1289);
			}
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

			break;

		case TIMEOUT_MED_LONG_PRESS:
			sendData.address = ADDR_TO_CORE_KEYBOARD_EVENT;
			sendData.data=DATA_TO_CORE_MED_LONG_PRESS_EVENT;
			sendData.RFU_64 = GlKeyboard.BitMask;

			if(xQueueSendFromISR(QueueCoreHandle,&sendData,&xHigherPriorityTaskWoken)!=pdPASS)
			{
				LogError(1289);
			}
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
			break;

		case TIMEOUT_LONG_PRESS:
			sendData.address = ADDR_TO_CORE_KEYBOARD_EVENT;
			sendData.data=DATA_TO_CORE_LONG_PRESS_EVENT;
			sendData.RFU_64 = GlKeyboard.BitMask;

			if(xQueueSendFromISR(QueueCoreHandle,&sendData,&xHigherPriorityTaskWoken)!=pdPASS)
			{
				LogError(1289);
			}
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
			break;

		case TIMEOUT_REPEAT_PRESS:
			GlKeyboard.Counter = TIMEOUT_LONG_PRESS ;
			sendData.address = ADDR_TO_CORE_KEYBOARD_EVENT;
			sendData.data=DATA_TO_CORE_REPEATE_PRESS_EVENT;
			sendData.RFU_64 = GlKeyboard.BitMask;

			if(xQueueSendFromISR(QueueCoreHandle,&sendData,&xHigherPriorityTaskWoken)!=pdPASS)
			{
				LogError(1289);
			}
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

			break;
		default:
			break;
	}
}



void CallbackChargerEdgeElapsed(void)
{
	tDataQueue sendData;
	sendData.pointer=NULL;
	signed portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken=pdFALSE;

	GlCharger.counter+=1;

	switch (GlCharger.counter)
	{
		case 200:	//ms

			LL_TIM_DisableCounter(TIM22);
			LL_TIM_DisableIT_UPDATE(TIM22);

			sendData.address = ADDR_TO_CORE_CHARGER_CHANGE;
			sendData.data = DATA_TO_CORE_CHARGER_CHANGE;

			if(xQueueSendFromISR(QueueCoreHandle,&sendData,&xHigherPriorityTaskWoken)!=pdPASS)
			{
				LogError(1289);
			}
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

			break;

		default:
			break;
	}

}

/*
 *
 */
void CORE_CheckEEPROM(tCoreGlobalData *glData)
{
	uint8_t tempShockMax[DOG_CNT];
	uint8_t tempBoosterMax[DOG_CNT];

	// read setting number of dogs
	glData->sharedData.numberOfDogs = SP_ConstrainU8(EA_GetNumOfDogs(), 1, NUMBER_OF_DOGS_MAX);
#if (VER_4DOGS == 0)
	glData->sharedData.numberOfDogs = 2;
#endif

	glData->sharedData.feedbackON = (bool)SP_ConstrainU8(EA_GetFeedbackON(), 0, 1);

	/* selected dog */
	glData->sharedData.selectedDog =(eSelectedDog) SP_ConstrainU8(EA_GetSelectedDog(), DOG_A, (NUMBER_OF_DOGS_MAX-1));
	EA_SetSelectedDog(glData->sharedData.selectedDog);
	RefreshWatchDog();

	/* lock for shock active */
	for(uint8_t i=0;i<(DOG_CNT); i++)
	{
		glData->sharedData.dogsInfo[i].onlyVibration = EA_GetLockActive(i);
		tempShockMax[i] = glData->sharedData.dogsInfo[i].onlyVibration ? VIBRATION_MAX : SHOCK_MAX;
		tempBoosterMax[i] = glData->sharedData.dogsInfo[i].onlyVibration ? 0 : SHOCK_MAX;
	}

	/* lock bottom keys */
	glData->sharedData.lockBottomKeys = SP_ConstrainU8(EA_GetLockBtmKeys(), 0, 1) ;
	EA_SetLockBtmKeys(glData->sharedData.lockBottomKeys);

#if (NC_MODE_AVAILABLE == 1)
	/* mode */
	glData->sharedData.dogsInfo[DOG_A].modeNC = (eMode) SP_ConstrainU8(EA_GetMode(DOG_A), MODE_CON, MODE_NICK);
	glData->sharedData.dogsInfo[DOG_B].modeNC = (eMode) SP_ConstrainU8(EA_GetMode(DOG_B), MODE_CON, MODE_NICK);

#if (VER_4DOGS == 1)
	glData->sharedData.dogsInfo[DOG_C].modeNC = (eMode) SP_ConstrainU8(EA_GetMode(DOG_C), MODE_CON, MODE_NICK);
	RefreshWatchDog();
	glData->sharedData.dogsInfo[DOG_D].modeNC = (eMode) SP_ConstrainU8(EA_GetMode(DOG_D), MODE_CON, MODE_NICK);
	RefreshWatchDog();
	glData->sharedData.dogsInfo[DOG_E].modeNC = (eMode) SP_ConstrainU8(EA_GetMode(DOG_E), MODE_CON, MODE_NICK);
	RefreshWatchDog();
	glData->sharedData.dogsInfo[DOG_F].modeNC = (eMode) SP_ConstrainU8(EA_GetMode(DOG_F), MODE_CON, MODE_NICK);
	RefreshWatchDog();
#endif
	EA_SetMode(DOG_A,glData->sharedData.dogsInfo[DOG_A].modeNC);
	RefreshWatchDog();
	EA_SetMode(DOG_B,glData->sharedData.dogsInfo[DOG_B].modeNC);
	RefreshWatchDog();
#if (VER_4DOGS == 1)
	EA_SetMode(DOG_C,glData->sharedData.dogsInfo[DOG_C].modeNC);
	RefreshWatchDog();
	EA_SetMode(DOG_D,glData->sharedData.dogsInfo[DOG_D].modeNC);
	RefreshWatchDog();
	EA_SetMode(DOG_E,glData->sharedData.dogsInfo[DOG_E].modeNC);
	RefreshWatchDog();
	EA_SetMode(DOG_F,glData->sharedData.dogsInfo[DOG_F].modeNC);
	RefreshWatchDog();
#endif
	RefreshWatchDog();

#endif

	/* shock values */
	glData->sharedData.dogsInfo[DOG_A].shockValue = SP_ConstrainU8(EA_GetShockA(), SHOCK_MIN, tempShockMax[DOG_A]);
	RefreshWatchDog();
	glData->sharedData.dogsInfo[DOG_A].boosterValue = SP_ConstrainU8(EA_GetBoosterA(), BOOSTER_MIN, tempBoosterMax[DOG_A]);
	RefreshWatchDog();
	glData->sharedData.dogsInfo[DOG_B].shockValue = SP_ConstrainU8(EA_GetShockB(), SHOCK_MIN, tempShockMax[DOG_B]);
	RefreshWatchDog();
	glData->sharedData.dogsInfo[DOG_B].boosterValue = SP_ConstrainU8(EA_GetBoosterB(), BOOSTER_MIN, tempBoosterMax[DOG_B]);
	RefreshWatchDog();

#if (VER_4DOGS == 1)
	glData->sharedData.dogsInfo[DOG_C].shockValue = SP_ConstrainU8(EA_GetShockC(), SHOCK_MIN, tempShockMax[DOG_C]);
	RefreshWatchDog();
	glData->sharedData.dogsInfo[DOG_C].boosterValue = SP_ConstrainU8(EA_GetBoosterC(), BOOSTER_MIN, tempBoosterMax[DOG_C]);
	RefreshWatchDog();
	glData->sharedData.dogsInfo[DOG_D].shockValue = SP_ConstrainU8(EA_GetShockD(), SHOCK_MIN, tempShockMax[DOG_D]);
	RefreshWatchDog();
	glData->sharedData.dogsInfo[DOG_D].boosterValue = SP_ConstrainU8(EA_GetBoosterD(), BOOSTER_MIN, tempBoosterMax[DOG_D]);
	RefreshWatchDog();
	glData->sharedData.dogsInfo[DOG_E].shockValue = SP_ConstrainU8(EA_GetShockE(), SHOCK_MIN, tempShockMax[DOG_E]);
	RefreshWatchDog();
	glData->sharedData.dogsInfo[DOG_E].boosterValue = SP_ConstrainU8(EA_GetBoosterE(), BOOSTER_MIN, tempBoosterMax[DOG_E]);
	RefreshWatchDog();
	glData->sharedData.dogsInfo[DOG_F].shockValue = SP_ConstrainU8(EA_GetShockF(), SHOCK_MIN, tempShockMax[DOG_F]);
	RefreshWatchDog();
	glData->sharedData.dogsInfo[DOG_F].boosterValue = SP_ConstrainU8(EA_GetBoosterF(), BOOSTER_MIN, tempBoosterMax[DOG_F]);
	RefreshWatchDog();
#endif

	EA_SetShockA(glData->sharedData.dogsInfo[DOG_A].shockValue);
	RefreshWatchDog();
	EA_SetBoosterA(glData->sharedData.dogsInfo[DOG_A].boosterValue);
	RefreshWatchDog();
	EA_SetShockB(glData->sharedData.dogsInfo[DOG_B].shockValue);
	RefreshWatchDog();
	EA_SetBoosterB(glData->sharedData.dogsInfo[DOG_B].boosterValue);
	RefreshWatchDog();

#if (VER_4DOGS == 1)
	EA_SetShockC(glData->sharedData.dogsInfo[DOG_C].shockValue);
	RefreshWatchDog();
	EA_SetBoosterC(glData->sharedData.dogsInfo[DOG_C].boosterValue);
	RefreshWatchDog();
	EA_SetShockD(glData->sharedData.dogsInfo[DOG_D].shockValue);
	RefreshWatchDog();
	EA_SetBoosterD(glData->sharedData.dogsInfo[DOG_D].boosterValue);
	RefreshWatchDog();
	EA_SetShockE(glData->sharedData.dogsInfo[DOG_E].shockValue);
	RefreshWatchDog();
	EA_SetBoosterE(glData->sharedData.dogsInfo[DOG_E].boosterValue);
	RefreshWatchDog();
	EA_SetShockF(glData->sharedData.dogsInfo[DOG_F].shockValue);
	RefreshWatchDog();
	EA_SetBoosterF(glData->sharedData.dogsInfo[DOG_F].boosterValue);
	RefreshWatchDog();

#endif



}

/**
 *
 */
static uint8_t CORE_StateINIT(tDataQueue receiveData,
		tCoreGlobalData* globalData, tStateCoreAutomat* stateAutomat,
		void** pointerMalloc){

	tDataQueue 		sendData,sendData2;
	sendData.pointer = NULL;
	sendData2.pointer = NULL;
	eStateCoreSystem stateSystem;

	GlCharger.pinState = 0xFFFF;
	globalData->sharedData.chargerState = CHARGER_UNPLUGGED;
	globalData->sharedData.stateBattery = BATTERY_STATUS_UNKNOWN;
	globalData->sharedData.needChargerToLive = false;
	globalData->sharedData.iUseLipol = true;
	osDelayWatchdog(150);	//cekame az se nabije Csample pro mereni ADC
	PCT_ChargerProcedure(DATA_TO_CORE_CHARGER_CHANGE,stateAutomat,globalData);

	stateSystem = EA_GetSystemState();
	if((stateSystem != STATE_CORE_OFF) && (stateSystem != STATE_CORE_ON))
	{
		EA_SetSystemState(STATE_CORE_ON);
		stateSystem = STATE_CORE_ON;
	}

	if(mcuIsLocked()==false) stateSystem = STATE_CORE_ON;

#if(SET_DEFAULT_MAC == 1)
	//uint32_t 	mac=0;
	//mac = EA_GetSystemMAC();
	//if((mac == 0)||(mac == UINT32_MAX))
	{
		EA_SetSystemMAC(DEFAULT_MAC);
	}
#endif

	taskENTER_CRITICAL();
	CallbackButtonIRQ(true);
	globalData->ShortPressLowLevelMask = ALL_KEY_PINS;
	taskEXIT_CRITICAL();

#if (KEYBOARD_SOUND == 1)
	/* PWM sound timer*/
	LL_TIM_EnableCounter(TIM3);
	uint32_t arr=__LL_TIM_CALC_ARR(HAL_RCC_GetSysClockFreq(),LL_TIM_GetPrescaler(TIM3),3400);	//2960
	LL_TIM_SetAutoReload(TIM3,arr);
	LL_TIM_OC_SetCompareCH4(TIM3,(uint16_t)(arr/2));
#endif

	/* timer for keyboard */
	HAL_NVIC_DisableIRQ(TIM7_IRQn);
	HAL_NVIC_ClearPendingIRQ(TIM7_IRQn);
	HAL_NVIC_EnableIRQ(TIM7_IRQn);

	if (stateSystem == STATE_CORE_ON)
	{
		EA_IncResetCounter();	// čítač resetů - při validním změření baterie se čítač vynuluje

		if (EA_IsResetCounterFull())
		{
			if(mcuIsLocked() == true)	stateSystem = STATE_CORE_OFF; // proběhlo několik resetů (ručka se vypne)
		}
	}

#if (VERSION_WITH_RING !=1)
	HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, false);

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7 , 0);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6, 0);

	osTimerStop(TimerRingHeartBeatHandle);
#endif

	if (stateSystem == STATE_CORE_OFF)
	{
		GlKeyboard.ProhibitionBitMask = (SW2_PLUS_Pin|
										SW3_MINUS_Pin|
										SW4_LEFT_Pin|
										SW5_RIGHT_Pin|
										SW6_BEEP_Pin|
										SW7_PULSE_Pin|
										SW8_BOOST_Pin
										);

		EXTI_DisableIT_0_31(LL_EXTI_LINE_0 | LL_EXTI_LINE_1 | LL_EXTI_LINE_2 | LL_EXTI_LINE_3 | LL_EXTI_LINE_4 |
											LL_EXTI_LINE_5 | LL_EXTI_LINE_6 | LL_EXTI_LINE_7 );
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0 | LL_EXTI_LINE_1 | LL_EXTI_LINE_2 | LL_EXTI_LINE_3 | LL_EXTI_LINE_4 |
									LL_EXTI_LINE_5 | LL_EXTI_LINE_6 | LL_EXTI_LINE_7 );

		LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_0);


		sendData.address = ADDR_TO_RF_CHANGE_STATE;
		sendData.data = DATA_TO_RF_START_OFF;

		sendData2.address = ADDR_TO_LCD_CHANGE_STATE;
		sendData2.data = DATA_TO_LCD_START_OFF;

		stateAutomat->PreviousState  = stateAutomat->ActualState;
		stateAutomat->ActualState = STATE_CORE_START_OFF;

		/* disable timers */
		osTimerStop(TimerBacklightHandle);
		osTimerStop(TimerMeasureBattHandle);
		osTimerStop(TimerStopTXHandle);
		osTimerStop(TimerLockShockHandle);

		/* pins go off */
		LL_GPIO_ResetOutputPin(BACKLIGHT_GPIO_Port, BACKLIGHT_Pin);
	}
	else
	{
		GlKeyboard.ProhibitionBitMask = 0;

		EXTI_ClearAndEnableIT_0_31(LL_EXTI_LINE_0 | LL_EXTI_LINE_1 | LL_EXTI_LINE_2 | LL_EXTI_LINE_3 | LL_EXTI_LINE_4 |
										LL_EXTI_LINE_5 | LL_EXTI_LINE_6 | LL_EXTI_LINE_7 );

		sendData.address = ADDR_TO_RF_CHANGE_STATE;
		sendData.data = DATA_TO_RF_START_ON;

		sendData2.address = ADDR_TO_LCD_CHANGE_STATE;
		sendData2.data = DATA_TO_LCD_START_ON;

		stateAutomat->ActualState = STATE_CORE_START_ON;

	}

	osDelayWatchdog(100);
	PCT_StartMeasureBatt(globalData,false);
	osDelayWatchdog(10);
	PCT_StartMeasureBatt(globalData,false);
	osDelayWatchdog(10);
	PCT_StartMeasureBatt(globalData,false);

	xQueueSend(QueueRFHandle, &sendData,portMAX_DELAY);
	xQueueSend(QueueDisplayHandle, &sendData2,portMAX_DELAY);


	return 0;
}

/**
 *
 */
static uint8_t CORE_StateOFF(tDataQueue receiveData,
		tCoreGlobalData *globalData, tStateCoreAutomat *stateAutomat,
		void **pointerMalloc) {

	if((receiveData.address == ADDR_TO_CORE_KEYBOARD_EVENT) &&
			(receiveData.data == DATA_TO_CORE_MED_LONG_PRESS_EVENT))
	{
		KP_KeyBoardActivated(receiveData,globalData,stateAutomat);
	}

	return 0;
}

/**
 *
 */
static uint8_t CORE_StateStartON(tDataQueue receiveData,tCoreGlobalData* globalData, tStateCoreAutomat* stateAutomat,
		void** pointerMalloc){

	typedef struct {
		bool ackTaskRF;
		bool ackTaskDisplay;

	} staticStructure_t, *pStaticStructure_t;

	pStaticStructure_t localStaticStructure;

	if (*pointerMalloc == NULL) {
		localStaticStructure = (pStaticStructure_t) pvPortMalloc(sizeof(staticStructure_t));
		localStaticStructure->ackTaskRF=false;
		localStaticStructure->ackTaskDisplay = false;
		*pointerMalloc = (void*) localStaticStructure;
	} else
		localStaticStructure = (pStaticStructure_t) *pointerMalloc;

	if ((receiveData.address == ADDR_TO_CORE_TASK_STATE_CHANGED) && (receiveData.data == DATA_TO_CORE_RF_IS_ON))
	{
		localStaticStructure->ackTaskRF = true;
	}
	else if ((receiveData.address == ADDR_TO_CORE_TASK_STATE_CHANGED) && (receiveData.data == DATA_TO_CORE_LCD_IS_ON))
	{
		localStaticStructure->ackTaskDisplay = true;
		osTimerStop(TimerChargerHandle);
	}

	if ((localStaticStructure->ackTaskRF) && (localStaticStructure->ackTaskDisplay))
	{
		/* load data from eeprom */
		CORE_CheckEEPROM(globalData);

		globalData->sharedData.actScreen = SCREEN_MAIN;
		globalData->sharedData.activeSettingsNumberOfDogs = false;
		globalData->repeatCounter=0;

		NotifyADCDone = NULL;

		osDelayWatchdog(10);
		stateAutomat->PreviousState = stateAutomat->ActualState;
		stateAutomat->ActualState = STATE_CORE_ON;

		/* kontrola pripojeni nabijecky */
		PCT_ChargerProcedure(DATA_TO_CORE_CHARGER_CHANGE,stateAutomat,globalData);
		/*priprava na dalsi mereni */
		osTimerStart(TimerMeasureBattHandle, 10);

		//osTimerStart(timerChargingCheckHandle, CHARGING_CHECK_PERIOD_MS);

		EA_SetSystemState(STATE_CORE_ON);

		KP_KeyBoardLight(true,false,false,globalData);

		GlKeyboard.ProhibitionBitMask = 0;
		EXTI_ClearAndEnableIT_0_31(LL_EXTI_LINE_0 | LL_EXTI_LINE_1 | LL_EXTI_LINE_2 | LL_EXTI_LINE_3 | LL_EXTI_LINE_4 |
		LL_EXTI_LINE_5 | LL_EXTI_LINE_6 | LL_EXTI_LINE_7 );

#if ASCENDING_MODE_ENABLED == true
		for(uint8_t dg=0;dg<DOG_CNT;dg++)
		{
			RefreshWatchDog();
			if(EA_GetAscendingMode(dg) == true)
			{
				PCT_SetAscendingMode(dg, true, globalData);
			}

			globalData->sharedData.dogsInfo[dg].ascendingStarted = false;
		}
#endif
		vPortFree(localStaticStructure);
		*pointerMalloc = NULL;

#if (VERSION_WITH_RING == 1)
		globalData->sharedData.ringInfo.isPairingMenuActive = false;
		globalData->sharedData.ringInfo.txActiveBy = TX_BY_NONE;

		if((bool) EA_GetNordicState() == true || mcuIsLocked() == false)
		{
			//if(mcuIsLocked() == false)	return 0;
			RefreshWatchDog();
			PCT_ChangeNORDICState(true,true,globalData);
			PCT_RefreshLCD(globalData);
		}
		else
		{
			RefreshWatchDog();
			PCT_ChangeNORDICState(false,true,globalData);
		}

#endif

	}
	return 0;
}

/**
 *
 */
static uint8_t CORE_StateON(tDataQueue receiveData,
		tCoreGlobalData* globalData, tStateCoreAutomat* stateAutomat,
		void** pointerMalloc)
{
#if (VERSION_WITH_RING == 1)
	uart_payloads_u uartPacketRX;
	uint8_t size;
#endif


	switch (receiveData.address)
	{
		case ADDR_TO_CORE_KEYBOARD_EVENT:
			KP_KeyBoardActivated(receiveData,globalData,stateAutomat);
			break;

		case ADDR_TO_CORE_STOP_TX:

#if (VERSION_WITH_RING == 1)
			if(receiveData.data == 1)
			{
				osThreadSuspendAll();
				ringTxStillActive_LOCK = false;
				osThreadResumeAll();
			}
#endif
			PCT_StopTX(globalData);

			break;

		case ADDR_TO_CORE_START_BAT_MEAS:

			if(globalData->sharedData.txActive == TX_ACTIVE_NO)
			{
				PCT_StartMeasureBatt(globalData,false);
				PCT_RefreshLCD(globalData);
				osTimerStart(TimerMeasureBattHandle, ADC_BATT_MEAS_PERIOD);
			}
			else
			{
				osTimerStart(TimerMeasureBattHandle, ADC_BATT_MEAS_IN_TX_POSTPONE);
			}

			break;

		case ADDR_TO_CORE_LOCK_SHOCK_TOOGLE:
			/* je stale zmacknuto=uzemeno? */
			if(((LL_GPIO_ReadInputPort(SW3_MINUS_GPIO_Port)&ALL_KEY_PINS)) == PRESS_SW3_MINUS)
			{
				KP_KeyBoardLight(false,false,false,globalData);
				PCT_LockShockToogle(globalData);
			}

			break;

#if (KEYBOARD_VIBRATION == 1)
		case ADDRT_TO_CORE_REPEAT_VIBRATION:

#if (VERSION_WITH_RING == 1)
			if(globalData->sharedData.ringInfo.txActiveBy != TX_BY_EDGE)
			{
				break;
			}
#endif

			if((globalData->sharedData.txActive == TX_ACTIVE_BOOSTER)
					||(globalData->sharedData.txActive == TX_ACTIVE_SHOCK))
			{
				KP_KeyBoardLight(false,true,false,globalData);
				osTimerStart(TimerVibratDuringShockHandle, PERIOD_VIBRAT_DURING_SHOCK);
			}
			else if((globalData->sharedData.txActive == TX_ACTIVE_BEEP))
			{
				KP_KeyBoardLight(false,false,false,globalData);
				osTimerStart(TimerVibratDuringShockHandle, PERIOD_VIBRAT_DURING_SHOCK);
			}

			break;
#endif

#if (VERSION_WITH_RING == 1)

		case ADDR_TO_CORE_RING_HB_ELAPSED:
			if(globalData->sharedData.ringInfo.ringIsOn == true)
			{

				if(mcuIsLocked() == false)	break;

				PCT_ResetRing(globalData);
				osTimerStart(TimerRingHeartBeatHandle, HB_WATCHDOG_TIME_TO_RESET);
			}

			break;

		case ADDR_TO_CORE_CHECK_UART_MSG:
			if(mcuIsLocked() == false)	break;

			size = UP_CheckUartMsg(&uartPacketRX.raw_paxload[RING_UART_SYNC_SIZE]);
			if(size)
			{
				/*mame prijata validni data po uartu - nordic Zije */
				osTimerStart(TimerRingHeartBeatHandle, HB_WATCHDOG_TIME_TO_RESET);
				PCT_DecodeRingPacket(&uartPacketRX,globalData);
				HAL_GPIO_WritePin(TP204_SCOPE_GPIO_Port, TP204_SCOPE_Pin, 0);
			}

			if(globalData->sharedData.ringInfo.ringIsOn == true)
			{
				osTimerStart(TimerUartCheckHandle, PERIOD_TO_CHECK_RING_RX);
			}

			break;

		case ADDR_TO_CORE_RING_UNPAIR_RING:
			PCT_UnpairRing(globalData);
			break;
#endif

#if ASCENDING_MODE_ENABLED == true
		case ADDR_TO_CORE_ASCENDING_BOOST:
		//case ADDR_TO_CORE_ONE_PACKET_SENT:

			osTimerStart(Timer_AscendingModeHandle, ASCENDING_INCREASING_PERIOD_MS);
			if(globalData->sharedData.dogsInfo[globalData->sharedData.selectedDog].ascendingMode == true &&
				 globalData->sharedData.txActive == TX_ACTIVE_BOOSTER)
			 {
				if(globalData->sharedData.dogsInfo[globalData->sharedData.selectedDog].ascendingShockValue < SHOCK_MAX)
				{
					globalData->sharedData.dogsInfo[globalData->sharedData.selectedDog].ascendingShockValue += ASCENDING_INCREASING_VALUE;
				}
				else
				{
					/* az kdyz mame plnou palbu tak zacina odpocet */
					if (xTimerIsTimerActive(TimerStopTXHandle) != pdTRUE)
					{
						osTimerStart(TimerStopTXHandle, STOP_TX_TIMEOUT_SHOCK);
					}

				}

				globalData->sharedData.dogsInfo[globalData->sharedData.selectedDog].ascendingStarted =true;

				PCT_SendBooster(globalData,globalData->sharedData.dogsInfo[globalData->sharedData.selectedDog].ascendingShockValue,
							globalData->sharedData.selectedDog, true);
			 }


			break;
#endif
		case ADDR_TO_CORE_START_SHUTDOWN:
			PCT_StartShutdown( stateAutomat);
			break;

		default:

			break;

	}


	return 0;
}

/**
 *
 */
static uint8_t CORE_StateStartOFF(tDataQueue receiveData,
		tCoreGlobalData* globalData, tStateCoreAutomat* stateAutomat,
		void** pointerMalloc)
{

	typedef struct
	{
		bool ackTaskRF;
		bool ackTaskDisplay;
	} staticStructure_t, *pStaticStructure_t;

	pStaticStructure_t localStaticStructure;

	if (*pointerMalloc == NULL)
	{
		localStaticStructure = (pStaticStructure_t) pvPortMalloc(sizeof(staticStructure_t));
		localStaticStructure->ackTaskRF=false;
		localStaticStructure->ackTaskDisplay = false;
		*pointerMalloc = (void*) localStaticStructure;
	} else
		localStaticStructure = (pStaticStructure_t) *pointerMalloc;

	if ((receiveData.address == ADDR_TO_CORE_TASK_STATE_CHANGED) && (receiveData.data == DATA_TO_CORE_RF_IS_OFF))
	{
		localStaticStructure->ackTaskRF = true;
	}
	else if ((receiveData.address == ADDR_TO_CORE_TASK_STATE_CHANGED) && (receiveData.data == DATA_TO_CORE_LCD_IS_OFF))
	{
		localStaticStructure->ackTaskDisplay = true;
	}

	if ((localStaticStructure->ackTaskRF) && (localStaticStructure->ackTaskDisplay))
	{

#if (VERSION_WITH_RING == 1)
		globalData->sharedData.ringInfo.isPairingMenuActive = false;
		PCT_ChangeNORDICState(false,false,globalData);
		RefreshWatchDog();
#else
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, false);
		HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, false);
		GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7 , 0);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6, 0);

		osTimerStop(TimerRingHeartBeatHandle);
#endif

		GlKeyboard.ProhibitionBitMask = (SW2_PLUS_Pin|
										SW3_MINUS_Pin|
										SW4_LEFT_Pin|
										SW5_RIGHT_Pin|
										SW6_BEEP_Pin|
										SW7_PULSE_Pin|
										SW8_BOOST_Pin
										);

		EXTI_ClearAndEnableIT_0_31(LL_EXTI_LINE_0);

		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_1 | LL_EXTI_LINE_2 | LL_EXTI_LINE_3 | LL_EXTI_LINE_4 |
						LL_EXTI_LINE_5 | LL_EXTI_LINE_6 | LL_EXTI_LINE_7 );

		EXTI_DisableIT_0_31( LL_EXTI_LINE_1 | LL_EXTI_LINE_2 | LL_EXTI_LINE_3 | LL_EXTI_LINE_4 |
						LL_EXTI_LINE_5 | LL_EXTI_LINE_6 | LL_EXTI_LINE_7 );

		/* disable timers */
		osTimerStop(TimerBacklightHandle);
		osTimerStop(TimerMeasureBattHandle);
		osTimerStop(TimerStopTXHandle);
		osTimerStop(TimerLockShockHandle);

		/* pins go off */
		LL_GPIO_ResetOutputPin(BACKLIGHT_GPIO_Port, BACKLIGHT_Pin);

		EA_SetSystemState(STATE_CORE_OFF);

		vPortFree(localStaticStructure);
		*pointerMalloc = NULL;

		stateAutomat->PreviousState = stateAutomat->ActualState;
		stateAutomat->ActualState = STATE_CORE_OFF;


		// turn off booster
//#if (TURN_OFF_BOOSTER_IN_ON_STATE==1)
		//osDelay(50);
		//LL_GPIO_ResetOutputPin(BOOST_EN_GPIO_Port, BOOST_EN_Pin);
//#endif
	}

	return 0;
}



/**
 *
 * @param argument
 */
void TaskCore(void const * argument)
{
    static tDataQueue receiveData;
    static tCoreGlobalData dataTaskCore;
    static tStateCoreAutomat stateAutomat = {STATE_CORE_INIT,STATE_CORE_INIT};
    static void* pointerToMalloc = NULL;
    static portBASE_TYPE returnValue;

    if(mcuIsLocked()==false)
	{
    	ft_stopTest();
	}

    RefreshWatchDog();
    CORE_StateINIT(receiveData, &dataTaskCore, &stateAutomat, &pointerToMalloc);
    if(mcuIsLocked()==false) ft_InitTest();

    PCT_WatchDogProcedure(WD_TIME_FOR_WATCHDOG);


    for (;;)
    {
    	returnValue = xQueueReceive(QueueCoreHandle, &receiveData, portMAX_DELAY);

		if (returnValue == pdPASS)
		{
			if(receiveData.address==ADDR_TO_CORE_WATCHDOG_ACTIONS)
			{
				PCT_WatchDogProcedure(receiveData.data);
			}
			else if(receiveData.address == ADDR_TO_CORE_CHARGER_CHANGE)
			{
				PCT_ChargerProcedure(receiveData.data,&stateAutomat, &dataTaskCore);
				/* pro znovu vyhodnoceni ?!? */
			//	PCT_ChargerProcedure(receiveData.data,&stateAutomat, &dataTaskCore);
			}
			else if(receiveData.address == ADDR_TO_CORE_FACTORY_TEST_DATA_RX)
			{
				if(mcuIsLocked() == false)
				{
					ft_DecodeRxMsg();
					osTimerStart(TimerFactoryTestHandle, 5);
				}
			}
			else
			{
				StateCORE[stateAutomat.ActualState](receiveData, &dataTaskCore, &stateAutomat,
						&pointerToMalloc);

			}

			/* Clear malloc */
			vPortFree(receiveData.pointer);
			receiveData.pointer=NULL;

		}
    }
}


