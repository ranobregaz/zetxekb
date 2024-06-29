/***********************************************************************//** 
 * \file  eflash.c
 * \brief  onchip flash(data flash and program flash) operations
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-18 <td>V0.0  <td>WNN   <td>initial for csi
 * <tr><td> 2021-1-6 <td>V0.1  <td>WNN   <td>support PFLASH PGM
 * </table>
 * *********************************************************************
*/
#include "drv/eflash.h"
#include "drv/common.h"
#include "csp_ifc.h"
#include "soc.h"


/**
  \brief       Initialize ptEflash Interface. Initialize the resources needed for the ptEflash interface
  \param[in]   ptEflash    ptEflash handle to operate
  \param[in]   idx    ptEflash index
  \return      csi_error_t
*/
csi_error_t csi_eflash_init(csi_eflash_t *ptEflash, uint32_t idx, void *arg)
{
	CSI_PARAM_CHK(ptEflash, CSI_ERROR);
    csi_error_t tRet = CSI_OK;
    csp_ifc_t *ptEflashBase;
	
	if (target_get(DEV_IFC_TAG, idx, &ptEflash->dev) != CSI_OK) {
        tRet = CSI_ERROR;
    }else{
        ptEflashBase = (csp_ifc_t *)HANDLE_REG_BASE(ptEflash);
		csi_clk_enable(&(ptEflash->dev));
		//csp_ifc_reset(ptEflashBase);
		csp_ifc_clk_enable(ptEflashBase, ENABLE);
		g_bFlashPgmDne = 1;
		
	}
	
	return tRet;
}

/**
  \brief       config data flash workmode
  \param[in]   ptEflash    ptEflash handle to operate
  \param[in]   bEnable	   	0: normal mode
							1: para mode
*/
void csi_eflash_dflash_paramode_enable(csi_eflash_t *ptEflash, bool bEnable)
{
	csp_ifc_t *ptEflashBase = (csp_ifc_t *)HANDLE_REG_BASE(ptEflash);
	csp_ifc_dflash_paramode_enable(ptEflashBase, bEnable);
}

/**
  \brief       De-initialize ptEflash Interface. stops operation and releases the software resources used by the interface
  \param[in]   handle    ptEflash handle to operate
  \return      None 
*/
void csi_eflash_uninit(csi_eflash_t *ptEflash)
{
	CSI_PARAM_CHK_NORETVAL(ptEflash);
    //csp_ifc_t *ptEflashBase;
	
	//ptEflashBase = (csp_ifc_t *)HANDLE_REG_BASE(ptEflash);
	
	//csp_ifc_reset(ptEflashBase);
	csi_clk_disable(&(ptEflash->dev));
	
}

/**
  \brief       Read data from Flash.
  \param[in]   ptEflash  ptEflash handle to operate.
  \param[in]   offset  Data address（(SHOULD BE WORD ALLIGNED)）
  \param[out]  data  Pointer to a buffer storing the data read from Flash.
  \param[in]   wDataNum   Number of data（bytes）to read.
  \return      error code
*/
csi_error_t csi_eflash_read(csi_eflash_t *ptEflash, uint32_t wOffset, void *data, uint32_t wDataNum)
{
	CSI_PARAM_CHK_NORETVAL(ptEflash);
    csp_ifc_t *ptEflashBase;
	uint32_t i, wAddr, *wDataBuff = (uint32_t *)data;
	
	ptEflashBase = (csp_ifc_t *)HANDLE_REG_BASE(ptEflash);
	
	wAddr = DFLASHBASE+ wOffset;
	
	if (wDataNum%4 == 0)
		wDataNum = wDataNum/4;
	else
		wDataNum = wDataNum/4 + 1;
		
		
	wDataBuff = (uint32_t *)data;
	i = sizeof(data);
	for(i=0; i<wDataNum ; i++)
	{

		*wDataBuff = csp_ifc_rd_1word(ptEflashBase, wAddr);
		wDataBuff ++;
		wAddr += 4;
	}
	return CSI_OK;
}


csi_error_t csi_eflash_wr_1word(csi_eflash_t * ptEflash, uint32_t wAddr, uint32_t wData)
{
	csi_error_t tRet;
	
	if (wAddr % 4 != 0)
	{
		tRet = CSI_ERROR;
		return tRet;
	}
	

    csp_ifc_t *ptEflashBase;
    ptEflashBase = (csp_ifc_t *)HANDLE_REG_BASE(ptEflash);
		
	if (wAddr >= 0x10000000)
		tRet = (csi_error_t)csp_ifc_wr_1word(ptEflashBase, DFLASH, wAddr, wData);
	else
		tRet = (csi_error_t)csp_ifc_wr_1word(ptEflashBase, PFLASH, wAddr, wData);
	
	return tRet;
}

