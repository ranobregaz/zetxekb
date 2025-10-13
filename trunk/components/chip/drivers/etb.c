
/***********************************************************************//** 
 * \file  etb.c
 * \brief  ETCB(event trigger control block) driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-10-8 <td>V0.0  <td>WNN   <td>initial
 * <tr><td> 2021-1-8  <td>V0.1  <td>WNN   <td>SW trigger
 * <tr><td> 2021-5-14 <td>V0.1  <td>ZJY   <td>initial
 * </table>
 * *********************************************************************
*/
#include <sys_clk.h>
#include <drv/etb.h>

/* Private macro------------------------------------------------------*/
/* externs function---------------------------------------------------*/
/* externs variablesr-------------------------------------------------*/
/* variablesr---------------------------------------------------------*/
static uint32_t csi_etb_alloc_status[1];

/** \brief etb channel[0->7] check
 * 
 *  \param[in] eChId: channel id = [0:7]
 *  \return none
 */ 
static int32_t check_is_alloced(csi_etb_chid_e eChId)
{
    uint32_t ch_offset, ch_group;
    int32_t ret = 0;
    uint8_t status = 0U;

    ch_group = (uint32_t)((uint32_t)eChId / 8U);
    ch_offset = (uint32_t)((uint32_t)eChId % 8U);

    status = csi_etb_alloc_status[ch_group];
    status &= (uint8_t)(1U << (uint8_t)ch_offset);

    if (status != 0U)
        ret = -1;

    return ret;
}
/** \brief etb channel[0->7] status 
 * 
 *  \param[in] eChId: channel id = [0:7]
 *  \param[in] status: status
 *  \return none
 */ 
static void set_ch_alloc_status(csi_etb_chid_e eChId, uint32_t status)
{
    uint32_t ch_offset, ch_group;

    ch_group = (uint32_t)((uint32_t)eChId / 8U);
    ch_offset = (uint32_t)((uint32_t)eChId % 8U);

    if (status == 1U) 
        csi_etb_alloc_status[ch_group] |= (uint8_t)(1U << (uint8_t)ch_offset);
	else if (status == 0U) 
        csi_etb_alloc_status[ch_group] &= ~(uint8_t)(1U << (uint8_t)ch_offset);
		
}
/** \brief etb channel[0->7] enable/disable 
 * 
 *  \param[in] ptEtbBase: pionter of ETB reg structure.
 *  \param[in] byChId: channel id = [0:7]
 *  \param[in] bEnable: enable/disable
 *  \return none
 */ 
static void etb_channel_enable(csp_etb_t *ptEtbBase, csi_etb_chid_e byChId, bool bEnable)
{
	switch(byChId)
	{
		case ETB_CH0_ID:		
			ptEtbBase->CFG1_CH0 =((ptEtbBase->CFG1_CH0 & (~ETB_CH_EN_MSK)) | (uint8_t)bEnable);
			break;
		case ETB_CH1_ID:
			ptEtbBase->CH1_2[0].CFG1 =((ptEtbBase->CH1_2[0].CFG1 & (~ETB_CH_EN_MSK)) | (uint8_t)bEnable);
			break;
		case ETB_CH2_ID:
			ptEtbBase->CH1_2[1].CFG1 =((ptEtbBase->CH1_2[1].CFG1 & (~ETB_CH_EN_MSK)) | (uint8_t)bEnable);
			break;
		default:
			if(byChId <= ETB_CH7_ID)
				ptEtbBase->CFG_CHX[byChId-3] = ((ptEtbBase->CFG_CHX[byChId-3] & (~ETB_CH_EN_MSK)) | (uint8_t)bEnable);
			break;
	}
}
/** \brief etb more source trigger one destination
 * 
 *  \param[in] ptEtbBase: pionter of etb reg structure.
 *  \param[in] bySrc0: trigger source 0
 *  \param[in] bySrc1: trigger source 1
 *  \param[in] bySrc2: trigger source 2
 *  \param[in] byDst: trigger destination
 *  \param[in] eTrgMode: hard/soft trigger
 *  \return none
 */ 
