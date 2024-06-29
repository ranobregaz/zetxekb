/***********************************************************************//** 
 * \file  ept.c
 * \brief  Enhanced purpose timer driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-8 <td>V0.0  <td>WNN   <td>initial
 * <tr><td> 2021-1-8 <td>V0.1  <td>LJY   <td>modify on 110
 * <tr><td> 2021-1-12 <td>V0.2  <td>WNN   <td>adapt to 102
 * </table>
 * *********************************************************************
*/

#include "sys_clk.h"
#include "drv/common.h"
#include "drv/ept.h"
#include "csp_ept.h"
#include "drv/pin.h"
#include <drv/irq.h>

///to maximize the  speed
extern uint32_t g_ept_prd;

/** \brief ept interrupt handle function
 *  \param[in] args: dev of ept
 *  \return none
 */ 
static void apt_ept_irqhandler(void *args)
{
	csi_ept_t *ptEpt		= (csi_ept_t *)args;
    csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	uint32_t wEmIntSt = 0;
	uint32_t wIntSt = 0;
	bool bCntDir = 0;
	
	wIntSt =   csp_ept_get_misr(ptEptBase);	
	wEmIntSt = csp_ept_get_emmisr(ptEptBase);
	bCntDir =  csp_ept_get_cntdir(ptEptBase);
	if (wIntSt > 0) {
		
		if (wIntSt & EPTINT_TRGEV0)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_TRGEV0);
		}
		else if (wIntSt & EPTINT_PEND)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_PEND);
		}
		else if (wIntSt & EPTINT_CAPLD0)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_CAPLD0);			
		}
		else if (wIntSt & EPTINT_CAPLD1)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_CAPLD1);
		}
		else if (wIntSt & EPTINT_CAPLD2)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_CAPLD2);
		}
		else if (wIntSt & EPTINT_CAPLD3)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_CAPLD3);
		}
		
		else if (wIntSt & EPTINT_TRGEV1)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_TRGEV1);
		}
		else if (wIntSt & EPTINT_TRGEV2)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_TRGEV2);
		}
		else if (wIntSt & EPTINT_TRGEV3)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_TRGEV3);
		}
		else if (wIntSt & EPTINT_CAU)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_CAU);
		}
		else if (wIntSt & EPTINT_CAD)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_CAD);
		}
		else if (wIntSt & EPTINT_CBU)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_CBU);
		}
		else if (wIntSt & EPTINT_CBD)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_CBD);
		}
		else if (wIntSt & EPTINT_CCU)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_CCU);
		}
		else if (wIntSt & EPTINT_CCD)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_CCD);
		}
		else if (wIntSt & EPTINT_CDU)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_CDU);
		}
		else if (wIntSt & EPTINT_CDD)
		{
			csp_ept_clr_int(ptEptBase, EPTINT_CDD);
		}
	}	
	if (wEmIntSt) {
		if (wEmIntSt & EPTINT_EP0){
			csp_ept_clr_emint(ptEptBase, EPTINT_EP0);
		}
		else if (wEmIntSt & EPTINT_EP1){
			csp_ept_clr_emint(ptEptBase, EPTINT_EP1);
		}
		else if (wEmIntSt & EPTINT_EP2){
			csp_ept_clr_emint(ptEptBase, EPTINT_EP2);
		}
		else if (wEmIntSt & EPTINT_EP3){
			csp_ept_clr_emint(ptEptBase, EPTINT_EP3);
		}
		else if (wEmIntSt & EPTINT_EP4){
			csp_ept_clr_emint(ptEptBase, EPTINT_EP4);
		}
		else if (wEmIntSt & EPTINT_EP5){
			csp_ept_clr_emint(ptEptBase, EPTINT_EP5);
		}
		else if (wEmIntSt & EPTINT_EP6){
			csp_ept_clr_emint(ptEptBase, EPTINT_EP6);
		}
		else if (wEmIntSt & EPTINT_EP7){
			csp_ept_clr_emint(ptEptBase, EPTINT_EP7);
		}
	}
	if (ptEpt->callback){
		ptEpt->callback(ptEpt, bCntDir,wEmIntSt>>1, wIntSt, ptEpt->arg);
		
	}
}

 /** \brief  register ept interrupt callback function
 * 
 *  \param[in] ept: ept handle to operate
 *  \param[in] callback: gpio interrupt handle function
 *  \param[in] arg: para
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_ept_attach_callback(csi_ept_t *ptEpt, void *callback, void *arg)
{
    CSI_PARAM_CHK(ptEpt, CSI_ERROR);
    CSI_PARAM_CHK(callback, CSI_ERROR);
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
    ptEpt->callback = callback;
    ptEpt->arg      = arg;

	csp_ept_int_enable(ptEptBase, (uint32_t)arg, ENABLE);
    csi_irq_attach((uint32_t)ptEpt->dev.irq_num, &apt_ept_irqhandler, &ptEpt->dev);
    csi_irq_enable((uint32_t)ptEpt->dev.irq_num);
    return CSI_OK;

}


 /** \brief  detach ept interrupt callback function
 *  \param[in] ept: ept handle to operate
 */ 
