/*
 * RadioUser.c
 *
 *  Created on: 24. 06. 2021
 *      Author: developer
 */

#include "main.h"
#include "TaskRF.h"
#include "cmsis_os.h"
#include "RadioUser.h"
#include "RadioInterface.h"
#include "spi.h"
#include "Si446x_api_lib.h"

extern osMessageQId QueueCoreHandle;
tRadioConfig 		spiDevice;
//uint16_t 			Gl_Packet_RAW[PACKET_SIZE_BIT];

/**
 *
 */
bool RU_RadioAssign(void)
{
	taskENTER_CRITICAL();
	/* init global variables. */
	spiDevice.AtomicActionEnter=vPortEnterCritical;
	spiDevice.AtomicActionExit=vPortExitCritical;
	spiDevice.pin_SDN.port=RF_SDN_GPIO_Port;
	spiDevice.pin_SDN.pin=RF_SDN_Pin;
	spiDevice.pin_NIRQ.port=RF_NIRQ_GPIO_Port;
	spiDevice.pin_NIRQ.pin=RF_NIRQ_Pin;
	spiDevice.pin_NSS.port=RF_NSEL_GPIO_Port;
	spiDevice.pin_NSS.pin=RF_NSEL_Pin;

#if (RF_USE_DMA==1)

#endif

	spiDevice.target = ReturnRF_SPIHandle();
	taskEXIT_CRITICAL();

	/* Select to high*/
	LL_GPIO_SetOutputPin(spiDevice.pin_NSS.port,spiDevice.pin_NSS.pin);

	RU_ClearAndDisableIRQ();
	/* Init si4463 + zkouska  komunikace */
	RI_InitRadio();
	RU_ClearAndEnableIRQ();

	return true;

}


/**
 *
 */
void RU_ClearAndDisableIRQ(void)
{
	taskENTER_CRITICAL();

	LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_15);//interrupt from Semtech
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_15);
	//HAL_NVIC_ClearPendingIRQ(EXTI4_15_IRQn);

	taskEXIT_CRITICAL();
}

/**
 *
 */
void RU_ClearAndEnableIRQ(void)
{
	taskENTER_CRITICAL();

	LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_15);//interrupt from Semtech
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_15);
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_15);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

	taskEXIT_CRITICAL();
}

/**
 *
 */
void RU_commandProcess(tDataQueue * receiveData, tGlobalDataRF *glData)
{
	RfCommands cmd = receiveData->data;

	switch (cmd)
	{
		case RF_CMD_INIT_OFF:
			RI_StopTXandDiscard(glData);
			break;

		case RF_CMD_INIT_ON:
			RI_InitRadio();
			RI_SetSleep();
			break;

		case RF_CMD_TX_CW:

			RI_SetReady();
			RU_ClearAndEnableIRQ();
			RI_StartTxCW(20);

			break;

		case RF_CMD_READY:
			RI_SetReady();
			break;

		case RF_CMD_STOP_TX_AND_DISCARD:

			if(glData->onePacketDone == true)
			{
				RI_StopTXandDiscard(glData);
				glData->inTxState = false;
			}
			else
			{
				glData->stopTXAfterTX = true;
			}

			glData->FILL_FIFO = false;

			break;

		case RF_CMD_SEND_UNIVERSAL_PAYLOAD:

			//RI_InitRadio();
			RU_ClearAndDisableIRQ();

			RI_SetReady();
			RI_ClearPendingIRQ();

			RU_ClearAndEnableIRQ();

			RI_SendPayload(receiveData->pointer, receiveData->temp_32,receiveData->temp_bool,receiveData->RFU_32, glData);

			if(receiveData->RFU_32)
			{
				memcpy(glData->payload,receiveData->pointer,receiveData->temp_32);
				glData->payload_size = receiveData->temp_32;
				glData->FILL_FIFO = true;
			}
			else
			{
				glData->FILL_FIFO = false;
			}

			break;

		case RF_CMD_REFILL_FIFO:

			memcpy(glData->payload,receiveData->pointer,receiveData->temp_32);
			glData->payload_size = receiveData->temp_32;
			//RI_FillTXFIFO(receiveData->pointer, receiveData->temp_32);
			glData->FILL_FIFO = true;

			break;

		default:
			break;
	}

}


/**
 *
 */
void RU_IrqProcess(tGlobalDataRF * glData)
{
	tDataQueue sendData;
	sendData.pointer = NULL;
	SI446X_CMD_REPLY_UNION	Si446xCmd;
	SI446X_CMD_REPLY_UNION	FifoInfo;  // Separate structure for FIFO info

	if(glData->inTxState == true)
	{
		if(si446x_get_int_status(0xff,0xff,0xff,&Si446xCmd) != SI446X_SUCCESS) LogError(9846);

		glData->onePacketDone = true;
		RI_ClearPendingIRQ();
		if(glData->stopTXAfterTX == true)
		{
			RI_StopTXandDiscard(glData);

			sendData.address = ADDR_TO_CORE_STOP_TX;
			sendData.data = 0;
			xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY);
		}
		else
		{
			if(Si446xCmd.GET_INT_STATUS.PH_STATUS&(1<<1))	/* TX FIFO almost empty? */
			{
				if(glData->FILL_FIFO == true && glData->payload_size > 0)
				{
					si446x_fifo_info(0b0, &FifoInfo);
					uint8_t available_space = FifoInfo.FIFO_INFO.TX_FIFO_SPACE;
					
					uint8_t payload_count = available_space / glData->payload_size;
					
					for(uint8_t i = 0; i < payload_count; i++)
					{
						si446x_write_tx_fifo(glData->payload_size, glData->payload);
					}
					
				}
			}

		}

	}
	else
	{
		/* some weird interupt */
		RI_ClearPendingIRQ();
		RI_StopTXandDiscard(glData);
	}
}
