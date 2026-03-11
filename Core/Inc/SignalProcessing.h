/*
 * SignalProcessing.h
 *
 *  Created on: 30. 3. 2020
 *      Author: developer
 */

#ifndef INC_SIGNALPROCESSING_H_
#define INC_SIGNALPROCESSING_H_

#include "main.h"

double		SP_ConstrainDouble					(double value, double min, double max);
uint8_t		SP_ConstrainU8						(uint8_t value, uint8_t min, uint8_t max);
uint32_t	SP_ConstrainU32						(uint32_t value, uint32_t min, uint32_t max);
bool 		SP_IsOUtOfRangeU32					(uint32_t value, uint32_t delta);
int			SP_ConstrainINT						(int value, int min, int max);
uint32_t 	SP_CalculateRFFrac					(uint32_t freq);
uint32_t 	SP_CalculateRfFreq					(uint32_t frac);
#endif /* INC_SIGNALPROCESSING_H_ */
