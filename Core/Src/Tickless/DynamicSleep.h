/*
 * DynamicSleep.h
 *
 *  Created on: 15. 11. 2019
 *      Author: developer
 */

#ifndef DYNAMICSLEEP_H_
#define DYNAMICSLEEP_H_

#include "sleepmgr.h"

/*
 *  Type of periphery
 */
typedef enum
{
	DS_PERI_TYPE_NONE=0,
	DS_PERI_TYPE_TIM=1,
	DS_PERI_TYPE_USART=2,
	DS_PERI_TYPE_DMA=3,
	DS_PERI_TYPE_ADC=4,
	DS_GENERAL_TYPE=5,
//	DS_PERI_TYPE_PORTA=5

}DS_ENUM_PERI_TYPE;

/*
 * Part of periphery
 */
typedef enum
{
	DS_PERI_PART_NONE=0,
	DS_PERI_TIMER_PART_CNT=1,
	DS_PERI_TIMER_PART_PWM=2,
	DS_PERI_ADC_ONGOING=3,
	DS_PERI_DMA_ONGOING=4,
//	DS_PERI_PIN_HIGH=5

	//DS_PERI_TIMER_=3

}DS_ENUM_PERI_PART;



typedef union
{
	//__IO uint32_t 	*TimersCR;
	TIM_TypeDef		*Timer;
	USART_TypeDef	*Usarts;
	DMA_TypeDef		*Dmas;
	ADC_TypeDef		*Adcs;
	bool			general;

	//	uint16_t 		GPIO_Pin;

}UNION_ALL_PERI_TYPES;


typedef struct
{
	DS_ENUM_PERI_TYPE			PeriType;
	DS_ENUM_PERI_PART			PeriPart;
	UNION_ALL_PERI_TYPES 		Registers;

}__packed DS_PERI_Struct;



/*
 *
 */
void DS_Init(void);
uint8_t DS_RegisterPeripher(DS_ENUM_PERI_TYPE PeriType,DS_ENUM_PERI_PART PeriPart,uint32_t  *Peri);
eSleepModes DS_GetPossibleDepthOfSleep(void);

/**
 * @brief Sets the activity flag for a general peripheral
 * @param periIndex Index of the general peripheral
 * @param active True to set peripheral as active (preventing sleep), False to allow sleep
 */
void DS_SetGeneralPeripheralActive(uint8_t periIndex, bool active);

#endif /* DYNAMICSLEEP_H_ */
