/***********************************************************************//** 
 * \file  Reliability_demo.c
 * \brief  说明LVD, Reset Source, IWDT, WWDT, memory check etc的用法
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-23 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/

/* include ----------------------------------------------------------------*/
#include "rlblty.h"
#include "wdt.h"

/* externs function--------------------------------------------------------*/
/* private function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
static bool bLvdIrqHappened;

void lvd_irq_handler(void)
{
	bLvdIrqHappened = 1;
}


csi_error_t lvd_senario(void)
{	uint8_t byLevel;
	uint8_t byRstSrc;
	
	bLvdIrqHappened = 0;
	csi_reliability_init();		//如果system_init()里已经做过，这里可以省略。
	
	csi_lvd_attach_callback(&lvd_irq_handler);	
	csi_lvd_f_enable(LVD_39);	 //VDD掉电到3.9V即触发LVD中断
	
	csi_lvr_enable(LVR_28);		//VDD掉电到2.8V，芯片复位
	
	byLevel = csi_lvd_get_lvdlevel();
	byLevel = csi_lvd_get_lvrlevel();
	
	byRstSrc = csi_get_rst_reason(); //查询复位源，值为csi_rst_rsr_e枚举量之一
	
	while(bLvdIrqHappened == 0) ;
	
	return CSI_OK;
}

void memorycheck_config(void)
{	
	csi_flashcheck_set_times(10);		//开启flashcheck功能，检查错误次数上限 10
	
	csi_flashcheck_rst();				//错误到达上限，芯片复位	
	csi_flashcheck_disable();			//关闭flashcheck功能	
		
	csi_sramcheck_set_times( 8);		//开启fsram check功能，检查错误次数上限 8	
	csi_sramcheck_rst();				//错误到达上限，芯片复位 
	
	csi_sramcheck_disable();			//关闭sram check功能
	
}

csi_wdt_t tIwdt;
void iwdt_senario(void)
{
	uint32_t temp0, temp1;
	csi_wdt_init(&tIwdt, 0);						//初始化IWDT设备， 0 代表IWDT
	csi_wdt_debug_enable(&tIwdt, ENABLE);		    //可以配置在debug模式下，iwdt是否继续计时
	csi_wdt_stop(&tIwdt);							//关闭IWDT
	csi_wdt_set_timeout(&tIwdt, 1000);
	csi_wdt_set_int_time(&tIwdt, IWDT_INT_3_8);		//IWDT 计数到 1000 x 3/8 = 375ms时产生中断
	csi_wdt_start(&tIwdt);							//软件开启IWDT
	
	temp0 = csi_wdt_get_remaining_time(&tIwdt);		//读取剩余时间
	mdelay(100);
	temp0 = csi_wdt_get_remaining_time(&tIwdt);
	
	csi_wdt_feed(&tIwdt);							//喂狗
	
	csi_wdt_stop(&tIwdt);							//关闭IWDT
	csi_wdt_uninit(&tIwdt);							//注销IWDT设备
	
}

csi_wdt_t tWwdt;
csi_error_t wwdt_senario(void)
{
	uint32_t temp1, temp2, temp3;	
	
	csi_wdt_init(&tWwdt, 1);					// 1 代表是WWDT
	csi_wdt_debug_enable(&tWwdt, ENABLE);		//可以配置在debug模式下，wdt是否继续计时
	csi_wdt_set_timeout(&tWwdt, 80);			//设置timeout时间为80ms 时间设置过大 会返回错误
	csi_wdt_set_window_time(&tWwdt, 40);		//设置窗口值为40ms
	
	csi_wdt_start(&tWwdt);						//WWDT一旦使能，软件将不能停止
	mdelay(2);
	temp1 = csi_wdt_get_remaining_time(&tWwdt);
	if (temp1 < 77 || temp1 >79)
		return CSI_ERROR;
	
	//csi_wdt_feed(&tWwdt);						//如果在窗口外喂狗，将会引起芯片复位
	
	mdelay(62);
	temp2 = csi_wdt_get_remaining_time(&tWwdt); //读取剩余时间
	
	
	while(1) {
		temp3 = csi_wdt_get_remaining_time(&tWwdt) ;
		if (temp3< 30) {						//此处仅为示例喂狗操作。实际应用需要对代码运行时间有全局的了解。只有在窗口内喂狗，芯片才不会复位
			csi_wdt_feed(&tWwdt);			
			if (csi_wdt_get_remaining_time(&tWwdt)  < 79) {
				return CSI_ERROR;
			}
		}			
	}
	
	return CSI_ERROR;
}