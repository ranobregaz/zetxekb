/***********************************************************************//** 
 * \file  wdt.h
 * \brief  head file of window WDT and indepedent WDT 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-24 <td>V0.0 <td>WNN     <td>initial
 * <tr><td> 2021-5-13 <td>V0.0 <td>ZJY     <td>initial
 * </table>
 * *********************************************************************
*/

#ifndef _DRV_WDT_H_
#define _DRV_WDT_H_

#include "csp.h"

#ifdef __cplusplus
extern "C" {
#endif

//typedef struct csi_wdt csi_wdt_t;
//
//struct csi_wdt {
//    csi_dev_t       dev;
//    void            (*callback)(csi_wdt_t *wdt,  void *arg);
//    void            *arg;
//    void            *priv;
//};

/**
 * \enum     csi_iwdt_intv_e
 * \brief    IWDT interrupt timer(timer over) select
 */
typedef enum{
	IWDT_INT_1_8 	= 0,
	IWDT_INT_2_8,
	IWDT_INT_3_8, 
	IWDT_INT_4_8, 
	IWDT_INT_5_8, 
	IWDT_INT_6_8, 
	IWDT_INT_7_8
}csi_iwdt_intv_e;

/**
 * \enum     csi_iwdt_tv_e
 * \brief    IWDT time over, unit: ms
 */
typedef enum{
	IWDT_TV_128 	= 0,	//128ms
	IWDT_TV_256,			//256ms
	IWDT_TV_512,			//512ms
	IWDT_TV_1000,			//1000ms = 1s
	IWDT_TV_2000,			//2000ms = 2s 
	IWDT_TV_3100,			//3100ms = 3.1s 
	IWDT_TV_4100, 			//4100ms = 4.1s
	IWDT_TV_8200			//8200ms = 8.2s 
}csi_iwdt_tv_e;


/**
  \brief       Initialize WDT Interface. Initializes the resources needed for the WDT interface 
  \param[in]   ptIwdtBase	pointer of iwdt register structure
  \param[in]   eOverTime    time length of system reset
  \return      error code \ref csi_error_t
*/
csi_error_t csi_wdt_init(csp_iwdt_t *ptIwdtBase, csi_iwdt_tv_e eOverTime);

/**
  \brief       De-initialize WDT Interface. stops operation and releases the software resources used by the interface
  \param[in]   wdt    handle to operate
  \return      None
*/
//void csi_wdt_uninit(csi_wdt_t *wdt);

/**
  \brief       Set the WDT value
  \param[in]   wdt    handle to operate
  \param[in]   ms     the timeout value(ms)
  \return      error code \ref csi_error_t
*/
//csi_error_t csi_wdt_set_timeout(csi_wdt_t *wdt, uint32_t ms);

/**
  \brief       Start the WDT
  \param[in]   wdt    handle to operate
  \return      error code \ref csi_error_t
*/
//csi_error_t csi_wdt_start(csi_wdt_t *wdt);

/**
  \brief       Stop the WDT
  \param[in]   wdt    handle to operate
  \return      None
*/
//void csi_wdt_stop(csi_wdt_t *wdt);

/** 
  \brief 	   open(start) iwdt
  \param[in]   ptIwdtBase	pointer of iwdt register structure
  \return 	   error code \ref csi_error_t
 */ 
csi_error_t csi_wdt_open(csp_iwdt_t *ptIwdtBase);

/** 
  \brief 	   close(stop) iwdt
  \param[in]   ptIwdtBase	pointer of iwdt register structure
  \return 	   error code \ref csi_error_t
 */ 
csi_error_t csi_wdt_close(csp_iwdt_t *ptIwdtBase);

/**
  \brief       Feed the WDT
  \param[in]   ptIwdtBase	pointer of iwdt register structure
  \return      error code \ref csi_error_t
*/
csi_error_t csi_wdt_feed(csp_iwdt_t *ptIwdtBase);

/**
  \brief       Get the remaining time to timeout
  \param[in]   ptIwdtBase	pointer of iwdt register structure
  \return      the remaining time of wdt(ms)
*/
uint32_t csi_wdt_get_remaining_time(csp_iwdt_t *ptIwdtBase);
    
/**
  \brief       Check if wdt is running
  \param[in]   ptIwdtBase	pointer of iwdt register structure
  \return      true->running, false->stopped
*/
bool csi_wdt_is_running(csp_iwdt_t *ptIwdtBase);

/** 
  \brief 	   iwdt INT enable/disable
  \param[in]   ptIwdtBase	pointer of iwdt register structure
  \param[in]   eIntTv		iwdt interrupt timer length(timer over), 1/2/3/4/5/6/7_8
  \param[in]   bEnable		enable/disable INT
  \return 	   error code \ref csi_error_t
 */
csi_error_t csi_wdt_irq_enable(csp_iwdt_t *ptIwdtBase, csi_iwdt_intv_e eIntTv, bool bEnable);

/**
  \brief       Attach the callback handler to wdt
  \param[in]   wdt         operate handle
  \param[in]   callback    callback function
  \param[in]   arg         callback's param
  \return      error code \ref csi_error_t
*/
//csi_error_t csi_wdt_attach_callback(csi_wdt_t *wdt, void *callback, void *arg);

/**
  \brief       Detach the callback handler
  \param[in]   wdt    operate handle
  \return      None
*/
//void csi_wdt_detach_callback(csi_wdt_t *wdt);

/**
  \brief       Enable wdt power manage
  \param[in]   wdt    wdt handle to operate
  \return      error code \ref csi_error_t
*/
//csi_error_t csi_wdt_enable_pm(csi_wdt_t *wdt);

/**
  \brief       Disable wdt power manage
  \param[in]   wdt    wdt handle to operate
  \return      None
*/
//void csi_wdt_disable_pm(csi_wdt_t *wdt);

/**
  \brief      set wwdt window, ONLY valid for WWDT
  \param[in]   wdt    wdt handle to operate
  \return      error code \ref csi_error_t
*/
//csi_error_t csi_wdt_set_window_time(csi_wdt_t *wdt, uint32_t ms);

/**
  \brief      enable or disable WDT when stop in debug mode
  \param[in]   wdt    wdt handle to operate
  \param	   bEnable 
  \return      none
*/
csi_error_t csi_wdt_debug_enable(csp_iwdt_t *ptIwdtBase, bool bEnable);

/**
  \brief      set wwdt window, ONLY valid for WWDT
  \param[in]   wdt    wdt handle to operate
  \param	   eVal   iwdt intv
  \return      error code \ref csi_error_t
*/
//csi_error_t csi_wdt_set_int_time(csi_wdt_t *wdt, csi_iwdt_intv_e eVal);


#ifdef __cplusplus
}
#endif

#endif /* _DRV_WDT_H_ */
