/***********************************************************************//** 
 * \file  csp_rtc.h
 * \brief RTC description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-25 <td>V0.0  <td>WNN   <td>initial
 *  * <tr><td> 2021-1-8 <td>V0.0  <td>WNN   <td>modify
 * </table>
 * *********************************************************************
*/

#ifndef CSP_RTC_H
#define CSP_RTC_H

#include "csi_core.h"
#include "stdint.h"
#include "stdbool.h"
#include "csp_common.h"


/******************************************************************************
************************* EPWM Structure Definition ***********************
******************************************************************************/
/**
*******************************************************************como.************
@brief EPWM bits Structure
*******************************************************************************
*/

 typedef struct
 {
   __IOM uint32_t TIMR;  		//0x0000	Time Control Register
   __IOM uint32_t DATR;		//0x0004	Date Control Register
   __IOM uint32_t CR;  		//0x0008	Control Register
   __IOM uint32_t	CCR;  		//0x000C	Clock Control register
   __IOM uint32_t	ALRA;		//0x0010	Alarm A
   __IOM uint32_t	ALRB;		//0x0014	Alarm B
   __IOM uint32_t	SSR;    	//0x0018	Sub second
   __IOM uint32_t	CAL;   		//0x001C	Calibration
   __IM uint32_t   	RISR;       //0x0020
   __IOM uint32_t	IMCR;   	//0x0024
   __IM	uint32_t	MISR;     	//0x0028
   __OM	uint32_t	ICR;   		//0x002C
   __IOM uint32_t	KEY;    	//0x0030
   __IOM uint32_t	EVTRG;  	//0x0034
   __IOM uint32_t	EVPS;   	//0x0038
   __OM	uint32_t    EVSWF;  		//0x003C
} csp_rtc_t;

/****** RTC time *****/
typedef struct {
    int sec;             ///< Second.      [0-59]
    int min;             ///< Minute.      [0-59]
    int hour;            ///< Hour.        [0-23]
    int mday;            ///< Day.         [1-31]
    int mon;             ///< Month.       [0-11]
    int year;            ///< Year-1900.   [70- ]      !NOTE:100=2000       
    int wday;            ///< Day of week. [0-6 ]      !NOTE:Sunday = 0     
    int yday;            ///< Days in year.[0-365]     !NOTE:January 1st = 0
} csp_rtc_time_t;

typedef struct {
    int sec;             ///< Second.      [0-59]
    int min;             ///< Minute.      [0-59]
    int hour;            ///< Hour.        [0-23]
    int wdaymsk;         ///< Week Day.         [1-31]

} csp_rtc_almtime_t;
/******************************************************************************
************************** RTC Registers Definition ****************************
******************************************************************************/

/******************************************************************************
* RTC Regiser
******************************************************************************/


///TIMR
#define RTC_SECU_POS	(0ul)
#define RTC_SECU_MSK	(0xf << RTC_SECU_POS)
#define RTC_SEC_MSK		(0x7f << RTC_SECU_POS)
#define RTC_SECT_POS	(4ul)
#define RTC_SECT_MSK	(0x7 << RTC_SECT_POS)
#define RTC_MINU_POS	(8ul)
#define RTC_MINU_MSK	(0xf << RTC_MINU_POS)
#define RTC_MIN_MSK		(0x7f << RTC_MINU_POS)
#define RTC_MINT_POS	(12ul)
#define RTC_MINT_MSK	(0x7 << RTC_MINT_POS)
#define RTC_HORU_POS	(16ul)
#define RTC_HORU_MSK	(0xf << RTC_HORU_POS)
#define RTC_HOR_MSK		(0x3f << RTC_HORU_POS)
#define RTC_HORT_POS	(20ul)
#define RTC_HORT_MSK	(0x3 << RTC_HORT_POS)

#define RTC_SECU(n)    ((n)<<RTC_SECU_POS)
#define RTC_SECT(n)    ((n)<<RTC_SECT_POS)
#define RTC_MINU(n)    ((n)<<RTC_MINU_POS)
#define RTC_MINT(n)    ((n)<<RTC_MINT_POS)
#define RTC_HORU(n)    ((n)<<RTC_HORU_POS)
#define RTC_HORT(n)    ((n)<<RTC_HORT_POS)
#define RTC_PM_POS		(22)
#define RTC_PM_MSK		(0x1 << RTC_PM_POS)
#define RTC_AM	0x0
#define RTC_PM 	0x1

