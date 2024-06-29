 /***********************************************************************//** 
 * \file  lp_test.c
 * \brief  low power test code
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-12-29 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/

/* include ----------------------------------------------------------------*/

#include "tick.h"
#include "pin.h"
#include "pm.h"
#include "rtc.h"
#include "lpt.h"
#include "common.h"  
#include "iostring.h"


// <<< Use Configuration Wizard in Context Menu >>>

// <q> select power mode <2> [PM_MODE_SLEEP//PM_MODE_DEEPSLEEP]
// <i> by using q, you can modify any string with the specified choices
#define _LOW_POWER_MODE_ PM_MODE_DEEPSLEEP

// <q> select wakup source from sleep <2> [WKUP_EXI0//WKUP_RTC//WKUP_LPT]
// <i> by using q, you can modify any string with the specified choices
#define _WAKEUP_SRC_FROM_SLEEP_ WKUP_LPT

// <q> select wakup source from deepsleep <2> [WKUP_EXI0//WKUP_RTC//WKUP_LPT]
// <i> by using q, you can modify any string with the specified choices
#define _WAKEUP_SRC_FROM_DEEPSLEEP_ WKUP_LPT



/* externs function--------------------------------------------------------*/
extern csi_gpio_t g_tGpioA0, g_tGpioA1, g_tGpioB0, g_tGpioC0;
/* private function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
volatile static uint8_t RtcFlg = 0x00;
volatile uint8_t LptFlg = 0x00;
volatile uint8_t ExiFlg = 0x00;

/* device instance------------------------------------------------------*/
csi_gpio_t g_tGpioA0;
csi_rtc_t tRtc_syscon;
csi_rtc_time_t ptRtcTime, ptRtcTime1;
csi_lpt_t	tLpt;

void prepare2lp(void)					//进入低功耗模式前的准备动作
{
	//USER CODE
}

void wkup_lp(void)					//退出低功耗模式后恢复动作
{
	my_printf("wake up \n");
}

void exi0_irq_new(void)
{

	if (ExiFlg == 255)
		ExiFlg = 0;
	else
		ExiFlg += 1;
	
	my_printf("EXI0 isr %d\n", ExiFlg);
	
}

void lpt_handler(csi_lpt_t *ptLpt, void *arg)
{
	if (LptFlg == 255)
		LptFlg = 0;
	else
		RtcFlg += 1;
	
	my_printf("LPT isr %d\n", RtcFlg);
}
/**
  \brief       RTC callback interrupt handler
   * 		   be called when rtc time matches alarmB 
  \param		   
*/
void rtc_wkup_isr_new(csi_rtc_t *ptRtc, void *sta)
{

	if (RtcFlg == 255)
		RtcFlg = 0;
	else
		RtcFlg += 1;
	
	my_printf("RTC isr %d\n", RtcFlg);	
}

void lp_io_config(void)
{
	csi_gpio_dir(&g_tGpioA0, ~(0x1<<0 |0x1<<6 | 0x1<<7), GPIO_DIRECTION_GPD); //keep SWD, UART_TX
	csi_gpio_dir(&g_tGpioB0, 0xffffffff, GPIO_DIRECTION_GPD); 				  //PB0.6 UART tx

}


/**
  \brief       可以通过图形配置，设置低功耗模式及唤醒源
   * 		   循环进入低功耗，并被所设置的 唤醒源唤醒
  \param		   
*/
csi_error_t lowpower_senario(void)
{
	
	mdelay(3000);
	
	csi_gpio_init(&g_tGpioA0, 0);
	csi_gpio_init(&g_tGpioB0, 1);
	
	
	csi_pm_attach_callback(_LOW_POWER_MODE_, prepare2lp, wkup_lp);				//需要在工程设置compiler tab下加入define CONFIG_USER_PM=1;
	
	//config wakeup source EXI0
	if ((_WAKEUP_SRC_FROM_SLEEP_ == WKUP_EXI0) || (_WAKEUP_SRC_FROM_DEEPSLEEP_ == WKUP_EXI0))
	{
		csi_gpio_dir(&g_tGpioB0, 1, GPIO_DIRECTION_INPUT);								// PA0.0 input	
		csi_gpio_irq_mode_ex(&g_tGpioB0, 1, EXI_GRP0, GPIO_IRQ_MODE_RISING_EDGE);		// PAB.0 as EXI 0 rising edge active
		csi_gpio_irq_enable(&g_tGpioB0, 0x1, ENABLE);
		csi_gpio_attach_callback(&g_tGpioB0, exi0_irq_new, (void *)0x1);
	}
		
	//config wakeup source RTC	
	if ((_WAKEUP_SRC_FROM_SLEEP_ == WKUP_RTC) || (_WAKEUP_SRC_FROM_DEEPSLEEP_ == WKUP_RTC)) 
	{
		csi_rtc_init(&tRtc_syscon, 0);
		csi_rtc_start_as_timer(&tRtc_syscon, rtc_wkup_isr_new, RTC_TIMER_0_5S);			//0.5s唤醒系统
	}
	
	
	//config wakeup source LPT
	csi_lpt_init(&tLpt, 0);
	if ((_WAKEUP_SRC_FROM_SLEEP_ == WKUP_LPT) || (_WAKEUP_SRC_FROM_DEEPSLEEP_ == WKUP_LPT)) 
	{
		csi_lpt_attach_callback(&tLpt,lpt_handler, (void *)LPT_INT_PEND); 
	}
	csi_lpt_start(&tLpt, LPT_CLK_ISCLK, 1000);			//use ISOSC，1s	
	

	
	switch(_LOW_POWER_MODE_)
	{
		case (PM_MODE_DEEPSLEEP):
			//soc_clk_pm_enable(IMOSC_STP, ENABLE);		
			
			csi_pm_config_wakeup_source(_WAKEUP_SRC_FROM_DEEPSLEEP_, ENABLE);
			my_printf("enter deepsleep mode\n");
			lp_io_config();
			if (_WAKEUP_SRC_FROM_DEEPSLEEP_ == WKUP_EXI0)
				csi_gpio_dir(&g_tGpioA0, 0x1<<1, GPIO_DIRECTION_INPUT); //PA0.1 

			break;
		case (PM_MODE_SLEEP):
			
			switch (_WAKEUP_SRC_FROM_SLEEP_)
			{
				case (WKUP_LPT):
					csi_vic_set_wakeup_irq(tLpt.dev.irq_num);
					break;
				case (WKUP_RTC):
					csi_vic_set_wakeup_irq(tRtc_syscon.dev.irq_num);
					break;
				case (WKUP_EXI0):
					csi_vic_set_wakeup_irq(g_tGpioA0.dev.irq_num);
					break;
				default:
					break;
			}
			my_printf("enter sleep mode\n");
			lp_io_config();
			break;
		default:
			return CSI_ERROR;
	}
	
	//mdelay(100);
	ExiFlg = 0;
	LptFlg = 0;
	RtcFlg = 0;
	
	csi_gpio_dir(&g_tGpioB0, 0x1<<2, GPIO_DIRECTION_OUTPUT);
	while(1) {
		csi_gpio_write(&g_tGpioB0, 0x1<<2,GPIO_PIN_HIGH);		//进入LP前置高
		csi_pm_enter_sleep(_LOW_POWER_MODE_);
		csi_gpio_write(&g_tGpioB0, 0x1<<2,GPIO_PIN_LOW);		//退出LP后置高
		mdelay(500);
		
	}
	return CSI_OK;
}