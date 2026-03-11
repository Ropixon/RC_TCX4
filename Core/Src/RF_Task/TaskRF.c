/*
 * TaskRF.c
 *
 *  Created on: 25. 3. 2021
 *      Author: jirik
 */

/************************************************************************/
/* include header												   		*/
/************************************************************************/
#include "cmsis_os.h"
#include "main.h"
#include "spi.h"
#include "Si446x_api_lib.h"
#include "radio_config_Si4463.h"
#include "Si446x_hal.h"
#include "TaskRF.h"
#include "RadioUser.h"
#include "SignalProcessing.h"
#include "RadioInterface.h"
/************************************************************************/
/* Declaration importing objects                                        */
/************************************************************************/
extern volatile osMessageQId 	QueueCoreHandle;
extern volatile osMessageQId	QueueRFHandle;


/************************************************************************/
/* Definition global variables                                          */
/************************************************************************/
uint16_t Gl_Packet_RAW[PACKET_SIZE_BIT];
uint16_t  GLTXDone;
uint16_t GlRepeatCounter;
DMA_HandleTypeDef hdma_tim2_ch1;

/************************************************************************/
/* Local #DEFINE														*/
/************************************************************************/

/************************************************************************/
/* Local TYPEDEF												   		*/
/************************************************************************/


/************************************************************************/
/* Definition local variables										   	*/
/************************************************************************/

/************************************************************************/
/* Declaration functions											   	*/
/************************************************************************/
static uint8_t RF_StateINIT(tDataQueue receiveData,
		tGlobalDataRF* glData, tStateRfSystem* stateAutomat,
		void** pointerMalloc);

static uint8_t RF_StateOFF(tDataQueue receiveData,
		tGlobalDataRF* glData, tStateRfSystem* stateAutomat,
		void** pointerMalloc);

static uint8_t RF_StateStartON(tDataQueue receiveData,
		tGlobalDataRF* glData, tStateRfSystem* stateAutomat,
		void** pointerMalloc);

static uint8_t RF_StateON(tDataQueue receiveData,
		tGlobalDataRF* glData, tStateRfSystem* stateAutomat,
		void** pointerMalloc);

static uint8_t RF_StateStartOFF(tDataQueue receiveData,
		tGlobalDataRF* glData, tStateRfSystem* stateAutomat,
		void** pointerMalloc);

static uint8_t (*StateRF[])(tDataQueue, tGlobalDataRF*,
		tStateRfSystem*, void**) =
		{RF_StateINIT, RF_StateOFF, RF_StateStartON , RF_StateON, RF_StateStartOFF};

/************************************************************************/
/* Definition functions                                                 */
/************************************************************************/

void CallbackPacketTxDone(struct __DMA_HandleTypeDef * hdma);

/**
 *
 */
void CallBackRFIRQ(void)
{
	tDataQueue sendData;
	sendData.pointer = NULL;
	signed portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken=pdFALSE;

	sendData.address = ADDR_TO_RF_IRQ_FIRED;

	if(xQueueSendFromISR(QueueRFHandle,&sendData,&xHigherPriorityTaskWoken)!=pdPASS)
	{
		LogError(5123);
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}


/**
 *
 */
void RFRefreschWatchdog(void)
{
	tDataQueue	sendData;
	sendData.pointer=NULL;

	sendData.address=ADDR_TO_CORE_WATCHDOG_ACTIONS;
	sendData.data=WD_RF_IS_ALIVE;
	xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);
	portYIELD();

}

/**
 *
 */
static uint8_t RF_StateINIT(tDataQueue receiveData,	tGlobalDataRF* glData, tStateRfSystem* stateAutomat,
		void** pointerMalloc)
{

	RU_RadioAssign();
	RI_StopTXandDiscard(glData);

	stateAutomat->PreviousState = stateAutomat->ActualState;
	stateAutomat->ActualState = STATE_RF_OFF;

	return 0;
}

/**
 *
 */
