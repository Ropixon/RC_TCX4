/*
 * FactoryTest.c
 *
 *  Created on: Feb 25, 2022
 *      Author: developer
 */

#include "FactoryTest.h"
#include "SignalProcessing.h"
#include "RadioUser.h"
#include "TaskCore.h"
#include "EepromAddress.h"

static tFactoryTest GlFactoryTest;

extern osMessageQId QueueCoreHandle;
extern osMessageQId QueueRFHandle;
extern osMessageQId QueueDisplayHandle;
extern osTimerId TimerFactoryTestHandle;
/**
 *
 */
 void CallbackFactoryTest(void const * argument)/*ft_IRQ(void)*/
{
	tDataQueue sendData;
	sendData.pointer=NULL;
	//signed portBASE_TYPE xHigherPriorityTaskWoken;
	//xHigherPriorityTaskWoken=pdFALSE;

	/* EXTI line interrupt detected */
	//if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_8) != RESET)
	{
	//	LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_8);
	//	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_8);

		sendData.address=ADDR_TO_CORE_FACTORY_TEST_DATA_RX;
		if(xQueueSend(QueueCoreHandle,&sendData,portMAX_DELAY)!=pdPASS)
		{
			LogError(9999912);
		}
	//	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	}
}



/**
 *
 */
void ft_InitTest(void)
{
	taskENTER_CRITICAL();
	memset(&GlFactoryTest,0,sizeof(GlFactoryTest));

	GlFactoryTest.testCanStart = true;

	taskEXIT_CRITICAL();

	osTimerStart(TimerFactoryTestHandle, 10);

}


void ft_stopTest(void)
{
	GlFactoryTest.testCanStart = false;
}

/**
 *
 */
void ft_DecodeRxMsg(void)
{
	tDataQueue sendData;
	sendData.pointer=NULL;

	RefreshWatchDog();
	//taskENTER_CRITICAL();

	if(GlFactoryTest.newRxPendingMsg == true)
	{
		LL_GPIO_SetOutputPin(BACKLIGHT_GPIO_Port, BACKLIGHT_Pin);

		switch (GlFactoryTest.factoryCmd)
		{
			case FT_CMD_START_CW:
				sendData.address = ADDR_TO_RF_CMD;
				sendData.data = RF_CMD_TX_CW;
				xQueueSend(QueueRFHandle,&sendData,portMAX_DELAY);

				break;

			case FT_CMD_STOP_CW:
				sendData.address = ADDR_TO_RF_CMD;
				sendData.data = RF_CMD_READY;
				xQueueSend(QueueRFHandle,&sendData,portMAX_DELAY);

				break;

			case FT_CMD_SET_FREQ:
				EA_SetRfFreq(GlFactoryTest.temp32);
				break;

			case FT_CMD_TEST_LCD:

				sendData.data = GlFactoryTest.lcdCOM;
				sendData.RFU_32 = GlFactoryTest.lcdSEG;
				sendData.address = ADDR_TO_LCD_LCD_TEST;
				xQueueSend(QueueDisplayHandle,&sendData,portMAX_DELAY);
				break;

			case FT_CMD_RESET_MCU:
				HAL_NVIC_SystemReset();
				break;

			case FT_CMD_DEV_OFF:
				/* Off */
				EA_SetSystemState(STATE_CORE_OFF);
				HAL_NVIC_SystemReset();

				break;

			case FT_CMD_DEV_ON:

				EA_SetSystemState(STATE_CORE_ON);
				HAL_NVIC_SystemReset();

				break;

			case FT_CMD_WRITE_SERIAL_ID:
				EA_SetSystemMAC(GlFactoryTest.temp32);

				break;

			case FT_CMD_SET_DOGS_CNT:
				EA_SetNumOfDogs(GlFactoryTest.temp32);

				break;

			case FT_CMD_SET_DEFAULT_VALS:
				EA_SetShockA(GlFactoryTest.lcdCOM);
				RefreshWatchDog();
				EA_SetShockB(GlFactoryTest.lcdCOM);
				RefreshWatchDog();
				EA_SetShockC(GlFactoryTest.lcdCOM);
				RefreshWatchDog();
				EA_SetShockD(GlFactoryTest.lcdCOM);
				RefreshWatchDog();
				EA_SetShockE(GlFactoryTest.lcdCOM);
				RefreshWatchDog();
				EA_SetShockF(GlFactoryTest.lcdCOM);
				RefreshWatchDog();
				EA_SetBoosterA(GlFactoryTest.lcdSEG);
				RefreshWatchDog();
				EA_SetBoosterB(GlFactoryTest.lcdSEG);
				RefreshWatchDog();
				EA_SetBoosterC(GlFactoryTest.lcdSEG);
				RefreshWatchDog();
				EA_SetBoosterD(GlFactoryTest.lcdSEG);
				RefreshWatchDog();
				EA_SetBoosterE(GlFactoryTest.lcdSEG);
				RefreshWatchDog();
				EA_SetBoosterF(GlFactoryTest.lcdSEG);
				RefreshWatchDog();

				EA_SetMode(DOG_A,MODE_CON);
				EA_SetMode(DOG_B,MODE_CON);
				EA_SetMode(DOG_C,MODE_CON);
				RefreshWatchDog();
				EA_SetMode(DOG_D,MODE_CON);
				EA_SetMode(DOG_E,MODE_CON);
				EA_SetMode(DOG_F,MODE_CON);
				RefreshWatchDog();

				EA_SetSelectedDog(DOG_A);
				RefreshWatchDog();
				EA_SetFeedbackON(true);

				break;

			case FT_CMD_LOCK_MCU:

				EA_SetSystemState(STATE_CORE_OFF);
				taskENTER_CRITICAL();
				/* Zamknout MCU */
				HAL_FLASH_OB_Unlock();
				FLASH_OBProgramInitTypeDef pOBInit;
				HAL_FLASHEx_OBGetConfig(&pOBInit);
				pOBInit.RDPLevel = 0xBB;
				pOBInit.WRPState = OB_WRPSTATE_ENABLE;
				pOBInit.OptionType =/*OPTIONBYTE_BOR |*/ OPTIONBYTE_RDP ;//| OPTIONBYTE_WRP;
				//pOBInit.BORLevel = OB_BOR_LEVEL1;
				HAL_FLASHEx_OBProgram(&pOBInit);
				HAL_FLASH_OB_Launch();

				LogError(351);
				break;

			default:
				break;
		}

		GlFactoryTest.newRxPendingMsg=false;
	}

	RefreshWatchDog();
	//	taskEXIT_CRITICAL();

}



