/***********************************************************************//** 
 * \file  csp_ifc.c
 * \brief  IFC function in csp level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-10 <td>V0.0  <td>WNN   <td>initial
 * <tr><td> 2021-1-6 <td>V0.1  <td>WNN   <td>support PFLASH PGM
 * </table>
 * *********************************************************************
*/


#include "soc.h" 
#include "csp_ifc.h"
#include "csp_common.h"

#define IFC_REG_BASE	(csp_ifc_t *)APB_IFC_BASE

volatile bool g_bFlashCheckPass = 1;
volatile bool g_bFlashPgmDne = 1;

///in case DFLASH writing, use int to do step4 and step5
static uint32_t g_wPageStAddr;
static uint32_t wBuffForCheck[DFLASH_PAGE_SZ];

 /** \brief set ifc working mode
 * 
 *  \param[in] ptIfcBase: pointer of IFC reg structure.
 *  \param[in] wFreq: frequency.
 *  \return csp_error_t.
 */
void csp_ifc_set_speed(csp_ifc_t * ptIfcBase, uint32_t wFreq)
{
	ptIfcBase->CEDR = IFC_CLKEN;
	ptIfcBase->MR = ptIfcBase->MR & (~(HIGH_SPEED|PF_WAIT3));
	if (wFreq > 24000000)
		ptIfcBase->MR |= HIGH_SPEED | PF_WAIT2;
    else if (wFreq >= 16000000) 
		ptIfcBase->MR |= HIGH_SPEED | PF_WAIT1;		
}

/** \brief flash opertation steps
 *  \param[in] ptIfcBase: pointer of IFC reg structure
 *	\param[in] eStepn: 	1 - PAGE_LAT_CLR
 * 						2 - PAGE_ERASE
 * 		  				3 - PRE_PGM
 * 		  				4 - CHIP_ERASE
 * 		 			 	5 - PROGRAM
 * \param[in] wPageStAddr: page start address
 * \return none
 */
void csp_ifc_step_sync(csp_ifc_t * ptIfcBase, ifc_cmd_e eStepn, uint32_t wPageStAddr)
{
	csp_ifc_unlock(ptIfcBase);
	csp_ifc_wr_cmd(ptIfcBase, eStepn);
	csp_ifc_addr(ptIfcBase, wPageStAddr);
	csp_ifc_start(ptIfcBase);
	
	///TODO do NOT support all sync operations for now
	if (eStepn == PROGRAM && ((ptIfcBase -> MR) & DFLASH_PMODE) && (wPageStAddr >= 0x10000000) ){
		while(ptIfcBase->RISR != IFCINT_PEP_END); /*// Wait for operation done*/
		csp_ifc_clr_int(ptIfcBase, IFCINT_PEP_END);
	}
	else {
		while(ptIfcBase->CR != 0);
	}
} 

/** \brief flash opertation steps
 *  \param[in] ptIfcBase: pointer of IFC reg structure
 *	\param[in] eStepn: 	1 - PAGE_LAT_CLR
 * 		  				3 - PRE_PGM
 * 		  				4 - PAGE_ERASE
 * 		 			 	5 - PROGRAM
 * \param[in] wPageStAddr: page start address
 * \return none
 */
void csp_ifc_step_async(csp_ifc_t * ptIfcBase, ifc_cmd_e eStepn, uint32_t wPageStAddr)
{
	csp_ifc_unlock(ptIfcBase);
	switch (eStepn)
	{
		case (PAGE_ERASE):
			csp_ifc_int_enable(ptIfcBase, IFCINT_ERS_END, ENABLE);
			break;
		case (PROGRAM):
			csp_ifc_int_enable(ptIfcBase, IFCINT_PGM_END, ENABLE);
			break;
		default: 
			break;
	}
	NVIC_EnableIRQ(IFC_IRQn);
	
	csp_ifc_wr_cmd(ptIfcBase, eStepn);
	csp_ifc_addr(ptIfcBase, wPageStAddr);
	csp_ifc_start(ptIfcBase);
} 

