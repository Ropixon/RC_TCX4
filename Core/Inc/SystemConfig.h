/*
 * SystemConfig.h
 *
 *  Created on: Jul 22, 2021
 *      Author: developer
 */

#ifndef INC_SYSTEMCONFIG_H_
#define INC_SYSTEMCONFIG_H_


/* hlavni nastaveni projektu */
#define RUN_WITH_WATCHDOG						(1)	//release 1 //TODO VYROBA
#define RESET_WHEN_ERROR						(1)	//release 1 //TODO VYROBA
#define DEBUG_IN_STOP_MODE						(0)	//release 0 //TODO VYROBA vypnout - zvysuje spotrebu!
#define DEBUG_PRETEND_MCU_IS_LOCKED				(1)	//release 0
#define TURN_OFF_BOOSTER_IN_ON_STATE 			(1) //release 1 // 1 = vypíná booster i v zapnutém stavu
#define SET_DEFAULT_MAC							(0)	//release 0

#if SET_DEFAULT_MAC == 1
#define DEFAULT_MAC			0x112
#endif

#define EDGE_TYPE_250							(0)
#define EDGE_TYPE_450							(0) // pro verzi R jako 250
#define EDGE_TYPE_650							(1)	// pro verzi R jako 650 + 250
#define VERSION_WITH_RING						(1)

#if (DEBUG_IN_STOP_MODE == 1)
#define ENABLE_SLEEP							(0)
#endif

#define	KEYBOARD_SOUND							(1)		//nechavame zaple, pouze se neosadi HW
#define KEYBOARD_VIBRATION						(1)		//nechavame zaple, pouze se neosadi HW
#define VER_4DOGS								(1)		// nyni predelano na 6 psu - uzivatel si sam voli kolik jich bude 1 - 6
#define NC_MODE_AVAILABLE						(1)
#define DOUBLE_CLICK_TO_LOCK_KEYS				(1)		// zamykani pomoci rychleho dvojliku ON_OFF, 0 = jednoduchy klik


#define BEEP_VIBRAT_POWER						(5)

/* P R S T Y N E K	*/
#if (VERSION_WITH_RING == 1)
	#define ASCENDING_MODE_ENABLED					(true)
	#define PERIOD_TO_CHECK_RING_RX					(20)
	#define STOP_TX_AFTER_LAST_RX_TIMEOUT_MS		((uint32_t)(240 * 1.12))
	#define STOP_TX_AFTER_LAST_RX_TIMEOUT_PRG_MS	(STOP_TX_AFTER_LAST_RX_TIMEOUT_MS + 400)
	#define HB_WATCHDOG_TIME_TO_RESET				(200)
	#define RING_MAX_ALLOWED_RING					(8)
	#define RING_UNPAIR_TIMEOUT						(15*CONST_ONE_SEC)
#endif


#if ASCENDING_MODE_ENABLED == true
	#define ASCENDING_VALUE_ZERO_OR_SHOCK			(false)	//true je pro zcatek od 0, false je pro aktualni hodnotu shock
	#define ASCENDING_INCREASING_VALUE				(1)
	#define ASCENDING_INCREASING_PERIOD_MS			(400)	//175
#endif

// Define timeout - key events
#define TIMEOUT_SHORT_PRESS			(1)		//-
#define TIMEOUT_MED_LONG_PRESS		(18)	//360 s tim6 period 1
#define TIMEOUT_LONG_PRESS			(20)	//360 s tim6 period 1
#define TIMEOUT_REPEAT_PRESS		(22)	//
#define TIMEOUT_SHONG_PRESS			(TIMEOUT_REPEAT_PRESS-TIMEOUT_LONG_PRESS)

#define SUPER_LONG_PRESS			(20)	//	je jiny druh konstanty nez ty vyse

#define RADIO_PA_HIGH_PWR 				0x7F		// max power (20dBm)
#define RADIO_PA_MID_PWR 				0x23		//  power (+- 15 dBm)
#define RADIO_PA_LOW_PWR 				0x14		//  power (+- 10 dBm)

#if (EDGE_TYPE_250 == 1)
#define RADIO_POWER				RADIO_PA_LOW_PWR
#elif (EDGE_TYPE_450 == 1)
#define RADIO_POWER				RADIO_PA_MID_PWR
#elif (EDGE_TYPE_650 == 1)
#define RADIO_POWER				RADIO_PA_HIGH_PWR
#endif


