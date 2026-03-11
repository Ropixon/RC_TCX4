/*
 * flash.c
 *
 *  Created on: 10. 7. 2019
 *      Author: jirik
 */

/************************************************************************/
/* include header												   		*/
/************************************************************************/
#include "main.h"
#include "flash.h"

/************************************************************************/
/* Declaration importing objects                                        */
/************************************************************************/

/************************************************************************/
/* Definition global variables                                          */
/************************************************************************/

/************************************************************************/
/* Local #DEFINE														*/
/************************************************************************/

/************************************************************************/
/* Local TYPEDEF												   		*/
/************************************************************************/

/************************************************************************/
/* Definition local variables										   	*/
/************************************************************************/
FLASH_OBProgramInitTypeDef OBInit;

/************************************************************************/
/* Declaration functions											   	*/
/************************************************************************/

/************************************************************************/
/* Definition functions                                                 */
/************************************************************************/
void FlashOB_Init(void) {
	if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_OPTVERR))
	{
		HAL_FLASHEx_OBGetConfig(&OBInit);
		//HAL_FLASH_OB_Launch();
		OBInit.OptionType = OPTIONBYTE_USER | OPTIONBYTE_BOR;
		OBInit.USERConfig = OB_IWDG_SW | OB_STOP_NORST | OB_STDBY_NORST;
		OBInit.BORLevel = OB_BOR_LEVEL1;

		HAL_FLASH_OB_Unlock();
		HAL_FLASHEx_OBProgram(&OBInit);
		HAL_FLASH_OB_Launch();
	}

	HAL_FLASHEx_OBGetConfig(&OBInit);

	if ((OBInit.USERConfig & (OB_IWDG_SW | OB_STOP_NORST | OB_STDBY_NORST)) != (OB_IWDG_SW | OB_STOP_NORST | OB_STDBY_NORST))
	{
		OBInit.OptionType = OPTIONBYTE_USER;
		OBInit.USERConfig = OB_IWDG_SW | OB_STOP_NORST | OB_STDBY_NORST;
		HAL_FLASH_OB_Unlock();
		HAL_FLASHEx_OBProgram(&OBInit);
		HAL_FLASH_OB_Launch();
	}

	if((OBInit.BORLevel != OB_BOR_LEVEL1))
	{
		OBInit.OptionType = OPTIONBYTE_BOR;
		OBInit.BORLevel = OB_BOR_LEVEL1;
		HAL_FLASH_OB_Unlock();
		HAL_FLASHEx_OBProgram(&OBInit);
		HAL_FLASH_OB_Launch();
	}

}

uint8_t FlashOB_GetRDP(void)
{
	HAL_FLASHEx_OBGetConfig(&OBInit);
	return OBInit.RDPLevel;
}
