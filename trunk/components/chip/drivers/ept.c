/***********************************************************************//** 
 * \file  ept.c
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
#include "drv/ept.h"
#include "csp_ept.h"
#include "drv/pin.h"
#include <drv/irq.h>

uint32_t gEptPrd;

 /**
 \brief  Basic configuration
 \param  ptEptBase    	pointer of ept register structure
 \param  pteptPwmCfg   	refer to csi_ept_config_t
 \return CSI_OK/CSI_ERROR
*/
csi_error_t csi_ept_config_init(csp_ept_t *ptEptBase, csi_ept_config_t *pteptPwmCfg)
{
	uint32_t wClkDiv;
	uint32_t wCrVal;
	uint32_t wCmpLoad; 
	uint32_t wPrdrLoad; 
	
	if(pteptPwmCfg->wFreq == 0 ){pteptPwmCfg->wFreq =100;}
	
	
	csi_clk_enable((uint32_t *)ptEptBase);								// clk enable
	
	csp_ept_clken(ptEptBase);
	csp_ept_wr_key(ptEptBase);                                           //Unlocking
	csp_ept_reset(ptEptBase);											// reset 
	
	wClkDiv = (csi_get_pclk_freq() / pteptPwmCfg->wFreq / 60000);		// clk div value
	if(wClkDiv == 0)wClkDiv = 1;
	
	wPrdrLoad  = (csi_get_pclk_freq()/pteptPwmCfg->wFreq/wClkDiv);	    //prdr load value
			
	wCrVal =pteptPwmCfg->byCountingMode | (pteptPwmCfg->byStartSrc<<EPT_STARTSRC_POS) |
	        pteptPwmCfg->byOneshotMode<<EPT_OPMD_POS | (pteptPwmCfg->byWorkmod<<EPT_MODE_POS);
    
	wCrVal=(wCrVal & ~(EPT_PSCLD_MSK))   |((pteptPwmCfg->byPscld&0x03)   <<EPT_PSCLD_POS);
	
	if(pteptPwmCfg->byWorkmod==EPT_CAPTURE)
	{
	 	wCrVal=(wCrVal & ~(EPT_CAPMD_MSK))   |((pteptPwmCfg->byCaptureCapmd&0x01)   <<EPT_CAPMD_POS);
		wCrVal=(wCrVal & ~(EPT_STOPWRAP_MSK))|((pteptPwmCfg->byCaptureStopWrap&0x03)<<EPT_STOPWRAP_POS);
		wCrVal=(wCrVal & ~(EPT_CMPA_RST_MSK))|((pteptPwmCfg->byCaptureLdaret&0x01)  <<EPT_CMPA_RST_POS);
		wCrVal=(wCrVal & ~(EPT_CMPB_RST_MSK))|((pteptPwmCfg->byCaptureLdbret&0x01)  <<EPT_CMPB_RST_POS);
		wCrVal=(wCrVal & ~(EPT_CMPC_RST_MSK))|((pteptPwmCfg->byCaptureLdcret&0x01)  <<EPT_CMPC_RST_POS);
		wCrVal=(wCrVal & ~(EPT_CMPD_RST_MSK))|((pteptPwmCfg->byCaptureLddret&0x01)  <<EPT_CMPD_RST_POS);
		
		if(pteptPwmCfg->byCaptureCapLden)wCrVal|=EPT_CAPLD_EN;
		if(pteptPwmCfg->byCaptureRearm)  wCrVal|=EPT_CAPREARM;
		
		wPrdrLoad=0xFFFF;
	}
	
	if(pteptPwmCfg->byBurst)
	{
		wCrVal=(wCrVal & ~(EPT_CGSRC_MSK))|((pteptPwmCfg->byCgsrc&0x03)<<EPT_CGSRC_POS);
		wCrVal=(wCrVal & ~(EPT_CGFLT_MSK))|((pteptPwmCfg->byCgflt&0x07)<<EPT_CGFLT_POS);
	}
	
    csp_ept_clken(ptEptBase);                                           // clkEN
	csp_ept_set_cr(ptEptBase, wCrVal);									// set bt work mode
	csp_ept_set_pscr(ptEptBase, (uint16_t)wClkDiv - 1);					// clk div
	csp_ept_set_prdr(ptEptBase, (uint16_t)wPrdrLoad);				    // prdr load value
		
	if(pteptPwmCfg->byDutyCycle){
	wCmpLoad =wPrdrLoad-(wPrdrLoad * pteptPwmCfg->byDutyCycle /100);	// cmp load value
	csp_ept_set_cmpa(ptEptBase, (uint16_t)wCmpLoad);					// cmp load value
	csp_ept_set_cmpb(ptEptBase, (uint16_t)wCmpLoad);
	csp_ept_set_cmpc(ptEptBase, (uint16_t)wCmpLoad);
	csp_ept_set_cmpd(ptEptBase, (uint16_t)wCmpLoad);
	}
	
	if(pteptPwmCfg->byInter)
	{
		csp_ept_int_enable(ptEptBase, pteptPwmCfg->byInter, true);		//enable interrupt
		csi_irq_enable((uint32_t *)ptEptBase);							//enable  irq
	}
	
	gEptPrd=wPrdrLoad;
	
	return CSI_OK;
}

 /**
 \brief  capture configuration
 \param  ptEptBase    	pointer of ept register structure
 \param  pteptPwmCfg   	refer to csi_ept_captureconfig_t
 \return CSI_OK /CSI_ERROR
*/
csi_error_t csi_ept_capture_init(csp_ept_t *ptEptBase, csi_ept_captureconfig_t *pteptPwmCfg)
{
	uint32_t wClkDiv=0;
	uint32_t wCrVal;
	uint32_t wPrdrLoad; 
			
	csi_clk_enable((uint32_t *)ptEptBase);								 // clk enable	
	csp_ept_clken(ptEptBase);
	csp_ept_wr_key(ptEptBase);                                           //Unlocking
	csp_ept_reset(ptEptBase);											 // reset 
	
			                                                             // clk div value
	if(wClkDiv == 0){wClkDiv = 1;}
					
	wCrVal =pteptPwmCfg->byCountingMode | (pteptPwmCfg->byStartSrc<<EPT_STARTSRC_POS) |
	        pteptPwmCfg->byOneshotMode<<EPT_OPMD_POS | (pteptPwmCfg->byWorkmod<<EPT_MODE_POS);
    
	wCrVal=(wCrVal & ~(EPT_PSCLD_MSK))   |((pteptPwmCfg->byPscld&0x03)   <<EPT_PSCLD_POS);
	
	wCrVal=(wCrVal & ~(EPT_CAPMD_MSK))   |((pteptPwmCfg->byCaptureCapmd&0x01)   <<EPT_CAPMD_POS);
	wCrVal=(wCrVal & ~(EPT_STOPWRAP_MSK))|((pteptPwmCfg->byCaptureStopWrap&0x03)<<EPT_STOPWRAP_POS);
	wCrVal=(wCrVal & ~(EPT_CMPA_RST_MSK))|((pteptPwmCfg->byCaptureLdaret&0x01)  <<EPT_CMPA_RST_POS);
	wCrVal=(wCrVal & ~(EPT_CMPB_RST_MSK))|((pteptPwmCfg->byCaptureLdbret&0x01)  <<EPT_CMPB_RST_POS);
	wCrVal=(wCrVal & ~(EPT_CMPC_RST_MSK))|((pteptPwmCfg->byCaptureLdcret&0x01)  <<EPT_CMPC_RST_POS);
	wCrVal=(wCrVal & ~(EPT_CMPD_RST_MSK))|((pteptPwmCfg->byCaptureLddret&0x01)  <<EPT_CMPD_RST_POS);
	
	wCrVal|=EPT_CAPLD_EN;
	wCrVal|=EPT_CAPREARM;
	wPrdrLoad=0xFFFF;

    csp_ept_clken(ptEptBase);                                           // clkEN
	csp_ept_set_cr(ptEptBase, wCrVal);									// set bt work mode
	csp_ept_set_pscr(ptEptBase, (uint16_t)wClkDiv - 1);					// clk div
	csp_ept_set_prdr(ptEptBase, (uint16_t)wPrdrLoad);				    // prdr load value
	
	if(pteptPwmCfg->byInter)
	{
		csp_ept_int_enable(ptEptBase, pteptPwmCfg->byInter, true);		//enable interrupt
		csi_irq_enable((uint32_t *)ptEptBase);							//enable  irq
	}
	
	gEptPrd=wPrdrLoad;
	
	return CSI_OK;
}

 /**
 \brief  wave configuration
 \param  ptEptBase    	pointer of ept register structure
 \param  pteptPwmCfg   	refer to csi_ept_pwmconfig_t
 \return CSI_OK /CSI_ERROR
*/
csi_error_t  csi_ept_wave_init(csp_ept_t *ptEptBase, csi_ept_pwmconfig_t *pteptPwmCfg)
{
    uint32_t wClkDiv;
	uint32_t wCrVal;
	uint32_t wCmpLoad; 
	uint32_t wPrdrLoad; 
	
	if(pteptPwmCfg->wFreq == 0 ){return CSI_ERROR;}
		
	csi_clk_enable((uint32_t *)ptEptBase);								// clk enable
	
	csp_ept_clken(ptEptBase);
	csp_ept_wr_key(ptEptBase);                                           //Unlocking
	csp_ept_reset(ptEptBase);											// reset 
	
	wClkDiv = (csi_get_pclk_freq() / pteptPwmCfg->wFreq / 60000);		// clk div value
	if(wClkDiv == 0)wClkDiv = 1;
	
	wPrdrLoad  = (csi_get_pclk_freq()/pteptPwmCfg->wFreq/wClkDiv);	    //prdr load value
		
	wCrVal =pteptPwmCfg->byCountingMode | (pteptPwmCfg->byStartSrc<<EPT_STARTSRC_POS) |
	        pteptPwmCfg->byOneshotMode<<EPT_OPMD_POS | (pteptPwmCfg->byWorkmod<<EPT_MODE_POS);
    
	wCrVal=(wCrVal & ~(EPT_PSCLD_MSK))   |((pteptPwmCfg->byPscld&0x03)   <<EPT_PSCLD_POS);
		
    csp_ept_clken(ptEptBase);                                           // clkEN
	csp_ept_set_cr(ptEptBase, wCrVal);									// set bt work mode
	csp_ept_set_pscr(ptEptBase, (uint16_t)wClkDiv - 1);					// clk div
	csp_ept_set_prdr(ptEptBase, (uint16_t)wPrdrLoad);				    // prdr load value
		
	if(pteptPwmCfg->byDutyCycle){
	wCmpLoad =wPrdrLoad-(wPrdrLoad * pteptPwmCfg->byDutyCycle /100);	// cmp load value
	csp_ept_set_cmpa(ptEptBase, (uint16_t)wCmpLoad);					// cmp load value
	csp_ept_set_cmpb(ptEptBase, (uint16_t)wCmpLoad);
	csp_ept_set_cmpc(ptEptBase, (uint16_t)wCmpLoad);
	csp_ept_set_cmpd(ptEptBase, (uint16_t)wCmpLoad);
	}
	
	if(pteptPwmCfg->byInter)
	{
		csp_ept_int_enable(ptEptBase, pteptPwmCfg->byInter, true);		//enable interrupt
		csi_irq_enable((uint32_t *)ptEptBase);							//enable  irq
	}
	
	gEptPrd=wPrdrLoad;
	
	return CSI_OK;	
}
 /**
 \brief  Channel configuration
 \param  ptEptBase    	pointer of ept register structure
 \param  tPwmCfg   	    refer to csi_ept_pwmchannel_config_t
 \param  channel        Channel label
 \return CSI_OK /CSI_ERROR
*/
csi_error_t csi_ept_channel_config(csp_ept_t *ptEptBase, csi_ept_pwmchannel_config_t *tPwmCfg, csi_ept_channel_e channel)
{
    uint32_t w_AQCRx_Val;
	
	w_AQCRx_Val=  tPwmCfg -> byActionZro 
	              | ( tPwmCfg -> byActionPrd  << EPT_ACT_PRD_POS  )
				  | ( tPwmCfg -> byActionCau  << EPT_ACT_CAU_POS  )
				  | ( tPwmCfg -> byActionCad  << EPT_ACT_CAD_POS  )
				  | ( tPwmCfg -> byActionCbu  << EPT_ACT_CBU_POS  )
				  | ( tPwmCfg -> byActionCbd  << EPT_ACT_CBD_POS  )
				  | ( tPwmCfg -> byActionT1u  << EPT_ACT_T1U_POS  )
				  | ( tPwmCfg -> byActionT1d  << EPT_ACT_T1D_POS  )
				  | ( tPwmCfg -> byActionT2u  << EPT_ACT_T2U_POS  )
				  | ( tPwmCfg -> byActionT2d  << EPT_ACT_T2D_POS  )
				  | ( tPwmCfg -> byChoiceCasel  << EPT_CASEL_POS  )
				  | ( tPwmCfg -> byChoiceCbsel  << EPT_CBSEL_POS  );
				  
	switch (channel)
	{	
		case (EPT_CHANNEL_A):csp_ept_set_aqcra(ptEptBase,w_AQCRx_Val);
			break;
		case (EPT_CHANNEL_B):csp_ept_set_aqcrb(ptEptBase,w_AQCRx_Val);
			break;
		case (EPT_CHANNEL_C):csp_ept_set_aqcrc(ptEptBase,w_AQCRx_Val);
            break;
		case (EPT_CHANNEL_D):csp_ept_set_aqcrd(ptEptBase,w_AQCRx_Val);
		    break;
		default:return CSI_ERROR;
			break;
	}
	return CSI_OK;
}
 /**
 \brief  DeadZoneTime configuration loading 
 \param  ptEptBase    	pointer of ept register structure
 \param  byVal         refer to csi_ept_dbdldr_e
 \param  byWod         refer to csp_ept_shdw_e
 \param  byWod2	       refer to csp_ept_lddb_e
 \return CSI_OK /CSI_ERROR
*/
csi_error_t csi_ept_dbldrload_config(csp_ept_t *ptEptBase, csi_ept_dbdldr_e byVal,csp_ept_shdw_e byWod,csp_ept_lddb_e byWod2)
{   uint32_t w_Val;
	w_Val=csp_ept_get_dbldr(ptEptBase);
	switch (byVal)
	{	
		case (DBCR) :w_Val=( w_Val &~(0x01<<EPT_DBCR_SHDWEN_POS) )|(byWod << EPT_DBCR_SHDWEN_POS);
		             w_Val=( w_Val &~(0x03<<(EPT_DBCR_SHDWEN_POS+1)))|(byWod2 << (EPT_DBCR_SHDWEN_POS+1));
			break;
		case (DBDTR):w_Val=( w_Val &~(0x01<<EPT_DBDTR_SHDWEN_POS) )|(byWod << EPT_DBDTR_SHDWEN_POS);
		             w_Val=( w_Val &~(0x03<<(EPT_DBDTR_SHDWEN_POS+1)))|(byWod2 << (EPT_DBDTR_SHDWEN_POS+1));
		    break;
		case (DBDTF):w_Val=( w_Val &~(0x01<<EPT_DBDTF_SHDWEN_POS) )|(byWod << EPT_DBDTF_SHDWEN_POS);
		             w_Val=( w_Val &~(0x03<<(EPT_DBDTF_SHDWEN_POS+1)))|(byWod2 << (EPT_DBDTF_SHDWEN_POS+1));
            break;
		case (DCKPSC):w_Val=( w_Val &~(0x01<<EPT_DCKPSC_SHDWEN_POS))|(byWod << EPT_DCKPSC_SHDWEN_POS);
		              w_Val=( w_Val &~(0x03<<(EPT_DCKPSC_SHDWEN_POS+1)))|(byWod2 << (EPT_DCKPSC_SHDWEN_POS+1));
		    break;
		default:return CSI_ERROR;
			break;
	}
	csp_ept_set_dbldr(ptEptBase,w_Val);
			
	return CSI_OK;
}   
 /**
 \brief  DeadZoneTime configuration 
 \param  ptEptBase    	pointer of ept register structure
 \param  tCfg           refer to csi_ept_deadzone_config_t
 \return CSI_OK /CSI_ERROR
*/
csi_error_t csi_ept_dbcr_config(csp_ept_t *ptEptBase, csi_ept_deadzone_config_t *tCfg)
{  uint32_t w_Val;
	w_Val=csp_ept_get_dbcr(ptEptBase);	
	w_Val=(w_Val&~(EPT_DCKSEL_MSK))|(tCfg-> byDcksel <<EPT_DCKSEL_POS);
	w_Val=(w_Val&~(EPT_CHA_DEDB_MSK))|(tCfg-> byChaDedb<<EPT_CHA_DEDB_POS);
	w_Val=(w_Val&~(EPT_CHB_DEDB_MSK))|(tCfg-> byChbDedb<<EPT_CHB_DEDB_POS);
	w_Val=(w_Val&~(EPT_CHC_DEDB_MSK))|(tCfg-> byChcDedb<<EPT_CHC_DEDB_POS);
	csp_ept_set_dbcr( ptEptBase, w_Val);	 
	csp_ept_set_dpscr(ptEptBase	,tCfg-> hwDpsc);
	csp_ept_set_dbdtr(ptEptBase	,tCfg-> hwRisingEdgereGister);
	csp_ept_set_dbdtf(ptEptBase	,tCfg-> hwFallingEdgereGister);
	return CSI_OK;	
}
 /**
 \brief  channelmode configuration 
 \param  ptEptBase    pointer of ept register structure
 \param  tCfg         refer to csi_ept_deadzone_config_t
 \param  byCh         refer to csi_ept_channel_e
 \return CSI_OK /CSI_ERROR
*/
csi_error_t csi_ept_channelmode_config(csp_ept_t *ptEptBase,csi_ept_deadzone_config_t *tCfg,csi_ept_channel_e byCh)
{    uint32_t w_Val;
     w_Val=csp_ept_get_dbcr(ptEptBase);	
	 switch (byCh)
	{	
		case (EPT_CHANNEL_A): w_Val=(w_Val&~(DB_CHA_OUTSEL_MSK)) |(tCfg-> byChxOuselS1S0   <<DB_CHA_OUTSEL_POS);
		                      w_Val=(w_Val&~(DB_CHA_POL_MSK))    |(tCfg-> byChxPolarityS3S2<<DB_CHA_POL_POS);
							  w_Val=(w_Val&~(DB_CHA_INSEL_MSK))  |(tCfg-> byChxInselS5S4   <<DB_CHA_INSEL_POS);
							  w_Val=(w_Val&~(DB_CHA_OUTSWAP_MSK))|(tCfg-> byChxOutSwapS8S7 <<DB_CHA_OUTSWAP_POS);
			break;
		case (EPT_CHANNEL_B): w_Val=(w_Val&~(DB_CHB_OUTSEL_MSK)) |(tCfg-> byChxOuselS1S0   <<DB_CHB_OUTSEL_POS);
		                      w_Val=(w_Val&~(DB_CHB_POL_MSK))    |(tCfg-> byChxPolarityS3S2<<DB_CHB_POL_POS);
							  w_Val=(w_Val&~(DB_CHB_INSEL_MSK))  |(tCfg-> byChxInselS5S4   <<DB_CHB_INSEL_POS);
							  w_Val=(w_Val&~(DB_CHB_OUTSWAP_MSK))|(tCfg-> byChxOutSwapS8S7 <<DB_CHB_OUTSWAP_POS);		            
		    break;
		case (EPT_CHANNEL_C): w_Val=(w_Val&~(DB_CHC_OUTSEL_MSK)) |(tCfg-> byChxOuselS1S0   <<DB_CHC_OUTSEL_POS);
		                      w_Val=(w_Val&~(DB_CHC_POL_MSK))    |(tCfg-> byChxPolarityS3S2<<DB_CHC_POL_POS);
							  w_Val=(w_Val&~(DB_CHC_INSEL_MSK))  |(tCfg-> byChxInselS5S4   <<DB_CHC_INSEL_POS);
							  w_Val=(w_Val&~(DB_CHC_OUTSWAP_MSK))|(tCfg-> byChxOutSwapS8S7 <<DB_CHC_OUTSWAP_POS);
		             
            break;
		default:return CSI_ERROR;
			break;
	}
	csp_ept_set_dbcr( ptEptBase, w_Val);
	return CSI_OK;
}
/**
 \brief  Carrier output parameter configuration 
 \param  ptEptBase    	pointer of ept register structure
 \param  tCfg         refer to csi_ept_Chopper_config_t
 \return CSI_OK /CSI_ERROR
*/
csi_error_t csi_ept_choppercpcr_config(csp_ept_t *ptEptBase, csi_ept_Chopper_config_t *tCfg)
{ uint32_t w_Val;
    w_Val=csp_ept_get_cpcr(ptEptBase);
 	w_Val= ( w_Val &~ EPT_OSPWTH_MSK ) |( (tCfg -> byOspwth&=0x1f) << EPT_OSPWTH_POS);	
	w_Val= ( w_Val &~ EPT_CDIV_MSK )   |( (tCfg -> byCdiv  &=0xf)   << EPT_CDIV_POS);
	w_Val= ( w_Val &~ EPT_CDUTY_MSK )  |( (tCfg -> byCduty &=0x7)   << EPT_CDUTY_POS);
	csp_ept_set_cpcr(ptEptBase,w_Val) ;  		   
	return CSI_OK;	   
}
/** \brief Carrier output
 *  \param[in] ptEptBase: pointer of ept register structure
 *  \param[in] bEnable:    refer to to csi_ept_chx_e
 *  \param[in] bEn         ENABLE/DISABLE
 *  \return  CSI_OK;
 */