///DATR
#define RTC_DAYU_POS	(0ul)
#define RTC_DAYU_MSK	(0xf << RTC_DAYU_POS)
#define RTC_DAY_MSK		(0x3f << RTC_DAYU_POS)
#define RTC_DAYT_POS	(4ul)
#define RTC_DAYT_MSK	(0x3 << RTC_DAYT_POS)
#define RTC_MONU_POS	(8ul)
#define RTC_MONU_MSK	(0xf << RTC_MONU_POS)
#define RTC_MON_MSK		(0x1f << RTC_MONU_POS)
#define RTC_MONT_POS	(12ul)
#define RTC_MONT_MSK	(0x1 << RTC_MONT_POS)
#define RTC_YEAU_POS	(16ul)
#define RTC_YEAU_MSK	(0xf << RTC_YEAU_POS)
#define RTC_YEA_MSK		(0xff << RTC_YEAU_POS)
#define RTC_YEAT_POS	(20ul)
#define RTC_YEAT_MSK	(0xf << RTC_YEAT_POS)
#define RTC_WKD_POS		(24ul)
#define RTC_WKD_MSK		(0x7 << RTC_WKD_POS)

#define RTC_DAYU(n)    ((n)<<RTC_DAYU_POS)
#define RTC_DAYT(n)    ((n)<<RTC_DAYT_POS)
#define RTC_MONU(n)    ((n)<<RTC_MONU_POS)
#define RTC_MONT(n)    ((n)<<RTC_MONT_POS)
#define RTC_YARU(n)    ((n)<<RTC_YEAU_POS)
#define RTC_YART(n)    ((n)<<RTC_YEAT_POS)
#define RTC_WKD(n)     ((n)<<RTC_WKD_POS)

///CR
#define RTC_INIT            (1ul)
#define RTC_BSY				(0x2ul)
#define RTC_ALMAE           (1ul << 3)
#define RTC_ALMBE           (1ul << 4)

#define RTC_FMT_POS			(5)
#define RTC_FMT_MSK			(0x1ul << RTC_FMT_POS)
typedef enum {
	RTC_24FMT = 0,
	RTC_12FMT
}rtc_fmt_e;

#define RTC_OSEL_POS		(10ul)
#define RTC_OSEL_MSK		(0x7 << RTC_OSEL_POS)


typedef enum {
	RTC_OUT_ALMA_PULSE = 0,
	RTC_OUT_ALMA_HIGH,
	RTC_OUT_ALMA_LOW,
	RTC_OUT_ALMB_PULSE = 0,
	RTC_OUT_ALMB_HIGH,
	RTC_OUT_ALMB_LOW,	
}rtc_osel_e;

#define RTC_CPRD_POS		(13ul)
#define RTC_CPRD_MSK		(0x7 << RTC_CPRD_POS)
typedef enum {
	RTC_CPRD_DIS = 0,
	RTC_CPRD_05SEC,
	RTC_CPRD_1SEC,
	RTC_CPRD_1MIN,
	RTC_CPRD_1HOR,
	RTC_CPRD_1DAY,
	RTC_CPRD_1MON
}rtc_cprd_e;

#define RTC_RBEN			(0x1 << 16)
#define RTC_TKEYTRG_POS		(17ul)
#define RTC_TKEYTRT_MSK		(0x3 << RTC_TKEYTRG_POS)


///CCR
#define RTC_DIVS_POS	(0ul)
#define RTC_DIVS_MSK	(0x7fff << RTC_DIVS_POS)
#define RTC_DIVA_POS	(16ul)
#define RTC_DIVA_MSK	(0x7f << RTC_DIVA_POS)
#define RTC_DIVS(n)    ((n))
#define RTC_DIVA(n)    ((n)<<RTC_DIVA_POS)
#define RTC_DBGEN      (1ul<<23)
#define RTC_CLKSRC_POS	(24ul)
#define RTC_CLKSRC_MSK	(0x3 << RTC_CLKSRC_POS)
typedef enum{
	RTC_ISOSC = 0,
	RTC_IMOSC_DIV4,
	RTC_EMOSC,
	RTC_EMOSC_DIV4
}rtc_clksrc_e;

