/***********************************************************************//** 
 * \file  csp_common.h
 * \brief  define error code and some macros
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-4 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/
 
#ifndef _CSP_COMMON_H_
#define _CSP_COMMON_H_

#include <stdint.h>

typedef enum {
    CSP_SUCCESS     =  0,
    CSP_FAIL        = -1,
    CSP_UNSUPPORTED = -4
} csp_error_t;

#define TRUE  		1
#define FALSE 		0
#define ENABLE  	1
#define DISABLE 	0 

#define nop			asm ("nop")

typedef enum {
   UART_WK_MD0=0,
   UART_WK_MD1,
   UART_WK_MD2,
   UART_WK_MD3,
   UART_WK_MD4,
} csp_uart_wkmode_t;

/****** UART Interrupt Flag *****/
typedef enum{
	UART_TX_INT_FLG   	= (0x1<<0),	//Transmitter INT Status      
	UART_RX_INT_FLG		= (0x1<<1),	//Receiver INT Status          
	UART_TX_IOV_FLG     = (0x1<<2),	//Transmitter Over INT Status  
	UART_RX_IOV_FLG     = (0x1<<3),	//Receiver Over INT Status   
	UART_PAR_ERR_FLG    = (0x1<<4),	//Parity Error Status  
	UART_TX_FIFO_FLG    = (0x1<<5),	//Transmitter FIFO INT Status      
	UART_RX_FIFO_FLG    = (0x1<<6),	//Receiver FIFO INT Status 
	UART_RXFIFO_IOV_FLG = (0x1<<7),	//Receiver FIFO Over INT Status 
	UART_TX_DONE_FLG    = (0x1<<19),	//Transmitter Complete INT Status
}uart_isrflg_e;

#endif //_CSP_COMMON_H_