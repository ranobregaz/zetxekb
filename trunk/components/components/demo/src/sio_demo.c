/***********************************************************************//** 
 * \file  sio_demo.c
 * \brief  SIO_DEMO description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-11 <td>V0.0 <td>ZJY     <td>initial
 * </table>
 * *********************************************************************
*/
/* Includes ---------------------------------------------------------------*/
#include <string.h>
#include <drv/sio.h>
#include <drv/pin.h>
#include <drv/uart.h>
#include <drv/tick.h>
#include <iostring.h>


#include "demo.h"
/* Private macro-----------------------------------------------------------*/
#define		HDQ_WR_CMD		(0x01 << 7)
/* externs function--------------------------------------------------------*/
// rgb led display
void led_rgb_display(uint8_t *byColData, uint16_t hwLedNum);

//ti hdq transfer
uint32_t sio_hdq_addr_conver(uint8_t byAddr);
uint32_t sio_data_conver(uint8_t byTxData);

/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

uint8_t	byDipData[24] = 
{
	//R		G		B
	0x0a,	0xdc,	0x04,
	0x94, 	0xf5, 	0x29,
	0xcd,	0xcd,	0x00,
	0xff,	0x00,	0x00,
	0xf5,	0x29,	0xe2,
	0x00,	0x00,	0xff,
	0x24,	0xa5,	0xb7,
	0x56,	0x12,	0x53
};

uint32_t	g_wSioRxBuf[24];		//接收缓存

/** \brief sio rgb led demo
 *  \brief sio 驱动RGB LED(ws2812), RGB DATA = 24bit; 驱动数据输出排列方式:GRB
 * 
 *  \param[in] none
 *  \return error code
 */
int sio_led_rgb_demo(void)
{
	int iRet = 0;
	volatile uint8_t byRecv;
	csi_sio_tx_config_t tSioTxCfg;

	
	csi_pin_set_mux(PA012, PA012_SIO);			//PA0.12 配置为SIO模式		
	
	//SIO TX 参数配置
	tSioTxCfg.byD0Len 		= 1;				//D0 对象序列长度(bit个数)，这里不用D0								
	tSioTxCfg.byD1Len 		= 1;				//D1 对象序列长度(bit个数)，这里不用D1
	tSioTxCfg.byDLLen 		= 4;				//DL 对象序列长度(bit个数)
	tSioTxCfg.byDHLen 		= 4;				//DH 对象序列长度(bit个数)
	tSioTxCfg.byDLLsq 		= 0x01;				//DL 对象序列具体定义: bit= 0  -     
												//					          | |___|					
	tSioTxCfg.byDHHsq 		= 0x07;				//DH 对象序列具体定义: bit= 1  ___
												//					          |   |_| 	
	tSioTxCfg.byTxBufLen 	= 8;				//发送数据缓存长度(bit个数 = 8)，txbuf 一次发送bit数量，len <= 16
	tSioTxCfg.byTxCnt 		= 24;				//SIO一次发送总的数据长度(bit个数 = 24)，byTxCnt >= byTxBufLen，byTxCnt < 256(最大32bytes)
	tSioTxCfg.byIdleLev 	= SIO_IDLE_L;		//SIO空闲时刻IO管脚输出电平
	tSioTxCfg.byTxDir 		= SIO_TXDIR_LSB;	//MSB->LSB, txbuf 数据按照bit[1:0]...[31:30]方式移出
	tSioTxCfg.wTxFreq 		= 4000000;			//tx clk =4MHz, Ttxshift = 1/4 = 250ns；发送每bit时间是250ns
	tSioTxCfg.byInter		= SIO_INTSRC_NONE;	//不使用中断。目前只支持非中断模式
	
	csi_sio_tx_init(SIO0, &tSioTxCfg);
	
	while(1)
	{
		byRecv = csi_uart_getc(UART1);
		if(byRecv == 0x06)
		{
			led_rgb_display(byDipData, 8);
		}
		nop;
	}
	
	return iRet;
}
/** \brief sio rgb led recv demo
 *  \brief sio 接收RGB LED驱动数据，采用RXFULL中断模式；每收到byRxBufLen个bit，产生中断
 * 
 *  \param[in] none
 *  \return error code
 */

