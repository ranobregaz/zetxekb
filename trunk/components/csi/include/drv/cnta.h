/***********************************************************************//** 
 * \file  cnta.h
 * \brief header file for cnta driver
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version   <th>Author  <th>Description
 * <tr><td> 2020-9-09 <td>V0.0  <td>ZJY     <td>initial
 * <tr><td> 2021-5-27 <td>V0.1  <td>LQ      <td>modify
 * </table>
 * *********************************************************************
*/

#ifndef _DRV_CNTA_H_
#define _DRV_CNTA_H_

#include <drv/common.h>
#include <csp_cnta.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	POLAR_LOW	= 0,
    POLAR_HIGH,       
	STOP_LOW,	
    STOP_HIGH   	    
}csi_cnta_pwmlev_t;

typedef enum
{
	PWM_CARRIER		= 0,
    PWM_ENVELOPE    
}csi_cnta_pwmout_t;

typedef enum
{
	MATCH_CARRIERON_DIS	= 0,
	MATCH_CARRIERON_CLR, 
	MATCH_CARRIERON_SET       
}csi_cnta_tcmatch_t;

typedef enum
{
	PEND_CARRIERON_DIS	= 0,
	PEND_CARRIERON_CLR, 
	PEND_CARRIERON_SET      
}csi_cnta_tcpend_t;

typedef enum
{
	CNTA_SW_EN			= (0x01ul)  
}csi_cnta_sw_updata_t;

typedef enum
{
	CNTA_HW_DIS			= (0x00ul),
	CNTA_HW_TCMATCH		= (0x01ul), 
	CNTA_HW_TCPEND		= (0x02ul)       
}csi_cnta_hw_updata_t;

/** \brief initialize cnta data structure
 * 
 *  \param[in] ptCntaBase: handle cnta handle to operate
 *  \param[in] eClkDiv: the cnta clk_div
 *  \param[in] eRunMode: the cnta run mode
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_cnta_timer_init(csp_cnta_t *ptCntaBase,cnta_ckdiv_e eClkDiv,cnta_mode_e eRunMode);

/** \brief start cnta
 * 
 *  \param[in] ptCntaBase: handle cnta handle to operate
 *  \param[in] timeout_us: the timeout for timer((0.333us * 1) ->(0.333us * 65535): 0.3333us -> 21.845ms)
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_cnta_timer_start(csp_cnta_t *ptCntaBase,uint32_t timeout_us);

/**
  \brief       Stop cnta
  \param[in]   ptCntaBase    handle cnta handle to operate
  \return      None
*/
void csi_cnta_stop(csp_cnta_t *ptCntaBase);

/**
  \brief       Get cnta datah value
  \param[in]   ptCntaBase    handle cnta handle to operate
  \return      cnta    load value
*/
uint32_t csi_cnta_get_datah_value(csp_cnta_t *cnta);

/** \brief get cnta datal load value
 * 
 *  \param[in] ptCntaBase: handle cnta handle to operate
 *  \return none
 */ 
uint32_t csi_cnta_get_datal_value(csp_cnta_t *ptCntaBase);

/**
  \brief       cnta pwm init
  \param[in]   ptCntaBase	cnta handle to operate.
  \param[in]   eClkDiv      the cnta clk_div
  \param[in]   init_polar   cnta pwm output mode, carrier/envelope
  \param[in]   init_polar   cnta pwm output initial polarity
  \param[in]   stop_lev     cnta pwm output stop level
*/
csi_error_t csi_cnta_pwm_init(csp_cnta_t *ptCntaBase,cnta_ckdiv_e eClkDiv, csi_cnta_pwmout_t pwm_mode ,csi_cnta_pwmlev_t init_polar, csi_cnta_pwmlev_t stop_lev);

/**
  \brief       start cnta pwm
  \param[in]   ptCntaBase    cnta handle to operate.
  \param[in]   freq    		pwm frequency 
  \param[in]   duty_cycle   duty cycle
*/
csi_error_t csi_cnta_pwm_start(csp_cnta_t *ptCntaBase, uint32_t freq, uint32_t duty_cycle);

/**
  \brief       updata cnta pwm freq para
  \param[in]   ptCntaBase   cnta handle to operate.
  \param[in]   datah    	pwm ouput high time long 
  \param[in]   datal   		pwm ouput low time long 
  \param[in] sw_updata:    cadata reg soft updata control
*/
void csi_cnta_pwm_para_updata(csp_cnta_t *ptCntaBase, uint16_t datah, uint16_t datal, csi_cnta_sw_updata_t sw_updata);

/** \brief set cnta tc sync 
 *  \param[in] ptCntaBase: handle cnta handle to operate
 *  \param[in] tcpend_rem: carrieron control of tc pend end 
 *  \param[in] tcmatch_rem: carrieron control of tc pulse match 
 *  \param[in] hw_updata: cadata reg auto updata control
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_cnta_set_sync(csp_cnta_t *ptCntaBase, csi_cnta_tcpend_t tcpend_rem, csi_cnta_tcmatch_t tcmatch_rem,csi_cnta_hw_updata_t hw_updata);
//---------------------------------------------------------------------------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* _DRV_TIMER_H_ */