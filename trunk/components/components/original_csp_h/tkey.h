/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     tkey.h
 * @brief    header file for tkey driver
 * @vertkeyn  V1.0
 * @date     08. Apr 2020
 * @model    tkey
 ******************************************************************************/

#ifndef _DRV_TKEY_H_
#define _DRV_TKEY_H_

#include <stdint.h>
#include <stdbool.h>
#include <drv/common.h>
#include <drv/dma.h>
#include "csp_tkey.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct csi_tkey csi_tkey_t;
struct csi_tkey {
    csi_dev_t          dev;
    void               (*callback)(csi_tkey_t *tkey, void *arg);
    void               *arg;
  
};

/**
  \brief       Init tkey ctrl block
               Initializes the resources needed for the tkey instance
  \param[in]   tkey    Handle of tkey instance
  \param[in]   idx    Index of instance
  \return      error code \ref csi_error_t
*/
csi_error_t csi_tkey_init(csi_tkey_t *tkey, uint32_t idx);

/**
  \brief       Uninit tkey ctrl block
               Stops operation and releases the software resources used by the instance
  \param[in]   tkey    Handle of tkey instance
  \return      None
*/
void csi_tkey_uninit(csi_tkey_t *tkey);

/**
  \brief       Attach callback to the tkey
  \param[in]   tkey    tkey handle to operate
  \param[in]   cb     Event callback function \ref csi_tkey_callback_t
  \param[in]   arg    User private param for event callback
  \return      error code \ref csi_error_t
*/
csi_error_t csi_tkey_attach_callback(csi_tkey_t *tkey, void *callback, void *arg);

/**
  \brief       Detach callback from the tkey
  \param[in]   tkey    tkey handle to operate
  \return      None
*/
void csi_tkey_detach_callback(csi_tkey_t *tkey);

/** \brief tkey interrupt enable/disable
 * 
 *  \param[in] tkey: tkey handle to operate
 *  \param[in] eNewState: ENABLE, DISABLE
 *  \param[in] dwTkeyIMSCRX: interrupt number
 *  \return none
 */
void csi_tkey_set_intr(csi_tkey_t *tkey,bool bNewState,uint32_t wTkeyIMSCRX);

/****************************************************
//TK test main
*****************************************************/
/** \brief tkey test main
 * 
 *  \param[in] tkey: tkey handle to operate
 *  \return none
 */
void csi_tkey_prgm(csi_tkey_t *tkey);

/** \brief TKey sampling program
 *
 *  \param[in] tkey: tkey handle to operate
 *  \return none
 */
void csi_tkey_baseline_prog(csi_tkey_t *tkey);

/** \brief parameter
 *
 *  \param[in] none
 *  \return none
 */
void csi_tkey_parameter(void);

#ifdef __cplusplus
}
#endif

#endif /* _DRV_TKEY_H_ */
