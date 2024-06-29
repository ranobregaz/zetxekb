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

#include "stdint.h"
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t g_ept_prd;
typedef struct csi_ept csi_ept_t;

struct csi_ept {
    csi_dev_t      dev;
    void           (*callback)(csi_ept_t *ptEpt, bool bCntDir, uint32_t wLckSt, uint32_t wBasicSt, void *arg);
    void           *arg;
    void           *priv;
};
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

typedef enum {
	EPT_UPCNT = 0,
	EPT_DNCNT,
	EPT_UPDNCNT
}csi_gpt_cntmd_e;

typedef enum{
	EPT_OP_CONT = 0,
	EPT_OP_OT,
}csi_ept_opmd_e;
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

typedef enum {
	EPT_EVSRC_ZRO = 1,
	EPT_EVSRC_PRD,
	EPT_EVSRC_ZRO_PRD,
	EPT_EVSRC_CAU,
	EPT_EVSRC_CAD,
	EPT_EVSRC_CBU,
	EPT_EVSRC_CBD,
	EPT_EVSRC_CCU,
	EPT_EVSRC_CCD,
	EPT_EVSRC_CDU,
	EPT_EVSRC_CDD,
	EPT_EVSRC_EX,
	EPT_EVSRC_PE0,
	EPT_EVSRC_PE1,
	EPT_EVSRC_PE2,
	EPT_EVSRC_PEND
}csi_ept_evtrg_src_e;

typedef enum{
	EPT_SYNC_CONT = 0,
	EPT_SYNC_OS
}csi_ept_syncmode_e;


/**
  \brief       Initialize EPT Interface. Initializes the resources needed for the EPT interface
  \param[in]   ptEpt    ept handle to operate
  \param[in]   idx    ept idx
  \return      error code \ref csi_error_t
*/
csi_error_t csi_ept_init(csi_ept_t *ptEpt, uint32_t idx);

/**
  \brief       De-initialize EPT Interface. stops operation and releases the software resources used by the interface
  \param[in]   ptEpt    EPT handle to operate
  \return      None
*/
void csi_ept_uninit(csi_ept_t *ptEpt);

/**
 \brief set EPT frequency(All channels use the same the counter). 
  * \param ptEpt 	EPT handle to operate
  * \param wHz		frequency
  * */
csi_error_t csi_ept_set_freq(csi_ept_t *ptEpt, uint32_t wHz);

/**
 \brief set EPT frequency(All channels use the same the counter). 
  * \param ptEpt 	EPT handle to operate
  * \param wHz		frequency
  * */
csi_error_t csi_ept_set_freq_edge_aligned(csi_ept_t *ptEpt, uint32_t wHz);

/**
 \brief config pwm dual channel output mode. 
  * CHnX active High ,ChnY active High
*/
csi_error_t csi_ept_set_ch_dualout_md1(csi_ept_t *ptEpt, csi_ept_chtype_e eCh,  uint32_t wDutyCycle, uint32_t wT1ns, uint32_t wT2ns);

/**
 \brief config pwm dual channel output mode. 
  * CHnX active Low ,ChnY active Low
*/
csi_error_t csi_ept_set_ch_dualout_md2(csi_ept_t *ptEpt, csi_ept_chtype_e eCh,  uint32_t wDutyCycle, uint32_t wT1ns, uint32_t wT2ns);

/**
 \brief config pwm dual channel output mode. 
  * CHnX active High ,ChnY active Low
*/
csi_error_t csi_ept_set_ch_dualout_md3(csi_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wDutyCycle, uint32_t wT1ns, uint32_t wT2ns);

/**
 \brief config pwm dual channel output mode. 
  * CHnX active Low ,ChnY active Low
*/
csi_error_t csi_ept_set_ch_dualout_md4(csi_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wDutyCycle, uint32_t wT1ns, uint32_t wT2ns);

uint16_t csi_ept_get_prd(csi_ept_t *ptEpt);

/**
 \brief change ept output dutycycle. 
 \param ptEpt    ept handle to operate
 \param wActiveTime cmpx data
*/
csi_error_t csi_ept_change_ch_duty(csi_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wActiveTime);

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

csi_error_t csi_ept_set_singleout_md1(csi_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wDutyCycle);


/**
 \brief config ptEpt single output mode2

  * ChnX      _______       _______
  *                 |______|
  * 	
  * \param ptEpt 	EPT handle to operate
  * \param eCh		EPT output channel(EPT_CH_A/B/C/D)
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \return tRet   
*/

csi_error_t csi_ept_set_singleout_md2(csi_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wDutyCycle);

/**
 \brief config ptEpt single output mode1, edge aligned
  * * Counter works in up and down mode
  * \verbatim
  *  	ChnX    	______
  *  		_______|      
  * \endverbatim	
  * \param ptEpt 	EPT handle to operate
  * \param eCh		EPT output channel(EPT_CH_A/B/C/D)
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \return tRet   
*/

csi_error_t csi_ept_set_singleout_edge_aligned_md1(csi_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wDutyCycle);




/**
 \brief config pwm dual channel output mode. 
  * CHnX active Low ,ChnY active Low
*/
void csi_ept_set_cp_ospwth(csi_ept_t *ptEpt, uint8_t byVal);

