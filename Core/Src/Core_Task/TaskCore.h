/*
 * TaskCore.h
 *
 *  Created on: 25. 3. 2021
 *      Author: jirik
 */

#ifndef INC_FREERTOS_TASKS_TASKCORE_H_
#define INC_FREERTOS_TASKS_TASKCORE_H_

#include "main.h"
/************************************************************************/
/* Area #DEFINE															*/
/************************************************************************/
#define ADC_BATT_BUFFER_SIZE		(8)	//kazdou hodnotu 2x kanaly+referenci+temp
#define ADC_EACH_CHANNEL_MEAS_CNT	(2)


#define KEYBOARD_TIMER						(TIM7)
#define SYSTEM_ADC							(ADC1)
#define SYSTEM_DMA_FOR_ADC					(DMA1)
#define ALL_KEY_PINS	(SW1_ON_OFF_Pin | SW2_PLUS_Pin | SW3_MINUS_Pin | SW4_LEFT_Pin | SW5_RIGHT_Pin \
						| SW6_BEEP_Pin | SW7_PULSE_Pin | SW8_BOOST_Pin )

#define PRESS_SW1_ON_OFF	(ALL_KEY_PINS & (~SW1_ON_OFF_Pin))
#define PRESS_SW2_PLUS		(ALL_KEY_PINS & (~SW2_PLUS_Pin))
#define PRESS_SW3_MINUS		(ALL_KEY_PINS & (~SW3_MINUS_Pin))
#define PRESS_SW4_1_LEFT	(ALL_KEY_PINS & (~SW4_LEFT_Pin))
#define PRESS_SW5_2_RIGHT	(ALL_KEY_PINS & (~SW5_RIGHT_Pin))
#define PRESS_SW6_BEEP		(ALL_KEY_PINS & (~SW6_BEEP_Pin))
#define PRESS_SW7_PULSE		(ALL_KEY_PINS & (~SW7_PULSE_Pin))
#define PRESS_SW8_BOOST		(ALL_KEY_PINS & (~SW8_BOOST_Pin))

#define KEY_PRESSED			0




/************************************************************************/
/* MAKRO with parameters                                                */
/************************************************************************/

/************************************************************************/
/* Area TYPEDEF                                                         */
/************************************************************************/

typedef enum
{
	DOG_A = 0,
	DOG_B = 1,
#if (VER_4DOGS == 1)
	DOG_C = 2,
	DOG_D = 3,
	DOG_E = 4,
	DOG_F = 5,
#endif
	DOG_CNT

}eSelectedDog;


/**
 *
 */
typedef enum
{
	BATTERY_STATUS_NONE = 0,
	BATTERY_STATUS_CRITICAL,
	BATTERY_STATUS_LOW,
	BATTERY_STATUS_MED,
	BATTERY_STATUS_HIGH,
	BATTERY_STATUS_UNKNOWN,
} eBatteryStatus;


typedef enum
{
	SCREEN_NONE = 0,
	SCREEN_ONLY_CHARGER = 1,
	SCREEN_MAIN = 2

}eActualScreen;

typedef enum
{
	MODE_CON = 0,
	MODE_NICK = 1,

}eMode;



typedef struct
{
	uint8_t 	shockValue;
	uint8_t 	boosterValue;
	eMode		modeNC;
	bool    	activeBooter;
	bool		onlyVibration;
#if ASCENDING_MODE_ENABLED == true
	bool		ascendingMode;	//pro booster zvysujici se rezim rany
	uint8_t		ascendingShockValue;	//aktualni hodnota pro odeslani rany
	bool		ascendingStarted;
#endif
}tDogs;

typedef enum
{
	TX_ACTIVE_NO = 0,
	TX_ACTIVE_BEEP = 1,
	TX_ACTIVE_SHOCK = 2,
	TX_ACTIVE_BOOSTER = 3,

}eTxActive;

/**
 *
 */
typedef enum {
	MOD_BIG_SHOCK=0,
	MOD_MED_SHOCK,
	MOD_VIBR,
	MOD_LIGHT,
	MOD_BEEP,
}eProtocolMode;

/**
 *
 */
