/***********************************************************************//** 
 * \file  timer.c
 * \brief  csi timer driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-09 <td>V0.0  <td>ZJY   <td>initial
 * </table>
 * *********************************************************************
*/
#include <sys_clk.h>
#include <drv/timer.h>
#include <drv/irq.h>
#include <drv/gpio.h>
#include <drv/pin.h>
#include <drv/porting.h>
#include <drv/tick.h>

#include "csp_bt.h"

/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/** \brief timer interrupt handle 
 * 
 *  \param[in] args: pointer of timer dev
 *  \return none
 */
void apt_timer_irqhandler(void *arg)
{
	csi_timer_t *timer = (csi_timer_t *)arg;
    csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
	
	if(csp_bt_get_isr(timer_base) & BT_PEND_INT) 
	{
		csp_bt_clr_isr(timer_base, BT_PEND_INT);
        if (timer->callback)
            timer->callback(timer, timer->arg);
    }
}
/** \brief initialize timer data structure
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \param[in] port_idx: timer index
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_timer_init(csi_timer_t *timer, uint32_t idx)
{
    CSI_PARAM_CHK(timer, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    csp_bt_t *timer_base = NULL;
	
    if(0 == target_get(DEV_BT_TAG, idx, &timer->dev)) 	
	{
		timer_base = (csp_bt_t *)timer->dev.reg_base;
       // memset((void *)timer_base, 0, sizeof(csp_bt_t));
		csi_clk_enable(&timer->dev);
		csp_bt_soft_rst(timer_base);
    }
	else
        ret = CSI_ERROR;

    return ret;
}
/** \brief de-initialize timer interface
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \return none
 */ 
void csi_timer_uninit(csi_timer_t *timer)
{
    CSI_PARAM_CHK_NORETVAL(timer);
    csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);

    memset((void *)timer_base, 0, sizeof(csp_bt_t));
}
/** \brief start timer
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \param[in] timeout_us: the timeout for timer((1us * 1) ->(1us * 65535): 1us ->65.535ms)
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_timer_start(csi_timer_t *timer, uint32_t timeout_us)
{
    CSI_PARAM_CHK(timer, CSI_ERROR);
    CSI_PARAM_CHK(timeout_us, CSI_ERROR);
    
	csi_error_t ret = CSI_OK;
    csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
	uint32_t tmp_load;
	
	timer->priv = (void*)(soc_get_pclk_freq()/100000 * timeout_us/600000);
	if((uint32_t)timer->priv == 0)
		timer->priv  = (void *)1;
	
	tmp_load = (soc_get_pclk_freq() / ((uint32_t)timer->priv * 20000)) * timeout_us / 50;
	csp_bt_set_timer(timer_base, BT_CLK_EN, BT_IMMEDIATE, BT_CONTINUOUS, BT_PCLKDIV, BT_CNTRLD_EN, (uint32_t)timer->priv - 1);		//timer clk = pclk/24
	csp_bt_set_prdr(timer_base,(uint16_t)tmp_load);
	csp_bt_set_cmp(timer_base,(uint16_t)tmp_load >> 1); 		//cmp = padr/2
	csp_bt_set_int(timer_base, BT_PEND_INT | BT_CMP_INT, true);
	
	csp_bt_start(timer_base);
	
	return ret;
}
/** \brief stop timer
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \return error code \ref csi_error_t
 */ 
void csi_timer_stop(csi_timer_t *timer)
{
    CSI_PARAM_CHK_NORETVAL(timer);

	csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
	
    csp_bt_stop(timer_base);
	mdelay(1);
}
/** \brief   attach the callback handler to timer
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \param[in] callback: timer interrupt callback function
 *  \param[in] arg: callback's param
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_timer_attach_callback(csi_timer_t *timer, void *callback, void *arg)
{
    CSI_PARAM_CHK(timer, CSI_ERROR);
	
	//csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
	//csi_timer_intsrc_t int_src = (csi_timer_intsrc_t)arg;
	
    timer->callback = callback;
    timer->arg = arg;
    csi_irq_attach((uint32_t)timer->dev.irq_num, &apt_timer_irqhandler, &timer->dev);
    csi_irq_enable((uint32_t)timer->dev.irq_num);
	//csp_bt_set_int(timer_base, int_src, true);
	
    return CSI_OK;
}
/** \brief detach timer callback handler
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \return none
 */ 
void csi_timer_detach_callback(csi_timer_t *timer)
{
    CSI_PARAM_CHK_NORETVAL(timer);

    timer->callback = NULL;
    timer->arg = NULL;
    csi_irq_disable((uint32_t)timer->dev.irq_num);
    csi_irq_detach((uint32_t)timer->dev.irq_num);
}

