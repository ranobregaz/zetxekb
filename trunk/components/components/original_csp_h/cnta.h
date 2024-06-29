/***********************************************************************//** 
 * \file  cnta.h
 * \brief header file for cnta driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-14 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-12 <td>V0.0  <td>ZJY   <td>modify
 * </table>
 * *********************************************************************
*/

#ifndef _DRV_CNTA_H_
#define _DRV_CNTA_H_

#include <drv/common.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct csi_cnta csi_cnta_t;

struct csi_cnta{
    csi_dev_t        dev;
    void            (*callback)(csi_cnta_t *cnta, void *arg);
    void            *arg;
    void            *priv;
} ;

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

/**
  \brief       Initialize CNTA Interface. Initializes the resources needed for the CNTA interface
  \param[in]   cnta    handle cnta handle to operate
  \param[in]   idx     cnta index
  \return      error code \ref csi_error_t
*/
csi_error_t csi_cnta_init(csi_cnta_t *cnta, uint32_t idx);

/**
  \brief       De-initialize TIMER Interface. stops operation and releases the software resources used by the interface
  \param[in]   cnta    handle cnta handle to operate
  \return      None
*/
void csi_cnta_uninit(csi_cnta_t *cnta);

/**
  \brief       Start cnta
  \param[in]   cnta          handle cnta handle to operate
  \param[in]   timeout_us    the timeout for cnta
  \return      error code \ref csi_error_t
*/
csi_error_t csi_cnta_start(csi_cnta_t *cnta, uint32_t timeout_us);

/**
  \brief       Stop cnta
  \param[in]   cnta    handle cnta handle to operate
  \return      None
*/
void csi_cnta_stop(csi_cnta_t *cnta);

/**
  \brief       Get cnta remaining value
  \param[in]   cnta    handle cnta handle to operate
  \return      cnta    remaining value
*/
uint32_t csi_cnta_get_remaining_value(csi_cnta_t *cnta);

/**
  \brief       Get cnta load value
  \param[in]   cnta    handle cnta handle to operate
  \return      cnta    load value
*/
uint32_t csi_cnta_get_load_value(csi_cnta_t *cnta);

/**
  \brief       Check cnta is running
  \param[in]   cnta    handle cnta handle to operate
  \return      true->running, false->stopped
*/
bool csi_cnta_is_running(csi_cnta_t *cnta);

/**
  \brief       Attach the callback handler to cnta
  \param[in]   cnta       operate handle.
  \param[in]   callback    callback function
  \param[in]   arg         callback's param
  \return      error  code \ref csi_error_t
*/
csi_error_t csi_cnta_attach_callback(csi_cnta_t *cnta, void *callback, void *arg);

/**
  \brief       Detach the callback handler
  \param[in]   cnta    operate handle.
*/
void csi_cnta_detach_callback(csi_cnta_t *cnta);

/**
  \brief       Enable cnta power manage
  \param[in]   cnta    cnta handle to operate.
  \return      error code \ref csi_error_t
*/
csi_error_t csi_cnta_enable_pm(csi_cnta_t *cnta);

/**
  \brief       Disable cnta power manage
  \param[in]   cnta    cnta handle to operate.
*/
void csi_cnta_disable_pm(csi_cnta_t *cnta);

/**
  \brief       cnta pwm init
  \param[in]   cnta			cnta handle to operate.
  \param[in]   init_polar   cnta pwm output mode, carrier/envelope
  \param[in]   init_polar   cnta pwm output initial polarity
  \param[in]   stop_lev     cnta pwm output stop level
*/
csi_error_t csi_cnta_pwm_init(csi_cnta_t *cnta, csi_cnta_pwmout_t pwm_mode ,csi_cnta_pwmlev_t init_polar, csi_cnta_pwmlev_t stop_lev);

/**
  \brief       start cnta pwm
  \param[in]   cnta    		cnta handle to operate.
  \param[in]   freq    		pwm frequency 
  \param[in]   duty_cycle   duty cycle
*/
csi_error_t csi_cnta_pwm_start(csi_cnta_t *cnta, uint32_t freq, uint32_t duty_cycle);

/**
  \brief       updata cnta pwm freq para
  \param[in]   cnta    		cnta handle to operate.
  \param[in]   datah    	pwm ouput high time long 
  \param[in]   datal   		pwm ouput low time long 
  \param[in] sw_updata:    cadata reg soft updata control
*/
void csi_cnta_pwm_para_updata(csi_cnta_t *cnta, uint16_t datah, uint16_t datal, csi_cnta_sw_updata_t sw_updata);

/** 
  \brief set cnta tc sync 
  \param[in] cnta: handle cnta handle to operate
  \param[in] tcpend_rem: carrieron control of tc pend end 
  \param[in] tcmatch_rem: carrieron control of tc pulse match 
  \param[in] hw_updata: cadata reg auto updata control
  \return error code \ref csi_error_t
 */
csi_error_t csi_cnta_set_sync(csi_cnta_t *cnta, csi_cnta_tcpend_t tcpend_rem, csi_cnta_tcmatch_t tcmatch_rem,csi_cnta_hw_updata_t hw_updata);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_TIMER_H_ */