/***********************************************************************//** 
 * \file  csp_sio.h
 * \brief  SIO description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-10-09 <td>V0.0  <td>XB   <td>initial
 * <tr><td> 2021-1-09 <td>V0.1  <td>ZHY   <td>modify
 * </table>
 * *********************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CSP_SIO_H
#define _CSP_SIO_H

/* Includes ------------------------------------------------------------------*/
#include <soc.h>

/// \struct csp_sio_t
/// \brief SIO reg description
typedef struct
 {
    __IOM 	uint32_t  CR;		//0x0000	Control Register
	__IOM 	uint32_t  TXCR0;	//0x0004	Send Control Register0		
	__IOM 	uint32_t  TXCR1;	//0x0008 	Send Control Register1			
	__OM 	uint32_t  TXBUF;	//0x000C	Send Data Cache Register			
	__IOM 	uint32_t  RXCR0;	//0x0010	Receive Control Register0			
	__IOM 	uint32_t  RXCR1;	//0x0014	Receive Control Register1	
	__IOM	uint32_t  RXCR2;	//0x0018	Receive Control Register2		
	__IM 	uint32_t  RXBUF;	//0x001C	Receive Data Cache Register			
	__IM 	uint32_t  RISR;		//0x0020	Raw Interrupt Status Register			
	__IM 	uint32_t  MISR;		//0x0024	Masked Interrupt Status Register
	__IOM 	uint32_t  IMCR;		//0x0028	Interrupt Enable Contorl Register
	__OM 	uint32_t  ICR;		//0x002C	Interrupt Status Clear Register	
 } csp_sio_t;
 
/*****************************************************************************
************************** SIO Para Macro defined ****************************
******************************************************************************/


/******************************************************************************
* CR : Control Register
******************************************************************************/
#define	SIO_CLKEN_POS		(0)			//SIO CLKEN
#define SIO_CLKEN_MSK		(0x01ul << SIO_CLKEN_POS)
#define SIO_CLKEN			(0x01ul << SIO_CLKEN_POS)

#define SIO_DEBDEP_POS		(1)			//Receive Filter period
#define SIO_DEBDEP_MSK		(0x07ul << SIO_DEBDEP_POS)
typedef enum{
	SIO_DEBDEP1			= 0,
	SIO_DEBDEP2,
	SIO_DEBDEP3,
	SIO_DEBDEP4,
	SIO_DEBDEP5,
	SIO_DEBDEP6,
	SIO_DEBDEP7,
	SIO_DEBDEP8
}sio_debdep_e;

#define SIO_DEBCKS_POS		(4)			//Receive Filter CLK Div
#define SIO_DEBCKS_MSK		(0x0Ful << SIO_DEBCKS_POS)
#define SIO_DEBCKS(val)   	(((val) & 0x0F) << SIO_DEBCKS_POS)

#define SIO_MODE_POS		(8)			//SIO TX or RX mode
#define SIO_MODE_MSK    	(0x01ul << SIO_MODE_POS)
typedef enum{
	SIO_MODE_TX			= 0,
	SIO_MODE_RX
}sio_mode_e;

#define SIO_TCKPRS_POS    	(16)		//Send CLK Div
#define SIO_TCKPRS_MSK    	(0xFFFFul << SIO_TCKPRS_POS)
#define SIO_TCKPRS(val)   	(((val) & 0xFFFF) << SIO_TCKPRS_POS)

/******************************************************************************
* TXCR0 : Send Control Register0	
******************************************************************************/
#define SIO_IDLEST_POS    	(0)			//SIO Idle Ouput 
#define SIO_IDLEST_MSK    	(0x03ul << SIO_IDLEST_POS)
typedef enum{
	SIO_IDLEST_Z		= 0,
	SIO_IDLEST_H,
	SIO_IDLEST_L
}sio_idlest_e;

