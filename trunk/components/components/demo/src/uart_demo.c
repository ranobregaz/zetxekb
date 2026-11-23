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

ringbuffer_t g_tRingbuf;						//循环buffer
uint8_t 	 g_byRxBuf[UART_RECV_MAX_LEN];		//接收缓存

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
	
	tUartConfig.byParity = UART_PARITY_ODD;		//校验位，奇校验
	tUartConfig.wBaudRate = 115200;				//波特率，115200
	tUartConfig.wInter = UART_INTSRC_NONE;		//无中断
	tUartConfig.byTxMode = UART_TX_MODE_POLL;	//发送 轮询模式
	tUartConfig.byRxMode = UART_RX_MODE_POLL;	//接收 轮询模式
	
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
	
	tUartConfig.byParity = UART_PARITY_ODD;		//校验位，奇校验
	tUartConfig.wBaudRate = 115200;				//波特率，115200
	tUartConfig.wInter = UART_INTSRC_NONE;		//UART中断关闭，轮询(同步)方式
	tUartConfig.byTxMode = UART_TX_MODE_POLL;	//发送 轮询模式
	tUartConfig.byRxMode = UART_RX_MODE_POLL;	//接收 轮询模式
	
	csi_uart_init(UART1, &tUartConfig);			//初始化串口
	csi_uart_start(UART1);

	while(1)
	{
		byRecv = csi_uart_getc(UART1);
		if(byRecv == 0x06)
			byRecv = csi_uart_send(UART1,(void *)bySendData,16);		//采用轮询方式,调用该函数时，UART发送中断关闭
		
		mdelay(5);
		if(byRecv == 16)
			csi_uart_putc(UART1, 0x03);
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
	
	tUartConfig.byParity = UART_PARITY_ODD;		//校验位，奇校验
	tUartConfig.wBaudRate = 115200;				//波特率，115200
	tUartConfig.wInter = UART_INTSRC_TXDONE;	//UART发送中断使能，采用(发送完成)TXDONE中断
	tUartConfig.byTxMode = UART_TX_MODE_INT;	//发送 中断模式
	tUartConfig.byRxMode = UART_RX_MODE_POLL;	//接收 轮询模式
	
	csi_uart_init(UART1, &tUartConfig);			//初始化串口
	csi_uart_start(UART1);

	while(1)
	{
		byRecv = csi_uart_getc(UART1);
		if(byRecv == 0x06)
			csi_uart_send(UART1,(void *)bySendData,16);		//采用中断方式。调用改函数时，UART发送中断使能
		
		while(1)			
		{
			//如果有需要，可用于判断发送是否完成；
			if(csi_uart_get_send_status(UART1) == UART_STATE_DONE)	//判断是否发送完成
			{
				
				//发送状态有三种，IDLE(空闲)/SEND(发送中)/DONE(发送完成)
				//具体定义参考：uart.h中csi_uart_state_e,
				csi_uart_clr_send_status(UART1);						//清除发送状态位，状态设置为空闲(idle)
				mdelay(10);
				break;
			}
		}
		
		//TODO
		mdelay(10);
	}
	
	return iRet;
}

/** \brief uart receive a bunch of data; polling(sync) mode
 * 
 *  \param[in] none
 *  \return error code
 */
int uart_receive_demo(void)
{
	int iRet = 0;
	//uint8_t bySendData[20]={1,2,3,4,5,6,7,8,9,21,22,23,24,25,26,27,28,29};
	uint8_t byRecvData[20]={0};
	volatile uint8_t byRecv;
	csi_uart_config_t tUartConfig;				//UART1 参数配置结构体
	
	csi_pin_set_mux(PA013, PA013_UART1_RX);		//UART1 RX管脚配置
	csi_pin_set_mux(PB00, PB00_UART1_TX);		//UART1 TX管脚配置
	csi_pin_pull_mode(PA013,GPIO_PULLUP);		//RX管脚上拉使能, 建议配置
	
	tUartConfig.byParity = UART_PARITY_ODD;		//校验位，奇校验
	tUartConfig.wBaudRate = 115200;				//波特率，115200
	tUartConfig.wInter = UART_INTSRC_NONE;		//串口中断关闭
	
	tUartConfig.byTxMode = UART_TX_MODE_POLL;	//发送 轮询模式
	tUartConfig.byRxMode = UART_RX_MODE_POLL;	//接收 轮询模式
	
	csi_uart_init(UART1, &tUartConfig);			//初始化串口
	csi_uart_start(UART1);
	
	while(1)
	{
		byRecv = csi_uart_receive(UART1,byRecvData,16,2000);			//UART接收采用轮询方式(同步)	
		if(byRecv  == 16)
			csi_uart_send(UART1,(void *)byRecvData,byRecv);		//UART发送采用轮询方式(同步)	
	}
	
	return iRet;
}

/** \brief uart receive assign(fixed) length data; interrupt(async) mode
 *  \brief 串口接收指定长度数据，RX使用中断
 * 
 *  \param[in] none
 *  \return error code
 */