int sio_led_rgb_recv_rxfull_demo(void)
{
	int iRet = 0;
	csi_sio_rx_config_t tSioRxCfg;
	uint32_t wLedRxBuf[24];
	
	csi_pin_set_mux(PA012, PA012_SIO);				//PA0.12 配置为SIO模式	
	
	//SIO RX 参数配置
	tSioRxCfg.byDebPerLen 	= 3;					//接收滤波周期	
	tSioRxCfg.byDebClkDiv 	= 2;					//接收滤波时钟分频
	tSioRxCfg.byTrgEdge 	= SIO_TRG_RISE;			//接收采样触发边沿，上升沿
	tSioRxCfg.byTrgMode		= SIO_TRGMD_DEB;		//接收采样触发模式，去抖后
	tSioRxCfg.byRxDir 	 	= SIO_RXDIR_LSB;		//接收数据方向， 数据按照bit0...31方式移入，根据不同应用移入数据
	tSioRxCfg.bySpMode		= SIO_SPMD_EDGE_EN;		//接收数据采样边沿对齐使能	
	tSioRxCfg.bySpExtra		= SIO_EXTRACT_HI;		//采样提取策略，(20H)HITHR; (BIT = 1)个数 > HITHR 提取H,否则提取L
	tSioRxCfg.byHithr		= 4;					//提取判定值, (BIT = 1)个数 > HITHR 提取H,否则提取L
	tSioRxCfg.byRxBufLen	= 8;					//发送数据缓存长度(bit个数 = 8)，rxbuf 一次接收bit数量，len <= 32				
	tSioRxCfg.byRxCnt		= 24;					//SIO一次接收总的数据长度(bit个数 = 24)，byRxCnt >= byRxBufLen，byRxCnt < 256(最大32bytes)				
	tSioRxCfg.wRxFreq		= 8000000;				//rx clk =8MHz, Trxsamp = 1/8 = 125ns；每125ns 采样一次
	tSioRxCfg.bySpBitLen	= 8;					//bit采样的长度，每个bit采样次数为8，总得采样时间 = 8*Trxsamp = 1us
	tSioRxCfg.byInter		= SIO_INTSRC_RXBUFFULL;	//接收采用RXBUFFULL中断，当接收到byRxBufLen个bit后，产生中断，读取到接收buf中，每次读取byRxBufLen(8)bit
	
	csi_sio_rx_init(SIO0, &tSioRxCfg);				//初始化SIO接收参数
	csi_sio_timeout_rst(SIO0, 10, ENABLE);			//接收超时复位, timeout cnt > bySpBitLen
	csi_sio_set_buffer(g_wSioRxBuf, 24);			//设置接收数据buf和buf长度len >= (24个byRxBufLen)，将接收到的数据存放于用户定义的buffer中
	
	
	
	while(1)
	{
		iRet = csi_sio_receive(SIO0, wLedRxBuf, 24);//接收数据, 失败返回0；成功返回用户设置的接收数据个数(8*byRxCnt)bit，byRxCnt bytes；此函数非阻塞
		if(iRet)									//接收到用户设置数据
		{
			my_printf("SIO0 receive data len: %d \n", iRet);
		}
	}
	
	return iRet;
}

/** \brief sio rgb led recv demo
 *  \brief sio 接收RGB LED驱动数据，采用RXDNE中断模式；每收到byRxCnt个bit，产生中断；此中断效率大于RXBUFFULL中断
 * 
 *  \param[in] none
 *  \return error code
 */
