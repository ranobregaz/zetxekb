/***********************************************************************//** 
 * \file  gpta.c
 * \brief  general purpose timerA driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-24 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/
#include "sys_clk.h"
#include "drv/common.h"
#include "drv/gpta.h"
#include "csp_gpta.h"
#include "drv/pin.h"
#include "drv/irq.h"
#include "drv/etb.h"

///to maximize the  speed
extern uint32_t g_gpta_prd;


/** \brief gpta interrupt handle function
 *  \param[in] args: dev of gpta
 *  \return none
 */ 
static void apt_gpta_irqhandler(void *args)
{
	csi_gpta_t *gpta		= (csi_gpta_t *)args;
    csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(gpta);
	uint32_t wIntSt = 0;
	if (gpta->callback){

		wIntSt = csp_gpta_get_misr(ptGptaBase);	
		if (wIntSt & GPTAINT_PEND)
		{
			csp_gpta_clr_int(ptGptaBase, GPTAINT_PEND);
		}
		else if (wIntSt & GPTAINT_CAPLD0)
		{
			csp_gpta_clr_int(ptGptaBase, GPTAINT_CAPLD0);
			
		}
		else if (wIntSt & GPTAINT_CAPLD1)
		{
			csp_gpta_clr_int(ptGptaBase, GPTAINT_CAPLD1);
		}
		else if (wIntSt & GPTAINT_CAPLD2)
		{
			csp_gpta_clr_int(ptGptaBase, GPTAINT_CAPLD2);
		}
		else if (wIntSt & GPTAINT_CAPLD3)
		{
			csp_gpta_clr_int(ptGptaBase, GPTAINT_CAPLD3);
		}
		else if (wIntSt & GPTAINT_TRGEV0)
		{
			csp_gpta_clr_int(ptGptaBase, GPTAINT_TRGEV0);
		}
		else if (wIntSt & GPTAINT_TRGEV1)
		{
			csp_gpta_clr_int(ptGptaBase, GPTAINT_TRGEV1);
		}
		else if (wIntSt & GPTAINT_TRGEV2)
		{
			csp_gpta_clr_int(ptGptaBase, GPTAINT_TRGEV2);
		}
		else if (wIntSt & GPTAINT_TRGEV3)
		{
			csp_gpta_clr_int(ptGptaBase, GPTAINT_TRGEV3);
		}
		else if (wIntSt & GPTAINT_CAU)
		{
			csp_gpta_clr_int(ptGptaBase, GPTAINT_CAU);
		}
		else if (wIntSt & GPTAINT_CAD)
		{
			csp_gpta_clr_int(ptGptaBase, GPTAINT_CAD);
		}
		else if (wIntSt & GPTAINT_CBU)
		{
			csp_gpta_clr_int(ptGptaBase, GPTAINT_CBU);
		}
		else if (wIntSt & GPTAINT_CBD)
		{
			csp_gpta_clr_int(ptGptaBase, GPTAINT_CBD);
		}
		if (gpta->callback)
			gpta->callback(gpta, wIntSt, gpta->arg);
		
	}
}

 /** \brief  register gpta interrupt callback function
 * 
 *  \param[in] gpta: gpta handle to operate
 *  \param[in] callback: gpio interrupt handle function
 *  \param[in] arg: para
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_gpta_attach_callback(csi_gpta_t *gpta, void *callback, void *arg)
{
    CSI_PARAM_CHK(gpta, CSI_ERROR);
    CSI_PARAM_CHK(callback, CSI_ERROR);
    csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(gpta);
	
    gpta->callback = callback;
    gpta->arg      = arg;


    csi_irq_attach((uint32_t)gpta->dev.irq_num, &apt_gpta_irqhandler, &gpta->dev);
    csi_irq_enable((uint32_t)gpta->dev.irq_num);
	csp_gpta_int_enable(ptGptaBase, (uint32_t)arg, ENABLE);
    return CSI_OK;

}

 /** \brief  detach gpta interrupt callback function
 *  \param[in] gpta: gpta handle to operate
 */ 
void csi_gpta_detach_callback(csi_gpta_t *ptEpt)
{
	CSI_PARAM_CHK_NORETVAL(ptEpt);

    ptEpt->callback = NULL;
    ptEpt->arg = NULL;
    csi_irq_disable((uint32_t)ptEpt->dev.irq_num);
    csi_irq_detach((uint32_t)ptEpt->dev.irq_num);
}

