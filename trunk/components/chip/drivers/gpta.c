/***********************************************************************//** 
 * \file  gpta.c
 * \brief  Enhanced purpose timer driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-6-17 <td>V0.0  <td>ljy   <td>initial
 * </table>
 * *********************************************************************
*/
#include "sys_clk.h"
#include "drv/common.h"
#include "drv/gpta.h"
#include "csp_gpta.h"
#include "drv/pin.h"
#include <drv/irq.h>

uint32_t gGptaPrd;

 /**
 \brief  Basic configuration
 \param  ptGptaBase    	    pointer of ept register structure
 \param  ptGptaPwmCfg   	refer to csi_gpta_config_t
 \return CSI_OK/CSI_ERROR
*/
csi_error_t csi_gpta_config_init(csp_gpta_t *ptGptaBase, csi_gpta_config_t *ptGptaPwmCfg)
{
	uint32_t wClkDiv;
	uint32_t wCrVal;
	uint32_t wCmpLoad; 
	uint32_t wPrdrLoad; 
	
	if(ptGptaPwmCfg->wFreq == 0 ){ptGptaPwmCfg->wFreq =100;}
	
	
	csi_clk_enable((uint32_t *)ptGptaBase);								// clk enable
	
	csp_gpta_clken(ptGptaBase);
	csp_gpta_wr_key(ptGptaBase);                                        //Unlocking
	csp_gpta_reset(ptGptaBase);											// reset 
	
	wClkDiv = (csi_get_pclk_freq() / ptGptaPwmCfg->wFreq / 60000);		// clk div value
	if(wClkDiv == 0)wClkDiv = 1;
	
	wPrdrLoad  = (csi_get_pclk_freq()/ptGptaPwmCfg->wFreq/wClkDiv);	    //prdr load value
			
	wCrVal =ptGptaPwmCfg->byCountingMode | (ptGptaPwmCfg->byStartSrc<<GPTA_STARTSRC_POS) |
	        ptGptaPwmCfg->byOneshotMode<<GPTA_OPMD_POS | (ptGptaPwmCfg->byWorkmod<<GPTA_MODE_POS);
    
	wCrVal=(wCrVal & ~(GPTA_PSCLD_MSK))   |((ptGptaPwmCfg->byPscld&0x03)   <<GPTA_PSCLD_POS);//
	
	if(ptGptaPwmCfg->byWorkmod==GPTA_CAPTURE)
	{
	 	wCrVal=(wCrVal & ~(GPTA_CAPMD_MSK))   |((ptGptaPwmCfg->byCaptureCapmd&0x01)   <<GPTA_CAPMD_POS);
		wCrVal=(wCrVal & ~(GPTA_STOPWRAP_MSK))|((ptGptaPwmCfg->byCaptureStopWrap&0x03)<<GPTA_STOPWRAP_POS);
		wCrVal=(wCrVal & ~(GPTA_CMPA_RST_MSK))|((ptGptaPwmCfg->byCaptureLdaret&0x01)  <<GPTA_CMPA_RST_POS);
		wCrVal=(wCrVal & ~(GPTA_CMPB_RST_MSK))|((ptGptaPwmCfg->byCaptureLdbret&0x01)  <<GPTA_CMPB_RST_POS);
		wCrVal=(wCrVal & ~(GPTA_CMPC_RST_MSK))|((ptGptaPwmCfg->byCaptureLdcret&0x01)  <<GPTA_CMPC_RST_POS);
		wCrVal=(wCrVal & ~(GPTA_CMPD_RST_MSK))|((ptGptaPwmCfg->byCaptureLddret&0x01)  <<GPTA_CMPD_RST_POS);
		
		if(ptGptaPwmCfg->byCaptureCapLden)wCrVal|=GPTA_CAPLD_EN;
		if(ptGptaPwmCfg->byCaptureRearm)  wCrVal|=GPTA_CAPREARM;
		
		wPrdrLoad=0xFFFF;
	}
	
	if(ptGptaPwmCfg->byBurst)
	{
		wCrVal=(wCrVal & ~(GPTA_CGSRC_MSK))|((ptGptaPwmCfg->byCgsrc&0x03)<<GPTA_CGSRC_POS);
		wCrVal=(wCrVal & ~(GPTA_CGFLT_MSK))|((ptGptaPwmCfg->byCgflt&0x07)<<GPTA_CGFLT_POS);
	}
	
    csp_gpta_clken(ptGptaBase);                                             // clkEN
	csp_gpta_set_cr(ptGptaBase, wCrVal);									// set bt work mode
	csp_gpta_set_pscr(ptGptaBase, (uint16_t)wClkDiv - 1);					// clk div
	csp_gpta_set_prdr(ptGptaBase, (uint16_t)wPrdrLoad);				        // prdr load value
		
	if(ptGptaPwmCfg->byDutyCycle){
	wCmpLoad =wPrdrLoad-(wPrdrLoad * ptGptaPwmCfg->byDutyCycle /100);	// cmp load value
	csp_gpta_set_cmpa(ptGptaBase, (uint16_t)wCmpLoad);					// cmp load value
	csp_gpta_set_cmpb(ptGptaBase, (uint16_t)wCmpLoad);
//	csp_gpta_set_cmpc(ptGptaBase, (uint16_t)wCmpLoad);
//	csp_gpta_set_cmpd(ptGptaBase, (uint16_t)wCmpLoad);
	}
	
	if(ptGptaPwmCfg->byInter)
	{
		csp_gpta_int_enable(ptGptaBase, ptGptaPwmCfg->byInter, true);		//enable interrupt
		csi_irq_enable((uint32_t *)ptGptaBase);							//enable  irq
	}
	
	gGptaPrd=wPrdrLoad;
	
	return CSI_OK;
}

 /**
 \brief  capture configuration
 \param  ptGptaBase    	    pointer of ept register structure
 \param  ptGptaPwmCfg   	refer to csi_gpta_captureconfig_t
 \return CSI_OK /CSI_ERROR
*/
csi_error_t csi_gpta_capture_init(csp_gpta_t *ptGptaBase, csi_gpta_captureconfig_t *ptGptaPwmCfg)
{
	uint32_t wClkDiv=0;
	uint32_t wCrVal;
	uint32_t wPrdrLoad; 
			
	csi_clk_enable((uint32_t *)ptGptaBase);								 // clk enable	
	csp_gpta_clken(ptGptaBase);
	csp_gpta_wr_key(ptGptaBase);                                           //Unlocking
	csp_gpta_reset(ptGptaBase);											 // reset 
	
			                                                             // clk div value
	if(wClkDiv == 0){wClkDiv = 1;}
					
	wCrVal =ptGptaPwmCfg->byCountingMode | (ptGptaPwmCfg->byStartSrc<<GPTA_STARTSRC_POS) |
	        ptGptaPwmCfg->byOneshotMode<<GPTA_OPMD_POS | (ptGptaPwmCfg->byWorkmod<<GPTA_MODE_POS);
    
	wCrVal=(wCrVal & ~(GPTA_PSCLD_MSK))   |((ptGptaPwmCfg->byPscld&0x03)   <<GPTA_PSCLD_POS);
	
	wCrVal=(wCrVal & ~(GPTA_CAPMD_MSK))   |((ptGptaPwmCfg->byCaptureCapmd&0x01)   <<GPTA_CAPMD_POS);
	wCrVal=(wCrVal & ~(GPTA_STOPWRAP_MSK))|((ptGptaPwmCfg->byCaptureStopWrap&0x03)<<GPTA_STOPWRAP_POS);
	wCrVal=(wCrVal & ~(GPTA_CMPA_RST_MSK))|((ptGptaPwmCfg->byCaptureLdaret&0x01)  <<GPTA_CMPA_RST_POS);
	wCrVal=(wCrVal & ~(GPTA_CMPB_RST_MSK))|((ptGptaPwmCfg->byCaptureLdbret&0x01)  <<GPTA_CMPB_RST_POS);
	wCrVal=(wCrVal & ~(GPTA_CMPC_RST_MSK))|((ptGptaPwmCfg->byCaptureLdcret&0x01)  <<GPTA_CMPC_RST_POS);
	wCrVal=(wCrVal & ~(GPTA_CMPD_RST_MSK))|((ptGptaPwmCfg->byCaptureLddret&0x01)  <<GPTA_CMPD_RST_POS);
	
	wCrVal|=GPTA_CAPLD_EN;
	wCrVal|=GPTA_CAPREARM;
	wPrdrLoad=0xFFFF;

    csp_gpta_clken(ptGptaBase);                                             // clkEN
	csp_gpta_set_cr(ptGptaBase, wCrVal);									// set bt work mode
	csp_gpta_set_pscr(ptGptaBase, (uint16_t)wClkDiv - 1);					// clk div
	csp_gpta_set_prdr(ptGptaBase, (uint16_t)wPrdrLoad);				        // prdr load value
	
	if(ptGptaPwmCfg->byInter)
	{
		csp_gpta_int_enable(ptGptaBase, ptGptaPwmCfg->byInter, true);		//enable interrupt
		csi_irq_enable((uint32_t *)ptGptaBase);							//enable  irq
	}
	
	gGptaPrd=wPrdrLoad;
	
	return CSI_OK;
}

 /**
 \brief  wave configuration
 \param  ptGptaBase    	    pointer of ept register structure
 \param  ptGptaPwmCfg   	refer to csi_gpta_pwmconfig_t
 \return CSI_OK /CSI_ERROR
*/
csi_error_t  csi_gpta_wave_init(csp_gpta_t *ptGptaBase, csi_gpta_pwmconfig_t *ptGptaPwmCfg)
{
    uint32_t wClkDiv;
	uint32_t wCrVal;
	uint32_t wCmpLoad; 
	uint32_t wPrdrLoad; 
	
	if(ptGptaPwmCfg->wFreq == 0 ){return CSI_ERROR;}
		
	csi_clk_enable((uint32_t *)ptGptaBase);								// clk enable
	
	csp_gpta_clken(ptGptaBase);
	csp_gpta_wr_key(ptGptaBase);                                           //Unlocking
	csp_gpta_reset(ptGptaBase);											// reset 
	
	wClkDiv = (csi_get_pclk_freq() / ptGptaPwmCfg->wFreq / 60000);		// clk div value
	if(wClkDiv == 0)wClkDiv = 1;
	
	wPrdrLoad  = (csi_get_pclk_freq()/ptGptaPwmCfg->wFreq/wClkDiv);	    //prdr load value
		
	wCrVal =ptGptaPwmCfg->byCountingMode | (ptGptaPwmCfg->byStartSrc<<GPTA_STARTSRC_POS) |
	        ptGptaPwmCfg->byOneshotMode<<GPTA_OPMD_POS | (ptGptaPwmCfg->byWorkmod<<GPTA_MODE_POS);
    
	wCrVal=(wCrVal & ~(GPTA_PSCLD_MSK))   |((ptGptaPwmCfg->byPscld&0x03)   <<GPTA_PSCLD_POS);
		
    csp_gpta_clken(ptGptaBase);                                           // clkEN
	csp_gpta_set_cr(ptGptaBase, wCrVal);									// set bt work mode
	csp_gpta_set_pscr(ptGptaBase, (uint16_t)wClkDiv - 1);					// clk div
	csp_gpta_set_prdr(ptGptaBase, (uint16_t)wPrdrLoad);				    // prdr load value
		
	if(ptGptaPwmCfg->byDutyCycle){
	wCmpLoad =wPrdrLoad-(wPrdrLoad * ptGptaPwmCfg->byDutyCycle /100);	// cmp load value
	csp_gpta_set_cmpa(ptGptaBase, (uint16_t)wCmpLoad);					// cmp load value
	csp_gpta_set_cmpb(ptGptaBase, (uint16_t)wCmpLoad);
//	csp_gpta_set_cmpc(ptGptaBase, (uint16_t)wCmpLoad);
//	csp_gpta_set_cmpd(ptGptaBase, (uint16_t)wCmpLoad);
	}
	
	if(ptGptaPwmCfg->byInter)
	{
		csp_gpta_int_enable(ptGptaBase, ptGptaPwmCfg->byInter, true);		//enable interrupt
		csi_irq_enable((uint32_t *)ptGptaBase);							    //enable  irq
	}
	
	gGptaPrd=wPrdrLoad;
	
	return CSI_OK;	
}



 /**
 \brief  Channel configuration
 \param  ptGptaBase    	pointer of ept register structure
 \param  tPwmCfg   	    refer to csi_gpta_pwmchannel_config_t
 \param  channel        Channel label
 \return CSI_OK /CSI_ERROR
*/
csi_error_t csi_gpta_channel_config(csp_gpta_t *ptGptaBase, csi_gpta_pwmchannel_config_t *tPwmCfg, csi_gpta_channel_e channel)
{
    uint32_t w_AQCRx_Val;
	
	w_AQCRx_Val=  tPwmCfg -> byActionZro 
	              | ( tPwmCfg -> byActionPrd  << GPTA_ACT_PRD_POS  )
				  | ( tPwmCfg -> byActionCau  << GPTA_ACT_CAU_POS  )
				  | ( tPwmCfg -> byActionCad  << GPTA_ACT_CAD_POS  )
				  | ( tPwmCfg -> byActionCbu  << GPTA_ACT_CBU_POS  )
				  | ( tPwmCfg -> byActionCbd  << GPTA_ACT_CBD_POS  )
				  | ( tPwmCfg -> byActionT1u  << GPTA_ACT_T1U_POS  )
				  | ( tPwmCfg -> byActionT1d  << GPTA_ACT_T1D_POS  )
				  | ( tPwmCfg -> byActionT2u  << GPTA_ACT_T2U_POS  )
				  | ( tPwmCfg -> byActionT2d  << GPTA_ACT_T2D_POS  )
				  | ( tPwmCfg -> byChoiceCasel  << GPTA_CASEL_POS  )
				  | ( tPwmCfg -> byChoiceCbsel  << GPTA_CBSEL_POS  );
				  
	switch (channel)
	{	
		case (GPTA_CHANNEL_A):csp_gpta_set_aqcra(ptGptaBase,w_AQCRx_Val);
			break;
		case (GPTA_CHANNEL_B):csp_gpta_set_aqcrb(ptGptaBase,w_AQCRx_Val);
			break;
//		case (GPTA_CHANNEL_C):csp_gpta_set_aqcrc(ptGptaBase,w_AQCRx_Val);
//            break;
//		case (GPTA_CHANNEL_D):csp_gpta_set_aqcrd(ptGptaBase,w_AQCRx_Val);
//		    break;
		default:return CSI_ERROR;
			break;
	}
	return CSI_OK;
}


