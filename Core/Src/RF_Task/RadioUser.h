/*
 * ProcessRFTask.h
 *
 *  Created on: 24. 6. 2021
 *      Author: developer
 */

#ifndef INC_FREERTOS_TASKS_RF_TASK_RADIOUSER_H_
#define INC_FREERTOS_TASKS_RF_TASK_RADIOUSER_H_

#include "TaskRF.h"
/*
 *
 */
typedef struct
{
	uint32_t 		pin;
	GPIO_TypeDef	*port;

}PinStruct;

/**
 *
 */
typedef struct {

	PinStruct 			pin_NSS;				/* SPI Select pin */
	PinStruct			pin_SDN;				/* BUSY pin */
	PinStruct			pin_RESET;				/* Reset pin */
	PinStruct			pin_NIRQ;				/* NIRQ1 - IRQ 1 pin */
	SPI_HandleTypeDef	*target;				/* SPI target */
#if (RF_USE_DMA==1)
	osSemaphoreId		RFBinarySPISemaphore;
	uint8_t				Gl_RF_RX_SPI_DMA_Buffer[100];
	uint8_t				Gl_RF_TX_SPI_DMA_Buffer[100];
	void (*RF_TX_DMATransferComplete(void));
	void (*RF_RX_DMATransferComplete(void));
#endif
	void (*AtomicActionEnter)(void);				/* pointer na funkci - vstup do Atomicke oblasti  */
	void (*AtomicActionExit)(void);					/* pointer na funkci - vystup do Atomicke oblasti */
//	void (*RadioRFSwitch) (Enum_RF_switch state);


}tRadioConfig;


typedef enum
{
	RF_CMD_NONE=0,
	RF_CMD_INIT_ON,
	RF_CMD_INIT_OFF,
	RF_CMD_SEND_UNIVERSAL_PAYLOAD,
	RF_CMD_REFILL_FIFO,
	RF_CMD_HEARTBEAT_STOPPED,
	RF_CMD_TX_CW,
	RF_CMD_STOP_TX_AND_DISCARD,
	RF_CMD_READY,
	RF_CMD_SLEEP,

}RfCommands;



extern tRadioConfig 	spiDevice;

bool RU_RadioAssign(void);
void RU_ClearAndDisableIRQ(void);
void RU_ClearAndEnableIRQ(void);
void RU_IrqProcess(tGlobalDataRF * glData);
void RU_commandProcess(tDataQueue * receiveData, tGlobalDataRF *glData);

#endif /* INC_FREERTOS_TASKS_RF_TASK_RADIOUSER_H_ */
