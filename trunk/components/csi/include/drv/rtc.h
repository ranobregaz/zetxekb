/***********************************************************************//** 
 * \file  rtc.h
 * \brief  head file for RTC
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-8 <td>V0.0  <td>WNN   <td>initial
 * <tr><td> 2021-1-8 <td>V0.0  <td>WNN   <td>modify
 * </table>
 * *********************************************************************
*/

#ifndef _DRV_RTC_H_
#define _DRV_RTC_H_

#include <drv/common.h>
#include "csp_rtc.h"

#ifdef __cplusplus
extern "C" {
#endif  

typedef struct  {
	uint8_t		byClkSrc;	/// clock source
	uint8_t		byFmt;		//  timer format
} csi_rtc_config_t;

typedef struct {
	uint8_t		byAlmMode;	///mode = 0:  day       hour min sec 
							///mode = 1:  weekday   hour min sec
							///mode = 2:            hour min sec
	uint8_t		byAlmSt;	// 0: not yet
							// 1: time up
}csi_rtc_alm_t;

/****** RTC time *****/
typedef struct {
    int tm_sec;             ///< Second.      [0-59]
    int tm_min;             ///< Minute.      [0-59]
    int tm_hour;            ///< Hour.        [0-23]
    int tm_mday;            ///< Day.         [1-31]
    int tm_mon;             ///< Month.       [1-12]
    int tm_year;            ///< Year-1900.   [70- ]      !NOTE:100=2000    
	int tm_wday;			/// weekday		  [1-7]	     
    int tm_yday;            ///< Days in year.[0-365]     !NOTE:January 1st = 0
	int tm_isdst;			/// Non-0 if daylight savings time is in effect
	int tm_pm;				///< PM.		  [0/1]
} csi_rtc_time_t;

/****** definition for rtc *****/
typedef struct csi_rtc csi_rtc_t;
struct csi_rtc {
    csi_dev_t           dev;
    void               (*callback)(csi_rtc_t *rtc, void *arg);
    void               *arg;
    void               *priv;
} ;

typedef enum{
	RTC_CLKSRC_ISOSC = 0,
	RTC_CLKSRC_IMOSC_DIV4,
	RTC_CLKSRC_EMOSC,
	RTC_CLKSRC_EMOSC_DIV4
}csi_rtc_clksrc_e;

typedef enum {
	RTC_TIMER_DIS = 0,
	RTC_TIMER_0_5S,
	RTC_TIMER_1S,
	RTC_TIMER_1MIN,
	RTC_TIMER_1H,
	RTC_TIMER_1DAY,
	RTC_TIMER_1MON	
}csi_rtc_timer_e;

typedef enum {
	RTC_ALMA_P = 0,
	RTC_ALMA_H,
	RTC_ALMA_L,
	RTC_ALMB_P,
	RTC_ALMB_H,
	RTC_ALMB_L,
	
}csi_rtc_osel_e;

typedef enum{
	RTC_TRGOUT_DIS = 0,
	RTC_TRGOUT_ALRMA,
	RTC_TRGOUT_ALRMB,
	RTC_TRGOUT_ALRMA_ALRMB,
	RTC_TRGOUT_CPRD
}csi_rtc_trgsrc_e;

typedef enum{
	RTCINT_ALMB = 0,
	RTCINT_CPRD = 2,
	RTCINT_TRGEV0,
	RTCINT_TRGEV1
}csi_rtc_int_e;

/**
  \brief       Initialize RTC Interface. Initializes the resources needed for the RTC interface
  \param       ptRtc    rtc handle
  \param	   eOsc		clock source 
  \param       eFmt     rtc format: RTC_24FMT/RTC_12FMT
  \return      none
*/
void csi_rtc_init(csp_rtc_t *ptRtc, csi_rtc_config_t *tConfig);

/**
  \brief       De-initialize RTC Interface. stops operation and releases the software resources used by the interface
  \param[in]   rtc    rtc handle to operate
  \return      None
*/
void csi_rtc_uninit(csi_rtc_t *rtc);

/**
  \brief       Set system date and wait for synchro
  \param[in]   rtc        rtc handle to operate
  \param[in]   rtctime    pointer to rtc time
  \return      error code \ref csi_error_t
*/
csi_error_t csi_rtc_set_time(csp_rtc_t *rtc, csi_rtc_time_t *rtctime);

/**
  \brief       To set/change RTC format
  \param       rtc handle rtc handle to operate
  \param       eFmt \ref rtc_fmt_e    
  \return      none
*/

/**
  \brief       To start RTC 
  \param       rtc handle rtc handle to operate
  \return      none
*/
void csi_rtc_start(csp_rtc_t *ptRtc);

/**
  \brief       To stop RTC 
  \param       rtc handle rtc handle to operate
  \return      none
*/
void csi_rtc_stop(csp_rtc_t *ptRtc);


/**
  \brief       To set/change RTC format
  \param       rtc handle rtc handle to operate
  \param       eFmt \ref rtc_fmt_e    
  \return      none
*/
void csi_rtc_change_fmt(csp_rtc_t *ptRtc,  rtc_fmt_e eFmt);

/**
  \brief   RTC interrupt enable/disable
  \param   ptRtc      handle rtc handle to operate
  \param   eIntSrc	  interrupt source	
  \param   bEnable    ENABLE/DISABLE 
  \return  none
*/
void csi_rtc_int_enable(csp_rtc_t *ptRtc, rtc_int_e eIntSrc, bool bEnable);

/**
  \brief       Get system date
  \param[in]   rtc        rtc handle to operate
  \param[out]  rtctime    pointer to rtc time
  \return      none
*/
void csi_rtc_get_time(csp_rtc_t *rtc, csi_rtc_time_t *rtctime);

/**
  \brief       Get alarm remaining time
  \param[in]   rtc    rtc handle to operate
  \param	   byAlm  RTC_ALMA/RTC_ALMB
  \return      the remaining time(s)
*/
uint32_t csi_rtc_get_alarm_remaining_time(csp_rtc_t *rtc, uint8_t byAlm);

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
csi_error_t csi_rtc_set_alarm(csp_rtc_t *ptRtc, uint8_t byAlm, uint8_t byMode, csi_rtc_time_t *tpRtcTime);

/**
  \brief       Cancel the rtc alarm
  \param       ptRtc    rtc handle to operate
  \param       byAlm	RTC_ALMA/RTC_ALMB
  \return      none
*/
void csi_rtc_cancel_alarm(csp_rtc_t *ptRtc, uint8_t byAlm);

/**
  \brief       Judge rtc is running
  \param[in]   rtc    rtc handle to operate
  \return      
               true  - rtc is running 
               false - rtc is not running 
*/
bool csi_rtc_is_running(csi_rtc_t *rtc);


/**
  \brief    use rtc as a timer
  \param	callback  callback function to be called when time = ePrd
  \param 	ePrd    time interval of the timer.
  \param[in]   rtc    rtc handle to operate
*/
void csi_rtc_start_as_timer(csp_rtc_t *ptRtc, csi_rtc_timer_e ePrd);

/**
  \brief       Config RTC alarm ture timer
  \param[in]   ptRtc      handle rtc handle to operate
  \param[in]   eOut     rtc output
  \return      none
*/
void csi_rtc_set_alarm_out(csp_rtc_t *ptRtc, csi_rtc_osel_e eOut);

/** \brief evtrg source output config  
 * 
 *  \param[in] ptRtc: RTC handle to operate
 *  \param[in] byEvtrg: rtc evtrg channel(1~4) 
 *  \param[in] eTrgSrc: rtc evtrg source
 *  \param[in] trg_prd: event count period 
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_rtc_set_evtrg(csp_rtc_t *ptRtc, uint8_t byEvtrg, csi_rtc_trgsrc_e eTrgSrc, uint8_t byTrgPrd);


#ifdef __cplusplus
}
#endif

#endif /* _DRV_RTC_H_ */
