/*
 * TaskDisplay.h
 *
 *  Created on: 25. 3. 2021
 *      Author: jirik
 */

#ifndef INC_FREERTOS_TASKS_TASKDISPLAY_H_
#define INC_FREERTOS_TASKS_TASKDISPLAY_H_


/************************************************************************/
/* Area #DEFINE															*/
/************************************************************************/
#define BLINKING_LCD_PERIOD			(400)
/************************************************************************/
/* MAKRO with parameters                                                */
/************************************************************************/

/************************************************************************/
/* Area TYPEDEF                                                         */
/************************************************************************/
/* No. of task states*/
/* No. of task states*/
typedef enum
{
	STATE_LCD_INIT=0,
	STATE_LCD_OFF,
	STATE_LCD_START_ON,
	STATE_LCD_ON,
	STATE_LCD_START_OFF,
} EnumStateCodesDisplay_t;

/* task state struct*/
typedef struct
{
	EnumStateCodesDisplay_t	actualState;
	EnumStateCodesDisplay_t	previousState;
} StructstateAutomatDisplay_t;

/************************************************************************/
/* Declaration global variables - EXTERN                                */
/************************************************************************/

/************************************************************************/
/* Definition inline global function	                               */
/************************************************************************/

/************************************************************************/
/* Declaration global function											*/
/************************************************************************/

void TaskDisplay(void const * argument);
void BlinkingSegment(bool rstDiv1);
void LCDRefreschWatchdog(void);

#endif /* INC_FREERTOS_TASKS_TASKDISPLAY_H_ */
