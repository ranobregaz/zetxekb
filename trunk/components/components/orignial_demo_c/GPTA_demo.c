/***********************************************************************//** 
 * \file  GPTA_demo.c
 * \brief  GPTA 示例代码 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-24 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/



#include "pin.h"
#include "gpta.h"
#include "etb.h"
#include "board_config.h"
#include "iostring.h"
/* externs function--------------------------------------------------------*/
/* private function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
uint32_t g_GptAIrq = 0;
uint32_t g_wTick = 0;

/* device instance------------------------------------------------------*/
csi_gpta_t g_tGpta;


void user_gpta_irq(csi_gpta_t *ptGpta, uint32_t wInt, void *arg)
{	volatile uint32_t wStamp[2];//1, byStamp2;
	if (wInt & GPTAINT_PEND) {
		g_GptAIrq = 1;
	}
	else if (wInt & GPTAINT_CAPLD1) {
			
		//wStamp[0] = csi_gpta_capture_get_cap_data(&g_tGpta, 0) ;	
		wStamp[1] = csi_gpta_capture_get_cap_data(&g_tGpta, 1);
		my_printf("time = %d ns\n",(wStamp[1])*g_wTick );
		csi_gpta_out_swstop(ptGpta);
	}
	
}



/**
 \brief 示例 GPTA双通道（CHA， CHB）输出模式 1/2/3/4
  * 
  * 互补模式1：CHA active High ,ChB active High
  * \verbatim
  *  	CHA    		______
  *  		_______|      |_______ 
  *   	CHB
  *  		_____			_______
  *      		|__________|
  * \endverbatim  
  * 互补模式2：CHA active Low ,ChB active Low
  *	\verbatim 
  *		CHA    ______		 _____
  *				     |_______| 
  *		CHB	      	__________
  *			   _____|         |______	
  * \endverbatim  
  * 互补模式3：CHA active High ,ChB active Low
  * \verbatim
  *  	CHA     	______
  *  		_______|      |_______ 
  *   	CHB        _________
  *  		______|			|_______
  *    
  * \endverbatim
  * 互补模式4：CHA active Low ,ChB active High
  *	\verbatim 
  *		CHA    ______		 _____
  *				     |_______| 
  *		CHB 	_____          ____
  *			        |_________|	
  * \endverbatim
*/
csi_error_t gpta_senario0(void)
{
	csi_error_t tRet = CSI_OK;
	
	csi_pin_set_mux(PA09, PA09_GPT_CHA);				//管脚设置。GPTA的这组管脚不在board_config.h里。
	csi_pin_set_mux(PA011, PA011_GPT_CHB);
	
	csi_gpta_init(&g_tGpta, 0);
	tRet = csi_gpta_set_outfreq(&g_tGpta, 20000);		// 20KHz
	tRet = csi_gpta_set_dualout_md1(&g_tGpta, 20, 500);	// 模式1， 占空比20%， 左右死区时间500ns。死区时间小于1ns会报错，且设置无效。
	csi_gpta_set_start_mode(&g_tGpta, GPTA_SW);			// 软件触发
	csi_gpta_set_os_mode(&g_tGpta, GPTA_OP_CONT); 		//  GPTA连续工作模式
	csi_gpta_swstart(&g_tGpta);							// 开始计数
	
	mdelay(2000);										//延时2s
	tRet = csi_gpta_set_dualout_md2(&g_tGpta, 20, 500);	// 模式2， 占空比20%， 左右死区时间500ns。死区时间小于1ns会报错，且设置无效。
	mdelay(2000);
	tRet = csi_gpta_set_dualout_md3(&g_tGpta, 20, 500);	// 模式3， 占空比20%， 左右死区时间500ns。死区时间小于1ns会报错，且设置无效。
	mdelay(2000);
	tRet = csi_gpta_set_dualout_md4(&g_tGpta, 20, 500);	// 模式4， 占空比20%， 左右死区时间500ns。死区时间小于1ns会报错，且设置无效。
	
	while(1);
	return tRet;
}