csi_error_t csi_ept_chopper_config(csp_ept_t *ptEptBase, csi_ept_chx_e byCh, bool bEn)
{   uint32_t w_Val;
	w_Val=csp_ept_get_cpcr(ptEptBase);
	switch (byCh)
	{	
		case (EPTCHAX): w_Val=(  w_Val &~ EPT_CPEN_CHAX_MSK )|( bEn  << EPT_CPEN_CHAX_POS);
			break;
		case (EPTCHAY): w_Val=(  w_Val &~ EPT_CPEN_CHAY_MSK )|( bEn  << EPT_CPEN_CHAY_POS);
			break;
		case (EPTCHBX): w_Val=(  w_Val &~ EPT_CPEN_CHBX_MSK )|( bEn  << EPT_CPEN_CHBX_POS);
            break;
		case (EPTCHBY): w_Val=(  w_Val &~ EPT_CPEN_CHBY_MSK )|( bEn  << EPT_CPEN_CHBY_POS);
		    break;
		case (EPTCHCX): w_Val=(  w_Val &~ EPT_CPEN_CHCX_MSK )|( bEn  << EPT_CPEN_CHCX_POS);
            break;
		case (EPTCHCY): w_Val=(  w_Val &~ EPT_CPEN_CHCY_MSK )|( bEn  << EPT_CPEN_CHCY_POS);
		    break;
		default:return CSI_ERROR;
			break;
	}
	csp_ept_set_cpcr(ptEptBase,w_Val) ;  
	return CSI_OK;
}
/**
 \brief  State of emergency configuration 
 \param  ptEptBase    	pointer of ept register structure
 \param  tCfg           refer to csi_ept_emergency_config_t
 \return CSI_OK /CSI_ERROR
*/
csi_error_t csi_ept_emergency_cfg(csp_ept_t *ptEptBase, csi_ept_emergency_config_t *tCfg)
{ uint32_t wEmsrc;
  uint32_t wEmsrc2;
  uint32_t wEmpol;
  uint32_t wEmecr;

    wEmsrc2=csp_ept_get_src2(ptEptBase);
	wEmsrc2=(wEmsrc2 & (~EPT_EPPACE0_MSK) & (~EPT_EPPACE1_MSK) ) | (tCfg -> byFltpace1 << EPT_EPPACE1_POS) | (tCfg ->byFltpace0  << EPT_EPPACE0_POS);
	wEmsrc2=(wEmsrc2 &~0xff0000) |  tCfg ->byOrl1 <<16;
	wEmsrc2=(wEmsrc2 &~0xff)     |  tCfg ->byOrl0 ;
	csp_ept_set_src2(ptEptBase,wEmsrc2);
		
	wEmsrc = csp_ept_get_src(ptEptBase);
    wEmsrc=(  wEmsrc &~ EPT_SEL_MSK_EP(tCfg -> byEpx) )|( tCfg -> byEpxInt  << EPT_SEL_POS_EP(tCfg -> byEpx));
    csp_ept_set_src(ptEptBase,wEmsrc);
	
    wEmpol=csp_ept_get_empol(ptEptBase);	
	 switch (tCfg ->byEpxInt)
	 {    case (EBI0): wEmpol=( wEmpol  &~ POL_MSK_EBI(0)) | (tCfg -> byPolEbix <<POL_POS_EBI(0) );break;
		  case (EBI1): wEmpol=( wEmpol  &~ POL_MSK_EBI(1)) | (tCfg -> byPolEbix <<POL_POS_EBI(1) );break;
		  case (EBI2): wEmpol=( wEmpol  &~ POL_MSK_EBI(2)) | (tCfg -> byPolEbix <<POL_POS_EBI(2) );break;
		  case (EBI3): wEmpol=( wEmpol  &~ POL_MSK_EBI(3)) | (tCfg -> byPolEbix <<POL_POS_EBI(3) );break;
		  case (EBI4): wEmpol=( wEmpol  &~ POL_MSK_EBI(4)) | (tCfg -> byPolEbix <<POL_POS_EBI(4) );break;
		  default:return CSI_ERROR;break;
	 }
	csp_ept_set_empol(ptEptBase,wEmpol);

    wEmecr =  csp_ept_get_emecr(ptEptBase);	
	wEmecr =(wEmecr & (~EPT_LCKMD_MSK_EP(tCfg ->byEpx))) | (   tCfg ->byEpxLckmd <<  EPT_LCKMD_POS_EP(tCfg ->byEpx));
	csp_ept_set_emecr(ptEptBase,wEmecr);
			
	return CSI_OK;
}
/**
 \brief  State of emergency configuration 
 \param  ptEptBase    	pointer of ept register structure
 \param  tCfg           refer to csi_ept_emergency_config_t
 \return CSI_OK /CSI_ERROR
*/
csi_error_t csi_ept_emergency_pinxout(csp_ept_t *ptEptBase,csi_ept_osrchx_e  byCh ,csp_ept_emout_e byCh2)
{ uint32_t wEmosr;
    wEmosr=csp_ept_get_emosr(ptEptBase);	
	 switch (byCh)
	 {    case (EMCOAX): wEmosr=( wEmosr &~(EPT_EMCHAX_O_MSK) )|( byCh2 <<EPT_EMCHAX_O_POS);break;
		  case (EMCOBX): wEmosr=( wEmosr &~(EPT_EMCHBX_O_MSK) )|( byCh2 <<EPT_EMCHBX_O_POS);break;
          case (EMCOCX): wEmosr=( wEmosr &~(EPT_EMCHCX_O_MSK) )|( byCh2 <<EPT_EMCHCX_O_POS);break;
		  case (EMCOD):  wEmosr=( wEmosr &~(EPT_EMCHD_O_MSK) )|( byCh2 <<EPT_EMCHD_O_POS);break;
		  case (EMCOAY): wEmosr=( wEmosr &~(EPT_EMCHAY_O_MSK) )|( byCh2 <<EPT_EMCHAY_O_POS);break;
		  case (EMCOBY): wEmosr=( wEmosr &~(EPT_EMCHBY_O_MSK) )|( byCh2 <<EPT_EMCHBY_O_POS);break;
		  case (EMCOCY): wEmosr=( wEmosr &~(EPT_EMCHCY_O_MSK) )|( byCh2 <<EPT_EMCHCY_O_POS);break;
		  default:return CSI_ERROR;break;
	 }
    csp_ept_set_emosr(ptEptBase,wEmosr);
	return CSI_OK;
}
/**
 \brief  ept configuration Loading
 \param  ptEptBase    	pointer of ept register structure
 \param  tCfg           refer to csi_ept_Global_load_control_config_t
 \return CSI_OK /CSI_ERROR
*/
csi_error_t csi_ept_global_config(csp_ept_t *ptEptBase,csi_ept_Global_load_control_config_t *Global)
{   uint32_t w_GLDCR;	
	w_GLDCR =0;
    w_GLDCR = (w_GLDCR &~EPT_GLDEN_MSK)       | ((Global->bGlden & 0x01)<<EPT_GLDEN_POS);
	w_GLDCR = (w_GLDCR &~EPT_GLDMD_MSK)       | ((Global->byGldmd & 0x0f)<<EPT_GLDMD_POS);
	w_GLDCR = (w_GLDCR &~EPT_GLDCR_OSTMD_MSK) | ((Global->bOstmd & 0x01)<<EPT_GLDCR_OSTMD_POS);
	w_GLDCR = (w_GLDCR &~EPT_GLDPRD_MSK)      | ((Global->bGldprd & 0x07)<<EPT_GLDPRD_POS);
	csp_ept_set_gldcr(ptEptBase,w_GLDCR);	
	return CSI_OK;
}
/**
 \brief  Software trigger loading
 \param  ptEptBase    	pointer of ept register structure
 \return CSI_OK
*/
csi_error_t csi_ept_global_sw(csp_ept_t *ptEptBase)
{
	csp_ept_set_gldcr2(ptEptBase,0x01);
	return CSI_OK;
}
/**
 \brief  rearm  loading
 \param  ptEptBase    	pointer of ept register structure
 \return CSI_OK
*/
csi_error_t csi_ept_global_rearm(csp_ept_t *ptEptBase)
{
	csp_ept_set_gldcr2(ptEptBase,0x02);
	return CSI_OK;
}
/** \brief start ept
 *  \param pteptBase:  pointer of bt register structure
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_ept_start(csp_ept_t *pteptBase)
{   csp_ept_wr_key(pteptBase); 
	csp_ept_start(pteptBase);
	return CSI_OK;
}
/**
 \brief SW stop EPT counter
 \param ptEpt    pointer of ept register structure
*/
void csi_ept_swstop(csp_ept_t *ptEpt)
{
  csp_ept_wr_key(ptEpt);
  csp_ept_stop(ptEpt);
}
/**
 \brief set EPT start mode. 
 \param ptEpt    pointer of ept register structure
 \return eMode ：EPT_SW/EPT_SYNC
*/
void csi_ept_set_start_mode(csp_ept_t *ptEpt, csi_ept_stmd_e eMode)
{
	csp_ept_set_start_src(ptEpt, eMode);
}
/**
 \brief set EPT operation mode
 \param ptEpt    pointer of ept register structure
 \param eMode 	 EPT_OP_CONT/EPT_OP_OT
*/
void csi_ept_set_os_mode(csp_ept_t *ptEpt, csi_ept_opmd_e eMode)
{
	csp_ept_set_opmd(ptEpt, eMode);
}