/**
 \brief  gpta configuration Loading
 \param  ptGptaBase    	pointer of ept register structure
 \param  tCfg           refer to csi_gpta_Global_load_control_config_t
 \return CSI_OK /CSI_ERROR
*/
csi_error_t csi_gpta_global_config(csp_gpta_t *ptGptaBase,csi_gpta_Global_load_control_config_t *Global)
{   uint32_t w_GLDCR;	
	w_GLDCR =0;
    w_GLDCR = (w_GLDCR &~GPTA_GLDEN_MSK)       | ((Global->bGlden & 0x01)<<GPTA_GLDEN_POS);
	w_GLDCR = (w_GLDCR &~GPTA_GLDMD_MSK)       | ((Global->byGldmd & 0x0f)<<GPTA_GLDMD_POS);
	w_GLDCR = (w_GLDCR &~GPTA_GLDCR_OSTMD_MSK) | ((Global->bOstmd & 0x01)<<GPTA_GLDCR_OSTMD_POS);
	w_GLDCR = (w_GLDCR &~GPTA_GLDPRD_MSK)      | ((Global->bGldprd & 0x07)<<GPTA_GLDPRD_POS);
	csp_gpta_set_gldcr(ptGptaBase,w_GLDCR);	
	return CSI_OK;
}
/**
 \brief  Software trigger loading
 \param  ptGptaBase    	pointer of ept register structure
 \return CSI_OK
*/
csi_error_t csi_gpta_global_sw(csp_gpta_t *ptGptaBase)
{
	csp_gpta_set_gldcr2(ptGptaBase,0x01);
	return CSI_OK;
}
/**
 \brief  rearm  loading
 \param  ptGptaBase    	pointer of ept register structure
 \return CSI_OK
*/
csi_error_t csi_gpta_global_rearm(csp_gpta_t *ptGptaBase)
{
	csp_gpta_set_gldcr2(ptGptaBase,0x02);
	return CSI_OK;
}
/** \brief start gpta
 *  \param ptgptaBase:  pointer of bt register structure
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_gpta_start(csp_gpta_t *ptgptaBase)
{   csp_gpta_wr_key(ptgptaBase); 
	csp_gpta_start(ptgptaBase);
	return CSI_OK;
}
/**
 \brief SW stop GPTA counter
 \param ptgptaBase    pointer of ept register structure
*/
void csi_gpta_swstop(csp_gpta_t *ptgptaBase)
{
  csp_gpta_wr_key(ptgptaBase);
  csp_gpta_stop(ptgptaBase);
}
/**
 \brief set GPTA start mode. 
 \param ptgptaBase    pointer of ept register structure
 \return eMode ：GPTA_SW/GPTA_SYNC
*/
void csi_gpta_set_start_mode(csp_gpta_t *ptgptaBase, csi_gpta_stmd_e eMode)
{
	csp_gpta_set_start_src(ptgptaBase, eMode);
}
/**
 \brief set GPTA operation mode
 \param ptgptaBase    pointer of ept register structure
 \param eMode 	 GPTA_OP_CONT/GPTA_OP_OT
*/
void csi_gpta_set_os_mode(csp_gpta_t *ptgptaBase, csi_gpta_opmd_e eMode)
{
	csp_gpta_set_opmd(ptgptaBase, eMode);
}

