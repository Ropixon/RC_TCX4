/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "crc.h"
#include "dma.h"
#include "lcd.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "wwdg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "flash.h"
#include "sleepmgr.h"
#include "TaskCore.h"
#include "TaskRF.h"
#include "error_code.h"
#include "DynamicSleep.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
extern osMessageQId QueueCoreHandle;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void osDelayWatchdog(uint32_t msDelay)
{
	uint32_t tempMod;
	uint32_t tempCount;

	tempMod=msDelay%10;
	tempCount=(uint32_t)(msDelay/10);

	for(uint32_t i=0;i<tempCount;i++)
	{
		osDelay(10);
		RefreshWatchDog();
	}

	osDelay(tempMod);
	RefreshWatchDog();
}

/**
 *
 * @param error
 */
void LogError(uint64_t error)
{

#if (RESET_WHEN_ERROR !=1 )
	taskDISABLE_INTERRUPTS();

	while(1)
	{
		RefreshWatchDog();
	}

#else
	HAL_RCC_DeInit();
	HAL_NVIC_SystemReset();
	while(1);	//pripadne to schodi WD
#endif

}

/**
 *
 */
bool mcuIsLocked(void)
{

#if DEBUG_PRETEND_MCU_IS_LOCKED == 1
	return true;

#else
	FLASH_OBProgramInitTypeDef flashTemp;

	HAL_FLASHEx_OBGetConfig(&flashTemp);
	if(flashTemp.RDPLevel != OB_RDP_LEVEL_0)
	{
		return true;
	}

	return false;
#endif

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

#if (DEBUG_IN_STOP_MODE == 1)
	 LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_DBGMCU);
	 DBGMCU->CR |= DBGMCU_CR_DBG_SLEEP | DBGMCU_CR_DBG_STOP | DBGMCU_CR_DBG_STANDBY;
	 DBGMCU->APB1FZ |= (DBGMCU_APB1_FZ_DBG_TIM2_STOP |
	 DBGMCU_APB1_FZ_DBG_TIM3_STOP) |
	 DBGMCU_APB1_FZ_DBG_TIM6_STOP |
	 DBGMCU_APB1_FZ_DBG_TIM7_STOP |
	 DBGMCU_APB1_FZ_DBG_RTC_STOP  |
	 DBGMCU_APB1_FZ_DBG_WWDG_STOP |
	 DBGMCU_APB1_FZ_DBG_IWDG_STOP |
	 DBGMCU_APB1_FZ_DBG_I2C1_STOP |
	 DBGMCU_APB1_FZ_DBG_I2C2_STOP |
	 DBGMCU_APB1_FZ_DBG_I2C3_STOP |
	DBGMCU_APB1_FZ_DBG_LPTIMER_STOP;

	DBGMCU->APB2FZ |= DBGMCU_APB2_FZ_DBG_TIM22_STOP | DBGMCU_APB2_FZ_DBG_TIM21_STOP;

#endif

	DS_Init();
	(void)DS_RegisterPeripher(DS_PERI_TYPE_TIM,DS_PERI_TIMER_PART_CNT,(uint32_t *)KEYBOARD_TIMER);
	(void)DS_RegisterPeripher(DS_PERI_TYPE_ADC,DS_PERI_ADC_ONGOING,(uint32_t *)SYSTEM_ADC);
	(void)DS_RegisterPeripher(DS_PERI_TYPE_TIM,DS_PERI_TIMER_PART_CNT,(uint32_t *)TIM22);
#if (KEYBOARD_SOUND == 1)
	(void)DS_RegisterPeripher(DS_PERI_TYPE_TIM,DS_PERI_TIMER_PART_PWM,(uint32_t *)TIM3);
#endif

	//DS_RegisterPeripher(DS_PERI_TYPE_TIM,DS_PERI_TIMER_PART_PWM,(uint32_t *)PWM_TIMER);

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC_Init();
  MX_CRC_Init();
  MX_LCD_Init();
  MX_SPI1_Init();
  MX_TIM7_Init();
  MX_WWDG_Init();
  MX_RTC_Init();
  MX_TIM6_Init();
  MX_TIM3_Init();
  MX_TIM22_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  FlashOB_Init();

  /* Systick turn of -we use a different timer*/
  SysTick->CTRL&=~ SysTick_CTRL_ENABLE_Msk;

  LL_GPIO_SetOutputPin(BOOST_EN_GPIO_Port,BOOST_EN_Pin);
  LL_GPIO_SetOutputPin(BACKLIGHT_GPIO_Port,BACKLIGHT_Pin);

  /* RF preruseni - zakazat*/
  EXTI_DisableIT_0_31(EXTI_LINE_15);
  /* od nabijecky zakazat */
  EXTI_DisableIT_0_31(EXTI_LINE_11);

  LL_GPIO_ResetOutputPin(BT_EN_GPIO_Port, BT_EN_Pin);
  LL_GPIO_ResetOutputPin(BACKLIGHT_GPIO_Port, BACKLIGHT_Pin);


  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
 *
 * @param GPIO_Pin
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
	case GPIO_PIN_0:
	case GPIO_PIN_1:
	case GPIO_PIN_2:
	case GPIO_PIN_3:
	case GPIO_PIN_4:
	case GPIO_PIN_5:
	case GPIO_PIN_6:
	case GPIO_PIN_7:
		CallbackButtonIRQ(false);
		break;
	case GPIO_PIN_15:
		CallBackRFIRQ();
		break;

	case CHARGER_STAT_Pin:
		CallbackChargerState();
		break;
	default:
		break;
	}
}

/**
 *
 * @param htim
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	tDataQueue sendData;
	sendData.pointer=NULL;
	signed portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken=pdFALSE;

	if (htim->Instance == TIM2)
	{

	}
	else if(htim->Instance == TIM3)
	{

	}
	else if(htim->Instance == TIM6)
	{
		sendData.address = ADDR_TO_CORE_WATCHDOG_ACTIONS;
		sendData.data = WD_TIME_FOR_WATCHDOG;

		if(xQueueSendFromISR(QueueCoreHandle,&sendData,&xHigherPriorityTaskWoken)!=pdPASS)
		{
			LogError(1289);
		}
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
	else if(htim->Instance == TIM7)
	{
		KeyboardTimElapsed(htim);
	}
	else if(htim->Instance == TIM22)
	{
		CallbackChargerEdgeElapsed();
	}
}

/*
 *
 */
void MallocFailed(void)
{
	LogError(5123);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	LogError(65315321);

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
