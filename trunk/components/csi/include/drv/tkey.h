/***********************************************************************//** 
 * \file  sio.c
 * \brief  csi sio driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-03 <td>V0.0  <td>XB   <td>initial
 * <tr><td> 2021-1-03 <td>V0.0  <td>ZJY   <td>modify
 * </table>
 * *********************************************************************
*/

#ifndef _DRV_TKEY_H_
#define _DRV_TKEY_H_

#include <stdint.h>
#include <stdbool.h>
#include <drv/common.h>
#include "csp.h"

#ifdef __cplusplus
extern "C" {
#endif


uint32_t	wTkeyIoEnable;					 		
uint16_t	hwTkeySenprd[17];
uint8_t		byTkeyScanTime[17];
uint16_t	hwTkeyTriggerLevel[17];	
uint8_t		byPressDebounce;		
uint8_t		byReleaseDebounce;	
uint16_t	hwTkeyIcon[17];
uint8_t		byMultiTimesFilter;			
uint8_t		byValidKeyNum;				
uint8_t 	byKeyMode;					
uint8_t 	byBaseSpeed;				
uint32_t 	wTkeyLongPressTime;			
uint16_t 	hwTkeyPselMode;		
uint16_t 	hwTkeyFVRLevel;		
uint16_t 	hwTkeyECLevel;
uint8_t 	byTkeyLowpowerMode;
uint8_t 	byTkeypowerLevel;	
uint8_t 	byTkeyWakeupLevel;	





extern volatile uint16_t hwOffseData0Abs[17];
extern volatile uint16_t hwOffseData1Abs[17];
extern volatile uint16_t hwOffseData2Abs[17];
extern volatile uint16_t hwSamplingData0[17];
extern volatile uint16_t hwBaselineData0[17];
extern volatile int16_t nOffsetData0[17];
extern volatile uint16_t hwSamplingData1[17];
extern volatile uint16_t hwBaselineData1[17];
extern volatile int16_t nOffsetData1[17];
extern volatile uint16_t hwSamplingData2[17];
extern volatile uint16_t hwBaselineData2[17];
extern volatile int16_t nOffsetData2[17];
extern volatile uint32_t wKeyMap;
extern volatile uint8_t byBaseUpdata;


extern void csi_tkey_timer_handler(void);
extern void csi_tkey_handler(void);
extern void csi_tkey_setup_sleep(void);
extern void csi_tkey_quit_sleep(void);
extern void csi_tkey_init(void);
extern void csi_tkey_reinit(void);
extern void csi_tkey_prgm(void);
extern void csi_tkey_baseline_prog(void);
extern void csi_tkey_parameter_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_tkey_H_ */