#define SIO_TDIR_POS    	(2)			//Send Data Dir
#define SIO_TDIR_MSK    	(0x01ul << SIO_TDIR_POS)
typedef enum{
	SIO_TDIR_LSB		= 0,
	SIO_TDIR_MSB
}sio_tdir_e;

#define SIO_TXBUFLEN_POS    (4)			//TX Buffer Length
#define SIO_TXBUFLEN_MSK    (0x0Ful << SIO_TXBUFLEN_POS)
#define SIO_TXBUFLEN(val)   (((val) & 0x0F) << SIO_TXBUFLEN_POS)

#define SIO_TXCNT_POS    	(8)
#define SIO_TXCNT_MSK    	(0xFFul << SIO_TXCNT_POS)
#define SIO_TXCNT(val)   	(((val) & 0xFF) << SIO_TXCNT_POS)

/******************************************************************************
* TXCR1 : Send Control Register0	
******************************************************************************/
#define SIO_D0DUR_POS    	(2)			//D0 Time Long
#define SIO_D0DUR_MSK    	(0x07ul << SIO_D0DUR_POS)
#define SIO_D0DUR(val)		(((val) & 0x07) << SIO_D0DUR_POS)

#define SIO_D1DUR_POS    	(5)			//D1 Time Long
#define SIO_D1DUR_MSK    	(0x07ul << SIO_D1DUR_POS)
#define SIO_D1DUR(val)		(((val) & 0x07) << SIO_D1DUR_POS)

#define SIO_LENOBH_POS    	(8)			//DH Sequence Long
#define SIO_LENOBH_MSK    	(0x07ul << SIO_LENOBH_POS)
#define SIO_LENOBL_POS    	(11)		//DH Sequence Long
#define SIO_LENOBL_MSK    	(0x07ul << SIO_LENOBL_POS)
typedef enum{
	SIO_DHL_BIT1		= 0,
	SIO_DHL_BIT2,
	SIO_DHL_BIT3,
	SIO_DHL_BIT4,
	SIO_DHL_BIT5,
	SIO_DHL_BIT6,
	SIO_DHL_BIT7,
	SIO_DHL_BIT8
}sio_lenob_e;

#define SIO_HSQ_POS    		(16)		//DH Sequence
#define SIO_HSQ_MSK    		(0xFFul << SIO_HSQ_POS)
#define SIO_HSQ(val)		(((val) & 0xFF) << SIO_HSQ_POS)

#define SIO_LSQ_POS    		(24)		//DL Sequence
#define SIO_LSQ_MSK    		(0xFFul << SIO_LSQ_POS)
#define SIO_LSQ(val)		(((val) & 0xFF) << SIO_LSQ_POS)

/******************************************************************************
* RXCR0 : Receive Control Register0	
******************************************************************************/
#define SIO_BSTSEL_POS    	(0)			//sample edge select
#define SIO_BSTSEL_MSK    	(0x03ul << SIO_BSTSEL_POS)
typedef enum{
	SIO_BST_RISE		= 0,
	SIO_BST_FALL,
	SIO_BST_BOTH,
	SIO_BST_R
}sio_bstsel_e;

#define SIO_TRGMODE_POS    	(3)			//sample trigger mode
#define SIO_TRGMODE_MSK    	(0x01ul << SIO_TRGMODE_POS)
typedef enum{
	SIO_TRG_MD0			= 0,
	SIO_TRG_MD1
}sio_trgmode_e;

#define SIO_SPLCNT_POS    	(4)			//BIT sample length
#define SIO_SPLCNT_MSK    	(0x1Ful << SIO_SPLCNT_POS)
#define SIO_SPLCNT(val)		(((val) & 0x1F) << SIO_SPLCNT_POS)

