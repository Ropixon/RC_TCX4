/*
 * Si4464x_api_lib.c
 *
 * Created: 18.12.2012 22:37:42
 *  Author: Jirka
 */ 
#include "main.h"
#include "Si446x_api_lib.h"
#include "RadioUser.h"

/*!
 * This functions is used to reset the si446x radio by applying shutdown and
 * releasing it.  After this function @ref si446x_boot should be called.  You
 * can check if POR has completed by waiting 4 ms or by polling GPIO 0, 2, or 3.
 * When these GPIOs are high, it is safe to call @ref si446x_boot.
 */
void si446x_reset(void)
{
	osDelayWatchdog(5);	// kvuli intenrimu RTC mame kratsi ms!!
	LL_GPIO_SetOutputPin(spiDevice.pin_SDN.port, spiDevice.pin_SDN.pin);
	osDelayWatchdog(5);
	LL_GPIO_ResetOutputPin(spiDevice.pin_SDN.port, spiDevice.pin_SDN.pin);
	osDelayWatchdog(10);
}

/*!
 * This function is used to initialize after power-up the radio chip.
 * Before this function @si446x_reset should be called.
 */
COMM_RETURN_VALUE si446x_power_up( U8 BOOT_OPTIONS, U8 XTAL_OPTIONS, U32 XO_FREQ)
{
	U8	abApi_Write[7];	
	
    abApi_Write[0] = SI446X_CMD_ID_POWER_UP;
    abApi_Write[1] = BOOT_OPTIONS;
    abApi_Write[2] = XTAL_OPTIONS;
    abApi_Write[3] = (U8)(XO_FREQ >> 24);
    abApi_Write[4] = (U8)(XO_FREQ >> 16);
    abApi_Write[5] = (U8)(XO_FREQ >> 8);
    abApi_Write[6] = (U8)(XO_FREQ);
	return bApi_SendCommand(sizeof(abApi_Write), abApi_Write, true,true); // Use NoCTS version for fast initialization
}

/*!
 * This function is used to load all properties and commands with a list of NULL terminated commands.
 * Before this function @si446x_reset should be called.
 */
COMM_RETURN_VALUE si446x_configuration_init( const U8* pSetPropCmd)
{	
	U8								col;
	size_t							numOfBytes;
	U8								abApi_Write[16];
	SI446X_CMD_REPLY_UNION			Si446xCmd;
	U16								DataOffset=0;

  /* While cycle as far as the pointer points to a command */
  while ((numOfBytes = pSetPropCmd[DataOffset]) != 0x00)
  {
    /* Commands structure in the array:
     * --------------------------------
     * LEN | <LEN length of data>
     */
	DataOffset++;

    if (numOfBytes > 16)
    {
      /* Number of command bytes exceeds maximal allowable length */
      return SI446X_COMMAND_ERROR;
    }

    for (col = 0u; col < numOfBytes; col++)
    {
      abApi_Write[col] = pSetPropCmd[DataOffset];
      DataOffset++;
    }
	

	bApi_SendCommand(numOfBytes,abApi_Write,false,true);  // Use NoCTS version for fast initialization

	if (abApi_Write[0]==SI446X_CMD_ID_SET_PROPERTY)	//verification of data
	{
		si446x_get_property(abApi_Write[1],abApi_Write[2],abApi_Write[3],&Si446xCmd);
		if (memcmp((void *) &abApi_Write[4],(void *) &Si446xCmd,abApi_Write[2]))
		{
			return SI446X_COMMAND_ERROR;
		}
	}
	
    if (HAL_GPIO_ReadPin(spiDevice.pin_NIRQ.port,spiDevice.pin_NIRQ.pin) == 0)
    {
      /* Get and clear all interrupts.  An error has occured... */
	  si446x_get_int_status(0,0,0,&Si446xCmd);
      if (Si446xCmd.GET_INT_STATUS.CHIP_PEND & SI446X_CMD_GET_CHIP_STATUS_REP_CHIP_PEND_CMD_ERROR_PEND_MASK)
      {
        return SI446X_COMMAND_ERROR;
      }
    }
  }

  return SI446X_SUCCESS;
}