#define RTC_CLK_STABLE	(0x1 << 26)
#define RTC_CLKEN	   (1ul << 27)

///ALRA/ALRB
#define RTC_ALMA		(0)
#define RTC_ALMB		(1)
#define RTC_ALM_DAY_POS	(24ul)
#define RTC_ALM_DAY_MSK	(0x3f << RTC_ALM_DAY_POS)
#define RTC_ALM_DAYT_POS	(28)
#define RTC_ALM_DAYT_MSK	(0x3 << RTC_ALM_DAYT_POS)
#define RTC_ALM_DAYU_POS	(24)
#define RTC_ALM_DAYU_MSK	(0xf << RTC_ALM_DAYU_POS)
#define RTC_ALM_DMSK_POS	(31)
#define RTC_ALM_DMSK_MSK	(0x1 << RTC_ALM_DMSK_POS)
#define RTC_ALM_WDSEL_POS	(30ul)
#define RTC_ALM_WDSEL_MSK	(0x1 << RTC_ALM_WDSEL_POS)
#define RTC_ALM_DSEL	(0x0)
#define RTC_ALM_WSEL	(0x1)
#define RTC_ALM_DAYU(n) ((n) << 24)
#define RTC_ALM_DAYT(n) ((n) << 28)


#define RTC_ALM_HOR_POS (16ul)
#define RTC_ALM_HOR_MSK	(0x3f << RTC_ALM_HOR_POS)
#define RTC_ALM_HORT_POS	(20)
#define RTC_ALM_HORT_MSK	(0x3 << RTC_ALM_HORT_POS)
#define RTC_ALM_HORU_POS	(16)
#define RTC_ALM_HORU_MSK	(0xf << RTC_ALM_HORU_POS)
#define RTC_ALM_HMSK_POS	(23)
#define RTC_ALM_PM		(0x1 << 22)
#define RTC_ALM_HORU(n) ((n) << 20)
#define RTC_ALM_HORT(n) ((n) << 16)

#define RTC_ALM_MIN_POS	(8)
#define RTC_ALM_MIN_MSK	(0x7f << RTC_ALM_MIN_POS)
#define RTC_ALM_MINT_POS	(12)
#define RTC_ALM_MINT_MSK	(0x7 << RTC_ALM_MINT_POS)
#define RTC_ALM_MINU_POS	(8)
#define RTC_ALM_MINU_MSK	(0xf << RTC_ALM_MINU_POS)
#define RTC_ALM_MMSK_POS    (15)
#define RTC_ALM_MINU(n) ((n) << 12)
#define RTC_ALM_MINT(n) ((n) << 8)

#define RTC_ALM_SEC_POS	(0)
#define RTC_ALM_SEC_MSK	(0x7f << RTC_ALM_SEC_POS)
#define RTC_ALM_SECT_POS	(4)
#define RTC_ALM_SECT_MSK	(0x7 << RTC_ALM_SECT_POS)
#define RTC_ALM_SECU_POS	(0)
#define RTC_ALM_SECU_MSK	(0xf << RTC_ALM_SECU_POS)
#define RTC_ALM_SMSK_POS	(7)
#define RTC_ALM_SECU(n) ((n) << 4)
#define RTC_ALM_SECT(n) ((n) << 0)

#define RTC_ALM_MODE 	(0xC0808080)

///SSR
#define RTC_SSR_MSK		(0xffff)

/// Interrupt Related
#define ALRA_INT_POS 	(0)
#define ALRB_INT_POS	(1)
#define CPRD_INT_POS	(2)
#define TRGEV0_INT_POS	(3)
#define TRGEV1_INT_POS	(4)
typedef enum{
	RTC_INT_ALMA = 0x1 << ALRA_INT_POS,
	RTC_INT_ALMB = 0x1 << ALRB_INT_POS,
	RTC_INT_CPRD = 0x1 <<CPRD_INT_POS,
	RTC_INT_TRGEV0 = 0x1 <<TRGEV0_INT_POS,
	RTC_INT_TRGEV1 = 0x1 <<TRGEV1_INT_POS
}rtc_int_e;


