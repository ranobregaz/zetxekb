/***********************************************************************//** 
 * \file  lpt.c
 * \brief  csi lpt driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-28 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-8  <td>V0.1  <td>WNN   <td>modify
 * <tr><td> 2021-5-20 <td>V0.2  <td>YYM   <td>modify
 * </table>
 * *********************************************************************
*/
#include <sys_clk.h>
#include <drv/lpt.h>
#include <drv/irq.h>
#include <drv/gpio.h>
#include <drv/pin.h>
#include <drv/porting.h>
#include <drv/tick.h>
#include "csp_syscon.h"
#include "csp_lpt.h"

/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

uint32_t wLptPrd = 0;

/** \brief get lpt clk
 * 
 *  \param[in] eClk: clk source selection
 *  \return lpt clk
 */ 
static uint32_t apt_get_lpt_clk(csi_lpt_clksrc_e eClk)
{
	uint32_t wLptClk =0;
	switch (eClk)
	{
		case (LPT_CLK_PCLK_DIV4): wLptClk =  soc_get_pclk_freq()>> 2; 
			break;
		case (LPT_CLK_ISCLK): wLptClk = ISOSC_VALUE;
			break;
		case (LPT_CLK_IMCLK_DIV4):  
			switch (csp_get_imosc_fre(SYSCON))
			{
				case (0): wLptClk = IMOSC_5M_VALUE >> 2;
					break;
				case (1): wLptClk = IMOSC_4M_VALUE >> 2;
					break;
				case (2): wLptClk = IMOSC_2M_VALUE >> 2;
					break;
				case (3): wLptClk = IMOSC_131K_VALUE >> 2;
					break;
				default:
					break;
			}
			break;
		case (LPT_CLK_EMCLK): wLptClk = EMOSC_VALUE;
			break;
		default: break;
	}	
	return wLptClk;
}

/** \brief get lpt prd
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \param[in] eClk: clk source selection
 *  \param[in] wMult_t: converted wMult value 
 *  \return lpt prd
 */ 
static uint32_t apt_get_lpt_prd(csp_lpt_t *ptLptBase,csi_lpt_clksrc_e eClk,uint32_t wMult_t)
{
	uint32_t hwLptPrd,wDiv = 0;
	lpt_pscdiv_e ePscDiv;
	
	if (wMult_t < 0x10000) {
		ePscDiv = LPT_PSC_DIV1;
	}
	else if (wMult_t < 0x20000) {
		ePscDiv = LPT_PSC_DIV2;
	}
	else if (wMult_t < 0x40000) {
		ePscDiv = LPT_PSC_DIV4;
	}
	else if (wMult_t < 0x80000){
		ePscDiv = LPT_PSC_DIV8;
	}
	else if (wMult_t < 0x100000) {
		ePscDiv = LPT_PSC_DIV16;
	}
	else if (wMult_t < 0x200000) {
		ePscDiv = LPT_PSC_DIV32;
	}
	else if (wMult_t < 0x400000) {
		ePscDiv = LPT_PSC_DIV64;
	}
	else if (wMult_t < 0x800000){
		ePscDiv = LPT_PSC_DIV128;
	}
	else if (wMult_t < 0x1000000){
		ePscDiv = LPT_PSC_DIV256;
	}
	else if (wMult_t < 0x2000000){
		ePscDiv = LPT_PSC_DIV512;
	}
	else if (wMult_t < 0x4000000){
		ePscDiv = LPT_PSC_DIV1024;
	}
	else if (wMult_t < 0x8000000){
		ePscDiv = LPT_PSC_DIV2048;
	}
	else if (wMult_t < 0x10000000){
		ePscDiv = LPT_PSC_DIV4096;
	}
	else
	{
		return ERR_LPT_CLK;	 		
	}
	wDiv = 1<<ePscDiv;
	csp_lpt_set_clk(ptLptBase, eClk, ePscDiv);
	hwLptPrd = wMult_t/wDiv;
	return hwLptPrd;	
}

