/***********************************************************************//** 
 * \file  tkey_demo.c
 * \brief TKEY_DEMO description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-6-7 <td>V0.0  <td>XB     <td>initial
 * </table>
 * *********************************************************************
*/
/* Includes ---------------------------------------------------------------*/
#include "csp.h"
#include "demo.h"
#include "tkey.h"
#include "porting.h"

uint32_t wKeyMapBk;
uint32_t wSleepCnt;
/***************************************
 * 按键处理程序，wKeyMap 的bit位代表TOUCH的通道是否有按键按下，如：bit0对应TOUCH通道0   bit16对应TOUCH通道16
 ***************************************/
void Keymap_Porg(void)  
{
	if(wKeyMap!=0)
	{
		wSleepCnt=0;
		if(wKeyMapBk!=wKeyMap)
		{
			wKeyMapBk=wKeyMap;
			if(wKeyMap==0x20)
			{
				nop;
			}
			if(wKeyMap==0x40)
			{
				nop;
			}
			if(wKeyMap==0x80)
			{
				nop;
			}
			//......
		}
		wKeyMap=0;
	}
	else
	{
		wKeyMapBk=0;
	}
}

/********************************************
 * TOUCH需要使用库使用说明：
 * ①把lib_csi_touch_1_01.a文件放置在chip->driwers目录下
 * ②打开chip的options 配置选项
 * ③在linker选项页的Library Name 输入库的名称lib_csi_touch_1_01
 * ④在linker选项页的Library Path 配置库的路径为drivers，库的添加即完成
 * TOUCH的使用：
 * 如果需要TOUCH按键扫描不受其他程序的影响就需要把csi_tkey_timer_handler();函数放置到定时器中断里，进行按键扫描。
 * 该函数也可放置在主循环里调用，但是如果主循环的一个循环周期时间较长的话，会影响按键的触摸体验。
 *  定时器调用csi_tkey_timer_handler()函数使用方法，以CORET为例：
 * 	system_init()函数下调用的csi_tick_init()函数里，如果csi_vic_enable_irq((uint32_t)CORET_IRQn);该语句被注释掉则需要去掉注释。
 * 	然后在interrupt.c里的CORETHandler中断里调用csi_tkey_timer_handler();注意需要添加相应的头文件。
 * 	
 ********************************************/
void touch_demo(void){
	
	csi_tkey_init();//TOUCH初始化函数调用
	while(1){
		
		//csi_tkey_timer_handler(); 如果不用定时器触发扫描，则去掉该语句前的注释即可
		Keymap_Porg();
		wSleepCnt++;
		
		if(wSleepCnt > 500){
			csi_tkey_setup_sleep();  				//设置TOUCH唤醒，及相关的配置
			soc_pm_enter_sleep(PM_MODE_DEEPSLEEP);	//进入低功耗模式（deepsleep）
			csi_tkey_quit_sleep();					//唤醒后清除一些操作
			wSleepCnt=0;
		}
		
		mdelay(10);
	}
	
}