///KEY
#define RTC_KEY		(0xca53)

///EVTRG
#define RTC_TRGSEL0_POS	(0)
#define RTC_TRGSEL0_MSK	(0xf)
#define RTC_TRGSEL1_POS	(4)
#define RTC_TRGSEL1_MSK	(0xf << RTC_TRGSEL1_POS)
typedef enum {
	RTC_TRG_DIS = 0,
	RTC_TRG_A,
	RTC_TRG_B,
	RTC_TRG_CPRD
}csp_rtc_trgsel_e;
#define RTC_TRG0OE		(0x1 << 20)
#define RTC_TRG1OE		(0x1 << 21)

///EVPS
#define RTC_TRGEV0PRD_POS	(0)
#define RTC_TRGEV0PRD_MSK	(0xf)
#define RTC_TRGEV1PRD_POS	(1)
#define RTC_TRGEV1PRD_MSK	(0xf << RTC_TRGEV1PRD_POS)
#define RTC_TRGEV0CNT_POS	(1)
#define RTC_TRGEV0CNT_MSK	(0xf << RTC_TRGEV0CNT_POS)

//EVSWF
#define RTC_EV0SWF 	(0x1)
#define RTC_EV1SWF  (0x2)


/// static inline functions
static inline void csp_rtc_clk_enable(csp_rtc_t *ptRtcBase, bool bEnable)
{
	ptRtcBase->KEY = 0xCA53;
	if (bEnable)
		ptRtcBase->CCR |= RTC_CLKEN;
	else
		ptRtcBase->CCR &= (~RTC_CLKEN);
	ptRtcBase->KEY = 0x0;
	while(ptRtcBase->CR & RTC_BSY);
}

static inline void csp_rtc_wr_key(csp_rtc_t *ptRtcBase)
{
	ptRtcBase->KEY = 0xCA53;
}

static inline void csp_rtc_ers_key(csp_rtc_t *ptRtcBase)
{
	ptRtcBase->KEY = 0x0;
}


static inline void csp_rtc_set_fmt(csp_rtc_t *ptRtcBase, rtc_fmt_e bFmt)
{
	ptRtcBase->KEY = 0xCA53;
	ptRtcBase->CR = (ptRtcBase->CR & (~RTC_FMT_MSK)) | (bFmt << RTC_FMT_POS);
	ptRtcBase->KEY = 0x0;
	while(ptRtcBase->CR & RTC_BSY);
}
static inline rtc_fmt_e csp_rtc_get_fmt(csp_rtc_t *ptRtcBase)
{
	return (((ptRtcBase->CR) & RTC_FMT_MSK) >> RTC_FMT_POS);
}

static inline void csp_rtc_debug_enable(csp_rtc_t *ptRtcBase, bool bEnable)
{
	ptRtcBase->KEY = 0xCA53;
	ptRtcBase->CCR = (ptRtcBase->CCR &(~RTC_DBGEN)) | (bEnable << 23);
	ptRtcBase->KEY = 0x0;
}

static inline void csp_rtc_rb_enable(csp_rtc_t *ptRtcBase, bool bEnable)
{
	//while(ptRtcBase->CR & RTC_BSY);
	ptRtcBase->KEY = 0xCA53;
	ptRtcBase->CR = (ptRtcBase->CR &(~RTC_RBEN)) | (bEnable << 16);
	ptRtcBase->KEY = 0x0;
	while(ptRtcBase->CR & RTC_BSY);
}

static inline void csp_rtc_set_cprd(csp_rtc_t *ptRtcBase, rtc_cprd_e eCprd)
{
	//while(ptRtcBase->CR & RTC_BSY);
	ptRtcBase->KEY = 0xCA53;
	ptRtcBase->CR  = (ptRtcBase->CR & (~RTC_CPRD_MSK)) | (eCprd << RTC_CPRD_POS) ;
	ptRtcBase->KEY = 0x0;
	while(ptRtcBase->CR & RTC_BSY);
}