/** \brief get timer remaining value
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \return none
 */ 
uint32_t csi_timer_get_remaining_value(csi_timer_t *timer)
{
    CSI_PARAM_CHK(timer, 0U);
	
    csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
    uint32_t wTimeVal = csp_bt_get_prdr(timer_base) - csp_bt_get_cnt(timer_base);

	return wTimeVal;
}
/** \brief get timer load value
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \return none
 */ 
uint32_t csi_timer_get_load_value(csi_timer_t *timer)
{
    CSI_PARAM_CHK(timer, 0U);

    csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
	return csp_bt_get_prdr(timer_base);
}
/** \brief get timer load value
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \return rue: running; false: stopped
 */ 
bool csi_timer_is_running(csi_timer_t *timer)
{
	CSI_PARAM_CHK(timer, false);

	csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
	 
    return (csp_bt_get_work_state(timer_base) ? true : false);
}

/** \brief timer pwm init 
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \param[in] start_lev: pwm out start level = high
 *  \param[in] idle_lev: pwm out idle level = high
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_timer_pwm_init(csi_timer_t *timer, csi_timer_pwmlev_e start_lev, csi_timer_pwmlev_e idle_lev)
{
    CSI_PARAM_CHK(timer, CSI_ERROR);
    CSI_PARAM_CHK(timeout_us, CSI_ERROR);
    
	csi_error_t ret = CSI_OK;
    csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
	bt_idlest_e eIdleLev = 0;
	bt_startst_e eStartLev = 0;
	uint32_t tmp_load  = (soc_get_pclk_freq() * 100 / (24 * 1000000U));
	
	if(start_lev == T_START_LOW)
		eStartLev = BT_START_LOW;
	else if(start_lev == T_START_HIGH)
		eStartLev = BT_START_HIGH;
	
	if(idle_lev == T_IDLE_LOW)
		eIdleLev = BT_IDLE_LOW;
	else if(idle_lev == T_IDLE_HIGH)
		eIdleLev = BT_IDLE_HIGH;

	csp_bt_set_pwm(timer_base, eStartLev, eIdleLev, (tmp_load >> 1) ,tmp_load);
	
	return ret;
}
/** \brief start timer pwm
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \param[in] freq: pwm frequency  
 *  \param[in] duty cycle: duty cycle(0 -> 100)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_timer_pwm_start(csi_timer_t *timer, uint32_t freq, uint32_t duty_cycle) 
{
	CSI_PARAM_CHK(timer, CSI_ERROR);
	
	//csi_error_t ret = CSI_OK;
	csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
	
	uint32_t prdr_load;  	//= (soc_get_pclk_freq() / (24 * freq));
	uint32_t cmp_load; 		//= prdr_load * duty_cycle /100;
	
	if(freq == 0 || duty_cycle == 0 || duty_cycle == 100)
		return CSI_ERROR;
		
	timer->priv = (void*)(soc_get_pclk_freq()/freq/60000);
	if((uint32_t)timer->priv == 0)
		timer->priv  = (void *)1;
	
	prdr_load  = (soc_get_pclk_freq() / ((uint32_t)timer->priv * freq));
	cmp_load = prdr_load * duty_cycle /100;
	
	csp_bt_set_timer(timer_base, BT_CLK_EN, BT_SHDOW, BT_CONTINUOUS, BT_PCLKDIV, BT_CNTRLD_EN, (uint32_t)timer->priv -1);		//timer clk = pclk/24
	csp_bt_set_prdr(timer_base,prdr_load);
	csp_bt_set_cmp(timer_base, cmp_load);
	csp_bt_set_int(timer_base, BT_PEND_INT, true);
	csp_bt_start(timer_base);
	
	return CSI_OK;
}
/** \brief  updata timer pwm freq para: (para and cmp)
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \param[in] prdr_data:  timer pwm period 
 *  \param[in] cmp_data:  duty cycle
 *  \return none
 */
void csi_timer_pwm_para_updata(csi_timer_t *timer, uint16_t prdr_data, uint16_t cmp_data) 
{
	CSI_PARAM_CHK(timer, CSI_ERROR);
	csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
	
	csp_timer_data_update(timer_base, prdr_data, cmp_data);
}

/** \brief  updata timer pwm duty cycle
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \param[in] duty_cycle:  duty cycle(0 -> 100)
 *  \return none
 */
