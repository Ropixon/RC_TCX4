/*
 * sleepmgr.h
 *
 *  Created on: 2.5.2019
 *      Author: developer
 */

#ifndef SLEEPMGR_H_
#define SLEEPMGR_H_

#include "main.h"
#include "interrupt_sam_nvic.h"


typedef enum {
	/** Active mode. */
	SLEEPMGR_ACTIVE = 0,

	SLEEPMGR_SLEEP = 1,
	SLEEPMGR_STOP_RTC_RUN,
	//SLEEPMGR_STOP_RTC_STOP,	//neni hotove
	//SLEEPMGR_STANDBY,

	SLEEPMGR_NR_OF_MODES,

}eSleepModes;


static inline void sleepmgr_sleep(eSleepModes sleep_mode)
{
//	HAL_GPIO_WritePin(VIBRATION_GPIO_Port, VIBRATION_Pin, 1);	// scope pin
	switch (sleep_mode)
	{
		case SLEEPMGR_ACTIVE:

			break;
		case SLEEPMGR_SLEEP:
			//HAL_GPIO_WritePin(TP205_SCOPE_GPIO_Port, TP205_SCOPE_Pin, 0);
			HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);
			break;
		case SLEEPMGR_STOP_RTC_RUN:
		//	HAL_GPIO_WritePin(TP205_SCOPE_GPIO_Port, TP205_SCOPE_Pin, 0);
			LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_MSI);		// musi byt podle errata - jinak se resetuje po vzbuzeni
			LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);	// See in Errata note
			HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_SLEEPENTRY_WFI);

			LL_RCC_HSI_Enable();
			while((__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET));
			while((__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET));

			LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

			break;

		default:
			break;
	}

//	HAL_GPIO_WritePin(VIBRATION_GPIO_Port, VIBRATION_Pin, 0);	// scope pin
	//HAL_GPIO_WritePin(TP205_SCOPE_GPIO_Port, TP205_SCOPE_Pin, 1);

}


#endif /* SLEEPMGR_H_ */
