/*
 * LCD_LowLevel.c
 *
 *  Created on: Jun 26, 2021
 *      Author: developer
 */


#include "TaskCore.h"
#include "LCD_LowLevel.h"
#include "lcd.h"
#include "SignalProcessing.h"
#include "ProcessLCDTask.h"


#define LCD_SYMBOL_R		(254)
#define LCD_SYMBOL_OK		(253)
#define LCD_SYMBOL_UNPAIR	(252)

/**
 *
 */
void LCD_DrawMainScreen(tSharedDataLCD_Core *info)
{
	uint8_t valueToView;

	/* antena */
	if (info->txActive != TX_ACTIVE_NO)	LCD_ShowTxActive();
	else								LCD_ClearTxActive(true);

	/* hodnota modu  */
	if(((info->txActive == TX_ACTIVE_BOOSTER) || (info->dogsInfo[info->selectedDog].activeBooter == true))
#if ASCENDING_MODE_ENABLED == true
			&& ((info->dogsInfo[info->selectedDog].ascendingMode == false) || (info->dogsInfo[info->selectedDog].onlyVibration == true) )
#endif
			)
	{
		valueToView = SP_ConstrainU8(info->dogsInfo[info->selectedDog].boosterValue, 0, SHOCK_MAX);

		if(info->dogsInfo[info->selectedDog].onlyVibration == true)
		{
			valueToView = 0;
		}
		else
		{
			valueToView = SP_ConstrainU8(info->dogsInfo[info->selectedDog].boosterValue, 0, SHOCK_MAX);
		}

		LCD_ShowNumber(valueToView,true,false);
	}
#if ASCENDING_MODE_ENABLED == true
	else if(info->txActive == TX_ACTIVE_BOOSTER)
	{
		valueToView = SP_ConstrainU8(info->dogsInfo[info->selectedDog].ascendingShockValue, 0, SHOCK_MAX);
		if(info->dogsInfo[info->selectedDog].ascendingMode == true)
		{
			LCD_ShowNumber(valueToView,false,false);
		}
		else
		{
			LCD_ShowNumber(valueToView,true,false);
		}
	}
#endif
	else if(info->activeSettingsNumberOfDogs == true)
	{
		valueToView = SP_ConstrainU8(info->numberOfDogs, 1, info->numberOfDogs);
		LCD_ShowNumber(valueToView,true,false);
		LCD_ShowDogID_Train(info->numberOfDogs);
		LCD_ClearModeIcon(false);
	}
#if ASCENDING_MODE_ENABLED == true
	else if(info->dogsInfo[info->selectedDog].ascendingMode == true && info->dogsInfo[info->selectedDog].activeBooter == true)
	{
		LCD_ShowNumber(0,false,true);
		LCD_ShowModeIcon();
	}
#endif
	else
	{
		valueToView = info->dogsInfo[info->selectedDog].shockValue;
		LCD_ShowNumber(valueToView,false,false);
		LCD_ShowModeIcon();
	}

	/* shock / vibrace */
	if (info->activeSettingsNumberOfDogs==false)
	{
		if(((info->dogsInfo[info->selectedDog].boosterValue == 0)
				|| (info->dogsInfo[info->selectedDog].onlyVibration == true))
#if ASCENDING_MODE_ENABLED == true
				&&(info->dogsInfo[info->selectedDog].ascendingMode == false)
#endif
		)
		{
			LCD_ShowShockOrVibration(false);
		}
		else
		{
			LCD_ShowShockOrVibration(true);
		}
	}
	else
	{
		LCD_ClearShockOrVibration(false);
	}

	/*Lock shock edit */
	LCD_ShowLockShockEdit(info->lockBottomKeys);

	/* zamek na shock */
	if (info->activeSettingsNumberOfDogs==false)
	{
		LCD_ShowLockShock(info->dogsInfo[info->selectedDog].onlyVibration);
	}
	else
	{
		LCD_ShowLockShock(false);
	}

	/* ID psa */
	if (info->activeSettingsNumberOfDogs==false)
	{
		LCD_ShowDogID(info->selectedDog);
	}

	/* Continual/Nick */
	if ((info->dogsInfo[info->selectedDog].onlyVibration == false) && (info->activeSettingsNumberOfDogs==false))
	{
		LCD_ShowModeNC(info->dogsInfo[info->selectedDog].modeNC);
	}
	else
	{
		LCD_ClearMode(true);
	}

	/* Battl level */
	if ((info->chargerState == CHARGER_UNPLUGGED) || (info->chargerState == CHARGER_BATT_FULL) )
	{
		LCD_ShowBattLevels(info->stateBattery);
	}

#if (VERSION_WITH_RING == 1)
	LCD_Show_RingState(info->ringInfo.ringIsOn,true);
	if(info->ringInfo.ringIsOn == true)
	{
		uint8_t functions;
		functions = PLT_GetAllRingFunctions(info);

		LCD_ShowRingBtnFunction(RING_FCE_NONE, true, true);

		if(info->ringInfo.txActiveBy == TX_BY_NORDIC)
		{
			switch (info->txActive)
			{
				case TX_ACTIVE_BEEP:
					LCD_ShowRingBtnFunction(RING_FCE_BEEP, true, true);
					break;

				case TX_ACTIVE_SHOCK:
					LCD_ShowRingBtnFunction(RING_FCE_SHOCK, true, true);
					break;

				case TX_ACTIVE_BOOSTER:
					LCD_ShowRingBtnFunction(RING_FCE_BOOSTER, true, true);
					break;
				default:
					break;
			}
		}
		else
		{
			for (uint8_t fn = 1; fn < RING_FCE_CNT; fn++)
			{
				// Zkontrolujeme, zda je daná funkce nastavena v bitové masce
				if (functions & (1 << fn))
				{
					// Zavoláme zobrazení funkce na LCD
					LCD_ShowRingBtnFunction((eSelectedBtnForRing)fn, false, true);
				}
			}
		}
	}

#endif
}


