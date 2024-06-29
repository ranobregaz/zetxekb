/***********************************************************************//** 
 * \file  csp_syscon.c
 * \brief  clk related function in csp level
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-10 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/
 
#include "csp_syscon.h"
 
 /** \brief isosc enable
 * 
 *  \param[in] ptSysconBase: pointer of SYSCON reg structure.
 *  \return csp_error_t.
 */
csp_error_t csp_isosc_enable(csp_syscon_t *ptSysconBase)
 {
	ptSysconBase->GCER = ISOSC;
	//wait for ISOSC to stable
	while(!(csp_get_ckst(ptSysconBase)& ISOSC));
	return CSP_SUCCESS;
 }
 
 /** \brief isosc disable
 * 
 *  \param[in] ptSysconBase: pointer of SYSCON reg structure.
 *  \return csp_error_t.
 */
csp_error_t csp_isosc_disable(csp_syscon_t *ptSysconBase)
{		
	if ((ptSysconBase->SCLKCR & SYSCLK_SRC_MSK) == SC_ISOSC || csp_iwdt_rd_st(ptSysconBase) )
		return CSP_FAIL;
	else {
		ptSysconBase->GCDR = ISOSC;
		return CSP_SUCCESS;
	}	
}

 /** \brief imosc enable
 * 
 *  \param[in] ptSysconBase: pointer of SYSCON reg structure.
 *  \return csp_error_t
 */
csp_error_t csp_imosc_enable(csp_syscon_t *ptSysconBase, uint32_t wFreq)
{
	csp_set_imosc_fre(ptSysconBase, wFreq);
	ptSysconBase->GCER = IMOSC;
	//wait for IMOSC to stable
	while(!(csp_get_ckst(ptSysconBase)& IMOSC));
	return CSP_SUCCESS;
}

 /** \brief imosc disable
 * 
 *  \param[in] ptSysconBase: pointer of SYSCON reg structure.
 *  \return csp_error_t
 */
csp_error_t csp_imosc_disable(csp_syscon_t *ptSysconBase)
{
	if ((ptSysconBase->SCLKCR & SYSCLK_SRC_MSK) == SC_IMOSC)
		return CSP_FAIL;
	else{
		ptSysconBase->GCDR = IMOSC;
		return CSP_SUCCESS;
	}
}

 /** \brief emosc enable
 * 
 *  \param[in] ptSysconBase: pointer of SYSCON reg structure.
 *  \return csp_error_t
 */
csp_error_t csp_emosc_enable(csp_syscon_t *ptSysconBase, uint32_t wFreq)
{
	if (wFreq > 20000000)
		csp_set_em_gain(ptSysconBase, 0x1f);
	else
		csp_set_em_gain(ptSysconBase, 0x7);
	ptSysconBase->GCER = EMOSC;
	//wait for IMOSC to stable
	while(!(csp_get_ckst(ptSysconBase)& EMOSC));
	return CSP_SUCCESS;
}

 /** \brief emosc disable
 * 
 *  \param[in] ptSysconBase: pointer of SYSCON reg structure.
 *  \return csp_error_t
 */
csp_error_t csp_emosc_disable(csp_syscon_t *ptSysconBase)
{
	
	if ((ptSysconBase->SCLKCR & SYSCLK_SRC_MSK) == SC_EMOSC)
		return CSP_FAIL;
	else{
		ptSysconBase->GCDR = EMOSC;
		return CSP_SUCCESS;
	}

	return CSP_SUCCESS;
}


 /** \brief hfosc enable
 * 
 *  \param[in] ptSysconBase: pointer of SYSCON reg structure.
 *  \return csp_error_t.
 */
csp_error_t csp_hfosc_enable(csp_syscon_t *ptSysconBase, uint32_t wFreq)
{
	csp_set_hfosc_fre(ptSysconBase, wFreq);	
	ptSysconBase->GCER = HFOSC;
	//wait for HFOSC to stable
	while(!(csp_get_ckst(ptSysconBase)& HFOSC));
	return CSP_SUCCESS;
}

 /** \brief hfosc disable
 * 
 *  \param[in] ptSysconBase: pointer of SYSCON reg structure.
 *  \return csp_error_t
 */
csp_error_t csp_hfosc_disable(csp_syscon_t *ptSysconBase)
{
	if ((ptSysconBase->SCLKCR & SYSCLK_SRC_MSK) == SC_HFOSC)
		return CSP_FAIL;
	else{
		ptSysconBase->GCDR = HFOSC;
		return CSP_SUCCESS;
	}
}

 /** \brief set flash to work in low power mode
 * 
 *  \param[in] ptSysconBase: pointer of SYSCON reg structure.
 *  \param	bEnable: ENABLE/DISABL
 *  \return none
 */
void csp_eflash_lpmd_enable(csp_syscon_t *ptSysconBase, bool bEnable)
{
	ptSysconBase->OPT1 = (ptSysconBase->OPT1 & ~FLASH_LPMODE_MSK)| (bEnable << FLASH_LPMODE_POS);
}


 /** \brief enable systick clock
 * 
 *  \param[in] ptSysconBase: pointer of SYSCON reg structure.
 *  \return none
 */