/*!
 * Get the Interrupt status/pending flags form the radio and clear flags if requested.
 *
 * @param PH_CLR_PEND     Packet Handler pending flags clear.
 * @param MODEM_CLR_PEND  Modem Status pending flags clear.
 * @param CHIP_CLR_PEND   Chip State pending flags clear.
 */
COMM_RETURN_VALUE si446x_get_int_status( U8 PH_CLR_PEND, U8 MODEM_CLR_PEND, U8 CHIP_CLR_PEND, SI446X_CMD_REPLY_UNION* ReturnData)
{
	COMM_RETURN_VALUE	ReturnFunction;
	U8	abApi_Write[SI446X_CMD_ARG_COUNT_GET_INT_STATUS];
	U8 	abApi_Read[SI446X_CMD_REPLY_COUNT_GET_INT_STATUS];
	
    abApi_Write[0] = SI446X_CMD_ID_GET_INT_STATUS;
    abApi_Write[1] = PH_CLR_PEND;
	abApi_Write[2] = MODEM_CLR_PEND;
    abApi_Write[3] = CHIP_CLR_PEND;
	
	ReturnFunction=bApi_SendCmdGetResp(SI446X_CMD_ARG_COUNT_GET_INT_STATUS,&abApi_Write[0],SI446X_CMD_REPLY_COUNT_GET_INT_STATUS,&abApi_Read[0]);
	
	ReturnData->GET_INT_STATUS.INT_PEND			= abApi_Read[0];			
	ReturnData->GET_INT_STATUS.INT_STATUS		= abApi_Read[1];		
    ReturnData->GET_INT_STATUS.PH_PEND			= abApi_Read[2];
    ReturnData->GET_INT_STATUS.PH_STATUS		= abApi_Read[3];
    ReturnData->GET_INT_STATUS.MODEM_PEND		= abApi_Read[4];
    ReturnData->GET_INT_STATUS.MODEM_STATUS		= abApi_Read[5];
    ReturnData->GET_INT_STATUS.CHIP_PEND		= abApi_Read[6];
    ReturnData->GET_INT_STATUS.CHIP_STATUS		= abApi_Read[7];
	return ReturnFunction;
}

/*! Sends START_TX command to the radio.
 *
 * @param CHANNEL   Channel number.
 * @param CONDITION Start TX condition.
 * @param TX_LEN    Payload length (exclude the PH generated CRC).
 */
void si446x_start_tx( U8 CHANNEL, U8 CONDITION, U16 TX_LEN, U8 TX_DELAY)
{
	U8 abApi_Write[SI446X_CMD_ARG_COUNT_START_TX];
	
    abApi_Write[0] = SI446X_CMD_ID_START_TX;
    abApi_Write[1] = CHANNEL;
    abApi_Write[2] = CONDITION;
    abApi_Write[3] = (U8)(TX_LEN >> 8);
    abApi_Write[4] = (U8)(TX_LEN & 0xFF);
	abApi_Write[5] = TX_DELAY;
	abApi_Write[6] = 0;
	
	bApi_SendCommand(SI446X_CMD_ARG_COUNT_START_TX, abApi_Write, true,true);

}
 
 /*!
 * Sends START_RX command to the radio.
 *
 * @param CHANNEL     Channel number.
 * @param CONDITION   Start RX condition.
 * @param RX_LEN      Payload length (exclude the PH generated CRC).
 * @param NEXT_STATE1 Next state when Preamble Timeout occurs.
 * @param NEXT_STATE2 Next state when a valid packet received.
 * @param NEXT_STATE3 Next state when invalid packet received (e.g. CRC error).
 */
