/***********************************************************************//** 
 * \file  csp_bt.c
 * \brief BT description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-08 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-08 <td>V0.1  <td>ZJY   <td>macro definition style, NOT use update immediately
 * </table>
 * *********************************************************************
*/

/* Includes ---------------------------------------------------------------*/
#include <sys_clk.h>
#include "csp_bt.h"

/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/** \brief bt reg default init
 * 
 *  \param[in] ptBtBase: pointer of BT reg structure
 *  \return none
 */
void csp_bt_rst_init(csp_bt_t *ptBtBase)
{	
	/*
	ptBtBase->RSSR	= BT_RESET_VALUE;
	ptBtBase->CR		= BT_RESET_VALUE;
	ptBtBase->PSCR	= BT_RESET_VALUE;
	ptBtBase->PRDR	= BT_RESET_VALUE;
	ptBtBase->CMP	= BT_RESET_VALUE;
	ptBtBase->CNT	= BT_RESET_VALUE;
	ptBtBase->EVTRG  = BT_RESET_VALUE;
	ptBtBase->EVSWF	= BT_RESET_VALUE;
	ptBtBase->IMCR   = BT_RESET_VALUE;
	ptBtBase->ICR	= 0x0000000f;
	*/
	csp_bt_soft_rst(ptBtBase);

}
/** \brief get bt running status
 * 
 *  \param[in] ptBtBase: pointer of BT reg structure
 *  \return none
 */ 
uint8_t csp_bt_get_work_state(csp_bt_t *ptBtBase)
{
	if(ptBtBase->RSSR & BT_START)
		return ENABLE;
	else
		return DISABLE;
		
}
/** \brief bt interrupt config
 * 
 *  \param[in] ptBtBase: pointer of BT reg structure
 *  \param[in] eBtInt: int num
 *  \param[in] bEnable: enable,disable
 *  \return none
 */ 
void csp_bt_set_int(csp_bt_t *ptBtBase, bt_int_e eBtInt,bool bEnable)
{
	if(bEnable)
		ptBtBase->IMCR |= eBtInt; 
	else
		ptBtBase->IMCR &= ~eBtInt; 
}
/** \brief bt timer mode config
 * 
 *  \param[in] ptBtBase: pointer of BT reg structure
 *  \param[in] eClkCtrl: count clk control,enable/disable
 *  \param[in] eShdwMode: shadow/immediate mode select
 *  \param[in] eOpmMode: count work mode,coutinuous/once
 *  \param[in] eExtckMode: external count work mode
 *  \param[in] eCntRld: count PRDR auto reload control
 *  \param[in] hwPscr: clk frequency division, bt clk = pclk/(hwPscr+1)
 *  \return none
 */
void csp_bt_set_timer(csp_bt_t *ptBtBase, bt_clk_e eClkCtrl, bt_shdw_e eShdwMode, bt_opm_e eOpmMode, 
					bt_extckm_e eExtckMode, bt_cntrld_e eCntRld, uint16_t hwPscr)
{
	ptBtBase->CR = (ptBtBase->CR & 0xfffeffc6) | (eClkCtrl << BT_CLK_POS) | (eShdwMode << BT_SHDW_POS) | 
				(eOpmMode << BT_OPM_POS) | (eExtckMode << BT_EXTCKM_POS) | (eCntRld << BT_CNTRLD_POS);// | BT_DBG_EN;
	ptBtBase->PSCR = hwPscr;
}
/** \brief bt timer pwm config
 * 
 *  \param[in] ptBtBase: pointer of BT reg structure
 *  \param[in] eStartLev: bt srart output status, high/low
 *  \param[in] eIdleLev: bt idle output status, high/low
 *  \param[in] hwCmp: bt CMP value
 *  \param[in] hwPrdr: bt PRDR value
 *  \return none
 */
void csp_bt_set_pwm(csp_bt_t *ptBtBase, bt_startst_e eStartLev, bt_idlest_e eIdelLev, uint16_t hwCmp, uint16_t hwPrdr)
{
	ptBtBase->CR = (ptBtBase->CR & 0xffffff3f) | (eStartLev << BT_STARTST_POS) | (eIdelLev << BT_IDLEST_POS);
	ptBtBase->PRDR = hwPrdr;		//period = (bt clk)/(pwm freq), prdr value = period 
	ptBtBase->CMP = hwCmp;			//cmp value = period * (duty cycle) 
}
/** \brief bt timer pwm frequency 
 * 
 *  \param[in] ptBtBase: pointer of BT reg structure
 *  \param[in] eStartLev: bt srart output status, high/low
 *  \param[in] eIdleLev: bt idle output status, high/low
 *  \param[in] wFreq: bt pwm frequency
 *  \param[in] byDutyCycle: bt pwm duty cycle,(0 -> 100)
 *  \return none
 */
