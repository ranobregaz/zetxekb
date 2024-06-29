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
#include "csp_common.h"
#include "csp_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

//apt user
//#define UART_RECV_DYNAMIC  			//uart receive dynamic length 

/*----- UART Control Codes: Mode Parameters: Data Bits -----*/
typedef enum {
    UART_DATA_BITS_5               = 0,    ///< 5 Data bits
    UART_DATA_BITS_6,                      ///< 6 Data bit
    UART_DATA_BITS_7,                      ///< 7 Data bits
    UART_DATA_BITS_8,                      ///< 8 Data bits (default)
    UART_DATA_BITS_9                       ///< 9 Data bits
} csi_uart_data_bits_t;

/*----- UART Control Codes: Mode Parameters: Parity -----*/
typedef enum {
    UART_PARITY_NONE               = 0,    ///< No Parity (default)
    UART_PARITY_EVEN,                      ///< Even Parity
    UART_PARITY_ODD,                       ///< Odd Parity
} csi_uart_parity_t;

/*----- UART Control Codes: Mode Parameters: Stop Bits -----*/
typedef enum {
    UART_STOP_BITS_1               = 0,    ///< 1 Stop bit (default)
    UART_STOP_BITS_2,                      ///< 2 Stop bits
    UART_STOP_BITS_1_5,                    ///< 1.5 Stop bits
} csi_uart_stop_bits_t;

/*----- UART Control Codes: Mode Parameters: Flow Control -----*/
typedef enum {
    UART_FLOWCTRL_NONE             = 0,    ///< none flowctrl
    UART_FLOWCTRL_RTS,                     ///< RTS
    UART_FLOWCTRL_CTS,                     ///< CTS
    UART_FLOWCTRL_RTS_CTS                  ///< RTS & CTS
} csi_uart_flowctrl_t;

/****** UART Event *****/
typedef enum {
    UART_EVENT_SEND_COMPLETE        = 0,        ///< Send data completed.
    UART_EVENT_RECEIVE_COMPLETE,                ///< Receive data completed.
    UART_EVENT_RECEIVE_FIFO_READABLE,           ///< Data in uart fifo, call csi_uart_receive() get the data.
	UART_EVENT_RECEIVE_DYNAMIC_COMPLETE,		///< Dynamic length data receive completed
    UART_ENENT_BREAK_INTR,                      ///< the serial input,sin, is held in a logic '0' state for longer than the sum of start time+data bits+parity+stop bits.
    UART_EVENT_ERROR_OVERFLOW,                  ///< A new data character was received before the previous data was read.
    UART_EVENT_ERROR_PARITY,                    ///< Occur parity error in the receiver.
    UART_EVENT_ERROR_FRAMING                    ///< the receiver does not detect a valid STOP bit in the received data.
} csi_uart_event_t;


typedef enum {
   UART_STOP=0,				///< uart dev stoped.
   UART_IDLE,				///< uart dev idle.
   UART_W,					///< uart dev send enable only.
   UART_R,					///< uart dev receive enable only.
   UART_RW,					///< uart dev both send and receive enabled.
   UART_ERR,				///< uart dev error occurred.
} csi_uart_state_t;

typedef enum {
	CSI_UART_OK          =  0,
    CSI_UART_ERROR       = -1,
    CSI_UART_BUSY        = -2,
    CSI_UART_TIMEOUT     = -3,
    CSI_UART_UNSUPPORTED = -4,
	CSI_UART_ERR_UART_UARTNO  	= -5,
	CSI_UART_ERR_UART_UARTBASE  = -6,
	CSI_UART_ERR_UART_UARTPARAM = -7,
	CSI_UART_ERR_UARTRXEMPTY  	= -8,
} csi_uart_error_t;

typedef struct csi_uart csi_uart_t;

struct csi_uart {
	csp_uart_t *		utbase;					///< baseaddr of uart peripheral.
	uint8_t				utNo;					///< the suffix number of uart peripheral.
	uint8_t				DataBits;               ///< databits setting .
	uint8_t				Parity;                 ///< parity setting .
	uint8_t				Stopbits;				///< stopbits setting .
	uint32_t			flowctrl;				///< flow control setting .
	uint32_t    		BaudRate;				///< baudrate setting .
	uint32_t			Interrupt;				///< Interrupt flag buffer.
	uint32_t			Event;					///< event flag buffer.	
	csp_uart_wkmode_t   wkmode;					///< wkmode setting for performance optimization in customer's cases.
    csi_uart_state_t	state;					///< dev current state flag.
	csi_uart_error_t 	errcode;				///< error flag buffer .
	const int   		manufacturerUID;		///< UID of this moudle driving code.
	ringbuffer_t *      ringbuf;				///< ringbuffer used in uart async transfer.    
	uint32_t    		readbacklen;			///< read back length 	
	void     			(*EventGen_callback)(csi_uart_t *uart);  ///< uart transfer event generation function.
	void     			(*Event_callback)(csi_uart_t *uart);     ///< handler of event.
	void *              eventArg;				
	void *				extparam;               ///< extparam control function .
	void *				extctrol;			    ///< extctrol function.
};

typedef struct csi_uart_devlist {
	csi_uart_t *ut[NUM_OF_UART];
}csi_uart_devlist_t;





extern csi_uart_devlist_t uartDevlist;

/**
  \brief       Initializes the resources needed for the UART interface.
  \param[in]   uart      operate handle.
  \param[in]   idx       the device idx.
  \return      error code.
*/
csi_error_t csi_uart_init(csi_uart_t *uart, uint32_t idx);
/**
  \brief       Open UART.
  \param[in]   uart       operate handle..
  \return      error code.
*/
csi_error_t        csi_uart_open(csi_uart_t *uart);

