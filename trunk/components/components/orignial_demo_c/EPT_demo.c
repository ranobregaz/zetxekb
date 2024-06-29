/***********************************************************************//** 
 * \file  EPT_demo.c
 * \brief  EPT 示例代码
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-10 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/

/* include ----------------------------------------------------------------*/
#include "ept.h"
#include "etb.h"
#include "pin.h"
#include "adc.h"
#include "board_config.h"
#include "iostring.h"

/* externs function--------------------------------------------------------*/
/* private function--------------------------------------------------------*/
void senario0_pin_config(void);
void senario1_pin_config(void);
void senario2_pin_config(void);
void senario3_pin_config(void);
csi_error_t senario1_etb_config(void);
csi_error_t senario1_adc_config(void);
csi_error_t senario1_ept_config(void);
csi_error_t senario0_ept_config(void);
csi_error_t senario2_ept_config(void);
csi_error_t senario3_ept_config(void);
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
csi_etb_config_t tEtbConfig;

volatile static uint32_t g_wAdcVal[1][1];			//1 通道, 深度 1

/* device instance------------------------------------------------------*/
csi_ept_t g_tEpt;
csi_adc_t g_tAdc;
csi_gpio_t g_tGpioB0;

#include "sys_clk.h"


/**
  \brief       EPT 工作于 dual mode3 (up and down 计数模式)。调整占空比及斩波输出示例。
  * \verbatim
  * CHnX active High ,ChnY active Low
  *  	ChnX    	______
  *  		_______|      |_______ 
  *   	ChnY       _________
  *  		______|			|_______
  *    
  * \endverbatim
  \return      csi_error_t
*/
csi_error_t ept_senario0(void)
{
	csi_error_t tRet = CSI_OK;
	uint32_t wPWMVal;
	
	tRet =  csi_gpio_init(&g_tGpioB0,1);
	senario0_pin_config();
	tRet =  senario0_ept_config();						//20KHz, 40% 占空比，前后死区500ns
	
	csi_ept_swstart(&g_tEpt);		
	
	mdelay(2000);										//延时2s
	wPWMVal = g_ept_prd * 10/100;						//在csi_ept_set_freq（）时会生成这个prd全局变量，可以直接使用
	csi_ept_change_ch_duty(&g_tEpt, EPT_CH_A, wPWMVal);	//占空比10%输出
	mdelay(2000);
	wPWMVal = g_ept_prd * 50/100;	
	csi_ept_change_ch_duty(&g_tEpt, EPT_CH_A, wPWMVal);	//占空比50%输出
	mdelay(2000);
	wPWMVal = g_ept_prd * 70/100;	
	csi_ept_change_ch_duty(&g_tEpt, EPT_CH_A, wPWMVal); //占空比10%输出
	mdelay(2000);
	csi_ept_set_stop_st(&g_tEpt, EPT_STP_HZ);			//设置ept停下来时的管脚高阻状态

	csi_ept_set_cp(&g_tEpt, 2, 3, 5);					//斩波频率fchop = PCLK/((2+1)*8)， duty = 3/8，第一个脉宽= byFirstWidth/fchop
	csi_ept_cpa_enable(&g_tEpt, ENABLE, ENABLE);		//CHAX, CHAY斩波输出
	mdelay(2000);


	csi_ept_swstop(&g_tEpt);							//EPT停止计数
	
	while(1);
	return tRet;
}

/**
  \brief       EPT 工作于 dual mode1 (up and down 计数模式)。EPT-ETCB-ADC 联合工作，将在counter值 = PRD时（中心点），通过ETCB，触发ADC 开始转换。
  * \verbatim
  * CHnX active High ,ChnY active High
  *  	ChnX    	______
  *  		_______|      |_______ 
  *   	ChnY
  *  		_____			_______
  *      		|__________|
  *   
  *		   !!!注意，这个senario，需要确保启动文件line 60的内容为 .long ADCIntHandler
  * 	   这样，中断时会调用interrupt.c中的ADCIntHandler()!!!
  * \endverbatim
  \return      csi_error_t
*/
csi_error_t ept_senario1(void)
{	
	csi_error_t tRet = CSI_OK;	
	
	senario1_pin_config();
	
	tRet =  senario1_adc_config();
	tRet =  senario1_etb_config();
	tRet =  senario1_ept_config();		

	csi_ept_swstart(&g_tEpt);
	csi_adc_int_enable(&g_tAdc, 16, ENABLE);					//使能第一个通道转换结束中断
	while(1);
	return tRet;
	
}