/**
 \brief set GPTA stop status
 \param ptgptaBase    pointer of ept register structure
 \param eSt 	 GPTA_STP_HZ/GPTA_STP_LOW
*/
void csi_gpta_set_stop_st(csp_gpta_t *ptgptaBase, csp_gpta_stpst_e eSt)
{	
  csp_gpta_set_stop_st(ptgptaBase,eSt);
}

/**
 \brief get counter period to calculate the duty cycle. 
 \param ptgptaBase   pointer of ept register structure
 \return counter period (reg data)
*/
uint16_t csi_gpta_get_prdr(csp_gpta_t *ptgptaBase)
{
	return csp_gpta_get_prdr(ptgptaBase);
}

/**
 \brief change gpta output dutycycle. 
 \param ptGptaBase    pointer of ept register structure
 \param eCh           refer to csi_gpta_chtype_e
 \param wActiveTime   cmpx data to be set directly
*/
csi_error_t csi_gpta_change_ch_duty(csp_gpta_t *ptGptaBase, csi_gpta_chtype_e eCh, uint32_t wActiveTime)
{ uint16_t  wCmpLoad;
    wCmpLoad =gGptaPrd-(gGptaPrd * wActiveTime /100);

	switch (eCh)
	{	
		case (GPTA_CH_A):csp_gpta_set_cmpa(ptGptaBase, (uint16_t)wCmpLoad);
			break;
		case (GPTA_CH_B):csp_gpta_set_cmpb(ptGptaBase, (uint16_t)wCmpLoad);
			break;
//		case (GPTA_CH_C):csp_gpta_set_cmpc(ptGptaBase, (uint16_t)wCmpLoad);
//		    break;
//		case (GPTA_CH_D):csp_gpta_set_cmpd(ptGptaBase, (uint16_t)wCmpLoad);
//		    break;
		default: return (1);
			break;
	}
    return (0);
}