/**
  \brief       Close UART.
  \param[in]   uart       operate handle.
  \return      error code.
*/
csi_error_t        csi_uart_close(csi_uart_t *uart);


/**
  \brief       Start send data to UART transmitter, this function is blocking.
  \param[in]   uart     uart handle to operate.
  \param[in]   data     pointer to buffer with data to send to UART transmitter.
  \param[in]   size     number of data to send (byte).
  \param[in]   timeout  the timeout between bytes(ms).
  \return      the num of data which is sent successfully or CSI_ERROR.
*/
int32_t csi_uart_send(csi_uart_t *uart, const void *data, uint32_t size, uint32_t timeout);

/**
  \brief       Start send data to UART transmitter, this function is non-blocking.
  \param[in]   uart     uart handle to operate.
  \param[in]   data     pointer to buffer with data to send to UART transmitter.
  \param[in]   size     number of data to send (byte).
  \return      error code.
*/
csi_error_t csi_uart_send_async(csi_uart_t *uart, const void *data, uint32_t size);

/**
  \brief       Query data from UART receiver FIFO, this function is blocking.
  \param[in]   uart     uart handle to operate.
  \param[out]  data     pointer to buffer for data to receive from UART receiver.
  \param[in]   size     number of data to receive.
  \param[in]   timeout  the timeout between bytes(ms).
  \return      the num of data witch is received successfully or CSI_ERROR.
*/
int32_t csi_uart_receive(csi_uart_t *uart, void *data, uint32_t size, uint32_t timeout);

/**
  \brief       Start receiving data from UART receiver, this function is non-blocking.
  \param[in]   uart  uart handle to operate.
  \param[out]  data  pointer to buffer for data to receive from UART receiver.
  \param[in]   size  number of data to receive (byte).
  \return      error code.
*/
csi_error_t csi_uart_receive_async(csi_uart_t *uart, void *data, uint32_t size);

/** \brief uart get character
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] blocking: blocking mode swtich
 *  \return  the character to get
 */
uint8_t csi_uartmd_getc(csi_uart_t *uart,uint8_t blocking);

/** \brief uart send character
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] ch: the character to be send
 *  \param[in] blocking: blocking mode swtich
 *  \return  none
 */
void csi_uartmd_putc(csi_uart_t *uart, uint8_t ch,uint8_t blocking);

/**
  \brief       Get character in query mode.
  \param[in]   uart  uart handle to operate.
  \return      the character to get.
*/
uint8_t  csi_uart_getc(csi_uart_t *uart);

/**
  \brief       Send character in query mode.
  \param[in]   uart uart handle to operate.
  \param[in]   ch   the character to be send.
*/
void csi_uart_putc(csi_uart_t *uart, uint8_t ch);



/**
  \brief       Get the state of uart device.
  \param[in]   uart   uart handle to operate.
  \param[out]  state  the state of uart device.
  \return      error code.
*/
csi_error_t csi_uart_get_state(csi_uart_t *uart, csi_uart_state_t *state);

/**
  \brief       Enable uart power manage.
  \param[in]   uart   uart handle to operate.
  \param[in]   param  pointer to param for power management.
  \return      error code.
*/
csi_error_t csi_uart_pm(csi_uart_t *uart, uint8_t *param);

/**
  \brief       Enable uart power manage.
  \param[in]   uart   uart handle to operate.
  \param[in]   param  pointer to param for performance control.
  \return      error code.
*/
csi_error_t csi_uart_performance_control(csi_uart_t *uart, uint8_t *param);


/** \brief get uart interrupt isr value
 * 
 *  \param[in] UART handle to operate
 *  \return uart isr reg value
 */ 
uint32_t csi_uart_get_isr(csi_uart_t *uart);

/** \brief clr uart interrupt isr flag
 * 
 *  \param[in] UART handle to operate
 *  \param[in] UART isr bits
 *  \return none
 */
 void csi_uart_clr_isr(csi_uart_t *uart,uint32_t value);

/** \brief uart interrupt event genneration
 * 
 *  \param[in] UART handle to operate
 *  \return none
 */ 

 void csi_uart_irqhandler(uint8_t devno);

/**
  \brief       Attach the interrupt callback handler to UART.
  \param[in]   uart       operate handle.
  \param[in]   evtcreat   event creat function.
  \param[in]   evtcallback   event callback function.
  \param[in]   arg        user can define it by himself as callback's param.
  \return      error code.
*/
csi_error_t csi_uart_attach_int_callback(csi_uart_t *uart, void *evtcreat, void *evtcallback,void *arg);

/**
  \brief       Attach the callback handler to UART.
  \param[in]   uart       operate handle.
  \param[in]   callback   callback function.
  \param[in]   arg        user can define it by himself as callback's param.
  \return      error code.
*/

/** \brief uart interrupt event generation 
 * 
 *  \param[in] UART handle to operate
 *  \return none
 */ 
void csi_uart_creat_event_default(csi_uart_t *uart);


/** \brief uart interrupt event handler default
 * 
 *  \param[in] UART handle to operate
 *  \return none
 */ 
void csi_uart_handle_event_default(csi_uart_t *uart, csi_uart_event_t event);


/** \brief de-initialize uart interface
 * 
 *  \param[in] uart: UART handle to operate
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_uart_uninit(csi_uart_t *uart);


#ifdef __cplusplus
}
#endif

#endif /* _DRV_UART_H_ */
