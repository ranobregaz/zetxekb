/***********************************************************************//** 
 * \file  csp_lpt.h
 * \brief LPT description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-01 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-8  <td>V0.1  <td>WNN   <td>modify
 * <tr><td> 2021-5-20 <td>V0.2  <td>YYM   <td>modify
 * </table>
 * *********************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CSP_LPT_H
#define _CSP_LPT_H

/* Includes ------------------------------------------------------------------*/
#include <soc.h>

/// \struct csp_lpt_t
/// \brief LPT reg description   
typedef struct
{
   __IOM uint32_t  CEDR;  		//0x0000	Clock control & ID
   __IOM uint32_t  RSSR;		//0x0004	Start & Stop Ctrl
   __IOM uint32_t  PSCR;		//0x0008	Clock prescaler
   __IOM uint32_t  CR;  		//0x000C	Control register
   __IOM uint32_t  SYNCR;		//0x0010	Synchronization control reg
   __IOM uint32_t  PRDR;   		//0x0014	Period reg
   __IOM uint32_t  CMP;   		//0x0018	Compare Value A
   __IOM uint32_t  CNT;      	//0x001C	Counter reg
   __IOM uint32_t  TRGFCR;    	//0x0020    Trigger Filter control reg
   __IOM uint32_t  TRGFWR;    	//0x0024    Trigger filter window
   __IOM uint32_t  EVTRG;      	//0x0028    Event trigger setting
   __IOM uint32_t  EVPS;       	//0x002C    Event presaler
   __IOM uint32_t  EVSWF;      	//0x0030    Event software forcing
   __IM  uint32_t  RISR;       	//0x0034    Interrupt RISR
   __IM  uint32_t  MISR;       	//0x0038    Interrupt MISR
   __IOM uint32_t  IMCR;       	//0x003C    Interrupt IMCR
   __OM  uint32_t  ICR;        	//0x0040    Interrupt clear
} csp_lpt_t;

/*****************************************************************************
************************** bt Function defined *******************************
******************************************************************************/
#define LPT_RESET_VALUE  (0x00000000)

/******************************************************************************
* CEDR : Clock control & ID  Register
******************************************************************************/
#define	LPT_CLK_POS			(0)
#define	LPT_CLK_MSK			(0x01ul << LPT_CLK_POS)


#define LPT_DBG_POS			(1)
#define LPT_DBG_MSK			(0x1 << LPT_DBG_POS)

#define	LPT_CSS_POS			(2)
#define	LPT_CSS_MSK			(0x07ul << LPT_CSS_POS)
typedef enum
{
	LPT_PCLK_DIV4 = 	(0x00ul),
    LPT_ISCLK = 		(0x01ul ),
	LPT_IMCLK_DIV4 = 	(0x02ul),
	LPT_EMCLK = 		(0x03ul ),
	LPT_IN_RISE = 		(0x04ul ),
	LPT_IN_FALL = 		(0x05ul),	
}lpt_css_e;

#define	LPT_SHDWSTP_POS		(6)
#define	LPT_SHDWSTP_MSK		(0x01ul << LPT_SHDWSTP_POS)
typedef enum
{
    LPT_START_SHADOW = 		(0x00ul ),
	LPT_START_IMMEDIATE= 	(0x01ul),
}lpt_shdwstp_e;


#define	LPT_FLTCKPRS_POS	(8)
#define	LPT_FLTCKPRS_MSK	(0xFFul << LPT_FLTCKPRS_POS)
#define	LPT_FLTCKPRS(val)	(((val) & 0xFFul) << LPT_FLTCKPRS_POS)

#define	LPT_IDCODE_POS		(15)
#define	LPT_IDCODE_MSK		(0xFFul << LPT_IDCODE_POS)
#define	LPT_IDCODE(val)		(((val) & 0xFFul) << LPT_IDCODE_POS)

/******************************************************************************
* RSSR : Start & Stop Ctrl  Register
******************************************************************************/
#define	LPT_STR_POS			(0)
#define	LPT_STR_MSK			(0x01ul << LPT_STR_POS)
typedef enum
{
    LPT_STOP = 			(0x00ul ),
	LPT_START = 		(0x01ul )
}lpt_str_e;

#define	LPT_SRR_POS			(12)
#define	LPT_SRR_MSK			(0x0Ful << LPT_SRR_POS)
#define	LPT_SRR_EN			(0x05ul << LPT_SRR_POS)