#if (VERSION_WITH_RING == 1)
/**
 *
 */
void LCD_DrawPairingScreen(tSharedDataLCD_Core *info)
{
	uint8_t tmp;

	if(info->ringInfo.pairSuccess == true)
	{
		/* zobrazime OK - parovani uspesne */
		LCD_ShowNumber(LCD_SYMBOL_OK,false,false);
	}
	else if(info->ringInfo.unPairSuccess == true)
	{
		LCD_ShowNumber(LCD_SYMBOL_UNPAIR,false,false);
	}
	else
	{
		/* cislo parovaneho prstynku 1 - 8 - max. pocet prstynku = 8*/
		tmp = SP_ConstrainU8(info->ringInfo.ringNumber, 1, 8);
		LCD_ShowNumber(tmp,false,false);

		/* rozsvit symbol R - pro parovani prstynku */
		LCD_ShowNumber(LCD_SYMBOL_R,false,false);
	}

	LCD_Show_RingState(info->ringInfo.ringIsOn,true);

	/* zobraz aktualne vybrane tlacitko k parovani*/
	LCD_ShowRingBtnFunction(info->ringInfo.selectedBtn,true,true);

	/* ID psa */
	LCD_ShowDogID(info->selectedDog);
	LCD_Write(LCD_DOG_ICO);

	/* Battl level */
//	if ((info->chargerState == CHARGER_UNPLUGGED) || (info->chargerState == CHARGER_BATT_FULL) )
//	{
//		LCD_ShowBattLevels(info->stateBattery);
//	}
}

#endif

/**
 *
 */