void csi_ept_detach_callback(csi_ept_t *ptEpt)
{
	CSI_PARAM_CHK_NORETVAL(ptEpt);

    ptEpt->callback = NULL;
    ptEpt->arg = NULL;
    csi_irq_disable((uint32_t)ptEpt->dev.irq_num);
    csi_irq_detach((uint32_t)ptEpt->dev.irq_num);
}

/**
  \brief       Initialize EPT Interface. Initializes the resources needed for the EPT interface
  \param[in]   ptEpt    ept handle to operate
  \param[in]   idx    ept idx
  \return      error code \ref csi_error_t
*/
csi_error_t csi_ept_init(csi_ept_t *ptEpt, uint32_t idx)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    csp_ept_t *ptEptBase = NULL;
	
	if (0 == target_get(DEV_EPT_TAG, idx, &ptEpt->dev)) {
		ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
		csi_clk_enable(&ptEpt->dev);
		csp_ept_clken(ptEptBase);
		csp_ept_reset(ptEptBase);
		
	} else {
		ret = CSI_ERROR;
	}
	return ret;
}

/**
  \brief       De-initialize EPT Interface. stops operation and releases the software resources used by the interface
  \param[in]   ptEpt    EPT handle to operate
  \return      None
*/
void csi_ept_uninit(csi_ept_t *ptEpt)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	csp_ept_reset(ptEptBase);
	csi_clk_disable(&ptEpt->dev);
}

/**
 \brief set EPT start mode. 
 \param ptEpt    EPT handle to operate
 \return eMode ：EPT_SW/EPT_SYNC
*/
void csi_ept_set_start_mode(csi_ept_t *ptEpt, csi_ept_stmd_e eMode)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
   	
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	csp_ept_set_start_mode(ptEptBase, eMode);
}

/**
 \brief SW start EPT counter
 \param ptEpt    EPT handle to operate
*/
void csi_ept_swstart(csi_ept_t *ptEpt)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
	
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	csp_ept_sw_start(ptEptBase);
}

/**
 \brief set EPT operation mode
 \param ptEpt    EPT handle to operate
 \param eMode 	 EPT_OP_CONT/EPT_OP_OT
*/
void csi_ept_set_os_mode(csi_ept_t *ptEpt, csi_ept_opmd_e eMode)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
	
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	csp_ept_set_opmd(ptEptBase, eMode);
}

/**
 \brief set EPT stop status
 \param ptEpt    EPT handle to operate
 \param eSt 	 EPT_STP_HZ/EPT_STP_LOW
*/
void csi_ept_set_stop_st(csi_ept_t *ptEpt, csi_ept_stopst_e eSt)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
	
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	csp_ept_set_stop_st(ptEptBase, eSt);
}