/**
  \brief       enable/disable gpta in debug mode
  \param[in]   ptgptaBase   pointer of ept register structure
  \param[in]   bEnable		ENABLE/DISABLE
*/

void csi_gpta_debug_enable(csp_gpta_t *ptGptaBase, bool bEnable)
{
	csp_gpta_dbg_enable(ptGptaBase, bEnable);
}

/**
  \brief   enable/disable gpta out trigger 
  \param   ptGptaBase   pointer of ept register structure
  \param 	eCh			0/1/2/3
  \param   bEnable		ENABLE/DISABLE
*/
csi_error_t csi_gpta_evtrg_enable(csp_gpta_t *ptGptaBase, csi_gpta_trgout_e byCh, bool bEnable)
{	
	if (byCh > 1)return CSI_ERROR;
    csp_gpta_trg_xoe_enable(ptGptaBase, byCh, bEnable);
	return CSI_OK;
}

/**
  \brief   One time software output 
  \param   ptGptaBase      pointer of ept register structure
  \param   byCh	           GPTA_OSTSFA/GPTA_OSTSFB	
  \param   bEnable 		   GPTA_NA/GPTA_LO/GPTA_HI/GPTA_TG
*/
csi_error_t csi_gpta_Onetimesoftware_output(csp_gpta_t *ptGptaBase, uint16_t byCh, csp_gpta_action_e bEnable)
{	
	switch (byCh){
	case GPTA_OSTSFA: ptGptaBase ->AQOSF |= GPTA_OSTSFA;
	                  ptGptaBase ->AQOSF = (ptGptaBase ->AQOSF &~(GPTA_ACTA_MSK))|((bEnable&0x03)<<GPTA_ACTA_POS);
	     break;
	case GPTA_OSTSFB: ptGptaBase ->AQOSF |= GPTA_OSTSFB;
	                  ptGptaBase ->AQOSF = (ptGptaBase ->AQOSF &~(GPTA_ACTB_MSK))|((bEnable&0x03)<<GPTA_ACTB_POS);
	     break;	
	default: return CSI_ERROR;
	     break;
    }
	return CSI_OK;
}
/** \brief  Continuous software waveform loading control
 *  \param[in] ptGptaBase: pointer of ept register structure
 *  \param[in] bEnable:    refer to csp_gpta_aqosf_e
 *  \return  none
 */
