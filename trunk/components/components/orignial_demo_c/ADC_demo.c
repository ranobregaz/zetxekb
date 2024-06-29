/***********************************************************************//** 
 * \file  ADC_demo.c
 * \brief  ADC 示例代码 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-24 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/



#include "pin.h"
#include "adc.h"
#include "board_config.h"

/* externs function--------------------------------------------------------*/
/* private function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

#define ADCDATADEPTH 5
volatile static uint32_t g_wAdcData[2][ADCDATADEPTH] ={0};			//1 通道, 深度 5

/* device instance------------------------------------------------------*/
csi_adc_t g_tAdc;

/** \brief ADC中断回调函数。用于对时序不敏感的场合。
 *  \param adc: ADC handle to operate
 *  \param event: adc event 
 *  \param arg: para
 *  \return none
 */ 
void user_adc_event(csi_adc_t *adc, csi_adc_event_t event, void *arg)
{
	/* add user code*/
	if(event == ADC_EVENT_CONVERT_COMPLETE)
	{
		
		csi_adc_stop_async(adc);									//采样结果保存于g_wAdcData
		csi_adc_set_buffer(&g_tAdc, (uint32_t *)g_wAdcData,5);		//必须重新设置采样数据存储位置和采样深度
		csi_adc_start_async(adc);		//可以在代码的合适地方restart adc sample
		
	}
}

/**
  \brief       同步方式，软件查询ADC转换结果
  \return      csi_error_t
*/
csi_error_t adc_senario0(void)
{
	csi_error_t tRet = CSI_OK;
	csi_pin_set_mux(ADC_PIN, ADC_PIN_FUNC);
	
	csi_adc_init(&g_tAdc, ADC_IDX);						//ADC设备初始化
	tRet =  csi_adc_freq_div(&g_tAdc, 2);				//adc clk =pclk/2		
	tRet =  csi_adc_sampling_time(&g_tAdc, 6);			//sample time = 16 + 6 = 22 ADC CLK		
	tRet =  csi_adc_continue_mode(&g_tAdc, false);		//ADC one time mode		
	tRet =  csi_adc_set_vref(&g_tAdc, ADCVERF_VDD_VSS); //如果需要用到外部参考，需要先进行管脚配置。要先用csi_pin_set_mux()进行管脚mux设置。
#ifdef CSI_TEST
	tRet =  csi_adc_channel_enable(&g_tAdc, ADC_CH, ENABLE);	//第1次调用，即对应内部的seq0。软件触发。
#else
	tRet =  csi_adc_channel_enable(&g_tAdc, ADC_CH, ENABLE, ADCSYNC_SWTGR);	//第1次调用，即对应内部的seq0。软件触发。
#endif	
	tRet =  csi_adc_start(&g_tAdc);		
	
	while(1){
		g_wAdcData[0][0] = csi_adc_read_channel(&g_tAdc,0);	//采一次，读一次。csi_adc_read_channel()会等待直到采样结束
		tRet =  csi_adc_start(&g_tAdc);	
	}
	return tRet;
}

/**
  \brief       对时间要求不敏感的场合的异步方式，利用中断，得到ADC转换结果
   * 
   * 		   !!!注意，要修改启动文件line 60，将 .long ADCIntHandler	改为	.long  do_irq
   * 			中断回调函数为user_adc_event() !!!
   * 			ADC工作于连续采样模式，软件使能后，采样ADCDATADEPTH次后，回调一次user_adc_event（），所有数据存储于g_wAdcData中
  \return      csi_error_t
*/
csi_error_t adc_senario1(void)
{
	csi_error_t tRet = CSI_OK;
	
	csi_pin_set_mux(ADC_PIN, ADC_PIN_FUNC);
	csi_pin_set_mux(ADC_PIN1, ADC_PIN1_FUNC);
	
	csi_adc_init(&g_tAdc, ADC_IDX);						//ADC设备初始化
	tRet =  csi_adc_freq_div(&g_tAdc, 2);				//adc clk =pclk/2		
	tRet =  csi_adc_sampling_time(&g_tAdc, 6);			//sample time = 16 + 6 = 22 ADC CLK		
	tRet =  csi_adc_continue_mode(&g_tAdc, true);		//中断方式必须将ADC设为连续采样模式		
	tRet =  csi_adc_set_vref(&g_tAdc, ADCVERF_VDD_VSS); //如果需要用到外部参考，需要先进行管脚配置。要先用csi_pin_set_mux()进行管脚mux设置。
#ifdef CSI_TEST
	tRet =  csi_adc_channel_enable(&g_tAdc, ADC_CH, ENABLE);		//第1次调用，即对应内部的seq0。软件触发。
	tRet =  csi_adc_channel_enable(&g_tAdc, ADC_CH1, ENABLE);	//第2次调用，即对应内部的seq1。软件触发。
#else	
	tRet =  csi_adc_channel_enable(&g_tAdc, ADC_CH, ENABLE, ADCSYNC_SWTGR);		//第1次调用，即对应内部的seq0。软件触发。
	tRet =  csi_adc_channel_enable(&g_tAdc, ADC_CH1, ENABLE, ADCSYNC_SWTGR);	//第2次调用，即对应内部的seq1。软件触发。
#endif
	tRet =  csi_adc_set_buffer(&g_tAdc, (uint32_t *)g_wAdcData,ADCDATADEPTH); 	//采样数据存于g_wAdcData, 每ADCDATADEPTH次回调一次user_adc_event();
	tRet =  csi_adc_attach_callback(&g_tAdc, user_adc_event, NULL);				//注册中断回调函数
	
	tRet =  csi_adc_start_async(&g_tAdc);				//开始异步方式的ADC采样
	
	while(1);
	return tRet;
}

/**
  \brief       对时间要求敏感的场合的异步方式，利用中断，得到ADC转换结果。
   * 
   * 		   !!!注意，确保启动文件line 60，.long ADCIntHandler
   * 			中断处理函数在interrupt.c中，ADCIntHandler()
  \return      csi_error_t
*/
csi_error_t adc_senario2(void)
{
	//参考EPT-ETCB-ADC
	return CSI_OK;
}