/**
 \brief set EPT stop status
 \param ptEpt    pointer of ept register structure
 \param eSt 	 EPT_STP_HZ/EPT_STP_LOW
*/
void csi_ept_set_stop_st(csp_ept_t *ptEpt, csp_ept_stpst_e eSt)
{	
  csp_ept_set_stop_st(ptEpt,eSt);
}

/**
 \brief get counter period to calculate the duty cycle. 
 \param ptEpt    pointer of ept register structure
 \return counter period (reg data)
*/
uint16_t csi_ept_get_prdr(csp_ept_t *ptEpt)
{
	return csp_ept_get_prdr(ptEpt);
}

/**
 \brief change ept output dutycycle. 
 \param ptEptBase     pointer of ept register structure
 \param eCh           refer to csi_ept_chtype_e
 \param wActiveTime   cmpx data to be set directly
*/
csi_error_t csi_ept_change_ch_duty(csp_ept_t *ptEptBase, csi_ept_chtype_e eCh, uint32_t wActiveTime)
{ uint16_t  wCmpLoad;
    wCmpLoad =gEptPrd-(gEptPrd * wActiveTime /100);

	switch (eCh)
	{	
		case (EPT_CH_A):csp_ept_set_cmpa(ptEptBase, (uint16_t)wCmpLoad);
			break;
		case (EPT_CH_B):csp_ept_set_cmpb(ptEptBase, (uint16_t)wCmpLoad);
			break;
		case (EPT_CH_C):csp_ept_set_cmpc(ptEptBase, (uint16_t)wCmpLoad);
		    break;
		case (EPT_CH_D):csp_ept_set_cmpd(ptEptBase, (uint16_t)wCmpLoad);
		    break;
		default: return (1);
			break;
	}
    return (0);
}