int sio_led_rgb_recv_rxdone_demo(void)
{
	int iRet = 0;
	csi_sio_rx_config_t tSioRxCfg;
	uint32_t wRxBuf[8];
	
	csi_pin_set_mux(PA012, PA012_SIO);				//PA0.12 配置为SIO模式	
	
	//SIO RX 参数配置
	tSioRxCfg.byDebPerLen 	= 3;					//接收滤波周期	
	tSioRxCfg.byDebClkDiv 	= 2;					//接收滤波时钟分频
	tSioRxCfg.byTrgEdge 	= SIO_TRG_RISE;			//接收采样触发边沿，上升沿
	tSioRxCfg.byTrgMode		= SIO_TRGMD_DEB;		//接收采样触发模式，去抖后
	tSioRxCfg.byRxDir 	 	= SIO_RXDIR_LSB;		//接收数据方向， 数据按照bit0...31方式移入，根据不同应用移入数据
	tSioRxCfg.bySpMode		= SIO_SPMD_EDGE_EN;		//接收数据采样边沿对齐使能	
	tSioRxCfg.bySpExtra		= SIO_EXTRACT_HI;		//采样提取策略，(20H)HITHR; (BIT = 1)个数 > HITHR 提取H,否则提取L
	tSioRxCfg.byHithr		= 4;					//提取判定值, (BIT = 1)个数 > HITHR 提取H,否则提取L
	tSioRxCfg.byRxBufLen	= 8;					//发送数据缓存长度(bit个数 = 8)，rxbuf 一次接收bit数量，len <= 32				
	tSioRxCfg.byRxCnt		= 24;					//SIO一次接收总的数据长度(bit个数 = 24)，byRxCnt >= byRxBufLen，byRxCnt < 256(最大32bytes)				
	tSioRxCfg.wRxFreq		= 8000000;				//rx clk =8MHz, Trxsamp = 1/8 = 125ns；每125ns 采样一次
	tSioRxCfg.bySpBitLen	= 8;					//bit采样的长度，每个bit采样次数为8，总得采样时间 = 8*Trxsamp = 1us
	tSioRxCfg.byInter		= SIO_INTSRC_RXDNE;		//接收采RXDNE中断，byRxCnt <= 32bit；接收byRxCnt(24)个bit，产生中断,读取数据到接收buf，每次读取byRxCnt(24)bit
	
	csi_sio_rx_init(SIO0, &tSioRxCfg);				//初始化SIO接收参数
	csi_sio_timeout_rst(SIO0, 10, ENABLE);			//接收超时复位, timeout cnt > bySpBitLen
	csi_sio_set_buffer(g_wSioRxBuf, 8);				//设置接收数据buf和buf长度，buf长度len >= (byRxCnt * 8)bit ;数据存放于用户定义的buffer中
	
	while(1)
	{
		iRet = csi_sio_receive(SIO0, wRxBuf, 8);	//接收数据, 失败返回0；成功返回用户设置的接收数据个数(8*byRxCnt)bit，byRxCnt bytes；此函数非阻塞
		if(iRet == 8)								//接收到用户设置数据								
		{
			my_printf("SIO0 receive data len: %d \n", iRet);		
		}
	}
	
	return iRet;
}



/** \brief sio ti hdq send demo
 *  \brief sio 实现TI HDQ单线通讯协议，主机发送数据；数据传输方式LSB, 低7位是地址，最高位是R/W(0/1)控制位；一次传输两个字节
 * 
 *  \param[in] none
 *  \return error code
 */
