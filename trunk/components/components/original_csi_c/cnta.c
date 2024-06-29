/***********************************************************************//** 
 * \file  cnta.c
 * \brief  csi cnta driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-09 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-09 <td>V0.0  <td>ZJY   <td>modify
 * </table>
 * *********************************************************************
*/
#include <sys_clk.h>
#include <drv/cnta.h>
#include <drv/irq.h>
#include <drv/gpio.h>
#include <drv/pin.h>
#include <drv/porting.h>
#include <drv/tick.h>

#include "csp_cnta.h"

/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/** \brief cnta interrupt handle 
 * 
 *  \param[in] args: pointer of cnta dev
 *  \return none
 */
/** \brief cnta interrupt handle 
 * 
 *  \param[in] args: pointer of cnta dev
 *  \return none
 */
void apt_cnta_irqhandler(void *arg)
{
	csi_cnta_t *cnta = (csi_cnta_t *)arg;
   // csp_cnta_t *cnta_base = (csp_cnta_t *)HANDLE_REG_BASE(cnta);
	
	if (cnta->callback)
		cnta->callback(cnta, cnta->arg);
        
}
/** \brief initialize cnta data structure
 * 
 *  \param[in] cnta: handle cnta handle to operate
 *  \param[in] port_idx: cnta index
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_cnta_init(csi_cnta_t *cnta, uint32_t idx)
{
    CSI_PARAM_CHK(cnta, CSI_ERROR);
	
	csi_error_t ret = CSI_OK;
    csp_cnta_t *cnta_base = NULL;
	
	if(0 == target_get(DEV_CNTA_TAG, idx, &cnta->dev)) 
	{
		cnta_base = (csp_cnta_t *)cnta->dev.reg_base;
        //memset((void *)cnta_base, 0, sizeof(csp_cnta_t));
		csi_clk_enable(&cnta->dev);
		csp_cnta_def_init(cnta_base);
    }
	else
        ret = CSI_ERROR;

    return ret;
}

/** \brief de-initialize cnta interface
 * 
 *  \param[in] cnta: handle cnta handle to operate
 *  \return none
 */ 
void csi_cnta_uninit(csi_cnta_t *cnta)
{
    CSI_PARAM_CHK_NORETVAL(cnta);
    csp_cnta_t *cnta_base = (csp_cnta_t *)HANDLE_REG_BASE(cnta);

    memset((void *)cnta_base, 0, sizeof(csp_cnta_t));
}
/** \brief start cnta
 * 
 *  \param[in] cnta: handle cnta handle to operate
 *  \param[in] timeout_us: the timeout for timer((0.333us * 1) ->(0.333us * 65535): 0.3333us -> 21.845ms)
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_cnta_start(csi_cnta_t *cnta, uint32_t timeout_us)
{
    CSI_PARAM_CHK(cnta, CSI_ERROR);
    CSI_PARAM_CHK(timeout_us, CSI_ERROR);
    
	csi_error_t ret = CSI_OK;
    csp_cnta_t *cnta_base = (csp_cnta_t *)HANDLE_REG_BASE(cnta);

	uint32_t tmp_load  = (soc_get_pclk_freq() / (8 * 20000)) * timeout_us / 50 - 1;
	
	csp_cnta_init(cnta_base, CNTA_CK_DIV8, CNTA_REPEAT_MODE);		//cnta clk = pclk/8 = 24/8 = 3MHz
	csp_cnta_set_datah(cnta_base, tmp_load);
	csp_cnta_set_int(cnta_base, CNTA_PENDH_INT, true);
	
	csp_cnta_start(cnta_base);
	
	return ret;
}
/** \brief stop cnta
 * 
 *  \param[in] cnta: handle cnta handle to operate
 *  \return error code \ref csi_error_t
 */ 
void csi_cnta_stop(csi_cnta_t *cnta)
{
    CSI_PARAM_CHK_NORETVAL(cnta);

	csp_cnta_t *cnta_base = (csp_cnta_t *)HANDLE_REG_BASE(cnta);
	
    csp_cnta_stop(cnta_base);
}
/** \brief get cnta load value
 * 
 *  \param[in] cnta: handle cnta handle to operate
 *  \return none
 */ 
uint32_t csi_cnta_get_load_value(csi_cnta_t *cnta)
{
    CSI_PARAM_CHK(cnta, 0U);

    csp_cnta_t *cnta_base = (csp_cnta_t *)HANDLE_REG_BASE(cnta);
	
	return csp_cnta_get_datah(cnta_base);
}
/** \brief   attach the callback handler to cnta
 * 
 *  \param[in] cnta: handle cnta handle to operate
 *  \param[in] callback: cnta interrupt callback function
 *  \param[in] arg: callback's param
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_cnta_attach_callback(csi_cnta_t *cnta, void *callback, void *arg)
{
    CSI_PARAM_CHK(cnta, CSI_ERROR);

    cnta->callback = callback;
    cnta->arg = arg;
    csi_irq_attach((uint32_t)cnta->dev.irq_num, &apt_cnta_irqhandler, &cnta->dev);
    csi_irq_enable((uint32_t)cnta->dev.irq_num);

    return CSI_OK;
}
/** \brief detach cnta callback handler
 * 
 *  \param[in] cnta: handle cnta handle to operate
 *  \return none
 */ 
