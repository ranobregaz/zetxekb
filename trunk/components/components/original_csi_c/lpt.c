/***********************************************************************//** 
 * \file  lpt.c
 * \brief  csi lpt driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-28 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-8  <td>V0.1  <td>WNN   <td>modify
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
extern const uint16_t hwLptPscr[];
/* Public variablesr------------------------------------------------------*/
uint16_t g_hwLptPrd;
/* Private variablesr------------------------------------------------------*/


/** \brief lpt interrupt handle 
 * 
 *  \param[in] args: pointer of lpt dev
 *  \return none
 */
void apt_lpt_irqhandler(void *arg)
{
	csi_lpt_t *lpt = (csi_lpt_t *)arg;
    csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
	if (lpt->callback)
            lpt->callback(lpt, (void *)csp_lpt_get_misr(lpt_base));
	
	csp_lpt_clr_all_int(lpt_base);
	
}

/** \brief initialize lpt data structure
 * 
 *  \param[in] lpt: handle lpt handle to operate
 *  \param[in] port_idx: lpt index
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_lpt_init(csi_lpt_t *lpt, uint32_t idx)
{
    CSI_PARAM_CHK(lpt, CSI_ERROR);

    csi_error_t ret = CSI_OK;
	csp_lpt_t *lpt_base = NULL;
	
    if(0 == target_get(DEV_LPT_TAG, idx, &lpt->dev)) 	
	{
		lpt_base = (csp_lpt_t *)lpt->dev.reg_base;
		
		
		csi_clk_enable(&lpt->dev);
		csp_lpt_soft_rst(lpt_base);
		csp_lpt_clk_enable(lpt_base, ENABLE);
		
    }
	else
        ret = CSI_ERROR;

    return ret;
}
/** \brief de-initialize lpt interface
 * 
 *  \param[in] lpt: handle lpt handle to operate
 *  \return none
 */ 
void csi_lpt_uninit(csi_lpt_t *lpt)
{
    CSI_PARAM_CHK_NORETVAL(lpt);
    csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);

    csp_lpt_soft_rst(lpt_base);
}


/** \brief stop lpt
 * 
 *  \param[in] lpt: handle lpt handle to operate
 *  \return error code \ref csi_error_t
 */ 
void csi_lpt_stop(csi_lpt_t *lpt)
{
    CSI_PARAM_CHK_NORETVAL(lpt);

	csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
    csp_lpt_stop(lpt_base);
}
/** \brief   attach the callback handler to lpt
 * 
 *  \param[in] lpt: handle lpt handle to operate
 *  \param[in] callback: lpt interrupt callback function
 *  \param[in] arg: callback's param
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_lpt_attach_callback(csi_lpt_t *lpt, void *callback, void *arg)
{
    CSI_PARAM_CHK(lpt, CSI_ERROR);
	csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
    lpt->callback = callback;
    lpt->arg = arg;
	csp_lpt_int_enable(lpt_base, (lpt_int_e)(arg), ENABLE);
    csi_irq_attach((uint32_t)lpt->dev.irq_num, &apt_lpt_irqhandler, &lpt->dev);
    csi_irq_enable((uint32_t)lpt->dev.irq_num);

    return CSI_OK;
}
/** \brief detach lpt callback handler
 * 
 *  \param[in] lpt: handle lpt handle to operate
 *  \return none
 */ 
void csi_lpt_detach_callback(csi_lpt_t *lpt)
{
    CSI_PARAM_CHK_NORETVAL(lpt);

    lpt->callback = NULL;
    lpt->arg = NULL;
    csi_irq_disable((uint32_t)lpt->dev.irq_num);
    csi_irq_detach((uint32_t)lpt->dev.irq_num);
}
/** \brief get lpt remaining value
 * 
 *  \param[in] lpt: handle lpt handle to operate
 *  \return none
 */ 
uint32_t csi_lpt_get_remaining_value(csi_lpt_t *lpt)
{
    CSI_PARAM_CHK(lpt, 0U);
	
	uint16_t hwTimeVal = 0;
    csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
    hwTimeVal = csp_lpt_get_prdr(lpt_base) - csp_lpt_get_cnt(lpt_base);
	
	return hwTimeVal;
}
/** \brief get lpt load value
 * 
 *  \param[in] lpt: handle lpt handle to operate
 *  \return none
 */ 