typedef enum {
	VIBRATION_OFF=0,
	VIBRATION_IP_M1,
	VIBRATION_CV_M2,
	VIBRATION_3P_M3,
	VIBRATION_1P_M4,
	VIBRATION_2P_M5
}eProtocolVibrationMode;

/**
 *
 */
typedef enum{
	CHARGER_NONE = 0,
	CHARGER_RUNNING = 1,
	CHARGER_BATT_FULL = 2,	//batt is full
	CHARGER_UNPLUGGED = 3
}eChagerState;

/*
 *
 */
typedef enum{
	RING_FCE_NONE		=	0,
	RING_FCE_SHOCK 		= 1,
	RING_FCE_BEEP		=	2,
	RING_FCE_BOOSTER	=	3,

	RING_FCE_CNT,

}eSelectedBtnForRing;

/*
 *
 */
typedef enum{
	TX_BY_NONE	=	0,
	TX_BY_NORDIC = 1,
	TX_BY_EDGE	=	2,

}eTXActiveBy;

/*
 *
 */
typedef struct{
	bool					isPairingMenuActive;	//jsme v menu pro parovani prstynku?
	uint8_t					ringNumber;	//1-8
	eSelectedBtnForRing		selectedBtn;
	bool					ringIsOn;	//je nordic zaply/vypnuty
	eTXActiveBy				txActiveBy;
	eSelectedDog			selectedDogBeforeTX;
	bool					pairSuccess;
	bool					unPairSuccess;

}tRingInfo;

/*
 *
 */
typedef struct
{
	eSelectedDog			selectedDog;
	tDogs					dogsInfo[DOG_CNT];
	eBatteryStatus			stateBattery;
	eTxActive				txActive;
	bool					lockBottomKeys;
	bool					stateOnlyCharging;
	bool					needChargerToLive;
	bool					iUseLipol;	//pouze pro zapnuti zateze pri mereni batt
	eChagerState			chargerState;
	eActualScreen			actScreen;
	uint8_t					numberOfDogs;
	bool					activeSettingsNumberOfDogs;
	bool					feedbackON;

#if (VERSION_WITH_RING == 1)
	tRingInfo			ringInfo;
#endif

}__packed tSharedDataLCD_Core;

/**
 *
 */
typedef struct
{
	uint32_t		 		ShortPressLowLevelMask;
	uint8_t					repeatCounter;
	tSharedDataLCD_Core		sharedData;
	bool					btns_disabled;

} tCoreGlobalData;

typedef enum
{
	STATE_CORE_INIT,     //!< STATE_INIT
	STATE_CORE_OFF,      //!< STATE_OFF
	STATE_CORE_START_ON, //!< STATE_START_ON
	STATE_CORE_ON,       //!< STATE_ON
	STATE_CORE_START_OFF,//!< STATE_START_OFF
	//STATE_CORE_ERROR

}eStateCoreSystem;

/**
 * Core task
 */
typedef struct
{
	eStateCoreSystem	ActualState;
	eStateCoreSystem	PreviousState;

} tStateCoreAutomat;


/**
 *
 */
typedef struct
{
	uint32_t BitMask;
	uint32_t ProhibitionBitMask;
	uint16_t Counter;
}tKeyBoard;


/**
 *
 */
typedef struct
{
	uint32_t counter;
	uint32_t pinState;

}tChargerStatepin;

/************************************************************************/
/* Declaration global variables - EXTERN                                */
/************************************************************************/

/************************************************************************/
/* Definition inline global function	                               */
/************************************************************************/

/************************************************************************/
/* Declaration global function											*/
/************************************************************************/
void ServiceOfChargeAccumCallback(void);
void CORE_CheckEEPROM(tCoreGlobalData *glData);
void CallbackButtonIRQ(bool init);
void KeyboardTimElapsed(TIM_HandleTypeDef *htim);
void Callback_DMA1_Channel1_IRQHandler( DMA_HandleTypeDef * _hdma);
void CallbackChargerState(void);
void CallbackChargerEdgeElapsed(void);

void TaskCore(void const * argument);

extern TaskHandle_t NotifyADCDone;

#endif /* INC_FREERTOS_TASKS_TASKCORE_H_ */

