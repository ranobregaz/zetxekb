/***********************************************************************//** 
 * \file  ept_demo.c
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
#include <drv/ept.h>
#include <drv/pin.h>

#include "demo.h"
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private macro-----------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/


/** \brief ept
 * 
 *  \param[in] none
 *  \return error code
 */
int ept_demo(void)
{
	int iRet = 0;	
//------------------------------------------------------------------------------------------------------------------------	
	csi_pin_set_mux(PA015, PA015_EPT_CHAX);						//
	csi_pin_set_mux(PA014, PA014_EPT_CHBX);						//
	csi_pin_set_mux(PB05, PB05_EPT_CHCX);						//	
	csi_pin_set_mux(PA012, PA012_EPT_CHAY);						//	
	csi_pin_set_mux(PA08, PA08_EPT_CHBY);						//
	csi_pin_set_mux(PA04, PA04_EPT_CHCY);
	
//	csi_pin_set_mux(PA07,PA07_EPI0);
	csi_pin_set_mux(PA013,PA013_EPI1);
	csi_pin_set_mux(PB03, PB03_EPI2);
//	csi_pin_set_mux(PB02, PB02_EPI3);
//------------------------------------------------------------------------------------------------------------------------	
	csi_ept_config_t tPwmCfg;								  
	tPwmCfg.byWorkmod       = EPT_WAVE;                        //WAVE or CAPTURE    //计数或捕获	
	tPwmCfg.byCountingMode  = EPT_UPCNT;                       //CNYMD  //计数方向
	tPwmCfg.byOneshotMode   = EPT_OP_CONT;                     //OPM    //单次或连续(工作方式)
	tPwmCfg.byStartSrc      = EPT_SYNC_START;				   //软件使能同步触发使能控制（RSSR中START控制位）//启动方式
	tPwmCfg.byPscld         = EPT_LDPSCR_ZRO;                  //PSCR(分频)活动寄存器载入控制。活动寄存器在配置条件满足时，从影子寄存器载入更新值
	
	if(tPwmCfg.byWorkmod==EPT_WAVE)                            //工作在波形模式
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
	tPwmCfg.byCgsrc   = EPT_CGSRC_TIOA;                           //CHAX作为CG的输入源
	tPwmCfg.byCgflt   = EPT_CGFLT_2;                              //门控输入数字滤波控制
		
	tPwmCfg.byInter 		 = EPT_INT_TRGEV0;                      //interrupt
	csi_ept_config_init(EPT0, &tPwmCfg);

//------------------------------------------------------------------------------------------------------------------------
	csi_ept_pwmchannel_config_t  tEptchannelCfg;
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
	tEptchannelCfg.byChoiceCasel  =   EPT_CMPA;
	tEptchannelCfg.byChoiceCbsel  =   EPT_CMPA;	
	csi_ept_channel_config(EPT0, &tEptchannelCfg,  EPT_CHANNEL_A);//channel
	csi_ept_channel_config(EPT0, &tEptchannelCfg,  EPT_CHANNEL_B);
	csi_ept_channel_config(EPT0, &tEptchannelCfg,  EPT_CHANNEL_C);
	csi_ept_channel_config(EPT0, &tEptchannelCfg,  EPT_CHANNEL_D);
	
    //csp_ept_set_aqtscr(EPT0,EPT_T1,EP1);//波形输出T事件选择
//------------------------------------------------------------------------------------------------------------------------	

	csi_ept_dbldrload_config(EPT0,DBCR,EPT_SHDW_IMMEDIATE,EPT_LD_ZRO);
	
//------------------------------------------------------------------------------------------------------------------------	
	csi_ept_deadzone_config_t  tEptDeadZoneTime;
	tEptDeadZoneTime.byDcksel               = EPT_DB_DPSC;     //
	tEptDeadZoneTime.hwDpsc                 =  0;              //FDBCLK = FHCLK / (DPSC+1)
	tEptDeadZoneTime.hwRisingEdgereGister = 24;                //NS/(1/(48000000/(DPSC+1))*10^9) // 500NS/(1000/48) = 24;
	tEptDeadZoneTime.hwFallingEdgereGister= 24;                //下降沿
	tEptDeadZoneTime.byChaDedb             = DB_AR_BF;         //不使用死区双沿
	tEptDeadZoneTime.byChbDedb             = DB_AR_BF;
	tEptDeadZoneTime.byChcDedb             = DB_AR_BF;
	csi_ept_dbcr_config(EPT0,&tEptDeadZoneTime);
	
	tEptDeadZoneTime.byChxOuselS1S0      = E_DBOUT_AR_BF;      //使能通道A的上升沿延时，使能通道B的下降沿延时
	tEptDeadZoneTime.byChxPolarityS3S2   = E_DB_POL_B;         //通道A和通道B延时输出电平是否反向
	tEptDeadZoneTime.byChxInselS5S4      = E_DBCHAIN_AR_AF;    //PWMA作为上升沿和下降沿延时处理的输入信号
	tEptDeadZoneTime.byChxOutSwapS8S7    = E_CHOUTX_OUA_OUB;   //OUTA=通道A输出，OUTB=通道B输出
    csi_ept_channelmode_config(EPT0,&tEptDeadZoneTime,EPT_CHANNEL_A);
	csi_ept_channelmode_config(EPT0,&tEptDeadZoneTime,EPT_CHANNEL_B);
    csi_ept_channelmode_config(EPT0,&tEptDeadZoneTime,EPT_CHANNEL_C);
	//---------------------------------------------------------------
	csi_ept_Chopper_config_t  tEptChopperOutCfg;
	tEptChopperOutCfg.byOspwth             =3 ;                 //首脉冲的宽度可以配置为载波周期的整数倍。当该控制位设为零时，
														        //所有脉冲宽度均由byCdiv和byCduty配置.Twidth = Tchop x byOspwth （Tchop为一个载波的周期时间）
	tEptChopperOutCfg.byCdiv               =9 ;                 //载波频率设置。载波的频率设置基于PCLK的8倍分频进行设置 Fchop = PCLK / ((byCdiv+1) x 8)
	tEptChopperOutCfg.byCduty              =EPT_CDUTY_4_8 ;     //载波的占空比设置  0h：禁止载波;   1h：Duty = 7/8------7h：Duty = 1/8
	tEptChopperOutCfg.byCasel              =EPT_CPSRC_IN ;      //载波信号源选择控制位: 0h：EPT内部产生载波;  1h：TIN的输入
	csi_ept_choppercpcr_config(EPT0,&tEptChopperOutCfg);

	csi_ept_chopper_config(EPT0,EPTCHAX,DISABLE);                //斩波输出使能控制位   0b：禁止当前通道斩波输出    1b：开启当前通道斩波输出
	csi_ept_chopper_config(EPT0,EPTCHAY,DISABLE);
	csi_ept_chopper_config(EPT0,EPTCHBX,DISABLE);
	csi_ept_chopper_config(EPT0,EPTCHBY,DISABLE);
	csi_ept_chopper_config(EPT0,EPTCHCX,DISABLE);
	csi_ept_chopper_config(EPT0,EPTCHCY,DISABLE);
			
//------------------------------------------------------------------------------------------------------------------------
    csi_ept_emergency_config_t   tEptEmergencyCfg;           //紧急状态输入
    tEptEmergencyCfg.byEpxInt    = EBI1 ;                    //EPx选择外部IO端口（EBI0~EBI4）
    tEptEmergencyCfg.byPolEbix   = EBI_POL_H;                //EBIx的输入有效极性选择控制
	tEptEmergencyCfg.byEpx       = EP1;                      //使能EPx
	tEptEmergencyCfg.byEpxLckmd  = EP_HLCK;                   //使能 锁
    tEptEmergencyCfg.byFltpace0  = EPFLT0_2P;                 //EP0、EP1、EP2和EP3的数字去抖滤波检查周期数
	tEptEmergencyCfg.byFltpace1  = EPFLT1_2P;                 //EP4、EP5、EP6和EP7的数字去抖滤波检查周期数
	if(tEptEmergencyCfg.byEpxInt ==ORL0){tEptEmergencyCfg.byOrl0 = ORLx_EP0 |ORLx_EP1|ORLx_EP2;}
	if(tEptEmergencyCfg.byEpxInt ==ORL1){tEptEmergencyCfg.byOrl1 = ORLx_EP4 |ORLx_EP5|ORLx_EP6;}
	csi_ept_emergency_cfg(EPT0,&tEptEmergencyCfg);
	
	csi_ept_emergency_pinxout(EPT0,EMCOAX,EM_OUT_L);
	csi_ept_emergency_pinxout(EPT0,EMCOBX,EM_OUT_L);
	csi_ept_emergency_pinxout(EPT0,EMCOCX,EM_OUT_L);
	csi_ept_emergency_pinxout(EPT0,EMCOAY,EM_OUT_L);
	csi_ept_emergency_pinxout(EPT0,EMCOBY,EM_OUT_L);
	csi_ept_emergency_pinxout(EPT0,EMCOCY,EM_OUT_L);
	
	csi_ept_emergency_interruption_en(EPT0,EPT_INT_EP1);      //紧急状态输入中断使能
//------------------------------------------------------------------------------------------------------------------------	

//------------------------------------------------------------------------------------------------------------------------

    csi_ept_Global_load_control_config_t  Globaldemo;
	Globaldemo.bGlden    =   DISABLE;                           //全局的Shadow到Active寄存器载入控制使能
	Globaldemo.byGldmd   =   EPT_LDGLD_ZRO_PRD_LD_SYNC;         //全局载入触发事件选择
	Globaldemo.bOstmd    =   EPT_LDMD_ANYTIME;                  //One Shot 载入模式使能控制位
	Globaldemo.bGldprd   =   0 ;                                //全局载入触发周期选择。可以选择N次触发条件满足后，才进行一次全局载入。	                       
	csi_ept_global_config(EPT0,&Globaldemo);
	csi_ept_global_rearm(EPT0) ;                                //重置ONE SHOT模式。ONE SHOT模式下，一次触发后，需要重置模式才允许再次触发
	csi_ept_global_sw(EPT0) ;                                   //软件产生一次GLD触发
//------------------------------------------------------------------------------------------------------------------------	
	csi_ept_start(EPT0);//start  timer
    while(1){
		    if((csp_ept_get_emHdlck(EPT0)&EPT_INT_EP1)==EPT_INT_EP1)
				{
					mdelay(10);
					csp_ept_clr_emHdlck(EPT0,EP1);
				}
//            csp_ept_evtrg_soft(EPT0,0);			
//		    csi_ept_change_ch_duty(EPT0,EPT_CH_A, 25);		      
		    mdelay(200);                        
			//csi_ept_Continuous_software_waveform(EPT0,EPT_CHANNEL_A,EM_AQCSF_L);//
			//mdelay(200);
			//csi_ept_Continuous_software_waveform(EPT0,EPT_CHANNEL_A,EM_AQCSF_NONE);//持续性软件波形控制寄存器
		    csi_ept_change_ch_duty(EPT0,EPT_CH_A, 50);	
		    mdelay(200);
	}			
	return iRet;
};



