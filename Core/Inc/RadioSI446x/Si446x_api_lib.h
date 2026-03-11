/*
 * Si4464x_api_lib.h
 *
 * Created: 18.12.2012 22:37:56
 *  Author: Jirka
 */ 


#ifndef SI4464X_API_LIB_H_
#define SI4464X_API_LIB_H_

#include "radio_comm.h"
#include "si446x_defs.h"

typedef union si446x_cmd_reply_union SI446X_CMD_REPLY_UNION;

typedef struct
{
	U8   Radio_ChannelNumber;
	U8   Radio_PacketLength;
	U8   Radio_State_After_Power_Up;

	U16  Radio_Delay_Cnt_After_Reset;
	U8   Radio_ConfigurationArray[];
} tRadioConfiguration;

void si446x_reset();
COMM_RETURN_VALUE si446x_power_up( U8 BOOT_OPTIONS, U8 XTAL_OPTIONS, U32 XO_FREQ);
COMM_RETURN_VALUE si446x_configuration_init( const U8* pSetPropCmd);
COMM_RETURN_VALUE si446x_get_int_status(U8 PH_CLR_PEND, U8 MODEM_CLR_PEND, U8 CHIP_CLR_PEND, SI446X_CMD_REPLY_UNION* ReturnData);
void si446x_start_tx( U8 CHANNEL, U8 CONDITION, U16 TX_LEN, U8 TX_DELAY);
void si446x_start_rx( U8 CHANNEL, U8 CONDITION, U16 RX_LEN, U8 NEXT_STATE1, U8 NEXT_STATE2, U8 NEXT_STATE3);
void si446x_gpio_pin_cfg( U8 GPIO_0, U8 GPIO_1, U8 GPIO_2, U8 GPIO_3, U8 NIRQ, U8 SDO, U8 GEN_CONFIG, SI446X_CMD_REPLY_UNION* ReturnData);
COMM_RETURN_VALUE si446x_set_property( U8 GROUP, U8 NUM_PROPS, U8 START_PROP, ... ); //another data is U16
COMM_RETURN_VALUE si446x_change_state( U8 NEXT_STATE1);
void si446x_nop();
COMM_RETURN_VALUE si446x_fifo_info( U8 FIFO, SI446X_CMD_REPLY_UNION* ReturnData);
void si446x_write_tx_fifo( U8 numBytes, U8* pTxData);
COMM_RETURN_VALUE si446x_read_rx_fifo( size_t numBytes, U8* pRxData);
COMM_RETURN_VALUE si446x_get_property( U8 GROUP, U8 NUM_PROPS, U8 START_PROP, SI446X_CMD_REPLY_UNION* ReturnData);
void si446x_get_ph_status(U8 PH_CLR_PEND,SI446X_CMD_REPLY_UNION* ReturnData);
COMM_RETURN_VALUE si446x_get_modem_status( U8 MODEM_CLR_PEND, SI446X_CMD_REPLY_UNION* ReturnData);
void si446x_request_device_state( SI446X_CMD_REPLY_UNION* ReturnData);
void si446x_part_info(SI446X_CMD_REPLY_UNION* ReturnData);
U8 si446x_FRR_A_READ(U8 respByteCount, SI446X_CMD_REPLY_UNION* ReturnData);

#endif /* SI4464X_API_LIB_H_ */
