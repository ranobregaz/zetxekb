/***********************************************************************//** 
 * \file  wdt.c
 * \brief  window WDT and indepedent WDT driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-24 <td>V0.0  <td>WNN   <td>initial
 * <tr><td> 2021-1-2  <td>V0.1  <td>ZJY，WNN   <td>support INT
 * <tr><td> 2020-8-24 <td>V0.2  <td>WNN   <td> support dettatch_callback
 * </table>
 * *********************************************************************
*/

#include "drv/wdt.h"
#include "csp_wwdt.h"
#include "csp_syscon.h"
#include "drv/irq.h"
#include "sys_clk.h"
#include "math.h"

uint8_t g_WwdtCntMax = 0xff;
/**
  \brief       Initialize WDT Interface. Initializes the resources needed for the WDT interface
  \param[in]   wdt    wdt handle to operate
  \param[in]   idx    wdt index, 0 for IWDT, 1 for WWDT
  \return      error code \ref csi_error_t
*/
csi_error_t csi_wdt_init(csi_wdt_t *wdt, uint32_t idx)
{
    CSI_PARAM_CHK(wdt, CSI_ERROR);
    csi_error_t ret = CSI_OK;
   	
	wdt->priv = (void *)0xff;
    if (target_get(DEV_WDT_TAG, idx, &wdt->dev) != 0) {
		ret = CSI_ERROR;
    }
	
	if (HANDLE_DEV_IDX(wdt) == 0) {
		csp_syscon_t *ptIwdtBase = (csp_syscon_t *)HANDLE_REG_BASE(wdt);
		csp_isosc_enable(ptIwdtBase);
		//csp_iwdt_reg_reset(ptIwdtBase);
	}
	else {
		csi_clk_enable(&(wdt->dev));
	}
	
    return ret;
}

/**
  \brief       De-initialize WDT Interface. stops operation and releases the software resources used by the interface
  \param[in]   wdt    handle to operate
  \return      None
*/
void csi_wdt_uninit(csi_wdt_t *wdt)
{
  csp_wwdt_t *wdt_base = (csp_wwdt_t *)HANDLE_REG_BASE(wdt);
	
	if(HANDLE_DEV_IDX(wdt) == 1){				//wwdt int
		
		csp_wwdt_int_enable(wdt_base,false);	//disable wwdt int
	}
	else {
		csp_syscon_int_enable((csp_syscon_t *)wdt_base, IWDT_INT, DISABLE);

	}
	csi_irq_disable((uint32_t)(wdt->dev.irq_num));
    csi_irq_detach((uint32_t)(wdt->dev.irq_num));
}


void wdt_irq_handler(void *arg)
{
	csi_wdt_t *ptWdt = (csi_wdt_t *)arg;
	csp_wwdt_t *wdt_base = (csp_wwdt_t *)HANDLE_REG_BASE(ptWdt);
	
	if (ptWdt->callback)
		ptWdt->callback(ptWdt, ptWdt->arg);
		
	if(HANDLE_DEV_IDX(ptWdt) == 1)			//wwdt int
		csp_wwdt_clr_isr(wdt_base);		//clear wwdt int
	else
		csp_syscon_int_clr((csp_syscon_t *)wdt_base, IWDT_INT);
}

/**
  \brief       Attach the callback handler to wwdt
  \param[in]   ptWdt         operate handle
  \param[in]   callback    callback function
  \param[in]   arg         callback's param
  \return      error code \ref csi_error_t
*/

csi_error_t csi_wdt_attach_callback(csi_wdt_t *ptWdt, void *callback, void *arg)
{
    CSI_PARAM_CHK(ptWdt, CSI_ERROR);
	csp_wwdt_t *wdt_base = (csp_wwdt_t *)HANDLE_REG_BASE(ptWdt);
	
    ptWdt->callback = callback;
    ptWdt->arg = arg;
    csi_irq_attach((uint32_t)ptWdt->dev.irq_num, &wdt_irq_handler, &ptWdt->dev);
    csi_irq_enable((uint32_t)ptWdt->dev.irq_num);
	
	if(HANDLE_DEV_IDX(ptWdt) == 1){				//wwdt int
		
		csp_wwdt_int_enable(wdt_base,true);	//enable wwdt int
	}
	else {
		csp_syscon_int_enable((csp_syscon_t *)wdt_base, IWDT_INT, ENABLE);

	}
		
	
    return CSI_OK;
}

