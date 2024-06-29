/***********************************************************************//** 
 * \file  csp_cnta.c
 * \brief CNTA description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-10 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-9 <td>V0.1  <td>ZJY   <td>modify
 * </table>
 * *********************************************************************
*/

/* Includes ---------------------------------------------------------------*/
#include <sys_clk.h>
#include "csp_cnta.h"
#include "csp_common.h"

/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/** \brief cnta reg default init
 * 
 *  \param[in] ptCntaBase: pointer of CNTA reg structure
 *  \return none
 */
void csp_cnta_def_init(csp_cnta_t *ptCntaBase)
{
	ptCntaBase->CADATAH = CNTA_RESET_VALUE;
    ptCntaBase->CADATAL = CNTA_RESET_VALUE;
    ptCntaBase->CACON   = CNTA_RESET_VALUE;
    ptCntaBase->INTMASK = CNTA_RESET_VALUE;
}
/** \brief cnta interrupt config
 * 
 *  \param[in] ptCntaBase: pointer of CNTA reg structure
 *  \param[in] eBtInt: int num
 *  \param[in] bEnable: enable,disable
 *  \return none
 */ 
void csp_cnta_set_int(csp_cnta_t *ptCntaBase, cnta_int_e eBtInt,bool bEnable)
{
	if(bEnable)
		ptCntaBase->INTMASK |= eBtInt; 
	else
		ptCntaBase->INTMASK  &= ~eBtInt; 
}
/** \brief cnta init
 * 
 *  \param[in] ptCntaBase: pointer of CNTA reg structure
 *  \param[in] eClkDiv: pclk div select
 *  \param[in] eMode: once/continuous
 *  \return none
 */
 /*
void csp_cnta_init(csp_cnta_t *ptCntaBase, cnta_carrier_e eCarCtrl, cnta_remstat_e eRemsta, 
				cnta_ckdiv_e eClkDiv, cnta_mode_e eMode, cnta_osp_e eOsp)
{
	ptCntaBase->CACON = (ptCntaBase->CACON & 0xfd7fffcc);
	ptCntaBase->CACON |= (eCarCtrl | eRemsta | eClkDiv | eMode | eOsp);
   
}
*/
void csp_cnta_init(csp_cnta_t *ptCntaBase, cnta_ckdiv_e eClkDiv, cnta_mode_e eMode)
{
	ptCntaBase->CACON = (ptCntaBase->CACON & ~(CNTA_CKDIV_MSK | CNTA_MODE_MSK));
	ptCntaBase->CACON |= (eClkDiv << CNTA_CKDIV_POS) | (eMode << CNTA_MODE_POS);
   
}
/** \brief cnta carrier control config
 * 
 *  \param[in] ptCntaBase: pointer of CNTA reg structure
 *  \param[in] eCarCtrl: carrier enable/disable
 *  \param[in] eEnvelope: rem ouput signal select
 *  \param[in] eRemsta: rem output level of close carrier, low/high
 *  \param[in] eOsp: rem output initial level, low/high
 *  \return none
 */
/*
void csp_cnta_set_carrier(csp_cnta_t *ptCntaBase, cnta_carrier_e eCarCtrl, cnta_envelope_e eRemSel, cnta_remstat_e eRemsta, 
					cnta_pendrem_e ePend, cnta_matchrem_e eMatch, cnta_strobe_e eHstrobe, cnta_osp_e eOsp)
{
	ptCntaBase->CACON = (ptCntaBase->CACON & 0xfc01fffe);
	ptCntaBase->CACON |= (eCarCtrl | eRemSel | eRemsta | ePend | eMatche | Hstrobe | eOsp);
   
}
*/
void csp_cnta_set_carrier(csp_cnta_t *ptCntaBase, cnta_carrier_e eCarCtrl, cnta_envelope_e eEnvelope, 
						cnta_remstat_e eRemsta, cnta_osp_e eOsp)
{
	ptCntaBase->CACON = (ptCntaBase->CACON & ~(CNTA_CARRIER_MSK | CNTA_ENVELOPE_MSK | CNTA_REMSTAT_MSK | CNTA_OSP_MSK));
	ptCntaBase->CACON |= (eCarCtrl << CNTA_CARRIER_POS) | (eEnvelope << CNTA_ENVELOPE_POS) | (eRemsta << CNTA_REMSTAT_POS) | (eOsp << CNTA_OSP_POS);
}
/** \brief set cnta sync evtrg control
 * 
 *  \param[in] ptCntaBase: pointer of CNTA reg structure
 *  \param[in] ePende: carrieron control of tc pend end 
 *  \param[in] eMatch: carrieron control of tc pulse match 
 *  \param[in] eHstrobe: data reg value auto reload control
 *  \return none
 */
