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

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t g_gpta_prd;
typedef struct csi_gpta csi_gpta_t;

struct csi_gpta {
    csi_dev_t      dev;
    void           (*callback)(csi_gpta_t *ptGpta, uint32_t wBasicSt, void *arg);
    void           *arg;
    void           *priv;
};
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
	UPCNT = 0,
	DNCNT,
	UPDNCNT
}csi_gpta_cntmd_e;

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
csi_error_t csi_gpta_init(csi_gpta_t *ptGpta, uint32_t idx);

/**
  \brief       De-initialize GPTA Interface. stops operation and releases the software resources used by the interface
  \param[in]   ptGpta    GPTA handle to operate
  \return      None
*/
void csi_gpta_uninit(csi_gpta_t *ptGpta);

/**
 \brief set GPTA frequency(All channels use the same the counter). 
  * \param ptGpta 	GPTA handle to operate
  * \param wHz		frequency
  * */
csi_error_t csi_gpta_set_outfreq(csi_gpta_t *ptGpta, uint32_t wHz);

/**
 \brief config gpta dual channel output mode. 
  * CHA active High ,CHB active High
*/
csi_error_t csi_gpta_set_dualout_md1(csi_gpta_t *ptGpta, uint32_t wDutyCycle, uint32_t wTns);

/**
 \brief config gpta dual channel output mode. 
  * CHA active Low ,CHB active Low
*/
csi_error_t csi_gpta_set_dualout_md2(csi_gpta_t *ptGpta, uint32_t wDutyCycle, uint32_t wTns);

/**
 \brief config gpta dual channel output mode. 
  * CHA active High ,CHB active Low
*/
csi_error_t csi_gpta_set_dualout_md3(csi_gpta_t *ptGpta,  uint32_t wDutyCycle, uint32_t wTns);

/**
 \brief config gpta dual channel output mode. 
  * CHA active Low ,CHB active Low
*/
csi_error_t csi_gpta_set_dualout_md4(csi_gpta_t *ptGpta, uint32_t wDutyCycle, uint32_t wTns);

uint16_t csi_gpta_get_prd(csi_gpta_t *ptGpta);

/**
 \brief change gpta output dutycycle. 
 \param ptGpta    gpta handle to operate
 \param wActiveTime cmpx data
*/
csi_error_t csi_gpta_change_ch_duty(csi_gpta_t *ptGpta, csi_gpta_chtype_e eCh, uint32_t wActiveTime);

/**
 \brief config gpta single output mode1

  * ChnX    		  _____
  *           _______|    |
  * 	
  * \param ptGpta 	GPTA handle to operate
  * \param eCh		GPTA output channel(GPTA_CH_A/B/C/D)
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \return tRet   
*/

csi_error_t csi_gpta_set_singleout_md1(csi_gpta_t *ptGpta, csi_gpta_chtype_e eCh, uint32_t wDutyCycle);

/**
 \brief config gpta single output mode1

  * CHA      _______       _______
  *                 |______|
  * 	
  * \param ptGpta 	GPTA handle to operate  
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \return tRet   
*/
csi_error_t csi_gpta_set_singleout_md2(csi_gpta_t *ptGpta, csi_gpta_chtype_e eCh, uint32_t wDutyCycle);
/**
 \brief config gpta dual channel output mode. 
  * CHA active Low ,CHB active Low
*/
void csi_gpta_set_cp_ospwth(csi_gpta_t *ptGpta, uint8_t byVal);



void csi_gpta_set_start_mode(csi_gpta_t *ptGpta, csi_gpta_stmd_e eMode);
csi_error_t csi_gpta_set_phase(csi_gpta_t *ptGpta, uint16_t bwVal);
void csi_gpta_set_os_mode(csi_gpta_t *ptGpta, csi_gpta_opmd_e eMode);
void csi_gpta_swstart(csi_gpta_t *ptGpta);
void csi_gpta_set_stop_st(csi_gpta_t *ptGpta, csi_gpta_stopst_e eSt);
void csi_gpta_out_swstop(csi_gpta_t *ptGpta);

 /** \brief  register gpta interrupt callback function
 * 
 *  \param[in] gpta: gpta handle to operate
 *  \param[in] callback: gpio interrupt handle function
 *  \param[in] arg: para
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_gpta_attach_callback(csi_gpta_t *gpta, void *callback, void *arg);

 /** \brief  detach gpta interrupt callback function
 *  \param[in] gpta: gpta handle to operate
 */ 
void csi_gpta_detach_callback(csi_gpta_t *ptEpt);

/**
  \brief       Config gpta width capture mode
  \param[in]   ptGpta    gpta handle to operate
  \param[in]   eMode     GPTA_CAP_CONT /  GPTA_CAP_OT
  \return      Tick Time in ns 
*/
uint32_t csi_gpta_cap_width_config(csi_gpta_t *ptGpta, csi_gpta_capmd_e eMode, uint8_t byCapWrap);

/**
  \brief       Config gpta period capture mode
  \param[in]   ptGpta    gpta handle to operate
  \param[in]   eMode     GPTA_CAP_CONT /  GPTA_CAP_OT
  \return      Tick Time in ns 
*/
uint32_t csi_gpta_cap_prd_config(csi_gpta_t *ptGpta, csi_gpta_capmd_e eMode, uint8_t byCapWrap);

/**
  \brief       get gpta capture data
  \param[in]   gpta         gpta handle to operate
  \param[in]   num          load sequence
  \return      hwData		capture data 
*/
uint16_t csi_gpta_capture_get_cap_data(csi_gpta_t *gpta, uint8_t num);

void csi_gpta_debug_enable(csi_gpta_t *ptGpta, bool bEnable);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_PWM_H_ */