/**
 \brief SW stop EPT counter
 \param ptEpt    EPT handle to operate
*/
void csi_ept_swstop(csi_ept_t *ptEpt)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
	
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	csp_ept_sw_stop(ptEptBase);
}
/**
 \brief set EPT frequency(All channels use the same the counter). 
  * \param ptEpt 	EPT handle to operate
  * \param wHz		frequency
  * */
csi_error_t csi_ept_set_freq(csi_ept_t *ptEpt, uint32_t wHz)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
	
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	return (csp_ept_set_freq(ptEptBase, wHz));
}

/**
 \brief set EPT frequency(All channels use the same the counter). 
  * !!!has to use with csi_ept_set_singleout_edge_aligned_md1/2() !!!
  * \param ptEpt 	EPT handle to operate
  * \param wHz		frequency
  * */
csi_error_t csi_ept_set_freq_edge_aligned(csi_ept_t *ptEpt, uint32_t wHz)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
	
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	return (csp_ept_set_freq_edge_aligned(ptEptBase, wHz));
}

/**
 \brief config ptEpt dual channel output mode1. 
  * Counter works in up and down mode
  * CHnX active High ,ChnY active High
  * \verbatim
  *  	ChnX    	______
  *  		_______|      |_______ 
  *   	ChnY
  *  		_____			_______
  *      		|__________|
  * \endverbatim
  * \param ptEpt 	EPT handle to operate
  * \param eCh		EPT output channel(EPT_CH_A/B/C/D)
  * \param wHz	    output frequency
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \param wT1ns	left db time(500 means 500ns)
  * \param wT2ns	right db time (500 means 500ns)
  * \return ret		error
*/
csi_error_t csi_ept_set_ch_dualout_md1(csi_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wDutyCycle, uint32_t wT1ns, uint32_t wT2ns)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
    csi_error_t ret = CSI_OK;
	
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	ret = csp_ept_set_ch_dualout(ptEptBase, eCh, wDutyCycle, wT1ns, wT2ns);
	if (ret < 0 )
		return CSI_ERROR;
		
	switch (eCh)
	{	
		case (EPT_CH_A):
			csp_ept_set_dbcha(ptEptBase, E_DBOUT_AR_BF, E_DB_POL_B, E_DBCHAIN_AR_AF);
			break;
		case (EPT_CH_B):
			csp_ept_set_dbchb(ptEptBase, E_DBOUT_AR_BF, E_DB_POL_B, E_DBCHBIN_BR_BF);
			break;
		case (EPT_CH_C):
			csp_ept_set_dbchc(ptEptBase, E_DBOUT_AR_BF, E_DB_POL_B, E_DBCHCIN_CR_CF);			
		default:
			break;
	
	}
	return ret;
}

/**
 \brief config ptEpt dual channel output mode.
  * * Counter works in up and down mode
  *	\verbatim 
  *  	CHnX active Low ,ChnY active Low
  *		ChnX    ______		 _____
  *				     |_______| 
  *		ChnY	     ___________
  *			   _____|          |______	
  * \endverbatim 	
  * \param ptEpt 	EPT handle to operate
  * \param eCh		EPT output channel(EPT_CH_A/B/C/D)
  * \param wHz	    output frequency
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \param wT1ns	left db time(500 means 500ns)
  * \param wT2ns	right db time (500 means 500ns)
*/
csi_error_t csi_ept_set_ch_dualout_md2(csi_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wDutyCycle, uint32_t wT1ns, uint32_t wT2ns)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
    csi_error_t ret = CSI_OK;
	
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	ret = csp_ept_set_ch_dualout(ptEptBase, eCh, wDutyCycle, wT1ns, wT2ns);
	if (ret < 0 )
		return CSI_ERROR;
		
	switch (eCh)
	{	
		case (EPT_CH_A):
			csp_ept_set_dbcha(ptEptBase, E_DBOUT_AR_BF, E_DB_POL_A, E_DBCHAIN_AR_AF);
			break;
		case (EPT_CH_B):
			csp_ept_set_dbchb(ptEptBase, E_DBOUT_AR_BF, E_DB_POL_A, E_DBCHBIN_BR_BF);
			break;
		case (EPT_CH_C):
			csp_ept_set_dbchc(ptEptBase, E_DBOUT_AR_BF, E_DB_POL_A, E_DBCHCIN_CR_CF);			
		default:
			break;
	
	}
	return ret;
}