/**
  \brief       Program data to Flash.
  \param[in]   ptEflash  eflash handle to operate.
  \param[in]   offset  Data address (SHOULD BE WORD ALLIGNED)
  \param[in]   data  Pointer to a buffer containing the data to be programmed to Flash.
  \param[in]   wDataNum   Number of data(BYTES) items to program.
  \return      error code
*/
csi_error_t csi_eflash_program(csi_eflash_t * ptEflash, uint32_t wOffset, void *pwData, uint32_t wDataNum)
{
	csi_error_t tRet = CSI_OK;
	uint32_t *wData = (uint32_t *)pwData;
	uint32_t i, wAdd, wFullPageNum, wLen0,wLen1;
	csp_ifc_t *ptEflashBase;
	if (wOffset % 4 != 0)
	{
		tRet = CSI_ERROR;
		return tRet;
	}
	
	wAdd = DFLASHBASE + wOffset;
//	wOffset = wOffset&0xfffffffc;
	
	if (wDataNum%4 == 0)
		wDataNum = wDataNum/4;
	else
		wDataNum = wDataNum/4 + 1;
	
	if (wAdd > DFLASHLIMIT) {
		return CSI_ERROR;
	}
	if (wDataNum > (DFLASH_PAGE_SZ-(wOffset>>2))){
			wLen0 = DFLASH_PAGE_SZ-(wOffset>>2);
			wFullPageNum = (wDataNum - wLen0)/DFLASH_PAGE_SZ;
			if (wDataNum > ((wFullPageNum*DFLASH_PAGE_SZ) + wLen0)) {
				wLen1 = wDataNum - ((wFullPageNum*DFLASH_PAGE_SZ) + wLen0);
			}
			else
				wLen1 = 0;

	}
	else {
			wLen0 = wDataNum;
			wFullPageNum = 0;
			wLen1 = 0;
	}
	
    ptEflashBase = (csp_ifc_t *)HANDLE_REG_BASE(ptEflash);
	
	//first page PGM if has
	tRet = (csi_error_t)csp_ifc_wr_nword(ptEflashBase, DFLASH, wAdd, wLen0, wData);	
	while (csi_eflash_get_status(ptEflash).busy);
	if (tRet == CSI_ERROR)
	return tRet;
	
	//middle pages PGM	
	for (i=0; i<wFullPageNum; i++)
	{
		tRet = (csi_error_t)csp_ifc_wr_nword(ptEflashBase, DFLASH, wAdd+ (wLen0<<2)+(DFLASH_PAGE_SZ<<2)*i, DFLASH_PAGE_SZ, & *wData+(wLen0+DFLASH_PAGE_SZ*i));
		while (csi_eflash_get_status(ptEflash).busy);	
		if (tRet == CSI_ERROR)
			return tRet;
	}
	
	//last page PGM if has
	if (wLen1){
		tRet = (csi_error_t)csp_ifc_wr_nword(ptEflashBase, DFLASH, wAdd+(wLen0<<2) + wFullPageNum*(DFLASH_PAGE_SZ<<2), wLen1, & *wData+(wLen0+DFLASH_PAGE_SZ*wFullPageNum));
		while (csi_eflash_get_status(ptEflash).busy);	
		if (tRet == CSI_ERROR)
			return tRet;
	}
	
	
	return tRet;
	
}
/**	\brief SWD IO remap. ONLY  valid after reset
 *  \param[in] bGrp
 * 				SWD       	pin number
 * bGrp = 2    PA0.6(CLK)   	2
 * 				PA0.7(DIO)		3
 * bGrp = 1	   PA0.0(DIO)		19
 * 				PA0.1(CLK)		20
 * bGrp = 0    PA0.5(CLK)		10
 * 				PA0.12(DIO)		11
 */
csi_error_t csi_eflash_swd_remap(csi_eflash_t *ptEflash, swd_grp_e eGrp)
{
	csi_error_t tRet = CSI_OK;

    csp_ifc_t *ptEflashBase;
    ptEflashBase = (csp_ifc_t *)HANDLE_REG_BASE(ptEflash);
		
	tRet = csp_ifc_swd_remap(ptEflashBase, eGrp);
	return tRet;
}

/**	\brief write useroption
 *  31 ~ 16       | 15 ~ 4 		| 3 ~ 0
 *  IWDT     		RSVD	  	EXIRST
 * Dis	: 0x55AA 				PA0.2 EXIRSTPIN: 0x5
 * En	: other					PA0.2 IO       : other    
 */
