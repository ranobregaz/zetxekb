/***********************************************************************//** 
 * \file  csp_etb.h
 * \brief ETB description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-4 <td>V0.0  <td>WNN     <td>initial
 * <tr><td> 2021-5-14 td>V0.0  <td>ZJY     <td>initial
 * </table>
 * *********************************************************************
*/

#ifndef _WJ_ETB_LL_H_
#define _WJ_ETB_LL_H_

#include <stdio.h>
#include <soc.h>
#include <csi_core.h>

/*
 * Usage
 * 1. Configure dest trigger user IP1, and enable the IP1
 * 2. Configure channel in the ETB
 *      1)source-trigger select
 *      2)dest-trigger select
 *      3)mode select
 *      4)channel enable
 *      5)ETB enable
 *      6)softtrigger if need
 * 3. Configure source trigger generator IP2, and enable the IP2
 * For example, etb channel3 for timer trigger adc working, and etb channel4 for adc trigger dma to carry data
 * 1)configure adc and dma IP first and waiting trigger signal.
 * 2)configure etb channel3 and channel4 include source,dest,mode select.
 * 3)enable etb channel3 and channel4
 * 4)enable etb
 * 5)configure timer and enable
 * Note:The GPIO can be configured as level or edge mode. when using GPIO trigger, the GPIO must be level mode.
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    __IOM uint32_t CFG0;
    __IOM uint32_t CFG1;
} wj_etb_ch1_2_regs_t;

/// \struct csp_etb_t
/// \brief ETB Reg Description     
typedef struct {
    __IOM uint32_t ETB_EN;          //0x00 (R/W) ETB enable Register                   
    __IOM uint32_t SOFTTRIG;        //0x04 (R/W) Software trigger Configure Register   
    __IOM uint32_t CFG0_CH0;        //0x08 (R/W) ETB Channel0 Configure0 Register       
    __IOM uint32_t CFG1_CH0;        //0x0c (R/W) ETB Channel0 Configure1 Register      
    wj_etb_ch1_2_regs_t CH1_2[2];   //0x10 ~ 0x14 (R/W) ETB Channel1 Configure0~1 Register; 0x18 ~ 0x1C (R/W) ETB Channel2 Configure0~1 Register 
    uint32_t RESERVED0[4];
    __IOM uint32_t CFG_CHX[29];     // 0x30~0xa0 (R/W) ETB CH_3~CH_31 Configure Register */
} csp_etb_t;

/******************************************************************************
* XXX : ETB trigger mode and channel enable
******************************************************************************/
#define	ETB_CH_EN_POS			(0U)	//CH(0->7) enable/disable		
#define	ETB_CH_EN_MSK			(0x01ul << ETB_CH_EN_POS)
typedef enum
{
	ETB_CH_DIS			= 0,	
	ETB_CH_EN	
}etb_ch_en_e;

#define	ETB_CH_TRG_MODE_POS	(1U)		//CH(0->7) trigger mode select
#define	ETB_CH_TRG_MODE_MSK	(0x01ul << ETB_CH_TRG_MODE_POS)
typedef enum
{
	ETB_CH_TRG_HARD	= 0,				//Hardware trigger	
	ETB_CH_TRG_SOFT						//Software trigger	
}etb_ch_trg_mode_e;

/******************************************************************************
* ENABLE : ETB Enable Register
******************************************************************************/
#define	ETB_ENABLE_POS			(0U)	//ETCB enable/disable
#define	ETB_ENABLE_MSK			(0x01ul << ETB_ENABLE_POS)
typedef enum
{
	ETB_DISABLE			= 0,	
	ETB_ENABLE	
}etb_enable_e;

/******************************************************************************
*SWTRG : ETB Software Trigger Register
******************************************************************************/
#define ETB_SWTRG_CH_POS(n)		(n)
#define ETB_SWTRG_CH_MSK(n)		(0x01ul << ETB_SWTRG_CH_POS(n))
#define ETB_SWTRG_CH_SET(n)     ETB_SWTRG_CH_MSK(n)

/******************************************************************************
*CH0CON0 : ETB Channel0 Control Register0
******************************************************************************/
//source 0
#define	ETB_CH0_SRC0_EN_POS		(0U)	//CH0 trigger source0 enable/disable		
#define	ETB_CH0_SRC0_EN_MSK		(0x01ul << ETB_CH0_SRC0_EN_POS)
typedef enum
{
	ETB_CH0_SRC0_DIS	= 0,	
	ETB_CH0_SRC0_EN	
}etb_ch0_src0_en_e;
#define	ETB_CH0_TRG_SRC0_POS	(1U)	//CH0 trigger source0 select
#define	ETB_CH0_TRG_SRC0_MSK	(0x3Ful << ETB_CH0_TRG_SRC0_POS)
#define	ETB_CH0_TRG_SRC0(val)	(((val) & 0x3Ful) << ETB_CH0_TRG_SRC0_POS)