static inline void csp_rtc_set_osel(csp_rtc_t *ptRtcBase, rtc_osel_e eOsel)
{
	//while(ptRtcBase->CR & RTC_BSY);
	ptRtcBase->KEY = 0xCA53;
	ptRtcBase->CR  = (ptRtcBase->CR & (~RTC_OSEL_MSK)) | (eOsel << RTC_OSEL_POS) ;
	ptRtcBase->KEY = 0x0;
	while(ptRtcBase->CR & RTC_BSY);
}

static inline void csp_rtc_stop(csp_rtc_t *ptRtcBase)
{
	//while(ptRtcBase->CR & RTC_BSY);
	ptRtcBase->KEY = 0xCA53;
	ptRtcBase->CR |= RTC_INIT;
	ptRtcBase->KEY = 0x0;
	while(ptRtcBase->CR & RTC_BSY);
}

static inline void csp_rtc_run(csp_rtc_t *ptRtcBase)
{
	//while(ptRtcBase->CR & RTC_BSY);
	
	ptRtcBase->KEY = 0xCA53;
	ptRtcBase->CR &= (~RTC_INIT);
	ptRtcBase->KEY = 0x0;
	while(ptRtcBase->CR & RTC_BSY);
}

static inline bool csp_rtc_is_busy(csp_rtc_t *ptRtcBase)
{
	return ((ptRtcBase->CR & RTC_BSY) >> 1);
}

static inline bool csp_rtc_is_running(csp_rtc_t *ptRtcBase)
{
	return (!(ptRtcBase->CR & RTC_INIT));
}

static inline void csp_rtc_set_time_hour(csp_rtc_t *ptRtcBase, bool bPm, uint8_t byVal)
{
	ptRtcBase->TIMR = (ptRtcBase->TIMR  & (~RTC_HOR_MSK) & (~RTC_PM_MSK)) |(byVal << RTC_HORU_POS) | (bPm << RTC_PM_POS);
}

static inline void csp_rtc_set_time_min(csp_rtc_t *ptRtcBase, uint8_t byVal)
{
	ptRtcBase->TIMR = (ptRtcBase->TIMR  & (~RTC_MIN_MSK)) | (byVal << RTC_MINU_POS);
}

static inline void csp_rtc_set_time_sec(csp_rtc_t *ptRtcBase, uint8_t byVal)
{
	ptRtcBase->TIMR = (ptRtcBase->TIMR  & (~RTC_SEC_MSK)) |  (byVal << RTC_SECU_POS);
}

static inline void csp_rtc_set_date_year(csp_rtc_t *ptRtcBase, uint8_t byVal)
{
	ptRtcBase->DATR = (ptRtcBase->DATR  & (~RTC_YEA_MSK)) |  (byVal << RTC_YEAU_POS);
}

static inline void csp_rtc_set_date_mon(csp_rtc_t *ptRtcBase,  uint8_t byVal)
{
	ptRtcBase->DATR = (ptRtcBase->DATR  & (~RTC_MON_MSK)) |  (byVal << RTC_MONU_POS);
}

static inline void csp_rtc_set_date_wday(csp_rtc_t *ptRtcBase, uint8_t byVal)
{
	ptRtcBase->DATR = (ptRtcBase->DATR  & (~RTC_WKD_MSK)) | (byVal << RTC_WKD_POS);
}

static inline void csp_rtc_set_date_day(csp_rtc_t *ptRtcBase, uint8_t byVal)
{
	ptRtcBase->DATR = (ptRtcBase->DATR  & (~RTC_DAY_MSK)) |  (byVal << RTC_DAYU_POS);
}

static inline void csp_rtc_set_date_week(csp_rtc_t *ptRtcBase, uint8_t byVal)
{
	ptRtcBase->DATR = (ptRtcBase->DATR  & (~RTC_WKD_MSK)) | (byVal << RTC_WKD_POS);
}

static inline uint8_t csp_rtc_read_year(csp_rtc_t *ptRtcBase)
{
	return (  ((ptRtcBase->DATR & RTC_YEAT_MSK) >> RTC_YEAT_POS) * 10 + ((ptRtcBase->DATR & RTC_YEAU_MSK) >> RTC_YEAU_POS)  );
}

