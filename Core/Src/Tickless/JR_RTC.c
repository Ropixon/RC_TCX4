/*
 * JR_RTC.c
 *
 *  Created on: 23. 8. 2018
 *      Author: developer
 */

#include "main.h"
#include "TicklessConfig.h"



#define DAYS_IN_LEAP_YEAR                        ( ( uint32_t )  366U )
#define DAYS_IN_YEAR                             ( ( uint32_t )  365U )
#define SECONDS_IN_1DAY                          ( ( uint32_t )86400U )
#define SECONDS_IN_1HOUR                         ( ( uint32_t ) 3600U )
#define SECONDS_IN_1MINUTE                       ( ( uint32_t )   60U )
#define MINUTES_IN_1HOUR                         ( ( uint32_t )   60U )
#define HOURS_IN_1DAY                            ( ( uint32_t )   24U )

#define  DAYS_IN_MONTH_CORRECTION_NORM     ((uint32_t) 0x99AAA0 )
#define  DAYS_IN_MONTH_CORRECTION_LEAP     ((uint32_t) 0x445550 )

#define DIVC( X, N )                                ( ( ( X ) + ( N ) -1 ) / ( N ) )


/*!
 * Number of days in each month on a normal year
 */
static const uint8_t DaysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/*!
 * Number of days in each month on a leap year
 */
static const uint8_t DaysInMonthLeapYear[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


static RTC_HandleTypeDef RtcHandle={0};


/*!
 * Backup the value of the RTC timer when alarm is set,
 * for having time reference for calculate alarm
 * - It is set by function RTC_SetTimerContext
 */
 RtcTimerContext_t RtcTimerContext;

static void RTC_SetConfig();
static void RTC_SetAlarmConfig();

static void RTC_StartWakeUpAlarm( uint32_t timeoutValue,bool isPeriodic );

/*!
 * @brief Initializes the RTC timer
 */
void RTC_Init( void )
{
    RTC_SetConfig( );
    RTC_SetAlarmConfig( );
    RTC_SetTimerContext( );
 }

/*
 * @brief: convert ms to ticks
 */
uint32_t RTC_ms2Tick(uint32_t milliSec)
{
	return ( uint32_t) ( ( ((uint64_t)milliSec) * CONV_DENOM ) / CONV_NUMER );
}

/*
 * @brief: convert ticks to ms
 */
uint32_t RTC_Ticks2ms(uint32_t ticks)
{
	return  ( ( (uint32_t)( ticks )* CONV_NUMER ) / CONV_DENOM );
}

/*!
 * @brief Stop the Alarm
 * @param none
 * @retval none
 */
void RTC_StopAlarm( void )
{
	/* Disable the Alarm A interrupt */
	LL_RTC_DisableWriteProtection(RTC);

	LL_RTC_ALMA_Disable(RTC);
	LL_RTC_DisableIT_ALRA(RTC);

	while(LL_RTC_IsActiveFlag_ALRAW(RTC)==0);
	/* Clear RTC Alarm Flag */
	LL_RTC_ClearFlag_ALRA(RTC);
	LL_RTC_EnableWriteProtection(RTC);
	/* Clear the EXTI's line Flag for RTC Alarm */
	LL_EXTI_DisableIT_0_31(RTC_EXTI_LINE_ALARM_EVENT);
	LL_EXTI_ClearFlag_0_31(RTC_EXTI_LINE_ALARM_EVENT);   //LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_17);
	LL_EXTI_EnableIT_0_31(RTC_EXTI_LINE_ALARM_EVENT);   //LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_17);
	NVIC_ClearPendingIRQ(RTC_IRQn);

}


/*!
 * @brief Set the alarm
 * @note The alarm is set at now (read in this function) + timeout
 * @param timeout Duration of the Timer ticks
 */
void RTC_SetAlarm( uint32_t timeout,bool isPeriodic)
{
  /* Clear interrupt of RTC tim*/

  LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_17);
  LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_17);

  RTC_StartWakeUpAlarm(timeout,isPeriodic);

  /*Enable interrupt from RTC wakeup ALarm*/
  LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
}


/*
 * @brief: SetCoonfig RTC
 */
static void RTC_SetConfig()
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;

	RtcHandle.Instance = RTC;

	RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
	RtcHandle.Init.AsynchPrediv = PREDIV_A; /* RTC_ASYNCH_PREDIV; */
	RtcHandle.Init.SynchPrediv = PREDIV_S; /* RTC_SYNCH_PREDIV; */
	RtcHandle.Init.OutPut = 0;
	RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

	HAL_RTC_Init( &RtcHandle );

#if (ticklessUSE_LSE_CLOCK==1)
	while( __HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET ) {}
#elif (ticklessUSE_LSI_CLOCK==1)
	while( __HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET ) {}