#define SIO_EXTRACT_POS    	(10)		//sample extract select
#define SIO_EXTRACT_MSK    	(0x3Ful << SIO_EXTRACT_POS)
typedef enum{
	SIO_EXTRACT_BIT0	= 0,
	SIO_EXTRACT_BIT1,
	SIO_EXTRACT_BIT2,
	SIO_EXTRACT_BIT3,
	SIO_EXTRACT_BIT4,
	SIO_EXTRACT_BIT5,
	SIO_EXTRACT_BIT6,
	SIO_EXTRACT_BIT7,
	SIO_EXTRACT_BIT8,
	SIO_EXTRACT_BIT9,
	SIO_EXTRACT_BIT10,
	SIO_EXTRACT_BIT11,
	SIO_EXTRACT_BIT12,
	SIO_EXTRACT_BIT13,
	SIO_EXTRACT_BIT14,
	SIO_EXTRACT_BIT15,
	SIO_EXTRACT_BIT16,
	SIO_EXTRACT_BIT17,
	SIO_EXTRACT_BIT18,
	SIO_EXTRACT_BIT19,
	SIO_EXTRACT_BIT20,
	SIO_EXTRACT_BIT21,
	SIO_EXTRACT_BIT22,
	SIO_EXTRACT_BIT23,
	SIO_EXTRACT_BIT24,
	SIO_EXTRACT_BIT25,
	SIO_EXTRACT_BIT26,
	SIO_EXTRACT_BIT27,
	SIO_EXTRACT_BIT28,
	SIO_EXTRACT_BIT29,
	SIO_EXTRACT_BIT30,
	SIO_EXTRACT_BIT31,
	SIO_EXTRACT_HITHR
}sio_extract_e;

#define SIO_HITHR_POS    	(16)		//sample extract high 
#define SIO_HITHR_MSK    	(0x1FuL << SIO_HITHR_POS)
#define SIO_HITHR(val)		(((val) & 0x1F) << SIO_HITHR_POS)

#define SIO_ALIGNEN_POS    	(28)		//sample alignment 
#define SIO_ALIGNEN_MSK    	(0x01ul << SIO_ALIGNEN_POS)
typedef enum{
	SIO_ALIGN_DIS		= 0,
	SIO_ALIGN_EN
}sio_align_e;

#define SIO_RDIR_POS    	(29)		//receive data dir MSB->LSB or LSB->MSB
#define SIO_RDIR_MSK    	(0x01ul << SIO_RDIR_POS)
typedef enum{
	SIO_RDIR_MSB		= 0,
	SIO_RDIR_LSB
}sio_rdir_e;

#define SIO_RMODE_POS    	(30)		//sample mode
#define SIO_RMODE_MSK    	(0x03ul << SIO_RMODE_POS)
typedef enum{
	SIO_RMODE0			= 0,
	SIO_RMODE1
}sio_rmode_e;

/******************************************************************************
* RXCR1 : Receive Control Register1
******************************************************************************/
#define SIO_RXCNT_POS    	(0)			//receive data length
#define SIO_RXCNT_MSK    	(0xFFul << SIO_RXCNT_POS)
#define SIO_RXCNT(val)		(((val) & 0xFF) << SIO_RXCNT_POS)

#define SIO_BUFCNT_POS    	(8)			//receive data buffer length
#define SIO_BUFCNT_MSK    	(0x1Ful << SIO_BUFCNT_POS)
#define SIO_BUFCNT(val)		(((val) & 0xFF) << SIO_BUFCNT_POS)

#define SIO_RCKPRS_POS    	(16)		//receive clk div
#define SIO_RCKPRS_MSK    	(0xFFFFul << SIO_RCKPRS_POS)
#define SIO_RCKPRS(val)		(((val) & 0xFFFF) << SIO_RCKPRS_POS)

/******************************************************************************
* RXCR2 : Receive Control Register2
******************************************************************************/
#define SIO_BREAKEN_POS    	(0)			//reset detection
#define SIO_BREAKEN_MSK    	(0x01ul << SIO_BREAKEN_POS)
typedef enum{
	SIO_BREAK_DIS		= 0,
	SIO_BREAK_EN
}sio_break_e;