int uart_recv_intr_demo(void)
{
	int iRet = 0;
	uint8_t byRxBuf[32];
	
	volatile uint16_t hwRecvLen;
	
	csi_uart_config_t tUartConfig;				//UART1 参数配置结构体
	
	csi_pin_set_mux(PA013, PA013_UART1_RX);		//UART1 RX管脚配置
	csi_pin_set_mux(PB00, PB00_UART1_TX);		//UART1 TX管脚配置
	csi_pin_pull_mode(PA013,GPIO_PULLUP);		//RX管脚上拉使能, 建议配置
	
	//接收缓存配置
	//实例化g_tRingbuf 结构体变量
	//UARTx ptRingBuf 指针初始化
	g_tRingbuf.pbyBuf = g_byRxBuf;				//循环buffer 缓存区地址指针赋值，指向g_byRxBuf
	g_tRingbuf.hwSize = sizeof(g_byRxBuf);		//获取循环buffer存储空间
	g_tUartTran[1].ptRingBuf = &g_tRingbuf;		//UART1循环buffer指针赋值 	
	ringbuffer_reset(g_tUartTran[1].ptRingBuf);	//初始化循环buffer
	
	tUartConfig.byParity = UART_PARITY_ODD;		//校验位，奇校验
	tUartConfig.wBaudRate = 115200;				//波特率，115200
	tUartConfig.wInter = UART_INTSRC_RXFIFO;	//串口接收中断打开，使用RXFIFO中断
	tUartConfig.byTxMode = UART_TX_MODE_POLL;	//发送模式：轮询模式
	tUartConfig.byRxMode = UART_RX_MODE_INT_FIX;//接收模式：中断指定接收模式
	
	csi_uart_init(UART1, &tUartConfig);			//初始化串口
	csi_uart_start(UART1);
	
	while(1)
	{
		//从串口缓存（UART接收循环buffer）里面读取数据，返回读取数据个数
		//用户应用根据实际不同协议来处理数据
		hwRecvLen = csi_uart_receive(UART1,(void *)byRxBuf, 13,0);	//读取接收循环buffer数据
		if(hwRecvLen)
			csi_uart_send(UART1,(void *)byRxBuf,hwRecvLen);		//UART发送采用轮询方式(同步)
	}
	
	return iRet;
}


/** \brief uart receive a bunch of data; interrupt(async) mode
 *  \brief 串口接收到一串字符串,RX使用中断模式
 *
 *  \param[in] none
 *  \return error code
 */
int uart_recv_dynamic_demo(void)
{
	int iRet = 0;
	uint8_t byRxBuf[128];
	
	volatile uint16_t hwRecvLen;
	
	csi_uart_config_t tUartConfig;				//UART1 参数配置结构体
	
	csi_pin_set_mux(PA013, PA013_UART1_RX);		//UART1 RX管脚配置
	csi_pin_set_mux(PB00, PB00_UART1_TX);		//UART1 TX管脚配置
	csi_pin_pull_mode(PA013,GPIO_PULLUP);		//RX管脚上拉使能, 建议配置
	
	//接收缓存配置
	//实例化g_tRingbuf 结构体变量
	g_tRingbuf.pbyBuf = g_byRxBuf;				//循环buffer 缓存区地址指针赋值，指向g_byRxBuf
	g_tRingbuf.hwSize = sizeof(g_byRxBuf);		//获取循环buffer存储空间
	//UARTx ptRingBuf 指针初始化
	g_tUartTran[1].ptRingBuf = &g_tRingbuf;		//UART1循环buffer指针赋值 	
	ringbuffer_reset(g_tUartTran[1].ptRingBuf);	//复位(初始化)循环buffer参数
	
	//串口参数配置
	tUartConfig.byParity = UART_PARITY_ODD;		//校验位，奇校验
	tUartConfig.wBaudRate = 115200;				//波特率，115200
	tUartConfig.wInter = UART_INTSRC_RXFIFO;	//串口接收中断打开，使用RXFIFO中断(默认推荐使用)
	tUartConfig.byTxMode = UART_TX_MODE_POLL;	//发送模式：轮询模式
	tUartConfig.byRxMode = UART_RX_MODE_INT_DYN;//接收模式：中断动态接收模式
	
	csi_uart_init(UART1, &tUartConfig);			//初始化串口
	csi_uart_start(UART1);
	
	while(1)
	{
		if(csi_uart_get_recv_status(UART1) == UART_STATE_DONE)			//获取串口接收状态，串口接收到一串字符
		{
			csi_uart_clr_recv_status(UART1);							//清除串口接收状态，设置为空闲
			hwRecvLen = csi_uart_receive(UART1,(void*)byRxBuf,0,0);		//获取接收到的一串数据，返回数据长度, 后面两个参数无意义
			csi_uart_send(UART1,(void *)byRxBuf,hwRecvLen);				//UART发送采用轮询方式(同步)
		}
		
	}
	
	return iRet;
}