/***********************************************************************//** 
 * \file  ept.h
 * \brief  Enhanced timer
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-8 <td>V0.0  <td>WNN   <td>initial
 * <tr><td> 2021-1-2 <td>V0.1 <td>LJY   <td>modify on 110
 * <tr><td> 2021-1-8 <td>V0.2  <td>WNN   <td>adapt to 102
 * </table>
 * *********************************************************************
*/


#ifndef _EPT_H_
#define _EPT_H_

//#include "stdint.h"
//#include "common.h"

#include <drv/common.h>
#include "csp.h"

#ifdef __cplusplus
extern "C" {
#endif



extern uint32_t g_ept_prd;

typedef struct csi_ept_config csi_ept_config_t;
struct csi_ept_config {
    uint8_t		byWorkmod;          //Count or capture
	uint8_t     byCountingMode;    //csi_ept_cntmd_e
	uint8_t     byOneshotMode;     //Single or continuous
	uint8_t     byStartSrc ;
	uint8_t     byPscld;
	uint8_t		byDutyCycle;		 //TIMER PWM OUTPUT duty cycle	
	uint8_t     byCaptureCapLden;
	uint8_t     byCaptureRearm;
	uint8_t     byCaptureCapmd;
	uint8_t     byCaptureStopWrap;
	uint8_t     byCaptureLdaret;
	uint8_t     byCaptureLdbret;
	uint8_t     byCaptureLdcret;
	uint8_t     byCaptureLddret;
	uint8_t     byBurst;
    uint8_t     byCgsrc;
	uint8_t     byCgflt;
	uint32_t	wFreq;				 //TIMER PWM OUTPUT frequency 
	uint32_t    byInter;
};

typedef struct csi_ept_pwmconfig  csi_ept_pwmconfig_t;
struct csi_ept_pwmconfig {
	uint8_t		byWorkmod;          //Count or capture
	uint8_t     byCountingMode;    //csi_ept_cntmd_e
	uint8_t     byOneshotMode;     //Single or continuous
	uint8_t     byStartSrc ;
	uint8_t     byPscld;
	uint8_t		byDutyCycle;		 //TIMER PWM OUTPUT duty cycle	
	uint32_t	wFreq;				 //TIMER PWM OUTPUT frequency 
	uint32_t    byInter;
};

typedef struct csi_ept_captureconfig  csi_ept_captureconfig_t;
struct csi_ept_captureconfig {
	uint8_t		byWorkmod;          //Count or capture
	uint8_t     byCountingMode;     //csi_ept_cntmd_e
	uint8_t     byOneshotMode;      //Single or continuous
	uint8_t     byStartSrc ;
	uint8_t     byPscld;
	uint8_t		byDutyCycle;		 //TIMER PWM OUTPUT duty cycle	
 	uint8_t     byCaptureCapLden;
	uint8_t     byCaptureRearm;
	uint8_t     byCaptureCapmd;
	uint8_t     byCaptureStopWrap;
	uint8_t     byCaptureLdaret;
	uint8_t     byCaptureLdbret;
	uint8_t     byCaptureLdcret;
	uint8_t     byCaptureLddret;
	uint32_t    byInter;
};

//typedef struct  csi_ept_syncr_config  csi_ept_syncr_config_t; 
//struct  csi_ept_syncr_config{
//	uint8_t  syncen0;
//	uint8_t  syncen1;
//	uint8_t  syncen2;
//	uint8_t  syncen3;
//	uint8_t  syncen4;
//	uint8_t  syncen5;	
//	uint8_t  ostmd0;
//	uint8_t  ostmd1;
//	uint8_t  ostmd2;
//	uint8_t  ostmd3;
//	uint8_t  ostmd4;
//	uint8_t  ostmd5;	
//	uint8_t  rearm0;
//	uint8_t  rearm1;
//	uint8_t  rearm2;
//	uint8_t  rearm3;
//	uint8_t  rearm4;
//	uint8_t  rearm5;
//	uint8_t  arearm;
//	uint8_t  Tx_rearm0;
//	uint8_t  trgo0sel;
//	uint8_t  trgo1sel;
//};

typedef struct csi_ept_pwmchannel_config      csi_ept_pwmchannel_config_t;
struct csi_ept_pwmchannel_config {
	
    uint8_t		byActionZro;          //
	uint8_t     byActionPrd;          //
	uint8_t     byActionCau;          //
    uint8_t     byActionCad;          //
	uint8_t     byActionCbu;          //
	uint8_t     byActionCbd;          //
	uint8_t     byActionT1u;          //
	uint8_t     byActionT1d;          //
	uint8_t     byActionT2u;          //
	uint8_t     byActionT2d;          //	
    uint8_t     byChoiceCasel;
	uint8_t     byChoiceCbsel;
};

//typedef struct csi_ept_dbldr_config      csi_ept_dbldr_config_t;
//struct csi_ept_dbldr_config {
//	
//    uint8_t		bycfgdbcrshadoworimmediate;          //
//	uint8_t     bycfgdbcrshadowLoadcontrol; 
//         
//	uint8_t     bycfgdbdtrshadoworimmediate;          
//    uint8_t     bycfgdbdtrshadowLoadcontrol; 
//		
//	uint8_t     bycfgdbdtfshadoworimmediate;        
//	uint8_t     bycfgdbdtfshadowLoadcontrol; 
//       
//	uint8_t     bycfgdckpscshadoworimmediate;          
//	uint8_t     bycfgdckpscshadowLoadcontrol;     
//};

typedef struct csi_ept_deadzone_config   csi_ept_deadzone_config_t;
struct  csi_ept_deadzone_config
{	
	uint8_t       byChxOuselS1S0      ;   //
	uint8_t       byChxPolarityS3S2   ;   //
	uint8_t       byChxInselS5S4      ;   //
	uint8_t       byChxOutSwapS8S7    ;   //		
	uint8_t      byDcksel;
	uint8_t      byChaDedb;
	uint8_t      byChbDedb;
	uint8_t      byChcDedb;
	uint16_t      hwDpsc;                 //
	uint16_t      hwRisingEdgereGister ;  //
	uint16_t      hwFallingEdgereGister;  //
};



typedef struct csi_ept_Chopper_config   csi_ept_Chopper_config_t;
struct csi_ept_Chopper_config
{
    uint8_t  byOspwth             ;                
	uint8_t  byCdiv               ;                
	uint8_t  byCduty              ;              
	uint8_t  byCasel              ;              
	uint8_t  chen            ;              

};
typedef enum{
	EPTCHAX = 0x1,
	EPTCHAY,
	EPTCHBX,
	EPTCHBY,
	EPTCHCX,
	EPTCHCY
}csi_ept_chx_e;


typedef struct csi_ept_emergency_input_config   csi_ept_emergency_input_config_t;
struct csi_ept_emergency_input_config
{
    uint8_t  ep0choiceebixint        ;
    uint8_t  ep1choiceebixint        ;
    uint8_t  ep2choiceebixint        ;
    uint8_t  ep3choiceebixint        ;
	uint8_t  ep4choiceebixint        ;
	uint8_t  ep5choiceebixint        ;
	uint8_t  ep6choiceebixint        ;
	uint8_t  ep7choiceebixint        ;

	uint8_t  fltpace0                  ;
	uint8_t  fltpace1                  ;
    uint8_t  POLPOSEBI0               ;
	uint8_t  POLPOSEBI1               ;
	uint8_t  POLPOSEBI2               ;
	uint8_t  POLPOSEBI3               ;
	uint8_t  POLPOSEBI4               ;
	uint8_t  ep0lckmd                  ;        
	uint8_t  ep1lckmd                  ;
	uint8_t  ep2lckmd                  ;
	uint8_t  ep3lckmd                  ;
	uint8_t  ep4lckmd                  ;
	uint8_t  ep5lckmd                  ;
	uint8_t  ep6lckmd                  ;
	uint8_t  ep7lckmd                  ;
	uint8_t  osrshdw                    ;
	uint8_t  osrldmd                    ;
	uint8_t  slclrmd                    ;
	uint8_t  emasync                    ;
	uint8_t  cpufault                  ;                               
	uint8_t  memfault                  ;                              
	uint8_t  eomfault                  ;                               
	uint8_t  emcoax                    ;                                          
	uint8_t  emcobx                    ;
	uint8_t  emcocx                    ;                                          
	uint8_t  emcod                     ;
	uint8_t  emcoay                    ;
	uint8_t  emcoby                    ;
	uint8_t  emcocy                    ;	
	uint16_t orl0choiceepxorl       ;
	uint32_t orl1choiceepxorl       ;
};

typedef struct  csi_ept_Event_trigger_config   csi_ept_Event_trigger_config_t;
struct  csi_ept_Event_trigger_config{
	uint8_t   trg0sel  ;
	uint8_t   trg1sel  ;
	uint8_t   trg2sel  ;
	uint8_t   trg3sel  ;	
	uint8_t   trg0en   ;
	uint8_t   trg1en   ;
	uint8_t   trg2en   ;
	uint8_t   trg3en   ;	
	uint8_t   cnt0initfrc;
	uint8_t   cnt1initfrc;
	uint8_t   cnt2initfrc;
	uint8_t   cnt3initfrc;
	uint8_t   cnt0initen;
	uint8_t   cnt1initen;
	uint8_t   cnt2initen;
	uint8_t   cnt3initen;
	uint8_t   trg0prd ;
	uint8_t   trg1prd ;
	uint8_t   trg2prd ;
	uint8_t   trg3prd ;
	uint8_t   cnt0init;
	uint8_t   cnt1init;
	uint8_t   cnt2init;
	uint8_t   cnt3init;
	
	uint8_t fltipscld;
	uint8_t fltckprs;
	uint8_t srcsel;
	uint8_t blkinv;
	uint8_t alignmd;
	uint8_t crossmd;
	uint16_t offset;
	uint16_t window;
	
};

typedef struct csi_ept_Global_load_control_config    csi_ept_Global_load_control_config_t;
struct csi_ept_Global_load_control_config{
	
	bool glden;
    uint8_t gldmd;
	bool ostmd;
	bool gldprd;
	bool prdren;
	bool cmpaen;
	bool cmpben;
	bool cmpcen;
	bool cmpden;
    bool dbdtren;
	bool dbdtfen;
	bool dbcren;
	bool aqcraen;
	bool aqcrben;
	bool aqcrcen;
	bool aqcrden;
	bool aqcsfen;
	bool aqosren;
	bool osrearm;
	bool gfrcld;
};

/// \struct csi_ept_filter_config_t
/// \brief  ept sync trigger filter parameter configuration, open to users  
typedef struct {
	uint8_t		byFiltSrc;			//filter input signal source
	uint8_t		byWinInv;			//window inversion 
	uint8_t		byWinAlign;			//window alignment 
	uint8_t		byWinCross;			//window cross
	uint16_t	byWinOffset;		//window offset
	uint16_t 	byWinWidth;			//window width		
} csi_ept_filter_config_t;

typedef enum
{
	EPT_CAPTURE	= 0,		
    EPT_WAVE 	= 1		
}csi_ept_wave_e;

typedef enum {
	EPT_UPCNT = 0,
	EPT_DNCNT,
	EPT_UPDNCNT
}csi_ept_cntmd_e;


typedef enum{
	EPT_OP_CONT = 0,
	EPT_OP_OT,
}csi_ept_opmd_e;

typedef enum{
    EPT_CHANNEL_A=1,
	EPT_CHANNEL_B,
	EPT_CHANNEL_C,
	EPT_CHANNEL_D
}csi_ept_channel_e;

typedef enum{
    SYNCIN0 =0,
	SYNCIN1,
	SYNCIN2,
	SYNCIN3,
	SYNCIN4,
	SYNCIN5,
	SYNCIN_other		
}csi_ept_trgo0sel_e;

typedef enum{
    DBCR =0,
	DBDTR,
	DBDTF,
	DCKPSC,		
}csi_ept_dbdldr_e;

typedef enum
{
	EPT_PWM_START_LOW	= 0,		//PWM start level is low 
    EPT_PWM_START_HIGH 	= 1,		//PWM start level is high        
	EPT_PWM_IDLE_LOW  	= 0,		//PWM idle level is low 
    EPT_PWM_IDLE_HIGH	= 1 		//PWM idle level is high   	    
}csi_ept_pwmlev_e;

typedef enum {
	EPT_AQCSF_NOW=0,
	EPT_AQCSF_ZRO,
	EPT_AQCSF_PRD,
	EPT_AQCSF_ZRO_PRD
}csp_ept_aqosf_e;

typedef enum {
	EM_AQCSF_NONE=0,
	EM_AQCSF_L,
	EM_AQCSF_H,
	EM_AQCSF_NONE1
}csp_ept_aqcsf_e;


typedef enum {
	EPT_CH_A = 0,
	EPT_CH_B,
	EPT_CH_C,
	EPT_CH_D
}csi_ept_chtype_e;

typedef enum {
	EPT_SW = 0,
	EPT_SYNC
}csi_ept_stmd_e;

typedef enum{
	EPT_STP_HZ = 0,
	EPT_STP_L
}csi_ept_stopst_e;

typedef enum{
	EPT_CAP_CONT = 0,
	EPT_CAP_OT
}csi_ept_capmd_e;

#define EPT_EVENT_SLCK_EBI(n)  n
typedef enum{
	EPT_EVENT_SLCK = 0,
	EPT_EVENT_HLCK,
}csi_ept_event_e;

typedef enum {
	EPT_EBI0 = 0,
	EPT_EBI1,
	EPT_EBI2,
	EPT_EBI3,
	EPT_EBILVD,
}csi_ept_ebi_e;

typedef enum {
	EPT_EBI_POL_H = 0,
	EPT_EBI_POL_L
}csi_ept_ebipol_e;

typedef enum{
	EPT_EP_DIS = 0,
	EPT_EP_SLCK,
	EPT_EP_HLCK
}csi_ept_ep_lckmd_e;

typedef enum {
	EPT_EM_OUT_HZ,
	EPT_EM_OUT_H,
	EPT_EM_OUT_L,
	EPT_EM_OUT_NONE
}csi_ept_emout_e;

typedef struct {
	csi_ept_emout_e eChaxOut;
	csi_ept_emout_e eChayOut;
	csi_ept_emout_e eChbxOut;
	csi_ept_emout_e eChbyOut;
	csi_ept_emout_e eChcxOut;
	csi_ept_emout_e eChcyOut;
	csi_ept_emout_e eChdOut;
}csi_ept_em_out_e;

/****** EPT Event *****/
typedef enum {
    EPT_EVENT_HARDLOCK = 0,      
    EPT_EVENT_SOFTLOCK,         
    EPT_EVENT_ERROR                     
} csi_ept_event_t;

typedef enum{
	EPTINT_TRGEV0 = 0x1,
	EPTINT_TRGEV1 = 0x2,
	EPTINT_TRGEV2 = 0x4,
	EPTINT_TRGEV3 = 0x8,
	EPTINT_CAPLD0 = 0x1 << 4,
	EPTINT_CAPLD1 = 0x1 << 5,
	EPTINT_CAPLD2 = 0x1 << 6,
	EPTINT_CAPLD3 = 0x1 << 7,
	EPTINT_CAU = 0x1 << 8,
	EPTINT_CAD = 0x1 << 9,
	EPTINT_CBU = 0x1 << 10,
	EPTINT_CBD = 0x1 << 11,
	EPTINT_CCU = 0x1 << 12,
	EPTINT_CCD = 0x1 << 13,
	EPTINT_CDU = 0x1 << 14,
	EPTINT_CDD = 0x1 << 15,
	EPTINT_PEND = 0x1 << 16	
}csi_ept_int_e;

typedef enum{
	EPTINT_EP0 = 0,
	EPTINT_EP1,
	EPTINT_EP2,
	EPTINT_EP3,
	EPTINT_EP4,
	EPTINT_EP5,
	EPTINT_EP6,
	EPTINT_EP7,
	EPTINT_CPUF,
	EPTINT_MEMF,
	EPTINT_EOMF	
}csi_ept_emint_e;

/**
 * \enum	csi_ept_trgsrc_e
 * \brief   EPT event trigger source
 */
typedef enum {
	EPT_TRGSRC_DIS		= 0,
	EPT_TRGSRC_ZRO,
	EPT_TRGSRC_PRD,
	EPT_TRGSRC_ZRO_PRD,
	EPT_TRGSRC_CAU,
	EPT_TRGSRC_CAD,
	EPT_TRGSRC_CBU,
	EPT_TRGSRC_CBD,
	EPT_TRGSRC_CCU,
	EPT_TRGSRC_CCD,
	EPT_TRGSRC_CDU,
	EPT_TRGSRC_CDD,
	EPT_TRGSRC_EX,
	EPT_TRGSRC_PE0,
	EPT_TRGSRC_PE1,
	EPT_TRGSRC_PE2,
	EPT_TRGSRC_PEND
}csi_ept_trgsrc_e;

/**
 * \enum     csi_ept_arearm_e
 * \brief    EPT hardware auto rearm 
 */
typedef enum{
	EPT_AUTO_REARM_DIS 	= 0,	//disable auto rearm
	EPT_AUTO_REARM_ZRO,			//CNT = ZRO auto rearm
	EPT_AUTO_REARM_PRD,			//CNT = PRD auto rearm
	EPT_AUTO_REARM_ZRO_PRD		//CNT = PRD or PRD auto rearm
}csi_ept_arearm_e;

/**
 * \enum     csi_ept_trgin_e
 * \brief    EPT sync trigger input 
 */
typedef enum{
	EPT_TRGIN_SYNCEN0	= 0,	//start	up or reset count			
	EPT_TRGIN_SYNCEN1,			//reg updata				
	EPT_TRGIN_SYNCEN2,			//capture				
	EPT_TRGIN_SYNCEN3,			//count inc or dec			
	EPT_TRGIN_SYNCEN4,			//change output status(pwm)			
	EPT_TRGIN_SYNCEN5			//change output status(pwm)						
}csi_ept_trgin_e;

/**
 * \enum     csi_ept_trgmode_e
 * \brief    EPT sync trigger mode 
 */
typedef enum{
	EPT_TRG_CONTINU		= 0,	//EPT continuous trigger mode 
	EPT_TRG_ONCE				//EPT once trigger mode 							
}csi_ept_trgmode_e;

/**
 * \enum	csi_ept_trgout_e
 * \brief   ept event trigger out port
 */
typedef enum{
	EPT_TRG_OUT0		= 0,	//trigger out0
	EPT_TRG_OUT1,				//trigger out1	
	EPT_TRG_OUT2,				//trigger out2
	EPT_TRG_OUT3				//trigger out3		
}csi_ept_trgout_e;

/**
 * \enum	csi_ept_filter_insrc_e
 * \brief   ept filter input signal source
 */
typedef enum{
	EPT_FILT_DIS		= 0,	//filter disable
	EPT_FILT_SYNCIN0,			//filter input syncin0
	EPT_FILT_SYNCIN1,			//filter input syncin1	
	EPT_FILT_SYNCIN2,			//filter input syncin2
	EPT_FILT_SYNCIN3,			//filter input syncin3	
	EPT_FILT_SYNCIN4,			//filter input syncin4
	EPT_FILT_SYNCIN5,			//filter input syncin5		
}csi_ept_filter_insrc_e;

typedef enum{
	EPT_EVTRG_Disable 	= 0,
	EPT_EVTRG_Enable
}csi_ept_evtrg_e;

typedef enum{
	EPT_EVTRG_trigger_Disable = 0,
	EPT_Enable_trigger_start
}csi_ept_evtrg_trigger_e;

uint16_t csi_ept_get_prd(csp_ept_t *ptEpt);


/**
 \brief config pwm dual channel output mode. 
  * CHnX active Low ,ChnY active Low
*/
csi_error_t csi_ept_set_ch_dualout_md2(csp_ept_t *ptEpt, csi_ept_chtype_e eCh,  uint32_t wDutyCycle, uint32_t wT1ns, uint32_t wT2ns);



/**
 \brief change ept output dutycycle. 
 \param ptEpt    ept handle to operate
 \param wActiveTime cmpx data
*/
csi_error_t csi_ept_change_ch_duty(csp_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wActiveTime);



/**
 \brief config pwm single output mode1

  * ChnX    		  _____
  *           _______|    |_______
  * 	
  * \param ptEpt 	EPT handle to operate
  * \param eCh		EPT output channel(EPT_CH_A/B/C/D)
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \return tRet   
*/

csi_error_t csi_ept_set_singleout_md1(csp_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wDutyCycle);




/**
 \brief chopper configuration
 \param ptEpt    		ept handle to operate 
 \param byCdiv   		fchop = PCLK/((byCdiv+1)*8)
 \param byCduty  		(0~7) duty = byduty/8 
 \param byFirstWidth 	T = Tchop * byFirstWidth
 \return none 
*/
void csi_ept_set_cp(csp_ept_t *ptEpt, uint8_t byCdiv, uint8_t byCduty, uint8_t byFirstWidth);

/**
 \brief channeln chopper output enable 
 \param ptEpt    ept handle to operate
 \param bEnableX/Y  ENABLE/DISABLE
 \return none
*/
void csi_ept_cpa_enable(csp_ept_t *ptEpt, bool bEnableX, bool bEnableY);
void csi_ept_cpb_enable(csp_ept_t *ptEpt, bool bEnableX, bool bEnableY);
void csi_ept_cpc_enable(csp_ept_t *ptEpt, bool bEnableX, bool bEnableY);

/**
 \brief get harklock status
 \param ptEpt    ept handle to operate
 \return uint8_t 0b_ _ _ _ _ _ _ _
*/
uint8_t csi_ept_get_hdlck_st(csp_ept_t *ptEp);

/**
 \brief clear harklock status
 \param ptEpt    ept handle to operate
 \return  eEbi 	 external emergency input: EPT_EBI0~3/EPT_EBILVD
*/
void csp_ept_clr_hdlck(csp_ept_t *ptEp, csi_ept_ebi_e eEbi);


/**
  \brief   config ept out trigger 
  \param   ptEpt       EPT handle to operate
  \param 	eCh			0/1/2/3
  \param 	eSrc	
  \param   byTime		output trigger when event happens the 'byTime'th time.
*/
csi_error_t csi_ept_set_evtrg(csp_ept_t *ptEpt,uint8_t eCh,csi_ept_trgsrc_e eSrc, uint8_t byTime);

/**
  \brief   enable/disable ept out trigger 
  \param   ptEpt       EPT handle to operate
  \param 	eCh			0/1/2/3
  \param   bEnable		ENABLE/DISABLE
*/
csi_error_t csi_ept_evtrg_enable(csp_ept_t *ptEpt, uint8_t byCh, bool bEnable);

/**
   \brief   set ept sync
   \param   ptEpt       EPT handle to operate
   \param   bySync    sync select: 0/1/2/3
   \param   csi_ept_syncmode_t  EPT_SYNC_CONT/EPT_SYNC_OS
   \param   bARearmEnable auto rearm enable/disable
   \return  csi_error_t   
*/

csi_error_t csi_ept_start(csp_ept_t *ptBtBase);
csi_error_t csi_ept_config_init(csp_ept_t *ptBtBase, csi_ept_config_t *ptBtPwmCfg);
csi_error_t csi_ept_channel_config(csp_ept_t *pteptBase, csi_ept_pwmchannel_config_t *tPwmCfg, csi_ept_channel_e changl);
//csi_error_t csi_ept_syncr_config_init(csp_ept_t *ptEptBase,csi_ept_syncr_config_t *tPwmCfg);

//csi_error_t csi_ept_dbldrloadcontrol_config(csp_ept_t *ptEptBase, csi_ept_dbldr_config_t *tCfg);
csi_error_t csi_ept_dbcr_config(csp_ept_t *ptEptBase, csi_ept_deadzone_config_t *tCfg);
csi_error_t csi_ept_choppercpcr_config(csp_ept_t *ptEptBase, csi_ept_Chopper_config_t *tCfg);
csi_error_t csi_ept_emergency_config(csp_ept_t *ptEptBase, csi_ept_emergency_input_config_t *tCfg);
csi_error_t  csi_ept_evtrg_config(csp_ept_t *ptEptBase, csi_ept_Event_trigger_config_t *tCfg);
void csi_ept_emergency_interruption_en(csp_ept_t *ptEptBase, csp_ept_emint_e eEbi);
uint16_t csi_ept_capture_get_cap_data(csp_ept_t *ptEpt, uint8_t num);
csi_error_t csi_ept_continuous_software_waveform(csp_ept_t *ptEptBase, csi_ept_channel_e byCh, csp_ept_aqcsf_e bEnable);
csi_error_t csi_ept_global_config(csp_ept_t *ptEptBase,csi_ept_Global_load_control_config_t *Global);
csi_error_t  csi_ept_wave_init(csp_ept_t *ptEptBase, csi_ept_pwmconfig_t *pteptPwmCfg);
csi_error_t csi_ept_dbldrload_config(csp_ept_t *ptEptBase, csi_ept_dbdldr_e byVal,csp_ept_shdw_e byWod,csp_ept_lddb_e byWod2);
csi_error_t csi_ept_channelmode_config(csp_ept_t *ptEptBase,csi_ept_deadzone_config_t *tCfg,csi_ept_channel_e byCh);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_EPT_H_ */