uint32_t csi_lpt_get_load_value(csi_lpt_t *lpt)
{
    CSI_PARAM_CHK(lpt, 0U);

    csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
	return csp_lpt_get_prdr(lpt_base);
}
/** \brief get lpt load value
 * 
 *  \param[in] lpt: handle lpt handle to operate
 *  \return rue: running; false: stopped
 */ 
bool csi_lpt_is_running(csi_lpt_t *lpt)
{
	CSI_PARAM_CHK(lpt, false);

	csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	 
    return (csp_lpt_get_work_state(lpt_base) ? true : false);
}

/** \brief lpt pwm init 
 * 
 *  \param[in] lpt: handle lpt handle to operate
 *  \param[in] ePol: pwm out polarity 
 *  \param[in] eIdleSt: pwm out idle level 
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_lpt_pwm_init(csi_lpt_t *lpt, csi_lpt_startpol_t ePol, csi_lpt_idlepol_t eIdleSt)
{
    CSI_PARAM_CHK(lpt, CSI_ERROR);
    CSI_PARAM_CHK(timeout_us, CSI_ERROR);
    
	csi_error_t ret = CSI_OK;
    csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
	csp_lpt_set_opmd(lpt_base, LPT_OPM_CONTINUOUS);
	csp_lpt_stopshadow_enable(lpt_base, ENABLE);
	csp_lpt_set_pol(lpt_base, ePol);
	csp_lpt_set_idle_st(lpt_base, eIdleSt);
	//csp_lpt_out_enable(lpt_base, DISABLE);
	csp_lpt_out_enable(lpt_base, ENABLE);
	
	return ret;
}


/** \brief updata lpt pwm freq para: (prdr and cmp value)
 * 
 *  \param[in] lpt: handle lpt handle to operate
 *  \param[in] hwCmp: duty cycle
 *  \param[in] hwPrdr: period 
 *  \param[in] mode_updata: updata mode 
 *  \return none
 */
void csi_lpt_pwm_para_updata(csi_lpt_t *lpt, uint16_t hwCmp, uint16_t hwPrdr, csi_lpt_updata_t mode_updata)
{
	csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
	csp_lpt_data_update(lpt_base, hwCmp, hwPrdr);
	
	if(mode_updata == UPDATA_IM)
	{
		csp_lpt_set_prdr(lpt_base, hwPrdr);
		csp_lpt_set_cmp(lpt_base, hwCmp);
	}
}