void csi_gpta_loading_method_aqcsf(csp_gpta_t *ptGptaBase, csp_gpta_aqosf_e bEnable)
{
	ptGptaBase ->AQOSF  = (ptGptaBase ->AQOSF &~(GPTA_AQCSF_LDTIME_MSK))|((bEnable&0x03)<<GPTA_AQCSF_LDTIME_POS);
}
/** \brief Continuous software waveform control
 *  \param[in] ptGptaBase: pointer of ept register structure
 *  \param[in] byCh        refer to csi_gpta_channel_e
 *  \param[in] bEnable:    refer to  csp_gpta_aqosf_e
 *  \return  none
 */
csi_error_t csi_gpta_continuous_software_waveform(csp_gpta_t *ptGptaBase, csi_gpta_channel_e byCh, csp_gpta_aqcsf_e bEnable)
{
	
	switch (byCh){
	case GPTA_CHANNEL_A:  ptGptaBase ->AQCSF = (ptGptaBase ->AQCSF &~(0x03))|(bEnable&0x03);            
	     break;
	case GPTA_CHANNEL_B:  ptGptaBase ->AQCSF = (ptGptaBase ->AQCSF &~(0x0c))|(bEnable&0x03)<<2;
	     break;	
//    case GPTA_CHANNEL_C:  ptGptaBase ->AQCSF = (ptGptaBase ->AQCSF &~(0x30))|(bEnable&0x03)<<4;
//	     break;
//	case GPTA_CHANNEL_D:  ptGptaBase ->AQCSF = (ptGptaBase ->AQCSF &~(0xc0))|(bEnable&0x03)<<6;
//	     break;
	default: return CSI_ERROR;
	     break;
    }		
	return CSI_OK;
}