/**
 \brief config ptEpt dual channel output mode. 
  * * Counter works in up and down mode
  * CHnX active High ,ChnY active High
  * \verbatim
  *  	ChnX    	______
  *  		_______|      |_______ 
  *   	ChnY       _________
  *  		______|			|_______
  *    
  * \endverbatim
  * 	
  * \param ptEpt 	EPT handle to operate
  * \param eCh		EPT output channel(EPT_CH_A/B/C/D)
  * \param wHz	    output frequency
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \param wT1ns	left db time(500 means 500ns)
  * \param wT2ns	right db time (500 means 500ns)
*/
csi_error_t csi_ept_set_ch_dualout_md3(csi_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wDutyCycle, uint32_t wT1ns, uint32_t wT2ns)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
    csi_error_t ret = CSI_OK;
	
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	ret = csp_ept_set_ch_dualout(ptEptBase, eCh,  wDutyCycle, wT1ns, wT2ns);
	if (ret < 0 )
		return CSI_ERROR;
		
	switch (eCh)
	{	
		case (EPT_CH_A):
			csp_ept_set_dbcha(ptEptBase, E_DBOUT_AR_BF, E_DB_POL_DIS, E_DBCHAIN_AR_AF);
			break;
		case (EPT_CH_B):
			csp_ept_set_dbchb(ptEptBase, E_DBOUT_AR_BF, E_DB_POL_DIS, E_DBCHBIN_BR_BF);
			break;
		case (EPT_CH_C):
			csp_ept_set_dbchc(ptEptBase, E_DBOUT_AR_BF, E_DB_POL_DIS, E_DBCHCIN_CR_CF);			
		default:
			break;
	
	}
	return ret;
}

/**
 \brief config ptEpt dual channel output mode. 
  * * Counter works in up and down mode
  *	CHnX active Low ,ChnY active Low
  *	\verbatim 
  *		ChnX    ______		 _____
  *				     |_______| 
  *		ChnY	_____          ____
  *			        |_________|	
  * \endverbatim 
  * 
  * \param ptEpt 	EPT handle to operate
  * \param eCh		EPT output channel(EPT_CH_A/B/C/D)
  * \param wHz	    output frequency
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \param wT1ns	left db time(500 means 500ns)
  * \param wT2ns	right db time (500 means 500ns)
*/
csi_error_t csi_ept_set_ch_dualout_md4(csi_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wDutyCycle, uint32_t wT1ns, uint32_t wT2ns)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
    csi_error_t ret = CSI_OK;
	
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	ret = csp_ept_set_ch_dualout(ptEptBase, eCh, wDutyCycle, wT1ns, wT2ns);
	csi_ept_get_prd(ptEpt);
	if (ret < 0 )
		return CSI_ERROR;
		
	switch (eCh)
	{	
		case (EPT_CH_A):
			csp_ept_set_dbcha(ptEptBase, E_DBOUT_AR_BF, E_DB_POL_AB, E_DBCHAIN_AR_AF);
			break;
		case (EPT_CH_B):
			csp_ept_set_dbchb(ptEptBase, E_DBOUT_AR_BF, E_DB_POL_AB, E_DBCHBIN_BR_BF);
			break;
		case (EPT_CH_C):
			csp_ept_set_dbchc(ptEptBase, E_DBOUT_AR_BF, E_DB_POL_AB, E_DBCHCIN_CR_CF);			
		default:
			break;
	
	}
	return ret;
}

/**
 \brief config ptEpt single output mode1
  * * Counter works in up and down mode
  * \verbatim
  *  	ChnX    	______
  *  		_______|      |_______ 
  * \endverbatim	
  * \param ptEpt 	EPT handle to operate
  * \param eCh		EPT output channel(EPT_CH_A/B/C/D)
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \return tRet   
*/

