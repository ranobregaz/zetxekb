/***********************************************************************//** 
 * \file  ept.c
 * \brief  Enhanced purpose timer driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-8 <td>V0.0  <td>WNN   <td>initial
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
uint32_t g_ept_prd;


/** \brief
 * 
 *  \param[in] pteptBase: pointer of bt register structure
 *  \param[in] pteptPwmCfg: pointer of bt pwm parameter config structure
 *  \return error code \ref csi_error_t
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
	
	wClkDiv = (soc_get_pclk_freq() / pteptPwmCfg->wFreq / 60000);		// clk div value
	if(wClkDiv == 0)wClkDiv = 1;
	
	wPrdrLoad  = (soc_get_pclk_freq()/pteptPwmCfg->wFreq/wClkDiv);	    //prdr load value
//			
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
	
//	
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
	
	g_ept_prd=wPrdrLoad;
	
	return CSI_OK;
}
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
	
	wClkDiv = (soc_get_pclk_freq() / pteptPwmCfg->wFreq / 60000);		// clk div value
	if(wClkDiv == 0)wClkDiv = 1;
	
	wPrdrLoad  = (soc_get_pclk_freq()/pteptPwmCfg->wFreq/wClkDiv);	    //prdr load value
		
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
	
	g_ept_prd=wPrdrLoad;
	
	return CSI_OK;


	
}

//csi_error_t csi_ept_syncr_config_init(csp_ept_t *ptEptBase,csi_ept_syncr_config_t *tPwmCfg)
//{  uint32_t w_A_Val;
//    w_A_Val =0;
//	w_A_Val =(w_A_Val & ~EPT_SYNC_MSK(0) )|((tPwmCfg->syncen0 &0x01)<<EPT_SYNC_POS(0));
//	w_A_Val =(w_A_Val & ~EPT_SYNC_MSK(1) )|((tPwmCfg->syncen1 &0x01)<<EPT_SYNC_POS(1));
//	w_A_Val =(w_A_Val & ~EPT_SYNC_MSK(2) )|((tPwmCfg->syncen2 &0x01)<<EPT_SYNC_POS(2));
//	w_A_Val =(w_A_Val & ~EPT_SYNC_MSK(3) )|((tPwmCfg->syncen3 &0x01)<<EPT_SYNC_POS(3));
//	w_A_Val =(w_A_Val & ~EPT_SYNC_MSK(4) )|((tPwmCfg->syncen4 &0x01)<<EPT_SYNC_POS(4));
//	w_A_Val =(w_A_Val & ~EPT_SYNC_MSK(5) )|((tPwmCfg->syncen5 &0x01)<<EPT_SYNC_POS(5));
//	
//	w_A_Val =(w_A_Val & ~EPT_OSTMD_MSK(0) )|((tPwmCfg->ostmd0 &0x01)<<EPT_OSTMD_POS(0));
//	w_A_Val =(w_A_Val & ~EPT_OSTMD_MSK(1) )|((tPwmCfg->ostmd1 &0x01)<<EPT_OSTMD_POS(1));
//	w_A_Val =(w_A_Val & ~EPT_OSTMD_MSK(2) )|((tPwmCfg->ostmd2 &0x01)<<EPT_OSTMD_POS(2));
//	w_A_Val =(w_A_Val & ~EPT_OSTMD_MSK(3) )|((tPwmCfg->ostmd3 &0x01)<<EPT_OSTMD_POS(3));
//	w_A_Val =(w_A_Val & ~EPT_OSTMD_MSK(4) )|((tPwmCfg->ostmd4 &0x01)<<EPT_OSTMD_POS(4));
//	w_A_Val =(w_A_Val & ~EPT_OSTMD_MSK(5) )|((tPwmCfg->ostmd5 &0x01)<<EPT_OSTMD_POS(5));
//	
//	w_A_Val =(w_A_Val & ~EPT_REARM_MSK(0) )|((tPwmCfg->rearm0 &0x01)<<EPT_REARM_POS(0));
//	w_A_Val =(w_A_Val & ~EPT_REARM_MSK(1) )|((tPwmCfg->rearm1 &0x01)<<EPT_REARM_POS(1));
//	w_A_Val =(w_A_Val & ~EPT_REARM_MSK(2) )|((tPwmCfg->rearm2 &0x01)<<EPT_REARM_POS(2));
//	w_A_Val =(w_A_Val & ~EPT_REARM_MSK(3) )|((tPwmCfg->rearm3 &0x01)<<EPT_REARM_POS(3));
//	w_A_Val =(w_A_Val & ~EPT_REARM_MSK(4) )|((tPwmCfg->rearm4 &0x01)<<EPT_REARM_POS(4));
//	w_A_Val =(w_A_Val & ~EPT_REARM_MSK(5) )|((tPwmCfg->rearm5 &0x01)<<EPT_REARM_POS(5));
//	
//	w_A_Val =(w_A_Val & ~EPT_AREARM_MSK )  |((tPwmCfg->arearm &0x03)   <<EPT_AREARM_POS);
//	w_A_Val =(w_A_Val & ~EPT_TXREARM0_MSK )|((tPwmCfg->Tx_rearm0 &0x03)<<EPT_TXREARM0_POS);
//	w_A_Val =(w_A_Val & ~EPT_TRGO0SEL_MSK )|((tPwmCfg->trgo0sel &0x07) <<EPT_TRGO0SEL_POS);
//	w_A_Val =(w_A_Val & ~EPT_TRGO1SEL_MSK )|((tPwmCfg->trgo1sel &0x07) <<EPT_TRGO1SEL_POS);
////	 mdelay(1);
//	csp_ept_sync_config(ptEptBase,w_A_Val);
//	
// return CSI_OK;
//}

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

//csi_error_t csi_ept_dbldrloadcontrol_config(csp_ept_t *ptEptBase, csi_ept_dbldr_config_t *tCfg)
//{  uint32_t w_Val;
//	w_Val= tCfg -> bycfgdbcrshadoworimmediate
//	      |(tCfg -> bycfgdbcrshadowLoadcontrol << (EPT_DBCR_SHDWEN_POS+1))
//		  |(tCfg -> bycfgdbdtrshadoworimmediate<<  EPT_DBDTR_SHDWEN_POS)
//		  |(tCfg -> bycfgdbdtrshadowLoadcontrol<< (EPT_DBDTR_SHDWEN_POS+1))
//		  |(tCfg -> bycfgdbdtfshadoworimmediate<<  EPT_DBDTF_SHDWEN_POS)
//		  |(tCfg -> bycfgdbdtfshadowLoadcontrol<< (EPT_DBDTF_SHDWEN_POS+1))
//		  |(tCfg -> bycfgdckpscshadoworimmediate<< EPT_DCKPSC_SHDWEN_POS)
//		  |(tCfg -> bycfgdckpscshadowLoadcontrol<<(EPT_DCKPSC_SHDWEN_POS+1))
//		  ;
//	csp_ept_set_dbldr(ptEptBase,w_Val);	  
//	return CSI_OK;	  
//}
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

csi_error_t csi_ept_choppercpcr_config(csp_ept_t *ptEptBase, csi_ept_Chopper_config_t *tCfg)
{ uint32_t w_Val;
    w_Val=csp_ept_get_cpcr(ptEptBase);
 	w_Val= ( w_Val &~ EPT_OSPWTH_MSK ) |( (tCfg -> byOspwth&=0x1f) << EPT_OSPWTH_POS);	
	w_Val= ( w_Val &~ EPT_CDIV_MSK )   |( (tCfg -> byCdiv  &=0xf)   << EPT_CDIV_POS);
	w_Val= ( w_Val &~ EPT_CDUTY_MSK )  |( (tCfg -> byCduty &=0x7)   << EPT_CDUTY_POS);
	csp_ept_set_cpcr(ptEptBase,w_Val) ;  		   
	return CSI_OK;	   
}

csi_error_t csi_ept_chopper_config(csp_ept_t *ptEptBase, csi_ept_chx_e byCh, bool bEn)
{   uint32_t w_Val;
	w_Val=csp_ept_get_cpcr(ptEptBase);
	
	
}


csi_error_t csi_ept_emergency_config(csp_ept_t *ptEptBase, csi_ept_emergency_input_config_t *tCfg)
{ uint32_t w_EMSRC;
  uint32_t w_EMSRC2;
  uint32_t w_EMPOL;
  uint32_t w_EMECR;
  uint32_t w_EMOSR;
  
    w_EMSRC=      ((tCfg -> ep0choiceebixint & 0x0f)<< EPT_SEL_POS_EP(EP0))
                 |((tCfg -> ep1choiceebixint & 0x0f)<< EPT_SEL_POS_EP(EP1))
				 |((tCfg -> ep2choiceebixint & 0x0f)<< EPT_SEL_POS_EP(EP2))
				 |((tCfg -> ep3choiceebixint & 0x0f)<< EPT_SEL_POS_EP(EP3))
				 |((tCfg -> ep4choiceebixint & 0x0f)<< EPT_SEL_POS_EP(EP4))
				 |((tCfg -> ep5choiceebixint & 0x0f)<< EPT_SEL_POS_EP(EP5))
				 |((tCfg -> ep6choiceebixint & 0x0f)<< EPT_SEL_POS_EP(EP6))
				 |((tCfg -> ep7choiceebixint & 0x0f)<< EPT_SEL_POS_EP(EP7))
                 ;
	csp_ept_set_src(ptEptBase,w_EMSRC);	

	w_EMSRC2= (tCfg -> orl1choiceepxorl &0xff0000) 
             |(tCfg -> orl0choiceepxorl &0xff)
			 |((tCfg -> fltpace0 &0x0f )<<EPT_EPPACE0_POS)
			 |((tCfg -> fltpace1 &0x0f )<<EPT_EPPACE1_POS)
             ;
	csp_ept_set_src2(ptEptBase,w_EMSRC2);

    w_EMPOL=  (tCfg -> POLPOSEBI0 &0x01)<<POL_POS_EBI(0)
             |(tCfg -> POLPOSEBI1 &0x01)<<POL_POS_EBI(1)
			 |(tCfg -> POLPOSEBI2 &0x01)<<POL_POS_EBI(2)
			 |(tCfg -> POLPOSEBI3 &0x01)<<POL_POS_EBI(3)
			 |(tCfg -> POLPOSEBI4 &0x01)<<POL_POS_EBI(4)
			 ;
	csp_ept_set_empol(ptEptBase,w_EMPOL);

    w_EMECR= ((tCfg -> ep0lckmd &0x3)<<EPT_LCKMD_POS_EP(EP0))
	        |((tCfg -> ep1lckmd &0x3)<<EPT_LCKMD_POS_EP(EP1))
			|((tCfg -> ep2lckmd &0x3)<<EPT_LCKMD_POS_EP(EP2))
			|((tCfg -> ep3lckmd &0x3)<<EPT_LCKMD_POS_EP(EP3))
			|((tCfg -> ep4lckmd &0x3)<<EPT_LCKMD_POS_EP(EP4))
			|((tCfg -> ep5lckmd &0x3)<<EPT_LCKMD_POS_EP(EP5))
			|((tCfg -> ep6lckmd &0x3)<<EPT_LCKMD_POS_EP(EP6))
			|((tCfg -> ep7lckmd &0x3)<<EPT_LCKMD_POS_EP(EP7))
			|((tCfg -> osrshdw &0x1)  <<EPT_EMOSR_SHDWEN_POS )
			|((tCfg -> osrldmd &0x3)  <<EPT_OSRLDMD_POS      )
			|((tCfg -> slclrmd &0x3)  <<EPT_SLCK_CLRMD_POS   )
			|((tCfg -> emasync &0x1)  <<EPT_EPASYNC_POS      )
			|((tCfg -> cpufault &0x1)<<EPT_CPUFAULT_HLCK_POS)
			|((tCfg -> memfault &0x1)<<EPT_MEMFAULT_HLCK_POS)
			|((tCfg -> eomfault &0x1)<<EPT_EOMFAULT_HLCK_POS)
			;
	csp_ept_set_emecr(ptEptBase,w_EMECR);
		
	w_EMOSR	=  ((tCfg -> emcoax &0x3)<< EPT_EMCHAX_O_POS)
	          |((tCfg -> emcobx &0x3)<< EPT_EMCHBX_O_POS)
			  |((tCfg -> emcocx &0x3)<< EPT_EMCHCX_O_POS)
			  |((tCfg -> emcocx &0x3)<< EPT_EMCHCX_O_POS)
			  |((tCfg -> emcod  &0x3)<<  EPT_EMCHD_O_POS)
			  |((tCfg -> emcoay &0x3)<< EPT_EMCHAY_O_POS)
			  |((tCfg -> emcoby &0x3)<< EPT_EMCHBY_O_POS)
			  |((tCfg -> emcocy &0x3)<< EPT_EMCHCY_O_POS)
			  ;
	csp_ept_set_emosr(ptEptBase,w_EMOSR);
	
	return CSI_OK;
}


csi_error_t  csi_ept_evtrg_config(csp_ept_t *ptEptBase, csi_ept_Event_trigger_config_t *tCfg)
{uint32_t w_EVTRG;
 uint32_t w_EVPS;
 uint32_t w_val=0;
// uint32_t w_EVCNTINIT; 
    w_EVTRG=  (tCfg -> trg0sel &0xf)   << EPT_SEL_POS_TRG(0)
             |(tCfg -> trg1sel &0xf)   << EPT_SEL_POS_TRG(1)
			 |(tCfg -> trg2sel &0xf)   << EPT_SEL_POS_TRG(2)
	         |(tCfg -> trg3sel &0xf)   << EPT_SEL_POS_TRG(3)
			 |(tCfg -> cnt0initen&0x01)<< EPT_INITEN_POS_CNT(0)
			 |(tCfg -> cnt1initen&0x01)<< EPT_INITEN_POS_CNT(1)
			 |(tCfg -> cnt2initen&0x01)<< EPT_INITEN_POS_CNT(2)
			 |(tCfg -> cnt3initen&0x01)<< EPT_INITEN_POS_CNT(3)
			 |(tCfg -> trg0en&0x01)    << EPT_OUTEN_POS_TRG(0)
			 |(tCfg -> trg1en&0x01)    << EPT_OUTEN_POS_TRG(1)
			 |(tCfg -> trg2en&0x01)    << EPT_OUTEN_POS_TRG(2)
			 |(tCfg -> trg3en&0x01)    << EPT_OUTEN_POS_TRG(3)
			 |(tCfg -> cnt0initfrc&0x01)<< EPT_SWTRG_EV(0)
			 |(tCfg -> cnt1initfrc&0x01)<< EPT_SWTRG_EV(1)
			 |(tCfg -> cnt2initfrc&0x01)<< EPT_SWTRG_EV(2)
			 |(tCfg -> cnt3initfrc&0x01)<< EPT_SWTRG_EV(3)
			 ;
	csp_ept_set_evtrg(ptEptBase,w_EVTRG);
		 
	w_EVPS= (tCfg -> trg0prd &0xf)<<EPT_PRD_POS_EV(0)
	       |(tCfg -> trg1prd &0xf)<<EPT_PRD_POS_EV(1)
		   |(tCfg -> trg2prd &0xf)<<EPT_PRD_POS_EV(2)
		   |(tCfg -> trg3prd &0xf)<<EPT_PRD_POS_EV(3)
		   ;
	csp_ept_set_evps(ptEptBase,w_EVPS);   
		   
//    w_EVCNTINIT=  (tCfg -> cnt0_init&0xf) <<EPT_CNT_INIT_POS_EV(0)
//	             |(tCfg -> cnt1_init&0xf) <<EPT_CNT_INIT_POS_EV(1)
//				 |(tCfg -> cnt2_init&0xf) <<EPT_CNT_INIT_POS_EV(2)
//				 |(tCfg -> cnt3_init&0xf) <<EPT_CNT_INIT_POS_EV(3)
//                 ;
	csp_ept_init_trgtime(ptEptBase, 0,tCfg -> cnt0init);
	csp_ept_init_trgtime(ptEptBase, 1,tCfg -> cnt1init);
	csp_ept_init_trgtime(ptEptBase, 2,tCfg -> cnt2init);
	csp_ept_init_trgtime(ptEptBase, 3,tCfg -> cnt3init);
			 
	if(tCfg -> trg0en){csp_ept_int_enable(ptEptBase,EPT_INT_TRGEV0,true);}//enable interrupt
	if(tCfg -> trg1en){csp_ept_int_enable(ptEptBase,EPT_INT_TRGEV1,true);}
	if(tCfg -> trg2en){csp_ept_int_enable(ptEptBase,EPT_INT_TRGEV2,true);}
	if(tCfg -> trg3en){csp_ept_int_enable(ptEptBase,EPT_INT_TRGEV3,true);}
	
	csi_irq_enable((uint32_t *)ptEptBase);							//enable  irq
	
	if(tCfg ->fltipscld){
		                 ptEptBase->CEDR =( ptEptBase->CEDR &~(EPT_FLTPRS_MSK))|((tCfg ->fltckprs & 0xff)<<EPT_FLTPRS_POS);
		                 ptEptBase->CR =( ptEptBase->CR &~(EPT_FLT_INIT))|((tCfg ->fltipscld)<<EPT_FLT_POS);
	                    
	                    }
	if((tCfg ->srcsel>0)&&(tCfg ->srcsel<7)){
		                 w_val=( w_val &~(EPT_FLTSRC_MSK))|((tCfg ->srcsel & 0x07)<<EPT_FLTSRC_POS );
						 w_val=( w_val &~(EPT_FLTBLKINV_MSK))|((tCfg ->blkinv & 0x01)<<EPT_FLTBLKINV_POS );
						 w_val=( w_val &~(EPT_ALIGNMD_MSK))|((tCfg ->alignmd & 0x03)<<EPT_ALIGNMD_POS );
						 w_val=( w_val &~(EPT_CROSSMD_MSK))|((tCfg ->crossmd & 0x01)<<EPT_CROSSMD_POS );
						 csp_ept_set_trgftcr(ptEptBase,w_val);
						 w_val=(tCfg ->window <<16)| tCfg ->offset;						 
						 csp_ept_set_trgftwr(ptEptBase,w_val);
	}					
		 
	return CSI_OK;	 
}

csi_error_t csi_ept_global_config(csp_ept_t *ptEptBase,csi_ept_Global_load_control_config_t *Global)
{uint32_t w_GLDCR;
 uint32_t w_GLDCR2;	
 uint32_t w_GLDCFG;	
	w_GLDCR =0;
    w_GLDCR = (w_GLDCR &~EPT_GLDEN_MSK)       | ((Global->glden & 0x01)<<EPT_GLDEN_POS);
	w_GLDCR = (w_GLDCR &~EPT_GLDMD_MSK)       | ((Global->gldmd & 0x0f)<<EPT_GLDMD_POS);
	w_GLDCR = (w_GLDCR &~EPT_GLDCR_OSTMD_MSK) | ((Global->ostmd & 0x01)<<EPT_GLDCR_OSTMD_POS);
	w_GLDCR = (w_GLDCR &~EPT_GLDPRD_MSK)      | ((Global->gldprd & 0x07)<<EPT_GLDPRD_POS);
	csp_ept_set_gldcr(ptEptBase,w_GLDCR);	
	w_GLDCR2 =  Global->osrearm ;
	w_GLDCR2 |= Global->gfrcld << 1;
	csp_ept_set_gldcr2(ptEptBase,w_GLDCR2);	
	w_GLDCFG=0;
	w_GLDCFG |= Global-> prdren ;
	w_GLDCFG |= Global-> cmpaen  << EPT_LD_CMPA_POS;
	w_GLDCFG |= Global-> cmpben  << EPT_LD_CMPB_POS;
	w_GLDCFG |= Global-> cmpcen  << EPT_LD_CMPC_POS;
	w_GLDCFG |= Global-> cmpden  << EPT_LD_CMPD_POS;
	w_GLDCFG |= Global-> dbdtren << EPT_LD_DBDTR_POS;
	w_GLDCFG |= Global-> dbdtfen << EPT_LD_DBDTF_POS;
	w_GLDCFG |= Global-> dbcren  << EPT_LD_DBCR_POS;
	w_GLDCFG |= Global-> aqcraen << EPT_LD_AQCRA_POS;
	w_GLDCFG |= Global-> aqcrben << EPT_LD_AQCRB_POS;
	w_GLDCFG |= Global-> aqcrcen << EPT_LD_AQCRC_POS;
	w_GLDCFG |= Global-> aqcrden << EPT_LD_AQCRD_POS;
	w_GLDCFG |= Global-> aqcsfen << EPT_LD_AQCSF_POS;
	w_GLDCFG |= Global-> aqosren << EPT_LD_EMOSR_POS;
	csp_ept_set_gldcfg(ptEptBase,w_GLDCFG);
	return CSI_OK;
}


/** \brief start ept
 * 
 *  \param[in] pteptBase: pointer of bt register structure
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_ept_start(csp_ept_t *pteptBase)
{   csp_ept_wr_key(pteptBase); 
	csp_ept_start(pteptBase);
	return CSI_OK;
}
/**
 \brief SW stop EPT counter
 \param ptEpt    EPT handle to operate
*/
void csi_ept_swstop(csp_ept_t *ptEpt)
{
  csp_ept_wr_key(ptEpt);
  csp_ept_stop(ptEpt);
}


