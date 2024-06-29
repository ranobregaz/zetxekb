/***********************************************************************//** 
 * \file  rtc.c
 * \brief  real time timer 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-24 <td>V0.0  <td>WNN   <td>initial
 *  * <tr><td> 2021-1-8 <td>V0.0  <td>WNN   <td>modify
 * </table>
 * *********************************************************************
*/

#include "drv/common.h"
#include "csp_common.h"
#include "drv/rtc.h"
#include "csp_rtc.h"
#include "rtc_alg.h"
#include "drv/irq.h"

/**
  \brief       Internal timeout interrupt process function
  \param[in]   rtc    handle rtc handle to operate
  \return      None
*/
void rtc_irq_handler(void *arg)
{
	csi_rtc_t *rtc = (csi_rtc_t *)arg;
    csp_rtc_t *ptRtcBase = (csp_rtc_t *)HANDLE_REG_BASE(rtc);
	
	///AlarmA is used to fix a known bug
	if (csp_rtc_get_int_st(ptRtcBase) & (RTC_INT_ALRA)) {
		csp_rtc_int_clr(ptRtcBase, RTC_INT_ALRA);
		csp_rtc_stop(ptRtcBase);
		csp_rtc_wr_key(ptRtcBase);
		csp_rtc_set_time_hour(ptRtcBase, 0, 0x10);
		csp_rtc_set_time_min(ptRtcBase, 0x0);
		csp_rtc_set_time_sec(ptRtcBase, 0x0);
		csp_rtc_run(ptRtcBase);
	}
	else {
		if (rtc->callback) {
			int temp = csp_rtc_get_int_st(ptRtcBase);
			csp_rtc_int_clr(ptRtcBase, RTC_INT_ALRB|RTC_INT_CPRD|RTC_INT_TRGEV0|RTC_INT_TRGEV1);
			rtc->callback(rtc, rtc->arg);
		}
	}
}