/**
 \brief software force lock
 \param ptEpt    pointer of ept register structure
 \param eEbi 	 external emergency input: EPT_EPI0~7 （EBI4 = LVD）
 \return none
*/
void csi_ept_force_em(csp_ept_t *ptEptBase, csp_ept_ep_e byEbi)
{
	csp_ept_force_em(ptEptBase,byEbi);
}

/**
 \brief get harklock status
 \param ptEpt    pointer of ept register structure
 \return uint8_t 0b_ _ _ _ _ _ _ _
*/
uint8_t csi_ept_get_hdlck_st(csp_ept_t *ptEptBase)
{	
	return (csp_ept_get_emHdlck(ptEptBase));
}

/**
 \brief clear harklock status
 \param ptEpt   pointer of ept register structure
 \return  eEbi 	 external emergency input: csp_ept_ep_e                 
*/
void csi_ept_clr_hdlck(csp_ept_t *ptEptBase, csp_ept_ep_e eEbi)
{
	csp_ept_clr_emHdlck(ptEptBase, eEbi);
}

/**
 \brief get 	 softlock status
 \param ptEpt    pointer of ept register structure
 \return uint8_t 0b_ _ _ _ _ _ _ _
*/
uint8_t csi_ept_get_sftlck_st(csp_ept_t *ptEptBase)
{	
	return (csp_ept_get_emSdlck(ptEptBase));
}

