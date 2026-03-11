/*
 * RadioInterface.h
 *
 *  Created on: Jun 29, 2021
 *      Author: developer
 */

#ifndef SRC_RF_TASK_RADIOINTERFACE_H_
#define SRC_RF_TASK_RADIOINTERFACE_H_


void RI_InitRadio(void);
void RI_SetSleep(void);
void RI_SetReady(void);
void RI_SendPayload(uint8_t *payload, uint8_t size, bool nick,bool willChangedata, tGlobalDataRF * glData);
void RI_CreateRAWPacket(uint8_t *packet, uint8_t size);
void RI_WaitForCTS(void);
void RI_ClearPendingIRQ(void);
void RI_StopTXandDiscard(tGlobalDataRF *glData);
void RI_StartTxCW(uint8_t power);
void RI_SetRfFreq(uint32_t freq);
void RI_FillTXFIFO(uint8_t *payload, uint8_t size);

#endif /* SRC_RF_TASK_RADIOINTERFACE_H_ */
