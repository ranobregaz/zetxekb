/***********************************************************************//** 
 * \file  csp_rtc.c
 * \brief  rtc functions in csp level
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-25 <td>V0.0  <td>WNN   <td>initial
 *  * <tr><td> 2021-1-10 <td>V0.0  <td>WNN   <td>modify
 * </table>
 * *********************************************************************
*/

#include "csp_rtc.h"
#include "sys_clk.h"

 uint8_t  dec2bcd(uint32_t wData)
 {	
	return (((wData / 10) << 4) | (wData % 10));
 }

void csp_rtc_clk_config(csp_rtc_t *ptRtcBase, rtc_clksrc_e eOsc)
{
	uint8_t byDiva = 0;
	uint16_t byDivs = 0;
	
	switch (eOsc)
	{ 
		case (RTC_ISOSC):
			
			byDiva = 49;
			byDivs = 134;
			break;
		case (RTC_EMOSC):
			byDiva = 63;
			byDivs = 31;
			break;
		case (RTC_IMOSC_DIV4):
			switch(csp_get_imosc_fre(SYSCON_REG_BASE))
			{
				case (0):	//5.556MHz
					byDiva = 124;
					byDivs = 2777;
					break;
				case (1):	//4.194MHz
					byDiva = 124;
					byDivs = 2096;
					break;
				case (2):	//2.097MHz
					byDiva = 124;
					byDivs = 1047;
					break;
				case (3):	//131.072KHz
					byDiva = 124;
					byDivs = 127;
					break;
				default:
					break;
			}			
			break;
		case (RTC_EMOSC_DIV4):
			byDiva = 63;
			byDivs = 31;
			break;
		default:
			break;
	}
	
	/*csp_rtc_set_clksrc(ptRtcBase, eOsc);
	mdelay(1);
	csp_rtc_set_diva(ptRtcBase, byDiva);	
	csp_rtc_set_divs(ptRtcBase, byDivs);*/
	ptRtcBase->KEY = 0xCA53;
	ptRtcBase->CCR = (ptRtcBase->CCR & (~RTC_CLKSRC_MSK) & (~RTC_DIVA_MSK)& (~RTC_DIVS_MSK)) | (eOsc << RTC_CLKSRC_POS)|(byDiva << RTC_DIVA_POS)| (byDivs << RTC_DIVS_POS);
	ptRtcBase->KEY = 0x0;
	//while((ptRtcBase->CCR & RTC_CLK_STABLE) == 0);
	
}


csp_rtc_time_t ptRtcAlarmTime;

void csp_rtc_init(csp_rtc_t *ptRtcBase, uint8_t byOsc)
{
	soc_clk_enable(RTC_SYS_CLK);
		
	csp_rtc_stop(ptRtcBase);
	csp_rtc_debug_enable(ptRtcBase, ENABLE);
	//csp_rtc_wr_key(ptRtcBase);
	csp_rtc_clk_enable(ptRtcBase, ENABLE);
	csp_rtc_clk_config(ptRtcBase, byOsc);
	//csp_rtc_ers_key(ptRtcBase);
	csp_rtc_rb_enable(ptRtcBase, ENABLE);
	csp_rtc_set_fmt(ptRtcBase, RTC_24FMT);
	csp_rtc_alm_enable(ptRtcBase, RTC_ALMB, DISABLE);
	csp_rtc_alm_enable(ptRtcBase, RTC_ALMA, DISABLE);
	csp_rtc_int_enable(ptRtcBase, RTC_INT_ALRA|RTC_INT_ALRB|RTC_INT_CPRD|RTC_INT_TRGEV0|RTC_INT_TRGEV1, DISABLE);
	csp_rtc_int_clr(ptRtcBase, RTC_INT_ALRA|RTC_INT_ALRB|RTC_INT_CPRD|RTC_INT_TRGEV0|RTC_INT_TRGEV1);

	
	///to fix a bug here
	ptRtcAlarmTime.mday = 2;
	ptRtcAlarmTime.hour = 9;
	ptRtcAlarmTime.min	= 59;
	ptRtcAlarmTime.sec = 59;	
	csp_rtc_alm_enable(ptRtcBase, RTC_ALMA, DISABLE);
	csp_rtc_alm_set(ptRtcBase, RTC_ALMA, ptRtcAlarmTime.mday, RTC_AM, ptRtcAlarmTime.hour, ptRtcAlarmTime.min, ptRtcAlarmTime.sec);
	csp_rtc_alm_set_msk(ptRtcBase, RTC_ALMA, 1, 0, 0, 0);
	csp_rtc_int_clr(ptRtcBase, RTC_INT_ALRA);
	csp_rtc_int_enable(ptRtcBase, RTC_INT_ALRA, ENABLE);
	csp_rtc_alm_enable(ptRtcBase, RTC_ALMA, ENABLE);
	
}