/**
  \brief       Initialize GPTA Interface. Initializes the resources needed for the GPTA interface
  \param[in]   ptGpta    gpta handle to operate
  \param[in]   idx    gpta idx
  \return      error code \ref csi_error_t
*/
csi_error_t csi_gpta_init(csi_gpta_t *ptGpta, uint32_t idx)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
    csi_error_t tRet = CSI_OK;
    csp_gpta_t *ptGptaBase = NULL;
	
	if (0 == target_get(DEV_GPT_TAG, idx, &ptGpta->dev)) {
		ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
		csi_clk_enable(&ptGpta->dev);
		csp_gpta_clken(ptGptaBase);
		csp_gpta_reset(ptGptaBase);
		
	} else {
		tRet = CSI_ERROR;
	}
	return tRet;
}

/**
  \brief       De-initialize GPTA Interface. stops operation and releases the software resources used by the interface
  \param[in]   ptGpta    GPTA handle to operate
  \return      None
*/
void csi_gpta_uninit(csi_gpta_t *ptGpta)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	csi_gpta_out_swstop(ptGpta);
	csp_gpta_reset(ptGptaBase);
	csi_clk_disable(&ptGpta->dev);
}

/**
 \brief set GPTA start mode. 
 \param ptGpta    GPTA handle to operate
 \return eMode ：GPTA_SW/GPTA_SYNC
*/
void csi_gpta_set_start_mode(csi_gpta_t *ptGpta, csi_gpta_stmd_e eMode)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
   	
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	csp_gpta_set_start_src(ptGptaBase, eMode);
}

/**
 \brief SW start GPTA counter
 \param ptGpta    GPTA handle to operate
*/
void csi_gpta_swstart(csi_gpta_t *ptGpta)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
	
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	csp_gpta_sw_start(ptGptaBase);
}

/**
 \brief set GPTA operation mode
 \param ptGpta    GPTA handle to operate
 \param eMode 	 GPTA_OP_CONT/GPTA_OP_OT
*/
void csi_gpta_set_os_mode(csi_gpta_t *ptGpta, csi_gpta_opmd_e eMode)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
	
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	csp_gpta_set_opmd(ptGptaBase, eMode);
}

/**
 \brief set GPTA stop status
 \param ptGpt    GPTA handle to operate
 \param eSt 	 GPTA_STP_HZ/GPTA_STP_LOW
*/
void csi_gpta_set_stop_st(csi_gpta_t *ptGpta, csi_gpta_stopst_e eSt)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
	
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	csp_gpta_set_stop_st(ptGptaBase, eSt);
}

/**
 \brief SW stop GPTA counter
 \param ptGpta    GPTA handle to operate
*/
void csi_gpta_out_swstop(csi_gpta_t *ptGpta)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
	
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	csp_gpta_sw_stop(ptGptaBase);
}


/**
  \brief       Config gpta out mode
  \param[in]   gpta               gpta handle to operate
  \param[in]   channel           channel num
  \param[in]   period_us         PWM period in us
  \param[in]   pulse_width_us    PMW pulse width in us
  \param[in]   polarity          PWM polarity: GPTA_POLARITY_HIGH/GPTA_POLARITY_LOW
  \return      error code 
*/
csi_error_t csi_gpta_singleout_config(csi_gpta_t *ptGpta,
                               uint32_t  channel,
                               uint32_t period_us,
                               uint32_t pulse_width_us,
                               csi_gpta_polarity_t polarity)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
	csi_error_t tRet = CSI_OK;
	
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	tRet = csp_gpta_set_outfreq(ptGptaBase, 1000000/period_us);
	
	return tRet;	
}

/**
 \brief set GPTA frequency(All channels use the same the counter). 
  * \param ptGpta 	GPTA handle to operate
  * \param wHz		frequency in Hertz
  * */
