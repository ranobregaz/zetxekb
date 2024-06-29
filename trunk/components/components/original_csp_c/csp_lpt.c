/***********************************************************************//** 
 * \file  csp_lpt.c
 * \brief LPT(low power timer) description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-08 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-8  <td>V0.1  <td>WNN   <td>modify
 * </table>
 * *********************************************************************
*/

/* Includes ---------------------------------------------------------------*/
#include <sys_clk.h>
#include "csp_lpt.h"
#include "csp_common.h"
#include "csp_syscon.h"

/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
const uint16_t hwLptPscr[] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096};


/** \brief get lpt running status
 * 
 *  \param[in] ptBtBase: pointer of BT reg structure
 *  \return none
 */ 
uint8_t csp_lpt_get_work_state(csp_lpt_t *ptLptBase)
{
	if(ptLptBase->RSSR & LPT_START)
		return ENABLE;
	else
		return DISABLE;
		
}
/** \brief lpt interrupt config
 * 
 *  \param[in] ptLptBase: pointer of ptLptBase reg structure
 *  \param[in] eLptInt: int num
 *  \param[in] bEnable: enable,disable
 *  \return none
 */ 
void csp_lpt_int_enable(csp_lpt_t *ptLptBase, lpt_int_e eLptInt,bool bEnable)
{
	if(bEnable)
		ptLptBase->IMCR |= 0x1 << eLptInt; 
	else
		ptLptBase->IMCR &= ~(0x1 <<eLptInt); 
}


/** \brief lpt evtrg source output config
 * 
 *  \param[in] ptLptBase: pointer of ptLptBase reg structure
 *  \param[in] eTrgSrc: lpt evtrg source(1~4)
 *  \return csp_error_t
 */
csp_error_t csp_lpt_set_evtrg(csp_lpt_t *ptLptBase, uint8_t byEvtrg, lpt_trgsrc_e eTrgSrc)
{
	if (byEvtrg > 0)
		return CSP_FAIL;
	ptLptBase->EVTRG = (ptLptBase->EVTRG & ~LPT_TRGSRC_MSK) |(eTrgSrc << LPT_TRGSRC_POS);
	return CSP_SUCCESS;
}
/** \brief updata lpt prdr value
 * 
 *  \param[in] ptLptBase: pointer of ptLptBase reg structure
 *  \param[in] hwPrdr: prdr reg val
 *  \param[in] hwCmp: cmp reg val
 *  \return none
 */
void csp_lpt_data_update(csp_lpt_t *ptLptBase,uint16_t hwPrdr, uint16_t hwCmp)
{
	ptLptBase->PRDR = hwPrdr;
	ptLptBase->CMP = hwCmp;		
}

/** \brief lpt timer mode config
 * 
 *  \param[in] ptLptBase: pointer of ptLptBase reg structure
 *  \param[in] eClkCtrl: lpt clk control,enable/disable
 *  \param[in] eCssSel: lpt clk source select
 *  \param[in] eShdwSel: start control shadow mode select
 *  \param[in] eOpm: lpt count mode select 
 *  \param[in] wFltckPrs: lpt Digital filtering Frequency division 
 *  \param[in] ePcsr: lpt clk Frequency division 
 *  \return none
 */