#define SIO_BREAKLVL_POS    (1)			//reset detection level
#define SIO_BREAKLVL_MSK    (0x01ul << SIO_BREAKLVL_POS)
typedef enum{
	SIO_BREAKLVL_LOW		= 0,
	SIO_BREAKLVL_HIGH
}sio_breaklel_e;

#define SIO_BREAKCNT_POS    (3)			//reset detection period
#define SIO_BREAKCNT_MSK    (0x1Ful<< SIO_BREAKCNT_POS)
#define SIO_BREAKCNT(val)	(((val) & 0x1F) << SIO_BREAKCNT_POS)

#define SIO_TORSTEN_POS    	(15)		//sample overtime reset
#define SIO_TORSTEN_MSK    	(0x01ul << SIO_TORSTEN_POS)
typedef enum{
	SIO_TORESET_DIS		= 0,
	SIO_TORESET_EN
}sio_torst_e;

#define SIO_TOCNT_POS    	(16)		//sample overtime length
#define SIO_TOCNT_MSK    	(0xFFul << SIO_TOCNT_POS)
#define SIO_TOCNT(val)		(((val) & 0xFF) << SIO_TOCNT_POS)
		
/******************************************************************************
* RISR, MISR,IMCR, ICR : CMP Status Registers and Interrupt Registers
******************************************************************************/ 	
#define SIO_TXDNE_POS    	(0)
#define SIO_TXDNE_MSK    	(0x01uL << SIO_TXDNE_POS)
#define SIO_RXDNE_POS    	(1)
#define SIO_RXDNE_MSK    	(0x01uL << SIO_RXDNE_POS)
#define SIO_TXBUFEMPT_POS	(2)
#define SIO_TXBUFEMPT_MSK	(0x01uL << SIO_TXBUFEMPT_POS)
#define SIO_RXBUFFULL_POS	(3)
#define SIO_RXBUFFULL_MSK	(0x01uL << SIO_RXBUFFULL_POS)
#define SIO_BREAK_POS    	(4)
#define SIO_BREAK_MSK		(0x01uL << SIO_BREAK_POS)
#define SIO_TIMEOUT_POS    	(5)
#define SIO_TIMEOUT_MSK    	(0x01uL << SIO_TIMEOUT_POS)

typedef enum
{
	SIO_TXDNE     	=	(0x01ul << SIO_TXDNE_POS),  
	SIO_RXDNE    	=	(0x01ul << SIO_RXDNE_POS),      
	SIO_TXBUFEMPT   =	(0x01ul << SIO_TXBUFEMPT_POS), 
	SIO_RXBUFFULL	=	(0x01ul << SIO_RXBUFFULL_POS), 
	SIO_BREAK		=	(0x01ul << SIO_BREAK_POS), 
	SIO_TIMEOUT		=	(0x01ul << SIO_TIMEOUT_POS)
}sio_int_e;  

typedef enum
{
	D0     	=	0x00,  
	D1    	=	0x01,      
	DL   	=	0x02, 
	DH		=	0x03
}sio_txbufdata_e;  

#define	TXBUF_0(val) 	((val & 0x3) << (0ul))
#define	TXBUF_1(val) 	((val & 0x3) << (2ul))
#define	TXBUF_2(val) 	((val & 0x3) << (4ul))
#define	TXBUF_3(val) 	((val & 0x3) << (6ul))
#define	TXBUF_4(val) 	((val & 0x3) << (8ul))
#define	TXBUF_5(val) 	((val & 0x3) << (10ul))
#define	TXBUF_6(val) 	((val & 0x3) << (12ul))
#define	TXBUF_7(val) 	((val & 0x3) << (14ul))
#define	TXBUF_8(val) 	((val & 0x3) << (16ul))
#define	TXBUF_9(val) 	((val & 0x3) << (18ul))
#define	TXBUF_10(val) 	((val & 0x3) << (20ul))
#define	TXBUF_11(val) 	((val & 0x3) << (22ul))
#define	TXBUF_12(val) 	((val & 0x3) << (24ul))
#define	TXBUF_13(val) 	((val & 0x3) << (26ul))
#define	TXBUF_14(val) 	((val & 0x3) << (28ul))
#define	TXBUF_15(val) 	((val & 0x3) << (30ul))