/** \brief gpta  input  config  
 *  \param[in] ptGptaBase:pointer of ept register structure
 *  \param[in] eInt:     refer to to csp_gpta_int_e
 *  \param[in] bEnable:  ENABLE/DISABLE
 *  \return CSI_OK;
 */
csi_error_t csi_gpta_int_enable(csp_gpta_t *ptGptaBase, csp_gpta_int_e eInt, bool bEnable)
{  
	csp_gpta_int_enable(ptGptaBase,eInt,bEnable);
	return CSI_OK;
}

/** \brief gpta sync input evtrg config  
 * 
 *  \param[in] ptGptaBase:pointer of ept register structure
 *  \param[in] eTrgin: gpta sync evtrg input channel(0~5)
 *  \param[in] eTrgMode: gpta sync evtrg mode, continuous/once
 *  \param[in] eAutoRearm: refer to csi_gpta_arearm_e 
 *  \return none
 */
void csi_gpta_set_sync(csp_gpta_t *ptGptaBase, csi_gpta_trgin_e eTrgIn, csi_gpta_trgmode_e eTrgMode, csi_gpta_arearm_e eAutoRearm)
{
	csp_gpta_set_sync_mode(ptGptaBase, eTrgIn, eTrgMode);
	csp_gpta_set_auto_rearm(ptGptaBase, eAutoRearm);
	csp_gpta_sync_enable(ptGptaBase, eTrgIn, ENABLE);
}

