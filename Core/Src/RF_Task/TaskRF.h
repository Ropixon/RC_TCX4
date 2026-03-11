/*
 * TaskRF.h
 *
 *  Created on: 17.6. 2021
 *      Author: Jan
 */

#ifndef INC_FREERTOS_TASKS_TASKRF_H_
#define INC_FREERTOS_TASKS_TASKRF_H_


/************************************************************************/
/* Area #DEFINE															*/
/************************************************************************/
#define PACKET_SIZE_BYTE	21						//Length telegram
#define FIX_PCKT_LEN		(PACKET_SIZE_BYTE)					//Fixed length of packet
#define NUM_PACKET_IN_FIFO	(5)  // 5×21=105 bytes fits in 129-byte shared FIFO
#define NUM_PACKET_IN_TX_FIFO_ALMOST_EMPTY		(3)
#define PACKET_SIZE_BIT		(PACKET_SIZE_BYTE*8)

#define RF_CENTER_FREQ		(869525000)
#define RF_FREQ_MAX_DELTA	(20000)
#define ALLOWED_RF_FREQ_MAX  (RF_CENTER_FREQ+RF_FREQ_MAX_DELTA)
#define ALLOWED_RF_FREQ_MIN  (RF_CENTER_FREQ-RF_FREQ_MAX_DELTA)

/************************************************************************/
/* MAKRO with parameters                                                */
/************************************************************************/

/************************************************************************/
/* Area TYPEDEF                                                         */
/************************************************************************/

/**
 *
 */
typedef enum
{
	STATE_RF_INIT,     //!< STATE_INIT
	STATE_RF_OFF,      //!< STATE_OFF
	STATE_RF_START_ON, //!< STATE_START_ON
	STATE_RF_ON,       //!< STATE_ON
	STATE_RF_START_OFF,//!< STATE_START_OFF

}eStateRfSystem;


/* task state struct*/
typedef struct
{
	eStateRfSystem	ActualState;
	eStateRfSystem	PreviousState;
} tStateRfSystem;


typedef struct
{
	bool		stopTXAfterTX;
	bool		onePacketDone;
	bool 		inTxState;
	bool		FILL_FIFO;
	uint8_t		payload[40];
	uint8_t		payload_size;
}
tGlobalDataRF;

/************************************************************************/
/* Declaration global variables - EXTERN                                */
/************************************************************************/
extern uint16_t Gl_Packet_RAW[PACKET_SIZE_BIT];
extern uint16_t GlRepeatCounter;
extern uint16_t GLTXDone;
/************************************************************************/
/* Definition inline global function	                               */
/************************************************************************/

/************************************************************************/
/* Declaration global function											*/
/************************************************************************/

void TaskRF(void const * argument);
void RFRefreschWatchdog(void);
void InitDMAForRAWTX(void);
void CallBackRFIRQ(void);


#endif /* INC_FREERTOS_TASKS_TASKRF_H_ */
