
/***********************************************************************//** 
 * \file  csp_gpta.c
 * \brief  general purpose timer description and static inline functions at register level
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-24 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/
 
#include "csp_gpta.h"
#include "sys_clk.h"

uint32_t g_gpta_prd;

void csp_gpta_sw_start(csp_gpta_t *ptGptaBase)
{
	
	csp_gpta_wr_key(ptGptaBase);
	ptGptaBase -> RSSR |= GPTA_START;
}

void csp_gpta_sw_stop(csp_gpta_t *ptGptaBase)
{
	
	csp_gpta_wr_key(ptGptaBase);
	ptGptaBase -> RSSR &= (~GPTA_START);
}
///only valid in sync start mode
void csp_gpta_phs_enable(csp_gpta_t *ptGptaBase,  csp_gpta_phsdir_e eVal,uint16_t hwVal)
{
	
	ptGptaBase -> PHSR = hwVal;
	ptGptaBase -> PHSR =(ptGptaBase -> PHSR & (~GPTA_PHSDIR_MSK)) |  (eVal<<GPTA_PHSDIR_POS);	
	ptGptaBase->CR |= GPTA_PHSEN_MSK;
}

void csp_gpta_phs_disable(csp_gpta_t *ptGptaBase)
{
	ptGptaBase->CR &= (~GPTA_PHSEN_MSK);
}

void csp_gpta_aqcra_config(csp_gpta_t *ptGptaBase, csp_gpta_csrc_sel_e eCasel, csp_gpta_action_e eZro, csp_gpta_action_e eCau, csp_gpta_action_e ePrd, csp_gpta_action_e eCad)
{
	ptGptaBase -> AQCRA = (ptGptaBase -> AQCRA & (~GPTA_CASEL_MSK) 
											 & (~GPTA_ACT_ZRO_MSK) &(~GPTA_ACT_CAU_MSK) &(~GPTA_ACT_CAD_MSK)  & (~GPTA_ACT_PRD_MSK)) 
									| (eCasel << GPTA_CASEL_POS) 
									| (eZro << GPTA_ACT_ZRO_POS) | (eCau << GPTA_ACT_CAU_POS) | (eCad << GPTA_ACT_CAD_POS)| (ePrd << GPTA_ACT_PRD_POS);
}

void csp_gpta_aqcrb_config(csp_gpta_t *ptGptaBase, csp_gpta_csrc_sel_e eCasel, csp_gpta_action_e eZro, csp_gpta_action_e eCau, csp_gpta_action_e ePrd, csp_gpta_action_e eCad)
{
	ptGptaBase -> AQCRB = (ptGptaBase -> AQCRB & (~GPTA_CASEL_MSK) 
											 & (~GPTA_ACT_ZRO_MSK) &(~GPTA_ACT_CAU_MSK) &(~GPTA_ACT_CAD_MSK)  & (~GPTA_ACT_PRD_MSK)) 
									| (eCasel << GPTA_CASEL_POS) 
									| (eZro << GPTA_ACT_ZRO_POS) | (eCau << GPTA_ACT_CAU_POS) | (eCad << GPTA_ACT_CAD_POS)| (ePrd << GPTA_ACT_PRD_POS);
}


csp_error_t csp_gpta_set_ch_duty(csp_gpta_t *ptGptaBase, csp_gpta_chtype_e eCh, uint32_t wActiveTime)
{
	csp_error_t tRet = CSP_SUCCESS;	
	
	if (wActiveTime <= g_gpta_prd) {
		switch (eCh) {
			case (GPTA_CHa): csp_gpta_set_cmpa(ptGptaBase, wActiveTime);
				break;
			case (GPTA_CHb): csp_gpta_set_cmpb(ptGptaBase, wActiveTime);
			default:
				break;
		}
	}
		
	else {
		tRet = CSI_ERROR;
	}
	return tRet;
}


csp_error_t csp_gpta_set_outfreq(csp_gpta_t *ptGptaBase, uint32_t wHz)
{
	uint32_t wPrd;
	csp_error_t tRet = CSP_SUCCESS;
	
	csp_gpta_set_cntmd(ptGptaBase, GPTA_CNTMD_UPDN);
	csp_gpta_set_mode(ptGptaBase, GPTA_MODE_OUT);
	
	wPrd = (soc_get_pclk_freq()/wHz) >>1;
	if (wPrd < 0xffff) {
		csp_gpta_set_prdr(ptGptaBase, wPrd);
		csp_gpta_set_psc(ptGptaBase, 0);
		g_gpta_prd = csp_gpta_get_prd(ptGptaBase);
	}
	else {
		tRet = CSP_UNSUPPORTED;
	}
	return tRet;
}

/** \brief in capture mode, get TCLK prd
 *  \param ptGptaBase   gpta handle to operate
 *  \return wTick  xns
 */ 
