/***********************************************************************//** 
 * \file  gpta_demo.c
 * \brief  TIMER_DEMO description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-11 <td>V0.0 <td>ljy     <td>initial
 * </table>
 * *********************************************************************
*/
/* Includes ---------------------------------------------------------------*/
#include <string.h>
#include <drv/gpta.h>
#include <drv/pin.h>

#include "demo.h"
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private macro-----------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/


/** \brief gpta
 * 
 *  \param[in] none
 *  \return error code
 */
int gpta_demo(void)
{
	int iRet = 0;	
//------------------------------------------------------------------------------------------------------------------------	
	csi_pin_set_mux(PA010, PA010_GPT_CHA);						//
	csi_pin_set_mux(PA011, PA011_GPT_CHB);						//
//------------------------------------------------------------------------------------------------------------------------	
	csi_gpta_config_t tPwmCfg;								  
	tPwmCfg.byWorkmod       = GPTA_WAVE;                        //WAVE or CAPTURE    //计数或捕获	
	tPwmCfg.byCountingMode  = GPTA_UPCNT;                       //CNYMD  //计数方向
	tPwmCfg.byOneshotMode   = GPTA_OP_CONT;                     //OPM    //单次或连续(工作方式)
	tPwmCfg.byStartSrc      = GPTA_SYNC_START;				   //软件使能同步触发使能控制（RSSR中START控制位）//启动方式
	tPwmCfg.byPscld         = GPTA_LDPSCR_ZRO;                  //PSCR(分频)活动寄存器载入控制。活动寄存器在配置条件满足时，从影子寄存器载入更新值
	
	if(tPwmCfg.byWorkmod==GPTA_WAVE)                            //工作在波形模式
	{
	    tPwmCfg.byDutyCycle 	 = 50;								//pwm ouput duty cycle//PWM初始值			
	    tPwmCfg.wFreq 			 = 10000;							//pwm ouput frequency
		
	}
	else{ tPwmCfg.wFreq 		 = 10000;                         //捕获频率大概范围
		tPwmCfg.byCaptureCapLden = true ;                         //CMPA和CMPB在捕捉事件载入使能
		tPwmCfg.byCaptureRearm   = true ;                         //1:重置捕捉计数器
		tPwmCfg.byCaptureCapmd   = 0;                             //0:连续捕捉模式    1h：一次性捕捉模式
		tPwmCfg.byCaptureStopWrap=4-1;                            //Capture模式下，捕获事件计数器周期设置值
		tPwmCfg.byCaptureLdaret  =0;                              //CMPA捕捉载入后，计数器值计数状态控制位(1h：CMPA触发后，计数器值进行重置;0h：CMPA触发后，计数器值不进行重置)
		tPwmCfg.byCaptureLdbret  =0;                              
		tPwmCfg.byCaptureLdcret  =0;                              
		tPwmCfg.byCaptureLddret  =0;                              
	}
	
	tPwmCfg.byBurst   = false ;                                   //使能群脉冲模式
	tPwmCfg.byCgsrc   = GPTA_CGSRC_TIOA;                           //CHAX作为CG的输入源
	tPwmCfg.byCgflt   = GPTA_CGFLT_2;                              //门控输入数字滤波控制
		
	tPwmCfg.byInter 		 = GPTA_INT_TRGEV0;                    //interrupt
	csi_gpta_config_init(GPT0, &tPwmCfg);

//------------------------------------------------------------------------------------------------------------------------
	csi_gpta_pwmchannel_config_t  tEptchannelCfg;
	tEptchannelCfg.byActionZro    =   LO;
	tEptchannelCfg.byActionPrd    =   NA;
	tEptchannelCfg.byActionCau    =   HI;
	tEptchannelCfg.byActionCad    =   LO;
	tEptchannelCfg.byActionCbu    =   NA;
	tEptchannelCfg.byActionCbd    =   NA;
	tEptchannelCfg.byActionT1u    =   LO;
	tEptchannelCfg.byActionT1d    =   LO;
	tEptchannelCfg.byActionT2u    =   NA;
	tEptchannelCfg.byActionT2d    =   NA;
	tEptchannelCfg.byChoiceCasel  =   GPTA_CMPA;
	tEptchannelCfg.byChoiceCbsel  =   GPTA_CMPA;	
	csi_gpta_channel_config(GPT0, &tEptchannelCfg,  GPTA_CHANNEL_A);//channel
	csi_gpta_channel_config(GPT0, &tEptchannelCfg,  GPTA_CHANNEL_B);

//------------------------------------------------------------------------------------------------------------------------

    csi_gpta_Global_load_control_config_t  Globaldemo;
	Globaldemo.bGlden    =   DISABLE;                            //全局的Shadow到Active寄存器载入控制使能
	Globaldemo.byGldmd   =   GPTA_LDGLD_ZRO_PRD_LD_SYNC;         //全局载入触发事件选择
	Globaldemo.bOstmd    =   GPTA_LDMD_ANYTIME;                  //One Shot 载入模式使能控制位
	Globaldemo.bGldprd   =   0 ;                                 //全局载入触发周期选择。可以选择N次触发条件满足后，才进行一次全局载入。	                       
	csi_gpta_global_config(GPT0,&Globaldemo);
	csi_gpta_global_rearm(GPT0) ;                                //重置ONE SHOT模式。ONE SHOT模式下，一次触发后，需要重置模式才允许再次触发
	csi_gpta_global_sw(GPT0) ;                                   //软件产生一次GLD触发
//------------------------------------------------------------------------------------------------------------------------	
	csi_gpta_start(GPT0);//start  timer
    while(1){ 
			csi_gpta_change_ch_duty(GPT0,GPTA_CH_A, 25);
		    mdelay(200);                        
		    csi_gpta_change_ch_duty(GPT0,GPTA_CH_A, 50);	
		    mdelay(200);
	}			
	return iRet;
};



