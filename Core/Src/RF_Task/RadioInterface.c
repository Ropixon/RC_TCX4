/*
 * RadioInterface.c
 *
 *  Created on: Jun 29, 2021
 *      Author: developer
 */

#include "main.h"
#include "TaskRF.h"
#include "RadioInterface.h"
#include "RadioUser.h"
#include "radio_config_Si4463.h"
#include "radio_comm.h"
#include "Si446x_api_lib.h"
#include "si446x_cmd.h"
#include "EepromAddress.h"
#include "SignalProcessing.h"

//uint16_t GlRepeatCounter;

static const 	U8  RadioConfigurationDataArray[] = RADIO_CONFIGURATION_DATA_ARRAY;
const 			U8* pRadioConfigurationDataArray = &RadioConfigurationDataArray[0];



/**
 *
 */
void RI_InitRadio(void)
{
	SI446X_CMD_REPLY_UNION	Si446xCmd;
	COMM_RETURN_VALUE ret;
	uint8_t temp;

	do
	{
		si446x_reset();
		vApi_WaitforCTSAfterReset();
		ret = si446x_configuration_init(pRadioConfigurationDataArray);

	} while (ret != SI446X_SUCCESS);


	si446x_part_info(&Si446xCmd);

	if(Si446xCmd.PART_INFO.PART != 0x4463)
	{
		LogError(15324);
	}

	//Setting FIFO half-duplex 129-byte shared FIFO mode
	ret=si446x_get_property(SI446X_PROP_GRP_ID_GLOBAL,1,SI446X_PROP_GRP_INDEX_GLOBAL_CONFIG,&Si446xCmd);
	if (ret!=SI446X_SUCCESS) LogError(1531);

	temp=Si446xCmd.GET_PROPERTY.DATA[0]|SI446X_PROP_GLOBAL_CONFIG_FIFO_MODE_BIT;

	ret=si446x_set_property(SI446X_PROP_GRP_ID_GLOBAL,1,SI446X_PROP_GRP_INDEX_GLOBAL_CONFIG,temp);
	if (ret!=SI446X_SUCCESS) LogError(1531);

	ret=si446x_get_property(SI446X_PROP_GRP_ID_GLOBAL,1,SI446X_PROP_GRP_INDEX_GLOBAL_CONFIG,&Si446xCmd);
	if ((temp!=Si446xCmd.GET_PROPERTY.DATA[0]) || (ret!=SI446X_SUCCESS)) LogError(15321);


	ret=si446x_set_property(SI446X_PROP_GRP_ID_PA, 1, SI446X_PROP_GRP_INDEX_PA_PWR_LVL, RF_POWER);
	if (ret!=SI446X_SUCCESS) LogError(15322);
	ret=si446x_get_property(SI446X_PROP_GRP_ID_PA,1,SI446X_PROP_GRP_INDEX_PA_PWR_LVL,&Si446xCmd);
	if ((Si446xCmd.GET_PROPERTY.DATA[0] != RF_POWER) || (ret!=SI446X_SUCCESS)) LogError(15323);

	//Setting length field 1 = 105 bytes (5×21 packets in shared FIFO)
	ret=si446x_set_property(SI446X_PROP_GRP_ID_PKT,2,SI446X_PROP_GRP_INDEX_PKT_FIELD_1_LENGTH,0x00,FIX_PCKT_LEN*NUM_PACKET_IN_FIFO);
	if (ret!=SI446X_SUCCESS) LogError(1531);

	ret=si446x_get_property(SI446X_PROP_GRP_ID_PKT,2,SI446X_PROP_GRP_INDEX_PKT_FIELD_1_LENGTH,&Si446xCmd);
	if ((Si446xCmd.GET_PROPERTY.DATA[0]!=0x00) || (Si446xCmd.GET_PROPERTY.DATA[1]!=FIX_PCKT_LEN*NUM_PACKET_IN_FIFO) || (ret!=SI446X_SUCCESS)) LogError(965231);

	RI_SetReady();
	RI_ClearPendingIRQ();
	RU_ClearAndDisableIRQ();
	RU_ClearAndEnableIRQ();

	RI_SetSleep();
}


