/***********************************************************************//** 
 * \file  wdt.c
 * \brief  window WDT and indepedent WDT driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-24 <td>V0.0 <td>WNN     <td>initial
 * <tr><td> 2021-5-13 <td>V0.0 <td>ZJY     <td>support INT
 * </table>
 * *********************************************************************
*/

#include "drv/iwdt.h"
#include "drv/irq.h"
#include "sys_clk.h"
#include "math.h"

/* Private macro------------------------------------------------------*/
/* externs function---------------------------------------------------*/
/* externs variablesr-------------------------------------------------*/
/* Private variablesr-------------------------------------------------*/
static uint16_t s_wIwdtTimeout	= 8200;

/** \brief Initialize WDT Interface. Initializes the resources needed for the WDT interface
 * 
 *  \param[in] ptIwdtBase: pointer of iwdt register structure
 *  \param[in] eOverTime: time length of system reset
 *  \return error code \ref csi_error_t
*/
csi_error_t csi_wdt_init(csp_iwdt_t *ptIwdtBase, csi_iwdt_tv_e eOverTime)
{
    CSI_PARAM_CHK(ptIwdtBase, CSI_ERROR);
    
   	uint8_t byOvt;
	
	if(!(csp_get_gcsr(SYSCON) & ISOSC))		//enable isosc
		csi_isosc_enable();		
	
	switch (eOverTime)						//set iwdt over time(time long of reset)
	{
		case IWDT_TV_128:	byOvt = 0x0;
			break;
		case IWDT_TV_256: 	byOvt = 0x1;
			break;
		case IWDT_TV_512: 	byOvt = 0x2;
			break;
		case IWDT_TV_1000: 	byOvt = 0x3;
			break;
		case IWDT_TV_2000:	byOvt = 0x4;
			break;
		case IWDT_TV_3100:	byOvt = 0x5;
			break;
		case IWDT_TV_4100: 	byOvt = 0x6;
			break;
		case IWDT_TV_8200:
		default:	byOvt = 0x7;
			break;
	}
	
	csp_iwdt_set_ovt(ptIwdtBase, byOvt);
	csp_iwdt_clr(ptIwdtBase);
	
    return CSI_OK;
}

/** \brief open(start) iwdt
 * 
 *  \param[in] ptTmBase: pointer of iwdt register structure
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_wdt_open(csp_iwdt_t *ptIwdtBase)
{
	CSI_PARAM_CHK(ptIwdtBase, CSI_ERROR);
	
	ptIwdtBase -> IWDEDR = EN_IWDT | IWDTE_KEY;
	while((ptIwdtBase->IWDCR & IWDT_ST) != IWDT_BUSY);
	ptIwdtBase -> IWDCNT = (ptIwdtBase -> IWDCNT & (~IWDT_CLR_MSK)) | IWDT_CLR << IWDT_CLR_POS;
	while((ptIwdtBase->IWDCNT & IWDT_CLR_BUSY) == 1);
	
	return CSI_OK;
}

/** \brief close(stop) iwdt
 * 
 *  \param[in] ptTmBase: pointer of iwdt register structure
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_wdt_close(csp_iwdt_t *ptIwdtBase)
{
	CSI_PARAM_CHK(ptIwdtBase, CSI_ERROR);
	
	csp_iwdt_disable(ptIwdtBase);
	
	return CSI_OK;
}

/** \brief close(stop) iwdt
 * 
 *  \param[in] ptTmBase: pointer of iwdt register structure
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_wdt_feed(csp_iwdt_t *ptIwdtBase)
{
	CSI_PARAM_CHK(ptIwdtBase, CSI_ERROR);

	csp_iwdt_clr(ptIwdtBase);
	
	return CSI_OK;
}

/** \brief iwdt INT enable/disable
 * 
 *  \param[in] ptTmBase: pointer of iwdt register structure
 *  \param[in] eIntTv: iwdt interrupt timer length(timer over), 1/2/3/4/5/6/7_8
 *  \param[in] bEnable: enable/disable INT
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_wdt_irq_enable(csp_iwdt_t *ptIwdtBase, csi_iwdt_intv_e eIntTv, bool bEnable)
{
	CSI_PARAM_CHK(ptIwdtBase, CSI_ERROR);

	csp_iwdt_set_intt(ptIwdtBase, eIntTv);					//iwdt interrupt timer, 1/2/3/4/5/6/7_8
	csp_syscon_int_enable(ptIwdtBase, IWDT_INT, bEnable);	//enable iwdt interrupt
	
	if(bEnable)
		csi_vic_enable_irq(SYSCON_IRQn);					//enable iwdt irq
	else
		csi_vic_disable_irq(SYSCON_IRQn);					//disable iwdt irq

	return CSI_OK;
}
/** \brief Attach the callback handler to wwdt
 *  \param[in] ptWdt: operate handle
 *  \param[in] callback: callback function
 *  \param[in] arg: callback's param
 *  \return error code \ref csi_error_t
*/
//csi_error_t csi_wdt_attach_callback(csi_wdt_t *ptWdt, void *callback, void *arg)
//{
//    CSI_PARAM_CHK(ptWdt, CSI_ERROR);
//	csp_wwdt_t *wdt_base = (csp_wwdt_t *)HANDLE_REG_BASE(ptWdt);
//	
//    ptWdt->callback = callback;
//    ptWdt->arg = arg;
//    csi_irq_attach((uint32_t)ptWdt->dev.irq_num, &wdt_irq_handler, &ptWdt->dev);
//    csi_irq_enable((uint32_t)ptWdt->dev.irq_num);
//	
//	if(HANDLE_DEV_IDX(ptWdt) == 1){				//wwdt int
//		
//		csp_wwdt_int_enable(wdt_base,true);	//enable wwdt int
//	}
//	else {
//		csp_syscon_int_enable((csp_syscon_t *)wdt_base, IWDT_INT, ENABLE);
//
//	}
//		
//	
//    return CSI_OK;
//}

