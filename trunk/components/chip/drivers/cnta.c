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
 *  \param[in] ptContaTimerCfg:point of timer parameter config
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_cnta_timer_init(csp_cnta_t *ptCntaBase,csi_conta_timer_config_t *ptContaTimerCfg)
{
	uint8_t byDivTemp = 1;
	uint32_t wTempLoad = 1;
	byDivTemp  = ( 0x01 << (ptContaTimerCfg->eClkDiv) );
	wTempLoad  = (soc_get_pclk_freq() / (byDivTemp * 1000000)) * ptContaTimerCfg->wTime - 1; // (timeout_us / 1000000) = (byDivTemp / pclk) * (tmp_load + 1)
	
	csi_clk_enable((uint32_t *)ptCntaBase);		//cnta clk enable
    csp_cnta_soft_rst(ptCntaBase);				//default init valu
	csp_cnta_set_ckdiv(ptCntaBase, ptContaTimerCfg->eClkDiv,ptContaTimerCfg->eRunMode);	//cnta clk = pclk/eClkDiv
	
	csp_cnta_set_datah(ptCntaBase, wTempLoad);				//set data	
	csp_cnta_set_int(ptCntaBase, ptContaTimerCfg->byInter, true);//set intrrupt
	csi_irq_enable((uint32_t *)ptCntaBase);					//enable cnta irq
	
	return CSI_OK;
}

/** \brief start cnta
 * 
 *  \param[in] ptCntaBase: handle cnta handle to operate
 *  \return none
 */ 
void csi_cnta_start(csp_cnta_t *ptCntaBase)
{		
	csp_cnta_start(ptCntaBase);								//cnta start	
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
 *  \param[in] ptContaPwmCfg:point of pwm parameter config
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_cnta_pwm_init(csp_cnta_t *ptCntaBase,csi_conta_pwm_config_t *ptContaPwmCfg)
{	
	csi_error_t ret = CSI_OK;
	volatile uint32_t wDatahLoad; 
	volatile uint32_t wDatalLoad;
	uint32_t wPeriod;
	//uint8_t byClkDiv;
	cnta_osp_e eOsp = 0;
	cnta_remstat_e eRemStat = 0;	
	
	if(ptContaPwmCfg->wFreq == 0 || ptContaPwmCfg->byDutyCycle == 0 || ptContaPwmCfg->byDutyCycle == 100)
		return CSI_ERROR;
		
	csi_clk_enable((uint32_t *)ptCntaBase);		//cnta clk enable
    csp_cnta_soft_rst(ptCntaBase);				//default init valu
	
	//byClkDiv = ptContaPwmCfg->eClkDiv;
	wPeriod = (soc_get_pclk_freq() / ptContaPwmCfg->wFreq) >> ptContaPwmCfg->eClkDiv;
	wDatahLoad = wPeriod * ptContaPwmCfg->byDutyCycle / 100 - 3 + 1;//加1是误差调整
	wDatalLoad = wPeriod * (100 - ptContaPwmCfg->byDutyCycle) / 100 - 3 + 1;//加1是误差调整
	
	if(ptContaPwmCfg->byStartLevel == POLAR_LOW)			//initial polarity
		eOsp = CNTA_OSP_LOW;
	else if(ptContaPwmCfg->byStartLevel == POLAR_HIGH)
		eOsp = CNTA_OSP_HIGH;
		
	if(ptContaPwmCfg->byStopLevel == STOP_LOW)				//stop output level
		eRemStat = CNTA_REMSTAT_LOW;
	else if(ptContaPwmCfg->byStopLevel == STOP_HIGH)
		eRemStat = CNTA_REMSTAT_HIGH;
			
	csp_cnta_set_ckdiv(ptCntaBase, ptContaPwmCfg->eClkDiv, CNTA_REPEAT_MODE);		//cnta clk = pclk/eClkDiv
	csp_cnta_set_carrier(ptCntaBase, CNTA_CARRIER_EN, PWM_CARRIER, eRemStat, eOsp);
	csp_cnta_set_datah(ptCntaBase, wDatahLoad);
	csp_cnta_set_datal(ptCntaBase, wDatalLoad);
	csp_cnta_set_int(ptCntaBase, ptContaPwmCfg->byInter, true);
	
	return ret;
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