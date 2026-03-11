/*
 * UartProcess.h
 *
 *  Created on: Oct 26, 2023
 *      Author: Uzivatel
 */

#ifndef INC_UARTPROCESS_H_
#define INC_UARTPROCESS_H_

#if (VERSION_WITH_RING == 1)

#define RING_UART_SYNC_SIZE			(1)
#define RING_UART_HEADER_SIZE		(6)
#define RING_UART_HEADER_CRC_SIZE	(1)
#define RING_UART_FINAL_CRC_SIZE	(1)
#define RING_PACKET_MAX_SIZE		(25)
#define RING_CIRCLE_BUFFER_SIZE		(200)
#define RING_UART_SYNC_WORD 		(0xFF)


/*
 *
 */
typedef enum{
    UART_PCKT_TYPE_RF   =   1,


    UART_PCKT_TYPE_HB   =   254

}uartPacketTypes_e;

/*
*
*/
typedef struct{
    uint8_t              next_0xFF_pos;
    uint8_t              payload_size;
    uartPacketTypes_e    payload_code;
    uint8_t              RFU[2];
    uint8_t              crcHeader;
}__attribute__((packed)) uartHeader_t;

/**
 *
*/
typedef struct{
    uint8_t         sync_word;
    uartHeader_t    header;
    uint8_t			rf_payld_code;
    uint32_t        ring_MAC;
    uint8_t         RFU_1;
    uint8_t         RFU_2;
    uint8_t         rx_rssi;
    uint8_t         crc;

}__attribute__((packed)) uart_pckt_1_t;


/**
 *
*/
typedef struct{
    uint8_t         sync_word;
    uartHeader_t    header;
    uint32_t        HB_MARK;
    uint32_t        RFU;
    uint8_t         crc;

}__attribute__((packed)) uart_pckt_254_t;

/**
*
*/
typedef union{
    uart_pckt_1_t       payload_1;
    uart_pckt_254_t     payload_254;
    uint8_t             raw_paxload[RING_PACKET_MAX_SIZE];
}uart_payloads_u;


uint8_t UP_CheckUartMsg(uint8_t *decodedPacket);
void UP_DecodePacket(uint8_t *rawPacket, uint8_t *output,uint8_t size);
void UP_InitUart(void);
uint8_t UP_CalcCRC(uint8_t *data, uint8_t size);
void UP_ResetRing(void);

#endif
#endif /* INC_UARTPROCESS_H_ */