/** \brief restart lpt sync 
 * 
 *  \param[in] lpt: LPT handle to operate
 *  \param bySync: sync select
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_rearm_sync(csi_lpt_t *lpt, uint8_t bySync)
{
	CSI_PARAM_CHK(adc, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
	if (csp_lpt_rearm_sync(lpt_base, bySync)<0 )
		return CSI_ERROR;
	
	return ret;
}
/** \brief lpt evtrg source output config  
 * 
 *  \param[in] lpt: LPT handle to operate
 *  \param[in] lpt_trgsrc: lpt evtrg source(1~4) 
 *  \param[in] trg_prd: event count period 
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_set_evtrg(csi_lpt_t *lpt, uint8_t byEvtrg, csi_lpt_trgsrc_t lpt_trgsrc, uint8_t trg_prd)
{
	CSI_PARAM_CHK(adc, CSI_ERROR);

    csi_error_t ret = CSI_OK;
    csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
	if (csp_lpt_set_evtrg(lpt_base, byEvtrg, lpt_trgsrc)<0)
		return CSI_ERROR;
	if (csp_lpt_set_trgprd(lpt_base, byEvtrg,trg_prd-1)<0)
		return CSI_ERROR;
	if (csp_lpt_trg_enable(lpt_base, byEvtrg, ENABLE) <0)
		return CSI_ERROR;
	
	return ret;
}


/** \brief lpt set frequency 
 * 
 *  \param[in] lpt: LPT handle to operate
 *  \param[in] eClk: lpt clock source selection
 *  \param[in] wHz: frequency
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_set_fre(csi_lpt_t *lpt, csi_lpt_clksrc_t eClk, uint16_t wHz)
{
	uint32_t wLptClk =0, wMult = 0, wDiv = 0;
	csi_error_t tRet = CSI_OK;
    csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
	switch (eClk)
	{
		case (LPT_CLK_PCLK_DIV4): wLptClk =  soc_get_pclk_freq()>> 2; 
			break;
		case (LPT_CLK_ISCLK): wLptClk = ISOSC_VALUE;
			break;
		case (LPT_CLK_IMCLK_DIV4):  
			switch (csp_get_imosc_fre(SYSCON_REG_BASE))
			{
				case (0): wLptClk = IMOSC_5M_VALUE>>2;
					break;
				case (1): wLptClk = IMOSC_4M_VALUE>>2;
					break;
				case (2): wLptClk = IMOSC_2M_VALUE>>2;
					break;
				case (3): wLptClk = IMOSC_131K_VALUE>>2;
					break;
				default:
					break;
			}
			break;
		case (LPT_CLK_EMCLK): wLptClk = EMOSC_VALUE;
			break;
		default: break;
	}
	
	wMult = wLptClk/wHz;
	if (wMult < 0x10000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV1);
		wDiv = 1;
	}
	else if (wMult < 0x20000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV2);
		wDiv = 2;
	}
	else if (wMult < 0x40000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV4);
		wDiv = 4;
	}
	else if (wMult < 0x80000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV8);
		wDiv = 8;
	}
	else if (wMult < 0x100000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV16);
		wDiv = 16;
	}
	else if (wMult < 0x200000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV32);
		wDiv = 32;
	}
	else if (wMult < 0x400000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV64);
		wDiv = 64;
	}
	else if (wMult < 0x800000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV128);
		wDiv = 128;
	}
	else if (wMult < 0x1000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV256);
		wDiv = 256;
	}
	else if (wMult < 0x2000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV512);
		wDiv = 512;
	}
	else if (wMult < 0x4000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV1024);
		wDiv = 1024;
	}
	else if (wMult < 0x8000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV2048);
		wDiv = 2048;
	}
	else if (wMult < 0x10000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV4096);
		wDiv = 4096;
	}
	else
		tRet = CSI_UNSUPPORTED;
	
	csp_lpt_set_prdr(lpt_base, wMult/wDiv);
	return tRet;
}

/** \brief start lpt pwm
 * 
 *  \param[in] lpt: handle lpt handle to operate
 *  \param eClk: clk source selection
 *  \param[in] freq: pwm frequency  in Hz
 *  \param[in] duty cycle: duty cycle(0 -> 100)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_pwm_start(csi_lpt_t *lpt, csi_lpt_clksrc_t eClk, uint32_t freq, uint32_t duty_cycle) 
{
	CSI_PARAM_CHK(lpt, CSI_ERROR);
	
	csi_error_t tRet = CSI_OK;
	csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
	uint32_t wLptClk =0, wMult = 0, wDiv = 0;
	volatile uint16_t hwCmp;	
	
	if(freq == 0 )
		return CSI_ERROR;
	
	switch (eClk)
	{
		case (LPT_CLK_PCLK_DIV4): wLptClk =  soc_get_pclk_freq()>> 2; 
			break;
		case (LPT_CLK_ISCLK): wLptClk = ISOSC_VALUE;
			break;
		case (LPT_CLK_IMCLK_DIV4):  
			switch (csp_get_imosc_fre(SYSCON_REG_BASE))
			{
				case (0): wLptClk = IMOSC_5M_VALUE>>2;
					break;
				case (1): wLptClk = IMOSC_4M_VALUE>>2;
					break;
				case (2): wLptClk = IMOSC_2M_VALUE>>2;
					break;
				case (3): wLptClk = IMOSC_131K_VALUE>>2;
					break;
				default:
					break;
			}
			break;
		case (LPT_CLK_EMCLK): wLptClk = EMOSC_VALUE;
			break;
		default: break;
	}
	
	wMult = wLptClk/freq;
	if (wMult < 0x10000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV1);
		wDiv = 1;
	}
	else if (wMult < 0x20000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV2);
		wDiv = 2;
	}
	else if (wMult < 0x40000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV4);
		wDiv = 4;
	}
	else if (wMult < 0x80000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV8);
		wDiv = 8;
	}
	else if (wMult < 0x100000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV16);
		wDiv = 16;
	}
	else if (wMult < 0x200000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV32);
		wDiv = 32;
	}
	else if (wMult < 0x400000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV64);
		wDiv = 64;
	}
	else if (wMult < 0x800000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV128);
		wDiv = 128;
	}
	else if (wMult < 0x1000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV256);
		wDiv = 256;
	}
	else if (wMult < 0x2000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV512);
		wDiv = 512;
	}
	else if (wMult < 0x4000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV1024);
		wDiv = 1024;
	}
	else if (wMult < 0x8000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV2048);
		wDiv = 2048;
	}
	else if (wMult < 0x10000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV4096);
		wDiv = 4096;
	}
	else
		tRet = CSI_UNSUPPORTED;
	
	
	g_hwLptPrd =  wMult/wDiv;
	csp_lpt_set_prdr(lpt_base, g_hwLptPrd);
	
	
	hwCmp = g_hwLptPrd * duty_cycle / 100;		//cmp value = period * (duty cycle) 
	
	csp_lpt_set_cmp(lpt_base, hwCmp);
	csp_lpt_sw_start(lpt_base);
	
	return tRet;
}

/** \brief start lpt pwm by external sync
 * 
 *  \param[in] lpt: handle lpt handle to operate
 *  \param eClk: clk source selection
 *  \param[in] freq: pwm frequency  in Hz
 *  \param[in] duty cycle: duty cycle(0 -> 100)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_pwm_start_sync(csi_lpt_t *lpt, csi_lpt_clksrc_t eClk, uint32_t freq, uint32_t duty_cycle) 
{
	CSI_PARAM_CHK(lpt, CSI_ERROR);
	
	csi_error_t tRet = CSI_OK;
	csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
	uint32_t wLptClk =0, wMult = 0, wDiv = 0;
	volatile uint16_t hwCmp;	
	
	if(freq == 0 )
		return CSI_ERROR;
	
	switch (eClk)
	{
		case (LPT_CLK_PCLK_DIV4): wLptClk =  soc_get_pclk_freq()>> 2; 
			break;
		case (LPT_CLK_ISCLK): wLptClk = ISOSC_VALUE;
			break;
		case (LPT_CLK_IMCLK_DIV4):  
			switch (csp_get_imosc_fre(SYSCON_REG_BASE))
			{
				case (0): wLptClk = IMOSC_5M_VALUE>>2;
					break;
				case (1): wLptClk = IMOSC_4M_VALUE>>2;
					break;
				case (2): wLptClk = IMOSC_2M_VALUE>>2;
					break;
				case (3): wLptClk = IMOSC_131K_VALUE>>2;
					break;
				default:
					break;
			}
			break;
		case (LPT_CLK_EMCLK): wLptClk = EMOSC_VALUE;
			break;
		default: break;
	}
	
	wMult = wLptClk/freq;
	if (wMult < 0x10000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV1);
		wDiv = 1;
	}
	else if (wMult < 0x20000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV2);
		wDiv = 2;
	}
	else if (wMult < 0x40000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV4);
		wDiv = 4;
	}
	else if (wMult < 0x80000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV8);
		wDiv = 8;
	}
	else if (wMult < 0x100000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV16);
		wDiv = 16;
	}
	else if (wMult < 0x200000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV32);
		wDiv = 32;
	}
	else if (wMult < 0x400000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV64);
		wDiv = 64;
	}
	else if (wMult < 0x800000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV128);
		wDiv = 128;
	}
	else if (wMult < 0x1000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV256);
		wDiv = 256;
	}
	else if (wMult < 0x2000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV512);
		wDiv = 512;
	}
	else if (wMult < 0x4000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV1024);
		wDiv = 1024;
	}
	else if (wMult < 0x8000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV2048);
		wDiv = 2048;
	}
	else if (wMult < 0x10000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV4096);
		wDiv = 4096;
	}
	else
		tRet = CSI_UNSUPPORTED;
	
	
	g_hwLptPrd =  wMult/wDiv;
	csp_lpt_set_prdr(lpt_base, g_hwLptPrd);
	
	
	hwCmp = g_hwLptPrd * duty_cycle / 100;		//cmp value = period * (duty cycle) 
	
	csp_lpt_prdr_ldmode(lpt_base, LPT_PRDLD_IM);
	csp_lpt_cmp_ldmode(lpt_base, LPT_CMPLD_IM);
	csp_lpt_set_prdr(lpt_base, wMult/wDiv);
	csp_lpt_set_cmp(lpt_base, hwCmp);
	
	csp_lpt_set_start_im_enable(lpt_base, ENABLE);
	
	return tRet;
}



/** \brief change lpt duty cycle
 * 
 *  \param lpt: LPT handle to operate
 *  \param duty_cycle: lpt clock source selection
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_change_duty(csi_lpt_t *lpt, uint32_t duty_cycle)
{
	uint16_t hwCmp;
	csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
	hwCmp = g_hwLptPrd*duty_cycle/100;
	csp_lpt_set_cmp(lpt_base, hwCmp);
	return CSI_OK;
}

/** \brief lpt work as a timer(Software start &restart)
 * 
 *  \param[in] lpt: LPT handle to operate
 *  \param[in] eClk: lpt clock source selection
 *  \param[in] wms: ms
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_start(csi_lpt_t *lpt, csi_lpt_clksrc_t eClk, uint32_t wms)
{
	uint32_t wLptClk =0, wMult = 0, wDiv = 0;
	csi_error_t tRet = CSI_OK;
    csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
	switch (eClk)
	{
		case (LPT_CLK_PCLK_DIV4): wLptClk =  soc_get_pclk_freq()>> 2; 
			break;
		case (LPT_CLK_ISCLK): wLptClk = ISOSC_VALUE;
			break;
		case (LPT_CLK_IMCLK_DIV4):  
			switch (csp_get_imosc_fre(SYSCON_REG_BASE))
			{
				case (0): wLptClk = IMOSC_5M_VALUE;
					break;
				case (1): wLptClk = IMOSC_4M_VALUE;
					break;
				case (2): wLptClk = IMOSC_2M_VALUE;
					break;
				case (3): wLptClk = IMOSC_131K_VALUE;
					break;
				default:
					break;
			}
			break;
		case (LPT_CLK_EMCLK): wLptClk = EMOSC_VALUE;
			break;
		default: break;
	}
	
	wMult = wLptClk/1000*wms;
	if (wMult < 0x10000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV1);
		wDiv = 1;
	}
	else if (wMult < 0x20000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV2);
		wDiv = 2;
	}
	else if (wMult < 0x40000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV4);
		wDiv = 4;
	}
	else if (wMult < 0x80000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV8);
		wDiv = 8;
	}
	else if (wMult < 0x100000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV16);
		wDiv = 16;
	}
	else if (wMult < 0x200000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV32);
		wDiv = 32;
	}
	else if (wMult < 0x400000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV64);
		wDiv = 64;
	}
	else if (wMult < 0x800000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV128);
		wDiv = 128;
	}
	else if (wMult < 0x1000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV256);
		wDiv = 256;
	}
	else if (wMult < 0x2000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV512);
		wDiv = 512;
	}
	else if (wMult < 0x4000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV1024);
		wDiv = 1024;
	}
	else if (wMult < 0x8000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV2048);
		wDiv = 2048;
	}
	else if (wMult < 0x10000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV4096);
		wDiv = 4096;
	}
	else
		tRet = CSI_UNSUPPORTED;
	
	g_hwLptPrd =  wMult/wDiv;
	csp_lpt_set_prdr(lpt_base, g_hwLptPrd);
	csp_lpt_sw_start(lpt_base);
	
	csp_lpt_set_start_im_enable(lpt_base, ENABLE);
	csp_lpt_swsync_enable(lpt_base, ENABLE);
	return tRet;
}

/** \brief lpt work as a timer(sync start)
 * 
 *  \param[in] lpt: LPT handle to operate
 *  \param[in] eClk: lpt clock source selection
 *  \param[in] wms: ms
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_lpt_start_sync(csi_lpt_t *lpt, csi_lpt_clksrc_t eClk, uint32_t wms)
{
	uint32_t wLptClk =0, wMult = 0, wDiv = 0;
	csi_error_t tRet = CSI_OK;
    csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
	switch (eClk)
	{
		case (LPT_CLK_PCLK_DIV4): wLptClk =  soc_get_pclk_freq()>> 2; 
			break;
		case (LPT_CLK_ISCLK): 
			csp_isosc_enable(SYSCON_REG_BASE);
			wLptClk = ISOSC_VALUE;
			break;
		case (LPT_CLK_IMCLK_DIV4):  
			switch (csp_get_imosc_fre(SYSCON_REG_BASE))
			{
				case (0): wLptClk = IMOSC_5M_VALUE;
					break;
				case (1): wLptClk = IMOSC_4M_VALUE;
					break;
				case (2): wLptClk = IMOSC_2M_VALUE;
					break;
				case (3): wLptClk = IMOSC_131K_VALUE;
					break;
				default:
					break;
			}
			break;
		case (LPT_CLK_EMCLK): wLptClk = EMOSC_VALUE;
			break;
		default: break;
	}
	
	wMult = wLptClk/1000*wms;
	if (wMult < 0x10000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV1);
		wDiv = 1;
	}
	else if (wMult < 0x20000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV2);
		wDiv = 2;
	}
	else if (wMult < 0x40000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV4);
		wDiv = 4;
	}
	else if (wMult < 0x80000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV8);
		wDiv = 8;
	}
	else if (wMult < 0x100000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV16);
		wDiv = 16;
	}
	else if (wMult < 0x200000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV32);
		wDiv = 32;
	}
	else if (wMult < 0x400000) {
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV64);
		wDiv = 64;
	}
	else if (wMult < 0x800000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV128);
		wDiv = 128;
	}
	else if (wMult < 0x1000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV256);
		wDiv = 256;
	}
	else if (wMult < 0x2000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV512);
		wDiv = 512;
	}
	else if (wMult < 0x4000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV1024);
		wDiv = 1024;
	}
	else if (wMult < 0x8000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV2048);
		wDiv = 2048;
	}
	else if (wMult < 0x10000000){
		csp_lpt_set_clk(lpt_base, eClk, LPT_PSC_DIV4096);
		wDiv = 4096;
	}
	else
		tRet = CSI_UNSUPPORTED;
	
	g_hwLptPrd =  wMult/wDiv;
	
	csp_lpt_prdr_ldmode(lpt_base, LPT_PRDLD_IM);
	csp_lpt_set_prdr(lpt_base, g_hwLptPrd);
	
	return tRet;
}

/** \brief lpt sync window config  
 * 
 *  \param[in] lpt: LPT handle to operate
 *  \param bCrossEnable: window cross enable/disable
 *  \param bInvEnable: window invert enable/disable
 *  \param hwOffsetRate: window start point from CNT=ZRO, in % (offset = hwOffset * g_hwLptPrd)
 *  \param hwWindowRate: window width, in % (width = hwWindow * g_hwLptPrd)
 *  \return error code \ref csi_error_t
 */
