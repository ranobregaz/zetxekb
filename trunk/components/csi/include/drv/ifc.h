/***********************************************************************//** 
 * \file  ifc.h
 * \brief  head file of ifc.c
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-18 <td>V0.0  <td>WNN   <td>initial for csi
 * </table>
 * *********************************************************************
*/
#ifndef _DRV_EFLASH_H_
#define _DRV_EFLASH_H_

#include "common.h"
#include "csp_ifc.h"

/**
 \brief  Data Flash information
*/
typedef struct {
    uint32_t          flash_size;         ///< Chip End address (start+size-1)
    uint32_t          sector_size;        ///< Uniform sector size in bytes
    uint32_t          erased_value;       ///< erased value
} csi_ifc_info_t ;

/**
\brief Flash Status
*/
typedef struct {
    uint8_t busy  : 1;                   ///< Flash busy flag
    uint8_t error : 1;                   ///< Read/Program/Erase error flag (cleared on start of next operation)
} csi_ifc_status_t;



// Function documentation


/**
  \brief       config data flash workmode
  \param[in]   ptEflash    ptEflash handle to operate
  \param[in]   bEnable	   	0: normal mode
							1: para mode
*/
void csi_ifc_dflash_paramode_enable(csp_ifc_t *ptIfcBase, bool bEnable);



/**
  \brief       Read data from Flash.
  \param[in]   ptEflash  ptEflash handle to operate.
  \param[in]   wAddr  Data address（(SHOULD BE WORD ALLIGNED)）
  \param[out]  data  Pointer to a buffer storing the data read from Flash.
  \param[in]   wDataNum   Number of data（bytes）to read.
  \return      error code
*/
csi_error_t csi_ifc_read(csp_ifc_t *ptIfcBase, uint32_t wAddr, uint32_t *data, uint32_t wDataNum);

/**
  \brief       Program data to Flash.
  \param[in]   ptEflash  ifc handle to operate.
  \param[in]   wAddr  Data address (SHOULD BE WORD ALLIGNED)
  \param[in]   data  Pointer to a buffer containing the data to be programmed to Flash.
  \param[in]   wDataNum   Number of data(BYTES) items to program.
  \return      error code
*/
csi_error_t csi_ifc_program(csp_ifc_t *ptIfcBase, uint32_t wAddr, uint32_t *pwData, uint32_t wDataNum);



/**
  \brief       Get Data Flash information.
  \param[in]   ptEflash  ifc handle to operate.
*/
void csi_ifc_info(csp_ifc_t *ptIfcBase,csi_ifc_info_t *ifc_info);




/**	\brief write useroption
 *  \param[in]   ptEflash  ifc handle to operate.
 *  31 ~ 16       | 15 ~ 4 		| 3 ~ 0
 *  IWDT     		RSVD	  	EXIRST
 * Dis	: 0x55AA 				PA0.2 EXIRSTPIN: 0x5
 * En	: other					PA0.2 IO       : other    
 * \return      error code: return CSI_ERROR when checksum is not correct
 */  
csi_error_t csi_ifc_wr_useroption(csp_ifc_t *ptIfcBase, uint32_t wData);

/**	\brief SWD IO remap. ONLY  valid after reset
 *  \param[in]   ptEflash  ifc handle to operate.
 *  \param[in] bGrp
 * 				SWD       	pin number
 * bGrp = 2    PA0.6(CLK)   	2
 * 				PA0.7(DIO)		3
 * bGrp = 1	   PA0.0(DIO)		19
 * 				PA0.1(CLK)		20
 * bGrp = 0    PA0.5(CLK)		10
 * 				PA0.12(DIO)		11
 * \return      error code: return CSI_ERROR when checksum is not correct
 */
csi_error_t csi_ifc_swd_remap(csp_ifc_t *ptIfcBase, swd_grp_e eGrp);


/** \brief get flash status
 *  \param ptEflash ifc handle to operate.
 *  \return ifc_status_t
 */
csi_ifc_status_t csi_ifc_get_status(csp_ifc_t *ptIfcBase);

extern bool g_bFlashCheckPass;
extern bool g_bFlashPgmDne;

#endif /* _CSI_EFLASH_H_ */