static void etb_more_trg_one_set(csp_etb_t *ptEtbBase, uint8_t bySrc0, uint8_t bySrc1, uint8_t bySrc2,uint8_t byDst, etb_ch_trg_mode_e eTrgMode)
{
	ptEtbBase->CFG0_CH0 = (ETB_CH0_TRG_SRC0(bySrc0) | ETB_CH0_TRG_SRC1(bySrc1) | ETB_CH0_TRG_SRC2(bySrc2)); 
	if(bySrc0 != SRC_NOT_USE)
		ptEtbBase->CFG0_CH0 |= ETB_CH0_SRC0_EN;
	else
		ptEtbBase->CFG0_CH0 &= ~ETB_CH0_SRC0_EN_MSK;
	
	if(bySrc1 != SRC_NOT_USE)
		ptEtbBase->CFG0_CH0 |= (ETB_CH0_SRC1_EN << ETB_CH0_SRC1_EN_POS);
	else
		ptEtbBase->CFG0_CH0 &= ~ETB_CH0_SRC1_EN_MSK;
		
	if(bySrc1 != SRC_NOT_USE)
		ptEtbBase->CFG0_CH0 |= (ETB_CH0_SRC2_EN << ETB_CH0_SRC2_EN_POS);
	else
		ptEtbBase->CFG0_CH0 &= ~ETB_CH0_SRC2_EN_MSK;
		
	ptEtbBase->CFG1_CH0 = (eTrgMode << ETB_CH_TRG_MODE_POS) | ETB_CH0_TRG_DST(byDst); 
	ptEtbBase->CFG1_CH0  |= ETB_CH_EN;		//enable etb channel
}
/** \brief etb one source trigger more destination
 * 
 *  \param[in] ptEtbBase: pionter of etb reg structure.
 *  \param[in] byChNum: channel number= [1:2]
 *  \param[in] bySrc: trigger source
 *  \param[in] byDst0: trigger destination 0
 *  \param[in] byDst1: trigger destination 1
 *  \param[in] byDst2: trigger destination 2
 *  \param[in] eTrgMode: hard/soft trigger
 *  \return none
 */ 
