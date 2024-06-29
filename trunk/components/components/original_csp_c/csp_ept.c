
/***********************************************************************//** 
 * \file  csp_ept.c
 * \brief  Enhanced timer description and static inline functions at register level
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-4 <td>V0.0  <td>WNN   <td>initial
 * <tr><td> 2021-1-4 <td>V0.1  <td>LJY   <td>modify on 110
 * <tr><td> 2021-1-12 <td>V0.2  <td>WNN   <td>adapt to 102
 * </table>
 * *********************************************************************
*/
 
#include "csp_ept.h"
#include "sys_clk.h"

uint32_t g_ept_prd;

void csp_ept_sw_start(csp_ept_t *ptEptBase)
{
	csp_ept_wr_key(ptEptBase);
	ptEptBase -> RSSR |= EPT_START;
}

void csp_ept_sw_stop(csp_ept_t *ptEptBase)
{
	csp_ept_wr_key(ptEptBase);
	ptEptBase -> RSSR &= (~EPT_START);
}
///only valid in sync start mode
void csp_ept_phs_enable(csp_ept_t *ptEptBase, uint16_t hwVal)
{
	ptEptBase -> CR |= EPT_INIT_AS_PHSEN;
	ptEptBase -> PHSR = hwVal;
}

void csp_ept_phs_disable(csp_ept_t *ptEptBase)
{
	ptEptBase->CR &= (~EPT_INIT_AS_PHSEN);
}

void csp_ept_aqcra_config(csp_ept_t *ptEptBase, csp_ept_csrc_sel_e eCasel, csp_ept_action_e eZro, csp_ept_action_e eCau, csp_ept_action_e ePrd, csp_ept_action_e eCad)
{
	ptEptBase -> AQCRA =   (ptEptBase -> AQCRA & (~EPT_CASEL_MSK)  & (~EPT_ACT_ZRO_MSK) &(~EPT_ACT_CAU_MSK) &(~EPT_ACT_CAD_MSK)  & (~EPT_ACT_PRD_MSK)) 
						 | (eCasel << EPT_CASEL_POS) | (eZro << EPT_ACT_ZRO_POS) | (eCau << EPT_ACT_CAU_POS) | (eCad << EPT_ACT_CAD_POS)| (ePrd << EPT_ACT_PRD_POS);
}

void csp_ept_aqcrb_config(csp_ept_t *ptEptBase, csp_ept_csrc_sel_e eCasel, csp_ept_action_e eZro, csp_ept_action_e eCau, csp_ept_action_e ePrd, csp_ept_action_e eCad)
{
	ptEptBase -> AQCRB = (ptEptBase -> AQCRB & (~EPT_CASEL_MSK)  & (~EPT_ACT_ZRO_MSK) &(~EPT_ACT_CAU_MSK) &(~EPT_ACT_CAD_MSK)  & (~EPT_ACT_PRD_MSK)) 
						| (eCasel << EPT_CASEL_POS) | (eZro << EPT_ACT_ZRO_POS) | (eCau << EPT_ACT_CAU_POS) | (eCad << EPT_ACT_CAD_POS)| (ePrd << EPT_ACT_PRD_POS);
}

void csp_ept_aqcrc_config(csp_ept_t *ptEptBase, csp_ept_csrc_sel_e eCasel, csp_ept_action_e eZro, csp_ept_action_e eCau, csp_ept_action_e ePrd, csp_ept_action_e eCad)
{
	ptEptBase -> AQCRC = (ptEptBase -> AQCRC & (~EPT_CASEL_MSK)  & (~EPT_ACT_ZRO_MSK) &(~EPT_ACT_CAU_MSK) &(~EPT_ACT_CAD_MSK)  & (~EPT_ACT_PRD_MSK)) 
						| (eCasel << EPT_CASEL_POS) | (eZro << EPT_ACT_ZRO_POS) | (eCau << EPT_ACT_CAU_POS) | (eCad << EPT_ACT_CAD_POS)| (ePrd << EPT_ACT_PRD_POS);
}