static uint8_t RF_StateOFF(tDataQueue receiveData,tGlobalDataRF* glData, tStateRfSystem* stateAutomat,
		void** pointerMalloc){

	tDataQueue 		sendData;
	sendData.pointer=NULL;

	if(receiveData.address==ADDR_TO_RF_CHANGE_STATE)
	{
		sendData.address=ADDR_TO_CORE_TASK_STATE_CHANGED;

		if(receiveData.data==DATA_TO_RF_START_ON)
		{
			RI_InitRadio();
			RI_StopTXandDiscard(glData);

			glData->inTxState = false;
			glData->onePacketDone = false;
			glData->stopTXAfterTX = false;

			sendData.data = DATA_TO_CORE_RF_IS_ON;
			xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);

			stateAutomat->PreviousState = stateAutomat->ActualState;
			stateAutomat->ActualState = STATE_RF_ON;

			glData->FILL_FIFO = false;
		}
		else if(receiveData.data==DATA_TO_RF_START_OFF)
		{
			RI_InitRadio();
			RI_StopTXandDiscard(glData);

			sendData.data = DATA_TO_CORE_RF_IS_OFF;
			xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);

		}
	}

	return 0;
}

/**
 *
 */
static uint8_t RF_StateStartON(tDataQueue receiveData,tGlobalDataRF* glData, tStateRfSystem* stateAutomat,
		void** pointerMalloc){

	return 0;
}

/**
 *
 */
static uint8_t RF_StateON(tDataQueue receiveData, tGlobalDataRF* glData, tStateRfSystem* stateAutomat,
		void** pointerMalloc){

	tDataQueue 		sendData;
	sendData.pointer=NULL;

	switch (receiveData.address)
	{
		case ADDR_TO_RF_CHANGE_STATE:
			if (receiveData.data == DATA_TO_RF_START_OFF)
			{
				RI_StopTXandDiscard(glData);

				sendData.address = ADDR_TO_CORE_TASK_STATE_CHANGED;
				sendData.data=DATA_TO_CORE_RF_IS_OFF;
				xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);

				stateAutomat->PreviousState = stateAutomat->ActualState;
				stateAutomat->ActualState = STATE_RF_OFF;
			}
			else if(receiveData.data==DATA_TO_RF_START_ON)
			{
				RI_StopTXandDiscard(glData);

				sendData.address=ADDR_TO_CORE_TASK_STATE_CHANGED;
				sendData.data=DATA_TO_CORE_RF_IS_ON;

				xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);

				stateAutomat->PreviousState=stateAutomat->ActualState;
				stateAutomat->ActualState=STATE_RF_ON;

				//if (GlobalData->RF_State != RF_IDLE)	RadioCleanAndStandby();
			}
			break;

		case ADDR_TO_RF_CMD:
			RU_commandProcess(&receiveData, glData);

			break;

		case ADDR_TO_RF_IRQ_FIRED:

			RU_IrqProcess(glData);

			break;

		default:
			break;
	}
	return 0;
}

/**
 *
 */
static uint8_t RF_StateStartOFF(tDataQueue receiveData,
		tGlobalDataRF* glData, tStateRfSystem* stateAutomat,
		void** pointerMalloc){

	return 0;
}

/**
 *
 * @param argument
 */
void TaskRF(void const * argument)
{
    static tDataQueue receiveData;
    static tGlobalDataRF dataTaskRF;
    static tStateRfSystem stateAutomat = {STATE_RF_INIT, STATE_RF_INIT};
    static void* pointerToMalloc = NULL;
    static portBASE_TYPE returnValue;

#if RTT_PRINT == 1
    SEGGER_RTT_printf(0, "%sRF_START\r\n", RTT_CTRL_BG_BLACK );
#endif

    RF_StateINIT(receiveData, &dataTaskRF, &stateAutomat, &pointerToMalloc);

    for (;;)
    {
		returnValue = xQueueReceive(QueueRFHandle, &receiveData, portMAX_DELAY);
		if (returnValue == pdPASS)
		{
			if(receiveData.address == ADDR_TO_RF_CHECK_RF_IS_ALIVE)
			{
				RFRefreschWatchdog();
			}
			else
			{
				StateRF[stateAutomat.ActualState](receiveData, &dataTaskRF, &stateAutomat,
							&pointerToMalloc);
			}

			/* Clear malloc */
			vPortFree(receiveData.pointer);
			receiveData.pointer=NULL;
		}
    }
}