void si446x_start_rx( U8 CHANNEL, U8 CONDITION, U16 RX_LEN, U8 NEXT_STATE1, U8 NEXT_STATE2, U8 NEXT_STATE3)
{
	U8 abApi_Write[SI446X_CMD_ARG_COUNT_START_RX];
	
    abApi_Write[0] = SI446X_CMD_ID_START_RX;
    abApi_Write[1] = CHANNEL;
    abApi_Write[2] = CONDITION;
    abApi_Write[3] = (U8)(RX_LEN >> 8);
    abApi_Write[4] = (U8)(RX_LEN);
    abApi_Write[5] = NEXT_STATE1;
    abApi_Write[6] = NEXT_STATE2;
    abApi_Write[7] = NEXT_STATE3;

	bApi_SendCommand(SI446X_CMD_ARG_COUNT_START_RX, abApi_Write, true,true);
}

/*!
 * Send GPIO pin config command to the radio and reads the answer into
 * @Si446xCmd union.
 *
 * @param GPIO0       GPIO0 configuration.
 * @param GPIO1       GPIO1 configuration.
 * @param GPIO2       GPIO2 configuration.
 * @param GPIO3       GPIO3 configuration.
 * @param NIRQ        NIRQ configuration.
 * @param SDO         SDO configuration.
 * @param GEN_CONFIG  General pin configuration.
 */
void si446x_gpio_pin_cfg( U8 GPIO_0, U8 GPIO_1, U8 GPIO_2, U8 GPIO_3, U8 NIRQ, U8 SDO, U8 GEN_CONFIG, SI446X_CMD_REPLY_UNION* ReturnData)
{
	U8 abApi_Write[SI446X_CMD_ARG_COUNT_GPIO_PIN_CFG];
	U8 abApi_Read[SI446X_CMD_REPLY_COUNT_GPIO_PIN_CFG];
	
    abApi_Write[0] = SI446X_CMD_ID_GPIO_PIN_CFG;
    abApi_Write[1] = GPIO_0;
    abApi_Write[2] = GPIO_1;
    abApi_Write[3] = GPIO_2;
    abApi_Write[4] = GPIO_3;
    abApi_Write[5] = NIRQ;
    abApi_Write[6] = SDO;
    abApi_Write[7] = GEN_CONFIG;
	
	bApi_SendCmdGetResp(SI446X_CMD_ARG_COUNT_GPIO_PIN_CFG, &abApi_Write[0],SI446X_CMD_REPLY_COUNT_PART_INFO, &abApi_Read[0]);

    ReturnData->GPIO_PIN_CFG.GPIO[0]        = abApi_Read[0];
    ReturnData->GPIO_PIN_CFG.GPIO[1]        = abApi_Read[1];
    ReturnData->GPIO_PIN_CFG.GPIO[2]        = abApi_Read[2];
    ReturnData->GPIO_PIN_CFG.GPIO[3]        = abApi_Read[3];
    ReturnData->GPIO_PIN_CFG.NIRQ          	= abApi_Read[4];
    ReturnData->GPIO_PIN_CFG.SDO           	= abApi_Read[5];
    ReturnData->GPIO_PIN_CFG.GEN_CONFIG    	= abApi_Read[6];
}

/*!
 * Send SET_PROPERTY command to the radio.
 *
 * @param GROUP       Property group.
 * @param NUM_PROPS   Number of property to be set. The properties must be in ascending order
 *                    in their sub-property aspect. Max. 12 properties can be set in one command.
 * @param START_PROP  Start sub-property address.
 */
												/* neni Neumber v API pro si4463, ale je to pocet, Start prop je number z dokumentace */
COMM_RETURN_VALUE si446x_set_property( U8 GROUP, U8 NUM_PROPS, U8 START_PROP, ... )
{
	U8 abApi_Write[13];
    va_list argList;
    size_t cmdIndex;
	COMM_RETURN_VALUE ReturnValue;

    abApi_Write[0] = SI446X_CMD_ID_SET_PROPERTY;
    abApi_Write[1] = GROUP;
    abApi_Write[2] = NUM_PROPS;
    abApi_Write[3] = START_PROP;

    va_start (argList, START_PROP);
    cmdIndex = 4;
    while(NUM_PROPS--)
    {
        abApi_Write[cmdIndex] = (U8)va_arg(argList, int);
        cmdIndex++;
    }
    va_end(argList);
	
	ReturnValue = bApi_SendCommand(cmdIndex, abApi_Write, true,true);
	if(ReturnValue!=SI446X_SUCCESS) LogError(4444441);
	return ReturnValue;
}

