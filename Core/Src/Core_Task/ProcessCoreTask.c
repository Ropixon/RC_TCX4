/*
 * ProcessCoreTask.c
 *
 *  Created on: 27. 6. 2021
 *      Author: developer
 */
#include "main.h"
#include "SignalProcessing.h"
#include "TaskCore.h"
#include "RadioUser.h"
#include "TaskRF.h"
#include "EepromAddress.h"
#include "ProcessCoreTask.h"
#include "KeyBoardProcess.h"
#include "UartProcess.h"

extern osMessageQId QueueDisplayHandle;
extern osMessageQId QueueRFHandle;
extern osMessageQId QueueCoreHandle;
extern osTimerId TimerStopTXHandle;
extern osTimerId TimerMeasureBattHandle;
extern osTimerId TimerChargerHandle;
extern osTimerId TimerUartCheckHandle;
extern osTimerId TimerRingHeartBeatHandle;
extern osTimerId TimerStopTxFromRingHandle;
extern osTimerId TimerUnpairringHandle;
extern osTimerId Timer_AscendingModeHandle;
extern osTimerId timerChargingCheckHandle;
extern volatile bool	ringTxStillActive_LOCK;


//volatile uint32_t smazat_Baterka;
//volatile int32_t smazat_Teplota;
//
//volatile uint32_t smazatHighLvl;
//volatile uint32_t smazatMedhLvl;
//volatile uint32_t smazatLowLvl;

TaskHandle_t NotifyADCDone = NULL;

uint16_t	GlAdcBattMeas[ADC_BATT_BUFFER_SIZE];

eProtocolVibrationMode VibrationConTable[] = {
		VIBRATION_OFF,
		VIBRATION_1P_M4,
		VIBRATION_2P_M5,
		VIBRATION_3P_M3,
		VIBRATION_IP_M1,
		VIBRATION_CV_M2
};

uint8_t ShockConTable[] = {

		//0 - 0us
		0,
		//1 - 4 us
		1,
		//2 - 6 us
		3,
		//3 - 8 us
		5,
		//4 - 10 us
		7,
		//5 - 12 us
		9,
		//6 - 14 us
		10,
		//7 - 16 us
		11,
		//8 - 18 us
		12,
		//9 - 20 us
		13,
		//10 - 24 us
		14,
		//11 - 28 us
		15,
		//12 - 32 us
		16,
		//13 - 36 us
		17,
		//14 - 40 us
		18,
		//15 - 44 us
		19,
		//16 - 48
		20,
		//17 - 52 us
		21,
		//18 - 58 us
		22,
		//19 - 64 us
		23,
        //20 - 78 us
        25,
        //21 - 94 us
        27,
        //22 - 110 us
        29,
        //23 - 130 us
        31,
        //24 - 150 us
        32,
        //25 - 170 us
        33,
        //26
        35,
        //27
        37,
        //28
        38,
        //29
        39,
        //30
        40
};


static void PCT_ADCBattProcess(tCoreGlobalData * glData, bool measAfterCharge, adcData_t *aData);
static void PCT_GetADCmVolts(adcData_t * aData);
static void GetBatteryStatus(uint32_t Voltage_mV, uint32_t Voltage_mV_LOAD, float Temperature,tCoreGlobalData *glData);


/**
 *
 */
void CallbackChargerCheck(void const * argument)
{
	// Note: This callback is currently commented out and not used
	// Uncomment the lines below if you want to send the charger change message
	/*
	tDataQueue sendData;
	sendData.pointer = NULL;

	sendData.address = ADDR_TO_CORE_CHARGER_CHANGE;
	sendData.data = DATA_TO_CORE_CHARGER_CHANGE;

	xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);
	*/
}


/**
 *
 */
void PCT_RefreshLCD(tCoreGlobalData * glData)
{
	tDataQueue sendData;
	sendData.pointer = NULL;
	tSharedDataLCD_Core * LCD_Packet;

	LCD_Packet = pvPortMalloc(sizeof(tSharedDataLCD_Core));
	if (LCD_Packet == NULL)
	{
		osThreadYield();
		osDelayWatchdog(10);
		LCD_Packet = pvPortMalloc(sizeof(tSharedDataLCD_Core));
		if (LCD_Packet == NULL)			MallocFailed();
	}
	memset(LCD_Packet, 0, sizeof(tSharedDataLCD_Core));


	/* Copy all critical information */
	memcpy(&LCD_Packet->selectedDog,&glData->sharedData.selectedDog,sizeof(tSharedDataLCD_Core));

	sendData.address = ADDR_TO_LCD_REFRESH_PAGE;

	sendData.pointer = LCD_Packet;
	xQueueSend(QueueDisplayHandle,&sendData,portMAX_DELAY);
	osThreadYield();

	//glData->RefreshDisplay=false;
	//glData->NewIncomingData=NO_INCOMING_NEW_DATA;

}

/**
 *
 */
bool PCT_ToggleFeedbackON(tCoreGlobalData *glData)
{
	bool backUp;

	if(glData->sharedData.lockBottomKeys == true)	return false;

	backUp = glData->sharedData.feedbackON;
	glData->sharedData.feedbackON = !glData->sharedData.feedbackON;

	EA_SetFeedbackON((uint8_t)glData->sharedData.feedbackON);

	if(backUp == glData->sharedData.feedbackON) return false;
	return true;
}

/**
 *
 */
bool PCT_IncreaseShock(eSelectedDog dog, uint8_t value, tCoreGlobalData *glData)
{
	uint8_t tempShockMax;
	uint8_t backUp;

	if(glData->sharedData.lockBottomKeys == true)	return false;

	if(glData->sharedData.dogsInfo[dog].onlyVibration == true) tempShockMax = VIBRATION_MAX;
	else													tempShockMax = SHOCK_MAX;

	backUp = glData->sharedData.dogsInfo[dog].shockValue;
	glData->sharedData.dogsInfo[dog].shockValue = (uint8_t) SP_ConstrainINT(glData->sharedData.dogsInfo[dog].shockValue+value, SHOCK_MIN, tempShockMax);

	if(dog == DOG_A)			EA_SetShockA(glData->sharedData.dogsInfo[dog].shockValue);
	else if(dog == DOG_B)		EA_SetShockB(glData->sharedData.dogsInfo[dog].shockValue);
#if (VER_4DOGS == 1)
	else if(dog == DOG_C)		EA_SetShockC(glData->sharedData.dogsInfo[dog].shockValue);
	else if(dog == DOG_D)		EA_SetShockD(glData->sharedData.dogsInfo[dog].shockValue);
	else if(dog == DOG_E)		EA_SetShockE(glData->sharedData.dogsInfo[dog].shockValue);
	else if(dog == DOG_F)		EA_SetShockF(glData->sharedData.dogsInfo[dog].shockValue);
#endif

	if(backUp == glData->sharedData.dogsInfo[dog].shockValue) return false;
	return true;
}

#if (VERSION_WITH_RING == 1)

/*
 *
 */
void PCT_EnterInPairMenu(tCoreGlobalData *glData)
{
	glData->sharedData.ringInfo.isPairingMenuActive = true;
	glData->sharedData.ringInfo.ringNumber = 1;
	glData->sharedData.ringInfo.selectedBtn = RING_FCE_SHOCK;

	PCT_RestartUnpairTimer();

}


void PCT_ResetRing(tCoreGlobalData *glData)
{
	PCT_ChangeNORDICState(false,false,glData);
	osDelayWatchdog(50);
	PCT_ChangeNORDICState(true,true,glData);
}

void PCT_RestartUnpairTimer(void)
{
	osTimerStart(TimerUnpairringHandle, RING_UNPAIR_TIMEOUT);
}
/**
 *
 */
void PCT_ChangeNORDICState(bool state /* true = on */,bool writeToEeprom,tCoreGlobalData *glData)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	glData->sharedData.ringInfo.ringIsOn = state;
	//osStatus tmp;

	if(writeToEeprom == true)	EA_SetNordicState(state);

	if(state == true)
	{
		UP_InitUart();

		GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF0_USART1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		if(osTimerStart(TimerUartCheckHandle, PERIOD_TO_CHECK_RING_RX) != osOK) 		LogError(8453453);
		if(osTimerStart(TimerRingHeartBeatHandle, HB_WATCHDOG_TIME_TO_RESET) != osOK) 	LogError(84534556);

		HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, true);

	}
	else
	{
		HAL_GPIO_WritePin(BT_EN_GPIO_Port, BT_EN_Pin, false);
		GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7 , 0);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6, 0);

		osTimerStop(TimerRingHeartBeatHandle);
	}

}