/** \brief gpta extsync input select
 * 
 *  \param[in] ptGptaBase:pointer of ept register structure
 *  \param[in] eTrgin: gpta sync evtrg input channel(0~5)
 *  \param[in] byTrgChx: trgxsel channel(0~1)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_gpta_set_extsync_chnl(csp_gpta_t *ptGptaBase, csi_gpta_trgin_e eTrgIn, uint8_t byTrgChx)
{
	switch(byTrgChx)
	{
		case 0:
			csp_gpta_sync_trg0sel(ptGptaBase, eTrgIn);
			break;
		case 1:
			csp_gpta_sync_trg1sel(ptGptaBase, eTrgIn);
			break;
		default:
			return CSI_ERROR;
		
	}
	return CSI_OK;
}

/** \brief gpta sync input filter config  
 * 
 *  \param[in] ptGptaBase: pointer of gpta register structure
 *  \param[in] ptFilter: pointer of sync input filter parameter config structure
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_gpta_set_sync_filter(csp_gpta_t *ptGptaBase, csi_gpta_filter_config_t *ptFilter)
{
	uint32_t wFiltCr;
	uint32_t wWindow;
	
	if(ptFilter->byFiltSrc > GPTA_FILT_SYNCIN5)
		return CSI_ERROR;
	wFiltCr = ptFilter->byFiltSrc | (ptFilter->byWinInv << GPTA_FLTBLKINV_POS) | 
			(ptFilter->byWinAlign << GPTA_ALIGNMD_POS) | (ptFilter->byWinCross << GPTA_CROSSMD_POS);
	wWindow = ptFilter->byWinOffset | (ptFilter->byWinWidth << GPTA_FLT_WDW_POS);
	
	csp_gpta_set_trgftcr(ptGptaBase, wFiltCr);
	csp_gpta_set_trgftwr(ptGptaBase, wWindow);
	
	return CSI_OK;
}
/** \brief rearm gpta sync evtrg  
 * 
 *  \param[in] ptGptaBase: pointer of gpta register structure
 *  \param[in] eTrgin: gpta sync evtrg input channel(0~5)
 *  \return none
 */
void csi_gpta_rearm_sync(csp_gpta_t *ptGptaBase,csi_gpta_trgin_e eTrgin)
{
	csp_gpta_rearm_sync(ptGptaBase, eTrgin);
}
/** \brief gpta evtrg output config
 * 
 *  \param[in] ptGptaBase: pointer of gpta register structure
 *  \param[in] byTrgOut: evtrg out port(0~1)
 *  \param[in] eTrgSrc: evtrg source(1~15) 
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_gpta_set_evtrg(csp_gpta_t *ptGptaBase, csi_gpta_trgout_e byTrgOut, csp_gpta_trgsrc0_e eTrgSrc)
{
	switch (byTrgOut)
	{
		case GPTA_TRG_OUT0:
		        if(eTrgSrc == GPTA_TRG01_DIS)								
				{
					csp_gpta_trg_xoe_enable(ptGptaBase, byTrgOut, DISABLE);	//disable evtrg source out
					return CSI_OK;
				}
				csp_gpta_set_trgsrc01(ptGptaBase, byTrgOut, eTrgSrc);
			break;
		
		case GPTA_TRG_OUT1: 
				if(eTrgSrc == GPTA_TRG01_DIS)								
				{
					csp_gpta_trg_xoe_enable(ptGptaBase, byTrgOut, DISABLE);	//disable evtrg source out
					return CSI_OK;
				}
				csp_gpta_set_trgsrc01(ptGptaBase, byTrgOut, eTrgSrc);
			break;
		default: 
			return CSI_ERROR;
	}
	
	csp_gpta_trg_xoe_enable(ptGptaBase, byTrgOut, ENABLE);				//evtrg out enable
	
	return CSI_OK;
}


/** \brief gpta evtrg cntxinit control
 * 
 *  \param[in] ptGptaBase: pointer of gpta register structure
 *  \param[in] byCntChx: evtrg countinit channel(0~3)
 *  \param[in] byCntVal: evtrg cnt value
 *  \param[in] byCntInitVal: evtrg cntxinit value
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_gpta_set_evcntinit(csp_gpta_t *ptGptaBase, uint8_t byCntChx, uint8_t byCntVal, uint8_t byCntInitVal)
{

 if(byCntChx > GPTA_CNT1INIT)
  return CSI_ERROR;
 
 csp_gpta_set_trgprd(ptGptaBase, byCntChx, byCntVal - 1);    //evtrg count
 csp_gpta_trg_cntxinit(ptGptaBase, byCntChx, byCntInitVal);
 csp_gpta_trg_cntxiniten_enable(ptGptaBase, byCntChx, ENABLE);
 
 return CSI_OK;
}

