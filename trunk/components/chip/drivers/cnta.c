/***********************************************************************//** 
 * \file  cnta.c
 * \brief  csi cnta driver
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version   <th>Author  <th>Description
 * <tr><td> 2020-9-09 <td>V0.0  <td>ZJY     <td>initial
 * <tr><td> 2021-1-09 <td>V0.1  <td>ZJY     <td>modify
 * <tr><td> 2021-5-27 <td>V0.2  <td>LQ      <td>modify
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
/* Private macro------------------------------------------------------*/
/* externs function---------------------------------------------------*/
/* externs variablesr-------------------------------------------------*/
/* Private variablesr-------------------------------------------------*/


/** \brief initialize cnta data structure
 * 
 *  \param[in] ptCntaBase: handle cnta handle to operate
 *  \param[in] eClkDiv: the cnta clk_div
 *  \param[in] eRunMode: the cnta run mode
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_cnta_timer_init(csp_cnta_t *ptCntaBase,cnta_ckdiv_e eClkDiv,cnta_mode_e eRunMode)
{
	csi_clk_enable((uint32_t *)ptCntaBase);		//cnta clk enable
    csp_cnta_soft_rst(ptCntaBase);				//default init valu
	
	csp_cnta_set_ckdiv(ptCntaBase, eClkDiv, eRunMode);		//cnta clk = pclk/eClkDiv
	
	return CSI_OK;
}

/** \brief start cnta
 * 
 *  \param[in] ptCntaBase: handle cnta handle to operate
 *  \param[in] timeout_us: if cnta clk is 3M,the timeout for timer((0.333us * 1) ->(0.333us * 65535): 0.3333us -> 21.845ms)
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_cnta_timer_start(csp_cnta_t *ptCntaBase,uint32_t timeout_us)
{	
	csi_error_t ret = CSI_OK;

	uint8_t byDivTemp  = (0x01 << csp_cnta_get_ckdiv(ptCntaBase));
	uint32_t tmp_load  = (soc_get_pclk_freq() / (byDivTemp * 1000000)) * timeout_us - 1; // (timeout_us / 1000000) = (byDivTemp / pclk) * (tmp_load + 1)
	
	csp_cnta_set_datah(ptCntaBase, tmp_load);				//set data
	csp_cnta_set_int(ptCntaBase, CNTA_PENDH_INT, true);		//set intrrupt
	csi_irq_enable((uint32_t *)ptCntaBase);					//enable cnta irq
	csp_cnta_start(ptCntaBase);								//cnta start
	
	return ret;
}

/** \brief stop cnta
 * 
 *  \param[in] ptCntaBase: handle cnta handle to operate
 *  \return error code \ref csi_error_t
 */ 
void csi_cnta_stop(csp_cnta_t *ptCntaBase)
{	
    csp_cnta_stop(ptCntaBase);
}

/** \brief get cnta datah load value
 * 
 *  \param[in] ptCntaBase: handle cnta handle to operate
 *  \return none
 */ 
uint32_t csi_cnta_get_datah_value(csp_cnta_t *ptCntaBase)
{	
	return csp_cnta_get_datah(ptCntaBase);
}

/** \brief get cnta datal load value
 * 
 *  \param[in] ptCntaBase: handle cnta handle to operate
 *  \return none
 */ 
uint32_t csi_cnta_get_datal_value(csp_cnta_t *ptCntaBase)
{	
	return csp_cnta_get_datal(ptCntaBase);
}

/** \brief cnta pwm init 
 * 
 *  \param[in] ptCntaBase: handle cnta handle to operate
 *  \param[in] eClkDiv:the cnta clk_div
 *  \param[in] pwm_mode: pwm output mode, carrier/envelope
 *  \param[in] init_polar: pwm out initial polarity
 *  \param[in] stop_lev: pwm out stop level
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_cnta_pwm_init(csp_cnta_t *ptCntaBase,cnta_ckdiv_e eClkDiv, csi_cnta_pwmout_t pwm_mode ,csi_cnta_pwmlev_t init_polar, csi_cnta_pwmlev_t stop_lev)
{	
	csi_error_t ret = CSI_OK;
	
	csi_clk_enable((uint32_t *)ptCntaBase);		//cnta clk enable
    csp_cnta_soft_rst(ptCntaBase);				//default init valu
	
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
	
	csp_cnta_set_ckdiv(ptCntaBase, eClkDiv, CNTA_REPEAT_MODE);		//cnta clk = pclk/eClkDiv
	csp_cnta_set_carrier(ptCntaBase, CNTA_CARRIER_EN, pwm_mode, eRemStat, eOsp);
	
	return ret;
}

/** \brief start cnta pwm
 * 
 *  \param[in] ptCntaBase: handle cnta handle to operate
 *  \param[in] freq: pwm frequency  
 *  \param[in] duty_cycle: duty cycle(0 -> 100)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_cnta_pwm_start(csp_cnta_t *ptCntaBase, uint32_t freq, uint32_t duty_cycle) 
{		
	volatile uint32_t datah_load; 
	volatile uint32_t datal_load;
	uint8_t clkdiv;
	uint32_t period;
	
	if(freq == 0 || duty_cycle == 0 || duty_cycle == 100)
		return CSI_ERROR;
	
	clkdiv = csp_cnta_get_ckdiv(ptCntaBase);
	period = (soc_get_pclk_freq() / freq) >> clkdiv;
	datah_load = period * duty_cycle / 100 - 3;
	datal_load = period * (100 - duty_cycle) / 100 - 3;
	
	csp_cnta_set_datah(ptCntaBase, datah_load);
	csp_cnta_set_datal(ptCntaBase, datal_load);
	//csp_cnta_set_int(cnta_base, CNTA_PENDH_L, true);
	csp_cnta_start(ptCntaBase);
	
	return CSI_OK;
}

/** \brief updata cnta pwm freq para: (datah and datal value)
 * 
 *  \param[in] ptCntaBase: handle cnta handle to operate
 *  \param[in] datah: rem ouput high time long 
 *  \param[in] datal: rem ouput low time long  
 *  \param[in] sw_updata: cadata reg soft updata control
 *  \return none
 */
void csi_cnta_pwm_para_updata(csp_cnta_t *ptCntaBase, uint16_t datah, uint16_t datal, csi_cnta_sw_updata_t sw_updata)
{	
	if(sw_updata == CNTA_SW_EN)
	{
		csp_cnta_set_datah(ptCntaBase,datah);	
		csp_cnta_set_datal(ptCntaBase,datal);
	}
	else
	{
		csp_cnta_set_datah(ptCntaBase,datah);	
		csp_cnta_set_datal(ptCntaBase,datal);
		csp_cnta_soft_updata(ptCntaBase);
	}
		
}

/** \brief set cnta tc sync 
 * 
 *  \param[in] ptCntaBase: handle cnta handle to operate
 *  \param[in] tcpend_rem: carrieron control of tc pend end 
 *  \param[in] tcmatch_rem: carrieron control of tc pulse match 
 *  \param[in] hw_updata: cadata reg auto updata control
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_cnta_set_sync(csp_cnta_t *ptCntaBase, csi_cnta_tcpend_t tcpend_rem, csi_cnta_tcmatch_t tcmatch_rem,csi_cnta_hw_updata_t hw_updata)
{
	csp_cnta_set_sync(ptCntaBase, tcpend_rem, tcmatch_rem, hw_updata);	
	return CSI_OK;
}