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
	csi_pin_set_mux(PB02, PB02_EPI3);
//------------------------------------------------------------------------------------------------------------------------	
	csi_ept_config_t tPwmCfg;								  
	tPwmCfg.byWorkmod       = EPT_CAPTURE;                     //WAVE or CAPTURE    //计数或捕获	
	tPwmCfg.byCountingMode  = EPT_UPCNT;                       //CNYMD  //计数方向
	tPwmCfg.byOneshotMode   = EPT_OP_CONT;                     //OPM    //单次或连续(工作方式)
	tPwmCfg.byStartSrc      = EPT_SYNC_START;					//软件使能同步触发使能控制（RSSR中START控制位）//启动方式
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
		
	tPwmCfg.byInter 		 = EPTINT_CAPLD3;                      //interrupt
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
	
    csp_ept_set_aqtscr(EPT0,EPT_T1,EP0);//波形输出T事件选择
//------------------------------------------------------------------------------------------------------------------------	

	csi_ept_dbldrload_config(EPT0,DBCR,EPT_SHDW_IMMEDIATE,EPT_LD_ZRO);
	
//------------------------------------------------------------------------------------------------------------------------	
	csi_ept_deadzone_config_t  tEptDeadZoneTime;
	tEptDeadZoneTime.byDcksel               = EPT_DB_DPSC;     //
	tEptDeadZoneTime.hwDpsc                 =  0;              //FDBCLK = FHCLK / (DPSC+1)
	tEptDeadZoneTime.hwRisingEdgereGister = 12;                //NS/(1/(24000000/(DPSC+1))*10^9) // 500NS/(1000/24) = 12;
	tEptDeadZoneTime.hwFallingEdgereGister= 12;                //下降沿
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
	
	csi_ept_Chopper_config_t  tEptChopperOutCfg;
	tEptChopperOutCfg.byOspwth             =3 ;                 //首脉冲的宽度可以配置为载波周期的整数倍。当该控制位设为零时，
														     //所有脉冲宽度均由byCdiv和byCduty配置.Twidth = Tchop x byOspwth （Tchop为一个载波的周期时间）
	tEptChopperOutCfg.byCdiv               =0 ;                   //载波频率设置。载波的频率设置基于PCLK的8倍分频进行设置 Fchop = PCLK / ((byCdiv+1) x 8)
	tEptChopperOutCfg.byCduty              =EPT_CDUTY_4_8 ;                       //载波的占空比设置  0h：禁止载波;   1h：Duty = 7/8------7h：Duty = 1/8
	tEptChopperOutCfg.byCasel              =EPT_CPSRC_IN ;                        //载波信号源选择控制位: 0h：EPT内部产生载波;  1h：TIN的输入
	csi_ept_choppercpcr_config(EPT0,&tEptChopperOutCfg);