int sio_hdq_send_demo(void)
{
	int iRet = 0;
	volatile uint8_t byRecv;
	csi_sio_tx_config_t tHdqTxCfg;
	
	uint32_t wSendBuf[2];		
	uint8_t byHdqData[2] = {0x68, 0x55};
	
	csi_pin_set_mux(PA012, PA012_SIO);			//PA0.12 配置为SIO模式		
	
	//SIO TX 参数配置
	tHdqTxCfg.byD0Len 		= 5;				//D0 对象序列长度(bit个数)，5个， break: 		__							
	tHdqTxCfg.byD1Len 		= 2;				//D1 对象序列长度(bit个数)，2个			 |_____|  |
	tHdqTxCfg.byDLLen 		= 5;				//DL 对象序列长度(bit个数)，5个
	tHdqTxCfg.byDHLen 		= 5;				//DH 对象序列长度(bit个数)，5个
	tHdqTxCfg.byDLLsq 		= 0x18;				//DL 对象序列具体定义: bit= 0 	   __      
												//					          |___|  |					
	tHdqTxCfg.byDHHsq 		= 0x1E;				//DH 对象序列具体定义: bit= 1 	 ____ 
												//					          |_|	 |
	tHdqTxCfg.byTxBufLen 	= 11;				//发送数据缓存长度(bit个数 = 8)，txbuf 一次发送bit数量，len <= 16
	tHdqTxCfg.byTxCnt 		= 19;				//SIO一次发送总的数据长度(bit个数 = 24)，byTxCnt >= byTxBufLen，byTxCnt < 256(最大32bytes)
	tHdqTxCfg.byIdleLev 	= SIO_IDLE_H;		//SIO空闲时刻IO管脚输出电平
	tHdqTxCfg.byTxDir 		= SIO_TXDIR_LSB;	//MSB->LSB, txbuf 数据按照bit[1:0]...[31:30]方式移出
	tHdqTxCfg.wTxFreq 		= 25000;			//tx clk =25kHz, Ttxshift = 1/4 = 40us；发送每bit时间是40us
	tHdqTxCfg.byInter		= SIO_INTSRC_NONE;	//不使用中断。目前只支持非中断模式
	
	csi_sio_tx_init(SIO0, &tHdqTxCfg);
	
	//发送写命令
	wSendBuf[0] = sio_hdq_addr_conver(byHdqData[0] | HDQ_WR_CMD);	//第一个字节：地址+命令(低7位是地址，第8位是读写(0/1)控制位);
	wSendBuf[1] = sio_data_conver(byHdqData[1]);					//第二个字节：数据字节
	
	while(1)
	{
		byRecv = csi_uart_getc(UART1);
		if(byRecv == 0x06)
		{
			csi_sio_send(SIO0, wSendBuf, 2);
		}
		nop;
	}
	
	return iRet;
}

/** \brief sio ti hdq receive write cmd demo
 *  \brief sio 实现TI HDQ单线通讯协议，接收主机写命令数据；数据传输方式LSB, 低7位是地址，最高位是R/W(0/1)控制位；一次传输两个字节
 * 
 *  \param[in] none
 *  \return error code
 */
int sio_hdq_recv_wrcmd_demo(void)
{
	int iRet = 0;
	csi_sio_rx_config_t tHdqRxCfg;
	uint32_t wHdqRxBuf[1];
	
	csi_pin_set_mux(PA012, PA012_SIO);				//PA0.12 配置为SIO模式	
	
	//SIO RX 参数配置
	tHdqRxCfg.byDebPerLen 	= 3;					//接收滤波周期	
	tHdqRxCfg.byDebClkDiv 	= 4;					//接收滤波时钟分频
	tHdqRxCfg.byTrgEdge 	= SIO_TRG_FALL;			//接收采样触发边沿，上升沿
	tHdqRxCfg.byTrgMode		= SIO_TRGMD_DEB;		//接收采样触发模式，去抖后
	tHdqRxCfg.byRxDir 	 	= SIO_RXDIR_MSB;		//接收数据方向， 数据按照bit31...0方式移入，根据不同应用移入数据
	tHdqRxCfg.bySpMode		= SIO_SPMD_EDGE_EN;		//接收数据采样边沿对齐使能	
	tHdqRxCfg.bySpExtra		= SIO_EXTRACT_HI;		//采样提取策略，(20H)HITHR; (BIT = 1)个数 > HITHR 提取H,否则提取L
	tHdqRxCfg.byHithr		= 13;					//提取判定值, (BIT = 1)个数 > HITHR 提取H,否则提取L
	tHdqRxCfg.byRxBufLen	= 8;					//发送数据缓存长度(bit个数 = 8)，rxbuf 一次接收bit数量，len <= 32				
	tHdqRxCfg.byRxCnt		= 16;					//SIO一次接收总的数据长度(bit个数 = 16)，byRxCnt >= byRxBufLen，byRxCnt < 256(最大32bytes)				
	tHdqRxCfg.wRxFreq		= 100000;				//rx clk =100kHz, Trxsamp = 1/100kHz = 10us；每10us采样一次
	tHdqRxCfg.bySpBitLen	= 19;					//bit采样的长度，每个bit采样次数为19，总得采样时间 = 19*Trxsamp = 190us
	tHdqRxCfg.byInter		= SIO_INTSRC_RXDNE | SIO_INTSRC_BREAK;	//接收RXDNE和BREAK中断，byRxCnt <= 32bit；接收byRxCnt(16)个bit，产生中断,读取数据到接收buf，每次读取byRxCnt(16)bit
	
	csi_sio_rx_init(SIO0, &tHdqRxCfg);					//初始化SIO接收参数
	csi_sio_break_rst(SIO0, SIO_BKLEV_LOW, 19, ENABLE);	//检测接收break

	csi_sio_set_buffer(g_wSioRxBuf, 1);					//设置接收数据buf和buf长度，buf长度len >= (byRxCnt * 8)bit ;数据存放于用户定义的buffer中
	
	while(1)
	{
		iRet = csi_sio_receive(SIO0, wHdqRxBuf, 1);		//接收数据, 失败返回0；成功返回用户设置的接收数据个数(8*byRxCnt)bit，byRxCnt bytes；此函数非阻塞
		if(iRet)										//接收到用户设置数据								
		{
			my_printf("SIO0 receive data len: %d \n", iRet);		
		}
	}
	
	return iRet;
}

