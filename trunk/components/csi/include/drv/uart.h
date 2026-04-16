/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     uart.h
 * @brief    header file for uart driver
 * @version  V1.0
 * @date     08. Apr 2020
 * @model    uart
 ******************************************************************************/

#ifndef _DRV_UART_H_
#define _DRV_UART_H_

#include <drv/common.h>
#include <drv/dma.h>
#include <drv/ringbuffer.h>

#include "csp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UART_IDX_NUM   			3
#define UART_RECV_MAX_LEN		256


/**
 * \enum     csi_uart_data_bits_e
 * \brief    UART Mode Parameters: Data Bits
 */
typedef enum {
    UART_DATA_BITS_5	= 0,			///< 5 Data bits
    UART_DATA_BITS_6,                   ///< 6 Data bit
    UART_DATA_BITS_7,                   ///< 7 Data bits
    UART_DATA_BITS_8,                   ///< 8 Data bits (default)
    UART_DATA_BITS_9                    ///< 9 Data bits
} csi_uart_data_bits_e;

/**
 * \enum     csi_uart_parity_e
 * \brief    UART Mode Parameters: Parity
 */
typedef enum {
    UART_PARITY_NONE	= 0,    		///< No Parity (default)
    UART_PARITY_EVEN,                   ///< Even Parity
    UART_PARITY_ODD,                    ///< Odd Parity
} csi_uart_parity_e;

/**
 * \enum     csi_uart_stop_bits_e
 * \brief    UART Mode Parameters: Stop bits
 */
typedef enum {
    UART_STOP_BITS_1	= 0,    		///< 1 Stop bit (default)
    UART_STOP_BITS_2,                   ///< 2 Stop bits
    UART_STOP_BITS_1_5,                 ///< 1.5 Stop bits
} csi_uart_stop_bits_e;

/**
 * \enum     csi_uart_flowctrl_e
 * \brief    UART Mode Parameters:  Flow Control
 */
typedef enum {
    UART_FLOWCTRL_NONE	= 0,    		///< none flowctrl
    UART_FLOWCTRL_RTS,                  ///< RTS
    UART_FLOWCTRL_CTS,                  ///< CTS
    UART_FLOWCTRL_RTS_CTS               ///< RTS & CTS
} csi_uart_flowctrl_e;

/**
 * \enum     csi_uart_state_e
 * \brief    UART working status
 */
typedef enum {
	UART_STATE_IDLE		= 0,			///< uart idle(rx/tx)
	UART_STATE_RECV,					///< uart receiving 
	UART_STATE_SEND,					///< uart sending 
	UART_STATE_FULL,					///< uart receive complete(full)
	UART_STATE_DONE						///< uart send complete
} csi_uart_state_e;

//typedef enum {
//	CSI_UART_OK          =  0,
//    CSI_UART_ERROR       = -1,
//    CSI_UART_BUSY        = -2,
//    CSI_UART_TIMEOUT     = -3,
//    CSI_UART_UNSUPPORTED = -4,
//	CSI_UART_ERR_UART_UARTNO  	= -5,
//	CSI_UART_ERR_UART_UARTBASE  = -6,
//	CSI_UART_ERR_UART_UARTPARAM = -7,
//	CSI_UART_ERR_UARTRXEMPTY  	= -8,
//} csi_uart_error_t;

/**
 * \enum     csi_uart_intsrc_e
 * \brief    UART interrupt source 
 */
typedef enum
{
	UART_INTSRC_NONE 		= (0x00ul << 0),		//UART none interrupt
	UART_INTSRC_TX   		= (0x01ul << 2), 		//TX interrupt
	UART_INTSRC_RX  		= (0x01ul << 3),		//RX interrupt
	UART_INTSRC_TX_OV   	= (0x01ul << 4),		//TX OVER interrupt
	UART_INTSRC_RX_OV   	= (0x01ul << 5),		//RX OVER interrupt
	UART_INTSRC_PAR_ERR		= (0x01ul << 7),		//PARITY ERROR interrupt
	UART_INTSRC_TXFIFO  	= (0x01ul << 12),		//TX FIFO interrupt
	UART_INTSRC_RXFIFO  	= (0x01ul << 13),		//RX FIFO interrupt
	UART_INTSRC_RXFIFO_OV	= (0x01ul << 18),		//RX FIFO OVER interrupt   
	UART_INTSRC_TXDONE   	= (0x01ul << 19) 		//TX DONE interrupt
}csi_uart_intsrc_e;


typedef struct {
	uint8_t				byDataBits;         //databits setting 
	uint8_t				byStopBits;			//stopbits setting 	
	uint8_t				byFlowCtrl;			//flowctrl
	uint8_t				byParity;           //parity type 
	uint32_t            wBaudRate;			//baud rate	
	uint32_t            wInter;				//interrupt
} csi_uart_config_t;

typedef struct {
    uint8_t				*pbyTxData;			//pointer of send buf 
	uint16_t            hwTxSize;			//tx send data size
	uint8_t				bySendStat;
	uint8_t				byRecvStat;
	ringbuffer_t		*ptRingBuf;			
} csi_uart_transfer_t;

extern csi_uart_transfer_t g_tUartTran[UART_IDX_NUM];	