/**
  \brief       Initialize RTC Interface. Initializes the resources needed for the RTC interface
  \param[in]   rtc    rtc handle to operate
  \param[in]   idx    rtc index
  \return      error code \ref csi_error_t
*/
csi_error_t csi_rtc_init(csi_rtc_t *rtc, uint32_t idx)
{
    CSI_PARAM_CHK(rtc, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    csp_rtc_t *ptRtcBase;

    if (0 == target_get(DEV_RTC_TAG, idx, &rtc->dev)) {
        ptRtcBase = (csp_rtc_t *)HANDLE_REG_BASE(rtc);
		
		csp_rtc_init(ptRtcBase, RTC_ISOSC);
		
		//to fix a known bug
		csi_irq_attach((uint32_t)rtc->dev.irq_num, &rtc_irq_handler, &rtc->dev);
        csi_irq_enable((uint32_t)rtc->dev.irq_num);	
		
		
    } else {
        ret = CSI_ERROR;
    }
    return ret;
}

/**
  \brief       De-initialize RTC Interface. stops operation and releases the software resources used by the interface
  \param[in]   rtc    rtc handle to operate
  \return      None
*/
void csi_rtc_uninit(csi_rtc_t *rtc)
{
    CSI_PARAM_CHK_NORETVAL(rtc);
    csp_rtc_t *ptRtcBase = (csp_rtc_t *)HANDLE_REG_BASE(rtc);
		
	csp_rtc_stop(ptRtcBase);
	
}

/**
  \brief       Set system date
  \param[in]   rtc        handle rtc handle to operate
  \param[in]   rtctime    pointer to rtc time
  \return      error code \ref csi_error_t
*/
csi_error_t csi_rtc_set_time(csi_rtc_t *rtc, const csi_rtc_time_t *rtctime)
{
    CSI_PARAM_CHK(rtc, CSI_ERROR);
    CSI_PARAM_CHK(rtctime, CSI_ERROR);
    csp_rtc_t *ptRtcBase = (csp_rtc_t *)HANDLE_REG_BASE(rtc);
    csi_error_t ret = CSI_OK;
		
	do {
		ret = (csi_error_t)clock_check_tm_ok((const struct tm *)rtctime);
        if (ret < CSI_OK) {
            break;
        }
		
		
		csp_rtc_stop(ptRtcBase);
		
		csp_rtc_set_date(ptRtcBase, rtctime->tm_year, rtctime->tm_mon, rtctime->tm_wday,rtctime->tm_mday);
		///TODO support 24FMT only for now
		ret = (csi_error_t) csp_rtc_set_time(ptRtcBase, RTC_AM, rtctime->tm_hour, rtctime->tm_min,rtctime->tm_sec);
		if (ret < CSI_OK) {
            break;
        }
	}while(0);
	
	csp_rtc_run(ptRtcBase);

	return ret;
}

/**
  \brief       Set system date but no wait
  \param[in]   rtc        rtc handle to operate
  \param[in]   rtctime    pointer to rtc time
  \return      error code \ref csi_error_t
*/
csi_error_t csi_rtc_set_time_no_wait(csi_rtc_t *rtc, const csi_rtc_time_t *rtctime)
{
	return (csi_rtc_set_time(rtc, rtctime));
}

/**
  \brief       Config RTC alarm ture timer
  \param[in]   rtc         handle rtc handle to operate
  \param[in]   rtctime     time(s) to wake up
  \param[in]   callback    callback function
  \param[in]   arg         callback's param
  \return      error code \ref csi_error_t
*/
csi_error_t csi_rtc_set_alarm(csi_rtc_t *rtc, const csi_rtc_time_t *rtctime, void *callback, void *arg)
{
    CSI_PARAM_CHK(rtc, CSI_ERROR);
    CSI_PARAM_CHK(rtctime, CSI_ERROR);
    csp_rtc_t *ptRtcBase = (csp_rtc_t *)HANDLE_REG_BASE(rtc);
    csi_error_t ret = (csi_error_t)clock_check_tm_ok((const struct tm *)rtctime);
	
	if (ret == CSI_OK) {
		rtc->callback = callback;
        rtc->arg = arg;
		csp_rtc_int_clr(ptRtcBase, RTC_INT_ALRB);
		csp_rtc_int_enable(ptRtcBase, RTC_INT_ALRB, ENABLE);
		/*csi_irq_attach((uint32_t)rtc->dev.irq_num, &rtc_irq_handler, &rtc->dev);
        csi_irq_enable((uint32_t)rtc->dev.irq_num);	*/	
		csp_rtc_alm_enable(ptRtcBase, RTC_ALMB, DISABLE);
		csp_rtc_alm_set(ptRtcBase, RTC_ALMB, rtctime->tm_mday, RTC_AM,  rtctime->tm_hour, rtctime->tm_min,rtctime->tm_sec);
		csp_rtc_alm_set_msk(ptRtcBase, RTC_ALMB, 1, 0, 0, 0);
		csp_rtc_alm_enable(ptRtcBase, RTC_ALMB, ENABLE);
	}
	return ret;
}

/**
  \brief       Config RTC alarm ture timer
  \param[in]   rtc      handle rtc handle to operate
  \param[in]   eOut     rtc output
  \return      error code \ref csi_error_t
*/
csi_error_t csi_rtc_set_alarm_out(csi_rtc_t *rtc, csi_rtc_osel_e eOut)
{
	csp_rtc_t *ptRtcBase = (csp_rtc_t *)HANDLE_REG_BASE(rtc);
	csp_rtc_set_osel(ptRtcBase, eOut);
	return CSI_OK;
}

/**
  \brief       Get system date
  \param[in]   rtc        handle rtc handle to operate
  \param[out]  rtctime    pointer to rtc time
  \return      error code \ref csi_error_t
*/
csi_error_t csi_rtc_get_time(csi_rtc_t *rtc, csi_rtc_time_t *rtctime)
{
	CSI_PARAM_CHK(rtc, CSI_ERROR);
    CSI_PARAM_CHK(rtctime, CSI_ERROR);
    csp_rtc_t *ptRtcBase = (csp_rtc_t *)HANDLE_REG_BASE(rtc);
	
	rtctime->tm_year = csp_rtc_read_year(ptRtcBase);
	rtctime->tm_mon = csp_rtc_read_mon(ptRtcBase);
	rtctime->tm_wday = csp_rtc_read_wday(ptRtcBase);
	rtctime->tm_mday = csp_rtc_read_mday(ptRtcBase);
	rtctime->tm_hour = csp_rtc_read_hour(ptRtcBase);
	rtctime->tm_min = csp_rtc_read_min(ptRtcBase);
	rtctime->tm_sec = csp_rtc_read_sec(ptRtcBase);
	
	return CSI_OK;
}


/**
  \brief       Judge rtc is working
  \param[in]   rtc    handle rtc handle to operate
  \return      state of work
               ture - rtc is running
               false -rtc is not running
*/
bool csi_rtc_is_running(csi_rtc_t *rtc)
{
    CSI_PARAM_CHK(rtc, false);
    csp_rtc_t *ptRtcBase = (csp_rtc_t *)HANDLE_REG_BASE(rtc);

    return (csp_rtc_is_running(ptRtcBase) ? true : false);
}

/**
  \brief       Get alarm remaining time
  \param[in]   rtc    rtc handle to operate
  \return      the remaining time(s)
*/
uint32_t csi_rtc_get_alarm_remaining_time(csi_rtc_t *rtc)
{
    volatile csi_rtc_time_t wCurrentTime ;
	volatile csi_rtc_time_t wAlmTime;
	csi_rtc_time_t wGap;
	//uint32_t wTime;
	uint32_t ret = CSI_OK;
	CSI_PARAM_CHK(rtc, 0U);
    csp_rtc_t *ptRtcBase = (csp_rtc_t *)HANDLE_REG_BASE(rtc);
	

	wCurrentTime.tm_mday = csp_rtc_read_mday(ptRtcBase); 
	wCurrentTime.tm_hour = csp_rtc_read_hour(ptRtcBase);
	wCurrentTime.tm_min = csp_rtc_read_min(ptRtcBase); 
	wCurrentTime.tm_sec = csp_rtc_read_sec(ptRtcBase); 
	
	
	wAlmTime.tm_mday = csp_rtc_alm_read_day(ptRtcBase, RTC_ALMB); 
	wAlmTime.tm_hour = csp_rtc_alm_read_hour(ptRtcBase, RTC_ALMB);
	wAlmTime.tm_min = csp_rtc_alm_read_min(ptRtcBase, RTC_ALMB); 
	wAlmTime.tm_sec = csp_rtc_alm_read_sec(ptRtcBase, RTC_ALMB); 
	
	///if only return seconds , can be done easier
	

		if (wAlmTime.tm_sec >= wCurrentTime.tm_sec)
			wGap.tm_sec = wAlmTime.tm_sec - wCurrentTime.tm_sec;
		else {
			wGap.tm_sec = wAlmTime.tm_sec + 60 - wCurrentTime.tm_sec;
			wAlmTime.tm_min --;
		}
		
		if (wAlmTime.tm_min >= wCurrentTime.tm_min)
			wGap.tm_min = wAlmTime.tm_min - wCurrentTime.tm_min;
		else {
			wGap.tm_min = wAlmTime.tm_min + 60 - wCurrentTime.tm_min;
			wAlmTime.tm_hour --;
		}
		
		if (wAlmTime.tm_hour >= wCurrentTime.tm_hour)
		{
			wGap.tm_hour = wAlmTime.tm_hour - wCurrentTime.tm_hour;
			if (!csp_rtc_alm_read_dmsk(ptRtcBase, RTC_ALMB))
			{
				if (wAlmTime.tm_hour >= wCurrentTime.tm_hour)
					wGap.tm_mday  = wAlmTime.tm_mday - wCurrentTime.tm_mday;
				else
					ret = CSI_ERROR;
			}
			else
				wGap.tm_mday = 0;
		}
		else {
			if (!csp_rtc_alm_read_dmsk(ptRtcBase, RTC_ALMB))
			{
				wGap.tm_hour = wAlmTime.tm_hour + 24 - wCurrentTime.tm_hour;
				wGap.tm_mday = wAlmTime.tm_mday - 1 - wCurrentTime.tm_mday;
			}
			else
				ret = CSI_ERROR;
		}

	if (ret != CSI_ERROR)
		ret =  wGap.tm_mday * 6206100 + wGap.tm_hour * 3600 + wGap.tm_min *60 + wGap.tm_sec;
	return ret;
}

/**
  \brief       Cancel the rtc alarm
  \param[in]   rtc    rtc handle to operate
  \return      error code csi_error_t
*/
csi_error_t csi_rtc_cancel_alarm(csi_rtc_t *rtc)
{
    CSI_PARAM_CHK(rtc, CSI_ERROR);
	
    csp_rtc_t *ptRtcBase = (csp_rtc_t *)HANDLE_REG_BASE(rtc);

    rtc->callback = NULL;
    rtc->arg = NULL;
	csp_rtc_alm_enable(ptRtcBase, RTC_ALMA, DISABLE);
    csp_rtc_alm_reset(ptRtcBase, RTC_ALMA);
	csp_rtc_int_enable(ptRtcBase, RTC_INT_ALRA, DISABLE);
	csp_rtc_int_clr(ptRtcBase, RTC_INT_ALRA);

    //wj_rtc_write_match_count(ptRtcBase, 0U);
    //csi_irq_disable((uint32_t)rtc->dev.irq_num);
    //csi_irq_detach((uint32_t)rtc->dev.irq_num);

    return CSI_OK;
}

/**
  \brief       Enable rtc power manage
  \param[in]   rtc    rtc handle to operate
  \return      error code
*/
csi_error_t csi_rtc_enable_pm(csi_rtc_t *rtc)
{	
	return CSI_UNSUPPORTED;
}

/**
  \brief       Disable rtc power manage
  \param[in]   rtc    rtc handle to operate
*/
void csi_rtc_disable_pm(csi_rtc_t *rtc)
{
	//CSI_UNSUPPORTED
}

/**
  \brief    use rtc as a timer
  \param	callback  callback function to be called when time = ePrd
  \param 	ePrd    time interval of the timer.
  \param[in]   rtc    rtc handle to operate
*/
void csi_rtc_start_as_timer(csi_rtc_t *rtc, void *callback, csi_rtc_timer_e ePrd)
{	
	csp_rtc_t *ptRtcBase = (csp_rtc_t *)HANDLE_REG_BASE(rtc);
	
	csp_rtc_set_cprd(ptRtcBase, ePrd);
	csp_rtc_int_enable(ptRtcBase, RTC_INT_CPRD , ENABLE);
	rtc->callback = callback;
	csp_rtc_run(ptRtcBase);
}

/** \brief evtrg source output config  
 * 
 *  \param[in] rtc: RTC handle to operate
 *  \param[in] rtc_trgsrc: rtc evtrg source(1~4) 
 *  \param[in] trg_prd: event count period 
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_rtc_set_evtrg(csi_rtc_t *rtc, uint8_t byEvtrg, csi_rtc_trgsrc_e eTrgSrc, uint8_t byTrgPrd)
{
	csp_rtc_t *ptRtcBase = (csp_rtc_t *)HANDLE_REG_BASE(rtc);
	csi_error_t ret = CSI_OK;
	
	if (csp_rtc_set_trgsrc(ptRtcBase, byEvtrg, eTrgSrc)<0)
		return CSI_ERROR;
	if (csp_rtc_set_trgprd(ptRtcBase, byEvtrg, byTrgPrd)<0)
		return CSI_ERROR;
	return ret;
}