void csp_ept_aqcrd_config(csp_ept_t *ptEptBase, csp_ept_csrc_sel_e eCasel, csp_ept_action_e eZro, csp_ept_action_e eCau, csp_ept_action_e ePrd, csp_ept_action_e eCad)
{
	ptEptBase -> AQCRD = (ptEptBase -> AQCRD & (~EPT_CASEL_MSK)  & (~EPT_ACT_ZRO_MSK) &(~EPT_ACT_CAU_MSK) &(~EPT_ACT_CAD_MSK)  & (~EPT_ACT_PRD_MSK)) 
						| (eCasel << EPT_CASEL_POS) | (eZro << EPT_ACT_ZRO_POS) | (eCau << EPT_ACT_CAU_POS) | (eCad << EPT_ACT_CAD_POS)| (ePrd << EPT_ACT_PRD_POS);
}
csp_error_t csp_ept_set_ch_duty(csp_ept_t *ptEptBase, csp_ept_chtype_e eCh, uint32_t wActiveTime)
{
	csp_error_t tRet = CSP_SUCCESS;	
	
	if (wActiveTime <= g_ept_prd) {
		switch (eCh) {
			case (EPT_CHa): csp_ept_set_cmpa(ptEptBase, wActiveTime);
				break;
			case (EPT_CHb): csp_ept_set_cmpb(ptEptBase, wActiveTime);
				break;
			case (EPT_CHc): csp_ept_set_cmpc(ptEptBase, wActiveTime);
				break;
			case (EPT_CHd): csp_ept_set_cmpd(ptEptBase, wActiveTime);
			default:
				break;
		}
	}
		
	else {
		tRet = CSI_ERROR;
	}
	return tRet;
}


csp_error_t csp_ept_set_freq(csp_ept_t *ptEptBase, uint32_t wHz)
{
	uint32_t wPrd, wTempPrd;
	csp_error_t tRet = CSP_SUCCESS;
	
	csp_ept_set_cntmd(ptEptBase, EPT_CNTMD_UPDN);
	csp_ept_set_mode(ptEptBase, EPT_MODE_OUT);	
	
	wPrd = (soc_get_pclk_freq()/wHz) >>1;
	
	wTempPrd = wPrd/0x10000;
	
	csp_ept_set_psc(ptEptBase, wTempPrd);
	csp_ept_set_prdr(ptEptBase, wPrd/(wTempPrd+1));
	
	g_ept_prd = csp_ept_get_prd(ptEptBase);
	
	return tRet;
}

csp_error_t csp_ept_set_freq_edge_aligned(csp_ept_t *ptEptBase, uint32_t wHz)
{
	uint32_t wPrd, wTempPrd;
	csp_error_t tRet = CSP_SUCCESS;
	
	csp_ept_set_cntmd(ptEptBase, EPT_CNTMD_UP);
	csp_ept_set_mode(ptEptBase, EPT_MODE_OUT);	
	
	wPrd = (soc_get_pclk_freq()/wHz);
	
	wTempPrd = wPrd/0x10000;
	
	csp_ept_set_psc(ptEptBase, wTempPrd);
	csp_ept_set_prdr(ptEptBase, wPrd/(wTempPrd+1));
	
	g_ept_prd = csp_ept_get_prd(ptEptBase);
	
	return tRet;
}

/// wHz >732Hz  @ PCLK = 48MHz
csp_error_t csp_ept_set_ch_dualout(csp_ept_t *ptEptBase, csp_ept_chtype_e eCh, uint32_t wDutyCycle, uint32_t wT1ns, uint32_t wT2ns)
{
	csp_error_t tRet = CSP_SUCCESS;
	uint32_t wPrd, wCmpData, wTdb;
	
	wPrd =  csp_ept_get_prd(ptEptBase);
	wCmpData = wPrd*(100-wDutyCycle)/100;

	switch(eCh)
	{
		case (EPT_CHa):
			csp_ept_set_cmpa(ptEptBase, wCmpData);
			csp_ept_aqcra_config(ptEptBase, EPT_CMPA, NA, HI, NA, LO);
			break;
		case (EPT_CHb):
			csp_ept_set_cmpb(ptEptBase, wCmpData);
			csp_ept_aqcrb_config(ptEptBase, EPT_CMPB, NA, HI, NA, LO);
			break;
		case (EPT_CHc):
			csp_ept_set_cmpc(ptEptBase, wCmpData);
			csp_ept_aqcrc_config(ptEptBase, EPT_CMPC, NA, HI, NA, LO);
			break;
		case (EPT_CHd):
		default:
			return CSP_UNSUPPORTED;
			break;
	}
	
	//dbclk = hclk
	csp_ept_set_dpscr(ptEptBase, 0);
	csp_ept_set_dbclksrc(ptEptBase, EPT_DB_DPSC);
	wTdb = wT1ns * (g_wSystemClk/1000) / 1000000;

//	if (wTdb < 1)
//		return CSP_UNSUPPORTED;
	csp_ept_set_dbdtr(ptEptBase, wTdb);
	wTdb = wT2ns * (g_wSystemClk/1000) / 1000000;
//	if (wTdb < 1)
//		return CSP_UNSUPPORTED;
	csp_ept_set_dbdtf(ptEptBase, wTdb);
	
	return tRet;
}