/**
 \brief set EPT start mode. 
 \param ptEpt    EPT handle to operate
 \return eMode ：EPT_SW/EPT_SYNC
*/
void csi_ept_set_start_mode(csp_ept_t *ptEpt, csi_ept_stmd_e eMode)
{
	csp_ept_set_start_src(ptEpt, eMode);
}


/**
 \brief set EPT operation mode
 \param ptEpt    EPT handle to operate
 \param eMode 	 EPT_OP_CONT/EPT_OP_OT
*/
void csi_ept_set_os_mode(csp_ept_t *ptEpt, csi_ept_opmd_e eMode)
{
	csp_ept_set_opmd(ptEpt, eMode);
}

/**
 \brief set EPT stop status
 \param ptEpt    EPT handle to operate
 \param eSt 	 EPT_STP_HZ/EPT_STP_LOW
*/
void csi_ept_set_stop_st(csp_ept_t *ptEpt, csp_ept_stpst_e eSt)
{	
  csp_ept_set_stop_st(ptEpt,eSt);
}

/**
 \brief get counter period to calculate the duty cycle. 
 \param ptGpta    ept handle to operate
 \return counter period (reg data)
*/
uint16_t csi_ept_get_prdr(csp_ept_t *ptEpt)
{
	return csp_ept_get_prdr(ptEpt);
}