/*!
 * Issue a change state command to the radio.
 *
 * @param NEXT_STATE1 Next state.
 */
COMM_RETURN_VALUE si446x_change_state( U8 NEXT_STATE1)
{
	U8 abApi_Write[SI446X_CMD_ARG_COUNT_CHANGE_STATE];
	
    abApi_Write[0] = SI446X_CMD_ID_CHANGE_STATE;
    abApi_Write[1] = NEXT_STATE1;
	
	return bApi_SendCommand(SI446X_CMD_ARG_COUNT_CHANGE_STATE, abApi_Write, true,NEXT_STATE1!=SI446X_CMD_CHANGE_STATE_ARG_NEXT_STATE1_NEW_STATE_ENUM_SLEEP ? true:false);

}

/*!
 * Sends NOP command to the radio. Can be used to maintain SPI communication.
 */
void si446x_nop()
{
	U8 abApi_Write[SI446X_CMD_ARG_COUNT_NOP];
	
	abApi_Write[0] = SI446X_CMD_ID_NOP;

	bApi_SendCommand(SI446X_CMD_ARG_COUNT_NOP, abApi_Write, true,true);
}

/*!
 * Send the FIFO_INFO command to the radio. Optionally resets the TX/RX FIFO. Reads the radio response back
 * into @Si446xCmd.
 *
 * @param FIFO  RX/TX FIFO reset flags.
 */
COMM_RETURN_VALUE si446x_fifo_info( U8 FIFO, SI446X_CMD_REPLY_UNION* ReturnData)
{
	COMM_RETURN_VALUE	ReturnFunction;

	U8 abApi_Write[SI446X_CMD_ARG_COUNT_FIFO_INFO];
	U8 abApi_Read[SI446X_CMD_REPLY_COUNT_FIFO_INFO];
	
    abApi_Write[0] = SI446X_CMD_ID_FIFO_INFO;
    abApi_Write[1] = FIFO;
	
    ReturnFunction = bApi_SendCmdGetResp(SI446X_CMD_ARG_COUNT_FIFO_INFO, &abApi_Write[0],SI446X_CMD_REPLY_COUNT_FIFO_INFO, &abApi_Read[0]);

    ReturnData->FIFO_INFO.RX_FIFO_COUNT   = abApi_Read[0];
    ReturnData->FIFO_INFO.TX_FIFO_SPACE   = abApi_Read[1];

    return ReturnFunction;
}

/*!
 * The function can be used to load data into TX FIFO.
 *
 * @param numBytes  Data length to be load.
 * @param pTxData   Pointer to the data (U8*).
 */
void si446x_write_tx_fifo( U8 numBytes, U8* pTxData)
{
	bApi_CommWriteData(SI446X_CMD_ID_WRITE_TX_FIFO, false, numBytes, pTxData);
}

/*!
 * Reads the RX FIFO content from the radio.
 *
 * @param numBytes  Data length to be read.
 * @param pRxData   Pointer to the buffer location.
 */
COMM_RETURN_VALUE si446x_read_rx_fifo( size_t numBytes, U8* pRxData)
{
	return bApi_CommReadData(SI446X_CMD_ID_READ_RX_FIFO, false, numBytes, pRxData);
}

/*!
 * Get property values from the radio. Reads them into Si446xCmd union.
 *
 * @param GROUP       Property group number.
 * @param NUM_PROPS   Number of properties to be read.
 * @param START_PROP  Starting sub-property number.
 */