/**
  \brief       EPT 工作于 dual mode2 (up and down 计数模式)。EBI3（PB02）下降沿将触发EPT硬锁。
  *	\verbatim 
  *  	CHnX active Low ,ChnY active Low
  *		ChnX    ______		 _____
  *				     |_______| 
  *		ChnY	     __________
  *			   _____|         |______	
  * \endverbatim 
  \return      csi_error_t
*/
csi_error_t ept_senario2(void)
{
	csi_error_t tRet = CSI_OK;	
	
	senario2_pin_config();												//设置EPT CHAX和CHAY和EBI3
	tRet =  senario2_ept_config();
	csi_ept_swstart(&g_tEpt);											//EPT开始工作
	while ((csi_ept_get_hdlck_st(&g_tEpt) & (0x1 << EPT_EBI3)) == 0); 	//等待PB02变高（手动）触发硬锁
	my_printf("Hardlock happened!\n");
	while(1);
	return tRet;
}

/**
  \brief       EPT 工作于 dual mode4 (up and down 计数模式)。3秒钟后，软件force EPT 硬锁。
  *	\verbatim 
  *   *	CHnX active Low ,ChnY active High
  *		ChnX    ______		 _____
  *				     |_______| 
  *		ChnY	_____          ____
  *			        |_________|	
  * \endverbatim 
  \return      csi_error_t
*/
csi_error_t ept_senario3(void)
{
	csi_error_t tRet = CSI_OK;
	
	
	senario3_pin_config();													//设置EPT CHAX和CHAY
	tRet =  senario3_ept_config();											//配置EPT
	
	csi_ept_swstart(&g_tEpt);												//开始计数	
	
	mdelay(3000);
	csi_ept_force_em(&g_tEpt,EPT_EBI1);										//软件触发EBI1通道的软定条件。
	
	if ((csi_ept_get_hdlck_st(&g_tEpt) & (0x1 << (EPT_EBI1))) == 0)			//查询硬锁状态
		tRet =  CSI_ERROR;
	else
		my_printf("SoftLock happened\n");
	
	mdelay(1000);
	csi_ept_clr_hdlck(&g_tEpt, EPT_EBI1);									//清除硬锁状态
	if ((csi_ept_get_hdlck_st(&g_tEpt) & (0x1 << EPT_EBI1)) == 1)
		tRet =  CSI_ERROR;
	else
		my_printf("SoftLock cleared\n");
		
	while(1);
	return tRet;

}


void senario0_pin_config(void)
{
	csi_pin_set_mux(EPT_PWMCHAX_PIN, EPT_PWMCHAX_PIN_FUNC);		//根据board_config.h的设定，配置EPT A通道的两路互补输出
	csi_pin_set_mux(EPT_PWMCHAY_PIN, EPT_PWMCHAY_PIN_FUNC);
}

void senario1_pin_config(void)
{
	csi_gpio_init(&g_tGpioB0,1);						//demo中的GPIO_PIN属于GPIOB0，所以需要先初始化GPIOB0
	csi_pin_mode(GPIO_PIN, GPIO_MODE_PUSH_PULL);				//将GPIO_PIN(见board_config.h）设为推挽输出，可用于调试
	
	csi_pin_set_mux(EPT_PWMCHAX_PIN, EPT_PWMCHAX_PIN_FUNC);		//根据board_config.h的设定，配置EPT A通道的两路互补输出
	csi_pin_set_mux(EPT_PWMCHAY_PIN, EPT_PWMCHAY_PIN_FUNC);
	csi_pin_set_mux(ADC_PIN, ADC_PIN_FUNC);						//根据board_config.h的设定，配置ADC的输入
}

