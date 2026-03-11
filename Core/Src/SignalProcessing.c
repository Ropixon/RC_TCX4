/*
 * SignalProcessing.c
 *
 *  Created on: 30. 3. 2020
 *      Author: developer
 */


#include "main.h"
#include "SignalProcessing.h"

#define LOW_WORD(x)  ((uint32_t)(x) & 0xffff)
#define HIGH_WORD(x) ((uint32_t)(x) >> 16)
#define ABS(x) (((x) >= 0) ? (x) : -(x))
#define SIGN(x) (((x) >= 0) ? 1 : -1)

#define UNSIGNED_MULT(a, b) \
    (((LOW_WORD(a)  * LOW_WORD(b))  <<  0) + \
     ((int64_t)(LOW_WORD(a) * HIGH_WORD(b) + HIGH_WORD(a) * LOW_WORD(b)) << 16) + \
     ((int64_t)(HIGH_WORD((a)) * HIGH_WORD((b))) << 32))

#define MULT(a, b)  (UNSIGNED_MULT(ABS((a)), ABS((b))) * SIGN((a)) * SIGN((b)))

/**
 *
 * @param value
 * @param min
 * @param max
 * @return
 */
double	SP_ConstrainDouble(double value, double min, double max)
{
	if(value>max)		return max;
	else if(value<min)	return min;

	return value;
}

/**
 *
 * @param value
 * @param min
 * @param max
 * @return
 */
uint8_t	SP_ConstrainU8(uint8_t value, uint8_t min, uint8_t max)
{
	if(value>max)		return max;
	else if(value<min)	return min;

	return value;
}


/**
 *
 * @param value
 * @param min
 * @param max
 * @return
 */
int	SP_ConstrainINT(int value, int min, int max)
{
	if(value>max)		return max;
	else if(value<min)	return min;

	return value;
}

/**
 *
 * @param value
 * @param min
 * @param max
 * @return
 */
uint32_t	SP_ConstrainU32(uint32_t value, uint32_t min, uint32_t max)
{
	if(value>max)		return max;
	else if(value<min)	return min;

	return value;
}


/**
 *
 */
uint32_t SP_CalculateRFFrac(uint32_t freq)
{
	float temp_64;
	uint32_t delta,temp_32;

	delta = (freq-840000);

	temp_64 = (uint64_t) MULT(524288,delta);
	temp_32 = (uint32_t) (temp_64/15000);

	return (temp_32);
}


/**
 *
 */
uint32_t SP_CalculateRfFreq(uint32_t frac)
{
	double 	 tempD;

	tempD =(double) frac/524288;
	tempD = (double) (56+tempD);
	tempD *= 15000;
	tempD = round(tempD);
	return (uint32_t)tempD*1000;
}