/** \brief Detach the callback handler
 *  \param[in] ptWdt: operate handle
 *  \return error code \ref csi_error_t
*/
//void csi_wdt_detach_callback(csi_wdt_t *ptWdt)
//{
//	CSI_PARAM_CHK(ptWdt, CSI_ERROR);
//	csp_wwdt_t *wdt_base = (csp_wwdt_t *)HANDLE_REG_BASE(ptWdt);
//	
//   ptWdt->callback = NULL;
//   ptWdt->arg = NULL;
//   
//	csi_irq_disable((uint32_t)(ptWdt->dev.irq_num));
//   csi_irq_detach((uint32_t)(ptWdt->dev.irq_num));
//	
//	
//	if(HANDLE_DEV_IDX(ptWdt) == 1){				//wwdt int
//		
//		csp_wwdt_int_enable(wdt_base,false);	//disable wwdt int
//	}
//	else {
//		csp_syscon_int_enable((csp_syscon_t *)wdt_base, IWDT_INT, DISABLE);
//
//	}
//}

/** \brief Check if wdt is running
 * 
 *  \param[in] ptTmBase: pointer of iwdt register structure
 *  \return true->running, false->stopped
*/
bool csi_wdt_is_running(csp_iwdt_t *ptIwdtBase)
{
	//CSI_PARAM_CHK(ptIwdtBase, CSI_ERROR);
	return csp_iwdt_rd_st(ptIwdtBase);;
	
}

/** \brief Get the remaining time to timeout
 * 
 *  \param[in] ptIwdtBase: pointer of iwdt register structure
 *  \return the remaining time of wdt(ms)
*/
uint32_t csi_wdt_get_remaining_time(csp_iwdt_t *ptIwdtBase)
{
	CSI_PARAM_CHK(ptIwdtBase, CSI_ERROR);
	uint32_t wRTime, wCntMax = 0x3f;
	
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
	wRTime = csp_iwdt_get_cnt(ptIwdtBase) * s_wIwdtTimeout/wCntMax;

	return wRTime;
}

/** \brief enable or disable WDT when stop in debug mode
 * 
 *  \param[in] ptIwdtBase: pointer of iwdt register structure
 *  \param[in] bEnable: enable/disable 
 *  \return  none
*/
csi_error_t csi_wdt_debug_enable(csp_iwdt_t *ptIwdtBase, bool bEnable)
{
	CSI_PARAM_CHK(ptIwdtBase, CSI_ERROR);
	
	csp_iwdt_debug_enable(ptIwdtBase, bEnable);
	
	return CSI_OK;
	
}