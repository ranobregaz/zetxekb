/***********************************************************************//** 
 * \file  rtc.c
 * \brief  rtc driver
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-6-2 <td>V2.0 <td>WNN    <td>initial
 * </table>
 * *********************************************************************
*/

/* include ----------------------------------------------------------------*/
#include "csp.h"
#include "rtc.h"
#include "rtc_alg.h"
#include "irq.h"
#include "soc.h"


/* externs function--------------------------------------------------------*/
/* private function--------------------------------------------------------*/


static void apt_rtc_alm_set_time(csp_rtc_t *ptRtc, uint8_t byAlm, uint8_t byDay, uint8_t byPm, uint8_t byHor, uint8_t byMin,uint8_t bySec);
static csp_error_t apt_rtc_set_time(csp_rtc_t *ptRtc, bool bPm, uint8_t byHor, uint8_t byMin,uint8_t bySec);
static csp_error_t apt_rtc_set_date(csp_rtc_t *ptRtc, uint8_t byYear, uint8_t byMon, uint8_t byWday, uint8_t byDay);
csp_error_t apt_rtc_set_trgsrc(csp_rtc_t *ptRtc, uint8_t byTrg, csp_rtc_trgsel_e eSrc);
csp_error_t apt_rtc_set_trgprd(csp_rtc_t *ptRtc, uint8_t byTrg, uint8_t byPrd);

/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/**
  \brief       Initialize RTC Interface. Initializes the resources needed for the RTC interface
  \param       ptRtc    rtc handle
  \param	   tConfig  rtc basic parameters \ref csi_rtc_config_t
  \return      none
*/
csp_rtc_time_t tRtcAlarmTime;
void csi_rtc_init(csp_rtc_t *ptRtc, csi_rtc_config_t *tConfig)
{
    uint8_t byDiva = 0;
	uint16_t byDivs = 0;
	
	soc_clk_enable(RTC_SYS_CLK);
		
	csp_rtc_stop(ptRtc);
	
	switch (tConfig->byClkSrc)
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
			switch(csp_get_imosc_fre(SYSCON))
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
	
	ptRtc->KEY = 0xCA53;
	ptRtc->CCR = (ptRtc->CCR & (~RTC_CLKSRC_MSK) & (~RTC_DIVA_MSK)& (~RTC_DIVS_MSK)) | (tConfig->byClkSrc << RTC_CLKSRC_POS)|(byDiva << RTC_DIVA_POS)| (byDivs << RTC_DIVS_POS) | RTC_CLKEN;
	ptRtc->KEY = 0x0;
	while((ptRtc->CCR & RTC_CLK_STABLE) == 0);
	
	//csp_rtc_ers_key(ptRtc);
	csp_rtc_rb_enable(ptRtc, ENABLE);
	csp_rtc_set_fmt(ptRtc, tConfig->byFmt);
	csp_rtc_alm_enable(ptRtc, RTC_ALMB, DISABLE);
	csp_rtc_alm_enable(ptRtc, RTC_ALMA, DISABLE);
	
	
	csp_rtc_int_enable(ptRtc, RTC_INT_ALMA|RTC_INT_ALMB|RTC_INT_CPRD|RTC_INT_TRGEV0|RTC_INT_TRGEV1, DISABLE);
	csp_rtc_int_clr(ptRtc, RTC_INT_ALMA|RTC_INT_ALMB|RTC_INT_CPRD|RTC_INT_TRGEV0|RTC_INT_TRGEV1);

	
	///to fix a bug here
	tRtcAlarmTime.mday = 2;
	tRtcAlarmTime.hour = 9;
	tRtcAlarmTime.min	= 59;
	tRtcAlarmTime.sec = 59;	
	csp_rtc_alm_enable(ptRtc, RTC_ALMB, DISABLE);
	apt_rtc_alm_set_time(ptRtc, RTC_ALMB, tRtcAlarmTime.mday, RTC_AM, tRtcAlarmTime.hour, tRtcAlarmTime.min, tRtcAlarmTime.sec);
	csp_rtc_alm_set_mode(ptRtc, RTC_ALMB, 2, 1, 0, 0, 0);
	csp_rtc_int_clr(ptRtc, RTC_INT_ALMB);
	csp_rtc_int_enable(ptRtc, RTC_INT_ALMB, ENABLE);
	csp_rtc_alm_enable(ptRtc, RTC_ALMB, ENABLE);
	csi_irq_enable((uint32_t *)ptRtc);	

}