/*****************************************************************************
 **************************** SIO TX PARA Struct *****************************
 *****************************************************************************/
typedef struct
{
	sio_tdir_e		eTdir;			//LSB or MSB
	sio_idlest_e	eIdlest;		//IDLE level 	
	uint8_t			byTxLen;		//TX Buffer length
	uint8_t			byTxCnt;		//TX Count 
	uint8_t			byD0Ticks;		//D0 timer long, tx ticks num 
	uint8_t			byD1Ticks;		//D1 timer long, tx ticks num 
	uint8_t			byDHTicks;		//DH timer long, tx ticks num  
	uint8_t			byDLTicks;		//DL timer long, tx ticks num   
	uint8_t			byHsq;			//DH Sequence
	uint8_t			byLsq;			//DL Sequence
	uint32_t		wTxclk;			//TX CLK,Ftxclk 
}csp_sio_tx_t;

/*****************************************************************************
 ******************** SIO extern Functions Declaration ***********************
 *****************************************************************************/
//extern void csp_sio_def_Init(csp_sio_t *ptSioBase);
//extern void csp_sio_set_int(csp_sio_t *ptSioBase,sio_int_e eSioInt,uint8_t byEnable);
//extern void csp_sio_set_dh(csp_sio_t *ptSioBase, sio_lenob_e eDhBit, uint8_t byDhSq);
//extern void csp_sio_set_dl(csp_sio_t *ptSioBase, sio_lenob_e eDlBit, uint8_t byDlSq);
//extern void csp_sio_tx_init(csp_sio_t *ptSioBase, csp_sio_tx_t *tpSioTx);
//extern uint32_t csp_sio_send(csp_sio_t *ptSioBase, uint32_t *byPdata, uint8_t byLen);

//SIO RX
//extern void csp_sio_set_sample_mode(csp_sio_t *ptSioBase,sio_bstsel_e eBst, sio_trgmode_e eTrgMd, sio_rmode_e eRmd);
//extern void csp_sio_set_sample(csp_sio_t *ptSioBase, sio_extract_e eExtract ,sio_align_e eAlign, uint8_t bySplCnt, uint8_t byHihr);
//extern void csp_sio_set_rx(csp_sio_t *ptSioBase,sio_rdir_e eRdir, uint8_t byRxCnt, uint8_t byBuflen);
//extern void csp_sio_set_break(csp_sio_t *ptSioBase, sio_break_e eBreak, sio_breaklel_e eBkLvl,uint8_t byBkCnt);

/*****************************************************************************
 ******************** SIO inline Functions Declaration ***********************
 *****************************************************************************/
static inline void csp_sio_clk_en(csp_sio_t *ptUsartBase)
{
	ptUsartBase->CR |= SIO_CLKEN ; 		//CLK EN
}
static inline void csp_sio_clk_dis(csp_sio_t *ptUsartBase)
{
	ptUsartBase->CR &= ~SIO_CLKEN_MSK ; //CLK DIS
}
static inline void csp_sio_set_mode(csp_sio_t *ptSioBase, sio_mode_e eTxRx)
{
	ptSioBase->CR = (ptSioBase->CR & ~SIO_MODE_MSK) | (eTxRx << SIO_MODE_POS);
}

//SIO TX 
static inline void csp_sio_set_tx_clkdiv(csp_sio_t *ptSioBase,uint16_t hwClkDiv)
{
	ptSioBase->CR = (ptSioBase->CR & ~SIO_TCKPRS_MSK) | SIO_TCKPRS(hwClkDiv);
}

