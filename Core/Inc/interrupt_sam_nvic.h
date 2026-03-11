/*
 * interrupt_sam_nvic.h
 *
 *  Created on: Mar 24, 2021
 *      Author: jirik
 */

#ifndef INC_INTERRUPT_SAM_NVIC_H_
#define INC_INTERRUPT_SAM_NVIC_H_

#include "cmsis_gcc.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* Area #DEFINE															*/
/************************************************************************/
#  define cpu_irq_enable()                     \
	do {                                       \
		__DMB();                               \
		__enable_irq();                        \
	} while (0)
#  define cpu_irq_disable()                    \
	do {                                       \
		__disable_irq();                       \
		__DMB();                               \
	} while (0)

#define cpu_irq_is_enabled()    (__get_PRIMASK() == 0)

#define Enable_global_interrupt()            cpu_irq_enable()
#define Disable_global_interrupt()           cpu_irq_disable()
#define Is_global_interrupt_enabled()        cpu_irq_is_enabled()

/************************************************************************/
/* MAKRO with parameters                                                */
/************************************************************************/

/************************************************************************/
/* Area TYPEDEF                                                         */
/************************************************************************/
typedef uint32_t irqflags_t;

/************************************************************************/
/* Declaration global variables - EXTERN                                */
/************************************************************************/

/************************************************************************/
/* Definition inline global function	                               */
/************************************************************************/
/**
 *
 */
static inline irqflags_t cpu_irq_save(void)
{
	irqflags_t flags = cpu_irq_is_enabled();
	cpu_irq_disable();
	return flags;
}

/**
 *
 */
static inline bool cpu_irq_is_enabled_flags(irqflags_t flags)
{
	return (flags);
}

/**
 *
 */
static inline void cpu_irq_restore(irqflags_t flags)
{
	if (cpu_irq_is_enabled_flags(flags))
		cpu_irq_enable();
}
/************************************************************************/
/* Declaration global function											*/
/************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* INC_INTERRUPT_SAM_NVIC_H_ */