/**
 *
 */
void RI_SetSleep(void)
{
	COMM_RETURN_VALUE		ret;

	ret = si446x_change_state(SI446X_CMD_REQUEST_DEVICE_STATE_REP_CURR_STATE_MAIN_STATE_ENUM_SLEEP);

	if(ret != SI446X_SUCCESS)	LogError(965312);

}

/**
 *
 */
void RI_SetReady(void)
{
	SI446X_CMD_REPLY_UNION	Si446xCmd;
	COMM_RETURN_VALUE		ret;

	//Wake up the radio from SLEEP mode
	while(1)
	{
		si446x_request_device_state(&Si446xCmd);
		if (Si446xCmd.REQUEST_DEVICE_STATE.CURR_STATE == 3) break;
		ret = si446x_change_state(SI446X_CMD_REQUEST_DEVICE_STATE_REP_CURR_STATE_MAIN_STATE_ENUM_READY);
		if(ret != SI446X_SUCCESS) LogError(13551);

	}

}


void RI_ClearPendingIRQ(void)
{
	SI446X_CMD_REPLY_UNION	Si446xCmd;

	/* Clear all pending interrupts */
	if(si446x_get_int_status(0,0,0,&Si446xCmd) != SI446X_SUCCESS) LogError(9846);

}



/**
 *
 */
void RI_SendPayload(uint8_t *payload, uint8_t size, bool nick,bool willChangedata, tGlobalDataRF * glData)
{

	SI446X_CMD_REPLY_UNION	Si446xCmd;

	RI_SetReady();
	RI_ClearPendingIRQ();

	RI_SetRfFreq(EA_GetRfFreq());

	glData->stopTXAfterTX = false;
	glData->onePacketDone = false;

	RI_FillTXFIFO(payload,size);

	if(willChangedata == true)
	{
		if(si446x_set_property(SI446X_PROP_GRP_ID_INT_CTL, 0x02, SI446X_PROP_GRP_INDEX_INT_CTL_ENABLE,
					1<<0 /* packet handler */, (1<<1)/* TX_FIFO_ALMOST_EMPTY */) != SI446X_SUCCESS) LogError(1531);

		if(si446x_set_property(SI446X_PROP_GRP_ID_PKT,0x01,SI446X_PROP_GRP_INDEX_PKT_TX_THRESHOLD,
				NUM_PACKET_IN_TX_FIFO_ALMOST_EMPTY*FIX_PCKT_LEN/* empty threshol*/ ) != SI446X_SUCCESS) LogError(1531); // irq nastane pokud ve fifu zbyva

		si446x_start_tx(0,(7<<4)/* go to ready state */ | (1<<2)  /* send last packet again*/ ,FIX_PCKT_LEN*NUM_PACKET_IN_FIFO,0);
	}
	else
	{
		if(si446x_set_property(SI446X_PROP_GRP_ID_INT_CTL,0x01,SI446X_PROP_GRP_INDEX_INT_CTL_PH_ENABLE,
				(1<<5)/*tx done*/ ) != SI446X_SUCCESS) LogError(1531);

		si446x_start_tx(0,(7<<4)/* stay in TX */ | (1<<2)  /*send last packet again*/,FIX_PCKT_LEN*NUM_PACKET_IN_FIFO,0);
	}

	si446x_request_device_state(&Si446xCmd);
	if(Si446xCmd.REQUEST_DEVICE_STATE.CURR_STATE != SI446X_CMD_REQUEST_DEVICE_STATE_REP_CURR_STATE_MAIN_STATE_ENUM_TX)
	{
		LL_GPIO_SetOutputPin(spiDevice.pin_SDN.port, spiDevice.pin_SDN.pin);
		LogError(863521);
	}

	if(nick == true) 	glData->stopTXAfterTX = true;

	glData->inTxState = true;

}

