/***********************************************************************//** 
 * \file  rtc_demo.c
 * \brief  rtc demo code
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-27 <td>V2.0 <td>WNN    <td>initial
 * </table>
 * *********************************************************************
*/

/* include ----------------------------------------------------------------*/
#include "rtc.h"
#include "pin.h"
#include "iostring.h"

/* externs function--------------------------------------------------------*/
/* private function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/


/** \brief 设置rtc时间的示例代码：包括时钟源、时间模式、时间设置，当前时间回读函数
 * 
 *  \param[in] none
 *  \return    none
 */
void rtc_set_time_demo(void)
{	
	csi_rtc_config_t tRtcConfig;
	csi_rtc_time_t tRtcTime,tRtcTimeRdbk;
	uint32_t wSec = 0;
	
	tRtcConfig.byClkSrc = RTC_CLKSRC_ISOSC;		//选择时钟源
	tRtcConfig.byFmt = RTC_24FMT;				//选择时间模式
	csi_rtc_init(RTC, &tRtcConfig);				//初始化设置
	
	
	tRtcTime.tm_year = 13;
	tRtcTime.tm_mon = 11;
	tRtcTime.tm_mday = 25;
	tRtcTime.tm_hour = 15;
	tRtcTime.tm_min = 15;
	tRtcTime.tm_sec = 0;	
	csi_rtc_set_time(RTC, &tRtcTime);			//设置时间
	csi_rtc_start(RTC);							//RTC开始计时
	
	tRtcTime.tm_year = 150;
	tRtcTime.tm_mon = 12;
	tRtcTime.tm_mday = 26;
	tRtcTime.tm_hour = 19;
	tRtcTime.tm_min = 59;
	tRtcTime.tm_sec = 55;	
	csi_rtc_set_time(RTC, &tRtcTime);			//修改时间以后需要重新start rtc
	csi_rtc_start(RTC);
	
	csi_rtc_get_time(RTC,  &tRtcTimeRdbk);		//回读当前时间
	my_printf("24FMT: %d:%d:%d\n", tRtcTimeRdbk.tm_hour, tRtcTimeRdbk.tm_min, tRtcTimeRdbk.tm_sec);
	
	csi_rtc_change_fmt(RTC, RTC_12FMT);			//修改时间模式为12小时制
	csi_rtc_get_time(RTC,  &tRtcTimeRdbk);		//回读当前时间
	my_printf("12FMT: %d:%d:%d\n", tRtcTimeRdbk.tm_hour, tRtcTimeRdbk.tm_min, tRtcTimeRdbk.tm_sec);
	
	csi_rtc_change_fmt(RTC, RTC_24FMT);			//修改时间模式为24小时制
	
	while(1){
		csi_rtc_get_time(RTC,  &tRtcTimeRdbk);
		
		if(wSec != tRtcTimeRdbk.tm_sec) {
			wSec = tRtcTimeRdbk.tm_sec;
			my_printf("%d:%d:%d\n", tRtcTimeRdbk.tm_hour, tRtcTimeRdbk.tm_min, tRtcTimeRdbk.tm_sec);
			
		}
	}
}

/** \brief 设置rtc 闹钟的示例代码：包括闹表模式、闹钟时间设置，当前时间回读和获取距离闹表时间到还有多久
 * 
 *  \param[in] none
 *  \return    none
 */