/** \brief initialize lpt data structure
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \param[in] eClk: clk source selection
 *  \param[in] wTimeOut: the timeout for bt, unit: ms
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_lpt_timer_init(csp_lpt_t *ptLptBase,csi_lpt_clksrc_e eClk, uint32_t wTimeOut)
{
	CSI_PARAM_CHK(ptLptBase, CSI_ERROR);
	
	uint32_t wLptClk =0, wMult = 0;
	csi_error_t tRet = CSI_OK;
	
	csi_clk_enable((uint32_t *)ptLptBase);
	csp_lpt_soft_rst(ptLptBase);
	csp_lpt_clk_enable(ptLptBase, ENABLE);
	
	wLptClk = apt_get_lpt_clk(eClk);	
	wMult = wLptClk/1000*wTimeOut;
		
	wLptPrd = apt_get_lpt_prd(ptLptBase,eClk,wMult);
	if(wLptPrd == ERR_LPT_CLK)
	{
		tRet = CSI_UNSUPPORTED;
	}
	else
	{
		csp_lpt_set_prdr(ptLptBase, (uint16_t)wLptPrd);
	}
	csi_lpt_int_enable(ptLptBase,LPT_PEND_INT,ENABLE);	 //选用MATCH中断 
	csi_irq_enable((uint32_t*)ptLptBase);	
	return tRet;	
}

/**
  \brief       Enable lpt power manage
  \param[in]   ptLptBase:pointer of lpt register structure
  \param[in]   eLptInt:irq mode
  \param[in]   bEnable:lpt irq enable or disable
*/
void csi_lpt_int_enable(csp_lpt_t *ptLptBase, lpt_int_e eLptInt,bool bEnable)
{
	csp_lpt_int_enable(ptLptBase, eLptInt, bEnable);
}

/** \brief de-initialize lpt interface
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \return none
 */ 
void csi_lpt_uninit(csp_lpt_t *ptLptBase)
{
    CSI_PARAM_CHK_NORETVAL(ptLptBase);
    csp_lpt_soft_rst(ptLptBase);
}

/** \brief stop lpt
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \return error code \ref csi_error_t
 */ 
void csi_lpt_stop(csp_lpt_t *ptLptBase)
{
    CSI_PARAM_CHK_NORETVAL(ptLptBase);
	
    csp_lpt_stop(ptLptBase);
}

/** \brief get lpt remaining value
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \return none
 */ 
uint32_t csi_lpt_get_remaining_value(csp_lpt_t *ptLptBase)
{
    CSI_PARAM_CHK(ptLptBase, 0U);
	
	uint16_t hwTimeVal = 0;
	
    hwTimeVal = csp_lpt_get_prdr(ptLptBase) - csp_lpt_get_cnt(ptLptBase);
	
	return hwTimeVal;
}

/** \brief get lpt load value
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \return none
 */ 
uint32_t csi_lpt_get_load_value(csp_lpt_t *ptLptBase)
{
    CSI_PARAM_CHK(ptLptBase, 0U);

	return csp_lpt_get_prdr(ptLptBase);
}

/** \brief get lpt load value
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \return rue: running; false: stopped
 */ 
bool csi_lpt_is_running(csp_lpt_t *ptLptBase)
{
	CSI_PARAM_CHK(ptLptBase, false);

    return (csp_lpt_get_work_state(ptLptBase) ? true : false);
}

/** \brief updata lpt pwm freq para: (prdr and cmp value)
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \param[in] hwCmp: duty cycle
 *  \param[in] hwPrdr: period 
 *  \param[in] mode_updata: updata mode 
 *  \return none
 */
void csi_lpt_pwm_para_updata(csp_lpt_t *ptLptBase, uint16_t hwCmp, uint16_t hwPrdr, csi_lpt_updata_e mode_updata)
{
	
	csp_lpt_data_update(ptLptBase, hwCmp, hwPrdr);
	if(mode_updata == UPDATA_IM)
	{
		csp_lpt_set_prdr(ptLptBase, hwPrdr);
		csp_lpt_set_cmp(ptLptBase, hwCmp);
	}
}