#endif


	/*Friday 1st January 2016*/
	RTC_DateStruct.Year = 0;
	RTC_DateStruct.Month = RTC_MONTH_JANUARY;
	RTC_DateStruct.Date = 1;
	RTC_DateStruct.WeekDay = RTC_WEEKDAY_FRIDAY;
	HAL_RTC_SetDate(&RtcHandle , &RTC_DateStruct, RTC_FORMAT_BIN);

	/*at 0:0:0*/
	RTC_TimeStruct.Hours = 0;
	RTC_TimeStruct.Minutes = 0;

	RTC_TimeStruct.Seconds = 0;
	RTC_TimeStruct.TimeFormat = 0;
	RTC_TimeStruct.SubSeconds = 0;
	RTC_TimeStruct.StoreOperation = RTC_DAYLIGHTSAVING_NONE;
	RTC_TimeStruct.DayLightSaving = RTC_STOREOPERATION_RESET;

	HAL_RTC_SetTime(&RtcHandle , &RTC_TimeStruct, RTC_FORMAT_BIN);

	/* Enable reading time through Shadow. */
	HAL_RTCEx_DisableBypassShadow(&RtcHandle);
}


/**
 * brief:
 */
static void RTC_SetAlarmConfig()
{
	RTC_StopAlarm();
}

/**
 * @brief set Time Reference, set also the global RtcTimerContext
 * @RetVal: Timer value in Ticks?!
 */
uint32_t RTC_SetTimerContext()
{
	RtcTimerContext.Rtc_Time = ( uint32_t ) RTC_GetCalendarValue( &RtcTimerContext.RTC_Calndr_Date, &RtcTimerContext.RTC_Calndr_Time );
	return ( uint32_t ) RtcTimerContext.Rtc_Time;
}

/*!
 * @brief Get the RTC timer elapsed time since the last Alarm was set
 * @param none
 * @retval RTC Elapsed time in ticks
 */
uint32_t RTC_GetTimerElapsedTime( void )
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;

	uint32_t CalendarValue = (uint32_t) RTC_GetCalendarValue(&RTC_DateStruct, &RTC_TimeStruct );
	return( ( uint32_t )( CalendarValue - RtcTimerContext.Rtc_Time ));
}


/*!
 * @brief get current time from calendar in ticks
 * @param pointer to RTC_DateTypeDef
 * @param pointer to RTC_TimeTypeDef
 * @retval time in ticks
 */
uint64_t RTC_GetCalendarValue( RTC_DateTypeDef* RTC_DateStruct, RTC_TimeTypeDef* RTC_TimeStruct )
{
	uint64_t calendarValue = 0;
	uint32_t correction;

	LL_RTC_DisableWriteProtection(RTC);
	while(LL_RTC_IsActiveFlag_RS(RTC)==0);

	RTC_TimeStruct->SubSeconds=LL_RTC_TIME_GetSubSecond(RTC);
	RTC_TimeStruct->Seconds=__LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC));
	RTC_TimeStruct->Minutes=__LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC));
	RTC_TimeStruct->Hours=__LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC));

	RTC_DateStruct->Year=__LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetYear(RTC));
	RTC_DateStruct->Month=__LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetMonth(RTC));
	RTC_DateStruct->Date=__LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetDay(RTC));
	RTC_DateStruct->WeekDay=__LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetWeekDay(RTC));

	LL_RTC_ClearFlag_RS(RTC);
	LL_RTC_EnableWriteProtection(RTC);

	/* calculte amount of elapsed days since 01/01/2000 */
	calendarValue= DIVC( (DAYS_IN_YEAR*3 + DAYS_IN_LEAP_YEAR)* RTC_DateStruct->Year , 4);

	correction = ( (RTC_DateStruct->Year % 4) == 0 ) ? DAYS_IN_MONTH_CORRECTION_LEAP : DAYS_IN_MONTH_CORRECTION_NORM ;

	calendarValue +=( DIVC( (RTC_DateStruct->Month-1)*(30+31) ,2 ) - (((correction>> ((RTC_DateStruct->Month-1)*2) )&0x3)));

	calendarValue += (RTC_DateStruct->Date -1);

	/* convert from days to seconds */
	calendarValue *= SECONDS_IN_1DAY;

	calendarValue += ( ( uint32_t )RTC_TimeStruct->Seconds +
					( ( uint32_t )RTC_TimeStruct->Minutes * SECONDS_IN_1MINUTE ) +
					( ( uint32_t )RTC_TimeStruct->Hours * SECONDS_IN_1HOUR ) ) ;

	calendarValue = (calendarValue<<N_PREDIV_S) + ( PREDIV_S - RTC_TimeStruct->SubSeconds);

	return( calendarValue );
}


/*!
 * @brief start wake up alarm
 * @note  alarm in RtcTimerContext.Rtc_Time + timeoutValue
 * @param timeoutValue in ticks
 * @retval none
 */
