/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     drv/pm.h
 * @brief    header File for pm manage
 * @version  V1.0
 * @date     16. Mar 2020
 ******************************************************************************/

#ifndef _PM_H_
#define _PM_H_

#include <stdint.h>
#include <drv/common.h>
#include <soc.h>
#include <csi_core.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_USER_PM
typedef struct{
	void (*prepare_to_sleep)(void );
	void (*wkup_frm_sleep)(void );
	void (*prepare_to_deepsleep)(void );
	void (*wkup_frm_deepsleep)(void );
}pm_core_t;
#endif

typedef enum {

    WKUP_EXI0 = 0,
	WKUP_EXI1,
	WKUP_EXI2,
	WKUP_EXI3,
	WKUP_EXI4,
	WKUP_IWDT = 8,
	WKUP_RTC,
	WKUP_LPT,
	WKUP_LVD,
	WKUP_TCH,
	WKUP_I2C
	
} wakeupn_type_e;



#ifdef __cplusplus
}
#endif

#endif /* _CSI_PM_H_ */