static void etb_one_trg_more_set(csp_etb_t *ptEtbBase, uint8_t byChNum, uint8_t bySrc, uint8_t byDst0, uint8_t byDst1, uint8_t byDst2, etb_ch_trg_mode_e eTrgMode)
{
	ptEtbBase->CH1_2[byChNum-1].CFG0 = (ETB_CH1_2_TRG_DST0(byDst0) | ETB_CH1_2_TRG_DST1(byDst1) | ETB_CH1_2_TRG_DST2(byDst2)); 
	if(byDst0 != DST_NOT_USE)
		ptEtbBase->CH1_2[byChNum-1].CFG0 |= (ETB_CH1_2_DST0_EN << ETB_CH1_2_DST0_EN_POS);
	else
		ptEtbBase->CH1_2[byChNum-1].CFG0 &= ~ETB_CH1_2_DST0_EN_MSK;
	
	if(byDst1 != DST_NOT_USE)
		ptEtbBase->CH1_2[byChNum-1].CFG0 |= (ETB_CH1_2_DST1_EN << ETB_CH1_2_DST1_EN_POS);
	else
		ptEtbBase->CH1_2[byChNum-1].CFG0 &= ~ETB_CH1_2_DST1_EN_MSK;
	
	if(byDst2 != DST_NOT_USE)
		ptEtbBase->CH1_2[byChNum-1].CFG0 |= (ETB_CH1_2_DST2_EN << ETB_CH1_2_DST2_EN_POS);
	else
		ptEtbBase->CH1_2[byChNum-1].CFG0 &= ~ETB_CH1_2_DST2_EN_MSK;
		
	ptEtbBase->CH1_2[byChNum-1].CFG1 = (eTrgMode << ETB_CH_TRG_MODE_POS) | ETB_CH1_2_TRG_SRC(bySrc);
	ptEtbBase->CH1_2[byChNum-1].CFG1 |= ETB_CH_EN;	//enable etb channel
}
/** \brief initialize etb; enable etb and etb clk
 * 
 *  \param[in] none
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_etb_init(void)
{
	soc_clk_enable(ETCB_SYS_CLK);			//enable peripheral clk
    csp_etb_enable(ETCB);					//enable etcb module

    return CSI_OK;
}
/** \brief alloc an etb channel
 * 
 *  \param[in] eChType: etb channel work mode
 *  \return channel id or CSI_ERROR
*/
int32_t csi_etb_ch_alloc(csi_etb_ch_type_e eChType)
{
    int32_t ret, ret_ch;
    uint32_t result = csi_irq_save();

    switch (eChType) {
        case ETB_MORE_TRG_ONE:
            ret_ch = 0;
            if (check_is_alloced(ret_ch) < 0) 
                ret = CSI_ERROR;
			else 
                ret = ret_ch;
				
            break;
        case ETB_ONE_TRG_MORE:
            for (ret_ch = 1; ret_ch < 3; ret_ch++) 
			{
                if (check_is_alloced(ret_ch) != -1) 
                    break;
            }

            if (ret_ch == 3) 
                ret = CSI_ERROR;
			else 
                ret = ret_ch;
				
            break;
        case ETB_ONE_TRG_ONE:
            for (ret_ch = 3; ret_ch < 32; ret_ch++) 
			{
                if (check_is_alloced(ret_ch) != -1)
                    break;
            }

            if (ret_ch == 32) 
                ret = CSI_ERROR;
			else 
                ret = ret_ch;
				
            break;
        default:
            ret = CSI_ERROR;
    }

    if (ret != CSI_ERROR) 
        set_ch_alloc_status(ret_ch, 1U);

    csi_irq_restore(result);
    return ret;
}
/** \brief free an etb channel
 * 
 *  \param[in] eChId: etb channel work mode
 *  \return none
*/
void csi_etb_ch_free(csi_etb_chid_e eChId)
{
    uint32_t result = csi_irq_save();
    set_ch_alloc_status(eChId, 0U);
    csi_irq_restore(result);
}
/** \brief config etb channel
 *  \param[in] eChId: etb channel id
 *  \param[in] ptConfig: the config structure pointer for etb channel
 *  \return csi error code
*/
csi_error_t csi_etb_ch_config(csi_etb_chid_e eChId, csi_etb_config_t *ptConfig)
{
    CSI_PARAM_CHK(ptConfig, CSI_ERROR);
	csi_error_t ret = CSI_OK;
	
	switch(ptConfig->eChType)
	{
		case ETB_ONE_TRG_ONE:					//channel num = [3:7]
			if(eChId > ETB_CH2_ID)
			{
				csp_etb_one_trg_one_set(ETCB, eChId, ptConfig->bySrcIp, ptConfig->byDstIp, ptConfig->eTrgMode);
				csp_etb_chx_en(ETCB, eChId);	//enable etb channel 
			}
			else
				ret = CSI_ERROR;

			break;
		case ETB_ONE_TRG_MORE:					//channel num = [1:2]		
			if((eChId == ETB_CH1_ID) || (eChId == ETB_CH2_ID))
				etb_one_trg_more_set(ETCB, eChId, ptConfig->bySrcIp, ptConfig->byDstIp, ptConfig->byDstIp1, ptConfig->byDstIp2,ptConfig->eTrgMode);
			else
				ret = CSI_ERROR;
				
			break;
		case ETB_MORE_TRG_ONE:					//channel num = 0
			if(eChId == ETB_CH0_ID)
				etb_more_trg_one_set(ETCB,ptConfig->bySrcIp, ptConfig->bySrcIp1, ptConfig->bySrcIp2, ptConfig->byDstIp, ptConfig->eTrgMode);
			else
				ret = CSI_ERROR;
			
			break;
		default:
			ret = CSI_ERROR;
			break;
	}

    return ret;
}
/** \brief etb channel sw force triger
 * 
 *  \param[in] eChId: etb channel id
 *  \return none
*/
void csi_etb_ch_swtrig(csi_etb_chid_e eChId)
{
	csp_etb_ch_swtrg_en(ETCB, eChId);
}
/**
  \brief       start an etb channel
  \param[in]   eChId      etb channel id
  \return      none
*/
void csi_etb_ch_start(csi_etb_chid_e eChId)
{
	etb_channel_enable(ETCB,eChId, ENABLE);
}

/**
  \brief       stop an etb channel
  \param[in]   etb        etb channel id
  \return      none
*/
void csi_etb_ch_stop(csi_etb_chid_e eChId)
{
    etb_channel_enable(ETCB, eChId, DISABLE);
}