int ept_demo1(void)
{
	int iRet = 0;	
//------------------------------------------------------------------------------------------------------------------------	
	csi_pin_set_mux(PA015, PA015_EPT_CHAX);						//
	csi_pin_set_mux(PA014, PA014_EPT_CHBX);						//
	csi_pin_set_mux(PB05, PB05_EPT_CHCX);						//	
	csi_pin_set_mux(PA012, PA012_EPT_CHAY);						//	
	csi_pin_set_mux(PA08, PA08_EPT_CHBY);						//
	csi_pin_set_mux(PA04, PA04_EPT_CHCY);
	
//	csi_pin_set_mux(PA07,PA07_EPI0);
	csi_pin_set_mux(PA013,PA013_EPI1);
	csi_pin_set_mux(PB03, PB03_EPI2);
//	csi_pin_set_mux(PB02, PB02_EPI3);
//------------------------------------------------------------------------------------------------------------------------	
	csi_ept_captureconfig_t tPwmCfg;								  
		tPwmCfg.byWorkmod       = EPT_CAPTURE;                     //WAVE or CAPTURE    //计数或捕获	
		tPwmCfg.byCountingMode  = EPT_UPCNT;                       //CNYMD  //计数方向
		tPwmCfg.byOneshotMode   = EPT_OP_CONT;                     //OPM    //单次或连续(工作方式)
		tPwmCfg.byStartSrc      = EPT_SYNC_START;				   //软件使能同步触发使能控制（RSSR中START控制位）//启动方式
	    tPwmCfg.byPscld         = EPT_LDPSCR_ZRO;                  //PSCR(分频)活动寄存器载入控制。活动寄存器在配置条件满足时，从影子寄存器载入更新值	
		tPwmCfg.byCaptureCapmd   = 0;                              //0:连续捕捉模式    1h：一次性捕捉模式
		tPwmCfg.byCaptureStopWrap=4-1;                             //Capture模式下，捕获事件计数器周期设置值
		tPwmCfg.byCaptureLdaret  =0;                               //CMPA捕捉载入后，计数器值计数状态控制位(1h：CMPA触发后，计数器值进行重置;0h：CMPA触发后，计数器值不进行重置)
		tPwmCfg.byCaptureLdbret  =0;                              
		tPwmCfg.byCaptureLdcret  =0;                              
		tPwmCfg.byCaptureLddret  =0;                              	
	    tPwmCfg.byInter 		 =EPTINT_CAPLD3;                 //interrupt
	csi_ept_capture_init(EPT0, &tPwmCfg);

//------------------------------------------------------------------------------------------------------------------------
	csi_ept_emergency_config_t   tEptEmergencyCfg;           //紧急状态输入
    tEptEmergencyCfg.byEpxInt    = EBI1 ;                    //EPx选择外部IO端口（EBI0~EBI4）
    tEptEmergencyCfg.byPolEbix   = EBI_POL_H;                //EBIx的输入有效极性选择控制
	tEptEmergencyCfg.byEpx       = EP1;                      //使能EPx
	tEptEmergencyCfg.byEpxLckmd  = EP_DIS;                   //
    tEptEmergencyCfg.byFltpace0  = EPFLT0_2P;                 //EP0、EP1、EP2和EP3的数字去抖滤波检查周期数
	tEptEmergencyCfg.byFltpace1  = EPFLT1_2P;                 //EP4、EP5、EP6和EP7的数字去抖滤波检查周期数
	if(tEptEmergencyCfg.byEpxInt ==ORL0){tEptEmergencyCfg.byOrl0 = ORLx_EP0 |ORLx_EP1|ORLx_EP2;}
	if(tEptEmergencyCfg.byEpxInt ==ORL1){tEptEmergencyCfg.byOrl1 = ORLx_EP4 |ORLx_EP5|ORLx_EP6;}
	csi_ept_emergency_cfg(EPT0,&tEptEmergencyCfg);		
	csi_ept_emergency_interruption_en(EPT0,EPT_INT_EP1);      //紧急状态输入中断使能
//------------------------------------------------------------------------------------------------------------------------	
    csi_ept_set_evtrg(EPT0, EPT_TRG_OUT0, EPT_TRGSRC_PE1);    //EP1用trg0输出，经过ETCB  触发sync2 捕获
	csi_ept_set_sync (EPT0, EPT_TRGIN_SYNCEN2, EPT_TRG_CONTINU,EPT_AUTO_REARM_ZRO);
	csi_ept_int_enable(EPT0, EPT_INT_TRGEV0,true);	
	csi_ept_start(EPT0);//start  timer
    while(1){		
		  		      
		    mdelay(200);                        
		    	
		    mdelay(200);
	}			
	return iRet;
};