csp_error_t csp_ept_set_ch_singleout(csp_ept_t *ptEptBase, csp_ept_chtype_e eCh, uint32_t wDutyCycle)
{
	csp_error_t tRet = CSP_SUCCESS;
	uint32_t wPrd, wCmpData;
		
	wPrd =  csp_ept_get_prd(ptEptBase);
	wCmpData = wPrd*(100-wDutyCycle)/100;
	switch(eCh)
	{
		case (EPT_CHa):
			csp_ept_set_cmpa(ptEptBase, wCmpData);
			break;
		case (EPT_CHb):
			csp_ept_set_cmpb(ptEptBase, wCmpData);
			break;
		case (EPT_CHc):
			csp_ept_set_cmpc(ptEptBase, wCmpData);
			break;
		case (EPT_CHd):
			csp_ept_set_cmpd(ptEptBase, wCmpData);
		default:
			tRet = CSP_FAIL;
			break;
	}
	
	return tRet;
}

void csp_ept_set_emlck_output(csp_ept_t *ptEptBase, csp_ept_chtype_e eCh, csp_ept_xy_e eXy,  csp_ept_emout_e eOutput)
{
	uint8_t byPos = 0;
	ptEptBase -> REGPROT = EPT_REGPROT;
	if (eCh == EPT_CHd) {
		ptEptBase -> EMOSR = (ptEptBase -> EMOSR & (~EPT_EMCHD_O_MSK )) | (eOutput << EPT_EMCHD_O_POS);
	}
	else {
		byPos = (eCh<<1) + (eXy<<3);
		ptEptBase -> EMOSR = (ptEptBase -> EMOSR & ~(0x3 << byPos )) | (eOutput << byPos);
	}
}


csp_error_t csp_ept_ldrst_enable(csp_ept_t *ptEptBase, uint8_t cmp, bool bEnable)
{
	csp_error_t tRet = CSP_SUCCESS;
	if(cmp < 4) {
		ptEptBase -> CR =  (ptEptBase->CR & ~(EPT_CMP_LDRST_MSK(cmp))) | ((~bEnable) << EPT_CMP_LDRST_POS(cmp));
	}
	else
		tRet = CSP_FAIL;
		
	return tRet;
}
//                                                                  csp_ept_cmpdata_ldmd_e  csp_ept_cmpdata_ldmd_e
csp_error_t csp_ept_set_cmp_ldmd(csp_ept_t *ptEptBase, uint8_t cmp, csp_ept_cmpdata_ldmd_e eLdmd, csp_ept_ldtcmp_e eLdT)
{
	csp_error_t tRet = CSP_SUCCESS;
	if(cmp < 4) {
		ptEptBase -> CMPLDR = (ptEptBase -> CMPLDR & ~(EPT_CMP_LDTIME_MSK(cmp))) | (eLdmd << EPT_CMP_LDTIME_POS(cmp));
	}
	else
		tRet = CSP_FAIL;
		
	return tRet;
}

void csp_ept_evtrg01_config(csp_ept_t *ptEptBase,uint8_t cHnaL ,csp_ept_trgsrc01_e Trigger_source)
{
	ptEptBase -> EVTRG  = (ptEptBase -> EVTRG &~EPT_SEL_MSK_TRG(cHnaL))|(Trigger_source<<EPT_SEL_POS_TRG(cHnaL));
	
}

void csp_ept_evtrg23_config(csp_ept_t *ptEptBase,uint8_t cHnaL ,csp_ept_trgsrc23_e Trigger_source)
{
	ptEptBase -> EVTRG  = (ptEptBase -> EVTRG &~EPT_SEL_MSK_TRG(cHnaL))|(Trigger_source<<EPT_SEL_POS_TRG(cHnaL));
	
}


void csp_ept_sync_config(csp_ept_t *ptEptBase,  uint8_t eCh,  csp_ept_syncmd_e os_mode, bool bARearmEn)
{   csp_ept_wr_key(ptEptBase);
	ptEptBase -> SYNCR  =  (ptEptBase -> SYNCR &~ EPT_SYNC_MSK(eCh)) |(1<<EPT_SYNC_POS (eCh));
	csp_ept_wr_key(ptEptBase);
	ptEptBase -> SYNCR  =  (ptEptBase -> SYNCR &~ EPT_OSTMD_MSK(eCh))|(os_mode<<EPT_OSTMD_POS(eCh));
}