static inline uint8_t csp_rtc_read_mon(csp_rtc_t *ptRtcBase)
{
	return (  ((ptRtcBase->DATR & RTC_MONT_MSK) >> RTC_MONT_POS) * 10 + ((ptRtcBase->DATR & RTC_MONU_MSK) >> RTC_MONU_POS)  );
}

static inline uint8_t csp_rtc_read_mday(csp_rtc_t *ptRtcBase)
{
	return (  ((ptRtcBase->DATR & RTC_DAYT_MSK) >> RTC_DAYT_POS) * 10 + ((ptRtcBase->DATR & RTC_DAYU_MSK) >> RTC_DAYU_POS)  );
}

static inline uint8_t csp_rtc_read_wday(csp_rtc_t *ptRtcBase)
{
	return  ((ptRtcBase->DATR & RTC_WKD_MSK) >> RTC_WKD_POS) ;
}

static inline uint8_t csp_rtc_read_hour(csp_rtc_t *ptRtcBase)
{
	return (  ((ptRtcBase->TIMR & RTC_HORT_MSK) >> RTC_HORT_POS) * 10 + ((ptRtcBase->TIMR & RTC_HORU_MSK) >> RTC_HORU_POS)  );
}

static inline uint8_t csp_rtc_read_min(csp_rtc_t *ptRtcBase)
{
	return (  ((ptRtcBase->TIMR & RTC_MINT_MSK) >> RTC_MINT_POS) * 10 + ((ptRtcBase->TIMR & RTC_MINU_MSK) >> RTC_MINU_POS)  );
}

static inline uint8_t csp_rtc_read_sec(csp_rtc_t *ptRtcBase)
{
	return (  ((ptRtcBase->TIMR & RTC_SECT_MSK) >> RTC_SECT_POS) * 10 + ((ptRtcBase->TIMR & RTC_SECU_MSK) >> RTC_SECU_POS)  );
}

static inline void csp_rtc_alm_enable(csp_rtc_t *ptRtcBase, uint8_t byAlm, bool bEnable)
{

	ptRtcBase->KEY = 0xCA53;
	if (bEnable)
		ptRtcBase->CR |= 0x1 << (3 + byAlm);
	else	
		ptRtcBase->CR &= ~(0x1 << (3 + byAlm));
	ptRtcBase->KEY = 0x0;
	while(ptRtcBase->CR & RTC_BSY);
}

static inline void csp_rtc_alm_set_day(csp_rtc_t *ptRtcBase, uint8_t byAlm,  uint8_t byVal)
{
	if (byAlm == RTC_ALMA)
		ptRtcBase->ALRA = (ptRtcBase->ALRA & (~RTC_ALM_DAY_MSK)) |   (byVal << RTC_ALM_DAYU_POS);
	else
		ptRtcBase->ALRB = (ptRtcBase->ALRB & (~RTC_ALM_DAY_MSK)) |   (byVal << RTC_ALM_DAYU_POS);
}

static inline void csp_rtc_alm_set_hour(csp_rtc_t *ptRtcBase, uint8_t byAlm, uint8_t byPm, uint8_t byVal)
{
	if (byAlm == RTC_ALMA)
		ptRtcBase->ALRA = (ptRtcBase->ALRA & (~RTC_ALM_HOR_MSK) & (~RTC_ALM_PM)) |  (byVal << RTC_ALM_HORU_POS) | (byPm<<22);
	else
		ptRtcBase->ALRB = (ptRtcBase->ALRB & (~RTC_ALM_HOR_MSK) & (~RTC_ALM_PM)) |  (byVal << RTC_ALM_HORU_POS) | (byPm<<22);
}

static inline void csp_rtc_alm_set_min(csp_rtc_t *ptRtcBase, uint8_t byAlm, uint8_t byVal)
{
	if (byAlm == RTC_ALMA)
		ptRtcBase->ALRA = (ptRtcBase->ALRA & (~RTC_ALM_MIN_MSK)) | (byVal << RTC_ALM_MINU_POS);
	else
		ptRtcBase->ALRB = (ptRtcBase->ALRB & (~RTC_ALM_MIN_MSK)) |  (byVal << RTC_ALM_MINU_POS);
}