void csp_lpt_set_timer(csp_lpt_t *ptLptBase, bool eClkCtrl, lpt_css_e eCssSel, bool eShdwSel, 
					lpt_opm_e eOpm, lpt_pscdiv_e ePscDiv, uint32_t wFltckPrs)
{
	if(eCssSel == LPT_ISCLK || eCssSel == LPT_IMCLK_DIV4 || eCssSel == LPT_EMCLK)
	{
		csp_syscon_t *sys_base = (csp_syscon_t *)APB_SYS_BASE;
		switch(eCssSel)
		{
			case LPT_ISCLK:
				if(!(csp_get_ckst(sys_base) & ISOSC))		// ISOSC 
					csp_isosc_enable(sys_base);
				break;
			case LPT_IMCLK_DIV4:
				if(!(csp_get_ckst(sys_base) & IMOSC))
					csp_imosc_enable(sys_base, 0x00);		//IMOSC
				break;
			case LPT_EMCLK:
				if(!(csp_get_ckst(sys_base) & EMOSC))
					csp_emosc_enable(sys_base, 12000000);	//EMOSC
				break;
			case LPT_IN_RISE:
			case LPT_IN_FALL:
			default:
				break;
		}
	}
	
	ptLptBase->CEDR = (ptLptBase->CEDR & 0xffff0000) | eClkCtrl | eCssSel | eShdwSel | wFltckPrs;
	ptLptBase->PSCR = ePscDiv;
	ptLptBase->CR = (ptLptBase->CR & (~LPT_OPM_MSK)) | eOpm;
}

csp_error_t csp_lpt_swf_trg(csp_lpt_t *ptLptBase, uint8_t byTrg)
{
	if (byTrg>0)
		return CSP_FAIL;
	ptLptBase->EVSWF = LPT_EVSWF;
	return CSP_SUCCESS;
}

csp_error_t csp_lpt_sync_enable(csp_lpt_t *ptLptBase, uint8_t byTrg, bool bEnable)
{
	if (byTrg>0)
		return CSP_FAIL;
	ptLptBase->SYNCR = (ptLptBase->SYNCR & ~LPT_SYNCEN_MSK)| (bEnable<<LPT_SYNCEN_POS);
	return CSP_SUCCESS;
}

csp_error_t csp_lpt_set_sync_mode(csp_lpt_t *ptLptBase, uint8_t byTrg, lpt_syncmd_e eMd)
{
	if (byTrg>0)
		return CSP_FAIL;
	ptLptBase->SYNCR = (ptLptBase->SYNCR & ~LPT_OSTMD_MSK) | (eMd << LPT_OSTMD_POS);
	return CSP_SUCCESS;
}
csp_error_t csp_lpt_rearm_sync(csp_lpt_t *ptLptBase,uint8_t byTrg)
{
	if (byTrg>0)
		return CSP_FAIL;
	ptLptBase->SYNCR |= LPT_REARM;
	return CSP_SUCCESS;
}

csp_error_t csp_lpt_auto_rearm_enable(csp_lpt_t *ptLptBase, uint8_t byTrg, bool bEnable)
{
	if (byTrg>0)
		return CSP_FAIL;
	ptLptBase -> SYNCR = (ptLptBase -> SYNCR & ~LPT_AREARM_MSK)| (bEnable <<LPT_AREARM_POS);
	return CSP_SUCCESS;
}

csp_error_t csp_lpt_set_trgprd(csp_lpt_t *ptLptBase, uint8_t byTrg, uint8_t byPeriod)
{
	if (byTrg>0)
		return CSP_FAIL;
	ptLptBase->EVPS = (ptLptBase->EVPS & ~LPT_TRGEVPRD_MSK)| LPT_TRGEVPRD(byPeriod);
	return CSP_SUCCESS;
}
csp_error_t csp_lpt_set_trgcnt(csp_lpt_t *ptLptBase, uint8_t byTrg, uint8_t byCnt)
{
	if (byTrg>0)
		return CSP_FAIL;
	ptLptBase->EVPS = (ptLptBase->EVPS & ~LPT_TRGEVCNT_MSK)| LPT_TRGEVCNT(byCnt);
	return CSP_SUCCESS;
}

csp_error_t csp_lpt_trg_enable(csp_lpt_t *ptLptBase, uint8_t byTrg, bool bEnable)
{
	if (byTrg>0)
		return CSP_FAIL;
	ptLptBase->EVTRG = (ptLptBase->EVTRG & ~LPT_TRG0OE_MSK) | (bEnable << LPT_TRG0OE_POS);
	return CSP_SUCCESS;
}