void LCD_ShowDogID(eSelectedDog dog)
{
	LCD_ClearDogsID(false);

	switch (dog)
	{
		case DOG_A:
			LCD_Write(LCD_DOG_1);
			//LCD_Write(LCD_COM0,LCD_MASK_ALL,SEG_C0_ID_A);
			break;

		case DOG_B:
			//LCD_Write(LCD_COM0,LCD_MASK_ALL,SEG_C0_ID_B);
			LCD_Write(LCD_DOG_2);
			break;

#if (VER_4DOGS == 1)
		case DOG_C:
			//LCD_Write(LCD_COM1,LCD_MASK_ALL,SEG_C1_ID_C);
			LCD_Write(LCD_DOG_3);
			break;

		case DOG_D:
			LCD_Write(LCD_DOG_4);
			//LCD_Write(LCD_COM2,LCD_MASK_ALL,SEG_C2_ID_D);
			break;

		case DOG_E:
			//LCD_Write(LCD_COM1,LCD_MASK_ALL,SEG_C1_ID_C);
			LCD_Write(LCD_DOG_5);
			break;

		case DOG_F:
			LCD_Write(LCD_DOG_6);
			//LCD_Write(LCD_COM2,LCD_MASK_ALL,SEG_C2_ID_D);
			break;

#endif
		default:
			break;
	}

	LCD_UpdateDisplayRequest();
}