/**
  \brief       Initializes the resources needed for the UART interface.
  \param[in]   uart      operate handle.
  \param[in]   idx       the device idx.
  \return      error code.
*/
csi_error_t csi_uart_init(csp_uart_t *ptUartBase, csi_uart_config_t *ptUartCfg);

/** 
  \brief start(enable) uart rx/tx
  \param[in] ptUartBase: pointer of uart register structure
  \return error code \ref csi_error_t
 */ 
csi_error_t csi_uart_start(csp_uart_t *ptUartBase);

/** 
  \brief stop(disable) uart rx/tx
  \param[in] ptUartBase: pointer of uart register structure
  \return error code \ref csi_error_t
 */ 
csi_error_t csi_uart_stop(csp_uart_t *ptUartBase);

/**
  \brief       Start send data to UART transmitter, this function is blocking.
  \param[in]   uart     uart handle to operate.
  \param[in]   data     pointer to buffer with data to send to UART transmitter.
  \param[in]   size     number of data to send (byte).
  \param[in]   timeout  the timeout between bytes(ms).
  \return      the num of data which is sent successfully or CSI_ERROR.
*/
int32_t csi_uart_send(csp_uart_t *ptUartBase, const void *pData, uint32_t wSize, uint32_t wTimeOut);

/** 
  \brief send data to uart transmitter, this function is interrupt mode(async/non-blocking)
  \param[in] ptUartBase: pointer of uart register structure
  \param[in] pData: pointer to buffer with data to send to uart transmitter.
  \param[in] wSize: number of data to send (byte).
  \return  error code \ref csi_error_t
 */
csi_error_t csi_uart_send_intr(csp_uart_t *ptUartBase, const void *pData, uint32_t wSize);


/**
  \brief       Query data from UART receiver FIFO, this function is blocking.
  \param[in]   uart     uart handle to operate.
  \param[out]  data     pointer to buffer for data to receive from UART receiver.
  \param[in]   size     number of data to receive.
  \param[in]   timeout  the timeout between bytes(ms).
  \return      the num of data witch is received successfully or CSI_ERROR.
*/
int32_t csi_uart_receive(csp_uart_t *ptUartBase, void *pData, uint32_t wSize, uint32_t wTimeOut);

/** 
  \brief receive data to uart transmitter; this function is interrupt mode(async/no-blocking),
  \param[in] ptUartBase: UART handle to operate
  \param[in] pData: pointer to buffer with data to be received.
  \param[in] wSize: number of data to receive (byte).
  \return  the num of data which is receive successfully
 */
int32_t csi_uart_recv_intr(csp_uart_t *ptUartBase, void *pData, uint32_t wSize);

/** 
  \brief receive data to uart transmitter, dynamic receive; this function is interrupt mode(async).
  \param[in] ptUartBase: UART handle to operate
  \param[in] pData: pointer to buffer with data to be received.
  \param[in] wTimeOut: the timeout between bytes(ms). 
  \return  the num of data which is send successfully
 */
int32_t csi_uart_recv_dynamic(csp_uart_t *ptUartBase, void *pData);

/**
  \brief       Get character in query mode.
  \param[in]   uart  uart handle to operate.
  \return      the character to get.
*/
uint8_t csi_uart_getc(csp_uart_t *ptUartBase);

/**
  \brief       Send character in query mode.
  \param[in]   uart uart handle to operate.
  \param[in]   ch   the character to be send.
*/
void csi_uart_putc(csp_uart_t *ptUartBase, uint8_t byData);

/**
  \brief       Link DMA channel to uart device.
  \param[in]   uart     uart handle to operate.
  \param[in]   tx_dma   the DMA channel handle for send, when it is NULL means to unlink the channel.
  \param[in]   rx_dma   the DMA channel handle for receive, when it is NULL means to unlink the channel.
  \return      error code.
*/
//csi_error_t csi_uart_link_dma(csi_uart_t *uart, csi_dma_ch_t *tx_dma, csi_dma_ch_t *rx_dma);

/**
  \brief       get the state of uart receive.
  \param[in]   uart   uart handle to operate.
  \return      error code.
*/
csi_uart_state_e csi_uart_get_recv_status(csp_uart_t *ptUartBase);

/**
  \brief       get the state of uart send.
  \param[in]   uart   uart handle to operate.
  \return      error code.
*/
csi_uart_state_e csi_uart_get_send_status(csp_uart_t *ptUartBase);

/**
  \brief       clr the state of uart receive.
  \param[in]   uart   uart handle to operate.
  \return      error code.
*/
void csi_uart_clr_recv_status(csp_uart_t *ptUartBase);

/**
  \brief       clr the state of uart send.
  \param[in]   uart   uart handle to operate.
  \return      error code.
*/
void csi_uart_clr_send_status(csp_uart_t *ptUartBase);

/**
  \brief       Enable uart power manage.
  \param[in]   uart   uart handle to operate.
  \return      error code.
*/
//csi_error_t csi_uart_enable_pm(csi_uart_t *uart);

/**
  \brief       Disable uart power manage.
  \param[in]   uart   uart handle to operate.
*/
//void csi_uart_disable_pm(csi_uart_t *uart);


#ifdef __cplusplus
}
#endif

#endif /* _DRV_UART_H_ */