uint32_t csp_ifc_rd_1word(csp_ifc_t * ptIfcBase, uint32_t wAddr)
{
	return (*(uint32_t *)wAddr);
}


csp_error_t csp_ifc_wr_1word(csp_ifc_t * ptIfcBase, uint8_t bFlashType, uint32_t wAddr, uint32_t wData)
{
	uint32_t i, wPageStAddr, wBuff[PFLASH_PAGE_SZ];
	uint8_t bPageSize = DFLASH_PAGE_SZ;
	csp_error_t tRet = CSP_SUCCESS;
	
	///wait until previous flash PGM done
	while(!g_bFlashPgmDne);
	g_bFlashPgmDne = 0;
	
	if (bFlashType == PFLASH)
		bPageSize = PFLASH_PAGE_SZ;
	else
		bPageSize = DFLASH_PAGE_SZ;
	
	
	wPageStAddr = wAddr & 0xffffffc0;
	wAddr = wAddr >> 2 & 0xf;
		
	csp_ifc_clk_enable(ptIfcBase, ENABLE);
	///step1
	csp_ifc_step_sync(ptIfcBase, PAGE_LAT_CLR, wPageStAddr);
	///step2
	for(i=0; i< bPageSize; i++) {
      if( i == wAddr )
        wBuff[i] = wData ;
      else {
        wBuff[i] = *(uint32_t *)(wPageStAddr+4*i);
      }
    }
	for(i=0; i<bPageSize; i++) {
        *(uint32_t *)(wPageStAddr+4*i) = wBuff[i];
    }
	///step3
	csp_ifc_step_sync(ptIfcBase, PRE_PGM, wPageStAddr);
	///step4
	csp_ifc_step_sync(ptIfcBase, PROGRAM, wPageStAddr);
	
	if (bFlashType == PFLASH)
	{
	///step5
		csp_ifc_step_sync(ptIfcBase, PAGE_ERASE, wPageStAddr);
	///step6
		csp_ifc_step_sync(ptIfcBase, PROGRAM, wPageStAddr);
	///whole page check
		for (i=0; i<bPageSize; i++)
		{
			if (csp_ifc_rd_1word(ptIfcBase, wPageStAddr+4*i) !=  wBuff[i])
				tRet = CSP_FAIL;
		}
	}
	else{
		///set dataflash to work in parmode	
		///csp_ifc_dflash_paramode_enable(ptIfcBase, ENABLE);
		///step5 
		for (i=0; i< DFLASH_PAGE_SZ;i++)
		{
			wBuffForCheck[i] = wBuff[i];
		}
		g_wPageStAddr = wPageStAddr;
		csp_ifc_step_async(ptIfcBase, PAGE_ERASE, wPageStAddr);
	}
		
	return tRet;
}