//source 1
#define	ETB_CH0_SRC1_EN_POS		(10U)	//CH0 trigger source1 enable/disable		
#define	ETB_CH0_SRC1_EN_MSK		(0x01ul << ETB_CH0_SRC1_EN_POS)
typedef enum
{
	ETB_CH0_SRC1_DIS	= 0,	
	ETB_CH0_SRC1_EN	
}etb_ch0_src1_en_e;
#define	ETB_CH0_TRG_SRC1_POS	(11U)	//CH0 trigger source1 select
#define	ETB_CH0_TRG_SRC1_MSK	(0x3Ful << ETB_CH0_TRG_SRC1_POS)
#define	ETB_CH0_TRG_SRC1(val)	(((val) & 0x3Ful) << ETB_CH0_TRG_SRC1_POS)

//source 2
#define	ETB_CH0_SRC2_EN_POS		(20U)	//CH0 trigger source2 enable/disable		
#define	ETB_CH0_SRC2_EN_MSK		(0x01ul << ETB_CH0_SRC2_EN_POS)
typedef enum
{
	ETB_CH0_SRC2_DIS	= 0,	
	ETB_CH0_SRC2_EN	
}etb_ch0_src2_en_e;
#define	ETB_CH0_TRG_SRC2_POS	(21U)	//CH0 trigger source2 select
#define	ETB_CH0_TRG_SRC2_MSK	(0x3Ful << ETB_CH0_TRG_SRC2_POS)
#define	ETB_CH0_TRG_SRC2(val)	(((val) & 0x3Ful) << ETB_CH0_TRG_SRC2_POS)

/******************************************************************************
*CH0CON1 : ETB Channel0 Control Register1
******************************************************************************/
//#define	ETB_CH0_EN_POS			(0U)	//CH0 enable/disable		
//#define	ETB_CH0_EN_MSK			(0x01ul << ETB_CH0_EN_POS)
//typedef enum
//{
//	ETB_CH0_DIS			= 0,	
//	ETB_CH0_EN	
//}etb_ch0_en_e;
//
//#define	ETB_CH0_TRG_MODE_POS	(1U)	//CH0 trigger mode select
//#define	ETB_CH0_TRG_MODE_MSK	(0x01ul << ETB_CH0_TRG_MODE_POS)
//typedef enum
//{
//	ETB_CH0_TRG_HARD	= 0,			//Hardware trigger	
//	ETB_CH0_TRG_SOFT					//Software trigger	
//}etb_ch0_trg_mode_e;

#define	ETB_CH0_TRG_DST_POS		(26U)	//CH0 trigger destination select
#define	ETB_CH0_TRG_DST_MSK		(0x3Ful << ETB_CH0_TRG_DST_POS)
#define	ETB_CH0_TRG_DST(val)	(((val) & 0x3Ful) << ETB_CH0_TRG_DST_POS)

/******************************************************************************
*CHxCON0 : ETB Channel x Control Register0,(x = 1->2)
******************************************************************************/
//destination 0
#define	ETB_CH1_2_DST0_EN_POS	(0U)	//CH1_2 trigger destination0 enable/disable		
#define	ETB_CH1_2_DST0_EN_MSK	(0x01ul << ETB_CH1_2_DST0_EN_POS)
typedef enum
{
	ETB_CH1_2_DST0_DIS	= 0,	
	ETB_CH1_2_DST0_EN	
}etb_chx_dst0_en_e;
#define	ETB_CH1_2_TRG_DST0_POS	(1U)	//CH1_2trigger destination0 select
#define	ETB_CH1_2_TRG_DST0_MSK	(0x3Ful << ETB_CH1_2_TRG_DST0_POS)
#define	ETB_CH1_2_TRG_DST0(val)	(((val) & 0x3Ful) << ETB_CH1_2_TRG_DST0_POS)

//destination 1
#define	ETB_CH1_2_DST1_EN_POS	(10U)	//CH1_2 trigger destination1 enable/disable		
#define	ETB_CH1_2_DST1_EN_MSK	(0x01ul << ETB_CH1_2_DST1_EN_POS)
typedef enum
{
	ETB_CH1_2_DST1_DIS	= 0,	
	ETB_CH1_2_DST1_EN	
}etb_chx_dst1_en_e;
#define	ETB_CH1_2_TRG_DST1_POS	(11U)	//CH1_2 trigger destination1 select
#define	ETB_CH1_2_TRG_DST1_MSK	(0x3Ful << ETB_CH1_2_TRG_DST1_POS)
#define	ETB_CH1_2_TRG_DST1(val)	(((val) & 0x3Ful) << ETB_CH1_2_TRG_DST1_POS)