void RI_FillTXFIFO(uint8_t *payload, uint8_t size)
{

	SI446X_CMD_REPLY_UNION	Si446xCmd;
	si446x_fifo_info(0b11, &Si446xCmd);
	/* 5 × 21 bytes = 105 bytes fits in 129-byte shared FIFO */

	if(size != FIX_PCKT_LEN)	LogError(15325);

	for(uint8_t i=0; i<NUM_PACKET_IN_FIFO; i++)
	{
		si446x_write_tx_fifo(size, payload);
	}

}

/**
 *
 */
void RI_StartTxCW(uint8_t power)
{
	SI446X_CMD_REPLY_UNION	Si446xCmd;
	COMM_RETURN_VALUE ret;
	uint8_t modulace;

	RI_SetRfFreq(EA_GetRfFreq());

	ret=si446x_get_property(SI446X_PROP_GRP_ID_MODEM,1,SI446X_PROP_GRP_INDEX_MODEM_MOD_TYPE,&Si446xCmd);
	modulace = Si446xCmd.GET_PROPERTY.DATA[0];

	modulace&=0b11111000;
	modulace |=SI446X_PROP_MODEM_MOD_TYPE_MOD_TYPE_ENUM_CW;

	ret=si446x_set_property(SI446X_PROP_GRP_ID_MODEM, 1, SI446X_PROP_GRP_INDEX_MODEM_MOD_TYPE, modulace);
	if (ret!=SI446X_SUCCESS) LogError(15322);
	ret=si446x_get_property(SI446X_PROP_GRP_ID_MODEM,1,SI446X_PROP_GRP_INDEX_MODEM_MOD_TYPE,&Si446xCmd);
	if ((Si446xCmd.GET_PROPERTY.DATA[0] != modulace) || (ret!=SI446X_SUCCESS)) LogError(15323);

	si446x_start_tx(0,(7<<4)|(1<<2),FIX_PCKT_LEN*NUM_PACKET_IN_FIFO,0);
}

void RI_SetRfFreq(uint32_t freq)
{
	COMM_RETURN_VALUE ret;
	SI446X_CMD_REPLY_UNION	Si446xCmd;
	uint32_t rfFrac;

	ret=si446x_get_property(SI446X_PROP_GRP_ID_FREQ_CONTROL,3,SI446X_PROP_GRP_INDEX_FREQ_CONTROL_FRAC,&Si446xCmd);
	if ((ret!=SI446X_SUCCESS)) LogError(15321);

	rfFrac = (Si446xCmd.GET_PROPERTY.DATA[2])|(Si446xCmd.GET_PROPERTY.DATA[1]<<8)|
			(Si446xCmd.GET_PROPERTY.DATA[0]<<16);

	rfFrac = SP_CalculateRfFreq(rfFrac);

	if(rfFrac != freq)
	{
		rfFrac = SP_CalculateRFFrac((uint32_t)(freq/1000));

		// Correctly set the bytes in the same order as they are read
		Si446xCmd.GET_PROPERTY.DATA[0] = (uint8_t)(rfFrac >> 16);  // MSB
		Si446xCmd.GET_PROPERTY.DATA[1] = (uint8_t)(rfFrac >> 8);   // Middle byte
		Si446xCmd.GET_PROPERTY.DATA[2] = (uint8_t)(rfFrac);        // LSB

		ret=si446x_set_property(SI446X_PROP_GRP_ID_FREQ_CONTROL,3,SI446X_PROP_GRP_INDEX_FREQ_CONTROL_FRAC,Si446xCmd.GET_PROPERTY.DATA[0],Si446xCmd.GET_PROPERTY.DATA[1],
				Si446xCmd.GET_PROPERTY.DATA[2]);

		if (ret!=SI446X_SUCCESS) LogError(1531);

	}
}
/**
 *
 */
void RI_StopTXandDiscard(tGlobalDataRF *glData)
{
	SI446X_CMD_REPLY_UNION	Si446xCmd;

	RI_SetReady();
	si446x_fifo_info(0b11, &Si446xCmd);
	RI_ClearPendingIRQ();
	RU_ClearAndDisableIRQ();
	RI_SetSleep();
}
