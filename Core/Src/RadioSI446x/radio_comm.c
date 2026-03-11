/*
 * ezrp_next_api.c
 *
 *  Created on: 07/11/2012.
 *      Author: vihuszar, atgosi
 */
#include "main.h"
#include <radio_comm.h>
#include "si446x_defs.h"
#include "RadioUser.h"
// Global variables, arrays
//U8	abApi_Write[16];									// Write buffer for API communication
uint8_t	abApi_Read[16];										// Read buffer for API communication
uint8_t	bItStatus[8];										// Read buffer for API communication
	

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  U8 bApi_SendCommand(U8 bCmdLength, U8 *pbCmdData)
  +
  + DESCRIPTION:    send API command, no response expected
  +
  + INPUT:          bCmdLength - nmbr of bytes to be sent
  +                 *pbCmdData - pointer to the commands
  +
  + RETURN:         None
  +
  + NOTES:          None
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
COMM_RETURN_VALUE bApi_SendCommand(size_t bCmdLength, U8 *pbCmdData,bool waitForCTSBefore,bool waitForCTSAfter)   // Send a command + data to the chip
{
	COMM_RETURN_VALUE			ReturnValue=SI446X_COMMAND_ERROR;

	if(waitForCTSBefore == true)
	{
		if (vApi_WaitforCTS(false) != SI446X_SUCCESS) LogError(485132); // CTS timeout
	}

	LL_GPIO_ResetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);
	if (HAL_SPI_Transmit(spiDevice.target,pbCmdData,bCmdLength,500) == HAL_OK) ReturnValue=SI446X_SUCCESS;
	LL_GPIO_SetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);

	if(waitForCTSAfter == true)
	{
		if (vApi_WaitforCTS(false) != SI446X_SUCCESS) LogError(485132); // CTS timeout
	}

	return ReturnValue;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  U8 vApi_WaitforCTS(void)
  +
  + DESCRIPTION:    wait for CTS
  +
  + INPUT:          None
  +
  + RETURN:         0 - CTS arrived
  +                 1 - CTS didn't arrive within MAX_CTS_RETRY
  +
  + NOTES:          None
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
COMM_RETURN_VALUE vApi_WaitforCTS(bool stayNselLow)
{
	uint8_t				bCtsValue=0;
	uint16_t			bErrCnt=0;

	while (1)
	{
		LL_GPIO_ResetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);
		LL_SPI_TransmitData8(spiDevice.target->Instance, SI446X_CMD_ID_READ_CMD_BUFF);
		while (!LL_SPI_IsActiveFlag_RXNE(spiDevice.target->Instance));
		LL_SPI_ReceiveData8(spiDevice.target->Instance);

		LL_SPI_TransmitData8(spiDevice.target->Instance, SI446X_CMD_ID_NOP);
		while (!LL_SPI_IsActiveFlag_RXNE(spiDevice.target->Instance));
		bCtsValue = LL_SPI_ReceiveData8(spiDevice.target->Instance);

		if(bCtsValue == 0xFF)
		{
			if(stayNselLow == false)
			{
				LL_GPIO_SetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);
			}

			return 	SI446X_SUCCESS;
		}
		else
		{
			LL_GPIO_SetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);
			if (++bErrCnt > MAX_CTS_RETRY)
			{
				LL_GPIO_SetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);
				return SI446X_COMMAND_ERROR;     // Error handling; if wrong CTS reads exceeds a limit
			}
		}

	}


	//while(!(LL_GPIO_ReadInputPort(RF_GPIO1_CTS_GPIO_Port)&RF_GPIO1_CTS_Pin));
	//return 	SI446X_SUCCESS;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  U8 vApi_WaitforCTS(void)
  +
  + DESCRIPTION:    wait for CTS
  +
  + INPUT:          None
  +
  + RETURN:         0 - CTS arrived
  +                 1 - CTS didn't arrive within MAX_CTS_RETRY
  +
  + NOTES:          None
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
COMM_RETURN_VALUE vApi_WaitforCTSAfterReset(void)
{
	while(!(LL_GPIO_ReadInputPort(RF_GPIO0_GPIO_Port) & RF_GPIO0_Pin))
	{
		osDelay(1); // wait for CTS pin to be high
	}
	return 	SI446X_SUCCESS;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  U8 bApi_GetFastResponseRegister(U8 bStartReg, U8 bNmbrOfRegs, U8 * pbRegValues)
  +
  + DESCRIPTION:    get fast response registers
  +
  + INPUT:          bStartReg - one of the fast response registers
  +                 bNmbrOfRegs - number of registers (1 ... 4)
  +                 pbRegValues - value of the registers
  +
  + RETURN:         0 - operation successful
  +                 1 - error
  +
  + NOTES:          None
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
COMM_RETURN_VALUE bApi_GetFastResponseRegister(U8 bStartReg, size_t bNmbrOfRegs, U8 * pbRegValues)
{
	COMM_RETURN_VALUE	ReturnValue=SI446X_COMMAND_ERROR;
	
	if((bNmbrOfRegs == 0) || (bNmbrOfRegs > 4))
	{
		return SI446X_COMMAND_ERROR;
	}
	
	LL_GPIO_ResetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);
	LL_SPI_TransmitData8(spiDevice.target->Instance, bStartReg);
	while (!LL_SPI_IsActiveFlag_RXNE(spiDevice.target->Instance));

	memset(pbRegValues, DUMMY_BYTE, bNmbrOfRegs);
	if (HAL_SPI_Receive(spiDevice.target,pbRegValues,bNmbrOfRegs,500)==HAL_OK) ReturnValue=SI446X_SUCCESS;
	LL_GPIO_SetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);

	return ReturnValue;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  U8 bApi_GetResponse(U8 bRespLength, U8 *pbRespData)
  +
  + DESCRIPTION:    wait for CTS and get the read bytes from the chip
  +
  + INPUT:          bRespLength - nmbr of bytes to be read
  +                 *pbRespData - pointer to the read data
  +
  + RETURN:         0 - operation successful
  +                 1 - no CTS within MAX_CTS_RETRY
  +
  + NOTES:          None
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
COMM_RETURN_VALUE bApi_GetResponse(size_t bRespLength, U8 *pbRespData) // Get response from the chip (used after a command)
{
	if(vApi_WaitforCTS(true) != SI446X_SUCCESS) // Wait for CTS
	{
		LogError(445182);
	}

	memset(pbRespData, DUMMY_BYTE, bRespLength);
	HAL_SPI_Receive(spiDevice.target, pbRespData, bRespLength, 500);
	LL_GPIO_SetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);
	return SI446X_SUCCESS;
	
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  U8 bApi_CommWriteData(U8 cmd, bool pollCts, size_t byteCount, U8* pData)
  +
  + DESCRIPTION:    send API command and writing data
  +
  + INPUT:          @param cmd           Command ID
  +                 @param pollCts       Set to poll CTS
  +                 @param byteCount     Number of bytes to get from the radio chip
  +                 @param pData         Pointer to where to put the data
  +
  + RETURN:         0 - operation successful
  +                 1 - error CTS
  +
  + NOTES:          None
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
COMM_RETURN_VALUE bApi_CommWriteData(U8 cmd, bool pollCts, size_t byteCount, U8* pData)
{
	if (pollCts==true)
	{
		if(vApi_WaitforCTS(true) != SI446X_SUCCESS) // Wait for CTS
		{
			LogError(445182);
		}
	}

	LL_GPIO_ResetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);
	LL_SPI_TransmitData8(spiDevice.target->Instance, cmd);
	while (!LL_SPI_IsActiveFlag_RXNE(spiDevice.target->Instance));

	HAL_SPI_Transmit(spiDevice.target, pData, byteCount, 500);
	LL_GPIO_SetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);

	return SI446X_SUCCESS;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Gets a command response from the radio chip
 *
 * @param cmd           Command ID
 * @param pollCts       Set to poll CTS
 * @param byteCount     Number of bytes to get from the radio chip.
 * @param pData         Pointer to where to put the data.
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