//destination 2
#define	ETB_CH1_2_DST2_EN_POS	(20U)	//CH1_2 trigger destination2 enable/disable		
#define	ETB_CH1_2_DST2_EN_MSK	(0x01ul << ETB_CH1_2_DST2_EN_POS)
typedef enum
{
	ETB_CH1_2_DST2_DIS	= 0,	
	ETB_CH1_2_DST2_EN	
}etb_chx_dst2_en_e;
#define	ETB_CH1_2_TRG_DST2_POS	(21U)	//CH1_2 trigger destination2 select
#define	ETB_CH1_2_TRG_DST2_MSK	(0x3Ful << ETB_CH1_2_TRG_DST2_POS)
#define	ETB_CH1_2_TRG_DST2(val)	(((val) & 0x3Ful) << ETB_CH1_2_TRG_DST2_POS)

/******************************************************************************
*CHxCON1 : ETB Channel x Control Register1,(x = 1->2)
******************************************************************************/
//#define	ETB_CH1_2_EN_POS		(0U)	//CH1_2 enable/disable		
//#define	ETB_CH1_2_EN_MSK		(0x01ul << ETB_CH1_2_EN_POS)
//typedef enum
//{
//	ETB_CH1_2_DIS		= 0,	
//	ETB_CH1_2_EN	
//}etb_ch1_2_en_e;
//
//#define	ETB_CH1_2_TRG_MODE_POS	(1U)	//CH1_2 trigger mode select
//#define	ETB_CH1_2_TRG_MODE_MSK	(0x01ul << ETB_CH1_2_TRG_MODE_POS)
//typedef enum
//{
//	ETB_CH1_2_TRG_HARD	= 0,			//Hardware trigger	
//	ETB_CH1_2_TRG_SOFT					//Software trigger	
//}etb_ch1_2_trg_mode_e;

#define	ETB_CH1_2_TRG_SRC_POS	(26U)	//CH1_2 trigger source select
#define	ETB_CH1_2_TRG_SRC_MSK	(0x3Ful << ETB_CH1_2_TRG_SRC_POS)
#define	ETB_CH1_2_TRG_SRC(val)	(((val) & 0x3Ful) << ETB_CH1_2_TRG_SRC_POS)

/******************************************************************************
*CHxCON : ETB Channel x Control Register,(x = 3->7)
******************************************************************************/
//#define	ETB_CHX_EN_POS			(0U)	//CHx(3->7) enable/disable		
//#define	ETB_CHX_EN_MSK			(0x01ul << ETB_CHX_EN_POS)
//typedef enum
//{
//	ETB_CHX_DIS			= 0,	
//	ETB_CHX_EN	
//}etb_chx_en_e;
//
//#define	ETB_CHX_TRG_MODE_POS	(1U)	//CHx(3->7) trigger mode select
//#define	ETB_CHX_TRG_MODE_MSK	(0x01ul << ETB_CHX_TRG_MODE_POS)
//typedef enum
//{
//	ETB_CHX_TRG_HARD	= 0,			//Hardware trigger	
//	ETB_CHX_TRG_SOFT					//Software trigger	
//}etb_chx_trg_mode_e;

//source 
#define	ETB_CHX_TRG_SRC_POS		(12U)	//CHx(3->7) trigger source select
#define	ETB_CHX_TRG_SRC_MSK		(0x3Ful << ETB_CHX_TRG_SRC_POS)
#define	ETB_CHX_TRG_SRC(val)	(((val) & 0x3Ful) << ETB_CHX_TRG_SRC_POS)

//destination
#define	ETB_CHX_TRG_DST_POS		(26U)	//CHx(3->7) trigger destination select
#define	ETB_CHX_TRG_DST_MSK		(0x3Ful << ETB_CHX_TRG_DST_POS)
#define	ETB_CHX_TRG_DST(val)	(((val) & 0x3Ful) << ETB_CHX_TRG_DST_POS)

/******************************************************************************
********************** ETCB inline Functions Declaration **********************
******************************************************************************/
static inline void csp_etb_enable(csp_etb_t *ptEtbBase)
{
    ptEtbBase->ETB_EN = (ETB_ENABLE << ETB_ENABLE_POS);
}

static inline void csp_etb_disable(csp_etb_t *ptEtbBase)
{
    ptEtbBase->ETB_EN &= ~(ETB_ENABLE_MSK);
}

//channel num = 0
//static inline void csp_etb_ch0_en(csp_etb_t *ptEtbBase)
//{
//    ptEtbBase->CFG1_CH0 |= ETB_CH_EN;
//}

