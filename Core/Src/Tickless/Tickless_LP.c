/*
 * Tickless_LP.c
 *
 *  Created on: 14. 8. 2018
 *      Author: developer
 */

#include "main.h"
#include "sleepmgr.h"
#include "TicklessConfig.h"



extern RtcTimerContext_t RtcTimerContext;

/* Flag set from the tick interrupt to allow the sleep processing to know if
sleep mode was exited because of an tick interrupt or a different interrupt. */
static volatile uint8_t ucTickFlag = pdFALSE;
static unsigned long ulTicksForOneMilliSec;

static eSleepModes GetPossibleDepthOfSleep(void);
static void PreSleepProcess(eSleepModes ActualSleepMode);
static void PostSleepProcess(eSleepModes ActualSleepMode);

#if (TRC_USE_TRACEALYZER_RECORDER == 1)
	volatile traceString TicklessChannel;;
#endif

/**
 *
 * @param hrtc
 */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	SystemTimerExpired();
	HAL_IncTick();
}

/**
 *
 * @return
 */
static eSleepModes GetPossibleDepthOfSleep()
{
#ifdef ticklessUSE_DYNAMIC_SLEEP
	return DS_GetPossibleDepthOfSleep();
#else
	return sleepmgr_get_sleep_mode();
#endif
}
/**
 *
 * @param ulExpectedIdleTime
 */
static void PreSleepProcess(eSleepModes ActualSleepMode)
{

	if(ActualSleepMode>=SLEEPMGR_STOP_RTC_RUN)
	{
//#if (TURN_OFF_BOOSTER_IN_ON_STATE==1)	//TODO
		LL_GPIO_ResetOutputPin(BOOST_EN_GPIO_Port, BOOST_EN_Pin);
//#endif

		GPIO_InitTypeDef initStruct={0};

		initStruct.Mode =GPIO_MODE_INPUT;
		initStruct.Pull =GPIO_PULLUP ;
		initStruct.Pin=RF_MOSI_Pin|RF_MISO_Pin/*|SI_SCK_Pin*/;
		HAL_GPIO_Init(RF_MOSI_GPIO_Port,&initStruct );

		initStruct.Mode =GPIO_MODE_INPUT;
		initStruct.Pull =GPIO_PULLDOWN;
		initStruct.Pin=RF_SCK_Pin;
		HAL_GPIO_Init(RF_SCK_GPIO_Port,&initStruct );

		initStruct.Mode =GPIO_MODE_INPUT;
		initStruct.Pull =GPIO_PULLUP ;
		initStruct.Pin=RF_NSEL_Pin;
		HAL_GPIO_Init(RF_NSEL_GPIO_Port,&initStruct );

		initStruct.Mode =GPIO_MODE_INPUT;
		initStruct.Pull =GPIO_PULLUP;
		initStruct.Pin=ADC_BATT_Pin;
		HAL_GPIO_Init(GPIOA,&initStruct );

	}
}


/**
 *
 * @param ulExpectedIdleTime
 */
static void PostSleepProcess(eSleepModes ActualSleepMode)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};

	if(ActualSleepMode>=SLEEPMGR_STOP_RTC_RUN)
	{
//#if (TURN_OFF_BOOSTER_IN_ON_STATE==1)
		LL_GPIO_SetOutputPin(BOOST_EN_GPIO_Port, BOOST_EN_Pin);
//#endif

		GPIO_InitStruct.Pin = RF_SCK_Pin|RF_MISO_Pin|RF_MOSI_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = RF_NSEL_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(RF_NSEL_GPIO_Port, &GPIO_InitStruct);

	}

}



/**
 * __attribute__((weak))
 */
void prvSetupTimerInterrupt(void)
{
	/* Enable the RTC interrupt. This must be executed at the lowest interrupt priority. */

	//HAL_NVIC_SetPriority(RTC_IRQn, configLIBRARY_LOWEST_INTERRUPT_PRIORITY, 0);	// Pro Cortex M3+M4 nastavit LOWEST_PRIORITY
	HAL_NVIC_EnableIRQ(RTC_IRQn);

	/* Clear flag for RTC interrupt */
	LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_17);
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_17);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);

	/*configure the RTC*/
	RTC_Init();

	ulTicksForOneMilliSec=RTC_ms2Tick(ticklessFREERTOS_PERIOD_IN_MS);
	RTC_SetAlarm(ulTicksForOneMilliSec,true);

#if (TRC_USE_TRACEALYZER_RECORDER == 1)
	TicklessChannel = xTraceRegisterString("RF_TASK");
#endif

}


/**
 *
 */
void SystemTimerExpired(void)
{
	/* The CPU woke because of a RTC tick. */
	ucTickFlag = pdTRUE;

	xPortSysTickHandler();
}