/**
 \brief chopper configuration
 \param ptEpt    		ept handle to operate 
 \param byCdiv   		fchop = PCLK/((byCdiv+1)*8)
 \param byCduty  		(0~7) duty = byduty/8 
 \param byFirstWidth 	T = Tchop * byFirstWidth
 \return none 
*/
void csi_ept_set_cp(csi_ept_t *ptEpt, uint8_t byCdiv, uint8_t byCduty, uint8_t byFirstWidth);

/**
 \brief channeln chopper output enable 
 \param ptEpt    ept handle to operate
 \param bEnableX/Y  ENABLE/DISABLE
 \return none
*/
void csi_ept_cpa_enable(csi_ept_t *ptEpt, bool bEnableX, bool bEnableY);
void csi_ept_cpb_enable(csi_ept_t *ptEpt, bool bEnableX, bool bEnableY);
void csi_ept_cpc_enable(csi_ept_t *ptEpt, bool bEnableX, bool bEnableY);

/**
 \brief channeln chopper output enable 
 \param ptEpt    ept handle to operate
 \param eEbi 	 external emergency input: EPT_EBI0~4 （EBI4 = LVD）
 \param ePol	 active polarity：EPT_EBI_POL_H/L
 \param eLckMode lock mode: EPT_EP_DIS/SLCK/HLCK
 \param eOutput  output when lock happens: EPT_EM_OUT_HZ/H/L/DIS
 \return none
*/
void csi_ept_set_em(csi_ept_t *ptEpt, csi_ept_ebi_e eEbi, csi_ept_ebipol_e ePol, csi_ept_ep_lckmd_e eLckMode, csi_ept_em_out_e eOutput, bool bIntEn);

/**
 \brief clear harklock status
 \param ptEpt    ept handle to operate
 \return  eEbi 	 external emergency input: EPT_EBI0~3/EPT_EBILVD
*/


void csi_ept_debug_enable(csi_ept_t *ptEpt, bool bEnable);
void csi_ept_clr_hdlck(csi_ept_t *ptEpt, csi_ept_ebi_e eEbi);

void csi_ept_force_em(csi_ept_t *ptEpt, uint8_t byEp);
void csi_ept_set_em_flt(csi_ept_t *ptEpt, uint8_t byVal);

void csi_ept_set_start_mode(csi_ept_t *ptEpt, csi_ept_stmd_e eMode);
csi_error_t csi_ept_set_phase(csi_ept_t *ptEpt, uint16_t bwVal);
void csi_ept_set_os_mode(csi_ept_t *ptEpt, csi_ept_opmd_e eMode);
void csi_ept_swstart(csi_ept_t *ptEpt);
void csi_ept_set_stop_st(csi_ept_t *ptEpt, csi_ept_stopst_e eSt);
void csi_ept_swstop(csi_ept_t *ptEpt);



uint16_t csi_ept_capture_get_cap_data(csi_ept_t *ptEpt, uint8_t num);

 /** \brief  register ept interrupt callback function
 * 
 *  \param[in] ept: ept handle to operate
 *  \param[in] callback: gpio interrupt handle function
 *  \param[in] arg: para
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_ept_attach_callback(csi_ept_t *ptEpt, void *callback, void *arg);

 /** \brief  detach ept interrupt callback function
 *  \param[in] ept: ept handle to operate
 */ 
void csi_ept_detach_callback(csi_ept_t *ptEpt);

/**
 \brief get harklock status
 \param ptEpt    ept handle to operate
 \return uint8_t 0b_ _ _ _ _ _ _ _
*/
uint8_t csi_ept_get_hdlck_st(csi_ept_t *ptEp);

/**
 \brief clear harklock status
 \param ptEpt    ept handle to operate
 \return  eEbi 	 external emergency input: EPT_EBI0~3/EPT_EBILVD
*/
void csp_ept_clr_hdlck(csi_ept_t *ptEp, csi_ept_ebi_e eEbi);


/**
  \brief   config ept out trigger 
  \param   ptEpt       EPT handle to operate
  \param 	eCh			0/1/2/3
  \param 	eSrc	
  \param   byTime		output trigger when event happens the 'byTime'th time.
*/
csi_error_t csi_ept_set_evtrg(csi_ept_t *ptEpt,uint8_t eCh,csi_ept_evtrg_src_e eSrc, uint8_t byTime);

/**
  \brief   enable/disable ept out trigger 
  \param   ptEpt       EPT handle to operate
  \param 	eCh			0/1/2/3
  \param   bEnable		ENABLE/DISABLE
*/
csi_error_t csi_ept_evtrg_enable(csi_ept_t *ptEpt, uint8_t byCh, bool bEnable);

/**
  \brief   set ept sync
  \param   ptEpt       EPT handle to operate
   \param   bySync    sync select: 0/1/2/3
   \param  csi_ept_syncmode_t  EPT_SYNC_CONT/EPT_SYNC_OS
   \param  bARearmEnable auto rearm enable/disable
  \return csi_error_t   
*/
csi_error_t csi_ept_set_sync(csi_ept_t *ptEpt, uint8_t bySync, csi_ept_syncmode_e tSyncMode, bool bARearmEnable);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_EPT_H_ */