static inline void csp_sio_set_txcr0(csp_sio_t *ptSioBase,sio_idlest_e eIdlest, sio_tdir_e eTdir, uint8_t byTxBufLen, uint8_t byTxCnt)
{
	ptSioBase->TXCR0 = (eIdlest) | (eTdir << SIO_TDIR_POS) | SIO_TXBUFLEN(byTxBufLen) | SIO_TXCNT(byTxCnt);
}

static inline void csp_sio_set_d0(csp_sio_t *ptSioBase, uint8_t byD0Time)
{
	ptSioBase->TXCR1 = (ptSioBase->TXCR1 & ~SIO_D0DUR_MSK) | SIO_D0DUR(byD0Time);
}

static inline void csp_sio_set_d1(csp_sio_t *ptSioBase, uint8_t byD1Time)
{
	ptSioBase->TXCR1 = (ptSioBase->TXCR1  &  ~SIO_D1DUR_MSK) | SIO_D1DUR(byD1Time) ;
}

static inline void csp_sio_set_dl(csp_sio_t *ptSioBase, sio_lenob_e eDlBit, uint8_t byDlSq)
{
	ptSioBase->TXCR1 = (ptSioBase->TXCR1 & ~(SIO_LENOBL_MSK | SIO_LSQ_MSK));
	ptSioBase->TXCR1 |= (eDlBit << SIO_LENOBL_POS) | SIO_LSQ(byDlSq);
}

static inline  void csp_sio_set_dh(csp_sio_t *ptSioBase, sio_lenob_e eDhBit, uint8_t byDhSq)
{
	ptSioBase->TXCR1 = (ptSioBase->TXCR1 & ~(SIO_LENOBH_MSK | SIO_HSQ_MSK));
	ptSioBase->TXCR1 |= (eDhBit << SIO_LENOBH_POS) | SIO_HSQ(byDhSq);
}

static inline void csp_sio_wait_tx(csp_sio_t *ptSioBase)
{
	while(!(ptSioBase->RISR & SIO_TXDNE));
}

static inline void csp_sio_set_txbuf(csp_sio_t *ptSioBase,uint32_t wVal)
{
	ptSioBase->TXBUF = wVal;
}

//SIO RX
static inline void csp_sio_set_rx_clkdiv(csp_sio_t *ptSioBase,uint16_t hwClkDiv)
{	
	ptSioBase->RXCR1 = (ptSioBase->RXCR1 & ~SIO_RCKPRS_MSK) | SIO_RCKPRS(hwClkDiv);
}

static inline void csp_sio_set_rx_deb(csp_sio_t *ptSioBase,sio_debdep_e eDebDep, uint8_t byDebCks)
{
	ptSioBase->CR = (ptSioBase->CR & ~(SIO_DEBDEP_MSK | SIO_DEBCKS_MSK)) | (eDebDep << SIO_DEBDEP_POS) | SIO_DEBCKS(byDebCks);
}

static inline void csp_sio_set_break_para(csp_sio_t *ptSioBase,sio_breaklel_e eBkLvl,uint8_t byBkCnt)
{
	ptSioBase->RXCR2 = (ptSioBase->RXCR2 & ~(SIO_BREAKLVL_MSK | SIO_BREAKCNT_MSK)) | (eBkLvl << SIO_BREAKLVL_POS) | SIO_BREAKCNT(byBkCnt);
}

static inline void csp_sio_set_align(csp_sio_t *ptSioBase, sio_align_e eAlign)
{
	ptSioBase->RXCR0 = (ptSioBase->RXCR0 & ~SIO_ALIGNEN_MSK) | (eAlign << SIO_ALIGNEN_POS);
}

static inline void csp_sio_set_torst(csp_sio_t *ptSioBase, sio_torst_e eTorst, uint8_t byToCnt)
{
	ptSioBase->RXCR2 = (ptSioBase->RXCR2 & ~(SIO_TORSTEN_MSK | SIO_TOCNT_MSK)) | (eTorst << SIO_TORSTEN_POS) | SIO_TOCNT(byToCnt);
}