csi_error_t csi_ept_set_singleout_md1(csi_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wDutyCycle)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
    csi_error_t tRet = CSI_OK;
	
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	tRet = csp_ept_set_ch_singleout(ptEptBase, eCh, wDutyCycle);
	switch(eCh)
	{
		case (EPT_CHa):
			csp_ept_aqcra_config(ptEptBase, EPT_CMPA, NA, HI, NA, LO);
			break;
		case (EPT_CHb):
			csp_ept_aqcrb_config(ptEptBase, EPT_CMPB, NA, HI, NA, LO);
			break;
		case (EPT_CHc):
			csp_ept_aqcrc_config(ptEptBase, EPT_CMPC, NA, HI, NA, LO);
			break;
		case (EPT_CHd):
			csp_ept_aqcrd_config(ptEptBase, EPT_CMPD, NA, HI, NA, LO);
		default:
			tRet = CSP_FAIL;
			break;
	}
		
	return tRet;
}

/**
 \brief config ptEpt single output mode2
  * * Counter works in up and down mode
  *	\verbatim 
  *		ChnX    ______		 _____
  *				     |_______| 
  * \endverbatim	
  * \param ptEpt 	EPT handle to operate
  * \param eCh		EPT output channel(EPT_CH_A/B/C/D)
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \return tRet   
*/

csi_error_t csi_ept_set_singleout_md2(csi_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wDutyCycle)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
    csi_error_t tRet = CSI_OK;
	
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	tRet = csp_ept_set_ch_singleout(ptEptBase, eCh, wDutyCycle);
	switch(eCh)
	{
		case (EPT_CHa):
			csp_ept_aqcra_config(ptEptBase, EPT_CMPA, HI, LO, NA, HI);
			break;
		case (EPT_CHb):
			csp_ept_aqcrb_config(ptEptBase, EPT_CMPB, HI, LO, NA, HI);
			break;
		case (EPT_CHc):
			csp_ept_aqcrc_config(ptEptBase, EPT_CMPC,  HI, LO, NA, HI);
			break;
		case (EPT_CHd):
			csp_ept_aqcrd_config(ptEptBase, EPT_CMPD,  HI, LO, NA, HI);
		default:
			tRet = CSP_FAIL;
			break;
	}
		
	return tRet;
}


/**
 \brief config ptEpt single output mode1, edge aligned
  * * Counter works in up and down mode
  * \verbatim
  *  	ChnX    	______
  *  		_______|      
  * \endverbatim	
  * \param ptEpt 	EPT handle to operate
  * \param eCh		EPT output channel(EPT_CH_A/B/C/D)
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \return tRet   
*/

csi_error_t csi_ept_set_singleout_edge_aligned_md1(csi_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wDutyCycle)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
    csi_error_t tRet = CSI_OK;
	
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	tRet = csp_ept_set_ch_singleout(ptEptBase, eCh, wDutyCycle);
	switch(eCh)
	{
		case (EPT_CHa):
			csp_ept_aqcra_config(ptEptBase, EPT_CMPA, LO, HI, NA, LO);
			csp_ept_aqcra_config(ptEptBase, EPT_CMPA, HI, NA,LO, HI);
			break;
		case (EPT_CHb):
			csp_ept_aqcrb_config(ptEptBase, EPT_CMPB, LO, HI, NA, LO);
			break;
		case (EPT_CHc):
			csp_ept_aqcrc_config(ptEptBase, EPT_CMPC, LO, HI, NA, LO);
			break;
		case (EPT_CHd):
			csp_ept_aqcrd_config(ptEptBase, EPT_CMPD, LO, HI, NA, LO);
		default:
			tRet = CSP_FAIL;
			break;
	}
		
	return tRet;
}

/**
 \brief config ptEpt single output mode1, edge aligned
  * * Counter works in up and down mode
  * \verbatim
  *  	ChnX  ______		
  *				    |_______
  * \endverbatim	
  * \param ptEpt 	EPT handle to operate
  * \param eCh		EPT output channel(EPT_CH_A/B/C/D)
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \return tRet   
*/