uint32_t csp_gpta_get_cap_tick(csp_gpta_t *ptGptaBase)
{
	uint32_t wTick;	
		
	csp_gpta_set_psc(ptGptaBase, 0);
	wTick = 1000000000/soc_get_pclk_freq();
	return wTick;
}
/// wHz >732Hz  @ PCLK = 48MHz
csp_error_t csp_gpta_set_dualout(csp_gpta_t *ptGptaBase,  uint32_t wDutyCycle, uint32_t wTns)
{
	csp_error_t tRet = CSP_SUCCESS;
	uint32_t wPrd, wCmpAData, wCmpBData;
	
	wPrd =  csp_gpta_get_prd(ptGptaBase);
	wCmpAData = wPrd*(100-wDutyCycle)/100;
    wCmpBData = wCmpAData - (wTns * (g_wSystemClk/1000) / 1000000) ;
    
	csp_gpta_set_cmpa(ptGptaBase, wCmpAData);
	csp_gpta_set_cmpb(ptGptaBase, wCmpBData);
	
	
///TODO
	
	return tRet;
}


csp_error_t csp_gpta_set_ch_singleout(csp_gpta_t *ptGptaBase, csp_gpta_chtype_e eCh, uint32_t wDutyCycle)
{
	csp_error_t tRet = CSP_SUCCESS;
	uint32_t wPrd, wCmpData;
		
	wPrd =  csp_gpta_get_prd(ptGptaBase);
	wCmpData = wPrd*(100-wDutyCycle)/100;
	switch(eCh)
	{
		case (GPTA_CHa):
			csp_gpta_set_cmpa(ptGptaBase, wCmpData);
			break;
		case (GPTA_CHb):
			csp_gpta_set_cmpb(ptGptaBase, wCmpData);
			break;
		default:
			tRet = CSP_FAIL;
			break;
	}
	
	return tRet;
}

csp_error_t csp_gpta_set_sync(csp_gpta_t *ptGptaBase, uint8_t bySync, csp_gpta_syncmd_e eMd)
{
	if (bySync > 6)
		return CSP_FAIL;
	
	csp_gpta_wr_key(ptGptaBase);	
	ptGptaBase -> SYNCR = (ptGptaBase -> SYNCR & ~GPTA_SYNCMD_MSK(bySync)) | (eMd << GPTA_SYNCMD_POS(bySync));
	return CSP_SUCCESS;
	
}

csp_error_t csp_gpta_sync_config(csp_gpta_t *ptGptaBase,  uint8_t bySync,  csp_gpta_syncmd_e eMd, bool bARearmEn)
{   
	if (bySync > 6)
		return CSP_FAIL;
		
	csp_gpta_wr_key(ptGptaBase);
	ptGptaBase -> SYNCR  =  (ptGptaBase -> SYNCR &~ GPTA_SYNC_MSK(bySync)) |(bARearmEn<<GPTA_SYNC_POS (bySync));
	csp_gpta_wr_key(ptGptaBase);
	ptGptaBase -> SYNCR  =  (ptGptaBase -> SYNCR &~ GPTA_SYNCMD_MSK(bySync))|(eMd<<GPTA_SYNCMD_POS(bySync));
	return CSP_SUCCESS;
}