/*
 *
 */
bool PCT_IncreaseRingNumber(tCoreGlobalData *glData)
{
	uint8_t backUp;

	backUp = glData->sharedData.ringInfo.ringNumber;
	glData->sharedData.ringInfo.ringNumber = SP_ConstrainU8(glData->sharedData.ringInfo.ringNumber+1, 1, RING_MAX_ALLOWED_RING);

	if(backUp == glData->sharedData.ringInfo.ringNumber) return false;
	return true;
}


/*
 *
 */
bool PCT_DecreaseRingNumber(tCoreGlobalData *glData)
{
	uint8_t backUp;

	backUp = glData->sharedData.ringInfo.ringNumber;
	glData->sharedData.ringInfo.ringNumber = (uint8_t) SP_ConstrainINT(glData->sharedData.ringInfo.ringNumber-1, 1, RING_MAX_ALLOWED_RING);

	if(backUp == glData->sharedData.ringInfo.ringNumber) return false;
	return true;
}


/*
 *
 */
uint8_t PCT_DecodeRingPacket(uart_payloads_u *packet,tCoreGlobalData *glData)
{
	uint32_t				RX_MAC;
	uint8_t					RX_RSSI;
	eSelectedDog 			currentDog;
	static eSelectedDog 	last_currentDog;
	uint8_t					ringNumber;
	static uint8_t			last_ringNumber;
	eSelectedBtnForRing functionOfRing;
	uint32_t tmp_delay=0;
	UBaseType_t queue_wait=0;

	uint32_t tx_timeout = STOP_TX_AFTER_LAST_RX_TIMEOUT_MS;

	switch ((packet->payload_1.header.payload_code))
	{
		case UART_PCKT_TYPE_RF:
			HAL_GPIO_WritePin(TP204_SCOPE_GPIO_Port, TP204_SCOPE_Pin, 1);
			if(glData->sharedData.ringInfo.txActiveBy == TX_BY_EDGE) break;

			RX_RSSI = packet->payload_1.rx_rssi;
			RX_MAC	= packet->payload_1.ring_MAC;
			//RX_RING_BATT_LEVEL = packet->payload_1.ring_batt_level;

			if(glData->sharedData.ringInfo.isPairingMenuActive)
			{
				if (RX_RSSI < 70)
				{
					/* 1) Prsten mohl mit drive jinou fci  -ted ho z odstranime */
					ringNumber = PCT_Ring_Is_MAC_Whitelisted(RX_MAC);
					if(ringNumber != 0xFF)
					{
						osDelayWatchdog(10);
						/*ano MAC je na pozici ringNumber, tak ji nejprve vymazneme */
						PCT_EraseRing(ringNumber);
					}

					/*2) smazeme vse k pozici prstenu */
					PCT_EraseRing(glData->sharedData.ringInfo.ringNumber);
					osDelayWatchdog(10);

					/* nyni muzeme prichozi MAC ulozit na vybranou pozici*/
					EA_SaveRing(glData->sharedData.ringInfo.ringNumber, glData->sharedData.selectedDog,glData->sharedData.ringInfo.selectedBtn,RX_MAC);

					glData->sharedData.ringInfo.pairSuccess = true;
					KP_KeyBoardLight(false,false,false,glData);
					PCT_RefreshLCD(glData);
					osTimerStop(TimerRingHeartBeatHandle);
					do
					{
						tmp_delay++;
						osDelayWatchdog(10);
						queue_wait = uxQueueMessagesWaiting(QueueCoreHandle);
					}
					while((queue_wait < 3)&&(tmp_delay<100));


					osTimerStart(TimerRingHeartBeatHandle, HB_WATCHDOG_TIME_TO_RESET);
					glData->sharedData.ringInfo.pairSuccess = false;
					glData->sharedData.ringInfo.isPairingMenuActive = false;
					PCT_RefreshLCD(glData);

				}
			}
			else
			{
				ringNumber = PCT_Ring_Is_MAC_Whitelisted(RX_MAC);
				if(ringNumber != 0xFF)
				{
					/* zname prstynek - provedeme naparovanou akci */
					currentDog = EA_GetRingDogID(ringNumber);

					if(currentDog >= EA_GetNumOfDogs()) break;

					currentDog = SP_ConstrainU8(currentDog, DOG_A, glData->sharedData.numberOfDogs);

					functionOfRing = EA_GetRingBtnFction(ringNumber);

					if(currentDog != last_currentDog || ringNumber != last_ringNumber)
					{
						ringTxStillActive_LOCK = false;
						PCT_StopTX(glData);
					}

					last_currentDog = currentDog;
					last_ringNumber	= ringNumber;

					if(ringTxStillActive_LOCK == false/*xTimerIsTimerActive(TimerStopTxFromRingHandle) == pdFALSE*/)
					{
						if(functionOfRing >= RING_FCE_SHOCK && functionOfRing <=RING_FCE_BOOSTER)
						{
							glData->sharedData.ringInfo.txActiveBy = TX_BY_NORDIC;
							glData->sharedData.ringInfo.selectedDogBeforeTX = glData->sharedData.selectedDog;
							glData->sharedData.selectedDog = currentDog;

							osThreadSuspendAll();
							ringTxStillActive_LOCK = true;
							osThreadResumeAll();
							KP_KeyBoardLight(true,true,true,glData);
						}

						switch (functionOfRing)
						{
							case RING_FCE_SHOCK:
								PCT_SendShock(glData,currentDog,true);

								break;

							case RING_FCE_BEEP:
								PCT_SendBeep(glData,currentDog,true);

								break;

							case RING_FCE_BOOSTER:

								uint8_t valueTosend;
#if ASCENDING_MODE_ENABLED == true
								if(EA_GetAscendingMode(currentDog) == true
										&& glData->sharedData.dogsInfo[currentDog].onlyVibration == false)
								{
									tx_timeout = STOP_TX_AFTER_LAST_RX_TIMEOUT_PRG_MS;
									glData->sharedData.dogsInfo[currentDog].ascendingMode = true;
									if(ASCENDING_VALUE_ZERO_OR_SHOCK)
									{
										valueTosend = glData->sharedData.dogsInfo[currentDog].ascendingShockValue = 0;
									}
									else
									{
										valueTosend = glData->sharedData.dogsInfo[currentDog].ascendingShockValue = glData->sharedData.dogsInfo[currentDog].shockValue;
									}

									osTimerStart(Timer_AscendingModeHandle, ASCENDING_INCREASING_PERIOD_MS);
								}
								else
#endif
								{
									valueTosend = glData->sharedData.dogsInfo[currentDog].boosterValue;
								}

								PCT_SendBooster(glData,valueTosend,currentDog,true);

								break;
							default:
								break;

						}

					}

					//prodluzujeme dobu vysilani bud jiz vysilaneho, nebo prave ted odeslaneho
					osTimerStart(TimerStopTxFromRingHandle, tx_timeout );
				}

			}

			break;

		case UART_PCKT_TYPE_HB:

			break;

		default:
			break;
	}

	return 0;;
}

/*
 *
 */
void PCT_UnpairRing(tCoreGlobalData *glData)
{
	UBaseType_t queue_wait=0;
	uint32_t	tmp_delay=0;

	if(glData->sharedData.ringInfo.isPairingMenuActive == true)
	{
		PCT_EraseRing(glData->sharedData.ringInfo.ringNumber);

		glData->sharedData.ringInfo.unPairSuccess = true;
		KP_KeyBoardLight(false,false,false,glData);
		PCT_RefreshLCD(glData);
		osTimerStop(TimerRingHeartBeatHandle);

		do
		{
			tmp_delay++;
			osDelayWatchdog(10);
			queue_wait = uxQueueMessagesWaiting(QueueCoreHandle);
		}
		while((queue_wait < 3)&&(tmp_delay<100));

		osTimerStart(TimerRingHeartBeatHandle, HB_WATCHDOG_TIME_TO_RESET);
		glData->sharedData.ringInfo.unPairSuccess = false;
		glData->sharedData.ringInfo.isPairingMenuActive = false;
		PCT_RefreshLCD(glData);
	}
}


/*
 *
 */
uint8_t PCT_Ring_Is_MAC_Whitelisted(uint32_t mac)
{
	uint32_t tmp;
	for(uint8_t i=0;i<RING_MAX_ALLOWED_RING;i++)
	{
		tmp = EA_GetRingMAC(i+1);
		if(tmp == mac && mac != UINT32_MAX)	return i+1;	//mac znam
	}

	return 0xFF;
}