void csi_timer_pwm_duty_cycle_updata(csi_timer_t *timer, uint32_t duty_cycle) 
{
	CSI_PARAM_CHK(timer, CSI_ERROR);
	csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
	
	uint32_t cmp_load = csp_bt_get_prdr(timer_base) * duty_cycle /100;
	
	csp_bt_set_cmp(timer_base, cmp_load);
	//csp_bt_updata_en(timer_base);
}

/** \brief  updata timer pwm freq and duty cycle
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \param[in] freq:  pwm frequency  
 *  \param[in] duty_cycle:  duty cycle(0 -> 100)
 *  \return none
 */
void csi_timer_pwm_updata(csi_timer_t *timer, uint32_t freq, uint32_t duty_cycle) 
{
	CSI_PARAM_CHK(timer, CSI_ERROR);
	csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
	
	uint32_t prdr_load  = (soc_get_pclk_freq() / ((uint32_t)timer->priv * freq));
	uint32_t cmp_load = prdr_load * duty_cycle /100;
	
	csp_timer_data_update(timer_base, prdr_load, cmp_load);
}

/** \brief timer sync input evtrg config  
 * 
 *  \param[in] timer: TIMER handle to operate
 *  \param[in] timer_trgin: timer sync evtrg input channel(0~1)
 *  \param[in] trg_mode: timer evtrg mode, continuous/once
 *  \param[in] bAutoRearm: auto rearm, ENABLE/DISABLE(true/false)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_timer_set_sync(csi_timer_t *timer,csi_timer_trgin_t timer_trgin, csi_timer_trgmode_t trg_mode, bool bAutoRearm)
{
	CSI_PARAM_CHK(adc, CSI_ERROR); 

    csi_error_t ret = CSI_OK;
    csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
	
	csp_bt_set_sync(timer_base, BT_SYNC_EN, BT_SYNCMD_DIS, trg_mode, bAutoRearm,timer_trgin);
	
	return ret;
}
/** \brief rearm timer sync evtrg  
 * 
 *  \param[in] timer: TIMER handle to operate
 *  \param[in] timer_trgin: timer sync evtrg input channel(0~1)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_timer_rearm_sync(csi_timer_t *timer,csi_timer_trgin_t timer_trgin)
{
	CSI_PARAM_CHK(adc, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
	
	csp_bt_rearm_sync(timer_base, timer_trgin);
	
	return ret;
}
/** \brief timer evtrg output config
 * 
 *  \param[in] timer: BT handle to operate
 *  \param[in] byEvtrg: trigger event (byEvtrg =  0)
 *  \param[in] adc_trgsrc: BT evtrg source(1~3) 
 *  \param[in] trg_prd: event count period
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_timer_set_evtrg(csi_timer_t *timer, uint8_t byEvtrg, csi_timer_trgsrc_t timer_trgsrc, uint8_t trg_prd)
{
	CSI_PARAM_CHK(adc, CSI_ERROR);

    csi_error_t ret = CSI_OK;
	csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
	
	if(csp_bt_set_evtrg(timer_base, byEvtrg, timer_trgsrc) < 0)
		return CSI_ERROR;
	
	csp_bt_evtrg_en(timer_base);
	
	return ret;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/** \brief start timer by sync event
 * 
 *  \param[in] timer: handle timer handle to operate
 *  \param[in] timeout_us: the timeout for timer((1us * 1) ->(1us * 65535): 1us ->65.535ms)
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_timer_start_sync(csi_timer_t *timer, uint32_t timeout_us)
{
	CSI_PARAM_CHK(timer, CSI_ERROR);
    CSI_PARAM_CHK(timeout_us, CSI_ERROR);
    
	csi_error_t ret = CSI_OK;
    csp_bt_t *timer_base = (csp_bt_t *)HANDLE_REG_BASE(timer);
	uint32_t tmp_load;
	
	timer->priv = (void*)(soc_get_pclk_freq()/100000 * timeout_us/600000);
	if((uint32_t)timer->priv == 0)
		timer->priv  = (void *)1;
	
	tmp_load = (soc_get_pclk_freq() / ((uint32_t)timer->priv * 20000)) * timeout_us / 50 - 1;
	
	csp_bt_set_timer(timer_base, BT_CLK_EN, BT_SHDOW, BT_CONTINUOUS, BT_PCLKDIV, BT_CNTRLD_EN, (uint32_t)timer->priv -1);		//timer clk = pclk/24
	csp_bt_set_prdr(timer_base,(uint16_t)tmp_load);
	csp_bt_set_int(timer_base, BT_PEND_INT, true);
	
	return ret;
}