/******************************************************************************
* PSCR : Clock prescaler
******************************************************************************/
#define	LPT_PSCDIV_POS			(0)
#define	LPT_PSCDIV_MSK			(0x0Ful << LPT_STR_POS)
typedef enum
{
	LPT_PSC_DIV1 = 		(0x00 ),
	LPT_PSC_DIV2 = 		(0x01 ),
	LPT_PSC_DIV4 = 		(0x02 ),
    LPT_PSC_DIV8 = 		(0x03 ),
	LPT_PSC_DIV16 = 	(0x04 ),
	LPT_PSC_DIV32 = 	(0x05 ),
	LPT_PSC_DIV64 = 	(0x06 ),
	LPT_PSC_DIV128 = 	(0x07 ),
    LPT_PSC_DIV256 = 	(0x08 ),
	LPT_PSC_DIV512 = 	(0x09 ),
	LPT_PSC_DIV1024 = 	(0x0A ),
	LPT_PSC_DIV2048 = 	(0x0B ),
	LPT_PSC_DIV4096 = 	(0x0C )	
}lpt_pscdiv_e;

/******************************************************************************
* CR : Control register
******************************************************************************/
#define	LPT_SWSYNC_POS		(2)
#define	LPT_SWSYNC_MSK		(0x01ul << LPT_SWSYNC_POS)


#define	LPT_IDLEST_POS		(3)
#define	LPT_IDLEST_MSK		(0x01ul << LPT_IDLEST_POS)
typedef enum
{
    LPTIDLE_Z = 		(0x00ul ),
	LPTIDLE_L = 		(0x01ul )
}lpt_idlest_e;

#define	LPT_PRDLD_POS		(4)
#define	LPT_PRDLD_MSK		(0x01ul << LPT_PRDLD_POS)
typedef enum
{
    LPT_PRDLD_IM = 		(0x00ul),
	LPT_PRDLD_END = 	(0x01ul)
}lpt_prdld_e;

#define	LPT_POL_POS			(5)
#define	LPT_POL_MSK			(0x01ul << LPT_POL_POS)
typedef enum
{
    LPTPOL_HIGH = 		(0x00ul),
	LPTPOL_LOW = 		(0x01ul)
}lpt_pol_e;

#define	LPT_OPM_POS			(6)
#define	LPT_OPM_MSK			(0x01ul << LPT_OPM_POS)
typedef enum
{
    LPT_OPM_CONTINUOUS = 	(0x00ul ),
	LPT_OPM_ONCE = 		 	(0x01ul )
}lpt_opm_e;

#define	LPT_FLT_POS			(10)
#define	LPT_FLT_MSK			(0x01ul << LPT_FLT_POS)
#define	LPT_FLT_INIT  		(0x01ul << LPT_FLT_POS);

#define	LPT_FLTDEB_POS		(13)
#define	LPT_FLTDEB_MSK		(0x07ul << LPT_FLTDEB_POS)
typedef enum
{
    LPT_FLTDEB_00 = 	(0x00ul ),         
	LPT_FLTDEB_02 = 	(0x01ul ), 
    LPT_FLTDEB_03 = 	(0x02ul ),         
	LPT_FLTDEB_04 = 	(0x03ul ), 
    LPT_FLTDEB_06 = 	(0x04ul ),         
	LPT_FLTDEB_08 = 	(0x05ul ), 
    LPT_FLTDEB_16 = 	(0x06ul ),         
	LPT_FLTDEB_32 = 	(0x07ul ), 	
}lpt_fltdeb_e;


#define	LPT_PSCLD_POS		(16)
#define	LPT_PSCLD_MSK		(0x01ul << LPT_PSCLD_POS)
typedef enum
{
    LPT_PSCLD_START = 		(0x00ul ),
	LPT_PSCLD_START_PEND = 		(0x01ul )
}lpt_pscld_e;

#define	LPT_CMPLD_POS		(17)
#define	LPT_CMPLD_MSK		(0x01ul << LPT_CMPLD_POS)
typedef enum
{
    LPT_CMPLD_IM = 		(0x00ul ),
	LPT_CMPLD_END = 	(0x01ul )
}lpt_cmpld_e;