/**
  \brief       Read back function enable control. To save current, disable read back.
  \param       rtc        handle rtc handle to operate
  \param       bEnable    
  \return      none
*/
void csi_rtc_rb_enable(csp_rtc_t *ptRtc, bool bEnable)
{
	csp_rtc_rb_enable(ptRtc, bEnable);
}

/**
  \brief       To set/change RTC format
  \param       rtc handle rtc handle to operate
  \param       eFmt \ref rtc_fmt_e    
  \return      none
*/
void csi_rtc_change_fmt(csp_rtc_t *ptRtc,  rtc_fmt_e eFmt)
{
	csp_rtc_set_fmt(ptRtc, eFmt);
}

/**
  \brief       To start RTC 
  \param       rtc handle rtc handle to operate
  \return      none
*/
void csi_rtc_start(csp_rtc_t *ptRtc)
{
	csp_rtc_run(ptRtc);
}

/**
  \brief       Set system date and run after setting
   * 				week day will be calculated in this function
  \param       rtc        handle rtc handle to operate
  \param       rtctime    pointer to rtc time
  \return      error code \ref csi_error_t
*/
csi_error_t csi_rtc_set_time(csp_rtc_t *ptRtc, csi_rtc_time_t *rtctime)
{   
    csi_error_t ret = CSI_OK;
		
	do {
		if (csp_rtc_get_fmt(ptRtc) == RTC_12FMT && rtctime->tm_hour>12) {
			ret = CSI_ERROR;
			break;
		}
		
		ret = (csi_error_t) clock_check_tm_ok((const struct tm *)rtctime);
        if (ret < CSI_OK) {
            break;
		
        }
		csp_rtc_stop(ptRtc);
		
		
		rtctime->tm_wday = get_week_by_date((struct tm *)rtctime);
		
		
		apt_rtc_set_date(ptRtc, rtctime->tm_year, rtctime->tm_mon, rtctime->tm_wday, rtctime->tm_mday);
		
		if (rtctime->tm_hour > 12 && csp_rtc_get_fmt(ptRtc) == RTC_12FMT)
			ret =  apt_rtc_set_time(ptRtc, RTC_PM, rtctime->tm_hour-12, rtctime->tm_min,rtctime->tm_sec);
		else
			ret =  apt_rtc_set_time(ptRtc, RTC_AM, rtctime->tm_hour, rtctime->tm_min,rtctime->tm_sec);
		
	
		if (ret < CSI_OK) {
            break;
        }
	}while(0);
	
	//csp_rtc_run(ptRtc);

	return ret;
}

/**
  \brief   Config RTC alarm
  \param   ptRtc      handle rtc handle to operate
  \param   byAlm	  RTC_ALMA/RTC_ALMB
  \param   rtctime    alarm time(s) 
  \param   byMode	  	0: day       hour min sec
						1: weekday   hour min sec
						2:           hour min sec

  \return  error code \ref csi_error_t
*/
csi_error_t csi_rtc_set_alarm(csp_rtc_t *ptRtc, uint8_t byAlm, uint8_t byMode, csi_rtc_time_t *tpRtcTime)
{ 	
	bool bDmsk = 0;
	bool bWdsel = 0;
	bool bHmsk = 0;
	bool bMmsk = 0;
	bool bSmsk = 0;
	

	if (tpRtcTime -> tm_yday == 0xff || byMode == 2) {
		bDmsk = 1;
		tpRtcTime->tm_mday = 0;			
	}
	if (tpRtcTime -> tm_hour == 0xff) {
		bHmsk = 1;
		tpRtcTime->tm_hour = 0;
	}
	if (tpRtcTime -> tm_min == 0xff) {
		bMmsk = 1;
		tpRtcTime->tm_min = 0;
	}
	if (tpRtcTime -> tm_sec == 0xff) {
		bSmsk = 1;	
		tpRtcTime->tm_sec = 0;
	}
	switch (byMode)
	{
		case 0:	bDmsk = 0;
				bWdsel = 0;
			break;
		case 1: bDmsk = 0;
				bWdsel = 1;
			break;
		case 2: bDmsk = 1;
				bWdsel = 0;
			break;
	}
	
	switch (byAlm)
	{
		case (RTC_ALMA): 	csp_rtc_int_clr(ptRtc, RTC_INT_ALMA);
							csp_rtc_int_enable(ptRtc, RTC_INT_ALMA, ENABLE);
							break;
		case (RTC_ALMB):	csp_rtc_int_clr(ptRtc, RTC_INT_ALMB);
							csp_rtc_int_enable(ptRtc, RTC_INT_ALMB, ENABLE);
							break;
		default:
			return CSI_ERROR;
	}
	
	csi_rtc_int_enable(RTC, RTC_INT_ALMA, ENABLE);
	csp_rtc_alm_enable(ptRtc, byAlm, DISABLE);
	apt_rtc_alm_set_time(ptRtc, byAlm, tpRtcTime->tm_mday, RTC_AM,  tpRtcTime->tm_hour, tpRtcTime->tm_min,tpRtcTime->tm_sec);
	csp_rtc_alm_set_mode(ptRtc, byAlm, bWdsel, bDmsk, bHmsk, bMmsk, bSmsk);
	csp_rtc_alm_enable(ptRtc, byAlm, ENABLE);
	
	return CSI_OK;
}