/**
 \brief 示例 GPTA 单通道输出模式 1和 2。
  * 
  * 单通道模式1：CHx active high
  * \verbatim
  *  	CHx      	______
  *  		_______|      |_______ 
  * \endverbatim
  * 单通道模式2：CHx active high
  * \verbatim
  *		CHx	    ______		  _____
  *				     |_______|
  * \endverbatim	
*/
csi_error_t gpta_senario1(void)
{	csi_error_t tRet = CSI_OK;
	csi_pin_set_mux(PA09, PA09_GPT_CHA);
	csi_pin_set_mux(PA011, PA011_GPT_CHB);
	

	csi_gpta_init(&g_tGpta, 0);									// 初始化GPTA0
	csi_gpta_set_outfreq(&g_tGpta, 10000);						// 10KHz
	tRet =csi_gpta_set_singleout_md1(&g_tGpta, GPTA_CH_A, 20);	// CHA模式1,占空比20%
	tRet =csi_gpta_set_singleout_md1(&g_tGpta, GPTA_CH_B, 40);	// CHB模式2， 占空比40%
	csi_gpta_set_start_mode(&g_tGpta, GPTA_SW);					// 软件触发
	csi_gpta_set_os_mode(&g_tGpta, GPTA_OP_CONT);				//  GPTA连续工作模式
	csi_gpta_swstart(&g_tGpta);									// 开始计数
	
	mdelay(2000);												//延时2s
	tRet =csi_gpta_set_singleout_md2(&g_tGpta, GPTA_CH_A, 20);	// CHA模式2,占空比20%
	tRet =csi_gpta_set_singleout_md2(&g_tGpta, GPTA_CH_B, 40);	// CHB模式2,占空比20%
	
	while(1);
	return tRet;
}



/**
 \brief 示例 GPTA 定时中断
*/
csi_error_t gpta_senario2(void)
{
	csi_error_t tRet = CSI_OK;
	g_GptAIrq =0;
	
	csi_pin_set_mux(PA09, PA09_GPT_CHA);
	csi_pin_set_mux(PA011, PA011_GPT_CHB);
	
	csi_gpta_init(&g_tGpta, 0);									// 初始化GPTA0
	csi_gpta_attach_callback(&g_tGpta, user_gpta_irq, (void *)GPTAINT_PEND);	//注册用户中断回调函数， 在Period End时触发中断。
	csi_gpta_set_outfreq(&g_tGpta, 5000);						// 5KHz
	csi_gpta_set_singleout_md1(&g_tGpta, GPTA_CH_A, 20);		// CHA 模式1， 占空比20%
	csi_gpta_set_start_mode(&g_tGpta, GPTA_SW);					// 软件触发
	csi_gpta_set_os_mode(&g_tGpta, GPTA_OP_CONT);				//  GPTA连续工作模式
	csi_gpta_set_stop_st(&g_tGpta, GPTA_STP_HZ);				// 停止时，管脚高阻状态
	csi_gpta_swstart(&g_tGpta);									// 开始计数
	
	mdelay(100);
	if (g_GptAIrq == 0)											// 进中断，变量会置高
		tRet = CSI_ERROR;
		
	csi_gpta_out_swstop(&g_tGpta);								// 停止计数
	
	return tRet;
}


/** 
 * \brief 示例 GPTA capture 模式测量高电平持续时间 
 * \verbatim
 *  PB02 Rising ---> ETB --> GPTA SYNCIN0（开始计数的同时，load CMPA）
 *  PB02 Falling ---> ETB --> GPTA SYNCIN2 （load CMPB）
 * \endverbatim
 */

csi_error_t gpta_senario3(void)
{
	csi_error_t tRet = CSI_OK;
	uint32_t ch;
	csi_etb_config_t tEtbConfig;
	csi_gpio_t g_tGpioB0;
	
	g_GptAIrq = 0;
	
	csi_gpta_init(&g_tGpta, 0);														//初始化GPTA设备
	
	csi_gpta_set_os_mode(&g_tGpta, GPTA_OP_CONT);
	csi_gpta_set_stop_st(&g_tGpta, GPTA_STP_HZ);									// 停止时，管脚高阻状态
	
	g_wTick = csi_gpta_cap_width_config(&g_tGpta, GPTA_CAP_OT, 2);						// 只capture一次， 一次装载2次load。 g_wTick为单位时间（ns）。
	csi_gpta_attach_callback(&g_tGpta, user_gpta_irq, (void *) (GPTAINT_CAPLD1)); 	// 使能第四次load中断，注册用户中断回调函数
	
	csi_etb_init();									//配置ETCB： 一对一，将EXI_TRGOUT0与GPT_SYNIN0连起来
	tEtbConfig.ch_type = ETB_CH_ONE_TRIGGER_ONE;	
	tEtbConfig.src_ip = 0x4;  	//EXI_TRGOUT0		
	tEtbConfig.src_ip1 = 0xff;
	tEtbConfig.src_ip2 = 0xff;	
	tEtbConfig.dst_ip = 0x24;    //GPT_SYNIN0（GPTA开始计数事件）	
	tEtbConfig.dst_ip1 = 0xff;
	tEtbConfig.dst_ip2 = 0xff;
	tEtbConfig.trig_mode = ETB_HARDWARE_TRIG;
	ch = csi_etb_ch_alloc(tEtbConfig.ch_type);
	csi_etb_ch_config(ch, &tEtbConfig);
	
	tEtbConfig.ch_type = ETB_CH_ONE_TRIGGER_ONE;	//配置ETCB： 一对一，将EXI_TRGOUT4与GPT_SYNIN2连起来
	tEtbConfig.src_ip = 0x8;  	//EXI_TRGOUT4		
	tEtbConfig.src_ip1 = 0xff;
	tEtbConfig.src_ip2 = 0xff;
	tEtbConfig.dst_ip = 0x26;  	//GPT_SYNIN2（GPTA load事件）
	tEtbConfig.dst_ip1 = 0xff;
	tEtbConfig.dst_ip2 = 0xff;
	tEtbConfig.trig_mode = ETB_HARDWARE_TRIG;
	ch = csi_etb_ch_alloc(tEtbConfig.ch_type);
	csi_etb_ch_config(ch, &tEtbConfig);
	
	
	//PB0.4 EXI0
	csi_gpio_init(&g_tGpioB0, GPIO_IDX);														// 初始化GPIOB 设备
	csi_gpio_dir(&g_tGpioB0, GPIO_PIN_MSK, GPIO_DIRECTION_INPUT);								// PB0.2 设为输入	
	csi_gpio_irq_mode_ex(&g_tGpioB0, GPIO_PIN_MSK, EXI_GRP2, GPIO_IRQ_MODE_RISING_EDGE);		// PB0.2 设为EXI 0上升沿触发GPTA开始计数	
	csi_gpio_irq_mode_ex(&g_tGpioB0, GPIO_PIN_MSK, EXI_GRP18, GPIO_IRQ_MODE_FALLING_EDGE);		// 同时将PB0.2视为 EXI18（扩展），下降沿load counter值
	
	//syscon configuration
	csi_exi_set_evtrg(0, TRGSRC_EXI2, 1);			// EXI_TRG0 的源设为EXI2， 1次即输出触发。 
	csi_exi_set_evtrg(4, TRGSRC_EXI18, 1);			// EXI_TRG4 的源设为EXI18，1次即输出触发。
		
		
	while(1);
	return tRet;
}