/**
 \brief clear softlock status
 \param ptEpt   pointer of ept register structure
 \return  eEpi 	 external emergency input: csp_ept_ep_e
*/
void csp_ept_clr_sftlck(csp_ept_t *ptEptBase, csp_ept_ep_e eEpi)
{	
	csp_ept_clr_emSdlck(ptEptBase, eEpi);
}

/**
  \brief       enable/disable ept in debug mode
  \param[in]   ptEpt      pointer of ept register structure
  \param[in]   bEnable		ENABLE/DISABLE
*/

void csi_ept_debug_enable(csp_ept_t *ptEptBase, bool bEnable)
{
	csp_ept_dbg_enable(ptEptBase, bEnable);
}

/**
  \brief       enable/disable ept emergencyinterruption
  \param[in]   ptEptBase       pointer of ept register structure
  \param[in]   eEbi		       refer to csp_ept_emint_e
*/
void csi_ept_emergency_interruption_en(csp_ept_t *ptEptBase, csp_ept_emint_e eEbi)
{
    csp_ept_Emergency_emimcr(ptEptBase,eEbi);
}

/**
  \brief   enable/disable ept out trigger 
  \param   ptEptBase      pointer of ept register structure
  \param 	eCh			0/1/2/3
  \param   bEnable		ENABLE/DISABLE
*/
csi_error_t csi_ept_evtrg_enable(csp_ept_t *ptEptBase, uint8_t byCh, bool bEnable)
{	
	if (byCh > 3)return CSI_ERROR;
    csp_ept_trg_xoe_enable(ptEptBase, byCh, bEnable);
	return CSI_OK;
}

