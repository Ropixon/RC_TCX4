/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lcd.c
  * @brief   This file provides code for the configuration
  *          of the LCD instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "lcd.h"

/* USER CODE BEGIN 0 */
#include "cmsis_os.h"

//static HAL_StatusTypeDef RTS_LCD_Write(LCD_HandleTypeDef *hlcd,
//		uint32_t RAMRegisterIndex, uint32_t RAMRegisterMask, uint32_t Data);
/* USER CODE END 0 */

LCD_HandleTypeDef hlcd;

/* LCD init function */
void MX_LCD_Init(void)
{

  /* USER CODE BEGIN LCD_Init 0 */

  /* USER CODE END LCD_Init 0 */

  /* USER CODE BEGIN LCD_Init 1 */

  /* USER CODE END LCD_Init 1 */
  hlcd.Instance = LCD;
  hlcd.Init.Prescaler = LCD_PRESCALER_1;
  hlcd.Init.Divider = LCD_DIVIDER_16;
  hlcd.Init.Duty = LCD_DUTY_1_4;
  hlcd.Init.Bias = LCD_BIAS_1_4;
  hlcd.Init.VoltageSource = LCD_VOLTAGESOURCE_INTERNAL;
  hlcd.Init.Contrast = LCD_CONTRASTLEVEL_4;
  hlcd.Init.DeadTime = LCD_DEADTIME_1;
  hlcd.Init.PulseOnDuration = LCD_PULSEONDURATION_1;
  hlcd.Init.HighDrive = LCD_HIGHDRIVE_1;
  hlcd.Init.BlinkMode = LCD_BLINKMODE_OFF;
  hlcd.Init.BlinkFrequency = LCD_BLINKFREQUENCY_DIV8;
  hlcd.Init.MuxSegment = LCD_MUXSEGMENT_DISABLE;
  if (HAL_LCD_Init(&hlcd) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the High Driver
  */
  __HAL_LCD_HIGHDRIVER_ENABLE(&hlcd);
  /* USER CODE BEGIN LCD_Init 2 */

  /* USER CODE END LCD_Init 2 */

}

void HAL_LCD_MspInit(LCD_HandleTypeDef* lcdHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(lcdHandle->Instance==LCD)
  {
  /* USER CODE BEGIN LCD_MspInit 0 */

  /* USER CODE END LCD_MspInit 0 */
    /* LCD clock enable */
    __HAL_RCC_LCD_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**LCD GPIO Configuration
    PA1     ------> LCD_SEG0
    PA2     ------> LCD_SEG1
    PA3     ------> LCD_SEG2
    PA6     ------> LCD_SEG3
    PA7     ------> LCD_SEG4
    PB0     ------> LCD_SEG5
    PB1     ------> LCD_SEG6
    PA8     ------> LCD_COM0
    PA9     ------> LCD_COM1
    PA10     ------> LCD_COM2
    PB3     ------> LCD_SEG7
    PB4     ------> LCD_SEG8
    PB5     ------> LCD_SEG9
    PB9     ------> LCD_COM3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_LCD;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_LCD;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN LCD_MspInit 1 */

  /* USER CODE END LCD_MspInit 1 */
  }
}

void HAL_LCD_MspDeInit(LCD_HandleTypeDef* lcdHandle)
{

  if(lcdHandle->Instance==LCD)
  {
  /* USER CODE BEGIN LCD_MspDeInit 0 */

  /* USER CODE END LCD_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_LCD_CLK_DISABLE();

    /**LCD GPIO Configuration
    PA1     ------> LCD_SEG0
    PA2     ------> LCD_SEG1
    PA3     ------> LCD_SEG2
    PA6     ------> LCD_SEG3
    PA7     ------> LCD_SEG4
    PB0     ------> LCD_SEG5
    PB1     ------> LCD_SEG6
    PA8     ------> LCD_COM0
    PA9     ------> LCD_COM1
    PA10     ------> LCD_COM2
    PB3     ------> LCD_SEG7
    PB4     ------> LCD_SEG8
    PB5     ------> LCD_SEG9
    PB9     ------> LCD_COM3
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_9);

  /* USER CODE BEGIN LCD_MspDeInit 1 */

  /* USER CODE END LCD_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void LCD_Write(uint32_t RAMRegisterIndex, uint32_t RAMRegisterMask, uint32_t Data)
{
	HAL_LCD_Write(&hlcd, RAMRegisterIndex, ~RAMRegisterMask, Data);
}

/**
 *
 */
void LCD_UpdateDisplayRequest(void)
{
	osThreadSuspendAll();
	HAL_LCD_UpdateDisplayRequest(&hlcd);
	osThreadResumeAll();
}
/* USER CODE END 1 */