/**
  \brief       Cancel the rtc alarm, keep last alarm time and mode setting
  \param       ptRtc    rtc handle to operate
  \param       byAlm	RTC_ALMA/RTC_ALMB
  \return      none
*/
void csi_rtc_cancel_alarm(csp_rtc_t *ptRtc, uint8_t byAlm)
{
    
	csp_rtc_alm_enable(ptRtc, byAlm, DISABLE);
	switch (byAlm)
	{
		case (RTC_ALMA): 	csi_rtc_int_enable(ptRtc, RTC_INT_ALMA, DISABLE);
							csp_rtc_int_clr(ptRtc, RTC_INT_ALMA);
							break;
		case (RTC_ALMB):	csi_rtc_int_enable(ptRtc, RTC_INT_ALMB, DISABLE);
							csp_rtc_int_clr(ptRtc, RTC_INT_ALMB);
							break;
		default: break;
	}
}

/**
  \brief       Config RTC alarm ture timer
  \param[in]   ptRtc      handle rtc handle to operate
  \param[in]   eOut     rtc output
  \return      none
*/
void csi_rtc_set_alarm_out(csp_rtc_t *ptRtc, csi_rtc_osel_e eOut)
{	
	csp_rtc_set_osel(ptRtc, eOut);
}



/**
  \brief    use rtc as a timer
  \param	callback  callback function to be called when time = ePrd
  \param 	ePrd    time interval of the timer.
  \param[in]   rtc    rtc handle to operate
*/
void csi_rtc_start_as_timer(csp_rtc_t *ptRtc, csi_rtc_timer_e ePrd)
{	
	
	csp_rtc_set_cprd(ptRtc, ePrd);
	csi_rtc_int_enable(ptRtc, RTC_INT_CPRD , ENABLE);
	//csp_rtc_run(ptRtc);
}

/**
  \brief   RTC interrupt enable/disable
  \param   ptRtc      handle rtc handle to operate
  \param   eIntSrc	  interrupt source	
  \param   bEnable    ENABLE/DISABLE 
  \return  none
*/
void csi_rtc_int_enable(csp_rtc_t *ptRtc, rtc_int_e eIntSrc, bool bEnable)
{
	csp_rtc_int_enable(ptRtc, eIntSrc, bEnable);	
	
	if (bEnable) {
		csi_irq_enable((uint32_t *)ptRtc);
	}
	else {
		if (eIntSrc == csp_rtc_get_imcr(ptRtc)) {
			csi_irq_disable((uint32_t *)ptRtc);
		}
	}
}