csp_error_t csp_gpta_sync_rearm(csp_gpta_t *ptGptaBase, uint8_t bySync)
{
	if (bySync > 6)
		return CSP_FAIL;
		
	csp_gpta_wr_key(ptGptaBase);
	ptGptaBase -> SYNCR = (ptGptaBase -> SYNCR & ~GPTA_REARM_MSK(bySync))| GPTA_REARM(bySync);
	return CSP_SUCCESS;
}

csp_error_t csp_gpta_set_sync_enable(csp_gpta_t *ptGptaBase, uint8_t bySync, bool bEnable)
{
	csp_error_t tRet = CSP_SUCCESS;
	
	csp_gpta_wr_key(ptGptaBase);
	
	if (bySync < 6) {
		ptGptaBase -> SYNCR = (ptGptaBase -> SYNCR & ~(GPTA_SYNC_MSK(bySync))) | (bEnable << GPTA_SYNC_POS(bySync));
	}
	else
		tRet = CSP_FAIL;
	return tRet;
}

csp_error_t csp_gpta_ldrst_enable(csp_gpta_t *ptGptaBase, uint8_t cmp, bool bEnable)
{
	csp_error_t tRet = CSP_SUCCESS;
	if(cmp < 4) {
		ptGptaBase -> CR =  (ptGptaBase->CR & ~(GPTA_CMP_LDRST_MSK(cmp))) | ((bEnable) << GPTA_CMP_LDRST_POS(cmp));
	}
	else
		tRet = CSP_FAIL;
		
	return tRet;
}


csp_error_t csp_gpta_set_cmp_ldmd(csp_gpta_t *ptGptaBase, uint8_t cmp, csp_gpta_cmpdata_ldmd_e eLdmd, csp_gpta_ldtcmp_e eLdT)
{
	csp_error_t tRet = CSP_SUCCESS;
	if(cmp < 2) {
		ptGptaBase -> CMPLDR = (ptGptaBase -> CMPLDR & ~(GPTA_CMP_LD_MSK(cmp))) | (eLdmd << GPTA_CMP_LD_MSK(cmp));
		if (eLdmd == GPTA_CMPLD_SHDW) {
			ptGptaBase -> CMPLDR = (ptGptaBase -> CMPLDR & ~(GPTA_CMP_LDTIME_MSK(cmp))) | (eLdmd << GPTA_CMP_LDTIME_POS(cmp));
		}
	}
	else
		tRet = CSP_FAIL;
		
	return tRet;
}

void csp_gpta_swf_ost_config(csp_gpta_t *ptGptaBase, csp_gpta_ldosf_e eChaAct, csp_gpta_ldosf_e eChbAct)
{
	ptGptaBase -> AQOSF = (ptGptaBase -> AQOSF & ~ GPTA_ACTA_MSK &  ~ GPTA_ACTB_MSK) | (eChaAct << GPTA_ACTA_POS) | (eChbAct <<  GPTA_ACTB_POS);
}

void csp_gpta_swf_cont(csp_gpta_t *ptGptaBase, csp_gpta_ldcsf_e eChaAct, csp_gpta_ldcsf_e eChbAct, csp_gpta_ldaqcr_e eLdMd)
{
	ptGptaBase -> AQOSF = (ptGptaBase -> AQOSF & ~GPTA_AQCSF_LDTIME_MSK)| (eLdMd << GPTA_AQCSF_LDTIME_POS);
	ptGptaBase -> AQCSF = (ptGptaBase -> AQCSF & ~ GPTA_CSFA_MSK &  ~ GPTA_CSFB_MSK) | (eChaAct << GPTA_CSFA_POS) | (eChbAct <<  GPTA_CSFB_POS);
}

void csp_gpta_swf_ost(csp_gpta_t *ptGptaBase, bool bChaEnable, bool bChbEnable)
{//csp_gpta_wr_key(ptGptaBase);
	
	 ptGptaBase -> AQOSF  |= (bChaEnable << GPTA_OSTSFA_POS) |  (bChbEnable << GPTA_OSTSFA_POS) ;
}