void PCT_EraseRing(uint8_t ring)
{
//	EA_EraseRingMAC(ring,0);
	uint8_t currentDog = EA_GetRingDogID(ring);

	EA_SaveRing(ring,currentDog,RING_FCE_NONE,0);

}

#endif



/**+
 *
 */
bool PCT_IncreaseBooster(eSelectedDog dog, uint8_t value, tCoreGlobalData *glData)
{
	uint8_t backUp;
	uint8_t tempBoosterkMax;
	if(glData->sharedData.dogsInfo[dog].onlyVibration == true) tempBoosterkMax = 0;
	else													   tempBoosterkMax = SHOCK_MAX;


	backUp = glData->sharedData.dogsInfo[dog].boosterValue;
	glData->sharedData.dogsInfo[dog].boosterValue = (uint8_t) SP_ConstrainINT(glData->sharedData.dogsInfo[dog].boosterValue+value, BOOSTER_MIN, tempBoosterkMax);


	if(dog == DOG_A)				EA_SetBoosterA(glData->sharedData.dogsInfo[dog].boosterValue);
	else if(dog == DOG_B)		EA_SetBoosterB(glData->sharedData.dogsInfo[dog].boosterValue);
#if (VER_4DOGS == 1)
	else if(dog == DOG_C)		EA_SetBoosterC(glData->sharedData.dogsInfo[dog].boosterValue);
	else if(dog == DOG_D)		EA_SetBoosterD(glData->sharedData.dogsInfo[dog].boosterValue);
	else if(dog == DOG_E)		EA_SetBoosterE(glData->sharedData.dogsInfo[dog].boosterValue);
	else if(dog == DOG_F)		EA_SetBoosterF(glData->sharedData.dogsInfo[dog].boosterValue);

#endif

	if(backUp == glData->sharedData.dogsInfo[dog].boosterValue) return false;
	return true;
}

/**+ num of dogs
 *
 */
bool PCT_IncreaseNumberOfDogs(tCoreGlobalData *glData)
{
	uint8_t backUp;
	backUp = glData->sharedData.numberOfDogs;
	glData->sharedData.numberOfDogs = (uint8_t) SP_ConstrainINT(glData->sharedData.numberOfDogs+1, 1, NUMBER_OF_DOGS_MAX);

	EA_SetNumOfDogs(glData->sharedData.numberOfDogs);

	if(backUp == glData->sharedData.numberOfDogs) return false;
	return true;
}

/**- num of dogs
 *
 */
bool PCT_DecreaseNumberOfDogs(tCoreGlobalData *glData)
{
	uint8_t backUp;
	backUp = glData->sharedData.numberOfDogs;
	glData->sharedData.numberOfDogs = (uint8_t) SP_ConstrainINT(((int)glData->sharedData.numberOfDogs)-1, 1, NUMBER_OF_DOGS_MAX);

	EA_SetNumOfDogs(glData->sharedData.numberOfDogs);

	if(backUp == glData->sharedData.numberOfDogs) return false;
	return true;
}

/**
 *
 */
bool PCT_DecreaseShock(eSelectedDog dog, uint8_t value, tCoreGlobalData *glData)
{
	uint8_t backUp;
	uint8_t tempShockMin;
	uint8_t tempShockMax;

	if(glData->sharedData.lockBottomKeys == true)	return false;

	if(glData->sharedData.dogsInfo[dog].onlyVibration == true) tempShockMin = VIBRATION_MIN, tempShockMax = VIBRATION_MAX;
	else													tempShockMin = SHOCK_MIN, tempShockMax = SHOCK_MAX;


	backUp = glData->sharedData.dogsInfo[dog].shockValue;
	glData->sharedData.dogsInfo[dog].shockValue =(uint8_t) SP_ConstrainINT(glData->sharedData.dogsInfo[dog].shockValue-value, tempShockMin, tempShockMax);

	if(dog == DOG_A)			EA_SetShockA(glData->sharedData.dogsInfo[dog].shockValue);
	else if(dog == DOG_B)		EA_SetShockB(glData->sharedData.dogsInfo[dog].shockValue);
#if (VER_4DOGS == 1)
	else if(dog == DOG_C)		EA_SetShockC(glData->sharedData.dogsInfo[dog].shockValue);
	else if(dog == DOG_D)		EA_SetShockD(glData->sharedData.dogsInfo[dog].shockValue);
	else if(dog == DOG_E)		EA_SetShockE(glData->sharedData.dogsInfo[dog].shockValue);
	else if(dog == DOG_F)		EA_SetShockF(glData->sharedData.dogsInfo[dog].shockValue);
#endif

	if(backUp == glData->sharedData.dogsInfo[dog].shockValue) return false;
	return true;
}

/**+
 *
 */
bool PCT_DecreaseBooster(eSelectedDog dog, uint8_t value, tCoreGlobalData *glData)
{
	uint8_t backUp;

	backUp = glData->sharedData.dogsInfo[dog].boosterValue;
	glData->sharedData.dogsInfo[dog].boosterValue = (uint8_t) SP_ConstrainINT(glData->sharedData.dogsInfo[dog].boosterValue-value, SHOCK_MIN, SHOCK_MAX);


	if(dog == DOG_A)			EA_SetBoosterA(glData->sharedData.dogsInfo[dog].boosterValue);
	else if(dog == DOG_B)		EA_SetBoosterB(glData->sharedData.dogsInfo[dog].boosterValue);

#if (VER_4DOGS == 1)
	else if(dog == DOG_C)		EA_SetBoosterC(glData->sharedData.dogsInfo[dog].boosterValue);
	else if(dog == DOG_D)		EA_SetBoosterD(glData->sharedData.dogsInfo[dog].boosterValue);
	else if(dog == DOG_E)		EA_SetBoosterE(glData->sharedData.dogsInfo[dog].boosterValue);
	else if(dog == DOG_F)		EA_SetBoosterF(glData->sharedData.dogsInfo[dog].boosterValue);

#endif

	if(backUp == glData->sharedData.dogsInfo[dog].boosterValue) return false;
	return true;
}


#if ASCENDING_MODE_ENABLED == true
/**
 *
 */
void PCT_SetAscendingMode(eSelectedDog dog, bool enable, tCoreGlobalData *glData)
{
	{
		glData->sharedData.dogsInfo[dog].ascendingMode = enable;
		EA_SetAscendingMode(dog, enable);
	}
}

#endif

/**
 *
 */
uint16_t CRC_Calculation(uint8_t Data[], uint16_t len)
{
	#define CRC_HIGH_BIT	0x8000
	#define POLYNOM			0x8005

	uint8_t 	i, j, c;
	uint16_t  	bit;
	uint16_t 	crc = 0x0000;

	for (i=0; i<len; i++) {

		c = Data[i];
		for (j=0x80; j; j>>=1)
		{
			bit = crc & CRC_HIGH_BIT;
			crc<<= 1;
			if (c & j) bit^= CRC_HIGH_BIT;
			if (bit) crc^= POLYNOM;
		}
	}
	crc^= 0x0000;

	return(crc);

}
/**
 *
 */
void PCT_TimerStartWatchdog()
{
#if (RUN_WITH_WATCHDOG==1)
	/* Periodic timer for checking WatchhDog*/
	HAL_NVIC_DisableIRQ(TIM6_IRQn);
	HAL_NVIC_ClearPendingIRQ(TIM6_IRQn);
	HAL_NVIC_EnableIRQ(TIM6_IRQn);

	LL_TIM_SetCounter(TIM6,0);
	LL_TIM_EnableIT_UPDATE(TIM6);
	LL_TIM_EnableCounter(TIM6);


#endif

}

/**
 *
 */
bool PCT_ExitFromSettings(tCoreGlobalData *glData)
{
	if ((glData->sharedData.dogsInfo[glData->sharedData.selectedDog].activeBooter) || (glData->sharedData.activeSettingsNumberOfDogs))
	{
		glData->sharedData.dogsInfo[glData->sharedData.selectedDog].activeBooter = false;

		if (glData->sharedData.activeSettingsNumberOfDogs)
		{
			glData->sharedData.activeSettingsNumberOfDogs = false;
			glData->sharedData.selectedDog = 0;
			EA_SetSelectedDog(glData->sharedData.selectedDog);
		}
		PCT_RefreshLCD(glData);

		return true;
	}

	return false;
}

