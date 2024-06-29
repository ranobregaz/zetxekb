/***********************************************************************//** 
 * \file  csp_adc.c
 * \brief  ADC description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-14 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-8  <td>V0.1  <td>WNN   <td>modify
 * </table>
 * *********************************************************************
*/

/* Includes ---------------------------------------------------------------*/
#include "csp_adc.h"
#include "csp_gpio.h"

/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/


/** \brief adc clk control
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] byState: true: enable; false: disable
 *  \return none
 */  
void csp_adc_set_clk(csp_adc_t *ptAdcBase, bool bEnable)
{
	if(bEnable)
	{
		ptAdcBase->ECR = ADC12_CLKEN;
		while(!(ptAdcBase->PMSR & ADC12_CLKEN));	
	}
	else
	{
		ptAdcBase->DCR = ADC12_CLKEN;
		while((ptAdcBase->PMSR & ADC12_CLKEN));
	}
}
/** \brief adc debug control
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] byState: true: enable; false: disable
 *  \return none
 */  
void csp_adc_set_dbgen(csp_adc_t *ptAdcBase, bool bEnable)
{
	if(bEnable)
	{
		ptAdcBase->ECR = ADC12_DBGEN;
		while(!(ptAdcBase->PMSR & ADC12_DBGEN));	
	}
	else
	{
		ptAdcBase->DCR = ADC12_DBGEN;
		while((ptAdcBase->PMSR & ADC12_DBGEN));
	}
}


/** \brief set adc conversion sequence config
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] bySeqx: 0`17,seq num
 *  \param[in] eAdcChnl: adc input channel
 *  \param[in] eCvcnt: repeat sample count
 *  \param[in] eAvgSel: Average coefficient
 *  \param[in] eAvgctrl: ENABLE,DISABLE
 *  \param 		eSrc	: start trigger src
 *  \return none
 */
void csp_adc_set_seqx(csp_adc_t *ptAdcBase, uint8_t bySeqx,adc_chnl_e eAdcChnl, adc_cnt_e eCvcnt,
									adc_avg_e eAvgSel,bool eAvgctrl, adc_trg_src_e eSrc)
{
	if(bySeqx > 15)
		bySeqx = 15;
	ptAdcBase->SEQ[bySeqx] = eAdcChnl | (eCvcnt<<ADC12_CVCNT_P0S) | (eAvgSel <<ADC12_AVGSEL_POS) | (eAvgctrl<<ADC12_AVGEN_POS) | (eSrc << ADC12_TRG_POS);
}


/** \brief adc interrupt enable/disable
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] eAdcInt: interrupt number
 *  \param[in] bEnable: ENABLE, DISABLE
 *  \return none
 */
void csp_adc_set_int(csp_adc_t *ptAdcBase,adc_int_e eAdcInt,bool bEnable)
{
	if(bEnable)
		ptAdcBase->IER = (0x01ul << eAdcInt);
	else
		ptAdcBase->IDR = (0x01ul << eAdcInt);
}
/** \brief adc clk div set
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] byDiv: 1~62
 *  \return none
 */
void csp_adc_set_clk_div(csp_adc_t *ptAdcBase, uint8_t byDiv)
{
	if(byDiv > 62)
		byDiv = 62;
	ptAdcBase->MR = (ptAdcBase->MR & (~ADC12_PRLVAL_MASK)) | (byDiv >> 1);
}
/** \brief adc clk div get
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \return adc clk div
 */
uint8_t csp_adc_get_clk_div(csp_adc_t *ptAdcBase)
{
	uint8_t div;
	
	div = (uint8_t)(ptAdcBase->MR & 0x1F);
	if(0 == div)
		return 1;
	else
		return (div << 1);
}
/** \brief adc vref source config
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] eVrefSel: vref source
 *  \return none
 */
void csp_adc_set_vref(csp_adc_t *ptAdcBase, adc_vref_e eVrefSel)
{
	ptAdcBase->CR =  (ptAdcBase->CR & (~ADC12_VREF_MSK)) | (eVrefSel<<ADC12_VREF_POS);

}
/** \brief adc cmp0  config
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] wCmpData: cmp0 data
 *  \param[in] byCmpChnl: cmp0 chnl(adc seqx)
 *  \return none
 */
void csp_adc_set_cmp0(csp_adc_t *ptAdcBase, uint32_t wCmpData, uint8_t byCmpChnl)
{
	ptAdcBase->CMP0 = wCmpData;
	ptAdcBase->MR = (ptAdcBase->MR & (~ADC12_NBRCMP0_MASK)) | ADC12_NBRCMP0(byCmpChnl);
} 
/** \brief adc cmp1  config
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] wCmpData: cmp1 data
 *  \param[in] byCmpChnl: cmp1 chnl(adc seqx)
 *  \return none
 */
