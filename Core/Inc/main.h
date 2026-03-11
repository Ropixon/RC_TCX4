/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define USE_FULL_LL_DRIVER				1
#include "stm32l0xx_ll_adc.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_comp.h"
#include "stm32l0xx_ll_rcc.h"
#include "stm32l0xx_ll_system.h"
#include "stm32l0xx_ll_utils.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_tim.h"
#include "stm32l0xx_ll_exti.h"
#include "stm32l0xx_ll_lptim.h"
#include "stm32l0xx_ll_dma.h"
#include "stm32l0xx_ll_spi.h"
#include "stm32l0xx_ll_rtc.h"
#include "stm32l0xx_ll_usart.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

#include "cmsis_os.h"
#include "stdbool.h"
#include "stdint.h"
#include "string.h"
#include "math.h"
#include "stdarg.h"
#include "compiler.h"
#include "exti.h"
#include "SystemConfig.h"
#include "wwdg.h"


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

//#define USE_FULL_LL_DRIVER

typedef struct
{
  uint32_t 	address;
  uint32_t 	data;
  uint32_t  temp_32;
  bool		temp_bool;
  uint32_t  RFU_32;
  uint64_t	RFU_64;
  void* pointer;
} tDataQueue;


/**
 *
 */
typedef enum
{
	WD_NONE = 0,             //!< WD_NONE
	WD_TIME_FOR_WATCHDOG = 1,//!< WD_TIME_FOR_WATCHDOG
	WD_RF_IS_ALIVE = 2,      //!< WD_RF_IS_ALIVE
	WD_LCD_IS_ALIVE = 3

}eWatchDog;


/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void SendErrorCode_AND_Reset(char id, char code);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BT_EN_Pin GPIO_PIN_13
#define BT_EN_GPIO_Port GPIOC
#define BATT_LOAD_Pin GPIO_PIN_14
#define BATT_LOAD_GPIO_Port GPIOC
#define VIBRATION_Pin GPIO_PIN_15
#define VIBRATION_GPIO_Port GPIOC
#define FW_VER_1_Pin GPIO_PIN_0
#define FW_VER_1_GPIO_Port GPIOH
#define FW_VER_2_Pin GPIO_PIN_1
#define FW_VER_2_GPIO_Port GPIOH
#define SW1_ON_OFF_Pin GPIO_PIN_0
#define SW1_ON_OFF_GPIO_Port GPIOC
#define SW1_ON_OFF_EXTI_IRQn EXTI0_1_IRQn
#define SW2_PLUS_Pin GPIO_PIN_1
#define SW2_PLUS_GPIO_Port GPIOC
#define SW2_PLUS_EXTI_IRQn EXTI0_1_IRQn
#define SW3_MINUS_Pin GPIO_PIN_2
#define SW3_MINUS_GPIO_Port GPIOC
#define SW3_MINUS_EXTI_IRQn EXTI2_3_IRQn
#define SW4_LEFT_Pin GPIO_PIN_3
#define SW4_LEFT_GPIO_Port GPIOC
#define SW4_LEFT_EXTI_IRQn EXTI2_3_IRQn
#define ADC_ACU_Pin GPIO_PIN_0
#define ADC_ACU_GPIO_Port GPIOA
#define ADC_BATT_Pin GPIO_PIN_4
#define ADC_BATT_GPIO_Port GPIOA
#define RF_SCK_Pin GPIO_PIN_5
#define RF_SCK_GPIO_Port GPIOA
#define SW5_RIGHT_Pin GPIO_PIN_4
#define SW5_RIGHT_GPIO_Port GPIOC
#define SW5_RIGHT_EXTI_IRQn EXTI4_15_IRQn
#define SW6_BEEP_Pin GPIO_PIN_5
#define SW6_BEEP_GPIO_Port GPIOC
#define SW6_BEEP_EXTI_IRQn EXTI4_15_IRQn
#define BOOST_EN_Pin GPIO_PIN_10
#define BOOST_EN_GPIO_Port GPIOB
#define CHARGER_STAT_Pin GPIO_PIN_11
#define CHARGER_STAT_GPIO_Port GPIOB
#define CHARGER_STAT_EXTI_IRQn EXTI4_15_IRQn
#define RF_GPIO1_CTS_Pin GPIO_PIN_13
#define RF_GPIO1_CTS_GPIO_Port GPIOB
#define RF_GPIO2_Pin GPIO_PIN_14
#define RF_GPIO2_GPIO_Port GPIOB
#define RF_NIRQ_Pin GPIO_PIN_15
#define RF_NIRQ_GPIO_Port GPIOB
#define RF_NIRQ_EXTI_IRQn EXTI4_15_IRQn
#define SW7_PULSE_Pin GPIO_PIN_6
#define SW7_PULSE_GPIO_Port GPIOC
#define SW7_PULSE_EXTI_IRQn EXTI4_15_IRQn
#define SW8_BOOST_Pin GPIO_PIN_7
#define SW8_BOOST_GPIO_Port GPIOC
#define SW8_BOOST_EXTI_IRQn EXTI4_15_IRQn
#define RF_MISO_Pin GPIO_PIN_11
#define RF_MISO_GPIO_Port GPIOA
#define RF_MOSI_Pin GPIO_PIN_12
#define RF_MOSI_GPIO_Port GPIOA
#define RF_GPIO0_Pin GPIO_PIN_15
#define RF_GPIO0_GPIO_Port GPIOA
#define RF_NSEL_Pin GPIO_PIN_10
#define RF_NSEL_GPIO_Port GPIOC
#define TP205_SCOPE_Pin GPIO_PIN_11
#define TP205_SCOPE_GPIO_Port GPIOC
#define TP204_SCOPE_Pin GPIO_PIN_12
#define TP204_SCOPE_GPIO_Port GPIOC
#define BACKLIGHT_Pin GPIO_PIN_2
#define BACKLIGHT_GPIO_Port GPIOD
#define RF_SDN_Pin GPIO_PIN_8
#define RF_SDN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
/***************************************************/
#define	VERSION								0
#define SUBVERSION							0