bool PCT_ExitFromBoosterSettings(tCoreGlobalData *glData)
{
	if (glData->sharedData.dogsInfo[glData->sharedData.selectedDog].activeBooter)
	{
		glData->sharedData.dogsInfo[glData->sharedData.selectedDog].activeBooter = false;
		PCT_RefreshLCD(glData);

		return true;
	}

	return false;
}


/**
 *
 */
void PCT_LockPulseEdit(tCoreGlobalData *glData)
{

#if (DOUBLE_CLICK_TO_LOCK_KEYS == 1)
	static TickType_t lastTime;
	TickType_t timeStamp;

	timeStamp = xTaskGetTickCount();

	if(timeStamp-lastTime > 500)
	{
		lastTime = timeStamp;
		return;
	}
#endif
	if ((glData->sharedData.dogsInfo[glData->sharedData.selectedDog].activeBooter == false) &&
			(glData->sharedData.activeSettingsNumberOfDogs == false))
	{
		if(glData->sharedData.lockBottomKeys == true)
		{
			glData->sharedData.lockBottomKeys = false;
		}
		else
		{
			glData->sharedData.lockBottomKeys = true;
		}

		EA_SetLockBtmKeys(glData->sharedData.lockBottomKeys);

		PCT_RefreshLCD(glData);
	}


}


void PCT_SendBeep(tCoreGlobalData *glData, eSelectedDog selectedDog, bool lcdRefresh)
{
	tDataQueue sendData;
	sendData.pointer = NULL;
	U8				ArrayDataTX[PACKET_SIZE_BYTE];
	uint8_t 		*txPacket;
	uint32_t		systemMAC;
	uint16_t		crcValue;

//	if (mcuIsLocked() == false)
//	{
//		sendData.address = ADDR_TO_RF_CMD;
//		sendData.data = RF_CMD_TX_CW;
//		xQueueSend(QueueRFHandle,&sendData,portMAX_DELAY);
//		return;
//	}

	txPacket = pvPortMalloc(20);
	if(txPacket==NULL) LogError(16531);

	/* preamble */
	memset(ArrayDataTX,0xAA,8);

	/* Sync word */
	ArrayDataTX[8]=0x2D;	//0xDA
	ArrayDataTX[9]=0xD4;	//0xDA

	/* mac */
	systemMAC = EA_GetSystemMAC();
	systemMAC += selectedDog;

#ifdef TESTER_TX
	systemMAC = 1;
#endif

	ArrayDataTX[10]=(uint8_t)((systemMAC & 0xFF));
	ArrayDataTX[11]=(uint8_t)((systemMAC & 0xFF00)>>8);
	ArrayDataTX[12]=(uint8_t)((systemMAC & 0xFF0000)>>16);	//high byte - serial number

	/* payload */
	ArrayDataTX[13]=(uint8_t) MOD_BEEP ;	//Beep,Shock,...
	ArrayDataTX[14]=(uint8_t) 1;		//Value of the shock
#if RF_POWER==RADIO_PA_LOW_PWR
	ArrayDataTX[15]=(uint8_t)PWR_IN_DATA_LOW;		// low power
#elif (RF_POWER == RADIO_PA_MID_PWR)
	ArrayDataTX[15]=(uint8_t)PWR_IN_DATA_MID;		// low power
#else
	ArrayDataTX[15]=(uint8_t)PWR_IN_DATA_HIGH;		// high power
#endif
	ArrayDataTX[16]=(uint8_t) 0;		//Value of the shock
	ArrayDataTX[17]=(uint8_t) 0;		//Value of the shock
	ArrayDataTX[18]=(uint8_t) 0;		//Value of the shock

	/* crc */
	crcValue = CRC_Calculation(&ArrayDataTX[10],PACKET_SIZE_BYTE-12);
	ArrayDataTX[19]=(uint8_t) (crcValue>>8) & 0xFF;
	ArrayDataTX[20]=(uint8_t) (crcValue & 0xFF);


	memcpy(txPacket,&ArrayDataTX,PACKET_SIZE_BYTE);

	sendData.address = ADDR_TO_RF_CMD;
	sendData.data = RF_CMD_SEND_UNIVERSAL_PAYLOAD;
	sendData.temp_32 = PACKET_SIZE_BYTE;
	sendData.pointer = txPacket;
	sendData.temp_bool = false;
	sendData.RFU_32 = false;	//true - budu menit data na pozadi behem vysilani

	osTimerStart(TimerStopTXHandle, STOP_TX_TIMEOUT_BEEP);

	xQueueSend(QueueRFHandle,&sendData,portMAX_DELAY);

	glData->sharedData.txActive = TX_ACTIVE_BEEP;

	if(lcdRefresh)
	PCT_RefreshLCD(glData);
}

/**
 *
 */
void PCT_SendShock( tCoreGlobalData *glData,eSelectedDog selectedDog, bool lcdRefresh)
{
	tDataQueue sendData;
	sendData.pointer = NULL;
	uint8_t 		*txPacket;
	U8				ArrayDataTX[PACKET_SIZE_BYTE];
	uint32_t		systemMAC;
	uint16_t		crcValue;
	uint16_t		txTimeout;
	uint8_t			shockValue;

	txPacket = pvPortMalloc(20);
	if(txPacket==NULL) LogError(16531);

	/* preamble */
	memset(ArrayDataTX,0xAA,8);

	/* Sync word */
	ArrayDataTX[8]=0x2D;	//0xDA
	ArrayDataTX[9]=0xD4;	//0xDA

	/* mac */
	systemMAC = EA_GetSystemMAC();
	systemMAC += selectedDog;

#ifdef TESTER_TX
	systemMAC = 1;
#endif

	ArrayDataTX[10]=(uint8_t)((systemMAC & 0xFF));
	ArrayDataTX[11]=(uint8_t)((systemMAC & 0xFF00)>>8);
	ArrayDataTX[12]=(uint8_t)((systemMAC & 0xFF0000)>>16);	//high byte - serial number

	/* payload */
	if(glData->sharedData.dogsInfo[selectedDog].onlyVibration == true)
	{
		ArrayDataTX[13]=(uint8_t) MOD_VIBR;	//Beep,Shock,...
		ArrayDataTX[14]=glData->sharedData.dogsInfo[selectedDog].shockValue;
		ArrayDataTX[14]=(U8)VibrationConTable[ArrayDataTX[14]];

		if((ArrayDataTX[14]==(U8)VIBRATION_1P_M4)||(ArrayDataTX[14]==(U8)VIBRATION_2P_M5)||(ArrayDataTX[14]==(U8)VIBRATION_3P_M3))	txTimeout = STOP_TX_TIMEOUT_NICK;
		else	txTimeout = STOP_TX_TIMEOUT_BEEP;
	}
	else
	{
		ArrayDataTX[13]=(uint8_t) MOD_BIG_SHOCK;	//Beep,Shock,...
		shockValue = glData->sharedData.dogsInfo[selectedDog].shockValue;
		ArrayDataTX[14]=ShockConTable[shockValue];

#if (NC_MODE_AVAILABLE != 1)
		// pouze continue
		osTimerStart(TimerStopTXHandle, STOP_TX_TIMEOUT_SHOCK);

#else
		if(glData->sharedData.dogsInfo[selectedDog].modeNC == MODE_NICK)
		{
			txTimeout = STOP_TX_TIMEOUT_NICK;
		}
		else
		{
			txTimeout = STOP_TX_TIMEOUT_SHOCK;
		}
#endif
	}

#if RF_POWER==RADIO_PA_LOW_PWR
	ArrayDataTX[15]=(uint8_t)PWR_IN_DATA_LOW;		// low power
#elif (RF_POWER == RADIO_PA_MID_PWR)
	ArrayDataTX[15]=(uint8_t)PWR_IN_DATA_MID;		// low power
#else
	ArrayDataTX[15]=(uint8_t)PWR_IN_DATA_HIGH;		// high power
#endif
	ArrayDataTX[16]=(uint8_t) 0;		//Value of the shock
	ArrayDataTX[17]=(uint8_t) 0;		//Value of the shock
	ArrayDataTX[18]=(uint8_t) 0;		//Value of the shock

	/* crc */
	crcValue = CRC_Calculation(&ArrayDataTX[10],PACKET_SIZE_BYTE-12);
	ArrayDataTX[19]=(uint8_t) (crcValue>>8) & 0xFF;
	ArrayDataTX[20]=(uint8_t) (crcValue & 0xFF);

	memcpy(txPacket,&ArrayDataTX,PACKET_SIZE_BYTE);

	osTimerStart(TimerStopTXHandle, txTimeout);

	sendData.address = ADDR_TO_RF_CMD;
	sendData.data = RF_CMD_SEND_UNIVERSAL_PAYLOAD;
	sendData.temp_32 = PACKET_SIZE_BYTE;
	sendData.pointer = txPacket;
	sendData.temp_bool = false;
	sendData.RFU_32 = false;	//true - budu menit data na pozadi behem vysilani

	xQueueSend(QueueRFHandle,&sendData,portMAX_DELAY);
	taskYIELD();
	glData->sharedData.txActive = TX_ACTIVE_SHOCK;

	if(lcdRefresh)
	PCT_RefreshLCD(glData);

}

