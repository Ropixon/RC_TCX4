/*
 * LCD_LowLevel.h
 *
 *  Created on: Jun 26, 2021
 *      Author: developer
 */

#ifndef INC_DISPLAYTASK_LCD_LOWLEVEL_H_
#define INC_DISPLAYTASK_LCD_LOWLEVEL_H_

#include "TaskCore.h"
#include "stdbool.h"
#include "stdint.h"

#define LCD_COM0	LCD_RAM_REGISTER0
#define LCD_COM1	LCD_RAM_REGISTER2
#define LCD_COM2	LCD_RAM_REGISTER4
#define LCD_COM3	LCD_RAM_REGISTER6

#define LCD_MASK_ALL	0x00
#define LCD_SEG_NONE	0x00

#if (OLD_LCD == 0)
#define LCD_DOG_1		LCD_COM0,LCD_MASK_ALL,1<<0
#define LCD_DOG_2		LCD_COM1,LCD_MASK_ALL,1<<0
#define LCD_DOG_3		LCD_COM2,LCD_MASK_ALL,1<<0
#define LCD_DOG_4		LCD_COM2,LCD_MASK_ALL,1<<1
#define LCD_DOG_5		LCD_COM1,LCD_MASK_ALL,1<<1
#define LCD_DOG_6		LCD_COM0,LCD_MASK_ALL,1<<1

#define LCD_DOG_1_CL		LCD_COM0,1<<0,LCD_SEG_NONE
#define LCD_DOG_2_CL		LCD_COM1,1<<0,LCD_SEG_NONE
#define LCD_DOG_3_CL		LCD_COM2,1<<0,LCD_SEG_NONE
#define LCD_DOG_4_CL		LCD_COM2,1<<1,LCD_SEG_NONE
#define LCD_DOG_5_CL		LCD_COM1,1<<1,LCD_SEG_NONE
#define LCD_DOG_6_CL		LCD_COM0,1<<1,LCD_SEG_NONE

#define LCD_MODE_N		LCD_COM1,LCD_MASK_ALL,1<<2
#define LCD_MODE_C		LCD_COM2,LCD_MASK_ALL,1<<2
#define LCD_MODE_ICON	LCD_COM0,LCD_MASK_ALL,1<<2
#define LCD_BAT_LOW		LCD_COM3,LCD_MASK_ALL,1<<2
#define LCD_BAT_MED		LCD_COM3,LCD_MASK_ALL,1<<3
#define LCD_BAT_HIGH	LCD_COM2,LCD_MASK_ALL,1<<3
#define LCD_BAT_FRAME	LCD_COM1,LCD_MASK_ALL,1<<3
#define LCD_TX_ICO		LCD_COM0,LCD_MASK_ALL,1<<3
#define LCD_LOCK		LCD_COM3,LCD_MASK_ALL,1<<1

#define LCD_MODE_N_CL		LCD_COM1,1<<2,LCD_SEG_NONE
#define LCD_MODE_C_CL		LCD_COM2,1<<2,LCD_SEG_NONE
#define LCD_MODE_ICON_CL	LCD_COM0,1<<2,LCD_SEG_NONE
#define LCD_BAT_LOW_CL		LCD_COM3,1<<2,LCD_SEG_NONE
#define LCD_BAT_MED_CL		LCD_COM3,1<<3,LCD_SEG_NONE
#define LCD_BAT_HIGH_CL		LCD_COM2,1<<3,LCD_SEG_NONE
#define LCD_BAT_FRAME_CL	LCD_COM1,1<<3,LCD_SEG_NONE
#define LCD_TX_ICO_CL		LCD_COM0,1<<3,LCD_SEG_NONE
#define LCD_LOCK_CL			LCD_COM3,1<<1,LCD_SEG_NONE


#define LCD_1A			LCD_COM0,LCD_MASK_ALL,1<<6
#define LCD_1B			LCD_COM1,LCD_MASK_ALL,1<<6
#define LCD_1C			LCD_COM2,LCD_MASK_ALL,1<<6
#define LCD_1D			LCD_COM3,LCD_MASK_ALL,1<<6
#define LCD_1E			LCD_COM2,LCD_MASK_ALL,1<<8
#define LCD_1F			LCD_COM0,LCD_MASK_ALL,1<<8
#define LCD_1G			LCD_COM1,LCD_MASK_ALL,1<<8
#define LCD_1H			LCD_COM3,LCD_MASK_ALL,1<<8