COMM_RETURN_VALUE si446x_get_property( U8 GROUP, U8 NUM_PROPS, U8 START_PROP, SI446X_CMD_REPLY_UNION* ReturnData)
{
	COMM_RETURN_VALUE ReturnStateValue;
	U8 abApi_Write[SI446X_CMD_ARG_COUNT_GET_PROPERTY];
	U8 abApi_Read[SI446X_CMD_REPLY_COUNT_GET_PROPERTY];
	
    abApi_Write[0] = SI446X_CMD_ID_GET_PROPERTY;
    abApi_Write[1] = GROUP;
    abApi_Write[2] = NUM_PROPS;
    abApi_Write[3] = START_PROP;
	
	ReturnStateValue=bApi_SendCmdGetResp(SI446X_CMD_ARG_COUNT_GET_PROPERTY,abApi_Write,NUM_PROPS,abApi_Read);
	
    ReturnData->GET_PROPERTY.DATA[0]    = abApi_Read[0];
    ReturnData->GET_PROPERTY.DATA[1]   = abApi_Read[1];
    ReturnData->GET_PROPERTY.DATA[2]    = abApi_Read[2];
    ReturnData->GET_PROPERTY.DATA[3]    = abApi_Read[3];
    ReturnData->GET_PROPERTY.DATA[4]    = abApi_Read[4];
    ReturnData->GET_PROPERTY.DATA[5]    = abApi_Read[5];
    ReturnData->GET_PROPERTY.DATA[6]    = abApi_Read[6];
    ReturnData->GET_PROPERTY.DATA[7]    = abApi_Read[7];
    ReturnData->GET_PROPERTY.DATA[8]    = abApi_Read[8];
    ReturnData->GET_PROPERTY.DATA[9]    = abApi_Read[9];
    ReturnData->GET_PROPERTY.DATA[10]   = abApi_Read[10];
    ReturnData->GET_PROPERTY.DATA[11]   = abApi_Read[11];
    ReturnData->GET_PROPERTY.DATA[12]   = abApi_Read[12];
    ReturnData->GET_PROPERTY.DATA[13]   = abApi_Read[13];
    ReturnData->GET_PROPERTY.DATA[14]   = abApi_Read[14];
    ReturnData->GET_PROPERTY.DATA[15]   = abApi_Read[15];
	
	return ReturnStateValue;
}

/*!
 * Gets the Packet Handler status flags. Optionally clears them.
 *
 * @param PH_CLR_PEND Flags to clear.
 */
void si446x_get_ph_status(U8 PH_CLR_PEND,SI446X_CMD_REPLY_UNION* ReturnData)
{
	U8 abApi_Write[SI446X_CMD_ARG_COUNT_GET_PH_STATUS];
	U8 abApi_Read[SI446X_CMD_REPLY_COUNT_GET_PH_STATUS];
	
    abApi_Write[0] = SI446X_CMD_ID_GET_PH_STATUS;
    abApi_Write[1] = PH_CLR_PEND;
	
	bApi_SendCmdGetResp(SI446X_CMD_ARG_COUNT_GET_PH_STATUS,abApi_Write,SI446X_CMD_REPLY_COUNT_GET_PH_STATUS,abApi_Read);

	ReturnData->GET_PH_STATUS.PH_PEND	= abApi_Read[0];
	ReturnData->GET_PH_STATUS.PH_STATUS	= abApi_Read[1];
}

/*!
 * Gets the Modem status flags. Optionally clears them.
 *
 * @param MODEM_CLR_PEND Flags to clear.
 */
COMM_RETURN_VALUE si446x_get_modem_status( U8 MODEM_CLR_PEND, SI446X_CMD_REPLY_UNION* ReturnData)
{
	COMM_RETURN_VALUE ReturnValue;
	U8 abApi_Write[SI446X_CMD_ARG_COUNT_GET_MODEM_STATUS];
	U8 abApi_Read[SI446X_CMD_REPLY_COUNT_GET_MODEM_STATUS];
	
    abApi_Write[0] = SI446X_CMD_ID_GET_MODEM_STATUS;
    abApi_Write[1] = MODEM_CLR_PEND;

    ReturnValue = bApi_SendCmdGetResp(SI446X_CMD_ARG_COUNT_GET_MODEM_STATUS,abApi_Write,SI446X_CMD_REPLY_COUNT_GET_MODEM_STATUS,abApi_Read);

    ReturnData->GET_MODEM_STATUS.MODEM_PEND   = abApi_Read[0];
    ReturnData->GET_MODEM_STATUS.MODEM_STATUS = abApi_Read[1];
    ReturnData->GET_MODEM_STATUS.CURR_RSSI    = abApi_Read[2];
    ReturnData->GET_MODEM_STATUS.LATCH_RSSI   = abApi_Read[3];
    ReturnData->GET_MODEM_STATUS.ANT1_RSSI    = abApi_Read[4];
    ReturnData->GET_MODEM_STATUS.ANT2_RSSI    = abApi_Read[5];
    MSB(ReturnData->GET_MODEM_STATUS.AFC_FREQ_OFFSET)  = abApi_Read[6];
    LSB(ReturnData->GET_MODEM_STATUS.AFC_FREQ_OFFSET)  = abApi_Read[7];

    return ReturnValue;
}