int ept_demo12(void)
{
	int iRet = 0;	
//------------------------------------------------------------------------------------------------------------------------	
	csi_pin_set_mux(PA015, PA015_EPT_CHAX);						//
	csi_pin_set_mux(PA014, PA014_EPT_CHBX);						//
	csi_pin_set_mux(PB05, PB05_EPT_CHCX);						//	
	csi_pin_set_mux(PA012, PA012_EPT_CHAY);						//	
	csi_pin_set_mux(PA08, PA08_EPT_CHBY);						//
	csi_pin_set_mux(PA04, PA04_EPT_CHCY);
//------------------------------------------------------------------------------------------------------------------------		
csi_ept_pwmconfig_t tPwmCfg;								  
	tPwmCfg.byWorkmod        = EPT_WAVE;                        //WAVE  波形模式
	tPwmCfg.byCountingMode   = EPT_UPCNT;                       //CNYMD  //计数方向
	tPwmCfg.byOneshotMode    = EPT_OP_CONT;                     //OPM    //单次或连续(工作方式)
	tPwmCfg.byStartSrc       = EPT_SYNC_START;					//软件使能同步触发使能控制（RSSR中START控制位）//启动方式
	tPwmCfg.byPscld          = EPT_LDPSCR_ZRO;                  //PSCR(分频)活动寄存器载入控制。活动寄存器在配置条件满足时，从影子寄存器载入更新值		
	tPwmCfg.byDutyCycle 	 = 50;								//pwm ouput duty cycle//PWM初始值			
	tPwmCfg.wFreq 			 = 10000;							//pwm ouput frequency	
    
	tPwmCfg.byInter 		 = EPTINT_PEND;                     //interrupt
	csi_ept_wave_init(EPT0, &tPwmCfg);

   
//------------------------------------------------------------------------------------------------------------------------	
	csi_ept_pwmchannel_config_t  channel;
	channel.byActionZro    =   LO;
	channel.byActionPrd    =   NA;
	channel.byActionCau    =   HI;
	channel.byActionCad    =   LO;
	channel.byActionCbu    =   NA;
	channel.byActionCbd    =   NA;
	channel.byActionT1u    =   LO;
	channel.byActionT1d    =   LO;
	channel.byActionT2u    =   NA;
	channel.byActionT2d    =   NA;
	channel.byChoiceCasel  =   EPT_CMPA;
	channel.byChoiceCbsel  =   EPT_CMPA;	
	csi_ept_channel_config(EPT0, &channel,  EPT_CHANNEL_A);//channel
	csi_ept_channel_config(EPT0, &channel,  EPT_CHANNEL_B);
	csi_ept_channel_config(EPT0, &channel,  EPT_CHANNEL_C);
	csi_ept_channel_config(EPT0, &channel,  EPT_CHANNEL_D);
	
//------------------------------------------------------------------------------------------------------------------------		
	csi_ept_start(EPT0);//start  timer
	while(1){	
		    csi_ept_change_ch_duty(EPT0,EPT_CH_A, 25);		      
		    mdelay(200);                        
		    csi_ept_change_ch_duty(EPT0,EPT_CH_A, 50);	
		    mdelay(200);	
	}
	return iRet;
}