#define LCD_1A_CL		LCD_COM0,1<<6,LCD_SEG_NONE
#define LCD_1B_CL		LCD_COM1,1<<6,LCD_SEG_NONE
#define LCD_1C_CL		LCD_COM2,1<<6,LCD_SEG_NONE
#define LCD_1D_CL		LCD_COM3,1<<6,LCD_SEG_NONE
#define LCD_1E_CL		LCD_COM2,1<<8,LCD_SEG_NONE
#define LCD_1F_CL		LCD_COM0,1<<8,LCD_SEG_NONE
#define LCD_1G_CL		LCD_COM1,1<<8,LCD_SEG_NONE
#define LCD_1H_CL		LCD_COM3,1<<8,LCD_SEG_NONE


#define LCD_2A			LCD_COM0,LCD_MASK_ALL,1<<4
#define LCD_2B			LCD_COM2,LCD_MASK_ALL,1<<4
#define LCD_2C			LCD_COM3,LCD_MASK_ALL,1<<4
#define LCD_2D			LCD_COM3,LCD_MASK_ALL,1<<0
#define LCD_2E			LCD_COM2,LCD_MASK_ALL,1<<5
#define LCD_2F			LCD_COM0,LCD_MASK_ALL,1<<5
#define LCD_2G			LCD_COM1,LCD_MASK_ALL,1<<5
#define LCD_2H			LCD_COM3,LCD_MASK_ALL,1<<5
#define LCD_2R			LCD_COM1,LCD_MASK_ALL,1<<4

#define LCD_2A_CL		LCD_COM0,1<<4,LCD_SEG_NONE
#define LCD_2B_CL		LCD_COM2,1<<4,LCD_SEG_NONE
#define LCD_2C_CL		LCD_COM3,1<<4,LCD_SEG_NONE
#define LCD_2D_CL		LCD_COM3,1<<0,LCD_SEG_NONE
#define LCD_2E_CL		LCD_COM2,1<<5,LCD_SEG_NONE
#define LCD_2F_CL		LCD_COM0,1<<5,LCD_SEG_NONE
#define LCD_2G_CL		LCD_COM1,1<<5,LCD_SEG_NONE
#define LCD_2H_CL		LCD_COM3,1<<5,LCD_SEG_NONE
#define LCD_2R_CL		LCD_COM1,1<<4,LCD_SEG_NONE


#define LCD_PULSE		LCD_COM0,LCD_MASK_ALL,1<<9
#define LCD_VIBRAT		LCD_COM1,LCD_MASK_ALL,1<<9
#define LCD_NO_SHOCK	LCD_COM2,LCD_MASK_ALL,1<<9
#define LCD_DOG_ICO		LCD_COM3,LCD_MASK_ALL,1<<9

#define LCD_PULSE_CL		LCD_COM0,1<<9,LCD_SEG_NONE
#define LCD_VIBRAT_CL		LCD_COM1,1<<9,LCD_SEG_NONE
#define LCD_NO_SHOCK_CL		LCD_COM2,1<<9,LCD_SEG_NONE
#define LCD_DOG_ICO_CL		LCD_COM3,1<<9,LCD_SEG_NONE

#define LCD_PULSE		LCD_COM0,LCD_MASK_ALL,1<<9
#define LCD_VIBRAT		LCD_COM1,LCD_MASK_ALL,1<<9
#define LCD_NO_SHOCK	LCD_COM2,LCD_MASK_ALL,1<<9
#define LCD_DOG_ICO		LCD_COM3,LCD_MASK_ALL,1<<9

#if(VERSION_WITH_RING == 1)
#define LCD_RING_SMALL_R_CL		LCD_COM3,1<<7,LCD_SEG_NONE
#define LCD_RING_FUNC_1_CL		LCD_COM2,1<<7,LCD_SEG_NONE
#define LCD_RING_FUNC_2_CL		LCD_COM1,1<<7,LCD_SEG_NONE
#define LCD_RING_FUNC_3_CL		LCD_COM0,1<<7,LCD_SEG_NONE