int gpta_demo1(void)
{
	int iRet = 0;	
//------------------------------------------------------------------------------------------------------------------------	
	csi_pin_set_mux(PA010, PA010_GPT_CHA);						//
	csi_pin_set_mux(PA011, PA011_GPT_CHB);						//
//------------------------------------------------------------------------------------------------------------------------	
	csi_gpta_captureconfig_t tPwmCfg;								  
		tPwmCfg.byWorkmod       = GPTA_CAPTURE;                     //WAVE or CAPTURE    //计数或捕获	
		tPwmCfg.byCountingMode  = GPTA_UPCNT;                       //CNYMD  //计数方向
		tPwmCfg.byOneshotMode   = GPTA_OP_CONT;                     //OPM    //单次或连续(工作方式)
		tPwmCfg.byStartSrc      = GPTA_SYNC_START;				   //软件使能同步触发使能控制（RSSR中START控制位）//启动方式
	    tPwmCfg.byPscld         = GPTA_LDPSCR_ZRO;                  //PSCR(分频)活动寄存器载入控制。活动寄存器在配置条件满足时，从影子寄存器载入更新值	
		tPwmCfg.byCaptureCapmd   = 0;                              //0:连续捕捉模式    1h：一次性捕捉模式
		tPwmCfg.byCaptureStopWrap=2-1;                             //Capture模式下，捕获事件计数器周期设置值
		tPwmCfg.byCaptureLdaret  =0;                               //CMPA捕捉载入后，计数器值计数状态控制位(1h：CMPA触发后，计数器值进行重置;0h：CMPA触发后，计数器值不进行重置)
		tPwmCfg.byCaptureLdbret  =0;                                                         	
	    tPwmCfg.byInter 		 =GPTAINT_CAPLD1;                 //interrupt//
	csi_gpta_capture_init(GPT0, &tPwmCfg);

//------------------------------------------------------------------------------------------------------------------------
    csi_gpta_set_sync(GPT0, GPTA_TRGIN_SYNCEN2, GPTA_TRG_CONTINU, GPTA_AUTO_REARM_DIS);//使能SYNCIN2外部触发
	
	csi_gpta_set_extsync_chnl(GPT0, GPTA_TRGIN_SYNCEN2,0);                             //SYNCIN2--TRGSEL0
	csi_gpta_set_evtrg(GPT0, GPTA_TRG_OUT0, GPTA_TRG01_SYNC);                          //TRGSEL0	
	csi_gpta_int_enable(GPT0, GPTA_INT_TRGEV0,true);
	
	csi_gpta_start(GPT0);//start  timer
    while(1){		
		  		      
		    mdelay(200);                        
		    	
		    mdelay(200);
	}			
	return iRet;
};