csi_error_t csi_ept_set_singleout_edge_aligned_md2(csi_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wDutyCycle)
{
	CSI_PARAM_CHK(ptEpt, CSI_ERROR);
    csi_error_t tRet = CSI_OK;
	
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	tRet = csp_ept_set_ch_singleout(ptEptBase, eCh, wDutyCycle);
	switch(eCh)
	{
		case (EPT_CHa):
			csp_ept_aqcra_config(ptEptBase, EPT_CMPA, HI, LO, LO, NA);
			break;
		case (EPT_CHb):
			csp_ept_aqcrb_config(ptEptBase, EPT_CMPB, HI, LO, LO, NA);
			break;
		case (EPT_CHc):
			csp_ept_aqcrc_config(ptEptBase, EPT_CMPC, HI, LO, LO, NA);
			break;
		case (EPT_CHd):
			csp_ept_aqcrd_config(ptEptBase, EPT_CMPD, HI, LO, LO, NA);
		default:
			tRet = CSP_FAIL;
			break;
	}
		
	return tRet;
}



/**
 \brief get counter period to calculate the duty cycle. 
 \param ptGpta    ept handle to operate
 \return counter period (reg data)
*/
uint16_t csi_ept_get_prd(csi_ept_t *ptEpt)
{
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	return csp_ept_get_prd(ptEptBase);
}

/**
 \brief change ept output dutycycle. 
 \param ptEpt    ept handle to operate
 \param wActiveTime cmpx data to be set directly
*/
csi_error_t csi_ept_change_ch_duty(csi_ept_t *ptEpt, csi_ept_chtype_e eCh, uint32_t wActiveTime)
{
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	return (csp_ept_set_ch_duty(ptEptBase, eCh, wActiveTime));
}

/**
 \brief chopper configuration
 \param ptEpt    		ept handle to operate 
 \param byCdiv   		fchop = PCLK/((byCdiv+1)*8)
 \param byCduty  		(0~7) duty = byduty/8 
 \param byFirstWidth 	T = Tchop * byFirstWidth
 \return none 
*/
void csi_ept_set_cp(csi_ept_t *ptEpt, uint8_t byCdiv, uint8_t byCduty, uint8_t byFirstWidth)
{
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	csp_ept_set_cp_div(ptEptBase, byCdiv);
	csp_ept_set_cp_duty(ptEptBase, byCduty);
	csp_ept_set_cp_ospwth(ptEptBase, byFirstWidth);
}

/**
 \brief channel A chopper output enable 
 \param ptEpt    ept handle to operate
 \param bEnableX/Y  ENABLE/DISABLE
 \return none
*/
void csi_ept_cpa_enable(csi_ept_t *ptEpt, bool bEnableX, bool bEnableY)
{
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	csp_ept_cpa_enable(ptEptBase, bEnableX, bEnableY);
}

/**
 \brief channel B chopper output enable 
 \param ptEpt    ept handle to operate
 \param bEnableX/Y  ENABLE/DISABLE
 \return none
*/
void csi_ept_cpb_enable(csi_ept_t *ptEpt, bool bEnableX, bool bEnableY)
{
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	csp_ept_cpb_enable(ptEptBase, bEnableX, bEnableY);
}

