/***********************************************************************//** 
 * \file  gpta.h
 * \brief  Enhanced timer
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-24 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/

#ifndef _GPTA_H_
#define _GPTA_H_

#include "stdint.h"
#include "common.h"
#include "csp.h"
#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t g_gpta_prd;

typedef struct csi_gpta_config    csi_gpta_config_t;
struct csi_gpta_config {
    uint8_t		byWorkmod;          //Count or capture
	uint8_t     byCountingMode;    //csi_gpta_cntmd_e
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


typedef struct csi_gpta_pwmconfig  csi_gpta_pwmconfig_t;
struct csi_gpta_pwmconfig {
	uint8_t		byWorkmod;          //Count or capture
	uint8_t     byCountingMode;     //csi_gpta_cntmd_e
	uint8_t     byOneshotMode;      //Single or continuous
	uint8_t     byStartSrc ;
	uint8_t     byPscld;
	uint8_t		byDutyCycle;		 //TIMER PWM OUTPUT duty cycle	
	uint32_t	wFreq;				 //TIMER PWM OUTPUT frequency 
	uint32_t    byInter;
};
typedef struct csi_gpta_captureconfig  csi_gpta_captureconfig_t;
struct csi_gpta_captureconfig {
	uint8_t		byWorkmod;          //Count or capture
	uint8_t     byCountingMode;     //csi_gpta_cntmd_e
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

typedef struct csi_gpta_pwmchannel_config      csi_gpta_pwmchannel_config_t;
struct csi_gpta_pwmchannel_config {
	
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
typedef struct csi_gpta_Global_load_control_config    csi_gpta_Global_load_control_config_t;
struct csi_gpta_Global_load_control_config{
	
	bool bGlden;
	bool bOstmd;
	uint8_t bGldprd;	
    uint8_t byGldmd;
};

/// \struct csi_gpta_filter_config_t
/// \brief  gpta sync trigger filter parameter configuration, open to users  
typedef struct {
	uint8_t		byFiltSrc;			//filter input signal source
	uint8_t		byWinInv;			//window inversion 
	uint8_t		byWinAlign;			//window alignment 
	uint8_t		byWinCross;			//window cross
	uint16_t	byWinOffset;		//window offset
	uint16_t 	byWinWidth;			//window width		
} csi_gpta_filter_config_t;


typedef enum{
	GPTA_SHDW_IMMEDIATE =0,
	GPTA_SHDW_SHADOW      
}csp_gpta_shdw_e;

typedef enum {
	GPTA_AQCSF_NOW=0,
	GPTA_AQCSF_ZRO,
	GPTA_AQCSF_PRD,
	GPTA_AQCSF_ZRO_PRD
}csp_gpta_aqosf_e;

typedef enum {
	GPTA_AQCSF_NONE=0,
	GPTA_AQCSF_L,
	GPTA_AQCSF_H,
	GPTA_AQCSF_NONE1
}csp_gpta_aqcsf_e;
typedef enum{
	GPTA_AUTO_REARM_DIS 	= 0,	//disable auto rearm
	GPTA_AUTO_REARM_ZRO,			//CNT = ZRO auto rearm
	GPTA_AUTO_REARM_PRD,			//CNT = PRD auto rearm
	GPTA_AUTO_REARM_ZRO_PRD		//CNT = PRD or PRD auto rearm
}csi_gpta_arearm_e;
/**
 * \enum     csi_gpta_trgin_e
 * \brief    GPTA sync trigger input 
 */
typedef enum{
	GPTA_TRGIN_SYNCEN0	= 0,	//start	up or reset count			
	GPTA_TRGIN_SYNCEN1,			//reg updata				
	GPTA_TRGIN_SYNCEN2,			//capture				
	GPTA_TRGIN_SYNCEN3,			//count inc or dec			
	GPTA_TRGIN_SYNCEN4,			//change output status(pwm)			
	GPTA_TRGIN_SYNCEN5			//change output status(pwm)						
}csi_gpta_trgin_e;


typedef enum {
	GPTA_UPCNT = 0,
	GPTA_DNCNT,
	GPTA_UPDNCNT
}csi_gpta_cntmd_e;

/**
 * \enum     csi_gpta_trgmode_e
 * \brief    GPTA sync trigger mode 
 */
typedef enum{
	GPTA_TRG_CONTINU		= 0,	//GPTA continuous trigger mode 
	GPTA_TRG_ONCE				//GPTA once trigger mode 							
}csi_gpta_trgmode_e;