/**
  \brief       Get system time
  \param[in]   rtc        handle rtc handle to operate
  \param[out]  rtctime    pointer to rtc time
  \return      none
*/
void csi_rtc_get_time(csp_rtc_t *ptRtc, csi_rtc_time_t *rtctime)
{
	rtctime->tm_year = csp_rtc_read_year(ptRtc);
	rtctime->tm_mon = csp_rtc_read_mon(ptRtc);
	rtctime->tm_wday = csp_rtc_read_wday(ptRtc);
	rtctime->tm_mday = csp_rtc_read_mday(ptRtc);
	rtctime->tm_hour = csp_rtc_read_hour(ptRtc);
	rtctime->tm_min = csp_rtc_read_min(ptRtc);
	rtctime->tm_sec = csp_rtc_read_sec(ptRtc);
}

/**
  \brief       Get alarm remaining time
  \param[in]   rtc    rtc handle to operate
  \param	   byAlm  RTC_ALMA/RTC_ALMB
  \return      the remaining time(s)
*/
uint32_t csi_rtc_get_alarm_remaining_time(csp_rtc_t *ptRtc, uint8_t byAlm)
{
    volatile csi_rtc_time_t tCurrentTime ;
	volatile csi_rtc_time_t tAlmTime;
	uint32_t wCurrentTime, wAlmTime;

	tCurrentTime.tm_mday = csp_rtc_read_mday(ptRtc); 
	tCurrentTime.tm_wday = csp_rtc_read_wday(ptRtc); 
	tCurrentTime.tm_hour = csp_rtc_read_hour(ptRtc);
	tCurrentTime.tm_min = csp_rtc_read_min(ptRtc); 
	tCurrentTime.tm_sec = csp_rtc_read_sec(ptRtc); 
	
	
	tAlmTime.tm_mday = csp_rtc_alm_read_mday(ptRtc, byAlm); 
	tAlmTime.tm_wday = csp_rtc_alm_read_wday(ptRtc, byAlm); 
	tAlmTime.tm_hour = csp_rtc_alm_read_hour(ptRtc, byAlm);
	tAlmTime.tm_min = csp_rtc_alm_read_min(ptRtc, byAlm); 
	tAlmTime.tm_sec = csp_rtc_alm_read_sec(ptRtc, byAlm); 
	
	if (csp_rtc_alm_read_dmsk(ptRtc, byAlm) == 1) {
		wCurrentTime = tCurrentTime.tm_hour * 3600 + tCurrentTime.tm_min * 60 + tCurrentTime.tm_sec;
		wAlmTime = tAlmTime.tm_hour * 3600 + tAlmTime.tm_min * 60 + tAlmTime.tm_sec;
		if(wAlmTime < wCurrentTime)
			return (24*3600 - wCurrentTime + wAlmTime);
	
	}
	else {
		if (csp_rtc_alm_read_wdsel(ptRtc, byAlm) == 1) {
			wCurrentTime = tCurrentTime.tm_wday * 86400 + tCurrentTime.tm_hour * 3600 + tCurrentTime.tm_min * 60 + tCurrentTime.tm_sec;
			wAlmTime = tAlmTime.tm_wday * 86400 + tAlmTime.tm_hour * 3600 + tAlmTime.tm_min * 60 + tAlmTime.tm_sec;
			if(wAlmTime < wCurrentTime)
				return (7*24*3600 - wCurrentTime + wAlmTime);
		}
		else {
			wCurrentTime = tCurrentTime.tm_mday * 86400 + tCurrentTime.tm_hour * 3600 + tCurrentTime.tm_min * 60 + tCurrentTime.tm_sec;
			wAlmTime = tAlmTime.tm_mday * 86400 + tAlmTime.tm_hour * 3600 + tAlmTime.tm_min * 60 + tAlmTime.tm_sec;
			if(wAlmTime < wCurrentTime)
				return CSI_UNSUPPORTED;	
		}
		
	}
	
	return (wAlmTime - wCurrentTime);
}


/** \brief evtrg source output config  
 * 
 *  \param[in] ptRtc: RTC handle to operate
 *  \param[in] byEvtrg: rtc evtrg channel(1~4) 
 *  \param[in] eTrgSrc: rtc evtrg source
 *  \param[in] trg_prd: event count period 
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_rtc_set_evtrg(csp_rtc_t *ptRtc, uint8_t byEvtrg, csi_rtc_trgsrc_e eTrgSrc, uint8_t byTrgPrd)
{
	
	csi_error_t ret = CSI_OK;
	
	if (apt_rtc_set_trgsrc(ptRtc, byEvtrg, eTrgSrc)<0)
		return CSI_ERROR;
	if (apt_rtc_set_trgprd(ptRtc, byEvtrg, byTrgPrd)<0)
		return CSI_ERROR;
	return ret;
}


//*****************************************************************************//