/**
 \brief channel C chopper output enable 
 \param ptEpt    ept handle to operate
 \param bEnableX/Y  ENABLE/DISABLE
 \return none
*/
void csi_ept_cpc_enable(csi_ept_t *ptEpt, bool bEnableX, bool bEnableY)
{
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	csp_ept_cpc_enable(ptEptBase, bEnableX, bEnableY);
}
/**
 \brief channeln chopper output enable 
 \param ptEpt    ept handle to operate
 \param eEbi 	 external emergency input: EPT_EBI0~4 （EBI4 = LVD）
 \param ePol	 active polarity：EPT_EBI_POL_H/L
 \param eLckMode lock mode: EPT_EP_DIS/SLCK/HLCK
 \param eOutput  output when lock happens: EPT_EM_OUT_HZ/H/L/DIS
 \return none
*/
void csi_ept_set_em(csi_ept_t *ptEpt, csi_ept_ebi_e eEbi, csi_ept_ebipol_e ePol, csi_ept_ep_lckmd_e eLckMode, csi_ept_em_out_e eOutput, bool bIntEn)
{
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	uint8_t byEp = eEbi;
/*	
	switch (eEbi) {
		case (EPT_EBILVD): byEp = 0;
			break;

		default:	byEp = eEbi + 1;
			switch (byEp) {
				case (1): csi_pin_set_mux(PA07, PA07_EBI0);
					break;
				case (2): csi_pin_set_mux(PA013, PA013_EBI1);
					break;
				case (3): csi_pin_set_mux(PB03, PB03_EBI2);
					break;
				case (4): csi_pin_set_mux(PB02, PB02_EBI3);
				default:
					break;
			}
			break;
	
	}
*/	
	csp_ept_set_em_src(ptEptBase, byEp, byEp+1);
	csp_ept_set_ebi_pol(ptEptBase, byEp, ePol);
	csp_ept_set_ep_lckmd(ptEptBase, byEp, eLckMode);
	csp_ept_emint_enable(ptEptBase, 0x1<<byEp, bIntEn);	
	
	
	csp_ept_set_emlck_output(ptEptBase, EPT_CH_A, EPT_X, (eOutput.eChaxOut & 0x3));
	csp_ept_set_emlck_output(ptEptBase, EPT_CH_A, EPT_Y, (eOutput.eChayOut & 0x3));
	csp_ept_set_emlck_output(ptEptBase, EPT_CH_B, EPT_X, (eOutput.eChbxOut & 0x3));
	csp_ept_set_emlck_output(ptEptBase, EPT_CH_B, EPT_Y, (eOutput.eChbyOut & 0x3));
	csp_ept_set_emlck_output(ptEptBase, EPT_CH_C, EPT_X, (eOutput.eChcxOut & 0x3));
	csp_ept_set_emlck_output(ptEptBase, EPT_CH_C, EPT_Y, (eOutput.eChcyOut & 0x3));
	csp_ept_set_emlck_output(ptEptBase, EPT_CH_D, EPT_X, (eOutput.eChdOut & 0x3));
	
	
}

/**
 \brief software force lock
 \param ptEpt    ept handle to operate
 \param eEbi 	 external emergency input: EPT_EBI0~4 （EBI4 = LVD）
 \return none
*/
void csi_ept_force_em(csi_ept_t *ptEpt, uint8_t byEbi)
{
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	/*if (byEbi == EPT_EBILVD)
		csp_ept_force_em(ptEptBase, 0);
	else*/
		csp_ept_force_em(ptEptBase,byEbi);
}

/**
 \brief get harklock status
 \param ptEpt    ept handle to operate
 \return uint8_t 0b_ _ _ _ _ _ _ _
*/
uint8_t csi_ept_get_hdlck_st(csi_ept_t *ptEpt)
{
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	return (csp_ept_get_emhdlck_st(ptEptBase));
}

/**
 \brief clear harklock status
 \param ptEpt    ept handle to operate
 \return  eEbi 	 external emergency input: EPT_EBI0~3/EPT_EBILVD
*/
void csi_ept_clr_hdlck(csi_ept_t *ptEpt, csi_ept_ebi_e eEbi)
{
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	csp_ept_clr_emhdlck(ptEptBase, eEbi);
}

/**
 \brief get 	 softlock status
 \param ptEpt    ept handle to operate
 \return uint8_t 0b_ _ _ _ _ _ _ _
*/
uint8_t csi_ept_get_sftlck_st(csi_ept_t *ptEpt)
{
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	return (csp_ept_get_emsftlck_st(ptEptBase));
}