typedef enum{
	GPTA_FILT_DIS		= 0,	//filter disable
	GPTA_FILT_SYNCIN0,			//filter input syncin0
	GPTA_FILT_SYNCIN1,			//filter input syncin1	
	GPTA_FILT_SYNCIN2,			//filter input syncin2
	GPTA_FILT_SYNCIN3,			//filter input syncin3	
	GPTA_FILT_SYNCIN4,			//filter input syncin4
	GPTA_FILT_SYNCIN5,			//filter input syncin5		
}csi_gpta_filter_insrc_e;


/**
 * \enum	csi_ept_trgout_e
 * \brief   ept event trigger out port
 */
typedef enum{
	GPTA_TRG_OUT0		= 0,	//trigger out0
	GPTA_TRG_OUT1,				//trigger out1	
//	GPTA_TRG_OUT2,				//trigger out2
//	GPTA_TRG_OUT3				//trigger out3		
}csi_gpta_trgout_e;

typedef enum{
	GPTA_CNT0INIT		= 0,	//filter input syncin0
	GPTA_CNT1INIT,				//filter input syncin1		
}csi_gpta_cntinit_e;


typedef enum
{
	GPTA_CAPTURE= 0,		
    GPTA_WAVE 	= 1		
}csi_gpta_wave_e;
typedef enum{
    GPTA_CHANNEL_A=1,
	GPTA_CHANNEL_B
}csi_gpta_channel_e;

typedef enum {
	GPTA_CH_A = 0,
	GPTA_CH_B,
}csi_gpta_chtype_e;

typedef enum {
	GPTA_SW = 0,
	GPTA_SYNC
}csi_gpta_stmd_e;

typedef enum{
	GPTA_STP_HZ = 0,
	GPTA_STP_L
}csi_gpta_stopst_e;



typedef enum {
	GPTA_POLARITY_HIGH = 0,
	GPTA_POLARITY_LOW
}csi_gpta_polarity_t;

typedef enum{
	GPTA_OP_CONT = 0,
	GPTA_OP_OT,
}csi_gpta_opmd_e;

typedef enum{
	GPTA_CAP_CONT = 0,
	GPTA_CAP_OT
}csi_gpta_capmd_e;

typedef enum{
	GPTAINT_TRGEV0 = 0x1,
	GPTAINT_TRGEV1 = 0x2,
	GPTAINT_TRGEV2 = 0x4,
	GPTAINT_TRGEV3 = 0x8,
	GPTAINT_CAPLD0 = 0x1 << 4,
	GPTAINT_CAPLD1 = 0x1 << 5,
	GPTAINT_CAPLD2 = 0x1 << 6,
	GPTAINT_CAPLD3 = 0x1 << 7,
	GPTAINT_CAU = 0x1 << 8,
	GPTAINT_CAD = 0x1 << 9,
	GPTAINT_CBU = 0x1 << 10,
	GPTAINT_CBD = 0x1 << 11,	
	GPTAINT_PEND = 0x1 << 16	
}csi_gpta_int_e;

/**
  \brief       Initialize GPTA Interface. Initializes the resources needed for the GPTA interface
  \param[in]   ptGpta    gpta handle to operate
  \param[in]   idx    gpta idx
  \return      error code \ref csi_error_t
*/
csi_error_t csi_gpta_init(csp_gpta_t *ptGpta, uint32_t idx);

/**
  \brief       De-initialize GPTA Interface. stops operation and releases the software resources used by the interface
  \param[in]   ptGpta    GPTA handle to operate
  \return      None
*/
void csi_gpta_uninit(csp_gpta_t *ptGpta);

/**
 \brief set GPTA frequency(All channels use the same the counter). 
  * \param ptGpta 	GPTA handle to operate
  * \param wHz		frequency
  * */
csi_error_t csi_gpta_set_outfreq(csp_gpta_t *ptGpta, uint32_t wHz);

/**
 \brief config gpta dual channel output mode. 
  * CHA active High ,CHB active High
*/
csi_error_t csi_gpta_set_dualout_md1(csp_gpta_t *ptGpta, uint32_t wDutyCycle, uint32_t wTns);

/**
 \brief config gpta dual channel output mode. 
  * CHA active Low ,CHB active Low
*/
csi_error_t csi_gpta_set_dualout_md2(csp_gpta_t *ptGpta, uint32_t wDutyCycle, uint32_t wTns);

/**
 \brief config gpta dual channel output mode. 
  * CHA active High ,CHB active Low
*/
csi_error_t csi_gpta_set_dualout_md3(csp_gpta_t *ptGpta,  uint32_t wDutyCycle, uint32_t wTns);