csp_error_t csp_rtc_set_date(csp_rtc_t *ptRtcBase, uint8_t byYear, uint8_t byMon, uint8_t byWday, uint8_t byDay)
{
	uint8_t byVal;
	csp_error_t ret = CSP_SUCCESS;
	if (csp_rtc_is_running(ptRtcBase))
		return CSP_FAIL;
		
	csp_rtc_wr_key(ptRtcBase);
	byVal = dec2bcd(byYear);
	csp_rtc_set_date_year(ptRtcBase, byVal);
	byVal = dec2bcd(byMon);
	csp_rtc_set_date_mon(ptRtcBase, byVal);
	byVal = dec2bcd(byWday);
	csp_rtc_set_date_wday(ptRtcBase, byVal);
	byVal = dec2bcd(byDay);
	csp_rtc_set_date_day(ptRtcBase, byVal);	
	csp_rtc_ers_key(ptRtcBase);
	
	return ret;
}

csp_error_t csp_rtc_set_time(csp_rtc_t *ptRtcBase, bool bPm, uint8_t byHor, uint8_t byMin,uint8_t bySec)
{
	uint8_t byVal;
	csp_error_t ret = CSP_SUCCESS;

	if (csp_rtc_is_running(ptRtcBase))
		return CSP_FAIL;
			
//	if (bPm == RTC_AM && byHor > 12)
//		ret = CSP_FAIL;
//	else {
	
		csp_rtc_wr_key(ptRtcBase);
		byVal = dec2bcd(byHor);
		csp_rtc_set_time_hour(ptRtcBase, bPm, byVal);
		byVal = dec2bcd(byMin);
		csp_rtc_set_time_min(ptRtcBase, byVal);
		byVal = dec2bcd(bySec);
		csp_rtc_set_time_sec(ptRtcBase, byVal);
		csp_rtc_ers_key(ptRtcBase);
//	}
	
	return ret;
}



csp_error_t csp_rtc_alm_set(csp_rtc_t *ptRtcBase, uint8_t byAlm, uint8_t byWeekDay, uint8_t byPm, uint8_t byHor, uint8_t byMin,uint8_t bySec)
{
	uint8_t byVal;
	csp_error_t ret = CSP_SUCCESS;
	
	
	csp_rtc_wr_key(ptRtcBase);
		
	byVal = dec2bcd(byWeekDay);
	csp_rtc_alm_set_day(ptRtcBase, byAlm, byWeekDay);
	byVal = dec2bcd(byHor);
	csp_rtc_alm_set_hour(ptRtcBase, byAlm, byPm, byVal);
	byVal = dec2bcd(byMin);
	csp_rtc_alm_set_min(ptRtcBase, byAlm, byVal);
	byVal = dec2bcd(bySec);
	csp_rtc_alm_set_sec(ptRtcBase, byAlm, byVal);
	
	csp_rtc_ers_key(ptRtcBase);
	return ret;
}


void csp_rtc_alm_reset(csp_rtc_t *ptRtcBase, uint8_t byAlm)
{
	
	csp_rtc_alm_enable(ptRtcBase, byAlm, DISABLE);
	ptRtcBase->KEY = 0xCA53;
	if (byAlm == RTC_ALMA)
		ptRtcBase->ALRA = 0x0;
	else
		ptRtcBase->ALRB = 0x0;
	ptRtcBase->KEY = 0x0;	
}


csp_error_t csp_rtc_set_trgsrc(csp_rtc_t *ptRtcBase, uint8_t byTrg, csp_rtc_trgsel_e eSrc)
{
	if (byTrg > 1)
		return CSP_FAIL;
	ptRtcBase -> EVTRG = (ptRtcBase->EVTRG & ~(RTC_TRGSEL0_MSK << (byTrg<<2) )) | (eSrc << (RTC_TRGSEL0_POS+ (byTrg<<2)));
	return CSP_SUCCESS;
}

csp_error_t csp_rtc_set_trgprd(csp_rtc_t *ptRtcBase, uint8_t byTrg, uint8_t byPrd)
{
	if (byTrg > 1)
		return CSP_FAIL;
	ptRtcBase -> EVPS = (ptRtcBase->EVPS & ~(RTC_TRGEV0PRD_MSK << (byTrg<<2))) | ((byPrd & 0xf) << (RTC_TRGEV0PRD_POS + (byTrg<<2)));
	return CSP_SUCCESS;
}

csp_error_t csp_rtc_set_trgcnt(csp_rtc_t *ptRtcBase, uint8_t byTrg, uint8_t byCnt)
{
	if (byTrg > 1)
		return CSP_FAIL;
	ptRtcBase -> EVPS =  (ptRtcBase->EVPS & ~(RTC_TRGEV0CNT_MSK << (byTrg<<2))) | ((byCnt & 0xf) << (RTC_TRGEV0CNT_POS + (byTrg<<2)));
	return CSP_SUCCESS;
}

uint8_t csp_rtc_get_trgcnt(csp_rtc_t *ptRtcBase, uint8_t byTrg)
{
	if (byTrg > 1)
		return CSP_FAIL;
	return (ptRtcBase->EVPS & (RTC_TRGEV0CNT_MSK << (RTC_TRGEV0CNT_POS + (byTrg<<2))));
}