/**
  \brief   One time software output 
  \param   ptEptBase      pointer of ept register structure 
  \param   byCh	         EPT_OSTSFA/EPT_OSTSFB/EPT_OSTSFC/EPT_OSTSFD		
  \param   bEnable 		EPT_LDAQCR_ZRO/EPT_LDAQCR_PRD/EPT_LDAQCR_ZROPRD
*/
csi_error_t csi_ept_Onetimesoftware_output(csp_ept_t *ptEptBase, uint16_t byCh, csp_ept_action_e bEnable)
{	
	switch (byCh){
	case EPT_OSTSFA: ptEptBase ->AQOSF |= EPT_OSTSFA;
	                 ptEptBase ->AQOSF = (ptEptBase ->AQOSF &~(EPT_ACTA_MSK))|((bEnable&0x03)<<EPT_ACTA_POS);
	     break;
	case EPT_OSTSFB: ptEptBase ->AQOSF |= EPT_OSTSFB;
	                 ptEptBase ->AQOSF = (ptEptBase ->AQOSF &~(EPT_ACTB_MSK))|((bEnable&0x03)<<EPT_ACTB_POS);
	     break;	
    case EPT_OSTSFC: ptEptBase ->AQOSF |= EPT_OSTSFC;
	                 ptEptBase ->AQOSF = (ptEptBase ->AQOSF &~(EPT_ACTC_MSK))|((bEnable&0x03)<<EPT_ACTC_POS);
	     break;
	case EPT_OSTSFD: ptEptBase ->AQOSF |= EPT_OSTSFD;
	                 ptEptBase ->AQOSF = (ptEptBase ->AQOSF &~(EPT_ACTD_MSK))|((bEnable&0x03)<<EPT_ACTD_POS);
	     break;
	default: return CSI_ERROR;
	     break;
    }
	return CSI_OK;
}
/** \brief  Continuous software waveform loading control
 *  \param[in] ptEptBase: pointer of ept register structure
 *  \param[in] bEnable:    refer to csp_ept_aqosf_e
 *  \return  none
 */
