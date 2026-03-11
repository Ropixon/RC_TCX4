/*
 * JR_RTC.h
 *
 *  Created on: 23. 8. 2018
 *      Author: developer
 */

#ifndef JR_RTC_H_
#define JR_RTC_H_

#include "main.h"

/* Delicky musi po vydeleni zdroje hodin vyjit na 1 HZ
 * As soon as the subsecond downcounter reachs zero, the RTC_SSR is reload with PREDIV_S
 * PREDIV_S - max is 0-32768
 * PREDIV_A - max is 0-127
 */

/* Nezapomenout zmenit AlarmSubSecondMask  */
/* subsecond number of bits */
#define N_PREDIV_S                 13

/* Synchonuous prediv  */
#define PREDIV_S                  ((1<<N_PREDIV_S)-1)
/* Asynchonuous prediv   */
#define PREDIV_A                  (1<<(15-N_PREDIV_S))-1

//#define PREDIV_S		(8192-1)//7400-1//3699//7400-1//3699*2//3699//18500 a 1	//Granulanity is 1/PREDIV_S it is equal about to 270 us(1/3700)
//#define PREDIV_A		(4-1)//5-1//9//5-1//9//

/* RTC Time base in us */
#define USEC_NUMBER               1000000
#define MSEC_NUMBER               (USEC_NUMBER/1000)
#define RTC_ALARM_TIME_BASE       (USEC_NUMBER>>N_PREDIV_S)

#define COMMON_FACTOR        	   3
#define CONV_NUMER                (MSEC_NUMBER>>COMMON_FACTOR)
#define CONV_DENOM                (1<<(N_PREDIV_S-COMMON_FACTOR))


typedef struct
{
  uint32_t  Rtc_Time; /* Reference time */

  RTC_TimeTypeDef RTC_Calndr_Time; /* Reference time in calendar format */

  RTC_DateTypeDef RTC_Calndr_Date; /* Reference date in calendar format */

} RtcTimerContext_t;

void RTC_Init( void );
void RTC_SetAlarm( uint32_t timeout,bool isPeriodic);
uint32_t RTC_SetTimerContext();
void RTC_StopAlarm( void );
uint32_t RTC_ms2Tick(uint32_t milliSec);
uint32_t RTC_Ticks2ms(uint32_t ticks);
uint64_t RTC_GetCalendarValue( RTC_DateTypeDef* RTC_DateStruct, RTC_TimeTypeDef* RTC_TimeStruct );
uint32_t RTC_GetTimerElapsedTime( void );
#endif /* JR_RTC_H_ */