/**
 *
 * @param xExpectedIdleTime
 */
 void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
 {
	eSleepModeStatus 		eSleepAction;
	uint32_t 				ulCompleteTickPeriods;
	uint32_t 				ExcpectedTicksInSleep=0;
	RtcTimerContext_t 		UnusedTime;
	uint64_t 				ulTimeBeforeSleep;
	uint64_t 				ulTimeAfterSleep;
	eSleepModes 			ActualSleepMode;

#if defined(ENABLE_SLEEP) && (ENABLE_SLEEP == 0)
	return;
#endif
	/* To avoid race conditions, enter a critical section.  */
	__disable_irq();

	/* Get Level of enabled sleep and sleep...*/
	ActualSleepMode=GetPossibleDepthOfSleep();

	eSleepAction = eTaskConfirmSleepModeStatus();

#if (ticklessENABLE_SLEEP==0)
	eSleepAction=eAbortSleep;
#endif

	if ((eSleepAction == eAbortSleep)||(ActualSleepMode==SLEEPMGR_ACTIVE)||(xExpectedIdleTime<ticklessMIN_TIM_TO_ENTER_SLEEP_MODE_IN_MS))
	{

#if (TRC_USE_TRACEALYZER_RECORDER == 1)
	vTracePrint(TicklessChannel,"Sleep aborted because: \n");
	vTracePrintF(TicklessChannel,"sleep mode is: %d\n",sleepmgr_get_sleep_mode());
	vTracePrintF(TicklessChannel,"expected time in sleep\n",xExpectedIdleTime);
#endif

#if (SEGGER_VYSTUP==1)
	SEGGER_RTT_WriteString(0,"Sleep aborted because: \n " );
	SEGGER_RTT_printf(0,"sleep mode is: %d\n",sleepmgr_get_sleep_mode());
	SEGGER_RTT_printf(0,"expected time in sleep \n", xExpectedIdleTime );
#endif

		/* Re-enable interrupts. */
		__enable_irq();
		return;

	}
	else if (eSleepAction == eNoTasksWaitingTimeout)	//no pending tasks - enter the deep sleep
	{
		RTC_StopAlarm( );

		PreSleepProcess(ActualSleepMode);

		/* Go sleep. */
		sleepmgr_sleep(ActualSleepMode);

		PostSleepProcess(ActualSleepMode);

		RTC_SetAlarm(ulTicksForOneMilliSec,true);

		/* Re-enable interrupts. */
		__enable_irq();

	}else	//wait for some Freertos time
	{

		RTC_StopAlarm();

		ExcpectedTicksInSleep=RTC_ms2Tick(xExpectedIdleTime);	//input is in ms

		ucTickFlag=pdFALSE;
		ulTimeBeforeSleep=RTC_GetCalendarValue( &UnusedTime.RTC_Calndr_Date, &UnusedTime.RTC_Calndr_Time );

		RTC_SetAlarm(ExcpectedTicksInSleep,false);

		PreSleepProcess(ActualSleepMode);

		/* Go sleep. */
		sleepmgr_sleep(ActualSleepMode);

		PostSleepProcess(ActualSleepMode);

		__enable_irq();
		/* Now the RTC IRQ handler can be called*/

		__disable_irq();

		ulTimeAfterSleep=RTC_GetCalendarValue(&UnusedTime.RTC_Calndr_Date, &UnusedTime.RTC_Calndr_Time);
		ulCompleteTickPeriods=RTC_Ticks2ms((uint32_t)(ulTimeAfterSleep-ulTimeBeforeSleep));
		if(ucTickFlag == pdFALSE)
		{
			/*  RTC alarm je stale nastaven - zastav ho*/
			RTC_StopAlarm();
		}

		/* If the mcu has been woken up by system timer. */
		if (ucTickFlag == pdTRUE)
		{
			RTC_SetAlarm(ulTicksForOneMilliSec,true);
		}
		else
		{
			/* Something other than the tick interrupt ended the sleep.  How
			many complete tick periods passed while the processor was
			sleeping? */

			/* We have some fraction below FREERTOS_PERIOD. */
			if(ulCompleteTickPeriods<ticklessFREERTOS_PERIOD_IN_MS )	ulCompleteTickPeriods++;

			RTC_SetAlarm(ulTicksForOneMilliSec,true);
		}

		/* Wind the tick forward by the number of tick periods that the CPU	remained in a low power state. */
		if(ulCompleteTickPeriods>xExpectedIdleTime)	ulCompleteTickPeriods=xExpectedIdleTime;
		vTaskStepTick(ulCompleteTickPeriods);

		/* Re-enable interrupts. */
		__enable_irq();

	}

 }

