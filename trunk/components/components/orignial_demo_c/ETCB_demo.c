/***********************************************************************//** 
 * \file  ETCB_demo.c
 * \brief  ETCB示例代码

 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-24 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * 
 * \verbatim
 * ETCB的使用方法可以参看
 * 	1、gpta_senario3(): EPT和ADC通过ETCB的联动
 * 	2、ept_senario1()：EXI和GPTA通过ETCB的联动
 * \endverbatim 
 * *********************************************************************
*/

/* include ----------------------------------------------------------------*/
#include "etb.h"
#include "gpio.h"
#include "adc.h"
#include "lpt.h"
#include "timer.h"
#include "iostring.h"
/* externs function--------------------------------------------------------*/

/* private function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
/* device instance ------------------------------------------------------*/
csi_etb_config_t g_tEtbConfig;

/*
	EXI1 		--> ADC_SYNC0 
-> 	ADC_TRGOUT0 --> BT0_SYNC0 
--> BT0_TRGOUT  --> LPT_SYNC0 
--> LPT_TRGOUT1 --> BT1_SYNC0 完成后进PEND中断打印信息
 * */
extern csi_gpio_t g_tGpioA0;
void apt_bt1_irq(void * arg)
{
	my_printf("trigger train done \n");
}

#ifndef CSI_TEST			
void etcb_srnario0(void)
{	
	csi_error_t tRet = CSI_OK;
	uint8_t ch;
	
	csi_adc_t tAdc;
	csi_lpt_t tLpt;
	csi_timer_t tBt0, tBt1;
	
	my_printf("Ready to connect PA0.1 to VDD\n");
	//EXI1
	csi_gpio_init(&g_tGpioA0, 0);
	csi_gpio_dir(&g_tGpioA0, 2, GPIO_DIRECTION_INPUT);
	csi_gpio_irq_mode_ex(&g_tGpioA0, 2, EXI_GRP1, GPIO_IRQ_MODE_RISING_EDGE);
	csi_exi_set_evtrg(1, TRGSRC_EXI1, 1);
	
	///ADC
	csi_adc_init(&tAdc,0);
	csi_adc_freq_div(&tAdc, 2);				//adc clk =pclk/2		
	csi_adc_sampling_time(&tAdc, 6);			//sample time = 16 + 6 = 22 ADC CLK		
	csi_adc_set_vref(&tAdc, ADCVERF_VDD_VSS);
	
	csi_adc_continue_mode(&tAdc, false);	

	csi_adc_channel_enable(&tAdc, 0, ENABLE, ADCSYNC_IN0);		//ADC channel 0 triggered by ADCSYNC_IN0
	csi_adc_channel_enable(&tAdc, 1, ENABLE, ADCSYNC_IN1);		//ADC channel 1 triggered by ADCSYNC_IN1

	csi_adc_set_sync(&tAdc, ADC_SYNCEN0, ADC_TRG_CONT, 99);		//ADCSYNC_IN0 delay (99+1)*4PCLK trigger channel 0
	csi_adc_set_evtrg(&tAdc, 0, ADC_TRGSRC_EOC);				//EOC event output trigger to ETCB
	
	///LPT
	csi_lpt_init(&tLpt, 0);
	csi_lpt_set_sync(&tLpt,0, LPT_SYNC_ONCE, ENABLE);			//sync0 允许一次触发，auto rearm
	//csi_lpt_set_sync(&tLpt,0, LPT_TRG_ONCE, ENABLE);
	csi_lpt_start_sync(&tLpt, LPT_CLK_ISCLK, 9); //ms			//使用ISCLK， 定时9ms

	
	csi_lpt_set_evtrg(&tLpt,0,LPT_TRGSRC_PRD, 1);				//PRD event output trigger to ETCB
		
	///BT0
	csi_timer_init(&tBt0, 0);	
	csi_timer_set_sync(&tBt0,TIMER_SYNCEN0 ,TIMER_TRG_ONCE, DISABLE);	//sync0 允许一次触发，不允许auto rearm
	csi_timer_start_sync(&tBt0, 1000); //us								//定时1ms
	csi_timer_set_evtrg(&tBt0, 0, TIMER_TRGSRC_PEND, 1);				//1次PEND event output trigger ETCB
	
	///BT1
	csi_timer_init(&tBt1, 1);
	csi_timer_set_sync(&tBt1,TIMER_SYNCEN0 ,TIMER_TRG_ONCE, DISABLE);	//sync0 允许一次触发，不允许auto rearm
	csi_timer_start_sync(&tBt1, 1000); //us								//定时1ms
	csi_timer_attach_callback(&tBt1, apt_bt1_irq, NULL);				//PEND event 即进入中断回调函数
	
	tRet |= csi_etb_init();
	g_tEtbConfig.ch_type = ETB_CH_ONE_TRIGGER_ONE;
	g_tEtbConfig.src_ip = 5; 			//EXI_TRGOUT0
	g_tEtbConfig.src_ip1 = 0xff;
	g_tEtbConfig.src_ip2 = 0xff;	
	g_tEtbConfig.dst_ip = 6; 			//ADC_SYNCIN0
	g_tEtbConfig.dst_ip1 = 0xff;
	g_tEtbConfig.dst_ip2 = 0xff;
	g_tEtbConfig.trig_mode = ETB_HARDWARE_TRIG; 
	ch = csi_etb_ch_alloc(g_tEtbConfig.ch_type);
	tRet |= csi_etb_ch_config(ch, &g_tEtbConfig);
	
	

	tRet |= csi_etb_init();
	g_tEtbConfig.ch_type = ETB_CH_ONE_TRIGGER_ONE;
	g_tEtbConfig.src_ip = 48;			//ADC_TRGOUT0 
	g_tEtbConfig.src_ip1 = 0xff;
	g_tEtbConfig.src_ip2 = 0xff;	
	g_tEtbConfig.dst_ip = 2; 			//BT0_SYNCIN2
	g_tEtbConfig.dst_ip1 = 0xff;
	g_tEtbConfig.dst_ip2 = 0xff;
	g_tEtbConfig.trig_mode = ETB_HARDWARE_TRIG;
	ch = csi_etb_ch_alloc(g_tEtbConfig.ch_type);
	tRet |= csi_etb_ch_config(ch, &g_tEtbConfig);
	

	tRet |= csi_etb_init();
	g_tEtbConfig.ch_type = ETB_CH_ONE_TRIGGER_ONE;
	g_tEtbConfig.src_ip = 12; 			//BT0_TRGOUT
	g_tEtbConfig.src_ip1 = 0xff;
	g_tEtbConfig.src_ip2 = 0xff;	
	g_tEtbConfig.dst_ip =  0; 			//LPT_SYNCIN0
	g_tEtbConfig.dst_ip1 = 0xff;
	g_tEtbConfig.dst_ip2 = 0xff;
	g_tEtbConfig.trig_mode = ETB_HARDWARE_TRIG;
	ch = csi_etb_ch_alloc(g_tEtbConfig.ch_type);
	tRet |= csi_etb_ch_config(ch, &g_tEtbConfig);
	

	tRet |= csi_etb_init();
	g_tEtbConfig.ch_type = ETB_CH_ONE_TRIGGER_ONE;
	g_tEtbConfig.src_ip = 0; 			//LPT_TRGOUT0
	g_tEtbConfig.src_ip1 = 0xff; 
	g_tEtbConfig.src_ip2 = 0xff;	
	g_tEtbConfig.dst_ip = 4;    		//BT1_SYNCIN0
	g_tEtbConfig.dst_ip1 = 0xff;
	g_tEtbConfig.dst_ip2 = 0xff;
	g_tEtbConfig.trig_mode = ETB_HARDWARE_TRIG;
	ch = csi_etb_ch_alloc(g_tEtbConfig.ch_type);
	tRet |= csi_etb_ch_config(ch, &g_tEtbConfig);
	
	
	while(1);
}

#endif 