#define LCD_RING_SMALL_R		LCD_COM3,LCD_MASK_ALL,1<<7
#define LCD_RING_FUNC_1			LCD_COM2,LCD_MASK_ALL,1<<7
#define LCD_RING_FUNC_2			LCD_COM1,LCD_MASK_ALL,1<<7
#define LCD_RING_FUNC_3			LCD_COM0,LCD_MASK_ALL,1<<7

#endif

#define LCD_ALL_FRAMES		LCD_COM1,LCD_MASK_ALL,1<<3


#define NO_1_CLEAR 	LCD_Write(LCD_1A_CL);\
					LCD_Write(LCD_1B_CL);\
					LCD_Write(LCD_1C_CL);\
					LCD_Write(LCD_1D_CL);\
					LCD_Write(LCD_1E_CL);\
					LCD_Write(LCD_1F_CL);\
					LCD_Write(LCD_1G_CL);\
					LCD_Write(LCD_1H_CL)\

#define NO_2_CLEAR 	LCD_Write(LCD_2A_CL);\
					LCD_Write(LCD_2B_CL);\
					LCD_Write(LCD_2C_CL);\
					LCD_Write(LCD_2D_CL);\
					LCD_Write(LCD_2E_CL);\
					LCD_Write(LCD_2F_CL);\
					LCD_Write(LCD_2G_CL);\
					LCD_Write(LCD_2R_CL);\
					LCD_Write(LCD_2H_CL)\


#define NO_1_1 	LCD_Write(LCD_1B);\
				LCD_Write(LCD_1C)\

#define NO_1_2 	LCD_Write(LCD_1A);\
				LCD_Write(LCD_1B);\
				LCD_Write(LCD_1G);\
				LCD_Write(LCD_1D);\
				LCD_Write(LCD_1E)

#define NO_1_3 	LCD_Write(LCD_1A);\
				LCD_Write(LCD_1B);\
				LCD_Write(LCD_1C);\
				LCD_Write(LCD_1D);\
				LCD_Write(LCD_1G)

#define NO_1_4 	LCD_Write(LCD_1F);\
				LCD_Write(LCD_1B);\
				LCD_Write(LCD_1G);\
				LCD_Write(LCD_1C)\

#define NO_1_5 	LCD_Write(LCD_1A);\
				LCD_Write(LCD_1F);\
				LCD_Write(LCD_1G);\
				LCD_Write(LCD_1D);\
				LCD_Write(LCD_1C)\

#define NO_1_6 	LCD_Write(LCD_1A);\
				LCD_Write(LCD_1F);\
				LCD_Write(LCD_1E);\
				LCD_Write(LCD_1D);\
				LCD_Write(LCD_1C);\
				LCD_Write(LCD_1G)\

#define NO_1_7 	LCD_Write(LCD_1A);\
				LCD_Write(LCD_1B);\
				LCD_Write(LCD_1C)\

#define NO_1_8 	LCD_Write(LCD_1A);\
				LCD_Write(LCD_1B);\
				LCD_Write(LCD_1C);\
				LCD_Write(LCD_1D);\
				LCD_Write(LCD_1E);\
				LCD_Write(LCD_1F);\
				LCD_Write(LCD_1G)\


#define NO_1_9 	LCD_Write(LCD_1A);\
				LCD_Write(LCD_1B);\
				LCD_Write(LCD_1C);\
				LCD_Write(LCD_1D);\
				LCD_Write(LCD_1F);\
				LCD_Write(LCD_1G)\

#define NO_1_0 	LCD_Write(LCD_1A);\
				LCD_Write(LCD_1B);\
				LCD_Write(LCD_1C);\
				LCD_Write(LCD_1D);\
				LCD_Write(LCD_1E);\
				LCD_Write(LCD_1F)\


#define NO_1_EMPTY 	LCD_Write(LCD_1G)

#define NO_1_R 	LCD_Write(LCD_1A);\
				LCD_Write(LCD_1B);\
				LCD_Write(LCD_1E);\
				LCD_Write(LCD_1F);\
				LCD_Write(LCD_1G);\
				LCD_Write(LCD_1H)\