//	Chopper_output.chaxen            =EPT_CHOPPER_DISABLE  ;                 //斩波输出使能控制位   0b：禁止当前通道斩波输出    1b：开启当前通道斩波输出
//	Chopper_output.chayen            =EPT_CHOPPER_DISABLE  ;                 //斩波输出使能控制位
//	Chopper_output.chbxen            =EPT_CHOPPER_DISABLE  ;                 //斩波输出使能控制位
//	Chopper_output.chbyen            =EPT_CHOPPER_DISABLE  ;                 //斩波输出使能控制位
//	Chopper_output.chcxen            =EPT_CHOPPER_DISABLE  ;                 //斩波输出使能控制位
//	Chopper_output.chcyen            =EPT_CHOPPER_DISABLE  ;                 //斩波输出使能控制位
	
			
//------------------------------------------------------------------------------------------------------------------------
    csi_ept_emergency_input_config_t   TESTEM;                  //紧急状态输入
    TESTEM.ep0choiceebixint         = EBI1   ;                  //EPx选择外部IO端口（EBI0~EBI4）
    TESTEM.ep1choiceebixint         = EBI2   ;
    TESTEM.ep2choiceebixint         = EBI3   ;
    TESTEM.ep3choiceebixint         = EBI2   ;
	TESTEM.ep4choiceebixint         = EBI0   ;
	TESTEM.ep5choiceebixint         = ORL0   ;
	TESTEM.ep6choiceebixint         = ORL1   ;
	TESTEM.ep7choiceebixint         = EBI3;
	TESTEM.orl0choiceepxorl = (EPT_ORL0_NO_EPI(EP0)+ EPT_ORL0_NO_EPI(EP1));//多路EP的逻辑OR输出作为EPx中的可选一路输入信号(ORL0)
	TESTEM.orl1choiceepxorl = (EPT_ORL1_NO_EPI(EP2)+ EPT_ORL1_NO_EPI(EP3));//多路EP的逻辑OR输出作为EPx中的可选一路输入信号(ORL1)。
	TESTEM.fltpace0                   = EPFLT0_DIS;              //EP0、EP1、EP2和EP3的数字去抖滤波检查周期数
	TESTEM.fltpace1                   = EPFLT1_1P ;              //EP4、EP5、EP6和EP7的数字去抖滤波检查周期数
		
    TESTEM.POLPOSEBI0                = EBI_POL_H ;              //EBIx的输入有效极性选择控制
	TESTEM.POLPOSEBI1                = EBI_POL_H ;
	TESTEM.POLPOSEBI2                = EBI_POL_H ;
	TESTEM.POLPOSEBI3                = EBI_POL_H ;
	TESTEM.POLPOSEBI4                = EBI_POL_H ;
	
	TESTEM.ep0lckmd                   = EP_DIS    ;             // EPx触发锁止模式（EP_SLCK：使能当前EPx触发软锁止，EP_HLCK：使能当前EPx触发硬锁止）
	TESTEM.ep1lckmd                   = EP_DIS    ;
	TESTEM.ep2lckmd                   = EP_DIS    ;
	TESTEM.ep3lckmd                   = EP_DIS    ;
	TESTEM.ep4lckmd                   = EP_DIS    ;
	TESTEM.ep5lckmd                   = EP_DIS    ;
	TESTEM.ep6lckmd                   = EP_DIS    ;
	TESTEM.ep7lckmd                   = EP_HLCK   ;
	
	TESTEM.osrshdw                     = EPT_SHDW_IMMEDIATE;   //EMOSR寄存器的Shadow功能使能控制
	TESTEM.osrldmd                     = EPT_LDEMOSR_ZRO;      //Shadow模式下，Active EMOSR从Shadow载入控制。
	TESTEM.slclrmd                     = EPT_SLCLRMD_CLR_ZRO;  //软锁止清除条件设置。当CNT值等于设置值，且软锁止不再触发时，硬件自动清除软锁止状态和标志位
	TESTEM.emasync                     = EPT_EMASYNC_ENABLE;   //EP端口同步设置控制位
	TESTEM.cpufault                   = EPT_FAULT_DISABLE;    //CPU错误触发硬锁止控制位。
	TESTEM.memfault                   = EPT_FAULT_DISABLE;    //MEM错误触发硬锁止控制位。（需要同时使能SRAM或者Flash校验功能）
	TESTEM.eomfault                   = EPT_FAULT_DISABLE;    //外部晶振错误触发硬锁止控制位
	
	TESTEM.emcoax                     = EM_OUT_L;                                          
	TESTEM.emcobx                     = EM_OUT_L;
	TESTEM.emcocx                     = EM_OUT_L;             //当发生EP触发的软锁止或者硬锁止时，在相应通道上的输出状态设置
	TESTEM.emcod                      = EM_OUT_L;
	TESTEM.emcoay                     = EM_OUT_L;
	TESTEM.emcoby                     = EM_OUT_L;
	TESTEM.emcocy                     = EM_OUT_L;
	csi_ept_emergency_config(EPT0,&TESTEM);
	
	csi_ept_emergency_interruption_en(EPT0,EPT_INT_EP7);      //紧急状态输入中断使能
	