void senario2_pin_config(void)
{
	csi_pin_set_mux(EPT_PWMCHAX_PIN, EPT_PWMCHAX_PIN_FUNC);		//根据board_config.h的设定，配置EPT A通道的两路互补输出
	csi_pin_set_mux(EPT_PWMCHAY_PIN, EPT_PWMCHAY_PIN_FUNC);
	csi_pin_set_mux(EPT_EBI_PIN, EPT_EBI);						//设置EPT锁定的外部触发管脚
	
}

void senario3_pin_config(void)
{
	csi_pin_set_mux(EPT_PWMCHAX_PIN, EPT_PWMCHAX_PIN_FUNC);
	csi_pin_set_mux(EPT_PWMCHAY_PIN, EPT_PWMCHAY_PIN_FUNC);
	//因为senario3只用到软件force锁定动作，所以pinmux不需要配置

}

csi_error_t senario1_adc_config(void)
{
	csi_error_t tRet = CSI_OK;
	
	csi_adc_init(&g_tAdc, ADC_IDX);
	tRet |= csi_adc_freq_div(&g_tAdc, 2);			//ADC的模拟时频率不能超过24MHz。PCLK=48MHz，所以必须2分频。adc clk =pclk/2		
	tRet |= csi_adc_sampling_time(&g_tAdc, 6);		//sample time = 16 + 6 = 22 ADC CLK		
	tRet |= csi_adc_continue_mode(&g_tAdc, false);		//ADC one time mode		
	csi_adc_set_sync(&g_tAdc, ADC_SYNCEN0, ADC_TRG_CONT, 0);	//允许连续触发， 不要dalay触发	
	tRet =  csi_adc_set_vref(&g_tAdc, ADCVERF_VDD_VSS);         //如果需要用到外部参考，需要先进行管脚配置。要先用csi_pin_set_mux()进行管脚mux设置。
#ifdef CSI_TEST
	tRet |= csi_adc_channel_enable(&g_tAdc, ADC_CH, ENABLE);	//第一次调用，会对应内部的seq0。ETCB 的ADC_SYNCIN0触发转换	
#else
	tRet |= csi_adc_channel_enable(&g_tAdc, ADC_CH, ENABLE, ADCSYNC_IN0);	//第一次调用，会对应内部的seq0。ETCB 的ADC_SYNCIN0触发转换	
#endif
	tRet |= csi_adc_attach_callback(&g_tAdc,NULL, NULL);		//使能中断，但因为这个senario，ADC使用独立的中断，所以不用配置回调函数。
	
	return tRet;
}

csi_error_t senario0_ept_config(void)
{	
	csi_error_t tRet = 0;
	
	tRet =  csi_ept_init(&g_tEpt, 0);									//初始化EPT设备
	tRet =  csi_ept_set_freq(&g_tEpt, 20000);							//EPT工作于20KHz

	tRet =  csi_ept_set_ch_dualout_md3(&g_tEpt, EPT_CH_A, 40, 500, 500);	//通道A两路互补输出，占空比40%， 左右死区时间500ns

	csi_ept_set_start_mode(&g_tEpt, EPT_SW);							
	csi_ept_set_os_mode(&g_tEpt, EPT_OP_CONT);
	
	return tRet;
	
}

csi_error_t senario1_ept_config(void)
{
	csi_error_t tRet = 0;
	
	tRet |= csi_ept_init(&g_tEpt, EPT_IDX);								//初始化EPT设备
	tRet |= csi_ept_set_freq(&g_tEpt, 20000);							//EPT工作于20KHz
	tRet |= csi_ept_set_ch_dualout_md1(&g_tEpt, EPT_CH_A, 40, 500, 500);//通道A两路互补输出，占空比40%， 左右死区时间500ns
	csi_ept_set_start_mode(&g_tEpt, EPT_SW);
	csi_ept_set_os_mode(&g_tEpt, EPT_OP_CONT);
	
	tRet |= csi_ept_set_evtrg(&g_tEpt, 0, EPT_EVSRC_PRD, 1);			//将PRD事件作为TRG0的输出事件，一次PRD事件即触发输出
	csi_ept_evtrg_enable(&g_tEpt, 0, ENABLE);							//使能TRG0输出
	
	return tRet;
}