#define NO_2_1 	LCD_Write(LCD_2B);\
				LCD_Write(LCD_2C)\

#define NO_2_2 	LCD_Write(LCD_2A);\
				LCD_Write(LCD_2B);\
				LCD_Write(LCD_2G);\
				LCD_Write(LCD_2D);\
				LCD_Write(LCD_2E)

#define NO_2_3 	LCD_Write(LCD_2A);\
				LCD_Write(LCD_2B);\
				LCD_Write(LCD_2C);\
				LCD_Write(LCD_2D);\
				LCD_Write(LCD_2G)

#define NO_2_4 	LCD_Write(LCD_2F);\
				LCD_Write(LCD_2B);\
				LCD_Write(LCD_2G);\
				LCD_Write(LCD_2C)\

#define NO_2_5 	LCD_Write(LCD_2A);\
				LCD_Write(LCD_2F);\
				LCD_Write(LCD_2G);\
				LCD_Write(LCD_2D);\
				LCD_Write(LCD_2C)\

#define NO_2_6 	LCD_Write(LCD_2A);\
				LCD_Write(LCD_2F);\
				LCD_Write(LCD_2E);\
				LCD_Write(LCD_2D);\
				LCD_Write(LCD_2C);\
				LCD_Write(LCD_2G)\

#define NO_2_7 	LCD_Write(LCD_2A);\
				LCD_Write(LCD_2B);\
				LCD_Write(LCD_2C)\

#define NO_2_8 	LCD_Write(LCD_2A);\
				LCD_Write(LCD_2B);\
				LCD_Write(LCD_2C);\
				LCD_Write(LCD_2D);\
				LCD_Write(LCD_2E);\
				LCD_Write(LCD_2F);\
				LCD_Write(LCD_2G)\


#define NO_2_9 	LCD_Write(LCD_2A);\
				LCD_Write(LCD_2B);\
				LCD_Write(LCD_2C);\
				LCD_Write(LCD_2D);\
				LCD_Write(LCD_2F);\
				LCD_Write(LCD_2G)\

#define NO_2_V 	LCD_Write(LCD_2B);\
				LCD_Write(LCD_2C);\
				LCD_Write(LCD_2H);\
				LCD_Write(LCD_2F)\

#define NO_2_OK	LCD_Write(LCD_2H);\
				LCD_Write(LCD_2R);\
				LCD_Write(LCD_2E);\
				LCD_Write(LCD_2F)\

#define NO_2_P 	LCD_Write(LCD_2A);\
				LCD_Write(LCD_2B);\
				LCD_Write(LCD_2E);\
				LCD_Write(LCD_2F);\
				LCD_Write(LCD_2G)\

#define NO_2_EMPTY 	LCD_Write(LCD_2G)

#define NO_2_0 	LCD_Write(LCD_2A);\
				LCD_Write(LCD_2B);\
				LCD_Write(LCD_2C);\
				LCD_Write(LCD_2D);\
				LCD_Write(LCD_2E);\
				LCD_Write(LCD_2F)\

#endif



//
//
//
//
//#define LCD_DOG_4		LCD_COM2,LCD_MASK_ALL,1<<1
//
//
///* COM 0 Area */
//#define SEG_C0_ID_B				(1<<0)
//#define SEG_C0_BATT_LOW			(1<<1)
//#define SEG_C0_BATT_MED			(1<<2)
//#define SEG_C0_2D				(1<<3)
//#define SEG_C0_2H				(1<<4)
//#define SEG_C0_1D				(1<<5)
//#define SEG_C0_ID_A				(1<<6)
//#define SEG_C0_DOG_ICON			(1<<7)
//
//
/////* COM 1 Area */
//#define SEG_C1_ID_C				(1<<0)
//#define SEG_C1_KEY				(1<<1)
//#define SEG_C1_BATT_HIGH		(1<<2)
//#define SEG_C1_2C				(1<<3)
//#define SEG_C1_2E				(1<<4)
//#define SEG_C1_1C				(1<<5)
//#define SEG_C1_1E				(1<<6)
//#define SEG_C1_NO_SHOCK			(1<<7)
//
//
/////* COM 2 Area */
//#define SEG_C2_ID_D				(1<<0)
//#define SEG_C2_MODE_C			(1<<1)
//#define SEG_C2_BATT_FRAME		(1<<2)
//#define SEG_C2_2B				(1<<3)
//#define SEG_C2_2G				(1<<4)
//#define SEG_C2_1B				(1<<5)
//#define SEG_C2_1G				(1<<6)
//#define SEG_C2_VIBRATION		(1<<7)
//
//
/////* COM 3 Area */
//#define SEG_C3_MODE_ICON		(1<<0)
//#define SEG_C3_MODE_N			(1<<1)
//#define SEG_C3_SIGNAL			(1<<2)
//#define SEG_C3_2A				(1<<3)
//#define SEG_C3_2F				(1<<4)
//#define SEG_C3_1A				(1<<5)
//#define SEG_C3_1F				(1<<6)
//#define SEG_C3_SHOCK			(1<<7)

