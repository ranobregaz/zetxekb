/***********************************************************************//** 
 * \file  lpt.c
 * \brief  csi lpt driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2017-10-27<td>V0.0 <td>Alibaba <td> initial
 * <tr><td> 2021-1-8  <td>V0.1 <td>WNN     <td> SWTRG
 * <tr><td> 2021-5-14 <td>V0.1 <td>ZJY     <td> initial
 * </table>
 * *********************************************************************
*/
#ifndef _DRV_ETB_H_
#define _DRV_ETB_H_

#include "csp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum     csi_etb_trig_mode_e
 * \brief    ETB channel trigger mode
 */
typedef enum {
    ETB_HARDWARE_TRG  	= 0,		//etb channel inout is hardware trigger.
    ETB_SOFTWARE_TRG               	//etb channel inout is software trigger.
} csi_etb_trg_mode_e;

/**
 * \enum     csi_etb_ch_type_e
 * \brief    ETB channel trigger type
 */
typedef enum {
    ETB_ONE_TRG_ONE	= 0,      		//one device trig one deivce
    ETB_ONE_TRG_MORE,          		//one device trig two for more device
    ETB_MORE_TRG_ONE           		//two or more device trig one deivce
} csi_etb_ch_type_e;

/**
 * \enum     csi_etb_chid_e
 * \brief    ETB channel id(number)
 */
typedef enum {
    ETB_CH0_ID			= 0,		//etb channel 0 id number
	ETB_CH1_ID,						//etb channel 1 id number
	ETB_CH2_ID,						//etb channel 2 id number
	ETB_CH3_ID,						//etb channel 3 id number
	ETB_CH4_ID,
	ETB_CH5_ID,
	ETB_CH6_ID,
	ETB_CH7_ID
} csi_etb_chid_e;

/**
 * \enum     csi_etb_src_e
 * \brief    ETB event source
 */
typedef enum{
	ETB_LPT_TRGOUT0 	= 0,
	ETB_EXI_TRGOUT0 	= 4,
	ETB_EXI_TRGOUT1,
	ETB_EXI_TRGOUT2,	
	ETB_EXI_TRGOUT3,
	ETB_EXI_TRGOUT4,
	ETB_EXI_TRGOUT5,
	ETB_RTC_TRGOUT0,
	ETB_RTC_TRGOUT1,
	ETB_BT_TRGOUT0,
	ETB_BT_TRGOUT1,
	ETB_ETP0_TRGOUT0 	= 16,
	ETB_ETP0_TRGOUT1,
	ETB_ETP0_TRGOUT2,
	ETB_ETP0_TRGOUT3,
	ETB_GPT0_TRGOUT0 	= 32,
	ETB_GPT0_TRGOUT1,
	ETB_ADC_TRGOUT0	 	= 48,
	ETB_ADC_TRGOUT1,
	ETB_TOUCH_TRGOUT 	= 60,
	SRC_NOT_USE 		= 0xff
}csi_etb_src_e;

/**
 * \enum     csi_etb_dst_e
 * \brief    ETB description event 
 */
typedef enum{
	ETB_LPT_SYNCIN0 	= 0,
	ETB_BT0_SYNCIN0 	= 2,
	ETB_BT0_SYNCIN1,
	ETB_BT1_SYNCIN0,
	ETB_BT1_SYNCIN1,
	ETB_ADC_SYNCIN0,
	ETB_ADC_SYNCIN1,
	ETB_ADC_SYNCIN2,
	ETB_ADC_SYNCIN3,
	ETB_ADC_SYNCIN4,
	ETB_ADC_SYNCIN5,
	ETB_EPT0_SYNCIN0 	= 16,
	ETB_EPT0_SYNCIN1,
	ETB_EPT0_SYNCIN2,
	ETB_EPT0_SYNCIN3,
	ETB_EPT0_SYNCIN4,
	ETB_EPT0_SYNCIN5,
	ETB_GPT0_SYNCIN0 	= 36,
	ETB_GPT0_SYNCIN1,
	ETB_GPT0_SYNCIN2,
	ETB_GPT0_SYNCIN3,
	ETB_GPT0_SYNCIN4,
	ETB_GPT0_SYNCIN5,
	ETB_TOUCH_SYNCIN 	= 60,
	DST_NOT_USE 		= 0xff
}csi_etb_dst_e;

typedef struct {
    uint8_t               bySrcIp;		//a specific number represent a location in an source trigger location map to trigger other ip(s).
	uint8_t               bySrcIp1; 
	uint8_t               bySrcIp2; 
    uint8_t               byDstIp;     	//a specific number represent an location in an dest trigger map to wait signal(s) from source ip(s) or location(s).
	uint8_t               byDstIp1;
	uint8_t               byDstIp2;  
    csi_etb_trg_mode_e    eTrgMode;		//the input source is hardware trigger or software trigger.
    csi_etb_ch_type_e     eChType;    	//channel type
} csi_etb_config_t;

/**
  \brief       Init the etb device
  \return      error code
*/
csi_error_t csi_etb_init(void);

/**
  \brief       Uninit the etb device
  \return      none
*/
//void csi_etb_uninit(void);

/**
  \brief       alloc an etb channel
  \param[in]   eChType		etb channel work mode
  \return      channel id or CSI_ERROR
*/
int32_t csi_etb_ch_alloc(csi_etb_ch_type_e eChType);

/**
  \brief       free an etb channel
  \param[in]   eChId		etb channel work mode
  \return      none
*/
void csi_etb_ch_free(csi_etb_chid_e eChId);

/**
  \brief       config etb channel
  \param[in]   eChId		etb channel id
  \param[in]   ptConfig     the config structure for etb channel
  \return      csi error code
*/
csi_error_t csi_etb_ch_config(csi_etb_chid_e eChId, csi_etb_config_t *ptConfig);


/** 
  \brief 	   etb channel sw force triger
  \param[in]   eChId		etb channel id
  \return 	   none
*/
void csi_etb_ch_swtrig(csi_etb_chid_e eChId);

/**
  \brief       open(start) an etb channel
  \param[in]   eChId      	etb channel id
  \return      none
*/
void csi_etb_ch_start(csi_etb_chid_e eChId);

/**
  \brief       close(stop) an etb channel
  \param[in]   etb        etb channel id
  \return      none
*/
void csi_etb_ch_stop(csi_etb_chid_e eChId);

#endif /* _CSI_ETB_H_ */