csp_error_t csp_systick_enable(csp_syscon_t *ptSysconBase)
{
	ptSysconBase->GCER = SYSTICK;
	//wait for HFOSC to stable
	while(!(csp_get_ckst(ptSysconBase)& (0x1 << 8)));
	return CSP_SUCCESS;
}

 /** \brief disable systick clock
 * 
 *  \param[in] ptSysconBase: pointer of SYSCON reg structure.
 *  \return none
 */
csp_error_t csp_systick_disable(csp_syscon_t *ptSysconBase)
{
	ptSysconBase->GCDR = SYSTICK;
	return CSP_SUCCESS;
}

void csp_clk_pm_enable(csp_syscon_t *ptSysconBase, clk_pm_e eClk, bool bEnable)
{
	if (bEnable)
		ptSysconBase->GCER = 0x1 << eClk;
	else
		ptSysconBase->GCDR = 0x1 << eClk;
}

void csp_iwdt_enable(csp_syscon_t *ptSysconBase)
{
	ptSysconBase -> IWDEDR = EN_IWDT | IWDTE_KEY;
	while((ptSysconBase->IWDCR & IWDT_ST) != IWDT_BUSY);
	ptSysconBase -> IWDCNT = (ptSysconBase -> IWDCNT & (~IWDT_CLR_MSK)) | IWDT_CLR << IWDT_CLR_POS;
	while((ptSysconBase->IWDCNT & IWDT_CLR_BUSY) == 1);
}

void csp_iwdt_clr(csp_syscon_t *ptSysconBase)
{
	ptSysconBase -> IWDCNT = (ptSysconBase -> IWDCNT & (~IWDT_CLR_MSK)) | IWDT_CLR << IWDT_CLR_POS;
	while((ptSysconBase->IWDCNT & IWDT_CLR_BUSY) == 1);
	//while((ptSysconBase->IWDCNT & IWDT_CLR_BUSY) == 0);
}


void csp_iwdt_reg_reset(csp_syscon_t *ptSysconBase)
{
	ptSysconBase->IWDCR = 0x70c |IWDTCR_KEY; 
	csp_iwdt_clr(ptSysconBase);
	
	//ptSysconBase->IWDEDR = 0x9 | IWDTE_KEY;
}

csp_error_t csp_syscon_set_evtrg_src(csp_syscon_t *ptSysconBase, uint8_t byTrgSeln, exi_trg_src_e eSrc)
{
	csp_error_t tRet = CSP_SUCCESS;
		
	if (byTrgSeln < 4 && eSrc < 16){
		ptSysconBase -> EVTRG = (ptSysconBase -> EVTRG & ~(TRG_SRC0_3_MSK(byTrgSeln))) | (eSrc << TRG_SRC0_3_POS(byTrgSeln));
	}
	else if (byTrgSeln < 6 && eSrc > 15) {
		
		ptSysconBase -> EVTRG = (ptSysconBase -> EVTRG & ~(TRG_SRC4_5_MSK(byTrgSeln)))| ((eSrc-16) << TRG_SRC4_5_POS(byTrgSeln));
	}
	else
		tRet = CSP_FAIL;
	return tRet;
}

csp_error_t csp_syscon_evtrg_enable(csp_syscon_t *ptSysconBase, uint8_t byTrgSeln, bool bEnable)
{
	csp_error_t tRet = CSP_SUCCESS;
	
	if (byTrgSeln < 6) {
		ptSysconBase -> EVTRG = (ptSysconBase -> EVTRG & ~(ENDIS_ESYNC_MSK(byTrgSeln))) | (bEnable << ENDIS_ESYNC_POS(byTrgSeln));
	}
	else
		tRet = CSP_FAIL;
	
	return tRet;		
} 

csp_error_t csp_syscon_set_evtrg_prd(csp_syscon_t *ptSysconBase, uint8_t byTrgSeln, uint8_t byPrd)
{
	csp_error_t tRet = CSP_SUCCESS;
	
	if (byTrgSeln < 4) {
		ptSysconBase -> EVPS = (ptSysconBase -> EVPS & ~(TRG_EVPRD_MSK(byTrgSeln)))| ((byPrd-1) << TRG_EVPRD_POS(byTrgSeln));
	}
	else
		tRet = CSP_FAIL;
	return tRet;
}

csp_error_t csp_syscon_evtrg_cnt_clr(csp_syscon_t *ptSysconBase, uint8_t byTrgSeln)
{
	csp_error_t tRet = CSP_SUCCESS;
	
	if (byTrgSeln < 4) {
		ptSysconBase -> EVTRG |= 0x1 << TRG_EVCNT_CLR_POS(byTrgSeln);
	}
	else
		tRet = CSP_FAIL;
	return tRet;
}

uint8_t csp_syscon_get_evtrg_cnt(csp_syscon_t *ptSysconBase, uint8_t byTrgSeln)
{
	csp_error_t tRet = CSP_SUCCESS;
	
	if (byTrgSeln < 4) {
		return (ptSysconBase -> EVPS & TRG_EVCNT_MSK(byTrgSeln));
	}
	else
		tRet = CSP_FAIL;
	return tRet;
}