csi_error_t csi_gpta_set_outfreq(csi_gpta_t *ptGpta, uint32_t wHz)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
	
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);

	return (csp_gpta_set_outfreq(ptGptaBase, wHz));
}
/**
 \brief config gpta dual channel output mode. 
  * CHA active High ,ChB active High
  * \verbatim
  *  	CHA      	______
  *  		_______|      |_______ 
  *   	CHB
  *  		_____			_______
  *      		|__________|
  * \endverbatim
  * \param ptGpta 	GPTA handle to operate
  
  * \param wHz	    output frequency
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \param wTns	db time in ns(500 means 500ns)
*/
csi_error_t csi_gpta_set_dualout_md1(csi_gpta_t *ptGpta, uint32_t wDutyCycle, uint32_t wTns)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
    csi_error_t tRet = CSI_OK;
	
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	tRet = csp_gpta_set_dualout(ptGptaBase, wDutyCycle, wTns);
	if (tRet < 0 )
		return CSI_ERROR;
		
	csp_gpta_aqcra_config(ptGptaBase, GPTA_CMPA, GPTA_LO, GPTA_HI, GPTA_NA, GPTA_LO);
	csp_gpta_aqcrb_config(ptGptaBase, GPTA_CMPB, GPTA_HI, GPTA_LO, GPTA_NA, GPTA_HI);
	return tRet;
}

/**
 \brief config gpta dual channel output mode. 
  * CHA active Low ,ChB active Low
  *	\verbatim 
  *  	CHA active Low ,CHB active Low
  *		CHA     ______		 _____
  *				     |_______| 
  *		CHB 	     __________
  *			   _____|         |______	
  * \endverbatim  	
  * \param ptGpta 	GPTA handle to operate
  
  * \param wHz	    output frequency
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \param wTns	db time(500 means 500ns)
  
*/
csi_error_t csi_gpta_set_dualout_md2(csi_gpta_t *ptGpta, uint32_t wDutyCycle, uint32_t wTns)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
    csi_error_t tRet = CSI_OK;
	
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	tRet = csp_gpta_set_dualout(ptGptaBase, wDutyCycle, wTns);
	if (tRet < 0 )
		return CSI_ERROR;
		
	csp_gpta_aqcra_config(ptGptaBase, GPTA_CMPA, GPTA_HI, GPTA_LO, GPTA_NA, GPTA_HI);
	csp_gpta_aqcrb_config(ptGptaBase, GPTA_CMPB, GPTA_LO, GPTA_HI, GPTA_NA, GPTA_LO);
	
	return tRet;
}

/**
 \brief config gpta dual channel output mode. 
  * CHA active High ,ChB active Low
  * \verbatim
  *  	CHA	    	______
  *  		_______|      |_______ 
  *   	CHB	       _________
  *  		______|			|_______
  *    
  * \endverbatim	
  * \param ptGpta 	GPTA handle to operate
  
  * \param wHz	    output frequency
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \param wTns	db time(500 means 500ns)  
*/
csi_error_t csi_gpta_set_dualout_md3(csi_gpta_t *ptGpta, uint32_t wDutyCycle, uint32_t wTns)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
    csi_error_t tRet = CSI_OK;
	
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	tRet = csp_gpta_set_dualout(ptGptaBase, wDutyCycle, wTns);
	if (tRet < 0 )
		return CSI_ERROR;
		
	csp_gpta_aqcra_config(ptGptaBase, GPTA_CMPA, GPTA_LO, GPTA_HI, GPTA_NA, GPTA_LO);
	csp_gpta_aqcrb_config(ptGptaBase, GPTA_CMPB, GPTA_LO, GPTA_HI, GPTA_NA, GPTA_LO);
	return tRet;
}

/**
 \brief config gpta dual channel output mode. 
  * CHA active Low ,ChB active High
  *	\verbatim 
  *		CHA	    ______		 _____
  *				     |_______| 
  *		CHB		_____          ____
  *			        |_________|	
  * \endverbatim 	
  * \param ptGpta 	GPTA handle to operate
  
  * \param wHz	    output frequency
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \param wTns	db time(500 means 500ns)
  
*/
csi_error_t csi_gpta_set_dualout_md4(csi_gpta_t *ptGpta, uint32_t wDutyCycle, uint32_t wTns)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
    csi_error_t tRet = CSI_OK;
	
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	tRet = csp_gpta_set_dualout(ptGptaBase, wDutyCycle, wTns);
	csi_gpta_get_prd(ptGpta);
	if (tRet < 0 )
		return CSI_ERROR;
		
	csp_gpta_aqcra_config(ptGptaBase, GPTA_CMPA, GPTA_HI, GPTA_LO, GPTA_NA, GPTA_HI);
	csp_gpta_aqcrb_config(ptGptaBase, GPTA_CMPB, GPTA_HI, GPTA_LO, GPTA_NA, GPTA_HI);
	
	return tRet;
}