/**
 *
 */
void PCT_SendBooster( tCoreGlobalData *glData,uint8_t shockValue,eSelectedDog selectedDog, bool lcdRefresh)
{
	tDataQueue sendData;
	sendData.pointer = NULL;
	uint8_t 		*txPacket;
	U8				ArrayDataTX[PACKET_SIZE_BYTE];
	uint32_t		systemMAC;
	uint16_t		crcValue;
	uint16_t 		txTimeout;
	//uint8_t			shockValue;

	txPacket = pvPortMalloc(20);
	if(txPacket==NULL) LogError(16531);

	/* preamble */
	memset(ArrayDataTX,0xAA,8);

	/* Sync word */
	ArrayDataTX[8]=0x2D;	//0xDA
	ArrayDataTX[9]=0xD4;	//0xDA

	/* mac */
	systemMAC = EA_GetSystemMAC();
	systemMAC += selectedDog;

#ifdef TESTER_TX
	systemMAC = 1;
#endif

	ArrayDataTX[10]=(uint8_t)((systemMAC & 0xFF));
	ArrayDataTX[11]=(uint8_t)((systemMAC & 0xFF00)>>8);
	ArrayDataTX[12]=(uint8_t)((systemMAC & 0xFF0000)>>16);	//high byte - serial number

#if ASCENDING_MODE_ENABLED == true
	if(glData->sharedData.dogsInfo[selectedDog].ascendingMode == true
			&& glData->sharedData.dogsInfo[selectedDog].onlyVibration == false)
	{
		ArrayDataTX[13]=(uint8_t) MOD_BIG_SHOCK;	//Beep,Shock,...
		//shockValue = glData->sharedData.dogsInfo[selectedDog].boosterValue;
		ArrayDataTX[14]=ShockConTable[shockValue];
		sendData.RFU_32 = true;	//true - budu menit data na pozadi behem vysilani
	}
	else
#endif
	{
		/* payload */
		if((glData->sharedData.dogsInfo[selectedDog].boosterValue == 0)
				|| (glData->sharedData.dogsInfo[selectedDog].onlyVibration == true))
		{
			ArrayDataTX[13]=(uint8_t) MOD_VIBR;	//Beep,Shock,...
			ArrayDataTX[14]=2;
			txTimeout = STOP_TX_TIMEOUT_BEEP;
		}
		else
		{
			ArrayDataTX[13]=(uint8_t) MOD_BIG_SHOCK;	//Beep,Shock,...
			//shockValue = glData->sharedData.dogsInfo[selectedDog].boosterValue;
			ArrayDataTX[14]=ShockConTable[shockValue];
			txTimeout = STOP_TX_TIMEOUT_SHOCK;
		}

		sendData.RFU_32 = false;	//true - budu menit data na pozadi behem vysilani

		osTimerStart(TimerStopTXHandle, txTimeout);
	}


#if RF_POWER==RADIO_PA_LOW_PWR
	ArrayDataTX[15]=(uint8_t)PWR_IN_DATA_LOW;		// low power
#elif (RF_POWER == RADIO_PA_MID_PWR)
	ArrayDataTX[15]=(uint8_t)PWR_IN_DATA_MID;		// low power
#else
	ArrayDataTX[15]=(uint8_t)PWR_IN_DATA_HIGH;		// high power
#endif
	ArrayDataTX[16]=(uint8_t) 0;		//Value of the shock
	ArrayDataTX[17]=(uint8_t) 0;		//Value of the shock
	ArrayDataTX[18]=(uint8_t) 0;		//Value of the shock

	/* crc */
	crcValue = CRC_Calculation(&ArrayDataTX[10],PACKET_SIZE_BYTE-12);
	ArrayDataTX[19]=(uint8_t) (crcValue>>8) & 0xFF;
	ArrayDataTX[20]=(uint8_t) (crcValue & 0xFF);


	memcpy(txPacket,&ArrayDataTX,PACKET_SIZE_BYTE);

#if ASCENDING_MODE_ENABLED == true
	if(glData->sharedData.dogsInfo[selectedDog].ascendingStarted == true) // jiz pouze zvysujeme dalsi hodnotu
	{
		sendData.address = ADDR_TO_RF_CMD;
		sendData.data = RF_CMD_REFILL_FIFO;
		sendData.temp_32 = PACKET_SIZE_BYTE;
		sendData.pointer = txPacket;
		sendData.temp_bool = false;

	}
	else
#endif
	{
		sendData.address = ADDR_TO_RF_CMD;
		sendData.data = RF_CMD_SEND_UNIVERSAL_PAYLOAD;
		sendData.temp_32 = PACKET_SIZE_BYTE;
		sendData.pointer = txPacket;
		sendData.temp_bool = false;	//nick
	}

	xQueueSend(QueueRFHandle,&sendData,portMAX_DELAY);
	glData->sharedData.txActive = TX_ACTIVE_BOOSTER;

	if(lcdRefresh)
	PCT_RefreshLCD(glData);


}


