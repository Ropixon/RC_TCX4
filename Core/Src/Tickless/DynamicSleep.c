/*
 * DynamicSleep.c
 *
 *  Created on: 15. 11. 2019
 *      Author: developer
 */

#include "main.h"
#include "sleepmgr.h"
#include "DynamicSleep.h"

#define DS_MAX_PERI_TO_REGISTER			10

DS_PERI_Struct	GL_AllPeripherals[DS_MAX_PERI_TO_REGISTER];

/**
 *
 */
void DS_Init(void)
{
	for(uint8_t i=0;i<DS_MAX_PERI_TO_REGISTER;i++)
	{
		GL_AllPeripherals[i].PeriType=DS_PERI_TYPE_NONE;
	}

}


/**
 * registrace periferii, jenz muzou menit hloubku spanku
 * @param PeriType
 * @param PeriPart
 * @param Peri
 */
uint8_t DS_RegisterPeripher(DS_ENUM_PERI_TYPE PeriType,DS_ENUM_PERI_PART PeriPart,uint32_t  *Peri)
{
	uint8_t empty=0xff;

	for(uint8_t i=0;i<DS_MAX_PERI_TO_REGISTER;i++)
	{
		if(GL_AllPeripherals[i].PeriType==DS_PERI_TYPE_NONE)
		{
			empty=i;
			break;
		}

	}
	if(empty==0xff)	 LogError(868451);

	GL_AllPeripherals[empty].PeriType=PeriType;
	GL_AllPeripherals[empty].PeriPart=PeriPart;

	switch (PeriType)
	{
		case DS_PERI_TYPE_TIM:
			GL_AllPeripherals[empty].Registers.Timer=(TIM_TypeDef*)Peri;
			break;

		case DS_PERI_TYPE_USART:
			GL_AllPeripherals[empty].Registers.Usarts=(USART_TypeDef*)Peri;
			break;

		case DS_PERI_TYPE_DMA:
			GL_AllPeripherals[empty].Registers.Dmas=(DMA_TypeDef*)Peri;
			break;

		case DS_PERI_TYPE_ADC:
			GL_AllPeripherals[empty].Registers.Adcs=(ADC_TypeDef*)Peri;
			break;

		case DS_GENERAL_TYPE:
			GL_AllPeripherals[empty].Registers.general=true;
			break;

		default:
			break;
	}

	return empty;
}

/**
 *
 * @return
 */
eSleepModes DS_GetPossibleDepthOfSleep(void)
{
//	return SLEEPMGR_ACTIVE;

	for(uint8_t i=0;i<DS_MAX_PERI_TO_REGISTER;i++)
	{
		if(GL_AllPeripherals[i].PeriType!=DS_PERI_TYPE_NONE)
		{
			switch (GL_AllPeripherals[i].PeriType)
			{
				case DS_PERI_TYPE_TIM:
					if(GL_AllPeripherals[i].PeriPart==DS_PERI_TIMER_PART_CNT)
					{
						if(GL_AllPeripherals[i].Registers.Timer->CR1 & TIM_CR1_CEN) return SLEEPMGR_ACTIVE;
					}
					else if(GL_AllPeripherals[i].PeriPart==DS_PERI_TIMER_PART_PWM)
					{
						if(GL_AllPeripherals[i].Registers.Timer->CCER & (TIM_CCER_CC1E|TIM_CCER_CC2E|TIM_CCER_CC3E|TIM_CCER_CC4E)) return SLEEPMGR_ACTIVE;
					}

					break;

				case DS_PERI_TYPE_ADC:
					if(GL_AllPeripherals[i].PeriPart==DS_PERI_ADC_ONGOING)
					{
						if(GL_AllPeripherals[i].Registers.Adcs->CR & ADC_CR_ADEN) return SLEEPMGR_ACTIVE;
					}
					break;

				case DS_PERI_TYPE_DMA:
					if(GL_AllPeripherals[i].PeriPart==DS_PERI_DMA_ONGOING)
					{
						// Check if any DMA channel is active (GIF = Global Interrupt Flag indicates channel activity)
						if(GL_AllPeripherals[i].Registers.Dmas->ISR & (DMA_ISR_GIF1|DMA_ISR_GIF2|DMA_ISR_GIF3|DMA_ISR_GIF4|DMA_ISR_GIF5|DMA_ISR_GIF6|DMA_ISR_GIF7)) return SLEEPMGR_ACTIVE;
					}
					break;

				case DS_GENERAL_TYPE:
					if(GL_AllPeripherals[i].Registers.general==true)  return SLEEPMGR_ACTIVE;
					break;

				default:
					break;
			}

		}
	}

	if(mcuIsLocked() == false) return SLEEPMGR_ACTIVE;	//kvuli factory testu

	/* Checking Pins. */
	if((LL_GPIO_ReadInputPort(RF_GPIO2_GPIO_Port)&RF_GPIO2_Pin))		return SLEEPMGR_ACTIVE;	// for TX State
	if((LL_GPIO_ReadOutputPort(BACKLIGHT_GPIO_Port)&BACKLIGHT_Pin))		return SLEEPMGR_ACTIVE;	// podsvit
	if(!(LL_GPIO_ReadOutputPort(RF_NSEL_GPIO_Port)&RF_NSEL_Pin))		return SLEEPMGR_ACTIVE;	// SPI RF chip select pin is low

#if (VERSION_WITH_RING==1)
	/* je zapnuty prstynek? - zakaz spanek */
	if((LL_GPIO_ReadOutputPort(BT_EN_GPIO_Port)&BT_EN_Pin))				return SLEEPMGR_SLEEP;	// prstynek je zapnuty
#endif


	return SLEEPMGR_STOP_RTC_RUN;

}

/**
 * Nastavení aktivity obecné periferie
 * @param periIndex Index periferie získaný při registraci
 * @param active True pro aktivaci, False pro deaktivaci
 */
void DS_SetGeneralPeripheralActive(uint8_t periIndex, bool active)
{
    // Kontrola platnosti indexu
    if (periIndex < DS_MAX_PERI_TO_REGISTER)
    {
        // Kontrola, zda se jedná o obecnou periferii
        if (GL_AllPeripherals[periIndex].PeriType == DS_GENERAL_TYPE)
        {
            GL_AllPeripherals[periIndex].Registers.general = active;
        }
    }
}