#define LPT_OUT_EN_POS	31
#define LPT_OUT_EN_MSK	(0x1<<LPT_OUT_EN_POS)


/******************************************************************************
* SYNCR : Synchronization control reg
******************************************************************************/
#define LPT_SYNCEN_POS		(0)
#define LPT_SYNCEN_MSK		(0x1 << LPT_SYNCEN_POS)


#define	LPT_OSTMD_POS		(8)
#define	LPT_OSTMD_MSK		(0x01ul << LPT_OSTMD_POS)
typedef enum
{
    LPT_SYNC_CT= (0x00ul ),  	       
	LPT_SYNC_OT =  (0x01ul)	
}lpt_syncmd_e;

#define	LPT_REARM_POS		(16)
#define	LPT_REARM_MSK		(0x01ul << LPT_REARM_POS)
#define LPT_REARM			(0x01ul << LPT_REARM_POS)
 

#define	LPT_AREARM_POS		(30)
#define	LPT_AREARM_MSK		(0x01ul << LPT_AREARM_POS)


/******************************************************************************
* TRGFTCR : Trigger Filter control reg
******************************************************************************/
#define	LPT_SRCSEL_POS		(0)
#define	LPT_SRCSEL_MSK		(0x01ul << LPT_SRCSEL_POS)


#define	LPT_BLKINV_POS		(4)
#define	LPT_BLKINV_MSK		(0x01ul << LPT_BLKINV_POS)


#define	LPT_CROSS_POS		(7)
#define	LPT_CROSS_MSK		(0x01ul << LPT_CROSS_POS)



/******************************************************************************
* EVTRG : Event trigger setting
******************************************************************************/
#define	LPT_TRGSRC_POS		(0)
#define	LPT_TRGSRC_MSK		(0x0ful << LPT_TRGSRC_POS)
typedef enum
{
    LPT_TRGSRC0_DIS = 		(0x00ul ),  	       
	LPT_TRGSRC0_ZRO = 		(0x01ul ),
	LPT_TRGSRC0_PRD = 		(0x02ul),
	LPT_TRGSRC0_ZRO_PRD = 	(0x03ul ),
	LPT_TRGSRC0_CMP = 		(0x04ul )
}lpt_trgsrc_e;

#define	LPT_TRG0OE_POS		(20)
#define	LPT_TRG0OE_MSK		(0x01ul << LPT_TRG0OE_POS)


/******************************************************************************
* RISR/MISR/IMCR/ICR: Interrupt RISR/MISR/IMCR/ICR
******************************************************************************/
typedef enum
{
    LPT_TRGEV_INT  = 	(0),  	       
	LPT_MATCH_INT = 	(1),	
	LPT_PEND_INT  = 	(2), 
	LPT_NONE_INT = 	    (3) 	       
}lpt_int_e;

/******************************************************************************
* TRGFTWR : Trigger filter window  
******************************************************************************/
#define	LPT_OFFSET_POS		(0)
#define	LPT_OFFSET_MSK		(0xFFFFul << LPT_OFFSET_POS)
#define	LPT_OFFSET(val)		(((val) & 0xFFFFul) << LPT_OFFSET_POS)

#define	LPT_WINDOW_POS		(16)
#define	LPT_WINDOW_MSK		(0xFFFFul << LPT_WINDOW_POS)
#define	LPT_WINDOW(val)		(((val) & 0xFFFFul) << LPT_WINDOW_POS)

/******************************************************************************
* EVPS : Event presaler
******************************************************************************/
#define	LPT_TRGEVPRD_POS		(0)
#define	LPT_TRGEVPRD_MSK		(0x0Ful << LPT_TRGEVPRD_POS)
#define	LPT_TRGEVPRD(val)		(((val) & 0x0Ful) << LPT_TRGEVPRD_POS)

#define	LPT_TRGEVCNT_POS		(16)
#define	LPT_TRGEVCNT_MSK		(0x0Ful << LPT_TRGEVCNT_POS)
#define	LPT_TRGEVCNT(val)		(((val) & 0x0Ful) << LPT_TRGEVCNT_POS)

/******************************************************************************
* EVSWF : Event software forcing
******************************************************************************/
#define	LPT_EVSWF_POS		(0)
#define	LPT_EVSWF_MSK		(0x01 << LPT_EVSWF_POS)
#define	LPT_EVSWF		(0x01 << LPT_EVSWF_POS)

