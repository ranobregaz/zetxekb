/***********************************************************************//** 
 * \file  RTC_demo.c
 * \brief  RTC示例代码
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-24 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/


/* include ----------------------------------------------------------------*/
#include "pin.h"
#include "uart.h"
#include "board_config.h"
#include "iostring.h"
#include "rtc.h"
/* externs function--------------------------------------------------------*/
/* private function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
csi_rtc_time_t ptRtcTime, ptRtcTime1;
csi_rtc_time_t ptRtcAlarmTime;
static uint8_t byRtcFlg = 0x00;
/* device instance------------------------------------------------------*/
csi_rtc_t tRtc;
csi_gpio_t g_tGpioA0;

/**
  \brief       RTC 中断回调函数
   * 		   alarm发生时，进入
  \param		   
*/
void rtc_isr(csi_rtc_t *ptRtc, void *sta)
{
	int temp =  *(uint32_t *)(ptRtc->priv);
	if (temp & RTCINT_ALMB)
		csi_rtc_uninit(ptRtc);
	if (temp & RTCINT_CPRD) {
		if (byRtcFlg == 255)
			byRtcFlg = 0;
		else
			byRtcFlg += 1;
		my_printf("RTC timer: %d \n", byRtcFlg);
		csi_gpio_toggle(&g_tGpioA0, 0x2)	;
	}
}

/**
  \brief       RTC使用内部ISOSC作为时钟源，24小时模式
	*		   现在ALARMA被用于修正一个芯片的bug，所以只支持一个闹钟时间
  \return      csi_error_t
*/
csi_error_t RTC_senario(void)
{
	uint32_t temp;
	csi_error_t tRet = CSI_OK;
	csi_gpio_init(&g_tGpioA0, 0);
	
	csi_gpio_write(&g_tGpioA0, 0x2, GPIO_PIN_LOW);
	
	csi_rtc_init(&tRtc, 0);
		
	ptRtcTime.tm_year = 13;
	ptRtcTime.tm_mon = 15;
	ptRtcTime.tm_mday = 35;
	ptRtcTime.tm_wday = 7;
	ptRtcTime.tm_hour = 30;
	ptRtcTime.tm_min = 69;
	ptRtcTime.tm_sec = 65;
	csi_rtc_set_time(&tRtc, &ptRtcTime);		//设置RTC时间为2013年11月29日9:59:55
	
	ptRtcAlarmTime.tm_mday = 29;
	ptRtcAlarmTime.tm_hour = 10;
	ptRtcAlarmTime.tm_min = 0;
	ptRtcAlarmTime.tm_sec = 5;
	csi_rtc_set_alarm(&tRtc, &ptRtcAlarmTime, rtc_isr, NULL);	//设置RTC alarm时间为每月29日10:00:10
	
//	csi_rtc_cancel_alarm(&tRtc);
	while(1)
	{
		
		csi_rtc_get_time(&tRtc,  &ptRtcTime1);
		temp = csi_rtc_get_alarm_remaining_time(&tRtc);
		my_printf("%d : %d : %d ---- %d\n", ptRtcTime1.tm_hour, ptRtcTime1.tm_min, ptRtcTime1.tm_sec, temp);
		
		if(byRtcFlg)
		{
			byRtcFlg = 0;
			my_printf("Rtc test Result: Right!\n");
			break;
		}
	}
	
	return tRet;
	
}

/**
  \brief       定时模式
	*		   
  \return      void
*/
void RTC_senario1(void)
{
	csi_rtc_init(&tRtc, 0);
	
	csi_gpio_init(&g_tGpioA0, 0);
	
	csi_gpio_write(&g_tGpioA0, 0x2, GPIO_PIN_LOW);
	csi_rtc_start_as_timer(&tRtc, rtc_isr, RTC_TIMER_1S);	//设置RTC 1s 中断一次
	
	while(1);
}