csi_error_t csi_eflash_wr_useroption(csi_eflash_t *ptEflash, uint32_t wData)
{
	csi_error_t tRet = CSI_OK;
	CSI_PARAM_CHK(ptEflash, CSI_ERROR);

    csp_ifc_t *ptEflashBase;
    ptEflashBase = (csp_ifc_t *)HANDLE_REG_BASE(ptEflash);
	
	
	tRet = csp_ifc_wr_useroption(ptEflashBase, wData);
	return tRet;
}


/**
  \brief       Erase flash from page (where wAddr is in) to page (where wAddr+wSize is in)
  \param       eflash  eflash handle to operate.
  \param 	   wAddr: start address
  \param       wSize: data(BYTES) to be erased (!!! whole page would be erased !!!)
  \return      error code
*/
csi_error_t csi_eflash_erase(csi_eflash_t *ptEflash, uint32_t wOffset, uint32_t wSize)
{
	uint8_t i;
	csi_error_t tRet = CSI_OK;
	uint32_t wAddr, wPageStAddr, wPageEndAddr;
	csp_ifc_t *ptEflashBase;
	wAddr = DFLASHBASE + wOffset;
//	if (wSize%4 == 0)
//		wSize = wSize/4;
//	else
//		wSize = wSize/4 + 1;
    
	wPageStAddr = wAddr & 0xffffffc0;
	
	
	wPageEndAddr  = ((wAddr + wSize)& 0xffffffc0) + DFLASH_PAGE_SZ*4 -1;
	
		
	if (wAddr > 0x10000800 || (wAddr < 0x10000000) ||(wPageEndAddr > DFLASHLIMIT) || wSize <1) {
		tRet = CSI_ERROR;
		return tRet;
	}
	
	ptEflashBase = (csp_ifc_t *)HANDLE_REG_BASE(ptEflash);
	do {
		csp_ifc_step_sync(ptEflashBase, PAGE_ERASE, wPageStAddr);
		while((ptEflashBase->RISR & IFCINT_ERS_END) == 0); /*// Wait for operation done*/
		csp_ifc_clr_int(ptEflashBase, IFCINT_ERS_END);
	
		g_bFlashCheckPass = 1;
		for (i=0 ; i<16; i++) {
			if ((*(uint32_t *)(wPageStAddr + (i<<2))) != 0x0) {
				tRet = CSI_ERROR;
				g_bFlashCheckPass = 0;
				return tRet;
			}
		}
		wPageStAddr += DFLASH_PAGE_SZ<<2;
	} while(wPageStAddr<=wPageEndAddr);
	
	return tRet;
}

/**
  \brief       Erase whole flash.
   * !!! The function would erase the whole chip. !!!
  \param[in]   eflash  eflash handle to operate.
  \return      error code
*/
csi_error_t csi_eflash_erase_chip(csi_eflash_t *ptEflash)
{
	
	return CSI_UNSUPPORTED;
}
/**
  \brief       Get ||Data Flash|| information.
  \param[in]   eflash  eflash handle to operate.
*/
void csi_eflash_info(csi_eflash_t *ptEflash,csi_eflash_info_t *eflash_info)
{
	eflash_info->erased_value = 0;
	eflash_info->flash_size = DFLASHSIZE;
	eflash_info->sector_size = DFLASH_PAGE_SZ << 2;
}

/**
  \brief       enable eflash power manage
  \param[in]   eflash  eflash handle to operate.
  \return      error code
*/
csi_error_t csi_eflash_enable_pm(csi_eflash_t *ptEflash)
{
	return CSI_UNSUPPORTED;
}

/**
  \brief       disable eflash power manage
  \param[in]   eflash  eflash handle to operate.
*/
void csi_eflash_disable_pm(csi_eflash_t *ptEflash)
{
	///CSI_UNSUPPORTED;
}

/** \brief get flash status
 *  \param ptEflash eflash handle to operate.
 *  \return eflash_status_t
 */
csi_eflash_status_t csi_eflash_get_status(csi_eflash_t *ptEflash)
{
	csi_eflash_status_t tStatus;
	csp_ifc_t *ptEflashBase;
    ptEflashBase = (csp_ifc_t *)HANDLE_REG_BASE(ptEflash);
	
	if (g_bFlashPgmDne)
		tStatus.busy = 0;
	else
		tStatus.busy =  1;
	if (csp_ifc_get_risr(ptEflashBase) & (IFCINT_PROT_ERR|IFCINT_UDEF_ERR|IFCINT_OVW_ERR|IFCINT_ADDR_ERR))
		tStatus.error = 1;
	else
		tStatus.error = 0;
		
	return tStatus;
}