/******************************************************************************
* PRDR : Period reg
******************************************************************************/
#define	LPT_PRDR_POS		(0)
#define	LPT_PRDR_MSK		(0xFFFFul << LPT_PRDR_POS)

/******************************************************************************
* CMP : Compare Value A
******************************************************************************/
#define	LPT_CMP_POS			(0)
#define	LPT_CMP_MSK			(0xFFFFul << LPT_CMP_POS)

/******************************************************************************
* CNT : Counter reg
******************************************************************************/
#define	LPT_CNT_POS			(0)
#define	LPT_CNT_MSK			(0xFFFFul << LPT_CNT_POS)

		
/******************************************************************************
********************** LPT inline Functions Declaration ***********************
******************************************************************************/
static inline void csp_lpt_int_enable(csp_lpt_t *ptLptBase, lpt_int_e eLptInt,bool bEnable)
{
	if(bEnable)
		ptLptBase->IMCR |= 0x1 << eLptInt; 
	else
		ptLptBase->IMCR &= ~(0x1 <<eLptInt); 
}

static inline void csp_lpt_data_update(csp_lpt_t *ptLptBase,uint16_t hwPrdr, uint16_t hwCmp)
{
	ptLptBase->PRDR = hwPrdr;
	ptLptBase->CMP = hwCmp;		
}

static inline uint8_t csp_lpt_get_work_state(csp_lpt_t *ptLptBase)
{
	if(ptLptBase->RSSR & LPT_START)
		return ENABLE;
	else
		return DISABLE;
		
}

static inline void csp_lpt_set_evtrg(csp_lpt_t *ptLptBase, lpt_trgsrc_e eTrgSrc)
{
	ptLptBase->EVTRG = (ptLptBase->EVTRG & ~LPT_TRGSRC_MSK) |(eTrgSrc << LPT_TRGSRC_POS);
}

static inline void csp_lpt_swf_trg(csp_lpt_t *ptLptBase, uint8_t byTrg)
{
	ptLptBase->EVSWF = LPT_EVSWF;
}

static inline void csp_lpt_sync_enable(csp_lpt_t *ptLptBase,bool bEnable)
{
	ptLptBase->SYNCR = (ptLptBase->SYNCR & ~LPT_SYNCEN_MSK)| (bEnable<<LPT_SYNCEN_POS);
}

static inline void csp_lpt_set_sync_mode(csp_lpt_t *ptLptBase,lpt_syncmd_e eMd)
{
	ptLptBase->SYNCR = (ptLptBase->SYNCR & ~LPT_OSTMD_MSK) | (eMd << LPT_OSTMD_POS);
}

static inline void csp_lpt_rearm_sync(csp_lpt_t *ptLptBase)
{
	ptLptBase->SYNCR |= LPT_REARM;
}

static inline void csp_lpt_auto_rearm_enable(csp_lpt_t *ptLptBase,bool bEnable)
{
	ptLptBase -> SYNCR = (ptLptBase -> SYNCR & ~LPT_AREARM_MSK)| (bEnable <<LPT_AREARM_POS);
}

static inline void csp_lpt_set_trgprd(csp_lpt_t *ptLptBase,uint8_t byPeriod)
{
	ptLptBase->EVPS = (ptLptBase->EVPS & ~LPT_TRGEVPRD_MSK)| LPT_TRGEVPRD(byPeriod);
}

static inline void csp_lpt_set_trgcnt(csp_lpt_t *ptLptBase,uint8_t byCnt)
{
	ptLptBase->EVPS = (ptLptBase->EVPS & ~LPT_TRGEVCNT_MSK)| LPT_TRGEVCNT(byCnt);
}

static inline void csp_lpt_trg_enable(csp_lpt_t *ptLptBase,bool bEnable)
{
	ptLptBase->EVTRG = (ptLptBase->EVTRG & ~LPT_TRG0OE_MSK) | (bEnable << LPT_TRG0OE_POS);
}



static inline void csp_lpt_set_start_im_enable(csp_lpt_t *ptLptBase, bool bEnable)
{
	ptLptBase->CEDR = (ptLptBase->CEDR & ~LPT_SHDWSTP_MSK) | (bEnable << LPT_SHDWSTP_POS);
}

