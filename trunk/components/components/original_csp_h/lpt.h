/***********************************************************************//** 
 * \file  lpt.h
 * \brief header file for lpt driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-14 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-8  <td>V0.1  <td>WNN   <td>modify
 * </table>
 * *********************************************************************
*/

#ifndef _DRV_LPT_H_
#define _DRV_LPT_H_

#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct csi_lpt csi_lpt_t;

struct csi_lpt{
    csi_dev_t        dev;
    void            (*callback)(csi_lpt_t *lpt, void *arg);
    void            *arg;
    void            *priv;
} ;

typedef enum
{     
	LPT_IDLE_LOW = 0,	
    LPT_IDLE_Z   	    
}csi_lpt_idlepol_t;

typedef enum
{
	LPT_POL_LOW	= 0,
    LPT_POL_HIGH,       
}csi_lpt_startpol_t;

typedef enum
{
    LPT_SYNC_CONT 	= (0x00ul),  	       
	LPT_SYNC_ONCE	= (0x01ul)	
}csi_lpt_syncmode_t;

typedef enum
{
    LPT_TRGSRC_DIS 		= (0x00ul ),  	       
	LPT_TRGSRC_ZRO 		= (0x01ul ),
	LPT_TRGSRC_PRD 		= (0x02ul ),
	LPT_TRGSRC_ZRO_PRD 	= (0x03ul ),
	LPT_TRGSRC_CMP		= (0x04ul )
}csi_lpt_trgsrc_t;

typedef enum
{
	LPT_CLK_PCLK_DIV4 	= (0x00ul ),
    LPT_CLK_ISCLK 		= (0x01ul),
	LPT_CLK_IMCLK_DIV4  = (0x02ul),
	LPT_CLK_EMCLK 		= (0x03ul ),
	LPT_CLK_IN_RISE 	= (0x04ul ),
	LPT_CLK_IN_FALL 	= (0x05ul),	
}csi_lpt_clksrc_t;

typedef enum
{
    UPDATA_IM 			= 0,  	       
	UPDATA_PEND		
}csi_lpt_updata_t;

typedef enum
{
	LPT_INT_TRGEV0 = 0,
	LPT_INT_MATCH,
	LPT_INT_PEND,
}csi_lpt_int_t;

/**
  \brief       Initialize LPT Interface. Initializes the resources needed for the LPT interface
  \param[in]   lpt    handle lpt handle to operate
  \param[in]   idx      lpt index
  \return      error code \ref csi_error_t
*/
csi_error_t csi_lpt_init(csi_lpt_t *lpt, uint32_t idx);

/**
  \brief       De-initialize LPT Interface. stops operation and releases the software resources used by the interface
  \param[in]   lpt    handle lpt handle to operate
  \return      None
*/
void csi_lpt_uninit(csi_lpt_t *lpt);

/**
  \brief       Start lpt
  \param[in]   lpt         handle lpt handle to operate
  \param[in]   timeout_us  the timeout for lpt
  \param[in]   clk_src     lpt clk source
  \return      error code \ref csi_error_t
*/
csi_error_t csi_lpt_start(csi_lpt_t *lpt, csi_lpt_clksrc_t clk_src, uint32_t timeout_us);

/**
  \brief       Stop lpt
  \param[in]   lpt    handle lpt handle to operate
  \return      None
*/
void csi_lpt_stop(csi_lpt_t *lpt);

/**
  \brief       Get lpt remaining value
  \param[in]   lpt    handle lpt handle to operate
  \return      lpt    remaining value
*/
uint32_t csi_lpt_get_remaining_value(csi_lpt_t *lpt);

/**
  \brief       Get lpt load value
  \param[in]   lpt    handle lpt handle to operate
  \return      lpt    load value
*/
uint32_t csi_lpt_get_load_value(csi_lpt_t *lpt);

/**
  \brief       Check lpt is running
  \param[in]   lpt    handle lpt handle to operate
  \return      true->running, false->stopped
*/
bool csi_lpt_is_running(csi_lpt_t *lpt);

/**
  \brief       Attach the callback handler to lpt
  \param[in]   lpt       operate handle.
  \param[in]   callback    callback function
  \param[in]   arg         callback's param
  \return      error  code \ref csi_error_t
*/
csi_error_t csi_lpt_attach_callback(csi_lpt_t *lpt, void *callback, void *arg);

/**
  \brief       Detach the callback handler
  \param[in]   lpt    operate handle.
*/
void csi_lpt_detach_callback(csi_lpt_t *lpt);

/**
  \brief       Enable lpt power manage
  \param[in]   lpt    lpt handle to operate.
  \return      error code \ref csi_error_t
*/
csi_error_t csi_lpt_enable_pm(csi_lpt_t *lpt);