static void RTC_StartWakeUpAlarm( uint32_t timeoutValue,bool isPeriodic )
{
	uint16_t rtcAlarmSubSeconds = 0;
	uint16_t rtcAlarmSeconds = 0;
	uint16_t rtcAlarmMinutes = 0;
	uint16_t rtcAlarmHours = 0;
	uint16_t rtcAlarmDays = 0;

	RTC_SetTimerContext();	//save time to global variable

	RTC_TimeTypeDef RTC_TimeStruct = RtcTimerContext.RTC_Calndr_Time;
	RTC_DateTypeDef RTC_DateStruct = RtcTimerContext.RTC_Calndr_Date;

	/*reverse counter */
	rtcAlarmSubSeconds =  PREDIV_S - RTC_TimeStruct.SubSeconds;
	rtcAlarmSubSeconds += ( timeoutValue & PREDIV_S);
	/* convert timeout  to seconds */
	timeoutValue >>= N_PREDIV_S;  /* convert timeout  in seconds */

	/*convert microsecs to RTC format and add to 'Now' */
	rtcAlarmDays = RTC_DateStruct.Date;
	while (timeoutValue >= SECONDS_IN_1DAY)
	{
	timeoutValue -= SECONDS_IN_1DAY;
	rtcAlarmDays++;
	}

	/* calc hours */
	rtcAlarmHours = RTC_TimeStruct.Hours;
	while (timeoutValue >= SECONDS_IN_1HOUR)
	{
	timeoutValue -= SECONDS_IN_1HOUR;
	rtcAlarmHours++;
	}

	/* calc minutes */
	rtcAlarmMinutes = RTC_TimeStruct.Minutes;
	while (timeoutValue >= SECONDS_IN_1MINUTE)
	{
	timeoutValue -= SECONDS_IN_1MINUTE;
	rtcAlarmMinutes++;
	}

	/* calc seconds */
	rtcAlarmSeconds =  RTC_TimeStruct.Seconds + timeoutValue;

	/***** correct for modulo********/
	while (rtcAlarmSubSeconds >= (PREDIV_S+1))
	{
	rtcAlarmSubSeconds -= (PREDIV_S+1);
	rtcAlarmSeconds++;
	}

	while (rtcAlarmSeconds >= SECONDS_IN_1MINUTE)
	{
	rtcAlarmSeconds -= SECONDS_IN_1MINUTE;
	rtcAlarmMinutes++;
	}

	while (rtcAlarmMinutes >= MINUTES_IN_1HOUR)
	{
	rtcAlarmMinutes -= MINUTES_IN_1HOUR;
	rtcAlarmHours++;
	}

	while (rtcAlarmHours >= HOURS_IN_1DAY)
	{
	rtcAlarmHours -= HOURS_IN_1DAY;
	rtcAlarmDays++;
	}

	if( RTC_DateStruct.Year % 4 == 0 )
	{
	if( rtcAlarmDays > DaysInMonthLeapYear[ RTC_DateStruct.Month - 1 ] )
	{
	  rtcAlarmDays = rtcAlarmDays % DaysInMonthLeapYear[ RTC_DateStruct.Month - 1 ];
	}
	}
	else
	{
	if( rtcAlarmDays > DaysInMonth[ RTC_DateStruct.Month - 1 ] )
	{
	  rtcAlarmDays = rtcAlarmDays % DaysInMonth[ RTC_DateStruct.Month - 1 ];
	}
	}

	LL_RTC_DisableWriteProtection(RTC);

	/* Disable the Alarm A interrupt */
	LL_RTC_ALMA_Disable(RTC);

	/* Clear flag alarm A */
	LL_RTC_ClearFlag_ALRA(RTC);
	// __HAL_RTC_ALARM_CLEAR_FLAG(RTC, RTC_FLAG_ALRAF);

	while (LL_RTC_IsActiveFlag_ALRAW(RTC)==0);

	LL_RTC_ALMA_SetSecond(RTC,__LL_RTC_CONVERT_BIN2BCD(rtcAlarmSeconds));
	LL_RTC_ALMA_SetMinute(RTC,__LL_RTC_CONVERT_BIN2BCD(rtcAlarmMinutes));
	LL_RTC_ALMA_SetHour(RTC,__LL_RTC_CONVERT_BIN2BCD(rtcAlarmHours));
	LL_RTC_ALMA_SetDay(RTC,__LL_RTC_CONVERT_BIN2BCD(rtcAlarmDays));


	if(isPeriodic==true)
	{
	  LL_RTC_ALMA_SetMask(RTC, LL_RTC_ALMA_MASK_ALL);
	  LL_RTC_ALMA_SetSubSecondMask(RTC,3);
	}
	else
	{
	  LL_RTC_ALMA_SetMask(RTC, LL_RTC_ALMA_MASK_NONE);
	  LL_RTC_ALMA_SetSubSecondMask(RTC,15);
	}

	LL_RTC_ALMA_SetSubSecond(RTC,PREDIV_S-rtcAlarmSubSeconds);

	LL_RTC_ALMA_Enable(RTC);
	LL_RTC_EnableIT_ALRA(RTC);

	LL_EXTI_EnableIT_0_31(RTC_EXTI_LINE_ALARM_EVENT);   //LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_17);
	LL_EXTI_EnableRisingTrig_0_31(RTC_EXTI_LINE_ALARM_EVENT);

	LL_RTC_EnableWriteProtection(RTC);
}