int gpta_demo12(void)
{
	int iRet = 0;	
//------------------------------------------------------------------------------------------------------------------------	
	csi_pin_set_mux(PA010, PA010_GPT_CHA);						//
	csi_pin_set_mux(PA011, PA011_GPT_CHB);						//
//------------------------------------------------------------------------------------------------------------------------		
csi_gpta_pwmconfig_t tPwmCfg;								  
	tPwmCfg.byWorkmod        = GPTA_WAVE;                        //WAVE  波形模式
	tPwmCfg.byCountingMode   = GPTA_UPCNT;                       //CNYMD  //计数方向
	tPwmCfg.byOneshotMode    = GPTA_OP_CONT;                     //OPM    //单次或连续(工作方式)
	tPwmCfg.byStartSrc       = GPTA_SYNC_START;					//软件使能同步触发使能控制（RSSR中START控制位）//启动方式
	tPwmCfg.byPscld          = GPTA_LDPSCR_ZRO;                  //PSCR(分频)活动寄存器载入控制。活动寄存器在配置条件满足时，从影子寄存器载入更新值		
	tPwmCfg.byDutyCycle 	 = 50;								//pwm ouput duty cycle//PWM初始值			
	tPwmCfg.wFreq 			 = 10000;							//pwm ouput frequency	
    
	tPwmCfg.byInter 		 = GPTAINT_PEND;                     //interrupt
	csi_gpta_wave_init(GPT0, &tPwmCfg);

   
//------------------------------------------------------------------------------------------------------------------------	
	csi_gpta_pwmchannel_config_t  tEptchannelCfg;
	tEptchannelCfg.byActionZro    =   LO;
	tEptchannelCfg.byActionPrd    =   NA;
	tEptchannelCfg.byActionCau    =   HI;
	tEptchannelCfg.byActionCad    =   LO;
	tEptchannelCfg.byActionCbu    =   NA;
	tEptchannelCfg.byActionCbd    =   NA;
	tEptchannelCfg.byActionT1u    =   LO;
	tEptchannelCfg.byActionT1d    =   LO;
	tEptchannelCfg.byActionT2u    =   NA;
	tEptchannelCfg.byActionT2d    =   NA;
	tEptchannelCfg.byChoiceCasel  =   GPTA_CMPA;
	tEptchannelCfg.byChoiceCbsel  =   GPTA_CMPA;	
	csi_gpta_channel_config(GPT0, &tEptchannelCfg,  GPTA_CHANNEL_A);//channel
	csi_gpta_channel_config(GPT0, &tEptchannelCfg,  GPTA_CHANNEL_B);

	
//------------------------------------------------------------------------------------------------------------------------		
	csi_gpta_start(GPT0);//start  timer
	while(1){	
		    csi_gpta_change_ch_duty(GPT0,GPTA_CH_A, 25);		      
		    mdelay(200);                        
		    csi_gpta_change_ch_duty(GPT0,GPTA_CH_A, 50);	
		    mdelay(200);	
	}
	return iRet;
}