/**
  \brief       Disable lpt power manage
  \param[in]   lpt    lpt handle to operate.
*/
void csi_lpt_disable_pm(csi_lpt_t *lpt);

/**
  \brief       lpt pwm init
  \param[in]   lpt			lpt handle to operate.
  \param[in]   init_polar   lpt pwm output initial polarity
  \param[in]   stop_lev     lpt pwm output stop level
*/
csi_error_t csi_lpt_pwm_init(csi_lpt_t *lpt, csi_lpt_startpol_t pol_lev, csi_lpt_idlepol_t idle_lev);

/** \brief start lpt pwm
 * 
 *  \param[in] lpt: handle lpt handle to operate
 *  \param eClk: clk source selection
 *  \param[in] freq: pwm frequency  
 *  \param[in] duty cycle: duty cycle(0 -> 100)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_pwm_start(csi_lpt_t *lpt, csi_lpt_clksrc_t eClk, uint32_t freq, uint32_t duty_cycle) ;

/** \brief lpt work as a timer(sync start)
 * 
 *  \param[in] lpt: LPT handle to operate
 *  \param[in] eClk: lpt clock source selection
 *  \param[in] wms: ms
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_start_sync(csi_lpt_t *lpt, csi_lpt_clksrc_t eClk, uint32_t wms);

/** \brief start lpt pwm by external sync
 * 
 *  \param[in] lpt: handle lpt handle to operate
 *  \param eClk: clk source selection
 *  \param[in] freq: pwm frequency  in Hz
 *  \param[in] duty cycle: duty cycle(0 -> 100)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_pwm_start_sync(csi_lpt_t *lpt, csi_lpt_clksrc_t eClk, uint32_t freq, uint32_t duty_cycle);

/** \brief change lpt duty cycle
 * 
 *  \param lpt: LPT handle to operate
 *  \param duty_cycle: lpt clock source selection
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_change_duty(csi_lpt_t *lpt, uint32_t duty_cycle);

/** 
  \brief updata lpt pwm freq para: (prdr and cmp value)
  \param[in] lpt: handle lpt handle to operate
  \param[in] hwCmp: duty cycle
  \param[in] hwPrdr: period 
  \param[in] mode_updata: updata mode 
  \return none
 */
void csi_lpt_pwm_para_updata(csi_lpt_t *lpt, uint16_t hwCmp, uint16_t hwPrdr, csi_lpt_updata_t mode_updata);

/** \brief lpt sync window config  
 * 
 *  \param[in] lpt: LPT handle to operate
 *  \param bCrossEnable: window cross enable/disable
 *  \param bInvEnable: window invert enable/disable
 *  \param hwOffset: window start point from CNT=ZRO
 *  \param hwWindow: window width
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_set_sync_window(csi_lpt_t *lpt, bool bCrossEnable, bool bInvEnable, uint16_t hwOffset, uint16_t hwWindow);

/** \brief lpt sync config  
 * 
 *  \param[in] lpt: LPT handle to operate
 *  \param bySync: select sync
 *  \param tSyncMode: LPT_TRG_CONT/LPT_TRG_ONCE
 *  \param bARearmEnable: auto rearm enable/disable
 *  \return csi_error_t
 */
csi_error_t csi_lpt_set_sync(csi_lpt_t *lpt, uint8_t bySync, csi_lpt_syncmode_t tSyncMode, bool bARearmEnable);


/** \brief restart lpt sync 
 * 
 *  \param[in] lpt: LPT handle to operate
 *  \param bySync: sync select
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_rearm_sync(csi_lpt_t *lpt, uint8_t bySync);

/** \brief lpt evtrg source output config  
 * 
 *  \param[in] lpt: LPT handle to operate
 *  \param[in] lpt_trgsrc: lpt evtrg source(1~4) 
 *  \param[in] trg_prd: event count period 
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_set_evtrg(csi_lpt_t *lpt, uint8_t  byEvtrg, csi_lpt_trgsrc_t tTrgSrc, uint8_t trg_prd);

/** \brief lpt set frequency 
 * 
 *  \param[in] lpt: LPT handle to operate
 *  \param[in] eClk: lpt clock source selection
 *  \param[in] wHz: frequency
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_set_fre(csi_lpt_t *lpt, csi_lpt_clksrc_t eClk, uint16_t wHz);

/** \brief LPT software sync enable control
 * 
 *  \param lpt: LPT handle to operate
 *  \param bEnable: ENABLE/DISABLE
 *  \return void
 */
void csi_lpt_swsync_enable(csi_lpt_t *lpt, bool bEnable);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_TIMER_H_ */