/*!
 * Requests the current state of the device and lists pending TX and RX requests
 */
void si446x_request_device_state( SI446X_CMD_REPLY_UNION* ReturnData)
{
	U8 abApi_Write[SI446X_CMD_ARG_COUNT_REQUEST_DEVICE_STATE];
	U8 abApi_Read[SI446X_CMD_REPLY_COUNT_REQUEST_DEVICE_STATE];
	
    abApi_Write[0] = SI446X_CMD_ID_REQUEST_DEVICE_STATE;

    bApi_SendCmdGetResp(SI446X_CMD_ARG_COUNT_REQUEST_DEVICE_STATE,abApi_Write,SI446X_CMD_REPLY_COUNT_REQUEST_DEVICE_STATE,abApi_Read);

    ReturnData->REQUEST_DEVICE_STATE.CURR_STATE       = abApi_Read[0];
    ReturnData->REQUEST_DEVICE_STATE.CURRENT_CHANNEL  = abApi_Read[1];
}

/*! This function sends the PART_INFO command to the radio and receives the answer
 *  into @Si446xCmd union.
 */
void si446x_part_info(SI446X_CMD_REPLY_UNION* ReturnData)
{
	U8 abApi_Write[SI446X_CMD_ARG_COUNT_PART_INFO];
	U8 abApi_Read[SI446X_CMD_REPLY_COUNT_PART_INFO];
	
    abApi_Write[0] = SI446X_CMD_ID_PART_INFO;

    bApi_SendCmdGetResp( SI446X_CMD_ARG_COUNT_PART_INFO,abApi_Write,SI446X_CMD_REPLY_COUNT_PART_INFO,abApi_Read);

    ReturnData->PART_INFO.CHIPREV			= abApi_Read[0];
    MSB(ReturnData->PART_INFO.PART)			= abApi_Read[1];
    LSB(ReturnData->PART_INFO.PART)			= abApi_Read[2];
    ReturnData->PART_INFO.PBUILD			= abApi_Read[3];
    MSB(ReturnData->PART_INFO.ID)			= abApi_Read[4];
    LSB(ReturnData->PART_INFO.ID)			= abApi_Read[5];
    ReturnData->PART_INFO.CUSTOMER			= abApi_Read[6];
    ReturnData->PART_INFO.ROMID				= abApi_Read[7];
}

/************************************************************************/
//Reads the fast response registers (FRR) starting with FRR_A.
/************************************************************************/
U8 si446x_FRR_A_READ(U8 respByteCount, SI446X_CMD_REPLY_UNION* ReturnData)
{
	U8 ReturnFunction;
	U8 abApi_Read[SI446X_CMD_REPLY_COUNT_FRR_A_READ]={0,0,0,0};
	
	ReturnFunction=bApi_CommReadData(SI446X_CMD_ID_FRR_A_READ, false, respByteCount, abApi_Read);
	
	ReturnData->FRR_A_READ.FRR_A_VALUE = abApi_Read[0];
	ReturnData->FRR_A_READ.FRR_B_VALUE = abApi_Read[1];
	ReturnData->FRR_A_READ.FRR_C_VALUE = abApi_Read[2];
	ReturnData->FRR_A_READ.FRR_D_VALUE = abApi_Read[3];
	
	return ReturnFunction;	
} 