/**
 \brief config gpta single output mode1

  * \verbatim
  *  	CHx	    	______
  *  		_______|      |_______ 
  * \endverbatim	
  * \param ptGpta 	GPTA handle to operate
  
  * \param wDutyCycle output dutycycle (40 means 40%)
  * \return tRet   
*/

csi_error_t csi_gpta_set_singleout_md1(csi_gpta_t *ptGpta, csi_gpta_chtype_e eCh, uint32_t wDutyCycle)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
    csi_error_t tRet = CSI_OK;
	
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	
	tRet = csp_gpta_set_ch_singleout(ptGptaBase, eCh, wDutyCycle);
	switch(eCh)
	{
		case (GPTA_CHa):
			csp_gpta_aqcra_config(ptGptaBase, GPTA_CMPA, GPTA_LO, GPTA_HI, GPTA_NA, GPTA_LO);
			break;
		case (GPTA_CHb):
			csp_gpta_aqcrb_config(ptGptaBase, GPTA_CMPB, GPTA_LO, GPTA_HI, GPTA_NA, GPTA_LO);
			break;
		default:
			tRet = CSP_FAIL;
			break;
	}
		
	return tRet;
}

/**
 \brief config gpta single output mode2

  * \verbatim
  *		CHx     ______		  _____
  *				     |_______|
  * \endverbatim	
  * \param ptGpta 	GPTA handle to operate  
  * \param wDutyCycle output dutycycle in % (40 means 40%)
  * \return tRet   
*/
csi_error_t csi_gpta_set_singleout_md2(csi_gpta_t *ptGpta, csi_gpta_chtype_e eCh, uint32_t wDutyCycle)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
    csi_error_t tRet = CSI_OK;
	
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	
	tRet = csp_gpta_set_ch_singleout(ptGptaBase, eCh, wDutyCycle);
	switch(eCh)
	{
		case (GPTA_CHa):
			csp_gpta_aqcra_config(ptGptaBase, GPTA_CMPA, GPTA_HI, GPTA_LO, GPTA_NA, GPTA_HI);
			break;
		case (GPTA_CHb):
			csp_gpta_aqcrb_config(ptGptaBase, GPTA_CMPB, GPTA_HI, GPTA_LO, GPTA_NA, GPTA_HI);
			break;
		default:
			tRet = CSP_FAIL;
			break;
	}
		
	return tRet;
}

/**
 \brief get counter period to calculate the duty cycle. 
 \param ptGpta    gpta handle to operate
 \return counter period (reg data)
*/
uint16_t csi_gpta_get_prd(csi_gpta_t *ptGpta)
{
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	return csp_gpta_get_prd(ptGptaBase);
}

/**
 \brief change gpta output dutycycle. 
 \param ptGpta    gpta handle to operate
 \param wActiveTime cmpx data to be set directly
*/
csi_error_t csi_gpta_change_ch_duty(csi_gpta_t *ptGpta, csi_gpta_chtype_e eCh, uint32_t wActiveTime)
{
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	
	return (csp_gpta_set_ch_duty(ptGptaBase, eCh, wActiveTime));
}