static inline void csp_lpt_sw_start(csp_lpt_t *ptLptBase)
{
	ptLptBase->RSSR = (ptLptBase->RSSR & (~LPT_STR_MSK)) | (LPT_START<<LPT_STR_POS);
}
static inline void csp_lpt_stop(csp_lpt_t *ptLptBase)
{
	ptLptBase->RSSR = (ptLptBase->RSSR & (~LPT_STR_MSK)) | (LPT_STOP<<LPT_STR_POS);
}
static inline void csp_lpt_soft_rst(csp_lpt_t *ptLptBase)
{
	ptLptBase->RSSR = (ptLptBase->RSSR & (~LPT_SRR_MSK)) | LPT_SRR_EN;
}

static inline void csp_lpt_set_prdr(csp_lpt_t *ptLptBase, uint16_t hwPrdr)
{
	ptLptBase->PRDR = hwPrdr;
}
static inline void csp_lpt_set_cmp(csp_lpt_t *ptLptBase, uint16_t hwCmp)
{
	ptLptBase->CMP = hwCmp;
}
static inline void csp_lpt_set_cnt(csp_lpt_t *ptLptBase, uint16_t hwCnt)
{
	ptLptBase->CNT = hwCnt;
}
static inline uint16_t csp_lpt_get_prdr(csp_lpt_t *ptLptBase)
{
	return (uint16_t)(ptLptBase->PRDR & LPT_PRDR_MSK);
}
static inline uint16_t csp_lpt_get_cmp(csp_lpt_t *ptLptBase)
{
	return (uint16_t)(ptLptBase->CMP & LPT_CMP_MSK);
}
static inline uint16_t csp_lpt_get_cnt(csp_lpt_t *ptLptBase)
{
	return (uint16_t)ptLptBase->CNT;
}
static inline uint8_t csp_lpt_get_pscr(csp_lpt_t *ptLptBase)
{
	return (uint8_t)(ptLptBase->PSCR & LPT_PSCDIV_MSK);
}
static inline uint8_t csp_lpt_get_css(csp_lpt_t *ptLptBase)
{
	return (uint8_t)((ptLptBase->CEDR & LPT_CSS_MSK) >> LPT_CSS_POS);
}
//
static inline uint32_t csp_lpt_get_risr(csp_lpt_t *ptLptBase)
{
	return ((ptLptBase->RISR) & 0x7);
}
static inline uint32_t csp_lpt_get_misr(csp_lpt_t *ptLptBase)
{
	return ((ptLptBase->MISR) & 0x7);
}
static inline void csp_lpt_clr_misr(csp_lpt_t *ptLptBase, lpt_int_e eIntNum)
{
	ptLptBase->ICR = 0x1 << eIntNum;
}
static inline void csp_lpt_clr_all_int(csp_lpt_t *ptLptBase)
{
	ptLptBase->ICR = 0x07;
}
//
static inline void csp_lpt_vic_irq_en(void)
{
	NVIC_EnableIRQ(LPT_IRQn);
}

static inline void csp_lpt_vic_irq_dis(void)
{
	NVIC_DisableIRQ(LPT_IRQn);
}
//


//
static inline void csp_lpt_debug_enable(csp_lpt_t *ptLptBase, bool bEnable)
{
	ptLptBase->CEDR = (ptLptBase -> CEDR & ~(LPT_DBG_MSK)) | (bEnable << LPT_DBG_POS); 
}

static inline void csp_lpt_clk_enable(csp_lpt_t *ptLptBase, bool bEnable)
{
	ptLptBase->CEDR = (ptLptBase->CEDR & ~LPT_CLK_MSK) | (bEnable << LPT_CLK_POS);
}

static inline void csp_lpt_stopshadow_enable(csp_lpt_t *ptLptBase, bool bEnable)
{
	ptLptBase->CEDR = (ptLptBase->CEDR & ~LPT_SHDWSTP_MSK) | (bEnable << LPT_SHDWSTP_POS);
}
static inline void csp_lpt_set_clk(csp_lpt_t *ptLptBase, lpt_css_e eClk, lpt_pscdiv_e ePscDiv)
{
	ptLptBase->CEDR = (ptLptBase->CEDR & ~LPT_CSS_MSK) | (eClk<<LPT_CSS_POS);
	ptLptBase->PSCR = ePscDiv << LPT_PSCDIV_POS;
	
}