/*csi_error_t csi_lpt_set_sync_window(csi_lpt_t *lpt, bool bCrossEnable, bool bInvEnable, uint16_t hwOffsetRate, uint16_t hwWindowRate)
{
	uint16_t hwOffset, hwWindow;
	csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
	
	csp_lpt_sync_window_cross_enable(lpt_base, bCrossEnable);
	csp_lpt_sync_window_inv_enable(lpt_base, bInvEnable);
	
	hwOffset = g_hwLptPrd * hwOffsetRate/100;
	hwWindow = g_hwLptPrd * hwOffsetRate/100;
	
	if (hwOffsetRate + hwWindowRate > 100)
		csp_lpt_sync_window_cross_enable(lpt_base, ENABLE);
	else
		csp_lpt_sync_window_cross_enable(lpt_base, DISABLE);
		
	csp_lpt_set_sync_offset(lpt_base, hwOffset);
	csp_lpt_set_sync_window(lpt_base, hwWindow);
	
	csp_lpt_sync_window_enable(lpt_base, ENABLE);
	return CSI_OK;
}
*/
/** \brief lpt sync config  
 * 
 *  \param[in] lpt: LPT handle to operate
 *  \param bySync: select sync
 *  \param tSyncMode: LPT_TRG_CONT/LPT_TRG_ONCE
 *  \param bARearmEnable: auto rearm enable/disable
 *  \return csi_error_t
 */
csi_error_t csi_lpt_set_sync(csi_lpt_t *lpt, uint8_t bySync, csi_lpt_syncmode_t tSyncMode, bool bARearmEnable)
{
	csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	
	if (csp_lpt_set_sync_mode(lpt_base, bySync, tSyncMode) < 0)
		return CSI_ERROR;
	if (csp_lpt_auto_rearm_enable(lpt_base, bySync, bARearmEnable)<0)
		return CSI_ERROR;
	if (csp_lpt_sync_enable(lpt_base, bySync, ENABLE)<0)
		return CSI_ERROR;
	return CSI_OK;
}

/** \brief LPT software sync enable control
 * 
 *  \param lpt: LPT handle to operate
 *  \param bEnable: ENABLE/DISABLE
 *  \return void
 */
void csi_lpt_swsync_enable(csi_lpt_t *lpt, bool bEnable)
{
	csp_lpt_t *lpt_base = (csp_lpt_t *)HANDLE_REG_BASE(lpt);
	csp_lpt_swsync_enable(lpt_base, bEnable);
}