/*
 * exti.h
 *
 *  Created on: 26. 3. 2020
 *      Author: Jiøí
 */

#ifndef EXTI_H_
#define EXTI_H_


/************************************************************************/
/* Area #DEFINE															*/
/************************************************************************/

/************************************************************************/
/* MAKRO with parameters                                                */
/************************************************************************/

/************************************************************************/
/* Area TYPEDEF                                                         */
/************************************************************************/

/************************************************************************/
/* Declaration global variables - EXTERN                                */
/************************************************************************/

/************************************************************************/
/* Declaration global function											*/
/************************************************************************/
/*
 * Clear and enable external interrupts 0 - 31
 */
static inline void EXTI_ClearAndEnableIT_0_31(uint32_t ExtiLine)
{
	taskENTER_CRITICAL();
	LL_EXTI_ClearFlag_0_31(ExtiLine);
	LL_EXTI_EnableIT_0_31(ExtiLine);
	taskEXIT_CRITICAL();
}

/*
 * Clear and enable external interrupts 0 - 31 from ISR
 */
static inline void EXTI_ClearAndEnableIT_0_31_ISR(uint32_t ExtiLine)
{
	uint32_t TempValue = taskENTER_CRITICAL_FROM_ISR();
	LL_EXTI_ClearFlag_0_31(ExtiLine);
	LL_EXTI_EnableIT_0_31(ExtiLine);
	taskEXIT_CRITICAL_FROM_ISR(TempValue);
}

/*
 * Enable external interrupts 0 - 31
 */
static inline void EXTI_EnableIT_0_31(uint32_t ExtiLine)
{
	taskENTER_CRITICAL();
	LL_EXTI_EnableIT_0_31(ExtiLine);
	taskEXIT_CRITICAL();
}

/*
 *  Disable external interrupts 0 - 31
 */
static inline void EXTI_DisableIT_0_31(uint32_t ExtiLine)
{
	taskENTER_CRITICAL();
	LL_EXTI_DisableIT_0_31(ExtiLine);
	taskEXIT_CRITICAL();
}

/*
 * Clear and enable external interrupts 0 - 31 from ISR
 */
static inline void EXTI_DisableIT_FromISR_0_31(uint32_t ExtiLine)
{
	uint32_t TempValue = taskENTER_CRITICAL_FROM_ISR();
	LL_EXTI_DisableIT_0_31(ExtiLine);
	taskEXIT_CRITICAL_FROM_ISR(TempValue);
}


/*
 * Activate external interrupts 0 - 31 from ISR
 */
static inline void EXTI_ActivateIT_FromISR_0_31(uint32_t ExtiLine)
{
	uint32_t TempValue = taskENTER_CRITICAL_FROM_ISR();
	LL_EXTI_GenerateSWI_0_31(ExtiLine);
	taskEXIT_CRITICAL_FROM_ISR(TempValue);
}


#endif /* EXTI_H_ */
