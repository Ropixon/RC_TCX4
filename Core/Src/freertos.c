/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "TaskCore.h"
#include "TaskDisplay.h"
#include "TaskRF.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId TaskCoreHandle;
uint32_t TaskCoreBuffer[ 300 ];
osStaticThreadDef_t TaskCoreControlBlock;
osThreadId TaskRFHandle;
uint32_t TaskRFBuffer[ 300 ];
osStaticThreadDef_t TaskRFControlBlock;
osThreadId TaskDisplayHandle;
uint32_t TaskDisplayBuffer[ 300 ];
osStaticThreadDef_t TaskDisplayControlBlock;
osMessageQId QueueCoreHandle;
uint8_t QueueCoreBuffer[ 20 * sizeof( tDataQueue ) ];
osStaticMessageQDef_t QueueCoreControlBlock;
osMessageQId QueueRFHandle;
uint8_t QueueRFBuffer[ 20 * sizeof( tDataQueue ) ];
osStaticMessageQDef_t QueueRFControlBlock;
osMessageQId QueueDisplayHandle;
uint8_t QueueDisplayBuffer[ 20 * sizeof( tDataQueue ) ];
osStaticMessageQDef_t QueueDisplayControlBlock;
osTimerId TimerLCDHandle;
osStaticTimerDef_t TimerLCDControlBlock;
osTimerId TimerBacklightHandle;
osStaticTimerDef_t TimerBacklightControlBlock;
osTimerId TimerStopTXHandle;
osStaticTimerDef_t TimerStopTXControlBlock;
osTimerId TimerMeasureBattHandle;
osStaticTimerDef_t TimerMeasureBattControlBlock;
osTimerId TimerLockShockHandle;
osStaticTimerDef_t TimerLockShockControlBlock;
osTimerId TimerBeepHandle;
osStaticTimerDef_t TimerBeepControlBlock;
osTimerId TimerVibratDuringShockHandle;
osStaticTimerDef_t TimerVibratDuringShockControlBlock;
osTimerId TimerChargerHandle;
osStaticTimerDef_t TimerChargerControlBlock;
osTimerId TimerChargAnimationHandle;
osStaticTimerDef_t TimerChargAnimationControlBlock;
osTimerId TimerFactoryTestHandle;
osStaticTimerDef_t TimerFactoryTestControlBlock;
osTimerId TimerUartCheckHandle;
osStaticTimerDef_t myTimer11ControlBlock;
osTimerId TimerRingHeartBeatHandle;
osStaticTimerDef_t TimerRingHeartBeatControlBlock;
osTimerId TimerStopTxFromRingHandle;
osStaticTimerDef_t TimerStopTxFromRingControlBlock;
osTimerId TimerUnpairringHandle;
osStaticTimerDef_t TimerUnpairringControlBlock;
osTimerId Timer_AscendingModeHandle;
osStaticTimerDef_t Timer_AscendingModeControlBlock;
osTimerId timerChargingCheckHandle;
osStaticTimerDef_t timerChargingCheckControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTaskCore(void const * argument);
void StartTaskRF(void const * argument);
void StartTaskDisplay(void const * argument);
void CallbackLCD(void const * argument);
void CallbackBacklight(void const * argument);
void CallbackStopTX(void const * argument);
void CallbackStartMeasureBatt(void const * argument);
void CallbackLockShock(void const * argument);
void CallbackBeepEnd(void const * argument);
void CallbackVibrationDuringShock(void const * argument);
void CallbackChargerTimeout(void const * argument);
void CallbackChargeAnimation(void const * argument);
void CallbackFactoryTest(void const * argument);
void CallbackUartCheck(void const * argument);
void CallbackRingHB(void const * argument);
void CallbackStopTxFromRing(void const * argument);
void CallbackUnpairRing(void const * argument);
void CallbackAscendingMode(void const * argument);
void CallbackChargerCheck(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
__weak void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

/* USER CODE BEGIN PREPOSTSLEEP */
__weak void PreSleepProcessing(uint32_t *ulExpectedIdleTime)
{
/* place for user code */
}

__weak void PostSleepProcessing(uint32_t *ulExpectedIdleTime)
{
/* place for user code */
}
/* USER CODE END PREPOSTSLEEP */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of TimerLCD */
  osTimerStaticDef(TimerLCD, CallbackLCD, &TimerLCDControlBlock);
  TimerLCDHandle = osTimerCreate(osTimer(TimerLCD), osTimerPeriodic, NULL);

  /* definition and creation of TimerBacklight */
  osTimerStaticDef(TimerBacklight, CallbackBacklight, &TimerBacklightControlBlock);
  TimerBacklightHandle = osTimerCreate(osTimer(TimerBacklight), osTimerOnce, NULL);

  /* definition and creation of TimerStopTX */
  osTimerStaticDef(TimerStopTX, CallbackStopTX, &TimerStopTXControlBlock);
  TimerStopTXHandle = osTimerCreate(osTimer(TimerStopTX), osTimerOnce, NULL);

  /* definition and creation of TimerMeasureBatt */
  osTimerStaticDef(TimerMeasureBatt, CallbackStartMeasureBatt, &TimerMeasureBattControlBlock);
  TimerMeasureBattHandle = osTimerCreate(osTimer(TimerMeasureBatt), osTimerOnce, NULL);

  /* definition and creation of TimerLockShock */
  osTimerStaticDef(TimerLockShock, CallbackLockShock, &TimerLockShockControlBlock);
  TimerLockShockHandle = osTimerCreate(osTimer(TimerLockShock), osTimerOnce, NULL);

  /* definition and creation of TimerBeep */
  osTimerStaticDef(TimerBeep, CallbackBeepEnd, &TimerBeepControlBlock);
  TimerBeepHandle = osTimerCreate(osTimer(TimerBeep), osTimerOnce, NULL);

  /* definition and creation of TimerVibratDuringShock */
  osTimerStaticDef(TimerVibratDuringShock, CallbackVibrationDuringShock, &TimerVibratDuringShockControlBlock);
  TimerVibratDuringShockHandle = osTimerCreate(osTimer(TimerVibratDuringShock), osTimerOnce, NULL);

  /* definition and creation of TimerCharger */
  osTimerStaticDef(TimerCharger, CallbackChargerTimeout, &TimerChargerControlBlock);
  TimerChargerHandle = osTimerCreate(osTimer(TimerCharger), osTimerOnce, NULL);

  /* definition and creation of TimerChargAnimation */
  osTimerStaticDef(TimerChargAnimation, CallbackChargeAnimation, &TimerChargAnimationControlBlock);
  TimerChargAnimationHandle = osTimerCreate(osTimer(TimerChargAnimation), osTimerPeriodic, NULL);

  /* definition and creation of TimerFactoryTest */
  osTimerStaticDef(TimerFactoryTest, CallbackFactoryTest, &TimerFactoryTestControlBlock);
  TimerFactoryTestHandle = osTimerCreate(osTimer(TimerFactoryTest), osTimerOnce, NULL);

  /* definition and creation of TimerUartCheck */
  osTimerStaticDef(TimerUartCheck, CallbackUartCheck, &myTimer11ControlBlock);
  TimerUartCheckHandle = osTimerCreate(osTimer(TimerUartCheck), osTimerOnce, NULL);

  /* definition and creation of TimerRingHeartBeat */
  osTimerStaticDef(TimerRingHeartBeat, CallbackRingHB, &TimerRingHeartBeatControlBlock);
  TimerRingHeartBeatHandle = osTimerCreate(osTimer(TimerRingHeartBeat), osTimerOnce, NULL);

  /* definition and creation of TimerStopTxFromRing */
  osTimerStaticDef(TimerStopTxFromRing, CallbackStopTxFromRing, &TimerStopTxFromRingControlBlock);
  TimerStopTxFromRingHandle = osTimerCreate(osTimer(TimerStopTxFromRing), osTimerOnce, NULL);

  /* definition and creation of TimerUnpairring */
  osTimerStaticDef(TimerUnpairring, CallbackUnpairRing, &TimerUnpairringControlBlock);
  TimerUnpairringHandle = osTimerCreate(osTimer(TimerUnpairring), osTimerOnce, NULL);

  /* definition and creation of Timer_AscendingMode */
  osTimerStaticDef(Timer_AscendingMode, CallbackAscendingMode, &Timer_AscendingModeControlBlock);
  Timer_AscendingModeHandle = osTimerCreate(osTimer(Timer_AscendingMode), osTimerOnce, NULL);

  /* definition and creation of timerChargingCheck */
  osTimerStaticDef(timerChargingCheck, CallbackChargerCheck, &timerChargingCheckControlBlock);
  timerChargingCheckHandle = osTimerCreate(osTimer(timerChargingCheck), osTimerOnce, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of QueueCore */
  osMessageQStaticDef(QueueCore, 20, tDataQueue, QueueCoreBuffer, &QueueCoreControlBlock);
  QueueCoreHandle = osMessageCreate(osMessageQ(QueueCore), NULL);

  /* definition and creation of QueueRF */
  osMessageQStaticDef(QueueRF, 20, tDataQueue, QueueRFBuffer, &QueueRFControlBlock);
  QueueRFHandle = osMessageCreate(osMessageQ(QueueRF), NULL);

  /* definition and creation of QueueDisplay */
  osMessageQStaticDef(QueueDisplay, 20, tDataQueue, QueueDisplayBuffer, &QueueDisplayControlBlock);
  QueueDisplayHandle = osMessageCreate(osMessageQ(QueueDisplay), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of TaskCore */
  osThreadStaticDef(TaskCore, StartTaskCore, osPriorityNormal, 0, 300, TaskCoreBuffer, &TaskCoreControlBlock);
  TaskCoreHandle = osThreadCreate(osThread(TaskCore), NULL);

  /* definition and creation of TaskRF */
  osThreadStaticDef(TaskRF, StartTaskRF, osPriorityNormal, 0, 300, TaskRFBuffer, &TaskRFControlBlock);
  TaskRFHandle = osThreadCreate(osThread(TaskRF), NULL);

  /* definition and creation of TaskDisplay */
  osThreadStaticDef(TaskDisplay, StartTaskDisplay, osPriorityNormal, 0, 300, TaskDisplayBuffer, &TaskDisplayControlBlock);
  TaskDisplayHandle = osThreadCreate(osThread(TaskDisplay), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartTaskCore */
/**
  * @brief  Function implementing the TaskCore thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTaskCore */
void StartTaskCore(void const * argument)
{
  /* USER CODE BEGIN StartTaskCore */
	TaskCore(argument);
  /* USER CODE END StartTaskCore */
}

/* USER CODE BEGIN Header_StartTaskRF */
/**
* @brief Function implementing the TaskRF thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskRF */
void StartTaskRF(void const * argument)
{
  /* USER CODE BEGIN StartTaskRF */
	TaskRF(argument);
  /* USER CODE END StartTaskRF */
}

/* USER CODE BEGIN Header_StartTaskDisplay */
/**
* @brief Function implementing the TaskDisplay thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskDisplay */
void StartTaskDisplay(void const * argument)
{
  /* USER CODE BEGIN StartTaskDisplay */
	TaskDisplay(argument);
  /* USER CODE END StartTaskDisplay */
}

/* CallbackLCD function */
void CallbackLCD(void const * argument)
{
  /* USER CODE BEGIN CallbackLCD */
	BlinkingSegment(false);
  /* USER CODE END CallbackLCD */
}

/* CallbackBacklight function */
__weak void CallbackBacklight(void const * argument)
{
  /* USER CODE BEGIN CallbackBacklight */

  /* USER CODE END CallbackBacklight */
}

/* CallbackStopTX function */
__weak void CallbackStopTX(void const * argument)
{
  /* USER CODE BEGIN CallbackStopTX */

  /* USER CODE END CallbackStopTX */
}

/* CallbackStartMeasureBatt function */
__weak void CallbackStartMeasureBatt(void const * argument)
{
  /* USER CODE BEGIN CallbackStartMeasureBatt */

  /* USER CODE END CallbackStartMeasureBatt */
}

/* CallbackLockShock function */
__weak void CallbackLockShock(void const * argument)
{
  /* USER CODE BEGIN CallbackLockShock */

  /* USER CODE END CallbackLockShock */
}

/* CallbackBeepEnd function */
__weak void CallbackBeepEnd(void const * argument)
{
  /* USER CODE BEGIN CallbackBeepEnd */

  /* USER CODE END CallbackBeepEnd */
}

/* CallbackVibrationDuringShock function */
__weak void CallbackVibrationDuringShock(void const * argument)
{
  /* USER CODE BEGIN CallbackVibrationDuringShock */

  /* USER CODE END CallbackVibrationDuringShock */
}

/* CallbackChargerTimeout function */
__weak void CallbackChargerTimeout(void const * argument)
{
  /* USER CODE BEGIN CallbackChargerTimeout */

  /* USER CODE END CallbackChargerTimeout */
}

/* CallbackChargeAnimation function */
__weak void CallbackChargeAnimation(void const * argument)
{
  /* USER CODE BEGIN CallbackChargeAnimation */

  /* USER CODE END CallbackChargeAnimation */
}

/* CallbackFactoryTest function */
__weak void CallbackFactoryTest(void const * argument)
{
  /* USER CODE BEGIN CallbackFactoryTest */

  /* USER CODE END CallbackFactoryTest */
}

/* CallbackUartCheck function */
__weak void CallbackUartCheck(void const * argument)
{
  /* USER CODE BEGIN CallbackUartCheck */

  /* USER CODE END CallbackUartCheck */
}

/* CallbackRingHB function */
__weak void CallbackRingHB(void const * argument)
{
  /* USER CODE BEGIN CallbackRingHB */

  /* USER CODE END CallbackRingHB */
}

/* CallbackStopTxFromRing function */
__weak void CallbackStopTxFromRing(void const * argument)
{
  /* USER CODE BEGIN CallbackStopTxFromRing */

  /* USER CODE END CallbackStopTxFromRing */
}

/* CallbackUnpairRing function */
__weak void CallbackUnpairRing(void const * argument)
{
  /* USER CODE BEGIN CallbackUnpairRing */

  /* USER CODE END CallbackUnpairRing */
}

/* CallbackAscendingMode function */
__weak void CallbackAscendingMode(void const * argument)
{
  /* USER CODE BEGIN CallbackAscendingMode */

  /* USER CODE END CallbackAscendingMode */
}

/* CallbackChargerCheck function */
__weak void CallbackChargerCheck(void const * argument)
{
  /* USER CODE BEGIN CallbackChargerCheck */

  /* USER CODE END CallbackChargerCheck */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

