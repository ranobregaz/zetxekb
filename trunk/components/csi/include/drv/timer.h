/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     timer.h
 * @brief    header file for timer driver
 * @version  V1.0
 * @date     27. Apr 2020
 * @model    timer
 ******************************************************************************/

#ifndef _DRV_TIMER_H_
#define _DRV_TIMER_H_

#include <drv/common.h>

#include "csp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct csi_timer csi_timer_t;

struct csi_timer {
	uint8_t		byIdleLevel;		//TIMER PWM OUTPUT idel level
	uint8_t		byStartLevel;		//TIMER PWM OUTPUT start Level
	uint8_t		byInter;			//TIMER PWM interrupt source select
	uint8_t		byDutyCycle;		//TIMER PWM OUTPUT duty cycle
	uint32_t	wFreq;				//TIMER PWM OUTPUT frequency
	
//	void        (*callback)(csp_bt_t *ptTmBase, void *pArg);
//	void         *pArg;
//	void         *priv;
//	csi_dev_t	dev;
//    void        (*callback)(csi_timer_t *timer, void *arg);
//    void        *arg;
//    void        *priv;
} ;

typedef enum
{
	T_START_LOW		= 0,
    T_START_HIGH 	= 1,       
	T_IDLE_LOW  	= 0,	
    T_IDLE_HIGH		= 1   	    
}csi_timer_pwmlev_e;

typedef enum{
	TIMER_SYNCEN0		= 0,
	TIMER_SYNCEN1		
}csi_timer_trgin_e;

typedef enum{
	TIMER_TRG_CONTINU	= 0,
	TIMER_TRG_ONCE					
}csi_timer_trgmode_e;

typedef enum
{
	TIMER_TRGSRC_DIS	= 0,  	       
	TIMER_TRGSRC_PEND,	
	TIMER_TRGSRC_CMP,	
	TIMER_TRGSRC_OVF	         
}csi_timer_trgsrc_e;

typedef enum
{
	TIMER_NONE_INT   =	(0x00ul << 0), 			//none interrupt
	TIMER_PEND_INT   =	(0x01ul << 0), 
	TIMER_CMP_INT    =	(0x01ul << 1),     
	TIMER_OVF_INT    =	(0x01ul << 2),  
	TIMER_EVTRG_INT   =	(0x01ul << 3)
}csi_timer_intsrc_e;

/**
  \brief       Initialize TIMER Interface. Initializes the resources needed for the TIMER interface
  \param[in]   ptTmBase		pointer of timer register structure
  \param[in]   wTimeOut		the timeout for timer, unit: us
  \return      error code \ref csi_error_t
*/
csi_error_t csi_bt_timer_init(csp_bt_t *ptTmBase, uint32_t wTimeOut);

/**
  \brief       De-initialize TIMER Interface. stops operation and releases the software resources used by the interface
  \param[in]   timer    handle timer handle to operate
  \return      None
*/
//void csi_timer_uninit(csi_timer_t *timer);

/**
  \brief       Start timer
  \param[in]   timer		handle timer handle to operate
  \param[in]   timeout_us    the timeout for timer
  \return      error code \ref csi_error_t
*/
//csi_error_t csi_timer_start(csi_timer_t *timer, uint32_t timeout_us);

/**
  \brief       Stop timer
  \param[in]   timer    	handle timer handle to operate
  \return      None
*/
//void csi_timer_stop(csi_timer_t *timer);

/** 
  \brief 	   open(start) timer
  \param[in]   ptTmBase		pointer of timer register structure
  \return      error code \ref csi_error_t
 */ 
csi_error_t csi_bt_open(csp_bt_t *ptTmBase);

/** 
  \brief       close(stop) timer
  \param[in]   ptTmBase		pointer of timer register structure
  \return      error code \ref csi_error_t
 */ 
csi_error_t csi_bt_close(csp_bt_t *ptTmBase);

/**
  \brief       Get timer remaining value
  \param[in]   ptTmBase		pointer of timer register structure
  \return      timer		remaining value
*/
uint32_t csi_timer_get_remaining_value(csp_bt_t *ptTmBase);

/**
  \brief       Get timer load value
  \param[in]   ptTmBase		pointer of timer register structure
  \return      timer		load value
*/
uint32_t csi_timer_get_load_value(csp_bt_t *ptTmBase);

/**
  \brief       Check timer is running
  \param[in]   ptTmBase		pointer of timer register structure
  \return      true->running, false->stopped
*/
bool csi_timer_is_running(csp_bt_t *ptTmBase);

