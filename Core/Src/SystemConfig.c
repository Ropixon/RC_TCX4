/*
 * SystemConfig.c
 *
 *  Created on: Feb 11, 2022
 *      Author: developer
 */


#include "SystemConfig.h"
#include "main.h"

/* return pcb version */
eHW_Version SC_GetHWVersion(void)
{
	uint8_t helpValue = 0;

	helpValue |= (HAL_GPIO_ReadPin(FW_VER_1_GPIO_Port , FW_VER_1_Pin));
	helpValue |= ((HAL_GPIO_ReadPin(FW_VER_2_GPIO_Port , FW_VER_2_Pin))<<1);

	return (eHW_Version)helpValue;
}
