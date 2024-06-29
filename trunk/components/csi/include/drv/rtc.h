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

#ifdef __cplusplus
extern "C" {
#endif  

/****** RTC time *****/
typedef struct {
    int tm_sec;             ///< Second.      [0-59]
    int tm_min;             ///< Minute.      [0-59]
    int tm_hour;            ///< Hour.        [0-23]
    int tm_mday;            ///< Day.         [1-31]
    int tm_mon;             ///< Month.       [0-11]
    int tm_year;            ///< Year-1900.   [70- ]      !NOTE:100=2000       
    int tm_wday;            ///< Day of week. [0-6 ]      !NOTE:Sunday = 0     
    int tm_yday;            ///< Days in year.[0-365]     !NOTE:January 1st = 0
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
  \param[in]   rtc    rtc handle to operate
  \param[in]   idx    rtc index
  \return      error code \ref csi_error_t
*/
csi_error_t csi_rtc_init(csi_rtc_t *rtc, uint32_t idx);

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
csi_error_t csi_rtc_set_time(csi_rtc_t *rtc, const csi_rtc_time_t *rtctime);

/**
  \brief       Set system date but no wait
  \param[in]   rtc        rtc handle to operate
  \param[in]   rtctime    pointer to rtc time
  \return      error code \ref csi_error_t
*/
csi_error_t csi_rtc_set_time_no_wait(csi_rtc_t *rtc, const csi_rtc_time_t *rtctime);

/**
  \brief       Get system date
  \param[in]   rtc        rtc handle to operate
  \param[out]  rtctime    pointer to rtc time
  \return      error code \ref csi_error_t
*/
csi_error_t csi_rtc_get_time(csi_rtc_t *rtc, csi_rtc_time_t *rtctime);

/**
  \brief       Get alarm remaining time
  \param[in]   rtc    rtc handle to operate
  \return      the remaining time(s)
*/
uint32_t csi_rtc_get_alarm_remaining_time(csi_rtc_t *rtc);

/**
  \brief       Config RTC alarm timer
  \param[in]   rtc         rtc handle to operate
  \param[in]   rtctime     time to wake up
  \param[in]   callback    callback function
  \param[in]   arg         callback's param
  \return      error code \ref csi_error_t
*/
csi_error_t csi_rtc_set_alarm(csi_rtc_t *rtc, const csi_rtc_time_t *rtctime, void *callback, void *arg);

/**
  \brief       Cancel the rtc alarm
  \param[in]   rtc    rtc handle to operate
  \return      error code \ref csi_error_t
*/
csi_error_t csi_rtc_cancel_alarm(csi_rtc_t *rtc);

/**
  \brief       Judge rtc is running
  \param[in]   rtc    rtc handle to operate
  \return      
               true  - rtc is running 
               false - rtc is not running 
*/
bool csi_rtc_is_running(csi_rtc_t *rtc);

/**
  \brief       Enable rtc power manage
  \param[in]   rtc    rtc handle to operate
  \return      error code
*/
csi_error_t csi_rtc_enable_pm(csi_rtc_t *rtc);

/**
  \brief       Disable rtc power manage
  \param[in]   rtc    rtc handle to operate
*/
void csi_rtc_disable_pm(csi_rtc_t *rtc);

/**
  \brief    use rtc as a timer
  \param	callback  callback function to be called when time = ePrd
  \param 	ePrd    time interval of the timer.
  \param[in]   rtc    rtc handle to operate
*/
void csi_rtc_start_as_timer(csi_rtc_t *rtc, void *callback, csi_rtc_timer_e ePrd);

/**
  \brief       Config RTC alarm ture timer
  \param[in]   rtc      handle rtc handle to operate
  \param[in]   eOut     rtc output
  \return      error code \ref csi_error_t
*/
csi_error_t csi_rtc_set_alarm_out(csi_rtc_t *rtc, csi_rtc_osel_e eOut);


#ifdef __cplusplus
}
#endif

#endif /* _DRV_RTC_H_ */