uint8_t  apt_dec2bcd(uint32_t wData)
{	
	return (((wData / 10) << 4) | (wData % 10));
}


static csp_error_t apt_rtc_set_date(csp_rtc_t *ptRtc, uint8_t byYear, uint8_t byMon, uint8_t byWday, uint8_t byDay)
{
	uint8_t byVal;
	csp_error_t ret = CSP_SUCCESS;
	if (csp_rtc_is_running(ptRtc))
		return CSP_FAIL;
		
	csp_rtc_wr_key(ptRtc);
	byVal = apt_dec2bcd(byYear);
	csp_rtc_set_date_year(ptRtc, byVal);
	byVal = apt_dec2bcd(byMon);
	csp_rtc_set_date_mon(ptRtc, byVal);
	byVal = apt_dec2bcd(byWday);
	csp_rtc_set_date_wday(ptRtc, byVal);
	byVal = apt_dec2bcd(byDay);
	csp_rtc_set_date_day(ptRtc, byVal);	
	csp_rtc_ers_key(ptRtc);
	
	return ret;
}


static csp_error_t apt_rtc_set_time(csp_rtc_t *ptRtc, bool bPm, uint8_t byHor, uint8_t byMin,uint8_t bySec)
{
	uint8_t byVal;
	csp_error_t ret = CSP_SUCCESS;

	if (csp_rtc_is_running(ptRtc))
		return CSP_FAIL;
			
//	if (bPm == RTC_AM && byHor > 12)
//		ret = CSP_FAIL;
//	else {
	
		csp_rtc_wr_key(ptRtc);
		byVal = apt_dec2bcd(byHor);
		csp_rtc_set_time_hour(ptRtc, bPm, byVal);
		byVal = apt_dec2bcd(byMin);
		csp_rtc_set_time_min(ptRtc, byVal);
		byVal = apt_dec2bcd(bySec);
		csp_rtc_set_time_sec(ptRtc, byVal);
		csp_rtc_ers_key(ptRtc);
//	}
	
	return ret;
}


static void apt_rtc_alm_set_time(csp_rtc_t *ptRtc, uint8_t byAlm, uint8_t byDay, uint8_t byPm, uint8_t byHor, uint8_t byMin,uint8_t bySec)
{
	uint8_t byVal;
		
	csp_rtc_wr_key(ptRtc);
		
	byVal = apt_dec2bcd(byDay);
	csp_rtc_alm_set_day(ptRtc, byAlm, byVal);
	byVal = apt_dec2bcd(byHor);
	csp_rtc_alm_set_hour(ptRtc, byAlm, byPm, byVal);
	byVal = apt_dec2bcd(byMin);
	csp_rtc_alm_set_min(ptRtc, byAlm, byVal);
	byVal = apt_dec2bcd(bySec);
	csp_rtc_alm_set_sec(ptRtc, byAlm, byVal);
	
	csp_rtc_ers_key(ptRtc);
}



csp_error_t apt_rtc_set_trgsrc(csp_rtc_t *ptRtc, uint8_t byTrg, csp_rtc_trgsel_e eSrc)
{
	if (byTrg > 1)
		return CSP_FAIL;
	ptRtc -> EVTRG = (ptRtc->EVTRG & ~(RTC_TRGSEL0_MSK << (byTrg<<2) )) | (eSrc << (RTC_TRGSEL0_POS+ (byTrg<<2)));
	return CSP_SUCCESS;
}


csp_error_t apt_rtc_set_trgprd(csp_rtc_t *ptRtc, uint8_t byTrg, uint8_t byPrd)
{
	if (byTrg > 1)
		return CSP_FAIL;
	ptRtc -> EVPS = (ptRtc->EVPS & ~(RTC_TRGEV0PRD_MSK << (byTrg<<2))) | (((byPrd-1) & 0xf) << (RTC_TRGEV0PRD_POS + (byTrg<<2)));
	return CSP_SUCCESS;
}
