/*
 * Si446x_hal.h
 *
 *  Created on: 22. 6. 2021
 *      Author: jirik
 */

#ifndef INC_RADIOSI446X_SI446X_HAL_H_
#define INC_RADIOSI446X_SI446X_HAL_H_


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
/* Definition inline global function	                               */
/************************************************************************/

/************************************************************************/
/* Declaration global function											*/
/************************************************************************/

// Start send packet
void SendPacket(tSettingSI446xCommunication* SI446xComm, uint8_t SerialNumber, uint16_t Value1, uint16_t Value2, uint16_t Value3);

// Repeat send packet
void RepeatSendPacket(tSettingSI446xCommunication* SI446xComm, uint16_t* Counter);

// Stop send packet
void StopSendPacket(tSettingSI446xCommunication* SI446xComm, uint16_t* Counter);

#endif /* INC_RADIOSI446X_SI446X_HAL_H_ */