void LCD_ShowDogID_Train(eSelectedDog dog)
{
	LCD_ClearDogsID(false);

	switch (dog-1)
	{
		case DOG_A:
			LCD_Write(LCD_DOG_1);
			break;
		case DOG_B:
			LCD_Write(LCD_DOG_1);
			LCD_Write(LCD_DOG_2);
			break;
		case DOG_C:
			LCD_Write(LCD_DOG_1);
			LCD_Write(LCD_DOG_2);
			LCD_Write(LCD_DOG_3);
			break;
		case DOG_D:
			LCD_Write(LCD_DOG_1);
			LCD_Write(LCD_DOG_2);
			LCD_Write(LCD_DOG_3);
			LCD_Write(LCD_DOG_4);
			break;
		case DOG_E:
			LCD_Write(LCD_DOG_1);
			LCD_Write(LCD_DOG_2);
			LCD_Write(LCD_DOG_3);
			LCD_Write(LCD_DOG_4);
			LCD_Write(LCD_DOG_5);
			break;
		case DOG_F:
			LCD_Write(LCD_DOG_1);
			LCD_Write(LCD_DOG_2);
			LCD_Write(LCD_DOG_3);
			LCD_Write(LCD_DOG_4);
			LCD_Write(LCD_DOG_5);
			LCD_Write(LCD_DOG_6);
			break;
		default:
			break;
	}

	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_ShowModeNC(eMode mode)
{
	LCD_ClearMode(false);

#if (NC_MODE_AVAILABLE == 1)
	switch (mode)
	{
		case MODE_CON:
			LCD_Write(LCD_MODE_C);
			break;

		case MODE_NICK:
			LCD_Write(LCD_MODE_N);
			break;

		default:
			break;
	}

	LCD_Write(LCD_MODE_ICON);
#endif

	LCD_UpdateDisplayRequest();
}


/**
 *
 */
void LCD_ShowShockOrVibration(bool shock)
{
	LCD_ClearShockOrVibration(false);

	if(shock)	LCD_Write(LCD_PULSE);
	else		LCD_Write(LCD_VIBRAT);

	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_ShowLockShock(bool showIt)
{
	if (showIt == true) LCD_Write(LCD_NO_SHOCK);
	else				LCD_ClearLockShock(false);

	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_ShowLockShockEdit(bool lock)
{
	if(lock)
	{
		LCD_Write(LCD_LOCK);
	}
	else
	{
		LCD_ClearLockShockEdit(false);
	}

	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_ShowNumber(uint8_t number,bool booster,bool ascendingMode)
{
	uint8_t hDigit = number/10;
	uint8_t lDigit = number%10;

#if VERSION_WITH_RING == 1
	/* ukazeme R pri parovani prstynku */
	if(number == LCD_SYMBOL_R)
	{
		LCD_Show_1_R();
		return;
	}
	else
#endif

#if ASCENDING_MODE_ENABLED == false
		ascendingMode = false;
#endif

#if ASCENDING_MODE_ENABLED == true
	if(ascendingMode == true)
	{
		LCD_Clear_1_All(true);
		LCD_Show_2_P();
		return;
	}
#endif

	if(number == LCD_SYMBOL_OK)
	{
		LCD_Show_1_0();
		LCD_Show_2_K();
		return;
	}
	else if(number == LCD_SYMBOL_UNPAIR)
	{
		LCD_Show_1_EMPTY();
		LCD_Show_2_EMPTY();
				return;
	}

	if(hDigit)
	{
		switch (hDigit)
		{
			case 1:
				LCD_Show_1_1();
				break;
			case 2:
				LCD_Show_1_2();
				break;
			case 3:
				LCD_Show_1_3();
				break;
			case 4:
				LCD_Show_1_4();
				break;
			case 5:
				LCD_Show_1_5();
				break;
			case 6:
				LCD_Show_1_6();
				break;
			case 7:
				LCD_Show_1_7();
				break;
			case 8:
				LCD_Show_1_8();
				break;
			case 9:
				LCD_Show_1_9();
				break;

			case 0:
				LCD_Show_1_0();
				break;
			default:
				break;
		}
	}
	else
	{
		LCD_Clear_1_All(true);
	}

	switch (lDigit)
	{
		case 1:
			LCD_Show_2_1();
			break;
		case 2:
			LCD_Show_2_2();
			break;
		case 3:
			LCD_Show_2_3();
			break;
		case 4:
			LCD_Show_2_4();
			break;
		case 5:
			LCD_Show_2_5();
			break;
		case 6:
			LCD_Show_2_6();
			break;
		case 7:
			LCD_Show_2_7();
			break;
		case 8:
			LCD_Show_2_8();
			break;
		case 9:
			LCD_Show_2_9();
			break;

		case 0:


			{
				if((hDigit == 0) && (booster == true))	LCD_Show_2_V();
				else 									LCD_Show_2_0();

			}

			break;

		default:
			break;
	}

}

/**
 *
 */
void LCD_ShowAllFrames()
{
	LCD_Write(LCD_ALL_FRAMES);
	LCD_Write(LCD_DOG_ICO);
	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_ShowBattLevels(eBatteryStatus batt)
{
	LCD_ClearBattLevels(false);
	LCD_Write(LCD_BAT_FRAME);
	switch (batt)
	{
		case BATTERY_STATUS_CRITICAL:
		case BATTERY_STATUS_UNKNOWN:

			break;

		case BATTERY_STATUS_LOW:
			LCD_Write(LCD_BAT_LOW);

			break;

		case BATTERY_STATUS_MED:
			LCD_Write(LCD_BAT_LOW);
			LCD_Write(LCD_BAT_MED);

			break;

		case BATTERY_STATUS_HIGH:
			LCD_Write(LCD_BAT_LOW);
			LCD_Write(LCD_BAT_MED);
			LCD_Write(LCD_BAT_HIGH);

			break;

		default:
			break;
	}


	LCD_UpdateDisplayRequest();

}

/**
 *
 */
void LCD_ShowTxActive(void)
{
	LCD_ClearTxActive(false);

	LCD_Write(LCD_TX_ICO);
	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_ShowModeIcon(void)
{
	LCD_ClearModeIcon(false);

	LCD_Write(LCD_MODE_ICON);
	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_ClearModeIcon(bool refresh)
{
	LCD_Write(LCD_MODE_ICON_CL);
	if(refresh)	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_ClearTxActive(bool refresh)
{
	LCD_Write(LCD_TX_ICO_CL);
	if(refresh)	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_ClearBattLevels(bool refresh)
{
	LCD_Write(LCD_BAT_LOW_CL);
	LCD_Write(LCD_BAT_MED_CL);
	LCD_Write(LCD_BAT_HIGH_CL);

	if(refresh)	LCD_UpdateDisplayRequest();
}

#if (VERSION_WITH_RING == 1)

/**
 *
 */
void LCD_Show_RingState(bool state, bool refresh)
{
	LCD_Write(LCD_RING_SMALL_R_CL);
	LCD_ShowRingBtnFunction(RING_FCE_NONE,true,false);
	if(state == true)	LCD_Write(LCD_RING_SMALL_R);


	if(refresh)	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_ShowRingBtnFunction(eSelectedBtnForRing selectedBtn,bool deletePrev, bool refresh)
{
	if(deletePrev == true)
	{
		/* nejprve smazene vsechny */
		LCD_Write(LCD_RING_FUNC_1_CL);
		LCD_Write(LCD_RING_FUNC_2_CL);
		LCD_Write(LCD_RING_FUNC_3_CL);
	}

	//TODO zobraz ikonu v kolecku R
	switch (selectedBtn)
	{
		case RING_FCE_SHOCK:
			LCD_Write(LCD_RING_FUNC_1);	//pozdeji editujeme na spravne segmenty

			break;

		case RING_FCE_BEEP:
			LCD_Write(LCD_RING_FUNC_2);
			break;

		case RING_FCE_BOOSTER:
			LCD_Write(LCD_RING_FUNC_3);
			break;

		default:
			break;
	}

	if(refresh)	LCD_UpdateDisplayRequest();

}

/*
 *
 */
void LCD_Clear_Btn1Function(bool refresh)
{
	LCD_Write(LCD_PULSE_CL);

	if(refresh)	LCD_UpdateDisplayRequest();
}

/*
 *
 */
void LCD_Clear_Btn2Function(bool refresh)
{
	LCD_Write(LCD_VIBRAT_CL);
	if(refresh)	LCD_UpdateDisplayRequest();
}

/*
 *
 */
void LCD_Clear_Btn3Function(bool refresh)
{
	LCD_Write(LCD_NO_SHOCK_CL);
	if(refresh)	LCD_UpdateDisplayRequest();
}

#endif

/**
 *
 */
void LCD_Clear_1_All(bool refresh)
{
	NO_1_CLEAR;

	if(refresh)	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_Clear_2_All(bool refresh)
{
	NO_2_CLEAR;

	if(refresh)	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_ClearDogsID(bool refresh)
{
	LCD_Write(LCD_DOG_1_CL);
	LCD_Write(LCD_DOG_2_CL);
	LCD_Write(LCD_DOG_3_CL);
	LCD_Write(LCD_DOG_4_CL);
	LCD_Write(LCD_DOG_5_CL);
	LCD_Write(LCD_DOG_6_CL);

	if(refresh)	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_ClearMode(bool refresh)
{
	LCD_Write(LCD_MODE_N_CL);
	LCD_Write(LCD_MODE_C_CL);

	if(refresh)	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_ClearShockOrVibration(bool refresh)
{
	LCD_Write(LCD_PULSE_CL);
	LCD_Write(LCD_VIBRAT_CL);

	if(refresh)	LCD_UpdateDisplayRequest();
}


/**
 *
 */
void LCD_ClearLockShock(bool refresh)
{
	LCD_Write(LCD_NO_SHOCK_CL);

	if(refresh)	LCD_UpdateDisplayRequest();
}


/**
 *
 */
void LCD_ClearLockShockEdit(bool refresh)
{
	LCD_Write(LCD_LOCK_CL);

	if(refresh)	LCD_UpdateDisplayRequest();
}


/**
 *
 */
void LCD_Show_1_1(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_1_All(false);

	NO_1_1;

	LCD_UpdateDisplayRequest();
}

void LCD_Show_1_2(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_1_All(false);

	NO_1_2;

	LCD_UpdateDisplayRequest();
}

void LCD_Show_1_3(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_1_All(false);

	NO_1_3;

	LCD_UpdateDisplayRequest();
}

void LCD_Show_1_4(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_1_All(false);

	NO_1_4;

	LCD_UpdateDisplayRequest();
}

void LCD_Show_1_5(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_1_All(false);

	NO_1_5;

	LCD_UpdateDisplayRequest();
}

void LCD_Show_1_6(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_1_All(false);

	NO_1_6;

	LCD_UpdateDisplayRequest();
}

void LCD_Show_1_7(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_1_All(false);

	NO_1_7;

	LCD_UpdateDisplayRequest();
}

void LCD_Show_1_8(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_1_All(false);

	NO_1_8;

	LCD_UpdateDisplayRequest();
}

void LCD_Show_1_9(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_1_All(false);

	NO_1_9;

	LCD_UpdateDisplayRequest();
}

void LCD_Show_1_0(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_1_All(false);

	NO_1_0;

	LCD_UpdateDisplayRequest();
}


void LCD_Show_1_EMPTY(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_1_All(false);

	NO_1_EMPTY;

	LCD_UpdateDisplayRequest();
}



void LCD_Show_2_0(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_2_All(false);

	NO_2_0;
	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_Show_2_1(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_2_All(false);

	NO_2_1;

	LCD_UpdateDisplayRequest();
}

void LCD_Show_2_2(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_2_All(false);

	NO_2_2;

	LCD_UpdateDisplayRequest();
}


void LCD_Show_2_3(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_2_All(false);

	NO_2_3;

	LCD_UpdateDisplayRequest();
}

void LCD_Show_2_4(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_2_All(false);

	NO_2_4;

	LCD_UpdateDisplayRequest();
}

void LCD_Show_2_5(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_2_All(false);

	NO_2_5;

	LCD_UpdateDisplayRequest();
}

void LCD_Show_2_6(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_2_All(false);

	NO_2_6;

	LCD_UpdateDisplayRequest();
}

void LCD_Show_2_7(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_2_All(false);

	NO_2_7;

	LCD_UpdateDisplayRequest();
}

void LCD_Show_2_8(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_2_All(false);

	NO_2_8;
	LCD_UpdateDisplayRequest();
}

void LCD_Show_2_9(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_2_All(false);

	NO_2_9;
	LCD_UpdateDisplayRequest();
}



void LCD_Show_2_V(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_2_All(false);

	NO_2_V;

	LCD_UpdateDisplayRequest();
}


void LCD_Show_2_K(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_2_All(false);

	NO_2_OK;

	LCD_UpdateDisplayRequest();
}


#if ASCENDING_MODE_ENABLED == true
void LCD_Show_2_P(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_2_All(false);

	NO_2_P;

	LCD_UpdateDisplayRequest();
}
#endif

void LCD_Show_2_EMPTY(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_2_All(false);

	NO_2_EMPTY;

	LCD_UpdateDisplayRequest();
}

#if (VERSION_WITH_RING == 1)
void LCD_Show_1_R(void)
{
	/* Clear all 7-segment*/
	LCD_Clear_1_All(false);

	NO_1_R;

	LCD_UpdateDisplayRequest();
}
#endif

/**
 *
 */
void LCD_TurnOffAllSegments(void)
{
	LCD_Write(LCD_COM0, 0x3FF,LCD_SEG_NONE);
	LCD_Write(LCD_COM1, 0x3FF,LCD_SEG_NONE);
	LCD_Write(LCD_COM2, 0x3FF,LCD_SEG_NONE);
	LCD_Write(LCD_COM3, 0x3FF,LCD_SEG_NONE);

//	LCD_Write(LCD_PULSE_CL);
//	LCD_Write(LCD_VIBRAT_CL);
//	LCD_Write(LCD_NO_SHOCK_CL);
//	LCD_Write(LCD_DOG_ICO_CL);

	LCD_UpdateDisplayRequest();
}

/**
 *
 */
void LCD_TurnOnAllSegments(void)
{
	LCD_Write(LCD_COM0,LCD_MASK_ALL, 0x3FF);
	LCD_Write(LCD_COM1,LCD_MASK_ALL, 0x3FF);
	LCD_Write(LCD_COM2,LCD_MASK_ALL, 0x3FF);
	LCD_Write(LCD_COM3,LCD_MASK_ALL, 0x3FF);

	LCD_UpdateDisplayRequest();
}