/**
 \brief change ept output dutycycle. 
 \param ptEpt    ept handle to operate
 \param wActiveTime cmpx data to be set directly
*/
csi_error_t csi_ept_change_ch_duty(csp_ept_t *ptEptBase, csi_ept_chtype_e eCh, uint32_t wActiveTime)
{ uint16_t  wCmpLoad;
    wCmpLoad =g_ept_prd-(g_ept_prd * wActiveTime /100);

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
 \brief chopper configuration
 \param ptEpt    		ept handle to operate 
 \param bybyCdiv   		fchop = PCLK/((bybyCdiv+1)*8)
 \param bybyCduty  		(0~7) duty = byduty/8 
 \param byFirstWidth 	T = Tchop * byFirstWidth
 \return none 
*/
void csi_ept_set_cp(csp_ept_t *ptEptBase, uint8_t bybyCdiv, uint8_t bybyCduty, uint8_t byFirstWidth)
{	
	csp_ept_set_cp_div(ptEptBase, bybyCdiv);
	csp_ept_set_cp_duty(ptEptBase, bybyCduty);
	csp_ept_set_cp_ospwth(ptEptBase, byFirstWidth);
}

/**
 \brief channel A chopper output enable 
 \param ptEpt    ept handle to operate
 \param bEnableX/Y  ENABLE/DISABLE
 \return none
*/
void csi_ept_cpa_enable(csp_ept_t *ptEptBase, bool bEnableX, bool bEnableY)
{	
	csp_ept_cpa_enable(ptEptBase, bEnableX, bEnableY);
}

/**
 \brief channel B chopper output enable 
 \param ptEpt    ept handle to operate
 \param bEnableX/Y  ENABLE/DISABLE
 \return none
*/
void csi_ept_cpb_enable(csp_ept_t *ptEptBase, bool bEnableX, bool bEnableY)
{
	csp_ept_cpb_enable(ptEptBase, bEnableX, bEnableY);
}

/**
 \brief channel C chopper output enable 
 \param ptEpt    ept handle to operate
 \param bEnableX/Y  ENABLE/DISABLE
 \return none
*/
void csi_ept_cpc_enable(csp_ept_t *ptEptBase, bool bEnableX, bool bEnableY)
{
	csp_ept_cpc_enable(ptEptBase, bEnableX, bEnableY);
}


/**
 \brief software force lock
 \param ptEpt    ept handle to operate
 \param eEbi 	 external emergency input: EPT_EBI0~4 （EBI4 = LVD）
 \return none
*/
void csi_ept_force_em(csp_ept_t *ptEptBase, csp_ept_ep_e byEbi)
{
	csp_ept_force_em(ptEptBase,byEbi);
}

/**
 \brief get harklock status
 \param ptEpt    ept handle to operate
 \return uint8_t 0b_ _ _ _ _ _ _ _
*/
uint8_t csi_ept_get_hdlck_st(csp_ept_t *ptEptBase)
{	
	return (csp_ept_get_emHdlck(ptEptBase));
}

/**
 \brief clear harklock status
 \param ptEpt    ept handle to operate
 \return  eEbi 	 external emergency input: EPT_EBI0~3/EPT_EBILVD
*/
void csi_ept_clr_hdlck(csp_ept_t *ptEptBase, csi_ept_ebi_e eEbi)
{
	csp_ept_clr_emHdlck(ptEptBase, eEbi);
}

/**
 \brief get 	 softlock status
 \param ptEpt    ept handle to operate
 \return uint8_t 0b_ _ _ _ _ _ _ _
*/
uint8_t csi_ept_get_sftlck_st(csp_ept_t *ptEptBase)
{	
	return (csp_ept_get_emSdlck(ptEptBase));
}

/**
 \brief clear softlock status
 \param ptEpt    ept handle to operate
 \return  eEpi 	 external emergency input: EPT_EBI0~3/EPT_EBILVD
*/
void csp_ept_clr_sftlck(csp_ept_t *ptEptBase, csp_ept_ep_e eEpi)
{	
	csp_ept_clr_emSdlck(ptEptBase, eEpi);
}

/**
  \brief       enable/disable ept in debug mode
  \param[in]   ptEpt       EPT handle to operate
  \param[in]   bEnable		ENABLE/DISABLE
*/

void csi_ept_debug_enable(csp_ept_t *ptEptBase, bool bEnable)
{
	csp_ept_dbg_enable(ptEptBase, bEnable);
}

void csi_ept_emergency_interruption_en(csp_ept_t *ptEptBase, csp_ept_emint_e eEbi)
{
    csp_ept_Emergency_emimcr(ptEptBase,eEbi);
}

/**
  \brief   enable/disable ept out trigger 
  \param   ptEptBase       EPT handle to operate
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
  \param   ptEptBase      EPT handle to operate 
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

void csi_ept_loading_method_aqcsf(csp_ept_t *ptEptBase, csp_ept_aqosf_e bEnable)
{
	ptEptBase ->AQOSF  = (ptEptBase ->AQOSF &~(EPT_AQCSF_LDTIME_MSK))|((bEnable&0x03)<<EPT_AQCSF_LDTIME_POS);
}

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