void csp_cnta_set_sync(csp_cnta_t *ptCntaBase, cnta_pendrem_e eTcPend, cnta_matchrem_e eTcMatch, cnta_hwstrobe_e eHwstrobe)
{
	ptCntaBase->CACON = (ptCntaBase->CACON & ~(CNTA_PEND_REM_MSK | CNTA_MATCH_REM_MSK | CNTA_HW_STROBE_MSK));
	ptCntaBase->CACON |= (eTcPend << CNTA_PEND_REM_POS) | (eTcMatch << CNTA_MATCH_REM_POS) | (eHwstrobe << CNTA_HW_STROBE_POS);
}
/** \brief set pwm ouput freq, high/low level time long
 * 
 *  \param[in] ptCntaBase: pointer of CNTA reg structure
 *  \param[in] hwDataH: rem ouput high time long = (clk/freq * (duty cycle) -3)
 *  \param[in] hwDataL: rem ouput low time long  = (clk/freq * (1 - duty cycle) -3)
 *  \return none
 */
void csp_cnta_set_cadata(csp_cnta_t *ptCntaBase, uint16_t hwDataH, uint16_t hwDataL)
{
	
	ptCntaBase->CADATAH = hwDataH;		//value = (cnta clk)/(pwm freq) 
	ptCntaBase->CADATAL = hwDataL;		//DATAH = value * (duty cycle) - 3; DATAL = value * (1 - duty cycle) - 3
}

/** \brief set cnta pwm frequency
 * 
 *  \param[in] ptCntaBase: pointer of CNTA reg structure
 *  \param[in] wFreq: rem ouput frequency
 *  \param[in] byDutyCycle: rem ouput duty cycle, (0->100)
 *  \return none
 */
void csp_cnta_set_carrier_fre(csp_cnta_t *ptCntaBase, uint32_t wFreq, uint8_t byDutyCycle)
{
	uint16_t hwClkDiv;
	uint32_t wCntaclk;
	//uint32_t hwDataH;
	//uint32_t hwDataL;
	uint32_t wPeriod;
	
	
	hwClkDiv = csp_cnta_get_ckdiv(ptCntaBase);
	//wCntaclk = IMOSC_5M_VALUE >> hwClkDiv;
	wCntaclk = soc_get_pclk_freq() >> hwClkDiv;
	wPeriod = wCntaclk / wFreq;				//pwm period = wTemp
	
	//hwDataH = wTemp * byDutyCycle / 100 - 3;
	//hwDataL = wTemp * (100- byDutyCycle) / 100 -3;
	
	ptCntaBase->CADATAH = wPeriod * byDutyCycle / 100 - 3;;			//value = (cnta clk)/(pwm freq) 
	ptCntaBase->CADATAL = wPeriod * (100- byDutyCycle) / 100 -3;;	//DATAH = value * (duty cycle) - 3; DATAL = value * (1 - duty cycle) - 3
	
	//csp_cnta_start(ptCntaBase);
}
/** \brief updata cnta datah and datal value
 * 
 *  \param[in] ptCntaBase: pointer of CNTA reg structure
 *  \param[in] hwDataH: rem ouput high time long 
 *  \param[in] hwDataL: rem ouput low time long  
 *  \return none
 */
void csp_cnta_cadata_update(csp_cnta_t *ptCntaBase, uint16_t hwDataH, uint16_t hwDataL)
{
	ptCntaBase->CADATAH = hwDataH;		
	ptCntaBase->CADATAL = hwDataL;
	
	ptCntaBase->CACON |= (CNTA_SW_STROBE << CNTA_SW_STROBE_POS);
}