csp_error_t csp_ifc_wr_nword(csp_ifc_t * ptIfcBase, uint8_t bFlashType, uint32_t wAddr, uint32_t wDataNum, uint32_t *pwData)
{
	uint32_t i, j, wPageStAddr, wBuff[PFLASH_PAGE_SZ];
	uint8_t bPageSize = DFLASH_PAGE_SZ;
	csp_error_t tRet = CSP_SUCCESS;
	
	while(!g_bFlashPgmDne);
	g_bFlashPgmDne = 0;
	
	if (bFlashType == PFLASH)
		bPageSize = PFLASH_PAGE_SZ;
	else
		bPageSize = DFLASH_PAGE_SZ;
	

	wPageStAddr = wAddr & 0xffffffc0;
	wAddr = wAddr >> 2 & 0xf;
	
	csp_ifc_clk_enable(ptIfcBase, ENABLE);
	///step1
	csp_ifc_step_sync(ptIfcBase, PAGE_LAT_CLR, wPageStAddr);
	///step2
	for(i=0; i< bPageSize; i++) {
      if( i == wAddr )
	  {
		for(j = 0; j<wDataNum; j++)
			wBuff[i++] = pwData[j];
		i--;
	  }
      else {
        wBuff[i] = *(uint32_t *)(wPageStAddr+4*i);
      }
    }
	for(i=0; i<bPageSize; i++) {
        *(uint32_t *)(wPageStAddr+4*i) = wBuff[i];
    }
	///step3
	csp_ifc_step_sync(ptIfcBase, PRE_PGM, wPageStAddr);
	///step4
	csp_ifc_step_sync(ptIfcBase, PROGRAM, wPageStAddr);

	if (bFlashType == DFLASH && csp_ifc_get_dflash_paramode(ptIfcBase) == 1)
	{
		///DFLASH step4 
		for (i=0; i< DFLASH_PAGE_SZ;i++)
		{
			wBuffForCheck[i] = wBuff[i];
		}
		g_wPageStAddr = wPageStAddr;
		csp_ifc_step_async(ptIfcBase, PAGE_ERASE, wPageStAddr);
	}
	else 
	{
	///step5
		csp_ifc_step_sync(ptIfcBase, PAGE_ERASE, wPageStAddr);
	///step6
		csp_ifc_step_sync(ptIfcBase, PROGRAM, wPageStAddr);
	///whole page check
		for (i=0; i<bPageSize; i++)
		{
			if (csp_ifc_rd_1word(ptIfcBase, wPageStAddr+4*i) !=  wBuff[i]) {
				tRet = CSP_FAIL;
				g_bFlashCheckPass = 0;
			}
		}
		if (tRet != CSP_FAIL)
			g_bFlashPgmDne = 1;
	}
	return tRet;
}

/**	\brief SWD IO remap
 *  \param[in] bGrp
 * 				SWD       	pin number
 * bGrp = 2    PA0.6(CLK)   	2
 * 				PA0.7(DIO)		3
 * bGrp = 1	   PA0.0(DIO)		19
 * 				PA0.1(CLK)		20
 * bGrp = 0    PA0.5(CLK)		10
 * 				PA0.12(DIO)		11
 */
csp_error_t csp_ifc_swd_remap(csp_ifc_t * ptIfcBase, uint8_t bGrp)
{
	uint32_t i, wBuff[DFLASH_PAGE_SZ];
	uint32_t wAddr, wData, wPageStAddr;
	csp_error_t tRet;
	
	while(!g_bFlashPgmDne);
	g_bFlashPgmDne = 0;
	
	tRet = CSP_SUCCESS;

	switch (bGrp)
	{
		case 0:
			wData = 0xd;
			break;
		case 1:
			wData = 0xaa;
			break;
		case 2:
			wData = 0x55;
			break;
		default:
			return CSP_FAIL;
			break;
	}
	
	wAddr = SWD_ADDR;
	wPageStAddr = wAddr & 0xffffffc0;
	wAddr = wAddr >> 2 & 0xf;
	
	csp_ifc_clk_enable(ptIfcBase, ENABLE);
	///step1
	csp_ifc_step_sync(ptIfcBase, PAGE_LAT_CLR|HIDM1, 0);
	///step2
	for(i=0;i< DFLASH_PAGE_SZ;i++) {
      if( i == wAddr )
        wBuff[i] = wData ;
      else {
        wBuff[i] = *(uint32_t *)(wPageStAddr+4*i);
      }
    }
	for(i=0;i<DFLASH_PAGE_SZ;i++) {
        *(uint32_t *)(wPageStAddr+4*i) = wBuff[i];
    }
	///step3
	csp_ifc_step_sync(ptIfcBase, PRE_PGM|HIDM1, 0);
	///step4
	csp_ifc_step_sync(ptIfcBase, SWDREMAP_ENABLE|HIDM1, 0);
	///step5
	csp_ifc_step_sync(ptIfcBase, SWDREMAP_ERASE|HIDM1, 0);
	///step6
	csp_ifc_step_sync(ptIfcBase, SWDREMAP_ENABLE|HIDM1, 0);
	
	///whole page check
	g_bFlashCheckPass = 1;
	for (i=0; i<DFLASH_PAGE_SZ; i++)
	{
		if (csp_ifc_rd_1word(ptIfcBase, wPageStAddr+4*i) !=  wBuff[i]) {
			tRet = CSP_FAIL;
			g_bFlashCheckPass = 0;
		}
	}
	g_bFlashPgmDne = 1;
	return tRet;
}

