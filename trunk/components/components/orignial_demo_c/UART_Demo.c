/***********************************************************************//** 
 * \file  UART_Demo.c
 * \brief  说明UART的用法
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-24 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/


/* include ----------------------------------------------------------------*/
#include "uart.h"
#include "board_config.h"
#include "iostring.h"

/* externs function--------------------------------------------------------*/
/* private function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/
extern csi_uart_t console_uart;		//如果不使用board_config.h中定义的console UART，请参考console_init()中的示例初始化uart
/* Private variablesr------------------------------------------------------*/
uint8_t byTxData[3] = {0x1, 0x2, 0x3};
uint8_t byRecvBuf[3] = {0};
/* device instance------------------------------------------------------*/

/**
  \brief       UART示例0，串口打印调试信息。
	*		   
  \return      csi_error_t
*/
void UART_senario0(void)
{	
	
	
	//管脚和UART参数设置 已经在board_init()中完成了
		
	while(1) {
		my_printf("HELLO: %x : %x : %x \n", byTxData[0], byTxData[1], byTxData[2]);  //如果工程设置compiler tab下define DBG_PRINT2PC=1，会通过硬件串口打印；否则打印在debug(print)
	}
}

/**
  \brief       UART示例1，串口通信发送
  \return      csi_error_t
*/
void UART_senario1(void)
{
	//管脚和UART参数设置 已经在board_init()中完成了
	
	csi_uart_send(&console_uart, (const void *)byTxData, sizeof(byTxData), 0);	//发送
}


/**
  \brief       UART示例2，串口通信接收
	*		   建议使用状态机
  \return      csi_error_t
*/
void UART_senario2(void)
{
	uint8_t byRecNum = 0;
	//管脚和UART参数设置 已经在board_init()中完成了
	
	csi_uart_send(&console_uart, (const void *)byRecvBuf, sizeof(byRecvBuf), 0);
	//发送后，操作串口软件，把接收到的数据回发出来
	do{
			byRecNum = csi_uart_receive(&console_uart, (void *)byRecvBuf, sizeof(byRecvBuf), 0);
			if(byRecNum == 16)
				csi_uart_send(&console_uart, (const void *)byRecvBuf, sizeof(byRecvBuf), 0);
		
		}while(1);
}