COMM_RETURN_VALUE bApi_CommReadData(U8 cmd, bool pollCts, size_t byteCount, U8* pData)
{
	if (pollCts==true)
	{
		if(vApi_WaitforCTS(true) != SI446X_SUCCESS) // Wait for CTS
		{
			LogError(445182);
		}
	}
	LL_GPIO_ResetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);
	LL_SPI_TransmitData8(spiDevice.target->Instance, cmd);
	while (!LL_SPI_IsActiveFlag_RXNE(spiDevice.target->Instance));

	memset(pData, DUMMY_BYTE, byteCount);
	HAL_SPI_Receive(spiDevice.target, pData, byteCount, 500);
	LL_GPIO_SetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);

	return SI446X_SUCCESS;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  U8 bApi_ReadRxDataBuffer(U8 bRxFifoLength, U8 *pbRxFifoData)
  +
  + DESCRIPTION:    Read RX FIFO content from the chip
  +
  + INPUT:          bRxFifoLength - nmbr of bytes to be read
  +
  + RETURN:         *pbRxFifoData - pointer to the read data
  +                 0             - operation successful
  +
  + NOTES:          None
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
COMM_RETURN_VALUE bApi_ReadRxDataBuffer(size_t bRxFifoLength, U8 *pbRxFifoData)
{
	LL_GPIO_ResetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);
	LL_SPI_TransmitData8(spiDevice.target->Instance, SI446X_CMD_ID_READ_RX_FIFO);
	while (!LL_SPI_IsActiveFlag_RXNE(spiDevice.target->Instance));

	memset(pbRxFifoData, DUMMY_BYTE, bRxFifoLength);
	HAL_SPI_Receive(spiDevice.target, pbRxFifoData, bRxFifoLength, 500);
	LL_GPIO_SetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);

	return SI446X_SUCCESS;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  +
  + FUNCTION NAME:  U8 bApi_WriteTxDataBuffer(U8 bTxFifoLength, U8 *pbTxFifoData)
  +
  + DESCRIPTION:    wait for CTS and get the read bytes from the chip
  +
  + INPUT:      bTxFifoLength - nmbr of bytes to be sent
  +         *pbTxFifoData - pointer to the transmit data
  +
  + RETURN:         0 - operation successful
  +         1 - no CTS within MAX_CTS_RETRY
  +
  + NOTES:          None
  +
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
COMM_RETURN_VALUE bApi_WriteTxDataBuffer(size_t bTxFifoLength, U8 *pbTxFifoData)
{
	LL_GPIO_ResetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);
	LL_SPI_TransmitData8(spiDevice.target->Instance, SI446X_CMD_ID_WRITE_TX_FIFO);
	while (!LL_SPI_IsActiveFlag_RXNE(spiDevice.target->Instance));

	HAL_SPI_Transmit(spiDevice.target, pbTxFifoData, bTxFifoLength, 500);
	LL_GPIO_SetOutputPin(spiDevice.pin_NSS.port, spiDevice.pin_NSS.pin);

	return SI446X_SUCCESS;
}

/*!
 * Sends a command to the radio chip and gets a response
 *
 * @param cmdByteCount  Number of bytes in the command to send to the radio device
 * @param pCmdData      Pointer to the command data
 * @param respByteCount Number of bytes in the response to fetch
 * @param pRespData     Pointer to where to put the response data
 *
 * @return CTS value
 */
COMM_RETURN_VALUE bApi_SendCmdGetResp(size_t bCmdLength, U8* pbCmdData, size_t respByteCount, U8* pRespData)
{

	bApi_SendCommand(bCmdLength,pbCmdData,true,false);
	return  bApi_GetResponse(respByteCount,pRespData);
}