csi_error_t csp_ifc_wr_useroption(csp_ifc_t * ptIfcBase, uint32_t wData)
{
	uint32_t i, wBuff[DFLASH_PAGE_SZ];
	uint32_t wAddr, wPageStAddr;
	csp_error_t tRet;
	
	while(!g_bFlashPgmDne);
	g_bFlashPgmDne = 0;
	
	tRet = CSP_SUCCESS;
	
	wAddr = USEROPTION_ADDR;
	wPageStAddr = wAddr & 0xffffffc0;
	wAddr = wAddr >> 2 & 0xf;
	
	csp_ifc_clk_enable(ptIfcBase, ENABLE);
	
	///step1
	csp_ifc_step_sync(ptIfcBase, PAGE_LAT_CLR|HIDM1, 0);
	///step2
	for(i=0;i< DFLASH_PAGE_SZ;i++) {
      if( i == wAddr )
        wBuff[i] = wData ;
      else {
        wBuff[i] = *(uint32_t *)(wPageStAddr+4*i);
      }
    }
	for(i=0;i<DFLASH_PAGE_SZ;i++) {
        *(uint32_t *)(wPageStAddr+4*i) = wBuff[i];
    }
	///step3
	csp_ifc_step_sync(ptIfcBase, PRE_PGM|HIDM1, 0);
	///step4
	csp_ifc_step_sync(ptIfcBase, USEROPTION_PGM|HIDM1, 0);
	///step5
	csp_ifc_step_sync(ptIfcBase, USEROPTION_ERASE|HIDM1, 0);
	///step6
	csp_ifc_step_sync(ptIfcBase, USEROPTION_PGM|HIDM1, 0);
	
	///whole page check
	g_bFlashCheckPass = 1;
	for (i=0; i<DFLASH_PAGE_SZ; i++)
	{
		if (csp_ifc_rd_1word(ptIfcBase, wPageStAddr+4*i) !=  wBuff[i]) {
			tRet = CSP_FAIL;
			g_bFlashCheckPass = 0;
		}
	}
	g_bFlashPgmDne = 1;
	return tRet;
}

void csp_ifc_irq_handler(void) 
{
	uint32_t i;
	csp_ifc_t * ptIfcBase = (csp_ifc_t *)IFC_REG_BASE;
	
	if (csp_ifc_get_misr(ptIfcBase) == IFCINT_ERS_END)
	{
		csp_ifc_int_enable(ptIfcBase, IFCINT_ERS_END, DISABLE);
		csp_ifc_clr_int(ptIfcBase, IFCINT_ERS_END);
		///DFLASH step6
		csp_ifc_step_async(ptIfcBase, PROGRAM, g_wPageStAddr);
	}
	if (csp_ifc_get_misr(ptIfcBase) == IFCINT_PGM_END)
	{
		csp_ifc_int_enable(ptIfcBase, IFCINT_PGM_END, DISABLE);
		csp_ifc_clr_int(ptIfcBase, IFCINT_PGM_END);
		///whole page check, only DFlash Write would use INT scheme
		g_bFlashCheckPass = 1;
		for (i=0; i<DFLASH_PAGE_SZ; i++)
		{
			if (csp_ifc_rd_1word(ptIfcBase, g_wPageStAddr+4*i) !=  wBuffForCheck[i])
				g_bFlashCheckPass = 0;
		}
		g_bFlashPgmDne = 1;
		
	}
}