//------------------------------------------------------------------------------------------------------------------------	
//   csi_ept_syncr_config_t   syncrdemo;  
//   syncrdemo.syncen0   =   ENABLE;                           //外部Sync事件            //SYNCENx 使能
//   syncrdemo.syncen1   =   DISABLE;                          //Load触发
//   syncrdemo.syncen2   =   ENABLE;                           //Capture触发事件
//   syncrdemo.syncen3   =   DISABLE;                          //CNT增减一拍触发事件
//   syncrdemo.syncen4   =   DISABLE;                          //外部COS事件T1（用于PWM波形输出控制）
//   syncrdemo.syncen5   =   DISABLE;                          //外部COS事件T2（用于PWM波形输出控制）
//   
//   syncrdemo.ostmd0    =   EPT_OSTMD_CONT;                          //同步触发模式选择(0h：连续触发模式;;;1h：一次性触发模式)
//   syncrdemo.ostmd1    =   EPT_OSTMD_OS; 
//   syncrdemo.ostmd2    =   EPT_OSTMD_CONT; 
//   syncrdemo.ostmd3    =   EPT_OSTMD_OS; 
//   syncrdemo.ostmd4    =   EPT_OSTMD_OS; 
//   syncrdemo.ostmd5    =   EPT_OSTMD_OS; 
//   if( syncrdemo.ostmd0 )syncrdemo.rearm0    =   ENABLE;            //在一次性同步触发模式下，软件重置当前通道状态控制位
//   if( syncrdemo.ostmd1 )syncrdemo.rearm1    =   ENABLE; 
//   if( syncrdemo.ostmd2 )syncrdemo.rearm2    =   ENABLE; 
//   if( syncrdemo.ostmd3 )syncrdemo.rearm3    =   ENABLE; 
//   if( syncrdemo.ostmd4 )syncrdemo.rearm4    =   ENABLE; 
//   if( syncrdemo.ostmd5 )syncrdemo.rearm5    =   ENABLE;
//   syncrdemo.arearm     =EPT_AREARM_ZRO;                            //在一次性同步触发模式下,硬件自动REARM控制位
//   
//   syncrdemo.Tx_rearm0  =EPT_TXREARM0_DIS; //禁止Tx硬件自动REARM    //T1或T2 触发SYNCIN0的REARM
//   
//   syncrdemo.trgo0sel   =SYNCIN2;          //输入触发通道直通作为TRGSRC0的ExtSync条件的选择。只有当EVTRG寄存器中TRGSRC0控制位选择为ExtSync条件时有效
//   syncrdemo.trgo1sel   =SYNCIN2;
//   
//   csi_ept_syncr_config_init(EPT0, &syncrdemo);	
//	
//------------------------------------------------------------------------------------------------------------------------	  
	csi_ept_Event_trigger_config_t  EVT_DEMO;
	EVT_DEMO.trg0en       =   EPT_EVTRG_Enable;               //TRGOUTx使能
	EVT_DEMO.trg1en       =   EPT_EVTRG_Disable;
	EVT_DEMO.trg2en       =   EPT_EVTRG_Disable;
	EVT_DEMO.trg3en       =   EPT_EVTRG_Disable;
	EVT_DEMO.trg0sel      =   EPT_TRG01_PE0;                  //TRGEVx事件的触发源选择。
	EVT_DEMO.trg1sel      =   EPT_TRG01_DIS;
	EVT_DEMO.trg2sel      =   EPT_TRG23_DIS;
	EVT_DEMO.trg3sel      =   EPT_TRG23_DIS;
			
	EVT_DEMO.cnt0initen    =   EPT_EVTRG_Disable;             //RGEVxCNT寄存器计数更新使能
	EVT_DEMO.cnt1initen    =   EPT_EVTRG_Disable;
	EVT_DEMO.cnt2initen    =   EPT_EVTRG_Disable;
	EVT_DEMO.cnt3initen    =   EPT_EVTRG_Disable;
	EVT_DEMO.trg0prd    =  0;                                //TRGEV0事件计数的周期设置
	EVT_DEMO.trg1prd     =  1;
	EVT_DEMO.trg2prd     =  2;
	EVT_DEMO.trg3prd     =  3;
	EVT_DEMO.cnt0init   = 0;                                 //RGEVxCNT计数器的初始化值设置
	EVT_DEMO.cnt1init   = 1;
	EVT_DEMO.cnt2init   = 2;
	EVT_DEMO.cnt3init   = 3;
	EVT_DEMO.cnt0initfrc   =   EPT_EVTRG_trigger_Disable;     //TRGEVxCNT软件触发更新标志位
	EVT_DEMO.cnt1initfrc   =   EPT_EVTRG_trigger_Disable;
	EVT_DEMO.cnt2initfrc   =   EPT_EVTRG_trigger_Disable;
	EVT_DEMO.cnt3initfrc   =   EPT_EVTRG_trigger_Disable;
	
	EVT_DEMO.fltipscld     =   ENABLE;                        //数字滤波器时钟分频使能
	EVT_DEMO.fltckprs      =   1;                             //数字滤波器的时钟频率为PCLK/( FLTCKPRS+1)
	EVT_DEMO.srcsel       =   1;                                 //0h：禁止滤波;/1h：使能SYNCIN0滤波;/2h：使能SYNCIN1滤波;/3h：使能SYNCIN2滤波/;
																  //4h：使能SYNCIN3滤波;/5h：使能SYNCIN4滤波;/6h：使能SYNCIN5滤波;/7h：保留
	EVT_DEMO.blkinv        =   0;                            //窗口使能反转控制(0h：窗口不反转;;1h：窗口反转)
	EVT_DEMO.alignmd       =   0;                            //窗口对齐模式选择(0h：CNT=ZRO;/1h：CNT=PRD;/2h：CNT=PRDorCNT=ZRO;/3h：T1事件;)
	EVT_DEMO.crossmd       =   0;                            //滤波窗跨越窗口对齐点(0h：禁止对齐点跨窗口;;1h：允许对齐点跨窗口)
	EVT_DEMO.offset        =  32768;                         //滤波窗的OFFSET设置;
	EVT_DEMO.window        =  65535;                         //滤波窗的宽度设置
	csi_ept_evtrg_config(EPT0,&EVT_DEMO);