static inline void csp_rtc_alm_set_sec(csp_rtc_t *ptRtcBase, uint8_t byAlm,  uint8_t byVal)
{
	if (byAlm == RTC_ALMA)
		ptRtcBase->ALRA = (ptRtcBase->ALRA & (~RTC_ALM_SEC_MSK)) |  (byVal << RTC_ALM_SECU_POS);
	else
		ptRtcBase->ALRB = (ptRtcBase->ALRB & (~RTC_ALM_SEC_MSK)) | (byVal << RTC_ALM_SECU_POS);
}

static inline uint8_t csp_rtc_alm_read_mday(csp_rtc_t *ptRtcBase, uint8_t byAlm)
{
	if (byAlm == RTC_ALMA)
		return (  ((ptRtcBase->ALRA & RTC_ALM_DAYT_MSK) >> RTC_ALM_DAYT_POS) * 10 + ((ptRtcBase->ALRA & RTC_ALM_DAYU_MSK) >> RTC_ALM_DAYU_POS)  );
	else
		return (  ((ptRtcBase->ALRB & RTC_ALM_DAYT_MSK) >> RTC_ALM_DAYT_POS) * 10 + ((ptRtcBase->ALRB & RTC_ALM_DAYU_MSK) >> RTC_ALM_DAYU_POS)  );
}

static inline uint8_t csp_rtc_alm_read_wday(csp_rtc_t *ptRtcBase, uint8_t byAlm)
{
	if (byAlm == RTC_ALMA)
		return (   ((ptRtcBase->ALRA & RTC_ALM_DAYU_MSK) >> RTC_ALM_DAYU_POS)  );
	else
		return (  ((ptRtcBase->ALRB & RTC_ALM_DAYU_MSK) >> RTC_ALM_DAYU_POS)  );
}

static inline uint8_t csp_rtc_alm_read_hour(csp_rtc_t *ptRtcBase, uint8_t byAlm)
{
	if (byAlm == RTC_ALMA)
		return (  ((ptRtcBase->ALRA & RTC_ALM_HORT_MSK) >> RTC_ALM_HORT_POS) * 10 + ((ptRtcBase->ALRA & RTC_ALM_HORU_MSK) >> RTC_ALM_HORU_POS)  );
	else
		return (  ((ptRtcBase->ALRB & RTC_ALM_HORT_MSK) >> RTC_ALM_HORT_POS) * 10 + ((ptRtcBase->ALRB & RTC_ALM_HORU_MSK) >> RTC_ALM_HORU_POS)  );
}

static inline uint8_t csp_rtc_alm_read_min(csp_rtc_t *ptRtcBase, uint8_t byAlm)
{
	if (byAlm == RTC_ALMA)
		return (  ((ptRtcBase->ALRA & RTC_ALM_MINT_MSK) >> RTC_ALM_MINT_POS) * 10 + ((ptRtcBase->ALRA & RTC_ALM_MINU_MSK) >> RTC_ALM_MINU_POS)  );
	else
		return (  ((ptRtcBase->ALRB & RTC_ALM_MINT_MSK) >> RTC_ALM_MINT_POS) * 10 + ((ptRtcBase->ALRB & RTC_ALM_MINU_MSK) >> RTC_ALM_MINU_POS)  );
}

static inline uint8_t csp_rtc_alm_read_sec(csp_rtc_t *ptRtcBase, uint8_t byAlm)
{
	if (byAlm == RTC_ALMA)
		return (  ((ptRtcBase->ALRA & RTC_ALM_SECT_MSK) >> RTC_ALM_SECT_POS) * 10 + ((ptRtcBase->ALRA & RTC_ALM_SECU_MSK) >> RTC_ALM_SECU_POS)  );
	else
		return (  ((ptRtcBase->ALRB & RTC_ALM_SECT_MSK) >> RTC_ALM_SECT_POS) * 10 + ((ptRtcBase->ALRB & RTC_ALM_SECU_MSK) >> RTC_ALM_SECU_POS)  );
}