void PCT_StartMeasureBatt(tCoreGlobalData * glData,bool measAfterCharge)
{

	adcData_t adcBuffData[ADC_MEAS_CNT];

	for(eADCMeasType measType=0;measType<ADC_MEAS_CNT;measType++)
	{
		uint32_t ulNotificationValue;
		bool battLoadConnected = false;

		osThreadSuspendAll();
		NVIC_DisableIRQ(DMA1_Channel1_IRQn);
		if(LL_ADC_REG_IsConversionOngoing(SYSTEM_ADC))
		{
			LL_ADC_REG_StopConversion(SYSTEM_ADC);
			while(LL_ADC_REG_IsStopConversionOngoing(SYSTEM_ADC)!=0);
		}
		osThreadResumeAll();

		osThreadSuspendAll();
		__HAL_RCC_HSI_ENABLE();	// for ADC
		while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET);
		osThreadResumeAll();

		LL_ADC_REG_SetDMATransfer(SYSTEM_ADC, LL_ADC_REG_DMA_TRANSFER_NONE);
		LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_1);

		/* Calibrate ADC. */
		osThreadSuspendAll();
		if (LL_ADC_IsEnabled(SYSTEM_ADC)) LL_ADC_Disable(SYSTEM_ADC);
		while(LL_ADC_IsDisableOngoing(SYSTEM_ADC)==1);
		LL_ADC_DisableIT_EOCAL(SYSTEM_ADC);
		LL_ADC_StartCalibration(SYSTEM_ADC);
		while(LL_ADC_IsCalibrationOnGoing(SYSTEM_ADC)==1);
		LL_ADC_ClearFlag_EOCAL(SYSTEM_ADC);
		LL_ADC_ClearFlag_ADRDY(SYSTEM_ADC);
		osThreadResumeAll();

		/*wait for ADRDY*/
		osThreadSuspendAll();
		LL_ADC_Enable(SYSTEM_ADC);
		while(LL_ADC_IsInternalRegulatorEnabled(SYSTEM_ADC)==0);
		while (LL_ADC_IsActiveFlag_ADRDY(SYSTEM_ADC) == 0);
		osThreadResumeAll();


		osThreadSuspendAll();
		LL_DMA_ConfigTransfer(DMA1,LL_DMA_CHANNEL_1,LL_DMA_DIRECTION_PERIPH_TO_MEMORY|LL_DMA_MODE_NORMAL|LL_DMA_PERIPH_NOINCREMENT|
								LL_DMA_MEMORY_INCREMENT|LL_DMA_PDATAALIGN_HALFWORD|LL_DMA_MDATAALIGN_HALFWORD|LL_DMA_PRIORITY_LOW);

		LL_DMA_ConfigAddresses(DMA1,LL_DMA_CHANNEL_1, LL_ADC_DMA_GetRegAddr(SYSTEM_ADC, LL_ADC_DMA_REG_REGULAR_DATA),
								(uint32_t)&GlAdcBattMeas[0],LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

		/* Set DMA transfer size */
		LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, ADC_BATT_BUFFER_SIZE);
		/* Enable DMA transfer interruption: transfer complete */
		LL_DMA_EnableIT_TC(DMA1,LL_DMA_CHANNEL_1);
		/* Enable the DMA transfer */
		LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_1);

		/* Configure GPIO in analog mode to be used as ADC input */
		LL_GPIO_SetPinMode(ADC_BATT_GPIO_Port,ADC_BATT_Pin, LL_GPIO_MODE_ANALOG);

		/*Set ADC*/
		LL_ADC_SetSamplingTimeCommonChannels(SYSTEM_ADC, LL_ADC_SAMPLINGTIME_79CYCLES_5);
		LL_ADC_REG_SetTriggerSource(SYSTEM_ADC,LL_ADC_REG_TRIG_SOFTWARE);
		LL_ADC_REG_SetContinuousMode(SYSTEM_ADC,LL_ADC_REG_CONV_CONTINUOUS);
		LL_ADC_REG_SetDMATransfer(SYSTEM_ADC, LL_ADC_REG_DMA_TRANSFER_LIMITED);
		LL_ADC_REG_SetOverrun(SYSTEM_ADC, LL_ADC_REG_OVR_DATA_OVERWRITTEN);
		LL_ADC_REG_SetSequencerChannels(SYSTEM_ADC,LL_ADC_CHANNEL_0|LL_ADC_CHANNEL_4|LL_ADC_CHANNEL_VREFINT|LL_ADC_CHANNEL_TEMPSENSOR);
		osThreadResumeAll();


		osThreadSuspendAll();
		LL_ADC_SetCommonPathInternalCh( __LL_ADC_COMMON_INSTANCE(SYSTEM_ADC),LL_ADC_PATH_INTERNAL_VREFINT|LL_ADC_PATH_INTERNAL_TEMPSENSOR);
		while(LL_PWR_IsActiveFlag_VREFINTRDY()==0);
		osThreadResumeAll();

		LL_ADC_DisableIT_EOC(SYSTEM_ADC);
		LL_ADC_DisableIT_EOS(SYSTEM_ADC);

		//NVIC_DisableIRQ(DMA1_Channel1_IRQn);
		NVIC_ClearPendingIRQ(DMA1_Channel1_IRQn);
		NVIC_EnableIRQ(DMA1_Channel1_IRQn);

		/*prepare notify*/
		if(NotifyADCDone!=NULL)	LogError(3541354);
		NotifyADCDone=xTaskGetCurrentTaskHandle();

		taskENTER_CRITICAL();
		if(glData->sharedData.iUseLipol == false)
		{
			if(measType == ADC_MEAS_WITH_LOAD)
			{
				/* turn on the load */
				LL_GPIO_SetOutputPin(BATT_LOAD_GPIO_Port,BATT_LOAD_Pin);
				for(uint16_t i=0; i <BATT_MEAS_DELAY;i++);
				battLoadConnected=true;
			}
		}

		/*start measure*/
		LL_ADC_REG_StartConversion(SYSTEM_ADC);
		taskEXIT_CRITICAL();

		/*wait for conversion - Batt*/
		ulNotificationValue = ulTaskNotifyTake(pdTRUE,10);

		if( ulNotificationValue == 1 )
		{
			/* The transfer ended as expected. */
			PCT_GetADCmVolts(&adcBuffData[measType]);

			// vynulování čítače restartů - proběhlo validní měření baterie s připojeným zatěžovacím rezistorem
			if(battLoadConnected)
			{
				EA_ClearResetCounter();
			}

		}
		else
		{
			LogError(653214);
			xTaskNotify(NotifyADCDone,0, eNoAction );
			/* There are no ADC in progress, so no tasks to notify. */
			NotifyADCDone = NULL;
		}
	}

	/* nyni mame v adcBuffdata 2 vysledky: pro zatizenou i nezatizenou baterii */
	PCT_ADCBattProcess(glData,measAfterCharge, adcBuffData);

}

static void PCT_GetADCmVolts(adcData_t * aData)
{
	uint32_t adcRef=0,adcRefmV=0,adcResultBatt=0,adcResultACU=0, adcResultTemperature=0;

	LL_GPIO_ResetOutputPin(BATT_LOAD_GPIO_Port,BATT_LOAD_Pin);
	LL_GPIO_SetPinMode(ADC_BATT_GPIO_Port,ADC_BATT_Pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetOutputPin(ADC_BATT_GPIO_Port,ADC_BATT_Pin);

	LL_ADC_ClearFlag_EOC(SYSTEM_ADC);
	LL_ADC_ClearFlag_EOS(SYSTEM_ADC);
	LL_DMA_ClearFlag_GI1(SYSTEM_DMA_FOR_ADC);

	osThreadSuspendAll();
	if (LL_ADC_IsEnabled(SYSTEM_ADC)) LL_ADC_Disable(SYSTEM_ADC);
	while(LL_ADC_IsDisableOngoing(SYSTEM_ADC)==1);
	osThreadResumeAll();

	LL_ADC_SetCommonPathInternalCh( __LL_ADC_COMMON_INSTANCE(SYSTEM_ADC),LL_ADC_PATH_INTERNAL_NONE);
	LL_ADC_DisableInternalRegulator(SYSTEM_ADC);
	LL_ADC_REG_SetDMATransfer(SYSTEM_ADC, LL_ADC_REG_DMA_TRANSFER_NONE);

	osThreadSuspendAll();
	for(uint8_t i = 0; i < ADC_BATT_BUFFER_SIZE; i++)
	{
		switch(i % 4)  // Použití modulo 4 pro čtyři kanály
		{
			case 0:
				adcResultACU += GlAdcBattMeas[i];
				break;
			case 1:
				adcResultBatt += GlAdcBattMeas[i];
				break;
			case 2:
				adcRef += GlAdcBattMeas[i];
				break;
			case 3:
				adcResultTemperature += GlAdcBattMeas[i];  // Akumulace pro nový kanál
				break;
		}
	}
	osThreadResumeAll();

	adcResultBatt=(uint32_t) (adcResultBatt/(ADC_EACH_CHANNEL_MEAS_CNT));
	adcResultACU = (uint32_t) (adcResultACU/(ADC_EACH_CHANNEL_MEAS_CNT));
	adcResultTemperature = (uint32_t) (adcResultTemperature/(ADC_EACH_CHANNEL_MEAS_CNT));
	adcRef=(uint32_t)(adcRef/(ADC_EACH_CHANNEL_MEAS_CNT));

	adcRefmV=__LL_ADC_CALC_VREFANALOG_VOLTAGE(adcRef,LL_ADC_RESOLUTION_12B);
	/*smazat_Baterka =*/ aData->volBatt_mv =__LL_ADC_CALC_DATA_TO_VOLTAGE(adcRefmV,adcResultBatt,LL_ADC_RESOLUTION_12B);
	/*smazat_Teplota =*/ aData->volTemp_mv=__LL_ADC_CALC_TEMPERATURE(adcRefmV,adcResultTemperature,LL_ADC_RESOLUTION_12B);
	aData->volAccu_mv=__LL_ADC_CALC_DATA_TO_VOLTAGE(adcRefmV,adcResultACU,LL_ADC_RESOLUTION_12B);

}

/**
 *
 */
static void PCT_ADCBattProcess(tCoreGlobalData * glData, bool measAfterCharge, adcData_t *aData)
{
	float lipolTemp;

	tDataQueue sendData;
	sendData.pointer = NULL;

	/* prepocet delice na lipolce*/
	lipolTemp =(float) aData[ADC_MEAS_WITHOUT_LOAD].volAccu_mv*1.39286;
	aData[ADC_MEAS_WITHOUT_LOAD].volAccu_mv = (uint32_t) lipolTemp;

	/* po vlozeni baterky je Lipol v true, nyni zjistime zda na Lipol namerime vetsi napeti nez 2 V */
	if((glData->sharedData.iUseLipol == true) && (aData[ADC_MEAS_WITHOUT_LOAD].volAccu_mv >= ACUMULATOR_DETECT_THRESHOLD))
	{
		/* jedeme z lipol */
		/* povoleni preruseni od nabijecky */
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_11);
		LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_11);
		glData->sharedData.iUseLipol = true;

		if( measAfterCharge == true)
		{
			/* muze se baterka zotavit */
			if((aData[ADC_MEAS_WITHOUT_LOAD].volAccu_mv > LIP_CAPACITY_70_PERCENT_V))
			{
				glData->sharedData.stateBattery = BATTERY_STATUS_HIGH;
			}
			else if((aData[ADC_MEAS_WITHOUT_LOAD].volAccu_mv > LIP_CAPACITY_50_PERCENT_V))
					glData->sharedData.stateBattery = BATTERY_STATUS_MED;
			else if((aData[ADC_MEAS_WITHOUT_LOAD].volAccu_mv > LIP_CAPACITY_30_PERCENT_V))
					glData->sharedData.stateBattery = BATTERY_STATUS_LOW;
			else
					glData->sharedData.stateBattery = BATTERY_STATUS_CRITICAL;
		}
		else
		{
			if((aData[ADC_MEAS_WITHOUT_LOAD].volAccu_mv > LIP_CAPACITY_70_PERCENT_V)&&(glData->sharedData.stateBattery >=BATTERY_STATUS_HIGH ))
			{
				glData->sharedData.stateBattery = BATTERY_STATUS_HIGH;
			}
			else if((aData[ADC_MEAS_WITHOUT_LOAD].volAccu_mv > LIP_CAPACITY_50_PERCENT_V)&&(glData->sharedData.stateBattery >=BATTERY_STATUS_MED ))
				glData->sharedData.stateBattery = BATTERY_STATUS_MED;
			else if((aData[ADC_MEAS_WITHOUT_LOAD].volAccu_mv > LIP_CAPACITY_30_PERCENT_V)&&(glData->sharedData.stateBattery >=BATTERY_STATUS_LOW ))
				glData->sharedData.stateBattery = BATTERY_STATUS_LOW;
			else
				glData->sharedData.stateBattery = BATTERY_STATUS_CRITICAL;
		}


		if(aData[ADC_MEAS_WITHOUT_LOAD].volAccu_mv<=LIP_CAPACITY_5_PERCENT_V)	/* vypneme system */
		{
			sendData.address = ADDR_TO_CORE_START_SHUTDOWN;
			xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);
			glData->sharedData.needChargerToLive = true;
		}
		else
		{
			glData->sharedData.needChargerToLive = false;
		}
	}
	else
	{
		/* jedeme z nenabijeci CR2 baterie */
		/***********************************/

		/* zakazani preruseni od nabijecky */
		LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_11);
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_11);
		glData->sharedData.iUseLipol = false;

		osTimerStop(TimerChargerHandle);

		// první měření CR2 po resetu (neaktualizuje stav baterie, aktualizace proběhne až při dalším měření - za 1s)
		if (LL_RCC_IsActiveFlag_LPWRRST() || LL_RCC_IsActiveFlag_PORRST())
		{
			// clear RCC flags (reset info)
			LL_RCC_ClearResetFlags();
			return;
		}

		GetBatteryStatus(aData[ADC_MEAS_WITHOUT_LOAD].volBatt_mv, aData[ADC_MEAS_WITH_LOAD].volBatt_mv, aData[ADC_MEAS_WITH_LOAD].volTemp_mv, glData);