//channel num = 1 or 2
//static inline void csp_etb_ch1_2_en(csp_etb_t *ptEtbBase, uint8_t byChNum)
//{
//    ptEtbBase->CH1_2[byChNum-1].CFG1 |= ETB_CH_EN;
//}

//chx enable: x > 2; channel num > 2
static inline void csp_etb_chx_en(csp_etb_t *ptEtbBase, uint8_t byChNum)
{
    ptEtbBase->CFG_CHX[byChNum-3] |= ETB_CH_EN;
}

static inline void csp_etb_soft_trigrer(csp_etb_t *ptEtbBase, uint8_t byChNum)
{
    if (byChNum >= 3U) 
        ptEtbBase->CFG_CHX[byChNum - 3U] |= (ETB_CH_TRG_SOFT << ETB_CH_TRG_MODE_POS);
}

static inline void csp_etb_hard_trigger(csp_etb_t *ptEtbBase, uint8_t byChNum)
{
    if (byChNum >= 3U)
        ptEtbBase->CFG_CHX[byChNum - 3U] &= ~(ETB_CH_TRG_MODE_MSK);
}

static inline void csp_etb_ch_swtrg_en(csp_etb_t *ptEtbBase, uint8_t byChNum)
{
    ptEtbBase->SOFTTRIG |= ETB_SWTRG_CH_SET(byChNum);
}

static inline void csp_etb_one_trg_one_set(csp_etb_t *ptEtbBase, uint8_t byChNum, uint8_t bySrc, uint8_t byDst, etb_ch_trg_mode_e eTrgMode)
{
	ptEtbBase->CFG_CHX[byChNum-3] = (eTrgMode << ETB_CH_TRG_MODE_POS) | ETB_CHX_TRG_SRC(bySrc) | ETB_CHX_TRG_DST(byDst);
}

//static void csp_etb_more_trg_one_set(csp_etb_t *ptEtbBase, uint8_t bySrc0, uint8_t bySrc1, uint8_t bySrc2,uint8_t byDst, etb_ch_trg_mode_e eTrgMode)
//{
//	ptEtbBase->CFG0_CH0 = (ETB_CH0_TRG_SRC0(bySrc0) | ETB_CH0_TRG_SRC1(bySrc1) | ETB_CH0_TRG_SRC2(bySrc2)) | 
//			(ETB_CH0_SRC0_EN | (ETB_CH0_SRC1_EN << ETB_CH0_SRC1_EN_POS) | (ETB_CH0_SRC2_EN << ETB_CH0_SRC2_EN_POS));
//	ptEtbBase->CFG1_CH0 = (eTrgMode << ETB_CH_TRG_MODE_POS) | ETB_CH0_TRG_DST(byDst);
//}
//
//static void csp_etb_one_trg_more_set(csp_etb_t *ptEtbBase, uint8_t byChNum, uint8_t bySrc, uint8_t byDst0, uint8_t byDst1, uint8_t byDst2, etb_ch_trg_mode_e eTrgMode)
//{
//	ptEtbBase->CH1_2[byChNum-1].CFG0 = (ETB_CH1_2_TRG_DST0(byDst0) | ETB_CH1_2_TRG_DST1(byDst1) | ETB_CH1_2_TRG_DST2(byDst2)) | 
//			(ETB_CH1_2_DST0_EN | (ETB_CH1_2_DST1_EN << ETB_CH1_2_DST1_EN_POS) | (ETB_CH1_2_DST2_EN << ETB_CH1_2_DST2_EN_POS));
//	ptEtbBase->CH1_2[byChNum-1].CFG1 = (eTrgMode << ETB_CH_TRG_MODE_POS) | ETB_CH1_2_TRG_SRC(bySrc);
//}

/******************************************************************************
********************** BT External Functions Declaration **********************
******************************************************************************/
//void wj_etb_ch_one_trig_one_config(csp_etb_t *etb_base, uint32_t ch_idx, uint8_t src, uint8_t dest,  uint32_t mode);
//void wj_etb_ch_more_trig_one_config(csp_etb_t *etb_base, uint8_t src0, uint8_t src1, uint8_t src2, uint8_t dest,  uint32_t mode);
//void wj_etb_ch_one_trig_more_config(csp_etb_t *etb_base, uint32_t ch_id, uint32_t src, uint8_t dst0, uint8_t dst1,uint8_t dst2, uint32_t mode);
//void wj_etb_channel_operate(csp_etb_t *etb_base, uint32_t ch_idx, uint32_t operate);

#ifdef __cplusplus
}
#endif

#endif /* _WJ_ETB_LL_H_ */