void LCD_DrawMainScreen(tSharedDataLCD_Core *info);
void LCD_SetBConScreen(tSharedDataLCD_Core *info);
void LCD_SetANickScreen(tSharedDataLCD_Core *info);
void LCD_SetBNickScreen(tSharedDataLCD_Core *info);
void LCD_ShowDogID(eSelectedDog dog);
void LCD_ShowDogID_Train(eSelectedDog dog);
void LCD_ShowModeNC(eMode mode);
void LCD_ShowNumber(uint8_t number,bool booster,bool ascendingMode);
void LCD_ShowLockShock(bool showIt);
void LCD_ShowShockOrVibration(bool shock);
void LCD_ClearLockShock(bool refresh);
void LCD_ClearLockShockEdit(bool lock);
void LCD_ShowLockShockEdit(bool lock);
void LCD_Show_1_0(void);
void LCD_Show_1_1(void);
void LCD_Show_1_2(void);
void LCD_Show_1_3(void);
void LCD_Show_1_4(void);
void LCD_Show_1_5(void);
void LCD_Show_1_6(void);
void LCD_Show_1_7(void);
void LCD_Show_1_8(void);
void LCD_Show_1_9(void);
void LCD_Show_1_EMPTY(void);
void LCD_Show_2_0(void);
void LCD_Show_2_V(void);
#if ASCENDING_MODE_ENABLED == true
void LCD_Show_2_P(void);
#endif
void LCD_Show_2_K(void);
void LCD_Show_2_EMPTY(void);
void LCD_Show_2_1(void);
void LCD_Show_2_2(void);
void LCD_Show_2_3(void);
void LCD_Show_2_4(void);
void LCD_Show_2_5(void);
void LCD_Show_2_6(void);
void LCD_Show_2_7(void);
void LCD_Show_2_8(void);
void LCD_Show_2_9(void);
void LCD_ShowBattLevels(eBatteryStatus batt);
void LCD_ShowAllFrames(void);
void LCD_ShowTxActive(void);
void LCD_ClearTxActive(bool refresh);
void LCD_ShowModeIcon(void);
void LCD_ClearModeIcon(bool refresh);
void LCD_ClearBattLevels(bool refresh);
void LCD_Clear_1_All(bool refresh);
void LCD_Clear_2_All(bool refresh);
void LCD_ClearDogsID(bool refresh);
void LCD_ClearMode(bool refresh);
void LCD_ClearShockOrVibration(bool refresh);
void LCD_TurnOffAllSegments(void);
void LCD_TurnOnAllSegments(void);

#if(VERSION_WITH_RING ==1)
void LCD_Show_RingState(bool state, bool refresh);
void LCD_ShowRingBtnFunction(eSelectedBtnForRing selectedBtn,bool deletePrev, bool refresh);
void LCD_Clear_Btn1Function(bool refresh);
void LCD_Clear_Btn2Function(bool refresh);
void LCD_Clear_Btn3Function(bool refresh);
void LCD_Show_1_R(void);
void LCD_DrawPairingScreen(tSharedDataLCD_Core *info);
#endif
#endif /* INC_DISPLAYTASK_LCD_LOWLEVEL_H_ */