//		if((aData[0].volBatt_mv > BAT_CAPACITY_60_PERCENT_V)&&(glData->sharedData.stateBattery >=BATTERY_STATUS_HIGH ))
//			glData->sharedData.stateBattery = BATTERY_STATUS_HIGH;
//		else if((adcResultmVBatt > BAT_CAPACITY_30_PERCENT_V)&&(glData->sharedData.stateBattery >=BATTERY_STATUS_MED ))
//			glData->sharedData.stateBattery = BATTERY_STATUS_MED;
//		else if((adcResultmVBatt > BAT_CAPACITY_10_PERCENT_V)&&(glData->sharedData.stateBattery >=BATTERY_STATUS_LOW ))
//			glData->sharedData.stateBattery = BATTERY_STATUS_LOW;
//		else
//			glData->sharedData.stateBattery = BATTERY_STATUS_CRITICAL;
	}
}


static uint32_t mapf(float x, float in_min, float in_max, float out_min, float out_max)
{

    // Ochrana proti dělení nulou

    if (in_max == in_min) return out_min;

    return (uint32_t)((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);

}

static void GetBatteryStatus(uint32_t Voltage_mV, uint32_t Voltage_mV_LOAD, float Temperature,tCoreGlobalData *glData)
{
	static eBatteryStatus LastCalculatedStatus = BATTERY_STATUS_HIGH;	//zpoždovací člen
	static eBatteryStatus theLowestStatEver = BATTERY_STATUS_HIGH;
	eBatteryStatus ReturnStatus = glData->sharedData.stateBattery;

	// recalculation of levels
	if (Temperature > 20) Temperature = 20;

	if (Temperature < -18) Temperature = -18;

	volatile uint32_t NEW_BATT_VOLTAGE_temp = mapf(Temperature, LOW_TEMP_LEVEL, HIGH_TEMP_LEVEL, (float)NEW_BATT_VOLTAGE_LOW, (float)NEW_BATT_VOLTAGE);
	volatile uint32_t BATT_HIGH_VOLTAGE_temp = mapf(Temperature, LOW_TEMP_LEVEL, HIGH_TEMP_LEVEL, (float)BATT_HIGH_VOLTAGE_LOW, (float)BATT_HIGH_VOLTAGE);
	volatile uint32_t BATT_MED_VOLTAGE_temp = mapf(Temperature, LOW_TEMP_LEVEL, HIGH_TEMP_LEVEL, (float)BATT_MED_VOLTAGE_LOW, (float)BATT_MED_VOLTAGE);
	volatile uint32_t BATT_LOW_VOLTAGE_temp = mapf(Temperature, LOW_TEMP_LEVEL, HIGH_TEMP_LEVEL, (float)BATT_LOW_VOLTAGE_LOW, (float)BATT_LOW_VOLTAGE);

	//smazatHighLvl = BATT_HIGH_VOLTAGE_temp;
	//smazatMedhLvl = BATT_MED_VOLTAGE_temp;
	//smazatLowLvl = BATT_LOW_VOLTAGE_temp;

	if ((Voltage_mV >= NEW_BATT_VOLTAGE_NO_LOAD) || (Voltage_mV_LOAD > NEW_BATT_VOLTAGE_temp))
	{
	    // This is a new battery or a battery that has more than 50% capacity
	    ReturnStatus = BATTERY_STATUS_HIGH;
	    LastCalculatedStatus = BATTERY_STATUS_HIGH;
	    theLowestStatEver = BATTERY_STATUS_HIGH;
	}
	else
	{
		if ((Voltage_mV_LOAD > BATT_HIGH_VOLTAGE_temp)/* && (glData->sharedData.stateBattery >= BATTERY_STATUS_MED)*/
			  && (theLowestStatEver>=BATTERY_STATUS_MED))
		{
		   // It returns to green from the state RED+GREEN
		   if (LastCalculatedStatus == BATTERY_STATUS_HIGH)
		   {
				ReturnStatus = BATTERY_STATUS_HIGH;
		   }
		   LastCalculatedStatus = BATTERY_STATUS_HIGH;
		}
		else if ((Voltage_mV_LOAD > BATT_MED_VOLTAGE_temp)/* && (glData->sharedData.stateBattery >= BATTERY_STATUS_MED)*/
			  && (theLowestStatEver>=BATTERY_STATUS_LOW))
		{
		   // Set RED+GREEN status
		   if (LastCalculatedStatus == BATTERY_STATUS_MED)
		   {
				ReturnStatus = BATTERY_STATUS_MED;
		   }
		   LastCalculatedStatus = BATTERY_STATUS_MED;
		}
		else if((Voltage_mV_LOAD > BATT_LOW_VOLTAGE_temp)/* && (glData->sharedData.stateBattery >= BATTERY_STATUS_LOW)*/
			  && (theLowestStatEver>=BATTERY_STATUS_CRITICAL))
		{
		  if (LastCalculatedStatus == BATTERY_STATUS_LOW)
		  {
			 ReturnStatus = BATTERY_STATUS_LOW;
		  }
		  LastCalculatedStatus = BATTERY_STATUS_LOW;
		}
		else
		{
		   // Critical state
		   if (LastCalculatedStatus == BATTERY_STATUS_CRITICAL)
		   {
				ReturnStatus = BATTERY_STATUS_CRITICAL;
		   }
		   LastCalculatedStatus = BATTERY_STATUS_CRITICAL;
		}
	}

	glData->sharedData.stateBattery = ReturnStatus;

	if(theLowestStatEver > glData->sharedData.stateBattery) theLowestStatEver = glData->sharedData.stateBattery;

}
/**
 *
 */
void PCT_LockShockToogle(tCoreGlobalData *glData)
{
	if(glData->sharedData.dogsInfo[glData->sharedData.selectedDog].onlyVibration == true)
	{
		glData->sharedData.dogsInfo[glData->sharedData.selectedDog].onlyVibration = false;
	}
	else
	{
		glData->sharedData.dogsInfo[glData->sharedData.selectedDog].onlyVibration = true;
	}

	PCT_DecreaseShock(glData->sharedData.selectedDog,0,glData);
	PCT_RefreshLCD(glData);

	EA_SetLockActive(glData->sharedData.selectedDog,glData->sharedData.dogsInfo[glData->sharedData.selectedDog].onlyVibration);
	/* save it to EEPROM */

}


/*
 *
 */
void PCT_ChargerInit(void)
{
	LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_11);
	LL_GPIO_SetPinPull(CHARGER_STAT_GPIO_Port, CHARGER_STAT_Pin, LL_GPIO_PULL_UP);
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_11);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_11);

	LL_GPIO_ResetOutputPin(BATT_LOAD_GPIO_Port,BATT_LOAD_Pin);
	LL_GPIO_SetPinMode(ADC_BATT_GPIO_Port,ADC_BATT_Pin, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetOutputPin(ADC_BATT_GPIO_Port,ADC_BATT_Pin);

}