void csi_ept_loading_method_aqcsf(csp_ept_t *ptEptBase, csp_ept_aqosf_e bEnable)
{
	ptEptBase ->AQOSF  = (ptEptBase ->AQOSF &~(EPT_AQCSF_LDTIME_MSK))|((bEnable&0x03)<<EPT_AQCSF_LDTIME_POS);
}
/** \brief Continuous software waveform control
 *  \param[in] ptEptBase: pointer of ept register structure
 *  \param[in] byCh        refer to csi_ept_channel_e
 *  \param[in] bEnable:    refer to  csp_ept_aqosf_e
 *  \return  none
 */
csi_error_t csi_ept_continuous_software_waveform(csp_ept_t *ptEptBase, csi_ept_channel_e byCh, csp_ept_aqcsf_e bEnable)
{
	
	switch (byCh){
	case EPT_CHANNEL_A:  ptEptBase ->AQCSF = (ptEptBase ->AQCSF &~(0x03))|(bEnable&0x03);            
	     break;
	case EPT_CHANNEL_B:  ptEptBase ->AQCSF = (ptEptBase ->AQCSF &~(0x0c))|(bEnable&0x03)<<2;
	     break;	
    case EPT_CHANNEL_C:  ptEptBase ->AQCSF = (ptEptBase ->AQCSF &~(0x30))|(bEnable&0x03)<<4;
	     break;
	case EPT_CHANNEL_D:  ptEptBase ->AQCSF = (ptEptBase ->AQCSF &~(0xc0))|(bEnable&0x03)<<6;
	     break;
	default: return CSI_ERROR;
	     break;
    }		
	return CSI_OK;
}

/** \brief ept  input  config  
 *  \param[in] ptEptBase: pointer of ept register structure
 *  \param[in] eInt:     refer to to csp_ept_int_e
 *  \param[in] bEnable:  ENABLE/DISABLE
 *  \return CSI_OK;
 */
csi_error_t csi_ept_int_enable(csp_ept_t *ptEptBase, csp_ept_int_e eInt, bool bEnable)
{  
	csp_ept_int_enable(ptEptBase,eInt,bEnable);
	return CSI_OK;
}