static inline void csp_lpt_set_opmd(csp_lpt_t *ptLptBase, lpt_opm_e eOpm)
{
	ptLptBase->CR = (ptLptBase->CR & ~LPT_OPM_MSK) | (eOpm<<LPT_OPM_POS);
}

static inline void csp_lpt_prdr_ldmode(csp_lpt_t *ptLptBase, lpt_prdld_e eMd)
{
	ptLptBase->CR &= ( ptLptBase->CR & ~LPT_PRDLD_MSK) | (eMd<<LPT_PRDLD_POS);
}

static inline void csp_lpt_cmp_ldmode(csp_lpt_t *ptLptBase, lpt_cmpld_e eMd)
{
	ptLptBase->CR &= ( ptLptBase->CR & ~LPT_CMPLD_MSK) | (eMd<<LPT_CMPLD_POS);
}

static inline void csp_lpt_prsc_ldmode(csp_lpt_t *ptLptBase, lpt_pscld_e eMd)
{
	ptLptBase->CR &= ( ptLptBase->CR & ~LPT_PSCLD_MSK) | (eMd<<LPT_PSCLD_POS);
}

static inline void csp_lpt_swsync_enable(csp_lpt_t *ptLptBase, bool bEnable)
{
	ptLptBase -> CR = (ptLptBase -> CR & ~(LPT_SWSYNC_MSK)) | (bEnable << LPT_SWSYNC_POS);
}

static inline void csp_lpt_set_filter(csp_lpt_t *ptLptBase, uint8_t byFre, lpt_fltdeb_e byNTimes)
{
	ptLptBase->CEDR = (ptLptBase->CEDR & ~(LPT_FLTCKPRS_MSK)) | LPT_FLTCKPRS(byFre);
	ptLptBase->CR = (ptLptBase->CR & ~(LPT_PSCDIV_MSK)) | (byNTimes << LPT_FLTDEB_POS);
}

static inline void csp_lpt_set_pol(csp_lpt_t *ptLptBase, lpt_pol_e ePol)
{
	ptLptBase->CR = (ptLptBase->CR & ~LPT_POL_MSK) | (ePol<<LPT_POL_POS);
}

static inline void csp_lpt_set_idle_st(csp_lpt_t *ptLptBase, lpt_idlest_e eSt)
{
	ptLptBase->CR = (ptLptBase->CR & ~LPT_IDLEST_MSK)| (eSt << LPT_IDLEST_POS);
}

static inline void csp_lpt_flt_init(csp_lpt_t *ptLptBase)
{
	ptLptBase -> CR |= LPT_FLT_INIT;
}

static inline void csp_lpt_out_enable(csp_lpt_t *ptLptBase, bool bEnable)
{
	ptLptBase->CR = (ptLptBase->CR & ~LPT_OUT_EN_MSK) | bEnable << LPT_OUT_EN_POS;
}

static inline void csp_lpt_sync_window_enable(csp_lpt_t *ptLptBase, bool bEnable)
{
	ptLptBase -> TRGFCR = (ptLptBase -> TRGFCR & ~LPT_SRCSEL_MSK)| (bEnable << LPT_SRCSEL_POS);
}

static inline void csp_lpt_sync_window_inv_enable(csp_lpt_t *ptLptBase,bool bEnable)
{
	ptLptBase -> TRGFCR = (ptLptBase -> TRGFCR & ~LPT_BLKINV_MSK)| (bEnable << LPT_BLKINV_POS);
}

static inline void csp_lpt_sync_window_cross_enable(csp_lpt_t *ptLptBase, bool bEnable)
{
	ptLptBase -> TRGFCR = (ptLptBase -> TRGFCR & ~LPT_CROSS_MSK)| (bEnable << LPT_CROSS_POS);
}

static inline void csp_lpt_set_sync_offset(csp_lpt_t *ptLptBase, uint16_t hwOffset)
{
	ptLptBase -> TRGFWR = (ptLptBase -> TRGFWR & ~LPT_OFFSET_MSK)| (hwOffset << LPT_OFFSET_POS );
}

static inline void csp_lpt_set_sync_window(csp_lpt_t *ptLptBase, uint16_t hwOffset)
{
	ptLptBase -> TRGFWR = (ptLptBase -> TRGFWR & ~LPT_WINDOW_MSK)| (hwOffset << LPT_WINDOW_POS );
}

#endif

