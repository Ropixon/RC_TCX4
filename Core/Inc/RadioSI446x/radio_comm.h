/*
 *radio_comm.h
 *
 *  Created on: 04.10.2012
 *      Author: vihuszar, atgosi
 */

#ifndef EZRP_NEXT_API_H_
#define EZRP_NEXT_API_H_

//++++++++++++++++++++++++++++++
// Define capacitor bank value
//++++++++++++++++++++++++++++++
#define CAP_BANK_VALUE    0x4A    // Capacitor bank value for adjusting the XTAL frequency
                                  // Note that it may vary from test card to test card

//+++++++++++++++++++++
//  TestCard selection
//+++++++++++++++++++++

#define EZRP_NEXT_TestCard TestCard_4463_TCE20B915


//++++++++++++++++++++++++
//  TestCard definitions
//++++++++++++++++++++++++

#define TestCard_4463_TSQ20B169  0
#define TestCard_4463_TSQ20D169  1
#define TestCard_4463_TSQ27F169  2
#define TestCard_4461_TCE14D434  3
#define TestCard_4460_TCE10D434  4
#define TestCard_4463_TCE20B460  5
#define TestCard_4463_TCE20C460  6
#define TestCard_4460_TSC10D868  7
#define TestCard_4460_TCE10D868  8
#define TestCard_4461_TSC14D868  9
#define TestCard_4461_TCE16D868 10
#define TestCard_4463_TCE20B868 11
#define TestCard_4463_TCE20C868 12
#define TestCard_4463_TCE27F868 13
#define TestCard_4463_TCE20B915 14
#define TestCard_4463_TCE20C915 15
#define TestCard_4463_TCE30E915 16
#define TestCard_4464_TCE20B420 17

//+++++++++++++++++++++
//  TestCard settings
//+++++++++++++++++++++

#if EZRP_NEXT_TestCard == TestCard_4463_TCE20C460
  #define ONE_SMA_WITH_RF_SWITCH
#elif EZRP_NEXT_TestCard == TestCard_4463_TCE20C868
  #define ONE_SMA_WITH_RF_SWITCH
#elif EZRP_NEXT_TestCard == TestCard_4463_TCE20C915
  #define ONE_SMA_WITH_RF_SWITCH
#endif

//++++++++++++++++++++++++++++++++++++
//  Modulations for Si446x devices
//++++++++++++++++++++++++++++++++++++

#define MOD_CW    0
#define MOD_OOK   1
#define MOD_2FSK  2
#define MOD_2GFSK 3
#define MOD_4FSK  4
#define MOD_4GFSK 5

//++++++++++++++++++++++++++
//  Communication settings
//++++++++++++++++++++++++++

#define MAX_CTS_RETRY					2500
#define NIRQ_TIMEOUT					10000

//++++++++++++++++++++++++++
//  Data type definition
//++++++++++++++++++++++++++
typedef struct
{
	SPI_HandleTypeDef 	*hspi;
	GPIO_TypeDef* 		NIRQ_Port;
	uint16_t 			NIRQ_Pin;
	GPIO_TypeDef* 		NSEL_Port;
	uint16_t 			NSEL_Pin;
	GPIO_TypeDef* 		SDN_Port;
	uint16_t 			SDN_Pin;
}tSettingSI446xCommunication;

typedef enum
{
	SI446X_SUCCESS,
	SI446X_NO_PATCH,
	SI446X_CTS_TIMEOUT,
	SI446X_PATCH_FAIL,
	SI446X_COMMAND_ERROR,
	SI446X_COMMAND_RUNNING,
}COMM_RETURN_VALUE;

//+++++++++++++++++++++
//  Function prototypes
//+++++++++++++++++++++


// API communication functions - communication layer 2
COMM_RETURN_VALUE bApi_SendCommand(size_t bCmdLength, U8 *pbCmdData,bool waitForCTSBefore,bool waitForCTSAfter);											// Send a command + data to the chip
COMM_RETURN_VALUE vApi_WaitforCTS(bool stayNselLow);																			//Wait for CTS
COMM_RETURN_VALUE vApi_WaitforCTSAfterReset(void);
COMM_RETURN_VALUE bApi_GetResponse(size_t bRespLength, U8 *pbRespData);										// Get response from the chip (to be used after a command)
COMM_RETURN_VALUE bApi_CommWriteData(U8 cmd, bool pollCts, size_t byteCount, U8* pData);						//Write data
COMM_RETURN_VALUE bApi_CommReadData(U8 cmd, bool pollCts, size_t byteCount, U8* pData);
COMM_RETURN_VALUE bApi_ReadRxDataBuffer(size_t bRxFifoLength, U8 *pbRxFifoData);								// Read Rx FIFO
COMM_RETURN_VALUE bApi_WriteTxDataBuffer(size_t bTxFifoLength, U8 *pbTxFifoData);								// Write Tx FIFO
COMM_RETURN_VALUE bApi_GetFastResponseRegister(U8 bStartReg, size_t bNmbrOfRegs, U8 * pbRegValues);			// Read Fast Response Register
COMM_RETURN_VALUE bApi_SendCmdGetResp(size_t bCmdLength, U8* pbCmdData, size_t respByteCount, U8* pRespData);	//Send a command and get response from the chip
COMM_RETURN_VALUE vSetPAMode(U8 bPaMode, U8 bModType);

//+++++++++++++++++++++
//  Global variables
//+++++++++++++++++++++


// Global variables, arrays
//extern	U8	abApi_Write[16];										// Write buffer for API communication
//extern	U8	abApi_Read[16];										// Read buffer for API communication
//extern	U8	bItStatus[8];										// Read buffer for API communication

#endif /* EZRP_NEXT_API_H_ */