void csp_adc_set_cmp1(csp_adc_t *ptAdcBase, uint32_t wCmpData, uint8_t byCmpChnl)
{
	ptAdcBase->CMP1 = wCmpData;
	ptAdcBase->MR = (ptAdcBase->MR & (~ADC12_NBRCMP1_MASK)) | ADC12_NBRCMP1(byCmpChnl);
} 

/** \brief adc cmp1  config
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] wCmpData: cmp1 data
 *  \param[in] byCmpChnl: cmp1 chnl(adc seqx)
 *  \return none
 */
uint32_t csp_adc_get_conv_mode(csp_adc_t *ptAdcBase)
{
	if((ptAdcBase->MR & ADC12_CV_MSK)>>ADC12_CV_POS)
		return ADC12_CONTCV;
	else
		return ADC12_OTCV;
}
/** \brief adc sync config
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] eTrgIn: adc sync input channel
 *  \param[in] eTrgMode: sync mode 
 *  \param[in] bEnChnl: sync input channel enable/disable
 *  \return none
 */
void csp_adc_set_sync(csp_adc_t *ptAdcBase, adc_sync_e eTrgIn, adc_evtrg_mode_e eTrgMode, bool bEnChnl)
{
	ptAdcBase->SYNCR = ptAdcBase->SYNCR & (~(0x01 << (8 + eTrgIn)));
	
	if(bEnChnl)
	{
		ptAdcBase->SYNCR |= ((0x01ul << eTrgIn) | (eTrgMode << (8 + eTrgIn)));
		
	}
	else
		ptAdcBase->SYNCR = (ptAdcBase->SYNCR & (~(0x01ul << eTrgIn))) | (eTrgMode << (8 + eTrgIn));
	
}
/** \brief adc input evtrg delay
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] eTrgIn: adc sync evtrg input channel(0~5)
 *  \param[in] byDelay: adc input evtrg delay, delay timer = (byDelay + 1)*4 PCLK
 *  \return none
 */
void csp_adc_set_sync_delay(csp_adc_t *ptAdcBase, adc_sync_e eTrgIn, uint8_t byDelay)
{
	if(eTrgIn < ADC12_SYNCEN3)
	{
		ptAdcBase->TDL0 = (ptAdcBase->TDL0 & ~(0xFFul << (eTrgIn * 8))) | byDelay;
	}
	else if(eTrgIn <= ADC12_SYNCEN5)
	{
		ptAdcBase->TDL1 = (ptAdcBase->TDL1 & ~(0xFFul << ((eTrgIn - 3)  * 8))) | byDelay;
	}
}
/** \brief adc evtrg ouput config
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] eEvSrc: adc evtrg source(0~23)
 *  \param[in] eEvOut: adc evtrg output, TRG0/TRG1
 *  \return none
 */
void csp_adc_set_evtrg(csp_adc_t *ptAdcBase, adc_evtrg_src_e eEvSrc, adc_evtrg_out_e eEvOut)
{
	if(eEvOut == ADC12_TRG0_SEL)
	{
		ptAdcBase->EVTRG = (ptAdcBase->EVTRG & (~ADC12_TRGSRC0_MSK)) | (eEvSrc << ADC12_TRGSRC0_POS);
		
	}
	else if(eEvOut == ADC12_TRG1_SEL)
	{
		ptAdcBase->EVTRG = (ptAdcBase->EVTRG & (~ADC12_TRGSRC1_MSK)) | (eEvSrc << ADC12_TRGSRC1_POS);
	}
}
/** \brief adc evtrg output enable
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] eEvOut: adc evtrg output, TRG0/TRG1
 *  \return none
 */
void csp_adc_evtrg_en(csp_adc_t *ptAdcBase, adc_evtrg_out_e eEvOut)
{
	if(eEvOut == ADC12_TRG0_SEL) 
		ptAdcBase->EVTRG |= ADC12_TRG0OE;
	else if(eEvOut == ADC12_TRG1_SEL)
		ptAdcBase->EVTRG |= ADC12_TRG1OE;
}
/** \brief adc evtrg output disable
 * 
 *  \param[in] ptAdcBase: pointer of ADC reg structure.
 *  \param[in] eEvOut: adc evtrg output, TRG0/TRG1
 *  \return none
 */
void csp_adc_evtrg_dis(csp_adc_t *ptAdcBase, adc_evtrg_out_e eEvOut)
{
	if(eEvOut == ADC12_TRG0_SEL) 
		ptAdcBase->EVTRG &= (~ADC12_TRG0OE);
	else if(eEvOut == ADC12_TRG1_SEL)
		ptAdcBase->EVTRG &= (~ADC12_TRG1OE);
}