/** \brief sio ti hdq send demo
 *  \brief sio 实现TI HDQ单线通讯协议，主机读取数据；数据传输方式LSB, 低7位是地址，最高位是R/W(0/1)控制位；一次传输两个字节
 * 
 *  \param[in] none
 *  \return error code
 */
int sio_hdq_send_recv_demo(void)
{
	int iRet = 0;
	volatile uint8_t byRecv;
	csi_sio_tx_config_t tHdqTxCfg;
	csi_sio_rx_config_t tHdqRxCfg;
	
	uint32_t wSendBuf[2];		
	uint8_t byHdqData[2] = {0x68, 0x55};
	
	uint32_t wHdqRx1Buf[2] ={0,0};
	
	csi_pin_set_mux(PA012, PA012_SIO);			//PA0.12 配置为SIO模式	
	
	//SIO RX 参数配置
	tHdqRxCfg.byDebPerLen 	= 3;					//接收滤波周期	
	tHdqRxCfg.byDebClkDiv 	= 4;					//接收滤波时钟分频
	tHdqRxCfg.byTrgEdge 	= SIO_TRG_FALL;			//接收采样触发边沿，上升沿
	tHdqRxCfg.byTrgMode		= SIO_TRGMD_DEB;		//接收采样触发模式，去抖后
	tHdqRxCfg.byRxDir 	 	= SIO_RXDIR_MSB;		//接收数据方向， 数据按照bit0...31方式移入，根据不同应用移入数据
	tHdqRxCfg.bySpMode		= SIO_SPMD_EDGE_EN;		//接收数据采样边沿对齐使能	
	tHdqRxCfg.bySpExtra		= SIO_EXTRACT_HI;		//采样提取策略，(20H)HITHR; (BIT = 1)个数 > HITHR 提取H,否则提取L
	tHdqRxCfg.byHithr		= 13;					//提取判定值, (BIT = 1)个数 > HITHR 提取H,否则提取L
	tHdqRxCfg.byRxBufLen	= 8;					//发送数据缓存长度(bit个数 = 8)，rxbuf 一次接收bit数量，len <= 32				
	tHdqRxCfg.byRxCnt		= 8;					//SIO一次接收总的数据长度(bit个数 = 16)，byRxCnt >= byRxBufLen，byRxCnt < 256(最大32bytes)				
	tHdqRxCfg.wRxFreq		= 100000;				//rx clk =100kHz, Trxsamp = 1/100kHz = 10us；每10us采样一次
	tHdqRxCfg.bySpBitLen	= 19;					//bit采样的长度，每个bit采样次数为19，总得采样时间 = 19*Trxsamp = 190us
	tHdqRxCfg.byInter		= SIO_INTSRC_RXDNE; 	//接收RXDNE中断，byRxCnt <= 32bit；接收byRxCnt(8)个bit，产生中断,读取数据到接收buf，每次读取byRxCnt(8)bit
	
	csi_sio_rx_init(SIO0, &tHdqRxCfg);					//初始化SIO接收参数
	csi_sio_set_buffer(g_wSioRxBuf, 1);					//设置接收数据buf和buf长度，buf长度len >= (byRxCnt * 8)bit ;数据存放于用户定义的buffer中
	
	//SIO TX 参数配置
	tHdqTxCfg.byD0Len 		= 5;				//D0 对象序列长度(bit个数)，5个， break: 		__							
	tHdqTxCfg.byD1Len 		= 2;				//D1 对象序列长度(bit个数)，2个			 |_____|  |
	tHdqTxCfg.byDLLen 		= 5;				//DL 对象序列长度(bit个数)，5个
	tHdqTxCfg.byDHLen 		= 5;				//DH 对象序列长度(bit个数)，5个
	tHdqTxCfg.byDLLsq 		= 0x18;				//DL 对象序列具体定义: bit= 0 	   __      
												//					          |___|  |					
	tHdqTxCfg.byDHHsq 		= 0x1E;				//DH 对象序列具体定义: bit= 1 	 ____ 
												//					          |_|	 |
	tHdqTxCfg.byTxBufLen 	= 11;				//发送数据缓存长度(bit个数 = 8)，txbuf 一次发送bit数量，len <= 16
	tHdqTxCfg.byTxCnt 		= 11;				//SIO一次发送总的数据长度(bit个数 = 24)，byTxCnt >= byTxBufLen，byTxCnt < 256(最大32bytes)
	tHdqTxCfg.byIdleLev 	= SIO_IDLE_H;		//SIO空闲时刻IO管脚输出电平
	tHdqTxCfg.byTxDir 		= SIO_TXDIR_LSB;	//MSB->LSB, txbuf 数据按照bit[1:0]...[31:30]方式移出
	tHdqTxCfg.wTxFreq 		= 25000;			//tx clk =25kHz, Ttxshift = 1/4 = 40us；发送每bit时间是40us
	tHdqTxCfg.byInter		= SIO_INTSRC_NONE;	//不使用中断。目前只支持非中断模式
	
	csi_sio_tx_init(SIO0, &tHdqTxCfg);
	
	//发送读命令
	wSendBuf[0] = sio_hdq_addr_conver(byHdqData[0]);		//第一个字节：地址+命令(低7位是地址，第8位是读写(0/1)控制位);
	
	while(1)
	{
		byRecv = csi_uart_getc(UART1);
		if(byRecv == 0x06)
		{
			csi_sio_send(SIO0, wSendBuf, 1);				//发送读命令
			csi_sio_set_mode(SIO0,SIO_RECV_MODE);			//设置SIO为接收模式
		}
		
		uint32_t wRecvStart = csi_tick_get_ms();			
		
		//等待接收数据，超出两秒，则退出接收模式
		do{
			if(csi_sio_receive(SIO0, wHdqRx1Buf, 1))		//接收到需要数据		
			{
				csi_sio_set_mode(SIO0,SIO_SEND_MODE);		//设置SIO为发送模式
				if((wHdqRx1Buf[0] >> 24) == 0x56)
				{
					nop;									//用户数据处理
				}
				break;
			}
			
			if((csi_tick_get_ms() - wRecvStart) >= 2000)	//超时处理，若超出2s 接收不到用户需要的数据则退出接收模式
			{
				csi_sio_set_mode(SIO0,SIO_SEND_MODE);		//设置SIO为发送模式
				break;
			}
			
		}while(1);
	}
	
	return iRet;
}
/** \brief sio ti hdq receive write cmd demo
 *  \brief sio 实现TI HDQ单线通讯协议，接收主机读命令数据；数据传输方式LSB, 低7位是地址，最高位是R/W(0/1)控制位；一次传输两个字节
 * 
 *  \param[in] none
 *  \return error code
 */