/**
 \brief config gpta dual channel output mode. 
  * CHA active Low ,CHB active Low
*/
csi_error_t csi_gpta_set_dualout_md4(csp_gpta_t *ptGpta, uint32_t wDutyCycle, uint32_t wTns);

uint16_t csi_gpta_get_prd(csp_gpta_t *ptGpta);

/**
 \brief change gpta output dutycycle. 
 \param ptGpta    gpta handle to operate
 \param wActiveTime cmpx data
*/
csi_error_t csi_gpta_change_ch_duty(csp_gpta_t *ptGpta, csi_gpta_chtype_e eCh, uint32_t wActiveTime);



 /** \brief  register gpta interrupt callback function
 * 
 *  \param[in] gpta: gpta handle to operate
 *  \param[in] callback: gpio interrupt handle function
 *  \param[in] arg: para
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_gpta_attach_callback(csp_gpta_t *gpta, void *callback, void *arg);

 /** \brief  detach gpta interrupt callback function
 *  \param[in] gpta: gpta handle to operate
 */ 
void csi_gpta_detach_callback(csp_gpta_t *ptEpt);

/**
  \brief       Config gpta width capture mode
  \param[in]   ptGpta    gpta handle to operate
  \param[in]   eMode     GPTA_CAP_CONT /  GPTA_CAP_OT
  \return      Tick Time in ns 
*/
uint32_t csi_gpta_cap_width_config(csp_gpta_t *ptGpta, csi_gpta_capmd_e eMode, uint8_t byCapWrap);

/**
  \brief       Config gpta period capture mode
  \param[in]   ptGpta    gpta handle to operate
  \param[in]   eMode     GPTA_CAP_CONT /  GPTA_CAP_OT
  \return      Tick Time in ns 
*/
uint32_t csi_gpta_cap_prd_config(csp_gpta_t *ptGpta, csi_gpta_capmd_e eMode, uint8_t byCapWrap);

/**
  \brief       get gpta capture data
  \param[in]   gpta         gpta handle to operate
  \param[in]   num          load sequence
  \return      hwData		capture data 
*/
uint16_t csi_gpta_capture_get_cap_data(csp_gpta_t *gpta, uint8_t num);

void csi_gpta_debug_enable(csp_gpta_t *ptGpta, bool bEnable);

csi_error_t csi_gpta_set_sync_filter(csp_gpta_t *ptGptaBase, csi_gpta_filter_config_t *ptFilter);

void csi_gpta_rearm_sync(csp_gpta_t *ptGptaBase,csi_gpta_trgin_e eTrgin);

csi_error_t csi_gpta_set_evtrg(csp_gpta_t *ptGptaBase, csi_gpta_trgout_e byTrgOut, csp_gpta_trgsrc0_e eTrgSrc);

csi_error_t csi_gpta_set_extsync_chnl(csp_gpta_t *ptGptaBase, csi_gpta_trgin_e eTrgIn, uint8_t byTrgChx);

void csi_gpta_set_sync(csp_gpta_t *ptGptaBase, csi_gpta_trgin_e eTrgIn, csi_gpta_trgmode_e eTrgMode, csi_gpta_arearm_e eAutoRearm);

csi_error_t csi_gpta_capture_init(csp_gpta_t *ptGptaBase, csi_gpta_captureconfig_t *ptGptaPwmCfg);

csi_error_t  csi_gpta_wave_init(csp_gpta_t *ptGptaBase, csi_gpta_pwmconfig_t *ptGptaPwmCfg);

csi_error_t csi_gpta_start(csp_gpta_t *ptgptaBase);

csi_error_t csi_gpta_global_sw(csp_gpta_t *ptGptaBase);

csi_error_t csi_gpta_global_rearm(csp_gpta_t *ptGptaBase);

csi_error_t csi_gpta_global_config(csp_gpta_t *ptGptaBase,csi_gpta_Global_load_control_config_t *Global);

csi_error_t csi_gpta_channel_config(csp_gpta_t *ptGptaBase, csi_gpta_pwmchannel_config_t *tPwmCfg, csi_gpta_channel_e channel);

csi_error_t csi_gpta_config_init(csp_gpta_t *ptGptaBase, csi_gpta_config_t *ptGptaPwmCfg);

csi_error_t csi_gpta_change_ch_duty(csp_gpta_t *ptGptaBase, csi_gpta_chtype_e eCh, uint32_t wActiveTime);

csi_error_t csi_gpta_int_enable(csp_gpta_t *ptGptaBase, csp_gpta_int_e eInt, bool bEnable);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_PWM_H_ */