static inline bool csp_rtc_alm_read_dmsk(csp_rtc_t *ptRtcBase, uint8_t byAlm)
{
	if (byAlm == RTC_ALMA)
		return ( (ptRtcBase->ALRA & RTC_ALM_DMSK_MSK) >> RTC_ALM_DMSK_POS);
	else
		return ( (ptRtcBase->ALRB & RTC_ALM_DMSK_MSK) >> RTC_ALM_DMSK_POS);
}

static inline bool csp_rtc_alm_read_wdsel(csp_rtc_t *ptRtcBase, uint8_t byAlm)
{
	if (byAlm == RTC_ALMA)
		return ( (ptRtcBase->ALRA & RTC_ALM_WDSEL_MSK) >> RTC_ALM_WDSEL_POS);
	else
		return ( (ptRtcBase->ALRB & RTC_ALM_WDSEL_MSK) >> RTC_ALM_WDSEL_POS);
}


static inline void csp_rtc_alm_set_mode(csp_rtc_t *ptRtcBase, uint8_t byAlm, bool bWdsel, bool bDmsk, bool bHmsk, bool bMmsk, bool bSmsk)
{
	ptRtcBase->KEY = 0xCA53;
	if (byAlm == RTC_ALMA)
		ptRtcBase->ALRA = (ptRtcBase->ALRA & (~RTC_ALM_MODE)) | bWdsel << RTC_ALM_WDSEL_POS |(bDmsk << RTC_ALM_DMSK_POS) | (bHmsk << RTC_ALM_HMSK_POS) | (bSmsk << RTC_ALM_SMSK_POS);
	else
		ptRtcBase->ALRB = (ptRtcBase->ALRB & (~RTC_ALM_MODE)) | (bDmsk << RTC_ALM_DMSK_POS) | (bHmsk << RTC_ALM_HMSK_POS) | (bSmsk << RTC_ALM_SMSK_POS);
	ptRtcBase->KEY = 0x0;
}

static inline void csp_rtc_int_enable(csp_rtc_t *ptRtcBase, rtc_int_e eInt, bool bEnable)
{
	//ptRtcBase->KEY = 0xCA53;	
	if (bEnable)
		ptRtcBase->IMCR |= eInt;
	else
		ptRtcBase->IMCR &= ~eInt;
	//ptRtcBase->KEY = 0x0;
}


static inline void csp_rtc_int_clr(csp_rtc_t *ptRtcBase, rtc_int_e eInt)
{
	ptRtcBase->ICR = eInt;
}

static inline uint32_t csp_rtc_get_int_st(csp_rtc_t *ptRtcBase)
{
	return (ptRtcBase->MISR);
}

static inline uint32_t csp_rtc_get_imcr(csp_rtc_t *ptRtcBase)
{
	return (ptRtcBase->IMCR);
}
static inline void csp_rtc_swf_trg(csp_rtc_t *ptRtcBase, uint8_t byTrg)
{
	ptRtcBase -> EVSWF = 0x1 << byTrg;
}

void csp_rtc_clk_config(csp_rtc_t *ptRtcBase, rtc_clksrc_e eOsc);
void csp_rtc_init(csp_rtc_t *ptRtcBase, uint8_t byOsc);
csp_error_t csp_rtc_set_date(csp_rtc_t *ptRtcBase, uint8_t byYear, uint8_t byMon, uint8_t byWday, uint8_t byDay);
csp_error_t csp_rtc_set_time(csp_rtc_t *ptRtcBase, bool bPm, uint8_t byHor, uint8_t byMin,uint8_t bySec);
csp_error_t csp_rtc_alm_set(csp_rtc_t *ptRtcBase, uint8_t byAlm, uint8_t byDay, uint8_t byPm, uint8_t byHor, uint8_t byMin,uint8_t bySec);
void csp_rtc_alm_reset(csp_rtc_t *ptRtcBase, uint8_t byAlm);
csp_error_t csp_rtc_set_trgsrc(csp_rtc_t *ptRtcBase, uint8_t byTrg, csp_rtc_trgsel_e eSrc);
csp_error_t csp_rtc_set_trgprd(csp_rtc_t *ptRtcBase, uint8_t byTrg, uint8_t byPrd);
#endif   /* CSP_EPWM_H */