int sio_hdq_recv_rdcmd_demo(void)
{
	int iRet = 0;
	csi_sio_rx_config_t tHdqRxCfg;
	csi_sio_tx_config_t tHdqTxCfg;
	
	uint8_t byTxChar = 0x56;
	uint32_t wTxData = 0;
	uint32_t wRxBuf[2] = {0,0};
	csi_pin_set_mux(PA012, PA012_SIO);				//PA0.12 配置为SIO模式	
	
	//SIO TX 参数配置
	tHdqTxCfg.byD0Len 		= 5;				//D0 对象序列长度(bit个数)，5个， break: 		__							
	tHdqTxCfg.byD1Len 		= 2;				//D1 对象序列长度(bit个数)，2个			 |_____|  |
	tHdqTxCfg.byDLLen 		= 5;				//DL 对象序列长度(bit个数)，5个
	tHdqTxCfg.byDHLen 		= 5;				//DH 对象序列长度(bit个数)，5个
	tHdqTxCfg.byDLLsq 		= 0x18;				//DL 对象序列具体定义: bit= 0 	   __      
												//					          |___|  |					
	tHdqTxCfg.byDHHsq 		= 0x1E;				//DH 对象序列具体定义: bit= 1 	 ____ 
												//					          |_|	 |
	tHdqTxCfg.byTxBufLen 	= 8;				//发送数据缓存长度(bit个数 = 8)，txbuf 一次发送bit数量，len <= 16
	tHdqTxCfg.byTxCnt 		= 8;				//SIO一次发送总的数据长度(bit个数 = 24)，byTxCnt >= byTxBufLen，byTxCnt < 256(最大32bytes)
	tHdqTxCfg.byIdleLev 	= SIO_IDLE_H;		//SIO空闲时刻IO管脚输出电平
	tHdqTxCfg.byTxDir 		= SIO_TXDIR_LSB;	//MSB->LSB, txbuf 数据按照bit[1:0]...[31:30]方式移出
	tHdqTxCfg.wTxFreq 		= 25000;			//tx clk =25kHz, Ttxshift = 1/4 = 40us；发送每bit时间是40us
	tHdqTxCfg.byInter		= SIO_INTSRC_NONE;	//不使用中断。目前只支持非中断模式
	
	csi_sio_tx_init(SIO0, &tHdqTxCfg);			//初始化SIO发送参数
	wTxData = sio_data_conver(byTxChar);		//数据转换
	
	//SIO RX 参数配置
	tHdqRxCfg.byDebPerLen 	= 3;					//接收滤波周期	
	tHdqRxCfg.byDebClkDiv 	= 4;					//接收滤波时钟分频
	tHdqRxCfg.byTrgEdge 	= SIO_TRG_FALL;			//接收采样触发边沿，上升沿
	tHdqRxCfg.byTrgMode		= SIO_TRGMD_DEB;		//接收采样触发模式，去抖后
	tHdqRxCfg.byRxDir 	 	= SIO_RXDIR_MSB;		//接收数据方向， 数据按照bit0...31方式移入，根据不同应用移入数据
	tHdqRxCfg.bySpMode		= SIO_SPMD_EDGE_EN;		//接收数据采样边沿对齐使能	
	tHdqRxCfg.bySpExtra		= SIO_EXTRACT_HI;		//采样提取策略，(20H)HITHR; (BIT = 1)个数 > HITHR 提取H,否则提取L
	tHdqRxCfg.byHithr		= 13;					//提取判定值, (BIT = 1)个数 > HITHR 提取H,否则提取L
	tHdqRxCfg.byRxBufLen	= 8;					//发送数据缓存长度(bit个数 = 8)，rxbuf 一次接收bit数量，len <= 32				
	tHdqRxCfg.byRxCnt		= 8;					//SIO一次接收总的数据长度(bit个数 = 16)，byRxCnt >= byRxBufLen，byRxCnt < 256(最大32bytes)				
	tHdqRxCfg.wRxFreq		= 100000;				//rx clk =100kHz, Trxsamp = 1/100kHz = 10us；每10us采样一次
	tHdqRxCfg.bySpBitLen	= 19;					//bit采样的长度，每个bit采样次数为19，总得采样时间 = 19*Trxsamp = 190us
	tHdqRxCfg.byInter		= SIO_INTSRC_RXDNE | SIO_INTSRC_BREAK;	//接收RXDNE和BREAK中断，byRxCnt <= 32bit；接收byRxCnt(16)个bit，产生中断,读取数据到接收buf，每次读取byRxCnt(16)bit
	
	csi_sio_rx_init(SIO0, &tHdqRxCfg);					//初始化SIO接收参数
	csi_sio_break_rst(SIO0, SIO_BKLEV_LOW, 19, ENABLE);	//接收检测break, HDQ协议起始需要break总线
	csi_sio_set_buffer(g_wSioRxBuf, 1);					//设置接收数据buf和buf长度，buf长度len >= (byRxCnt * 8)bit ;数据存放于用户定义的buffer中
	
	while(1)
	{
		iRet = csi_sio_receive(SIO0, wRxBuf, 1);		//接收数据, 失败返回0；成功返回用户设置的接收数据个数(8*byRxCnt)bit，byRxCnt bytes；此函数非阻塞
		if(iRet)										//接收到用户设置数据								
		{
			if((g_wSioRxBuf[0] >> 24) == 0x68)
			{
				csi_sio_set_mode(SIO0,SIO_SEND_MODE);	//设置SIO为发送模式
				csi_sio_send(SIO0, &wTxData, 1);		//发送数据
				
				csi_sio_set_mode(SIO0,SIO_RECV_MODE);	//设置SIO为接收模式
			}
		}
		nop;
	}
	
	return iRet;
}

