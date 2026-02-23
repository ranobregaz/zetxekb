/***********************************************************************//** 
 * \file  uart_demo.c
 * \brief  UART_DEMO description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-28 <td>V0.0 <td>ZJY     <td>initial
 * </table>
 * *********************************************************************
*/
/* Includes ---------------------------------------------------------------*/
#include <string.h>
#include <drv/uart.h>
#include <drv/pin.h>

#include "demo.h"
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private macro-----------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/** \brief uart char receive and send 
 * 
 *  \param[in] none
 *  \return error code
 */
int uart_char_demo(void)
{
	int iRet = 0;
	volatile uint8_t byRecv;
	csi_uart_config_t tUartConfig;				//UART1 参数配置结构体
	
	csi_pin_set_mux(PA013, PA013_UART1_RX);		//UART1 RX管脚配置
	csi_pin_set_mux(PB00, PB00_UART1_TX);		//UART1 TX管脚配置
	csi_pin_pull_mode(PA013,GPIO_PULLUP);		//RX管脚上拉使能, 建议配置
	
	tUartConfig.byDataBits = UART_DATA_BITS_8;	//数据位，8位
	tUartConfig.byStopBits = UART_STOP_BITS_1;	//停止位，1位
	tUartConfig.byParity = UART_PARITY_ODD;		//校验位，奇校验
	tUartConfig.wBaudRate = 115200;				//波特率，115200
	tUartConfig.wInter = UART_INTSRC_NONE;		//无中断
	
	csi_uart_init(UART1, &tUartConfig);			//初始化串口
	csi_uart_start(UART1);						//打开串口
	
	while(1)
	{
		byRecv = csi_uart_getc(UART1);
		csi_uart_putc(UART1, byRecv+1);
		
		mdelay(10);
	}
	
	return iRet;
}


/** \brief uart send a bunch of data; polling(sync,no interrupt)mode
 * 
 *  \param[in] none
 *  \return error code
 */
int uart_send_demo(void)
{
	int iRet = 0;
	uint8_t bySendData[20]={1,2,3,4,5,6,7,8,9,21,22,23,24,25,26,27,28,29};
	volatile uint8_t byRecv;
	csi_uart_config_t tUartConfig;				//UART1 参数配置结构体
	
	csi_pin_set_mux(PA013, PA013_UART1_RX);		//UART1 RX管脚配置
	csi_pin_set_mux(PB00, PB00_UART1_TX);		//UART1 TX管脚配置
	csi_pin_pull_mode(PA013,GPIO_PULLUP);		//RX管脚上拉使能, 建议配置
	
	tUartConfig.byDataBits = UART_DATA_BITS_8;	//数据位，8位
	tUartConfig.byStopBits = UART_STOP_BITS_1;	//停止位，1位
	tUartConfig.byParity = UART_PARITY_ODD;		//校验位，奇校验
	tUartConfig.wBaudRate = 115200;				//波特率，115200
	tUartConfig.wInter = UART_INTSRC_NONE;		//发送不使用中断，轮询(同步)方式
	
	csi_uart_init(UART1, &tUartConfig);			//初始化串口
	csi_uart_start(UART1);

	while(1)
	{
		byRecv = csi_uart_getc(UART1);
		if(byRecv == 0x06)
			csi_uart_send(UART1,(void *)bySendData,16,0);		//采用轮询方式
		//mdelay(10);
	}
	
	return iRet;
}

/** \brief uart send a bunch of data; interrupt(nsync) mode
 * 
 *  \param[in] none
 *  \return error code
 */
int uart_send_intr_demo(void)
{
	int iRet = 0;
	uint8_t bySendData[20]={1,2,3,4,5,6,7,8,9,21,22,23,24,25,26,27,28,29};
	volatile uint8_t byRecv;
	csi_uart_config_t tUartConfig;				//UART1 参数配置结构体
	
	csi_pin_set_mux(PA013, PA013_UART1_RX);		//UART1 RX管脚配置
	csi_pin_set_mux(PB00, PB00_UART1_TX);		//UART1 TX管脚配置
	csi_pin_pull_mode(PA013,GPIO_PULLUP);		//RX管脚上拉使能, 建议配置
	
	tUartConfig.byDataBits = UART_DATA_BITS_8;	//数据位，8位
	tUartConfig.byStopBits = UART_STOP_BITS_1;	//停止位，1位
	tUartConfig.byParity = UART_PARITY_ODD;		//校验位，奇校验
	tUartConfig.wBaudRate = 115200;				//波特率，115200
	tUartConfig.wInter = UART_INTSRC_TXDONE;	//发送使用中断方式，采用(发送完成)TXDONE中断
	
	csi_uart_init(UART1, &tUartConfig);			//初始化串口
	csi_uart_start(UART1);

	while(1)
	{
		byRecv = csi_uart_getc(UART1);
		if(byRecv == 0x06)
			csi_uart_send_intr(UART1,(void *)bySendData,16);		//采用中断方式
		//mdelay(10);
		
		
		while(1)			
		{
			//如果有需要，可用于判断发送是否完成；
			if(csi_uart_get_send_state(UART1) == UART_STATE_DONE)	//判断是否发送完成
			{
				
				//发送状态有三种，IDLE(空闲)/SEND(发送中)/DONE(发送完成)
				//具体定义参考：uart.h中csi_uart_state_e,
				csi_uart_clr_send_state(UART1);						//清除发送状态位，状态设置为空闲(idle)
				mdelay(10);
				break;
			}
		}
		
		//TODO
		mdelay(10);
	}
	
	return iRet;
}