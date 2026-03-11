/*
 * TicklessConfig.h
 *
 *  Created on: 22. 4. 2020
 *      Author: developer
 */

#ifndef TICKLESS_TICKLESSCONFIG_H_
#define TICKLESS_TICKLESSCONFIG_H_

#include "main.h"
#include "JR_RTC.h"
#include "Tickless_LP.h"
#include "DynamicSleep.h"


/**
 * CONFIG AREA
 */
#define ticklessFREERTOS_PERIOD_IN_MS					(configTICK_RATE_HZ/1000)
#define ticklessMIN_TIM_TO_ENTER_SLEEP_MODE_IN_MS		(10)
#define ticklessUSE_LSI_CLOCK							(1)
#define ticklessUSE_LSE_CLOCK							(0)
#define ticklessUSE_DYNAMIC_SLEEP						(1)
#define ticklessENABLE_SLEEP							(1)

#define TICK_IN_ONE_SEC									(PREDIV_S)
#define MIN_TIME_TO_ENTER_STOPMODE_IN_TICK				RTC_ms2Tick(ticklessMIN_TIM_TO_ENTER_SLEEP_MODE_IN_MS)

#if (ticklessUSE_LSI_CLOCK==ticklessUSE_LSE_CLOCK)
#error "Define source for RTC timer"
#endif

#if (ticklessMIN_TIM_TO_ENTER_SLEEP_MODE_IN_MS<5)
#error "Min time to Enter sleep mode has to be equal or great than 5 ms"
#endif

#endif /* TICKLESS_TICKLESSCONFIG_H_ */