csi_rtc_alm_t  tAlmA;
void rtc_alarm_demo(void)	
{	
	uint32_t wTemp0;
	uint32_t wSec = 0;
	csi_rtc_time_t tRtcTime, tAlmTime, tRtcTimeRdbk;
	csi_rtc_config_t tRtcConfig;
	
	tRtcConfig.byClkSrc = RTC_CLKSRC_IMOSC_DIV4;  //选择时钟源
	tRtcConfig.byFmt = RTC_24FMT;				  //选择时间模式
	csi_rtc_init(RTC, &tRtcConfig);				  //初始化RTC
	
	tRtcTime.tm_year = 13;
	tRtcTime.tm_mon = 11;
	tRtcTime.tm_mday = 25;
	tRtcTime.tm_hour = 15;
	tRtcTime.tm_min = 15;
	tRtcTime.tm_sec = 0;	
	csi_rtc_set_time(RTC, &tRtcTime);				//设置时间
	csi_rtc_start(RTC);								//RTC开始工作
	
	tAlmA.byAlmMode = 1;							//日，小时，分钟，秒模式
	tAlmA.byAlmSt = 0;								//清除alarm时间到标志位
	tAlmTime.tm_mday = 0;
	tAlmTime.tm_hour = 14;
	tAlmTime.tm_min = 16;
	tAlmTime.tm_sec = 0xff;							//不要比较sec（0xFF意味着不要比较）
	csi_rtc_set_alarm(RTC, RTC_ALMA, tAlmA.byAlmMode, &tAlmTime);	//设置闹钟A	
	while(tAlmA.byAlmSt == 0){										//如果闹钟时间没有到，每秒打印一次当前时间和距离闹钟的时间
		csi_rtc_get_time(RTC,  &tRtcTimeRdbk);
		wTemp0 = csi_rtc_get_alarm_remaining_time(RTC, RTC_ALMA);

		if( wSec!= tRtcTimeRdbk.tm_sec) {
			wSec = tRtcTimeRdbk.tm_sec;
			my_printf("%d:%d:%d - %d\n", tRtcTimeRdbk.tm_hour, tRtcTimeRdbk.tm_min, tRtcTimeRdbk.tm_sec, wTemp0);
			
		}
	}
	my_printf("RTC Alarm happens!\n");				//一旦alarm时间到，AlarmA中断处理函数中会置位tAlmA.byAlmSt
	csi_rtc_cancel_alarm(RTC, RTC_ALMA);			//取消闹钟，保持原闹钟时间
	tAlmA.byAlmSt = 0;								//清alarm时间到标志位			

}

/** \brief 示例如何将RTC当做一个简单timer来使用
 * 
 *  \param[in] none
 *  \return    none
 */
void rtc_timer_demo(void)
{
	csi_rtc_config_t tRtcConfig;
	
	tRtcConfig.byClkSrc = RTC_CLKSRC_IMOSC_DIV4;  	//选择时钟源
	tRtcConfig.byFmt = RTC_24FMT;				  	//选择时间模式
	csi_rtc_init(RTC, &tRtcConfig);				  	//初始化RTC
	
	csi_rtc_start_as_timer(RTC, RTC_TIMER_1S);	  	//每1s进一次中断
	csi_rtc_start(RTC);								//RTC开始工作
}


/** \brief 示例RTC通过ETCB触发BT开始工作
 * 这个示例并不完整，如果要实现RTC定时触发功能，需要ETCB和其他被触发模块合作
 * 
 *  \param[in] none
 *  \return    none
 */
 void rtc_trgev_demo(void)	
 {
	csi_rtc_config_t tRtcConfig;
	
	tRtcConfig.byClkSrc = RTC_CLKSRC_IMOSC_DIV4;  	//选择时钟源
	tRtcConfig.byFmt = RTC_24FMT;				  	//选择时间模式
	csi_rtc_init(RTC, &tRtcConfig);				  	//初始化RTC
	
	csi_rtc_start_as_timer(RTC, RTC_TIMER_1S);	  	//每1s进一次中断
	csi_rtc_int_enable(RTC, RTC_INT_CPRD , DISABLE);//不需要中断的话，可以关掉
	csi_rtc_start(RTC);								//RTC开始工作
	
	csi_rtc_set_evtrg(RTC, 0, RTC_TRGOUT_CPRD, 2);  //RTC TRGEV0 每两秒钟输出一次trigger event
	while(1);
 }