/** \brief restart lpt sync 
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \param[in] bySync: sync select
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_rearm_sync(csp_lpt_t *ptLptBase, uint8_t bySync)
{
	CSI_PARAM_CHK(ptLptBase, CSI_ERROR);

    csi_error_t ret = CSI_OK;
	if(bySync > 0)
		return CSI_ERROR;
	csp_lpt_rearm_sync(ptLptBase);
	
	return ret;
}

/** \brief lpt evtrg source output config  
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \param[in] lpt_trgsrc: lpt evtrg source(1~4) 
 *  \param[in] trg_prd: event count period 
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_set_evtrg(csp_lpt_t *ptLptBase, uint8_t byEvtrg, csi_lpt_trgsrc_e lpt_trgsrc, uint8_t trg_prd)
{
	CSI_PARAM_CHK(ptLptBase, CSI_ERROR);

    csi_error_t ret = CSI_OK;
	if(byEvtrg > 0)
		return CSI_ERROR;
	
	csp_lpt_set_evtrg(ptLptBase, lpt_trgsrc);
	csp_lpt_set_trgprd(ptLptBase, trg_prd-1);
	csp_lpt_trg_enable(ptLptBase, ENABLE);
	
	return ret;
}

/** \brief lpt set frequency 
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \param[in] eClk: lpt clock source selection
 *  \param[in] wHz: frequency
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_set_fre(csp_lpt_t *ptLptBase, csi_lpt_clksrc_e eClk, uint16_t wHz)
{

	uint32_t wLptClk =0, wMult = 0;
	csi_error_t tRet = CSI_OK;
	
	wLptClk = apt_get_lpt_clk(eClk);
	wMult = wLptClk/wHz;
	
	wLptPrd = apt_get_lpt_prd(ptLptBase,eClk,wMult);	

	if(wLptPrd == ERR_LPT_CLK)
	{
		tRet = CSI_UNSUPPORTED;
	}	
	else
	{
		csp_lpt_set_prdr(ptLptBase, (uint16_t)wLptPrd);	
	}
		
	return tRet;
}

/** \brief lpt pwm init
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \param[in] ptLptPara: pointer of lpt parameter structure
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_pwm_init(csp_lpt_t *ptLptBase, csi_lpt_pwm_config_t *ptLptPara)
{
	CSI_PARAM_CHK(ptLptBase, CSI_ERROR);
	
	csi_error_t tRet = CSI_OK;
	uint32_t wLptClk = 0,wMult = 0,hwCmp = 0;
	
	csi_clk_enable((uint32_t *)ptLptBase);
	csp_lpt_soft_rst(ptLptBase);
	csp_lpt_clk_enable(ptLptBase, ENABLE);
	
	csp_lpt_set_opmd(ptLptBase, LPT_OPM_CONTINUOUS);
	csp_lpt_stopshadow_enable(ptLptBase, ENABLE);
	csp_lpt_set_pol(ptLptBase, ptLptPara->byStartpol);
	csp_lpt_set_idle_st(ptLptBase, ptLptPara->byIdlepol);
	csp_lpt_out_enable(ptLptBase, ENABLE);

	
	if(ptLptPara->wFreq== 0 )
		return CSI_ERROR;
		
	wLptClk = apt_get_lpt_clk(ptLptPara->byClksrc);
	

	wMult = wLptClk/ptLptPara->wFreq;
	
	wLptPrd = apt_get_lpt_prd(ptLptBase,ptLptPara->byClksrc,wMult);	
	
	if(wLptPrd == ERR_LPT_CLK)
	{
		tRet = CSI_UNSUPPORTED;
	}
	else
	{
		csp_lpt_set_prdr(ptLptBase, (uint16_t)wLptPrd);
		hwCmp = wLptPrd * ptLptPara->byCycle / 100;		//cmp value = period * (duty cycle) 
		csp_lpt_set_cmp(ptLptBase, (uint16_t)hwCmp);
		
		if(ptLptPara->byInter != LPT_NONE_INT)
		{
			csi_lpt_int_enable(ptLptBase,ptLptPara->byInter,ENABLE);	 //选用MATCH中断 
			csi_irq_enable((uint32_t*)ptLptBase);	
		}
	}
	return tRet;	
		
}

/** \brief start lpt pwm by external sync
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \param[in] eClk: clk source selection
 *  \param[in] freq: pwm frequency  in Hz
 *  \param[in] duty cycle: duty cycle(0 -> 100)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_pwm_start_sync(csp_lpt_t *ptLptBase, csi_lpt_clksrc_e eClk, uint32_t freq, uint32_t duty_cycle) 
{
	CSI_PARAM_CHK(ptLptBase, CSI_ERROR);
	
	csi_error_t tRet = CSI_OK;
	uint32_t wLptClk = 0,wMult = 0 ,hwCmp = 0;
	
	if(freq == 0 )
		return CSI_ERROR;
		
	wLptClk = apt_get_lpt_clk(eClk);
	wMult = wLptClk/freq;
	wLptPrd = apt_get_lpt_prd(ptLptBase,eClk,wMult);	
	if(wLptPrd == ERR_LPT_CLK)
	{
		tRet = CSI_UNSUPPORTED;
	}
	else
	{
		csp_lpt_set_prdr(ptLptBase, (uint16_t)wLptPrd);
		
		hwCmp = wLptPrd * duty_cycle / 100;		//cmp value = period * (duty cycle) 
		
		csp_lpt_prdr_ldmode(ptLptBase, LPT_PRDLD_IM);
		csp_lpt_cmp_ldmode(ptLptBase, LPT_CMPLD_IM);
		csp_lpt_set_prdr(ptLptBase, (uint16_t)wLptPrd);
		csp_lpt_set_cmp(ptLptBase, (uint16_t)hwCmp);
		
		csp_lpt_set_start_im_enable(ptLptBase, ENABLE);
	}
	return tRet;	

}

/** \brief change lpt duty cycle
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \param[in] duty_cycle: lpt clock source selection
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_change_duty(csp_lpt_t *ptLptBase, uint32_t duty_cycle)
{
	uint16_t hwCmp;
	
	hwCmp = wLptPrd*duty_cycle/100;
	csp_lpt_set_cmp(ptLptBase, hwCmp);
	return CSI_OK;
}

/** \brief lpt start
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_start(csp_lpt_t *ptLptBase)
{
	csp_lpt_sw_start(ptLptBase);
	csp_lpt_set_start_im_enable(ptLptBase, ENABLE);
	csp_lpt_swsync_enable(ptLptBase, ENABLE);
	return CSI_OK;
}

/** \brief lpt work as a timer(sync start)
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \param[in] eClk: lpt clock source selection
 *  \param[in] wms: ms
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_start_sync(csp_lpt_t *ptLptBase, csi_lpt_clksrc_e eClk, uint32_t wms)
{
	uint32_t wLptClk =0, wMult = 0;
	csi_error_t tRet = CSI_OK;
	
	wLptClk = apt_get_lpt_clk(eClk);
	wMult = wLptClk/1000*wms;

	wLptPrd = apt_get_lpt_prd(ptLptBase,eClk,wMult);	

	if(wLptPrd == ERR_LPT_CLK)
	{
		tRet = CSI_UNSUPPORTED;
	}
	else
	{	
		csp_lpt_prdr_ldmode(ptLptBase, LPT_PRDLD_IM);
		csp_lpt_set_prdr(ptLptBase, (uint16_t)wLptPrd);
	}
	
	return tRet;
}

/** \brief lpt sync config  
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \param[in] bySync: select sync
 *  \param[in] tSyncMode: LPT_TRG_CONT/LPT_TRG_ONCE
 *  \param[in] bARearmEnable: auto rearm enable/disable
 *  \return csi_error_t
 */
csi_error_t csi_lpt_set_sync(csp_lpt_t *ptLptBase, uint8_t bySync, csi_lpt_syncmode_e tSyncMode, bool bARearmEnable)
{
	if(bySync > 0)
		return CSI_ERROR;		
	csp_lpt_set_sync_mode(ptLptBase, tSyncMode);
	csp_lpt_auto_rearm_enable(ptLptBase, bARearmEnable);
	csp_lpt_sync_enable(ptLptBase, ENABLE);
	return CSI_OK;
}

/** \brief LPT software sync enable control
 * 
 *  \param[in] ptLptBase:pointer of lpt register structure
 *  \param[in] bEnable: ENABLE/DISABLE
 *  \return void
 */
void csi_lpt_swsync_enable(csp_lpt_t *ptLptBase, bool bEnable)
{
	csp_lpt_swsync_enable(ptLptBase, bEnable);
}