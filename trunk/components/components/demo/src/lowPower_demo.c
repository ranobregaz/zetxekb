/***********************************************************************//** 
 * \file  lowPower_demo.c
 * \brief  low power demo code
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-6-2 <td>V2.0 <td>WNN    <td>initial
 * </table>
 * *********************************************************************
*/

/* include ----------------------------------------------------------------*/
#include "pm.h"
#include "pin.h"
#include "rtc.h"
#include "lpt.h"
#include "common.h"  
#include "iostring.h"
#include "tick.h"
#include "irq.h"

// <<< Use Configuration Wizard in Context Menu >>>

// <q> select power mode <2> [PM_MODE_SLEEP//PM_MODE_DEEPSLEEP]
// <i> by using q, you can modify any string with the specified choices
#define _LOW_POWER_MODE_ PM_MODE_SLEEP

/* externs function--------------------------------------------------------*/
/* private function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

volatile uint8_t byExiFlg = 0x00;

void prepare2lp(void)					//进入低功耗模式前的准备动作
{
	//USER CODE，如记忆管脚状态
}

void wkup_lp(void)					   //退出低功耗模式后恢复动作
{
	//USER CODE，如回复管脚状态
}


void lp_exi_wakeup_demo(void)
{
	mdelay(3000);
	
	soc_pm_attach_callback(_LOW_POWER_MODE_, prepare2lp, wkup_lp);	//需要在工程设置compiler tab下加入define CONFIG_USER_PM=1;
	
	csi_pin_set_mux(PA00, PA00_OUTPUT);
	
	csi_pin_set_mux(PA05,PA05_INPUT);							
	csi_pin_pull_mode(PA05, GPIO_PULLUP);						//PA05 上拉
	csi_pin_irq_mode(PA05,EXI_GRP5, GPIO_IRQ_FALLING_EDGE);		//PA05 下降沿产生中断
	csi_pin_irq_enable(PA05, EXI_GRP5, ENABLE);					//PA05 中断使能，选择中断组5	
	
	switch(_LOW_POWER_MODE_)
	{
		case (PM_MODE_DEEPSLEEP):
			soc_pm_config_wakeup_source(WKUP_EXI3, ENABLE);		//将WKUP_EXI3（包括GROUP4~9）设为唤醒源
			my_printf("enter deep-sleep mode\n");
			break;
		case (PM_MODE_SLEEP):
			csi_vic_set_wakeup_irq(EXI3_IRQn);
			my_printf("enter sleep mode\n");
			break;
		default:
			break;
	}
	
	while(1) {
		csi_pin_set_high(PA00);
		my_printf("e\n");
		soc_pm_enter_sleep(_LOW_POWER_MODE_);
		my_printf("w\n");
		csi_pin_set_low(PA00);
		mdelay(500);
	}

}