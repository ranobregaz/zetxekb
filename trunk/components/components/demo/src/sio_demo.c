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
#include <iostring.h>

#include "demo.h"
/* externs function--------------------------------------------------------*/
// rgb led display
void led_rgb_display(uint8_t *byColData, uint16_t hwLedNum);
/* externs variablesr------------------------------------------------------*/
/* Private macro-----------------------------------------------------------*/
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
	//uint32_t wRecvBuf[24];
	
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
	csi_sio_timeout_rst(SIO0, 8, ENABLE);			//接收超时复位, timeout cnt >= bySpBitLen
	csi_sio_set_buffer(g_wSioRxBuf, 24);			//设置接收数据buffer和数据长度(24个byRxBufLen)，将接收到的数据存放于用户定义的buffer中
	
	
	
	while(1)
	{
		iRet = csi_sio_receive(SIO0);				//接收数据, 失败返回0；成功返回用户设置的接收数据个数(8*byRxCnt)bit，byRxCnt bytes；此函数非阻塞
		if(iRet == 24)								//接收到用户设置数据
		{
			my_printf("SIO0 receive data len: %d \n", iRet);
			csi_sio_set_buffer(g_wSioRxBuf, 24);	
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
	//uint32_t wRecvBuf[24];
	
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
	csi_sio_timeout_rst(SIO0, 10, ENABLE);			//接收超时复位, timeout cnt >= bySpBitLen
	csi_sio_set_buffer(g_wSioRxBuf, 8);				//设置接收数据buffer和数据长度，接收数据长度len = (byRxCnt * 8)bit ;数据存放于用户定义的buffer中
	
	while(1)
	{
		iRet = csi_sio_receive(SIO0);				//接收数据, 失败返回0；成功返回用户设置的接收数据个数(8*byRxCnt)bit，byRxCnt bytes；此函数非阻塞
		if(iRet == 8)								//接收到用户设置数据								
		{
			my_printf("SIO0 receive data len: %d \n", iRet);		
			csi_sio_set_buffer(g_wSioRxBuf, 8);	
		}
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