/**
  \brief       Detach the callback handler
  \param[in]   ptWdt         operate handle
  \return      error code \ref csi_error_t
*/
void csi_wdt_detach_callback(csi_wdt_t *ptWdt)
{
	CSI_PARAM_CHK(ptWdt, CSI_ERROR);
	csp_wwdt_t *wdt_base = (csp_wwdt_t *)HANDLE_REG_BASE(ptWdt);
	
   ptWdt->callback = NULL;
   ptWdt->arg = NULL;
   
	csi_irq_disable((uint32_t)(ptWdt->dev.irq_num));
   csi_irq_detach((uint32_t)(ptWdt->dev.irq_num));
	
	
	if(HANDLE_DEV_IDX(ptWdt) == 1){				//wwdt int
		
		csp_wwdt_int_enable(wdt_base,false);	//disable wwdt int
	}
	else {
		csp_syscon_int_enable((csp_syscon_t *)wdt_base, IWDT_INT, DISABLE);

	}
}
static uint32_t g_wIwdtTimeout = 8200;
static uint32_t g_wWwdtTimeout = 0; 
/**
  \brief       Set the WDT value
  \param[in]   wdt    handle to operate
  \param[in]   ms     the timeout value(ms)
  \return      error code \ref csi_error_t
*/
csi_error_t csi_wdt_set_timeout(csi_wdt_t *wdt, uint32_t ms)
{
    CSI_PARAM_CHK(ptWdt, CSI_ERROR);
	csi_error_t ret = CSI_OK;
	uint8_t byOvt;
	
	///IWDT: idx = 0
	if (HANDLE_DEV_IDX(wdt) == 0)
	{
		g_wIwdtTimeout = ms;	
		csp_syscon_t *ptIwdtBase = (csp_syscon_t *)HANDLE_REG_BASE(wdt);
		switch (ms)
		{
			case (128): byOvt = 0x0;
				break;
			case (256): byOvt = 0x1;
				break;
			case (512): byOvt = 0x2;
				break;
			case (1024): byOvt = 0x3;
				break;
			case (2048): byOvt = 0x4;
				break;
			case (3072): byOvt = 0x5;
				break;
			case (4096): byOvt = 0x6;
				break;
			case (8192):
			default: byOvt = 0x7;
				break;
		}
		csp_iwdt_set_ovt(ptIwdtBase, byOvt);
		csp_iwdt_clr(ptIwdtBase);
	}
	///WWDT
	else
	{
		g_wWwdtTimeout = ms;
		csp_wwdt_t *ptWwdtBase = (csp_wwdt_t *)HANDLE_REG_BASE(wdt);
		uint32_t temp = (soc_get_apb_freq(0U) >> 12) * ms/1000;
		uint8_t byPdiv = 3;
		uint8_t byCnt = 0xff;
		
		if (temp < 0x80) {
			byPdiv = 0;
		}
		else if (temp >> 1 < 0x80) {
			byPdiv = 1;
		}
		else if (temp >> 2 < 0x80) {
			byPdiv = 2;
		}
		else if (temp >> 3 < 0x80) {
			byPdiv = 3;
		}
		else {
			ret = CSI_ERROR;
		}
		byCnt = temp/(1<<byPdiv)+0x80;	
		
		g_WwdtCntMax = byCnt;
		csp_wwdt_set_psc(ptWwdtBase, byPdiv);
		csp_wwdt_set_cnt(ptWwdtBase, byCnt);
	}
	return ret;
}

/**
  \brief       Set window time for WWDT
  \param[in]   wdt    handle to operate
  \param[in]   ms     the timeout value(ms)
  \return      error code \ref csi_error_t
*/
csi_error_t csi_wdt_set_window_time(csi_wdt_t *wdt, uint32_t ms)
{
	CSI_PARAM_CHK(ptWdt, CSI_ERROR);
	csi_error_t ret = CSI_OK;
	
	if (HANDLE_DEV_IDX(wdt) == 0)
	{
		ret = CSI_UNSUPPORTED;
		return ret;
	}
	else{
		csp_wwdt_t *ptWwdtBase = (csp_wwdt_t *)HANDLE_REG_BASE(wdt);
		uint8_t pdiv = csp_wwdt_get_psc(ptWwdtBase);
		uint32_t wWnd = (soc_get_apb_freq(0U) >> 12) * ms/1000/(1<<pdiv);
		if (wWnd > (csp_wwdt_get_cnt(ptWwdtBase) -0x80))
			return CSI_ERROR;
		wWnd = wWnd + 0x80;
		csp_wwdt_set_wnd(ptWwdtBase, (uint8_t)(wWnd));
	}
	return ret;
}

/**
  \brief       Set IWDT INT time
  \param[in]   wdt    handle to operate
  \param[in]   ms     the int time value: IWDT_INT_1/2/3/4/5/6/7_8
  \return      error code \ref csi_error_t
*/   
csi_error_t csi_wdt_set_int_time(csi_wdt_t *wdt, ciwdt_intv_e eVal)
{
	CSI_PARAM_CHK(wdt, CSI_ERROR);
	csi_error_t ret = CSI_OK;
	
	if (HANDLE_DEV_IDX(wdt) == 0)
	{
		csp_syscon_t *ptIwdtBase = (csp_syscon_t *)HANDLE_REG_BASE(wdt);
		csp_iwdt_set_intt(ptIwdtBase, eVal);
	}
	else
		ret = CSI_UNSUPPORTED;
	return ret;
}