//
static uint32_t sio_led_data_conver(uint8_t byData)
{
	uint8_t i;
	uint32_t wData = 0xaaaa;
	for(i = 0; i < 8; i++)
	{
		if(byData & 0x80)
			wData |= (0x01 << (2*i));	
		byData = byData << 1;
	}
	return wData;
}

static void set_led_rgb(uint8_t byColR, uint8_t byColG, uint8_t byColB)
{
	uint32_t wRgbData[3];
	//led
	wRgbData[0] = sio_led_data_conver(byColG) ;		//G
	wRgbData[1] = sio_led_data_conver(byColR);		//R		
	wRgbData[2] = sio_led_data_conver(byColB);		//B	
	csi_sio_send(SIO0, wRgbData,3);
}

void led_rgb_display(uint8_t *byColData, uint16_t hwLedNum)
{
	uint16_t i;
	for(i = 0; i < hwLedNum; i++)
		set_led_rgb(byColData[3*i], byColData[3*i+1], byColData[3*i+2]);
}

/**
  \brief       SIO_HDQ 数据格式转换
  \param[in]   byAddr		send data
  \return      null
*/
uint32_t sio_hdq_addr_conver(uint8_t byAddr)
{
	uint8_t i;
	volatile uint32_t wDout = 0x00;
	
	wDout |=  TXBUF_D0 << 0;			//t(B)	break
	wDout |=  TXBUF_D1 << 2 ;			//t(BR) 
	
	for(i = 0; i < 8; i++)
	{
		if(byAddr & 0x01)
			wDout |=  (TXBUF_DH << ((i+2) *2));
		else
			wDout |=  (TXBUF_DL << ((i+2) *2));
			
		byAddr = byAddr >> 1;
	}
	
	wDout |=  TXBUF_D1 << 20 ;		//t(RSPS)
	
	return wDout;
}

/**
  \brief       SIO 通用数据格式转换
  \param[in]   byTxData		send data
  \return      null
*/
uint32_t sio_data_conver(uint8_t byTxData)
{
	uint8_t i;
	volatile uint32_t wDout = 0x00;
	
	for(i = 0; i < 8; i++)
	{
		if(byTxData & 0x01)
			wDout |=  (TXBUF_DH << (i*2));
		else
			wDout |=  (TXBUF_DL << (i*2));
			
		byTxData = byTxData >> 1;
			
	}
	return wDout;
	
}