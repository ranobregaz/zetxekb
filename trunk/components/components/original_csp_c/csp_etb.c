/***********************************************************************//** 
 * \file  csp_etb.c
 * \brief  ETB description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/

#include <csp_etb.h>

void wj_etb_ch_one_trig_one_config(wj_etb_regs_t *etb_base, uint32_t ch_id, uint8_t src, uint8_t dest,  uint32_t mode)
{
    etb_base->CFG_CHX[ch_id] &= ~(WJ_ETB_CFG_CHX_SRC_SEL_CHX_MSK);
    etb_base->CFG_CHX[ch_id] |= src << WJ_ETB_CFG_CHX_SRC_SEL_CHX_POS;
    etb_base->CFG_CHX[ch_id] &= ~(WJ_ETB_CFG_CHX_DEST_USER_MSK);
    etb_base->CFG_CHX[ch_id] |= dest << WJ_ETB_CFG_CHX_DEST_USER_POS;
    etb_base->CFG_CHX[ch_id] &= ~(WJ_ETB_CFG_CHX_TRIG_MODE_MSK);
    etb_base->CFG_CHX[ch_id] |= (mode << WJ_ETB_CFG_CHX_TRIG_MODE_POS);
    etb_base->CFG_CHX[ch_id] |= WJ_ETB_CFG_CHX_EN_SET;
}

void wj_etb_channel_operate(wj_etb_regs_t *etb_base, uint32_t ch_id, uint32_t operate)
{
    if (ch_id == 0U) {
        etb_base->CFG1_CH0 = ((~WJ_ETB_CFG1_CH0_EN_SET) & (etb_base->CFG1_CH0)) | operate;
    } else if (ch_id == 1U) {
        etb_base->CH1_2[0U].CFG1 = ((~WJ_ETB_CFG1_CHX_EN_SET) & (etb_base->CH1_2[0].CFG1)) | operate;
    } else if (ch_id == 2U) {
        etb_base->CH1_2[1].CFG1 = ((~WJ_ETB_CFG1_CHX_EN_SET) & (etb_base->CH1_2[1].CFG1)) | operate;
    } else if ((ch_id >= 3U) && (ch_id <= 31U)) {
        etb_base->CFG_CHX[ch_id - 3U] = ((~WJ_ETB_CFG_CHX_EN_SET) & (etb_base->CFG_CHX[ch_id - 3U])) | operate;
    }
}


void wj_etb_ch_more_trig_one_config(wj_etb_regs_t *etb_base, uint8_t src0, uint8_t src1, uint8_t src2, uint8_t dest,  uint32_t mode)
{
	etb_base->CFG0_CH0 = src0<< WJ_ETB_CFG0_CH0_SRC0_SEL_CH0_POS | src1<<WJ_ETB_CFG0_CH0_SRC1_SEL_CH0_POS | src2<<WJ_ETB_CFG0_CH0_SRC2_SEL_CH0_POS;
	if (src0 != 0xff)
		etb_base->CFG0_CH0 |= 0x1 << WJ_ETB_CFG0_CH0_SRC0_EN_CH0_POS;
	else
		etb_base->CFG0_CH0 &= ~(0x1 << WJ_ETB_CFG0_CH0_SRC0_EN_CH0_POS);
	if (src1 != 0xff)
		etb_base->CFG0_CH0 |= 0x1 << WJ_ETB_CFG0_CH0_SRC1_EN_CH0_POS ;
	else
		etb_base->CFG0_CH0 &= ~(0x1 << WJ_ETB_CFG0_CH0_SRC1_EN_CH0_POS);
	if (src2 != 0xff)
		etb_base->CFG0_CH0 |= 0x1 << WJ_ETB_CFG0_CH0_SRC2_EN_CH0_POS ;
	else
		etb_base->CFG0_CH0 &= ~(0x1 << WJ_ETB_CFG0_CH0_SRC2_EN_CH0_POS);
		
	etb_base->CFG1_CH0 = dest <<WJ_ETB_CFG1_CH0_DEST_USER_POS | mode << WJ_ETB_CFG1_CH0_TRIG_MODE_POS;
	etb_base->CFG1_CH0 |= WJ_ETB_CFG_CHX_EN_SET;
}

void wj_etb_ch_one_trig_more_config(wj_etb_regs_t *etb_base, uint32_t ch_id, uint32_t src, uint8_t dst0, uint8_t dst1,uint8_t dst2, uint32_t mode)
{

	etb_base->CH1_2[ch_id-1].CFG0 = dst0<<WJ_ETB_CFG0_CHX_DEST0_USER_CHX_POS | dst1 << WJ_ETB_CFG0_CHX_DEST1_USER_CHX_POS | dst2 << WJ_ETB_CFG0_CHX_DEST2_USER_CHX_POS;
	if (dst0 != 0xff)
		etb_base->CH1_2[ch_id-1].CFG0 |= 0x1 << WJ_ETB_CFG0_CHX_DEST0_EN_CHX_POS;
	else {
			etb_base->CH1_2[ch_id-1].CFG0 &= ~(0x7f << WJ_ETB_CFG0_CHX_DEST0_EN_CHX_POS);
			
	}
	if (dst1 != 0xff)
		etb_base->CH1_2[ch_id-1].CFG0 |= 0x1 << WJ_ETB_CFG0_CHX_DEST1_EN_CHX_POS;
	else {
			etb_base->CH1_2[ch_id-1].CFG0 &= ~(0x7f << WJ_ETB_CFG0_CHX_DEST1_EN_CHX_POS);
			
	}
	if (dst2 != 0xff)
		etb_base->CH1_2[ch_id-1].CFG0 |= 0x1 << WJ_ETB_CFG0_CHX_DEST2_EN_CHX_POS;
	else {
			etb_base->CH1_2[ch_id-1].CFG0 &= ~(0x7f << WJ_ETB_CFG0_CHX_DEST2_EN_CHX_POS);
			
	}	
	
 	etb_base->CH1_2[ch_id-1].CFG1 = src << WJ_ETB_CFG1_SRC_SEL_CHX_POS| mode << WJ_ETB_CFG1_TRIG_MODE_CHX_POS;
	etb_base->CH1_2[ch_id-1].CFG1 |= WJ_ETB_CFG_CHX_EN_SET;
}