/**
 *
 */
void PCT_ChargerProcedure(uint32_t data, tStateCoreAutomat* stateAutomat,tCoreGlobalData *glData)
{

	bool chargerState;
	eChagerState	tmpCharStat = CHARGER_NONE;
	//uint32_t		backUpIMR;

	if(data == DATA_TO_CORE_CHARGER_CHANGE)
	{
		if(glData->sharedData.iUseLipol == false)	return;

		if(stateAutomat->ActualState == STATE_CORE_ON)
		{
		//	osTimerStart(timerChargingCheckHandle, CHARGING_CHECK_PERIOD_MS);
		}

		chargerState = LL_GPIO_ReadInputPort(CHARGER_STAT_GPIO_Port) & CHARGER_STAT_Pin;

		LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_11);	//kvuli nasledujicimu pulldownu	- vyvola znovu preruseni
		if(chargerState == true)
		{	/* bud mame nabito - dokonceni nabijeni = H, nebo odpojeni nabijecky - High Z */
			LL_GPIO_SetPinPull(CHARGER_STAT_GPIO_Port, CHARGER_STAT_Pin, LL_GPIO_PULL_DOWN);
			osDelay(2);
			if((LL_GPIO_ReadInputPort(CHARGER_STAT_GPIO_Port) & CHARGER_STAT_Pin))	tmpCharStat = CHARGER_BATT_FULL;
			else
			{
				tmpCharStat = CHARGER_UNPLUGGED;
			}
		}
		else
		{
			/* zacalo nabijeni */
			LL_GPIO_SetPinPull(CHARGER_STAT_GPIO_Port, CHARGER_STAT_Pin, LL_GPIO_PULL_UP);
			osDelay(2);
			/*znovu kontrolujeme nizky stav - tedy nabijeni */
			if(!(LL_GPIO_ReadInputPort(CHARGER_STAT_GPIO_Port) & CHARGER_STAT_Pin))	tmpCharStat = CHARGER_RUNNING;
			else
			{
				tmpCharStat = CHARGER_UNPLUGGED;
			}
		}

		if(tmpCharStat == CHARGER_UNPLUGGED)
		{
			LL_GPIO_SetPinPull(CHARGER_STAT_GPIO_Port, CHARGER_STAT_Pin, LL_GPIO_PULL_UP);
			osDelay(2);
			LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_11);
			/* po chvili zhasneme symbol batt v Offu */
			osTimerStart(TimerChargerHandle, CHARGER_UNPLGED_LCD_TIMEOUT);
		}
		else if(tmpCharStat == CHARGER_RUNNING)
		{
			osTimerStop(TimerChargerHandle);
			glData->sharedData.needChargerToLive = false;
		}

		LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_11);

		glData->sharedData.chargerState = tmpCharStat;

		if(tmpCharStat != CHARGER_UNPLUGGED)
		{
			/* aktualizuje napeti baterie */
			PCT_StartMeasureBatt(glData, true);
		}

		PCT_RefreshLCD(glData);
		KP_KeyBoardLight(true,false,true,glData);
	}
	else if(data == DATA_TO_CORE_CHARGER_TIMEOUT)
	{
		if(stateAutomat->ActualState != STATE_CORE_ON)
		{
			PCT_RefreshLCD(glData);
		}
	}
}


/**
 *
 */
void PCT_StopTX(tCoreGlobalData *glData)
{
	tDataQueue sendData;
	sendData.pointer = NULL;

	if (mcuIsLocked() == false)
	{
		sendData.address = ADDR_TO_RF_CMD;
		sendData.data = RF_CMD_READY;
		xQueueSend(QueueRFHandle,&sendData,portMAX_DELAY);
		return;
	}

	if(glData->sharedData.txActive == TX_ACTIVE_NO) return;

	sendData.address = ADDR_TO_RF_CMD;
	sendData.data = RF_CMD_STOP_TX_AND_DISCARD;

	xQueueSend(QueueRFHandle,&sendData,portMAX_DELAY);

#if (VERSION_WITH_RING == 1)
		if(glData->sharedData.ringInfo.txActiveBy == TX_BY_NORDIC)
		{
			glData->sharedData.selectedDog = glData->sharedData.ringInfo.selectedDogBeforeTX;
		}

		glData->sharedData.ringInfo.txActiveBy = TX_BY_NONE;
#endif

	//if(glData->sharedData.txActive != TX_ACTIVE_NO)	osTimerStart(TimerMeasureBattHandle, ADC_BATT_MEAS_PERIOD_AFTER_TX);

#if ASCENDING_MODE_ENABLED == true
	/* nulovani pocitadla pro ascending */
	/* vsem vypneme flag ascendingStarted */
	for(eSelectedDog dog = 0; dog < DOG_CNT; dog++)
	{
		glData->sharedData.dogsInfo[dog].ascendingStarted = false;
	}

	osTimerStop(Timer_AscendingModeHandle);

#endif

	glData->sharedData.txActive = TX_ACTIVE_NO;

	osTimerStop(TimerStopTXHandle);
	PCT_RefreshLCD(glData);

}

/**
 *
 */
void PCT_WatchDogProcedure(uint32_t	data)
{
#define SUB_COUNTER_VAL 6

	tDataQueue sendData, sendData2;;
	sendData.pointer=NULL, sendData2.pointer = NULL;;

	static bool rfOK,lcdOK;
	static uint8_t wdtSubCounter = SUB_COUNTER_VAL;

	switch ((eWatchDog)data)
	{
		case WD_TIME_FOR_WATCHDOG:
			sendData.address=ADDR_TO_RF_CHECK_RF_IS_ALIVE;
			xQueueSend(QueueRFHandle,&sendData,portMAX_DELAY);

			sendData2.address=ADDR_TO_LCD_CHECK_LCD_IS_ALIVE;
			xQueueSend(QueueDisplayHandle,&sendData2,portMAX_DELAY);

			break;

		case WD_RF_IS_ALIVE:
			rfOK = true;
			break;
		case WD_LCD_IS_ALIVE:
			lcdOK = true;
			break;

		default:
			break;
	}

	if((rfOK==true) && (lcdOK == true))
	{
		wdtSubCounter=SUB_COUNTER_VAL;
		RefreshWatchDog();
		PCT_TimerStartWatchdog();
		rfOK = false;
		lcdOK = false;

	}else if(wdtSubCounter)
	{
		RefreshWatchDog();
		PCT_TimerStartWatchdog();
	}

	if(wdtSubCounter>0) wdtSubCounter--;

}


/**
 *
 */
void PCT_StartShutdown(tStateCoreAutomat* stateAutomat)
{
	tDataQueue sendData,sendData2;
	sendData.pointer = NULL;
	sendData2.pointer = NULL;

	sendData.address = ADDR_TO_RF_CHANGE_STATE;
	sendData.data = DATA_TO_RF_START_OFF;

	sendData2.address = ADDR_TO_LCD_CHANGE_STATE;
	sendData2.data = DATA_TO_LCD_START_OFF;

	xQueueSend(QueueRFHandle,&sendData, portMAX_DELAY);
	xQueueSend(QueueDisplayHandle,&sendData2, portMAX_DELAY);

	stateAutomat->PreviousState = stateAutomat->ActualState;
	stateAutomat->ActualState = STATE_CORE_START_OFF;

	osTimerStop(timerChargingCheckHandle);
}