static inline void csp_sio_ctrl_break(csp_sio_t *ptSioBase, sio_break_e eBreak)
{
	ptSioBase->RXCR2 = (ptSioBase->RXCR2 & ~SIO_BREAKEN_MSK) | (eBreak << SIO_BREAKEN_POS);
}

static inline uint32_t csp_sio_get_rxbuf(csp_sio_t *ptSioBase)
{
	return (uint32_t)(ptSioBase->RXBUF);
}

static inline void csp_sio_set_sample_mode(csp_sio_t *ptSioBase,sio_bstsel_e eBst, sio_trgmode_e eTrgMode, sio_rmode_e eRMode)
{
	ptSioBase->RXCR0 = (ptSioBase->RXCR0 & ~(SIO_BSTSEL_MSK | SIO_TRGMODE_MSK | SIO_RMODE_MSK));
	ptSioBase->RXCR0 |= (eBst << SIO_BSTSEL_POS) | (eTrgMode << SIO_TRGMODE_POS) | (eRMode << SIO_RMODE_POS);
}

static inline void csp_sio_set_sample(csp_sio_t *ptSioBase, sio_extract_e eExtract ,sio_align_e eAlign, uint8_t bySplCnt, uint8_t byHihr)
{
	ptSioBase->RXCR0 = (ptSioBase->RXCR0 & ~(SIO_SPLCNT_MSK | SIO_EXTRACT_MSK | SIO_HITHR_MSK | SIO_ALIGNEN_MSK));
	ptSioBase->RXCR0 |= (eExtract << SIO_EXTRACT_POS) | (eAlign << SIO_ALIGNEN_POS)| SIO_SPLCNT(bySplCnt) | SIO_HITHR(byHihr);
}

static inline void csp_sio_set_recv(csp_sio_t *ptSioBase,sio_rdir_e eRdir, uint8_t byBuflen, uint8_t byRxCnt)
{
	ptSioBase->RXCR0 = (ptSioBase->RXCR0 & ~SIO_RDIR_MSK) | (eRdir << SIO_RDIR_POS);
	ptSioBase->RXCR1 = (ptSioBase->RXCR1 & ~(SIO_BUFCNT_MSK | SIO_RXCNT_MSK)) | SIO_BUFCNT(byBuflen) | SIO_RXCNT(byRxCnt) ;
}

static inline void csp_sio_set_break(csp_sio_t *ptSioBase, sio_break_e eBreak, sio_breaklel_e eBkLvl,uint8_t byBkCnt)
{
	ptSioBase->RXCR2 = (ptSioBase->RXCR2 & ~(SIO_BREAKEN_MSK | SIO_BREAKLVL_MSK | SIO_BREAKCNT_MSK));
	ptSioBase->RXCR2 |= eBreak | (eBkLvl << SIO_BREAKLVL_POS) | SIO_BREAKCNT(byBkCnt);
}
//SIO INT
static inline uint32_t csp_sio_get_isr(csp_sio_t *ptSioBase)
{
	return (uint32_t)(ptSioBase->MISR);
}

static inline uint32_t csp_sio_get_risr(csp_sio_t *ptSioBase)
{
	return (uint32_t)(ptSioBase->RISR);
}

static inline void csp_sio_clr_isr(csp_sio_t *ptSioBase, sio_int_e eSioInt)
{
	ptSioBase->ICR = eSioInt;
}
//static inline void csp_sio_vic_irq_en(void)
//{
//	NVIC_EnableIRQ(SIO_IRQn); 
//}
//static inline void csp_sio_vic_irq_dis(void)
//{
//	NVIC_DisableIRQ(SIO_IRQn); 
//}
static inline void csp_sio_int_enable(csp_sio_t *ptSioBase,sio_int_e eSioInt, bool bEnable)
{
	if(bEnable)
		ptSioBase->IMCR |= eSioInt;
	else
		ptSioBase->IMCR &= ~eSioInt;

}

#endif