/**
  \brief       Feed the WDT
  \param[in]   wdt    handle to operate
  \return      error code \ref csi_error_t
*/
csi_error_t csi_wdt_feed(csi_wdt_t *wdt)
{
	CSI_PARAM_CHK(ptWdt, CSI_ERROR);
	csi_error_t ret = CSI_OK;
	
	if (HANDLE_DEV_IDX(wdt) == 0) {
		csp_syscon_t *ptIwdtBase = (csp_syscon_t *)HANDLE_REG_BASE(wdt);
		csp_iwdt_clr(ptIwdtBase);
	}
	else  {
		csp_wwdt_t *ptWwdtBase = (csp_wwdt_t *)HANDLE_REG_BASE(wdt);
		csp_wwdt_set_cnt(ptWwdtBase, g_WwdtCntMax);
	}
	
	return ret;
}

/**
  \brief       Start the WDT
  \param[in]   wdt    handle to operate
  \return      error code \ref csi_error_t
*/
csi_error_t csi_wdt_start(csi_wdt_t *wdt)
{
	CSI_PARAM_CHK(ptWdt, CSI_ERROR);
	csi_error_t ret = CSI_OK;
	
	if (HANDLE_DEV_IDX(wdt) == 0) {
		csp_syscon_t *ptIwdtBase = (csp_syscon_t *)HANDLE_REG_BASE(wdt);
		csp_iwdt_enable(ptIwdtBase);
	}
	else {
		csp_wwdt_t *ptWwdtBase = (csp_wwdt_t *)HANDLE_REG_BASE(wdt);
		csp_wwdt_enable(ptWwdtBase);
	}	
	return ret;
}

/**
  \brief       Stop the WDT
  \param[in]   wdt    handle to operate
  \return      None
*/
void csi_wdt_stop(csi_wdt_t *wdt)
{
	CSI_PARAM_CHK(ptWdt, CSI_ERROR);
	
	if (HANDLE_DEV_IDX(wdt) == 0) {
		csp_syscon_t *ptIwdtBase = (csp_syscon_t *)HANDLE_REG_BASE(wdt);
		csp_iwdt_disable(ptIwdtBase);
	}
	else
	{
		/// can not stop WWDT once it is enabled
	}	

}

/**
  \brief       Check if wdt is running
  \param[in]   wdt    handle wdt handle to operate
  \return      true->running, false->stopped
*/
bool csi_wdt_is_running(csi_wdt_t *wdt)
{
	CSI_PARAM_CHK(ptWdt, CSI_ERROR);
	csi_error_t tRet = CSI_OK;
	
	if (HANDLE_DEV_IDX(wdt) == 0) {	
		csp_syscon_t *ptIwdtBase = (csp_syscon_t *)HANDLE_REG_BASE(wdt);
		return (csp_iwdt_rd_st(ptIwdtBase));
	}
	else{
		csp_wwdt_t *ptWwdtBase = (csp_wwdt_t *)HANDLE_REG_BASE(wdt);
		return (csp_wwdt_status_get(ptWwdtBase));
	}
	return tRet;
}

/**
  \brief       Get the remaining time to timeout
  \param[in]   wdt    handle to operate
  \return      the remaining time of wdt(ms)
*/
uint32_t csi_wdt_get_remaining_time(csi_wdt_t *wdt)
{
	CSI_PARAM_CHK(ptWdt, CSI_ERROR);
	uint32_t wRTime, wCntMax = 0x3f;
	
	if (HANDLE_DEV_IDX(wdt) == 0) {	
		csp_syscon_t *ptIwdtBase = (csp_syscon_t *)HANDLE_REG_BASE(wdt);
		switch ((ptIwdtBase->IWDCR & IWDT_OVT_MSK) >> IWDT_OVT_POS)
		{
			case (0): wCntMax = 0x3f;
				break;
			case (1): wCntMax = 0x7f;
				break;
			case (2): wCntMax = 0xff;
				break;
			case (3): wCntMax = 0x1ff;
				break;
			case (4): wCntMax = 0x3ff;
				break;
			case (5): wCntMax = 0x60c;
				break;
			case (6): wCntMax = 0x7ff;
				break;
			case (7): wCntMax = 0xfff;
				break;
			default :
				break;
		}
		wRTime = csp_iwdt_get_cnt(ptIwdtBase) * g_wIwdtTimeout/wCntMax;
	}
	else{
		csp_wwdt_t *ptWwdtBase = (csp_wwdt_t *)HANDLE_REG_BASE(wdt);
		wRTime = (csp_wwdt_get_cnt(ptWwdtBase) - 0x80) * g_wWwdtTimeout/(g_WwdtCntMax-0x80) ;
	}
	return (wRTime);
}

/**
  \brief      enable or disable WDT when stop in debug mode
  \param[in]   wdt    wdt handle to operate
  \param	   bEnable 
  \return      none
*/
void csi_wdt_debug_enable(csi_wdt_t *wdt, bool bEnable)
{
	CSI_PARAM_CHK(ptWdt, CSI_ERROR);

	
	if (HANDLE_DEV_IDX(wdt) == 0){
		csp_syscon_t *ptIwdtBase = (csp_syscon_t *)HANDLE_REG_BASE(wdt);
		csp_iwdt_debug_enable(ptIwdtBase, bEnable);
	}
	else{
		csp_wwdt_t *ptWwdtBase = (csp_wwdt_t *)HANDLE_REG_BASE(wdt);
		csp_wwdt_debug_enable(ptWwdtBase, bEnable);
	}
}