//------------------------------------------------------------------------------------------------------------------------

    csi_ept_Global_load_control_config_t  Globaldemo;
	Globaldemo.glden   =  DISABLE;                           //全局的Shadow到Active寄存器载入控制使能
	Globaldemo.gldmd   =  EPT_LDGLD_ZRO_PRD_LD_SYNC;         //全局载入触发事件选择
	Globaldemo.ostmd   =  EPT_LDMD_ANYTIME;                  //One Shot 载入模式使能控制位
	Globaldemo.gldprd  =  0 ;                                //全局载入触发周期选择。可以选择N次触发条件满足后，才进行一次全局载入。	
	Globaldemo.prdren  =  ENABLE;                            //PRDR寄存器在  glden=1时  使用全局载入配置
	Globaldemo.cmpaen  =  ENABLE;
	Globaldemo.cmpben  =  ENABLE;
	Globaldemo.cmpcen  =  ENABLE;
	Globaldemo.cmpden  =  ENABLE;
	Globaldemo.dbdtren =  ENABLE;
	Globaldemo.dbdtfen =  ENABLE;
	Globaldemo.dbcren  =  ENABLE;
	Globaldemo.aqcraen =  ENABLE;
	Globaldemo.aqcrben =  ENABLE;
	Globaldemo.aqcrcen =  ENABLE;
	Globaldemo.aqcrden =  ENABLE;
	Globaldemo.aqcsfen =  ENABLE;
	Globaldemo.aqosren =  ENABLE;	
    Globaldemo.osrearm =  ENABLE;                           //重置ONE SHOT模式。ONE SHOT模式下，一次触发后，需要重置模式才允许再次触发
	Globaldemo.gfrcld  =  ENABLE;                           //软件产生一次GLD触发
	csi_ept_global_config(EPT0,&Globaldemo);
//------------------------------------------------------------------------------------------------------------------------	
	csi_ept_start(EPT0);//start  timer
//	iRet=csp_ept_get_emSdlck(EPT0);
//  iRet=csp_ept_get_emHdlck(EPT0);
    while(1){
		    if((csp_ept_get_emHdlck(EPT0)&EPT_INT_EP7)==EPT_INT_EP7)
				{
					csp_ept_clr_emHdlck(EPT0,EP7);
				}
//            csp_ept_evtrg_soft(EPT0,0);			
		    csp_ept_set_cmpa(EPT0, g_ept_prd/2);		      
		    mdelay(200);                        
			//csi_ept_Continuous_software_waveform(EPT0,EPT_CHANNEL_A,EM_AQCSF_L);//
			//mdelay(200);
			//csi_ept_Continuous_software_waveform(EPT0,EPT_CHANNEL_A,EM_AQCSF_NONE);//持续性软件波形控制寄存器
		    csp_ept_set_cmpa(EPT0, g_ept_prd/4);
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
//------------------------------------------------------------------------------------------------------------------------		
csi_ept_pwmconfig_t tPwmCfg;								  
	tPwmCfg.byWorkmod        = EPT_WAVE;                        //WAVE  波形模式
	tPwmCfg.byCountingMode   = EPT_UPCNT;                       //CNYMD  //计数方向
	tPwmCfg.byOneshotMode    = EPT_OP_CONT;                     //OPM    //单次或连续(工作方式)
	tPwmCfg.byStartSrc       = EPT_SYNC_START;					//软件使能同步触发使能控制（RSSR中START控制位）//启动方式
	tPwmCfg.byPscld          = EPT_LDPSCR_ZRO;                  //PSCR(分频)活动寄存器载入控制。活动寄存器在配置条件满足时，从影子寄存器载入更新值		
	tPwmCfg.byDutyCycle 	 = 50;								//pwm ouput duty cycle//PWM初始值			
	tPwmCfg.wFreq 			 = 10000;							//pwm ouput frequency	
    
	tPwmCfg.byInter 		 = EPTINT_PEND;                   //interrupt
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
	return iRet;
}