#define RADIO_PA_LOW_PWR_TESTER			0x0

#define PWR_IN_DATA_HIGH				0			// 20dBm = 0
#define PWR_IN_DATA_MID					-6			// 10dBm = -10
#define PWR_IN_DATA_LOW					-10			// 10dBm = -10

// MASTER SETTINGS //
// --------------- //
#define RF_POWER						RADIO_POWER
#define MAX_NUMBER_OF_DOGS				(6)//(6/2)


#ifdef TESTER_TX
#define RF_POWER						RADIO_PA_LOW_PWR_TESTER
#endif


#define CONST_ONE_SEC							(1120)		//LSI kmita na 37000 misto pocitaneho 32768
#define CONST_ONE_MIN							(67200)
#define CONST_ONE_HOUR							(4032000)

#define SHOCK_MIN					(0)
#define SHOCK_MAX					(30)
#define BOOSTER_MIN					(0)
#define BOOSTER_MAX					(30)
#define VIBRATION_MAX				(4)
#define VIBRATION_MIN				(1)
#define NUMBER_OF_DOGS_MAX			MAX_NUMBER_OF_DOGS
#define NUMBER_OF_RESET_MAX			(5)


#define BACKLIGHT_TIMEOUT			(5*CONST_ONE_SEC)
#define BEEP_TIMEOUT				(50)
#define VIBRAT_SHORT_PRESS_TIMEOUT	(40)
#define STOP_TX_TIMEOUT_BEEP		(30*CONST_ONE_SEC)
#define STOP_TX_TIMEOUT_SHOCK		(8*CONST_ONE_SEC)
#define STOP_TX_TIMEOUT_NICK		(200)
#define CHARGING_CHECK_PERIOD_MS	(1500)


#define ADC_BATT_MEAS_PERIOD			(10*CONST_ONE_MIN)	// 10 minut
#define ADC_BATT_MEAS_IN_TX_POSTPONE	(10*CONST_ONE_SEC)	// 10 sec
#define ADC_BATT_MEAS_PERIOD_AFTER_TX	(1*CONST_ONE_SEC)	// 10 sec
#define LOCK_SHOCK_TIMEOUT			(3*CONST_ONE_SEC)
#define PERIOD_VIBRAT_DURING_SHOCK	(80)
#define CHARGER_UNPLGED_LCD_TIMEOUT	(5*CONST_ONE_SEC)	//vy vypnutem stavu pro zhasnuti batt pri odpojeni nabijecky


#define NEW_BATT_VOLTAGE_NO_LOAD        3000

#define HIGH_TEMP_LEVEL                 20.0f
#define NEW_BATT_VOLTAGE                2400
#define BATT_HIGH_VOLTAGE               2200    // cca 30 % cap
#define BATT_MED_VOLTAGE                2150     // cca 20 %
#define BATT_LOW_VOLTAGE                 2100   // cca 10 % cap

#define LOW_TEMP_LEVEL                  -18.0f
#define NEW_BATT_VOLTAGE_LOW            1900
#define BATT_HIGH_VOLTAGE_LOW           1700 // cca 30 % cap
#define BATT_MED_VOLTAGE_LOW            1650 // cca 20 % cap
#define BATT_LOW_VOLTAGE_LOW            1600 // cca 10 % cap


//#define BAT_CAPACITY_60_PERCENT_V 	2250	//2300
//#define BAT_CAPACITY_30_PERCENT_V 	2150	//2190
//#define BAT_CAPACITY_10_PERCENT_V 	2010

#define LIP_CAPACITY_70_PERCENT_V 	3930	//3900
#define LIP_CAPACITY_50_PERCENT_V 	3820	//
#define LIP_CAPACITY_30_PERCENT_V 	3650
#define LIP_CAPACITY_10_PERCENT_V 	3550
#define LIP_CAPACITY_5_PERCENT_V 	3300

#define ACUMULATOR_DETECT_THRESHOLD	500	//mV

#define BATT_MEAS_DELAY				1500//200

typedef enum
{
	HW_VERS_RF_LOW_PA = 0,
	HW_VERS_RF_HIGH_PA = 1,
	HW_VERS_RESERVED_1 = 2,
	HW_VERS_RESERVED_2 = 3,

}eHW_Version;

eHW_Version SC_GetHWVersion(void);

#endif /* INC_SYSTEMCONFIG_H_ */