/**
 \brief clear softlock status
 \param ptEpt    ept handle to operate
 \return  eEbi 	 external emergency input: EPT_EBI0~3/EPT_EBILVD
*/
void csp_ept_clr_sftlck(csi_ept_t *ptEpt, csi_ept_ebi_e eEbi)
{
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	csp_ept_clr_emhdlck(ptEptBase, eEbi);
}

/**
  \brief       enable/disable ept in debug mode
  \param[in]   ptEpt       EPT handle to operate
  \param[in]   bEnable		ENABLE/DISABLE
*/

void csi_ept_debug_enable(csi_ept_t *ptEpt, bool bEnable)
{
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	csp_ept_dbg_enable(ptEptBase, bEnable);
}

/**
  \brief   config ept out trigger 
  \param   ptEpt       EPT handle to operate
  \param 	eCh			0/1/2/3
  \param 	eSrc	
  \param   byTime		output trigger when event happens the 'byTime'th time.
*/
csi_error_t csi_ept_set_evtrg(csi_ept_t *ptEpt,uint8_t byCh,csi_ept_evtrg_src_e eSrc, uint8_t byTime)
{

	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	switch (byCh){
		case 0:
		case 1: if (eSrc == EPT_EVSRC_PEND)
					return CSI_ERROR;
				csp_ept_set_trgsrc01(ptEptBase, byCh, eSrc);
				break;
		case 2:
		case 3: if (eSrc == EPT_EVSRC_EX)
					return CSI_ERROR;
				if (eSrc == EPT_EVSRC_PEND)
					eSrc = 12;
				csp_ept_set_trgsrc23(ptEptBase, byCh, eSrc);
				break;
		default: return CSI_ERROR;
				break;
	}
	
	csp_ept_set_trgprd(ptEptBase, byCh, byTime-1);
	csp_ept_trg_xoe_enable(ptEptBase, byCh, ENABLE);
	return CSI_OK;
}


/**
  \brief   enable/disable ept out trigger 
  \param   ptEpt       EPT handle to operate
  \param   byCh   0/1/2/3
  \param   bEnable  ENABLE/DISABLE
*/
csi_error_t csi_ept_evtrg_enable(csi_ept_t *ptEpt, uint8_t byCh, bool bEnable)
{
 csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
 
 if (byCh > 3)
  return CSI_ERROR;
 csp_ept_trg_xoe_enable(ptEptBase, byCh, bEnable);
 return CSI_OK;
}
/**
  \brief       get ept capture data
  \param[in]   ptGpta         gpta handle to operate
  \param[in]   num          load sequence
  \return      hwData		capture data 
*/
uint16_t csi_ept_capture_get_cap_data(csi_ept_t *ptEpt, uint8_t num)
{
	uint16_t hwData=0;
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	switch (num) 
	{
		case (0) :return ptEptBase -> CMPA ;
			break;
		case (1) :return ptEptBase -> CMPB ;
			break;
		case (2) :return ptEptBase -> CMPC ;
			break;
		case (3) :return ptEptBase -> CMPD ;
			break;
		default :
			return CSI_ERROR;
			break;
	}
	return hwData;
}

/**
  \brief   set ept sync
  \param   ptEpt       EPT handle to operate
   \param   bySync    sync select: 0/1/2/3
   \param  csi_ept_syncmode_t  EPT_SYNC_CONT/EPT_SYNC_OS
   \param  bARearmEnable auto rearm enable/disable
  \return csi_error_t   
*/
csi_error_t csi_ept_set_sync(csi_ept_t *ptEpt, uint8_t bySync, csi_ept_syncmode_e tSyncMode, bool bARearmEnable)
{
	csp_ept_t *ptEptBase = (csp_ept_t *)HANDLE_REG_BASE(ptEpt);
	
	csp_ept_set_sync_mode(ptEptBase, bySync, tSyncMode);
	if (bARearmEnable)
		csp_ept_set_auto_rearm(ptEptBase, EPT_AREARM_ZRO);
	else
		csp_ept_set_auto_rearm(ptEptBase, EPT_AREARM_DIS);
	csp_ept_sync_enable(ptEptBase, bySync, ENABLE);
	
	return CSI_OK;
}