/** \brief ept sync input evtrg config  
 * 
 *  \param[in] ptEptBase: pointer of ept register structure
 *  \param[in] eTrgin: ept sync evtrg input channel(0~5)
 *  \param[in] eTrgMode: ept sync evtrg mode, continuous/once
 *  \param[in] eAutoRearm: refer to csi_ept_arearm_e 
 *  \return none
 */
void csi_ept_set_sync(csp_ept_t *ptEptBase, csi_ept_trgin_e eTrgIn, csi_ept_trgmode_e eTrgMode, csi_ept_arearm_e eAutoRearm)
{
	csp_ept_set_sync_mode(ptEptBase, eTrgIn, eTrgMode);
	csp_ept_set_auto_rearm(ptEptBase, eAutoRearm);
	csp_ept_sync_enable(ptEptBase, eTrgIn, ENABLE);
}

/** \brief ept sync -> evtrv config
 * 
 *  \param[in] ptEptBase: pointer of ept register structure
 *  \param[in] eTrgin: ept sync evtrg input channel(0~5)
 *  \param[in] byTrgChx: trgxsel channel(0~1)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_ept_set_sync2evtrg(csp_ept_t *ptEptBase, csi_ept_trgin_e eTrgIn, uint8_t byTrgChx)
{
	switch(byTrgChx)
	{
		case 0:
			csp_ept_sync_trg0sel(ptEptBase, eTrgIn);
			break;
		case 1:
			csp_ept_sync_trg1sel(ptEptBase, eTrgIn);
			break;
		default:
			return CSI_ERROR;
		
	}
	return CSI_OK;
}

/** \brief ept sync input filter config  
 * 
 *  \param[in] ptEptBase: pointer of ept register structure
 *  \param[in] ptFilter: pointer of sync input filter parameter config structure
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_ept_set_sync_filter(csp_ept_t *ptEptBase, csi_ept_filter_config_t *ptFilter)
{
	uint32_t wFiltCr;
	uint32_t wWindow;
	
	if(ptFilter->byFiltSrc > EPT_FILT_SYNCIN5)
		return CSI_ERROR;
	wFiltCr = ptFilter->byFiltSrc | (ptFilter->byWinInv << EPT_FLTBLKINV_POS) | 
			(ptFilter->byWinAlign << EPT_ALIGNMD_POS) | (ptFilter->byWinCross << EPT_CROSSMD_POS);
	wWindow = ptFilter->byWinOffset | (ptFilter->byWinWidth << EPT_FLT_WDW_POS);
	
	csp_ept_set_trgftcr(ptEptBase, wFiltCr);
	csp_ept_set_trgftwr(ptEptBase, wWindow);
	
	return CSI_OK;
}
/** \brief rearm ept sync evtrg  
 * 
 *  \param[in] ptEptBase: pointer of ept register structure
 *  \param[in] eTrgin: ept sync evtrg input channel(0~5)
 *  \return none
 */
void csi_ept_rearm_sync(csp_ept_t *ptEptBase,csi_ept_trgin_e eTrgin)
{
	csp_ept_rearm_sync(ptEptBase, eTrgin);
}
/** \brief ept evtrg output config
 * 
 *  \param[in] ptEptBase: pointer of ept register structure
 *  \param[in] byTrgOut: evtrg out port(0~3)
 *  \param[in] eTrgSrc: evtrg source(1~15) 
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_ept_set_evtrg(csp_ept_t *ptEptBase, uint8_t byTrgOut, csi_ept_trgsrc_e eTrgSrc)
{
	switch (byTrgOut)
	{
		case EPT_TRG_OUT0:
		case EPT_TRG_OUT1: 
			if(eTrgSrc == EPT_TRGSRC_PEND)
				return CSI_ERROR;
			if(eTrgSrc == EPT_TRGSRC_DIS)								
			{
				csp_ept_trg_xoe_enable(ptEptBase, byTrgOut, DISABLE);	//disable evtrg source out
				return CSI_OK;
			}
			csp_ept_set_trgsrc01(ptEptBase, byTrgOut, eTrgSrc);
			break;
		case EPT_TRG_OUT2:
		case EPT_TRG_OUT3: 
			if(eTrgSrc == EPT_TRGSRC_EX)
				return CSI_ERROR;
			if(eTrgSrc == EPT_TRGSRC_DIS)								
			{
				csp_ept_trg_xoe_enable(ptEptBase, byTrgOut, DISABLE);	//disable evtrg source out
				return CSI_OK;
			}
			if (eTrgSrc == EPT_TRGSRC_PEND)
				eTrgSrc = 12;
			csp_ept_set_trgsrc23(ptEptBase, byTrgOut, eTrgSrc);
			break;
		default: 
			return CSI_ERROR;
	}
	
	csp_ept_trg_xoe_enable(ptEptBase, byTrgOut, ENABLE);				//evtrg out enable
	
	return CSI_OK;
}


/** \brief ept evtrg cntxinit control
 * 
 *  \param[in] ptEptBase: pointer of ept register structure
 *  \param[in] byCntChx: evtrg countinit channel(0~3)
 *  \param[in] byCntVal: evtrg cnt value
 *  \param[in] byCntInitVal: evtrg cntxinit value
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_ept_set_evcntinit(csp_ept_t *ptEptBase, uint8_t byCntChx, uint8_t byCntVal, uint8_t byCntInitVal)
{

 if(byCntChx > EPT_CNT3INIT)
  return CSI_ERROR;
 
 csp_ept_set_trgprd(ptEptBase, byCntChx, byCntVal - 1);    //evtrg count
 csp_ept_trg_cntxinit(ptEptBase, byCntChx, byCntInitVal);
 csp_ept_trg_cntxiniten_enable(ptEptBase, byCntChx, ENABLE);
 
 return CSI_OK;
}