/**
  \brief       Attach the callback handler to timer
  \param[in]   timer       operate handle.
  \param[in]   callback    callback function
  \param[in]   arg         callback's param
  \return      error  code \ref csi_error_t
*/
//csi_error_t csi_timer_attach_callback(csi_timer_t *timer, void *callback, void *arg);

/**
  \brief       Detach the callback handler
  \param[in]   timer    operate handle.
*/
//void csi_timer_detach_callback(csi_timer_t *timer);

/**
  \brief       Enable timer power manage
  \param[in]   timer    timer handle to operate.
  \return      error code \ref csi_error_t
*/
csi_error_t csi_timer_enable_pm(csi_timer_t *timer);

/**
  \brief       Disable timer power manage
  \param[in]   timer    timer handle to operate.
*/
void csi_timer_disable_pm(csi_timer_t *timer);

/**
  \brief       timer pwm init
  \param[in]   ptTmBase		pointer of timer register structure
  \param[in]   ptTmPara		pointer of timer parameter structure
  \return      error code \ref csi_error_t
*/
csi_error_t csi_bt_pwm_init(csp_bt_t *ptTmBase, csi_timer_t *ptTmPara);

/**
  \brief       start timer pwm
  \param[in]   timer		timer handle to operate.
  \param[in]   freq    		pwm frequency 
  \param[in]   duty_cycle   duty cycle
  \return      error code \ref csi_error_t
*/
//csi_error_t csi_timer_pwm_start(csi_timer_t *timer, uint32_t freq, uint32_t duty_cycle);

/** 
  \brief  	   updata timer para and cmp reg value
  \param[in]   ptTmBase		pointer of timer register structure
  \param[in]   hwPrdr		timer prdr reg value
  \param[in]   hwCmp  		timer cmp reg value
  \return      none
 */
csi_error_t csi_timer_prdr_cmp_updata(csp_bt_t *ptTmBase, uint16_t hwPrdr, uint16_t hwCmp);

/** 
  \brief  	   updata timer pwm duty cycle
  \param[in]   ptTmBase		pointer of timer register structure
  \param[in]   byDutyCycle	duty cycle(0 -> 100)
  \return none
 */
csi_error_t csi_bt_pwm_duty_cycle_updata(csp_bt_t *ptTmBase, uint8_t byDutyCycle); 

/** 
  \brief  	   updata timer pwm freq and duty cycle
  \param[in]   ptTmBase		pointer of timer register structure
  \param[in]   wfreq  	  	pwm frequency  
  \param[in]   byDutyCycle	pwm duty cycle(0 -> 100)
  \return 	   none
 */
csi_error_t csi_bt_pwm_updata(csp_bt_t *ptTmBase, uint32_t wfreq, uint8_t byDutyCycle); 

/** 
  \brief 	   timer sync evtrg config  
  \param[in]   ptTmBase		pointer of timer register structure
  \param[in]   eTrgin		timer evtrg input channel(0~1)
  \param[in]   eTrgMode 	timer evtrg mode, continuous/once
  \param[in]   bAutoRearm 	auto rearm, ENABLE/DISABLE(true/false)
  \return      error code \ref csi_error_t
*/
csi_error_t csi_timer_set_sync(csp_bt_t *ptTmBase,csi_timer_trgin_e eTrgin, csi_timer_trgmode_e eTrgMode, bool bAutoRearm);

/** 
  \brief 	   restart timer sync evtrg 
  \param[in]   ptTmBase		pointer of timer register structure
  \param[in]   eTrgin		timer evtrg input channel(0~1)
  \return      error code \ref csi_error_t
 */
csi_error_t csi_timer_rearm_sync(csp_bt_t *ptTmBase,csi_timer_trgin_e eTrgin);

/** 
  \brief 	   timer evtrg output config
  \param[in]   ptTmBase		pointer of timer register structure
  \param[in]   byEvtrg		trigger event (only 0)
  \param[in]   eTrgSrc 		timer evtrg source(1~3) 
  \param[in]   eTrgPrd 		event count period
  \return 	   error code \ref csi_error_t
 */
csi_error_t csi_timer_set_evtrg(csp_bt_t *ptTmBase, uint8_t byEvtrg, csi_timer_trgsrc_e eTrgSrc, uint8_t eTrgPrd);


/** \brief     start timer by sync event
  \param[in]   ptTmBase		pointer of timer register structure
  \param[in]   wTimeOut 	the timeout for timer, unit: us
  \return      error code \ref csi_error_t
 */ 
csi_error_t csi_timer_start_sync(csp_bt_t *ptTmBase, uint32_t wTimeOut);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_TIMER_H_ */