/****************************************************
* CODE OF RTOS QUEUE
***************************************************/
#define DATA_VALUE_EMPTY					0


/*!
 *	Address for Core Task
 */
#define ADDR_TO_CORE_TASK_STATE_CHANGED			255
#define ADDR_TO_CORE_KEYBOARD_EVENT				254
#define ADDR_TO_CORE_STOP_TX					253
#define ADDR_TO_CORE_START_BAT_MEAS				252
#define ADDR_TO_CORE_ADC_MEAS_DONE				251
#define	ADDR_TO_CORE_WATCHDOG_ACTIONS			250
#define	ADDR_TO_CORE_LOCK_SHOCK_TOOGLE			249
#define ADDR_TO_CORE_FACTORY_TEST_DATA_RX		248
#define ADDRT_TO_CORE_FT_START_CW				247
#define ADDRT_TO_CORE_FT_STOP_CW				246
#define ADDRT_TO_CORE_REPEAT_VIBRATION			245
#define ADDR_TO_CORE_CHARGER_CHANGE				244
#define ADDR_TO_CORE_START_SHUTDOWN				243
#define ADDR_TO_CORE_CHECK_UART_MSG				242
#define ADDR_TO_CORE_RING_HB_ELAPSED			241
#define ADDR_TO_CORE_RING_UNPAIR_RING			240
#define	ADDR_TO_CORE_ASCENDING_BOOST			238	//increase actual boost
#define ADDR_TO_CORE_ONE_PACKET_SENT			237
/*!
 *	Data for Core Task
 */
#define DATA_TO_CORE_RF_IS_ON				255
#define DATA_TO_CORE_RF_IS_OFF				254
#define DATA_TO_CORE_SHORT_PRESS_EVENT		253
#define DATA_TO_CORE_LONG_PRESS_EVENT		252
#define DATA_TO_CORE_REPEATE_PRESS_EVENT	251
#define DATA_TO_CORE_LCD_IS_ON				250
#define DATA_TO_CORE_LCD_IS_OFF				249
#define DATA_TO_CORE_CHARGER_CHANGE			248
#define DATA_TO_CORE_CHARGER_TIMEOUT		247
#define DATA_TO_CORE_MED_LONG_PRESS_EVENT	246

/*!
 *	Address for RF Task
 */
#define ADDR_TO_RF_CHANGE_STATE				255
#define ADDR_TO_RF_CMD						254
#define ADDR_TO_RF_IRQ_FIRED				253
#define ADDR_TO_RF_1_PACKET_TX_DONE			252
#define ADDR_TO_RF_CHECK_RF_IS_ALIVE		251

/*!
 * Data for RF Task
 */
#define DATA_TO_RF_START_ON					255
#define DATA_TO_RF_START_OFF				254


/*!
 *	Address for LCD Task
 */
#define ADDR_TO_LCD_CHANGE_STATE				255
#define ADDR_TO_LCD_REFRESH_PAGE				254
#define ADDR_TO_LCD_CHECK_LCD_IS_ALIVE			253
#define ADDR_TO_LCD_LCD_TEST					252


/*!
 * Data for LCD Task
 */
#define DATA_TO_LCD_START_ON					255
#define DATA_TO_LCD_START_OFF					254



#define ADDR_CORE_SWITCH_REPEAT_EVENT		251

//Address with prohibition storage in state of core task
#define ADDR_CORE_WITHOUT_STORAGE_NOT_USED	150
#define ADDR_CORE_REFRESH_WATCHDOG			149


/****************************************************
 *  Packet / System
 ***************************************************/
#define DUMMY_BYTE              	0xFF
#define ERASE_NUMBER				0xFFFFFFFF


void LogError(uint64_t error);
bool mcuIsLocked(void);
void MallocFailed(void);
void osDelayWatchdog(uint32_t msDelay);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