/** 
 * \brief 示例 GPTA capture 模式测周期时间（PB0.2 下降沿间隔） 
 * 		注意：csi_gpta_cap_prd_config（）中将
 * \verbatim
 *  PB02 Falling ---> ETB --> GPTA SYNCIN2 （load CMPB）
 * \endverbatim
 */

csi_error_t gpta_senario4(void)
{
	csi_error_t tRet = CSI_OK;
	uint32_t ch;
	csi_etb_config_t tEtbConfig;
	csi_gpio_t g_tGpioB0;
	
	g_GptAIrq = 0;
	
	csi_gpta_init(&g_tGpta, 0);														//初始化GPTA设备
	
	csi_gpta_set_os_mode(&g_tGpta, GPTA_OP_CONT);
	csi_gpta_set_stop_st(&g_tGpta, GPTA_STP_HZ);									// 停止时，管脚高阻状态
	
	g_wTick = csi_gpta_cap_prd_config(&g_tGpta, GPTA_CAP_OT, 2);						// 只capture一次， 一次装载2次load。 g_wTick为单位时间（ns）。
	csi_gpta_attach_callback(&g_tGpta, user_gpta_irq, (void *) (GPTAINT_CAPLD1)); 	// 使能第2次load中断，注册用户中断回调函数
	
	csi_etb_init();									
		
	tEtbConfig.ch_type = ETB_CH_ONE_TRIGGER_ONE;	//配置ETCB： 一对一，将EXI_TRGOUT4与GPT_SYNIN2连起来
	tEtbConfig.src_ip = 0x4;  	//EXI_TRGOUT0		
	tEtbConfig.src_ip1 = 0xff;
	tEtbConfig.src_ip2 = 0xff;
	tEtbConfig.dst_ip = 0x26;  	//GPT_SYNIN2（GPTA load事件）
	tEtbConfig.dst_ip1 = 0xff;
	tEtbConfig.dst_ip2 = 0xff;
	tEtbConfig.trig_mode = ETB_HARDWARE_TRIG;
	ch = csi_etb_ch_alloc(tEtbConfig.ch_type);
	csi_etb_ch_config(ch, &tEtbConfig);
	
	
	//PB0.2 EXI0
	csi_gpio_init(&g_tGpioB0, GPIO_IDX);														// 初始化GPIOB 设备
	csi_gpio_dir(&g_tGpioB0, GPIO_PIN_MSK, GPIO_DIRECTION_INPUT);								// PB0.2 设为输入	

	csi_gpio_irq_mode_ex(&g_tGpioB0, GPIO_PIN_MSK, EXI_GRP2, GPIO_IRQ_MODE_FALLING_EDGE);		// 同时将PB0.2视为 EXI2，下降沿load counter值
	
	//syscon configuration
	csi_exi_set_evtrg(0, TRGSRC_EXI2, 1);			// EXI_TRG0 的源设为EXI2，1次即输出触发。
		
	csi_gpta_swstart(&g_tGpta);
	while(1);
	return tRet;
}