/**
  \brief       Config gpta width capture mode
  \param[in]   ptGpta    gpta handle to operate
  \param[in]   eMode     GPTA_CAP_CONT /  GPTA_CAP_OT
  \return      Tick Time in ns 
*/
uint32_t csi_gpta_cap_width_config(csi_gpta_t *ptGpta, csi_gpta_capmd_e eMode, uint8_t byCapWrap)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
    uint32_t wTick;
	
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	
	
	csp_gpta_set_cntmd(ptGptaBase, GPTA_CNTMD_UP);
	csp_gpta_set_mode(ptGptaBase, GPTA_MODE_CAP);
	csp_gpta_set_capmd(ptGptaBase, eMode);
	csp_gpta_capld_enable(ptGptaBase, ENABLE);	
	//csp_gpta_capld_enable(ptGptaBase, DISABLE);	
	//support 2 load for now
	csp_gpta_set_stopwrap(ptGptaBase, byCapWrap-1);
	csp_gpta_set_start_src(ptGptaBase, GPTA_SYNC_START);
	//csp_gpta_set_start_src(ptGptaBase, GPTA_SW_START);
	csp_gpta_set_sync_enable(ptGptaBase, 0, ENABLE);	// enable SYNCIN0
	csp_gpta_set_sync_enable(ptGptaBase, 2, ENABLE);	// enable SYNCIN2
	csp_gpta_set_prdr(ptGptaBase, 0xffff);
	csp_gpta_ldrst_enable(ptGptaBase, 0, ENABLE);
	csp_gpta_ldrst_enable(ptGptaBase, 1, ENABLE);

	csp_gpta_set_cmp_ldmd(ptGptaBase, 0, GPTA_CMPLD_SHDW, GPTA_LDCMP_NEVER);
	csp_gpta_set_cmp_ldmd(ptGptaBase, 1, GPTA_CMPLD_SHDW, GPTA_LDCMP_NEVER);
	wTick = csp_gpta_get_cap_tick(ptGptaBase);
	
	return wTick;

}

/**
  \brief       Config gpta period capture mode
  \param[in]   ptGpta    gpta handle to operate
  \param[in]   eMode     GPTA_CAP_CONT /  GPTA_CAP_OT
  \return      Tick Time in ns 
*/
uint32_t csi_gpta_cap_prd_config(csi_gpta_t *ptGpta, csi_gpta_capmd_e eMode, uint8_t byCapWrap)
{
	CSI_PARAM_CHK(ptGpta, CSI_ERROR);
    uint32_t wTick;
	
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	
	
	csp_gpta_set_cntmd(ptGptaBase, GPTA_CNTMD_UP);
	csp_gpta_set_mode(ptGptaBase, GPTA_MODE_CAP);
	csp_gpta_set_capmd(ptGptaBase, eMode);
	csp_gpta_capld_enable(ptGptaBase, ENABLE);	
	//csp_gpta_capld_enable(ptGptaBase, DISABLE);	
	//support 2 load for now
	csp_gpta_set_stopwrap(ptGptaBase, byCapWrap-1);
	//csp_gpta_set_start_src(ptGptaBase, GPTA_SYNC_START);
	csp_gpta_set_start_src(ptGptaBase, GPTA_SW_START);
	//csp_gpta_set_sync_enable(ptGptaBase, 0, ENABLE);	// enable SYNCIN0
	csp_gpta_set_sync_enable(ptGptaBase, 2, ENABLE);	// enable SYNCIN2
	csp_gpta_set_prdr(ptGptaBase, 0xffff);
	csp_gpta_ldrst_enable(ptGptaBase, 0, ENABLE);
	csp_gpta_ldrst_enable(ptGptaBase, 1, ENABLE);

	csp_gpta_set_cmp_ldmd(ptGptaBase, 0, GPTA_CMPLD_SHDW, GPTA_LDCMP_NEVER);
	csp_gpta_set_cmp_ldmd(ptGptaBase, 1, GPTA_CMPLD_SHDW, GPTA_LDCMP_NEVER);
	wTick = csp_gpta_get_cap_tick(ptGptaBase);
	
	return wTick;

}

/**
  \brief       get gpta capture data
  \param[in]   ptGpta         gpta handle to operate
  \param[in]   num          load sequence
  \return      hwData		capture data 
*/
uint16_t csi_gpta_capture_get_cap_data(csi_gpta_t *ptGpta, uint8_t num)
{
	uint16_t hwData;
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	switch (num) 
	{
		case (0): hwData = csp_gpta_get_cmpa(ptGptaBase);
			break;
		case (1): hwData = csp_gpta_get_cmpb(ptGptaBase);
			break;
		default :
			return CSI_ERROR;
			break;
	}
	return hwData;
}

/**
  \brief       enable/disable gpta in debug mode
  \param[in]   ptGpta       gpta handle to operate
  \param[in]   bEnable		ENABLE/DISABLE
*/

void csi_gpta_debug_enable(csi_gpta_t *ptGpta, bool bEnable)
{
	csp_gpta_t *ptGptaBase = (csp_gpta_t *)HANDLE_REG_BASE(ptGpta);
	
	csp_gpta_dbg_enable(ptGptaBase, bEnable);
}