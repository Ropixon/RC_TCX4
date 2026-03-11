/*
 * FactoryTest.h
 *
 *  Created on: Feb 25, 2022
 *      Author: developer
 */

#ifndef SRC_FACTORYTEST_FACTORYTEST_H_
#define SRC_FACTORYTEST_FACTORYTEST_H_

#include "main.h"

typedef enum
{
	FT_CMD_NONE=0,          //!< FT_CMD_NONE
	FT_CMD_START_CW=1,        //!< FT_CMD_START_CW
	FT_CMD_STOP_CW=2,         //!< FT_CMD_STOP_CW
	FT_CMD_SET_FREQ =3,
	FT_CMD_RESET_MCU=4,
	FT_CMD_DEV_OFF=5,	// vypnuti monitoru
	FT_CMD_DEV_ON=6,	// zapnuti monitoru
	FT_CMD_TEST_LCD = 7,
	FT_CMD_WRITE_SERIAL_ID	= 8,
	FT_CMD_SET_DOGS_CNT	=	9,
	FT_CMD_SET_DEFAULT_VALS	=	10,
	FT_CMD_LOCK_MCU	=	11,

}eFactoryTestCmd;


/**
 *
 */
typedef struct
{
	bool				newRxPendingMsg;
	eFactoryTestCmd		factoryCmd;
	uint32_t			temp32;
	bool				testCanStart;
	uint8_t				lcdCOM;
	uint32_t			lcdSEG;

} __attribute__((packed, aligned(4))) tFactoryTest;

void ft_IRQ(void);
void ft_InitTest(void);
void ft_stopTest(void);
void ft_DecodeRxMsg(void);
#endif /* SRC_FACTORYTEST_FACTORYTEST_H_ */