csi_error_t senario2_ept_config(void)
{
	csi_error_t tRet = 0;
	csi_ept_em_out_e eEmOut;
	
	tRet |= csi_ept_init(&g_tEpt, EPT_IDX);
	tRet |= csi_ept_set_freq(&g_tEpt, 20000);								//20KHz
	tRet |= csi_ept_set_ch_dualout_md2(&g_tEpt, EPT_CH_A, 20, 500, 500);	//通道A两路互补输出，占空比20%，左右各500ns死区时间
	csi_ept_set_start_mode(&g_tEpt, EPT_SW);								//软件触发
	csi_ept_set_os_mode(&g_tEpt, EPT_OP_CONT);								//EPT连续模式
		
	eEmOut.eChaxOut = EPT_EM_OUT_H;											//设置锁定发生时，管脚输出状态
	eEmOut.eChayOut = EPT_EM_OUT_H;


	csi_ept_set_em(&g_tEpt, EPT_EBI3, EPT_EBI_POL_H, EPT_EP_HLCK, eEmOut, DISABLE);	//EBI1 高电平触发 硬锁，不产生中断	
	
	return tRet;
}

csi_error_t senario3_ept_config(void)
{
	csi_error_t tRet = 0;
	csi_ept_em_out_e eEmOut;
	
	tRet |= csi_ept_init(&g_tEpt, EPT_IDX);
	tRet |= csi_ept_set_freq(&g_tEpt, 20000);								//20KHz
	tRet |= csi_ept_set_ch_dualout_md1(&g_tEpt, EPT_CH_A, 20, 500, 500);	//通道A两路互补输出，占空比20%，左右各500ns死区时间
	csi_ept_set_start_mode(&g_tEpt, EPT_SW);								//软件触发
	csi_ept_set_os_mode(&g_tEpt, EPT_OP_CONT);								//EPT连续模式
		
	eEmOut.eChaxOut = EPT_EM_OUT_H;											//设置锁定发生时，管脚输出状态
	eEmOut.eChayOut = EPT_EM_OUT_H;

	csi_ept_set_em(&g_tEpt, EPT_EBI1, EPT_EBI_POL_H, EPT_EP_HLCK, eEmOut, DISABLE);	//EBI1 高电平触发 硬锁，不产生中断	
	
	return tRet;
}

csi_error_t senario1_etb_config(void)
{
	uint8_t ch;
	csi_error_t tRet = CSI_OK;
	
	tRet |= csi_etb_init();
	tEtbConfig.ch_type = ETB_CH_ONE_TRIGGER_ONE;
	tEtbConfig.src_ip = 0x10;  	//EPT0_TRGOUT0
	tEtbConfig.src_ip1 = 0xff;
	tEtbConfig.src_ip2 = 0xff;	
	tEtbConfig.dst_ip = 0x6;    //ADC_SYNCIN0
	tEtbConfig.dst_ip1 = 0xff;
	tEtbConfig.dst_ip2 = 0xff;
	tEtbConfig.trig_mode = ETB_HARDWARE_TRIG;
	ch = csi_etb_ch_alloc(tEtbConfig.ch_type);
	tRet |= csi_etb_ch_config(ch, &tEtbConfig);
	
	return tRet;
}


/*
 * 边沿对齐，输出
 */ 
void ept_senario4(void)
{
	csi_pin_set_mux(EPT_PWMCHAX_PIN, EPT_PWMCHAX_PIN_FUNC);		//根据board_config.h的设定，配置EPT A通道的两路互补输出
	csi_pin_set_mux(EPT_PWMCHAY_PIN, EPT_PWMCHAY_PIN_FUNC);
	
	csi_ept_init(&g_tEpt, 0);									//初始化EPT设备
	csi_ept_set_freq_edge_aligned(&g_tEpt, 50);							//EPT工作于20KHz
	csi_ept_set_singleout_edge_aligned_md1(&g_tEpt, EPT_CH_A, 40);	//通道单路输出，占空比40%
	csi_ept_set_start_mode(&g_tEpt, EPT_SW);							
	csi_ept_set_os_mode(&g_tEpt, EPT_OP_CONT);
	
	csi_ept_swstart(&g_tEpt);
	while(1);
}