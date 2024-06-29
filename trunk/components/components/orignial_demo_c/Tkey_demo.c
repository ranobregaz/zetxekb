/*============================================================================
 * Name        : main.c
 * Author      : $(username)
 * Version     : 0.0.0
 * Copyright   : Your copyright notice
 * Description : Simple function in C, Ansi-style
 *============================================================================
 */

//#include "include/test_func.h"
#include "iostring.h"
#include <string.h>
#include "soc.h"
#include "sys_clk.h"
#include <drv/pm.h>
#include <drv/rlblty.h>
#include <drv/gpio.h>
#include <drv/hwdiv.h>
#include <drv/eflash.h>
#include <drv/wdt.h>
#include <drv/pin.h>
#include "uart.h"
#include "gpio.h"
#include "timer.h"
#include "pin.h"
#include "cnta.h"
#include "tkey.h"



csi_cnta_t	g_tCnta;
csi_tkey_t g_tTkey;
csi_gpio_t g_gpioA0;
csi_uart_t	g_tUart;
extern void system_init(void);
extern uint16_t 	hwScanCnt;
extern uint32_t 	wKeyMap;
extern uint32_t 	wTimeCnt;
extern uint8_t		byBaseUpdata;
extern uint32_t	 	wTkeyBaseCnt;

/** \brief cnta interrupt callback function
 * 
 *  \param cnta_handle: handle timer handle to operate
 *  \param arg: para
 *  \return none
 */
static void user_cnta_event(csi_cnta_t *cnta_handle, void *arg)			//定时器中断，用来定时更新基线（任意定时器都可以，只要把下面的代码放于定时器中断里就ok）
{

	if(wKeyMap!=0)	//判断是否有按键按下
	{
		wTimeCnt++;
		if(wTimeCnt>wTkeyBaseCnt)	// 基线更新时间 =  (5ms*wTkeyBaseCnt ) ms
		{
			byBaseUpdata=1;		//基线更新标志
			wTimeCnt=0;
		}
	}
}


/**
  \brief       counterA 定时中断初始化 
  \return      csi_error_t
*/
csi_error_t cnta_init(void)
{	
	csi_error_t tRet = CSI_OK;

	tRet = csi_cnta_init(&g_tCnta, 0);	
	
	tRet =  csi_cnta_attach_callback(&g_tCnta, user_cnta_event, NULL);		
	tRet = csi_cnta_start(&g_tCnta, 5000);		//5ms定时
	
	return tRet;
	
}

/**
  \brief       tkey示例
  \return      csi_error_t
*/
csi_error_t tkey_init(void)		
{	
	csi_error_t tRet = CSI_OK;

	tRet = csi_tkey_init(&g_tTkey, 0);	//touch初始化，  touch的IO使能在tkey_parameter.h里的wTkeyIOEnable，第几位对应TOUCH几如： bit0 -->  touch 0      bit16 --> touch16
	                                                	//注意IO使能是，相应的IO请勿用于其他功能，否则会导致touch工作不正常 ，IO的初始化已经在csi_tkey_init()函数里完成，不需要自己去配置GPIO
														//touch的其他一些参数配置请在tkey_parameter.h里完成。
	tRet =  csi_tkey_attach_callback(&g_tTkey, NULL, NULL);		//注册中断函数
	csi_tkey_set_intr(&g_tTkey,1,TKEY_DNE);	//使能相应的中断
	
	return tRet;
	
}


int uart_init(void) 	//串口初始化  串口用于发送当前的按键 
{
	int iRet = 0;
	//csi_pin_set_mux(PB05,PB05_UART2_RX);		//PB05 = RX
	csi_pin_set_mux(PA014,PA014_UART1_TX);			//PA014= TX
	iRet = csi_uart_init(&g_tUart,1);
	CSI_CHECK_RETURN(iRet);
	
	iRet = csi_uart_format(&g_tUart, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1);
	CSI_CHECK_RETURN(iRet);
	
	iRet = csi_uart_baud(&g_tUart,115200);
	CSI_CHECK_RETURN(iRet);
	
	iRet = csi_uart_attach_callback(&g_tUart,NULL, NULL);
	CSI_CHECK_RETURN(iRet);
	
	return  iRet;
}

int tkey_main()
{
	int i=0;
	system_init();		//系统初始化
	uart_init();		//串口初始化
	tkey_init();		//touch初始化
	csi_tkey_baseline_prog(&g_tTkey);		//获取第一次上电时的按键原始值作为基线（baseline）
	cnta_init();	//定时器初始化，
	
	while(1){

		csi_tkey_prgm(&g_tTkey);  //循环扫描touch

		if(wKeyMap !=0)   //判断是否有按键按下，有则PA015输出低，没有则输出高
		{
			csi_gpio_write(&g_gpioA0,0X1<<15,0);	
		}else
		{
			csi_gpio_write(&g_gpioA0,0X1<<15,1);
		}
		for(i=0;i!=17;i++)		//遍历wKeyMap 变量，wKeyMap变量的第几位被值1，则对应按键被按下。如： bit0 =1 则  touch0 被按下      bit16=1 则 touch16被按下
		{
			if((wKeyMap >>i)&0x1) 
			{
				
				csi_uart_putc(&g_tUart,'T');
				csi_uart_putc(&g_tUart,'C');
				csi_uart_putc(&g_tUart,'H');
				csi_uart_putc(&g_tUart,':');
				if(i<10)
					csi_uart_putc(&g_tUart,i+0x30);
				else
				{
					csi_uart_putc(&g_tUart,0x31);
					csi_uart_putc(&g_tUart,i-10+0x30);
				}
				csi_uart_putc(&g_tUart,0x0d);
				csi_uart_putc(&g_tUart,0x0a);
		
			}
		}
		
	}

    return 0;
	
}