void csp_bt_pwm_config(csp_bt_t *ptBtBase, bt_startst_e eStartLev, bt_idlest_e eIdelLev, uint32_t wFreq,uint8_t byDutyCycle)
{
	uint16_t hwClkDiv;
	uint32_t wBtclk;
	uint32_t hwPrdr;
	uint32_t hwCmp;
	
	//hwClkDiv = csp_bt_get_pscr(ptBtBase);
	//wBtclk = soc_get_pclk_freq()/(hwClkDiv +1);
	
	hwClkDiv = 4;
	wBtclk = HFOSC_48M_VALUE/(hwClkDiv +1);	//bt clk
	hwPrdr = (wBtclk / wFreq);				//prdr value = period = (bt clk)/(pwm freq), 
	hwCmp = hwPrdr * byDutyCycle / 100;		//cmp value = period * (duty cycle) 
	
	ptBtBase->CR = (ptBtBase->CR & 0xffffff3f) | (eStartLev << BT_STARTST_POS) | (eIdelLev << BT_IDLEST_POS);
	ptBtBase->PRDR = hwPrdr;		
	ptBtBase->CMP = hwCmp;			
}
/** \brief updata timer prdr and pscr value
 * 
 *  \param[in] ptCntaBase: pointer of CNTA reg structure
 *  \param[in] hwPrdr prdr reg value
 *  \param[in] hwCmp: cmp reg value
 *  \param[in] eUpdata: force updata prdr\cmp reg  
 *  \return none
 */
void csp_timer_data_update(csp_bt_t *ptBtBase, uint16_t hwPrdr, uint16_t hwCmp)
{
	ptBtBase->PRDR = hwPrdr;		
	ptBtBase->CMP  = hwCmp;	
	//ptBtBase->CR |= (BT_UPDATE_EN << BT_UPDATA_POS);
}
/** \brief bt sync evtrg config
 * 
 *  \param[in] ptBtBase: pointer of BT reg structure
 *  \param[in] eSync: external sync control, enable/disable
 *  \param[in] eSyncCmd: external trg work mode
 *  \param[in] eOstmd: sync trg mode,continuous/once
 *  \param[in] eArearm: hardware auto REARM control
 *  \param[in] eTrgin: sync trg input channel(0~1)
 *  \return none
 */
void csp_bt_set_sync(csp_bt_t *ptBtBase, bt_sync_e eSync, bt_synccmd_e eSyncCmd, bt_ostmd_e eOstmd, bt_arearm_e eArearm, bt_evtrg_in_e eTrgin)
{
	
	ptBtBase->CR &= 0xffff30ff;
	
	if(eTrgin == BT_SYNCEN1)
	{
		eOstmd = eOstmd << 1;
		eSync = eSync << 1;
	}
		
	ptBtBase->CR |= (eSync << BT_SYNC_POS) | (eSyncCmd << BT_SYNCCMD_POS) | (eOstmd << BT_OSTMD_POS) | (eArearm << BT_AREARM_POS);
}
/** \brief bt timer evtrg output config
 * 
 *  \param[in] ptBtBase: pointer of BT reg structure
 *  \param[in] byEvtrg: trigger event (only 0)
 *  \param[in] eTrgSrc: bt evtrg source(1~3)
 */
csp_error_t csp_bt_set_evtrg(csp_bt_t *ptBtBase, uint8_t byEvtrg, bt_evtrg_src_e eTrgSrc)
{
	if (byEvtrg > 0)
		return CSP_FAIL;
	
	if(eTrgSrc == BT_TRGSRC_DIS)
	{
		ptBtBase->EVTRG = 0x00;
	}
	else
		ptBtBase->EVTRG =(ptBtBase->EVTRG & (~BT_TRGSRC_MSK)) | eTrgSrc;
		
	return CSP_SUCCESS;
}
/** \brief bt vic int enbale
 * 
 *  \param[in] ptBtBase: pointer of BT reg structure
 *  \return none
 */ 
void csp_bt_vic_irq_en(csp_bt_t *ptBtBase)
{
	irqn_type_e eIrqNum;
	switch((uint32_t)ptBtBase)
	{
		case APB_BT0_BASE:
			eIrqNum = BT0_IRQn;
			break;
		case APB_BT1_BASE:
			eIrqNum = BT1_IRQn;
			break;
		default:
			eIrqNum = BT0_IRQn;
			break;
	}
	
	ptBtBase->ICR = 0x0f;
	NVIC_EnableIRQ(eIrqNum);
}