void csi_cnta_detach_callback(csi_cnta_t *cnta)
{
    CSI_PARAM_CHK_NORETVAL(cnta);

    cnta->callback = NULL;
    cnta->arg = NULL;
    csi_irq_disable((uint32_t)cnta->dev.irq_num);
    csi_irq_detach((uint32_t)cnta->dev.irq_num);
}

/** \brief cnta pwm init 
 * 
 *  \param[in] timer: handle cnta handle to operate
 *  \param[in] pwm_mode: pwm output mode, carrier/envelope
 *  \param[in] init_polar: pwm out initial polarity
 *  \param[in] stop_lev: pwm out stop level
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_cnta_pwm_init(csi_cnta_t *cnta, csi_cnta_pwmout_t pwm_mode ,csi_cnta_pwmlev_t init_polar, csi_cnta_pwmlev_t stop_lev)
{
	CSI_PARAM_CHK(cnta, CSI_ERROR);
    
    
	csi_error_t ret = CSI_OK;
    csp_cnta_t *cnta_base = (csp_cnta_t *)HANDLE_REG_BASE(cnta);
	
	cnta_osp_e eOsp = 0;
	cnta_remstat_e eRemStat = 0;
	
	if(init_polar == POLAR_LOW)				//initial polarity
		eOsp = CNTA_OSP_LOW;
	else if(init_polar == POLAR_HIGH)
		eOsp = CNTA_OSP_HIGH;
		
	//
	if(stop_lev == STOP_LOW)				//stop output level
		eRemStat = CNTA_REMSTAT_LOW;
	else if(stop_lev == STOP_HIGH)
		eRemStat = CNTA_REMSTAT_HIGH;
		
	csp_cnta_init(cnta_base,CNTA_CK_DIV8, CNTA_REPEAT_MODE);			//cnta clk = pclk/8 = 24/8 = 3MHz
	csp_cnta_set_carrier(cnta_base, CNTA_CARRIER_EN, pwm_mode, eRemStat, eOsp);
	
	return ret;
}
/** \brief start cnta pwm
 * 
 *  \param[in] cnta: handle cnta handle to operate
 *  \param[in] freq: pwm frequency  
 *  \param[in] duty_cycle: duty cycle(0 -> 100)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_cnta_pwm_start(csi_cnta_t *cnta, uint32_t freq, uint32_t duty_cycle) 
{
	CSI_PARAM_CHK(cnta, CSI_ERROR);
	
	csp_cnta_t *cnta_base = (csp_cnta_t *)HANDLE_REG_BASE(cnta);
	
	volatile uint32_t datah_load; 
	volatile uint32_t datal_load;
	uint8_t clkdiv;
	uint32_t period;
	
	if(freq == 0 || duty_cycle == 0 || duty_cycle == 100)
		return CSI_ERROR;
	
	clkdiv = csp_cnta_get_ckdiv(cnta_base);
	period = (soc_get_pclk_freq() / freq) >> clkdiv;
	datah_load = period * duty_cycle / 100 - 3;
	datal_load = period * (100 - duty_cycle) / 100 - 3;
	
	csp_cnta_set_datah(cnta_base, datah_load);
	csp_cnta_set_datal(cnta_base, datal_load);
	//csp_cnta_set_int(cnta_base, CNTA_PENDH_L, true);
	csp_cnta_start(cnta_base);
	
	return CSI_OK;
}
/** \brief updata cnta pwm freq para: (datah and datal value)
 * 
 *  \param[in] cnta: handle cnta handle to operate
 *  \param[in] datah: rem ouput high time long 
 *  \param[in] datal: rem ouput low time long  
 *  \param[in] sw_updata: cadata reg soft updata control
 *  \return none
 */
void csi_cnta_pwm_para_updata(csi_cnta_t *cnta, uint16_t datah, uint16_t datal, csi_cnta_sw_updata_t sw_updata)
{
	csp_cnta_t *cnta_base = (csp_cnta_t *)HANDLE_REG_BASE(cnta);
	
	if(sw_updata == CNTA_SW_EN)
		csp_cnta_cadata_update(cnta_base, datah, datal);
	else
		csp_cnta_set_cadata(cnta_base, datah, datal);
}
/** \brief set cnta tc sync 
 * 
 *  \param[in] cnta: handle cnta handle to operate
 *  \param[in] tcpend_rem: carrieron control of tc pend end 
 *  \param[in] tcmatch_rem: carrieron control of tc pulse match 
 *  \param[in] hw_updata: cadata reg auto updata control
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_cnta_set_sync(csi_cnta_t *cnta, csi_cnta_tcpend_t tcpend_rem, csi_cnta_tcmatch_t tcmatch_rem,csi_cnta_hw_updata_t hw_updata)
{
	CSI_PARAM_CHK(cnta, CSI_ERROR);
	csp_cnta_t *cnta_base = (csp_cnta_t *)HANDLE_REG_BASE(cnta);
	csp_cnta_set_sync(cnta_base, tcpend_rem, tcmatch_rem, hw_updata);
	
	return CSI_OK;
}