/***********************************************************************//** 
 * \file  iwdt_demo.c
 * \brief  IWDT_DEMO description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-13 <td>V0.0 <td>ZJY    <td>initial
 * </table>
 * *********************************************************************
*/
/* Includes ---------------------------------------------------------------*/
#include <string.h>
#include "drv/etb.h"
#include "drv/pin.h"
#include "demo.h"
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private macro-----------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
typedef enum{
	ETB_DEMO_ONE_TRG_ONE	= 0,	//ETB one trigger one
	ETB_DEMO_ONE_TRG_MORE,			//ETB one trigger more
	ETB_DEMO_MORE_TRG_ONE			//ETB more trigger one
}etb_trg_mode_e;

/** \brief etcb one trg one 
 * 
 *  \param[in] none
 *  \return error code
 */
int etcb_one_trg_one_demo(void)
{
	int iRet = 0;
	uint8_t ch;
	csi_etb_config_t tEtbConfig;	
			
	//para config
	tEtbConfig.eChType 	= ETB_ONE_TRG_ONE;		//one channel trigger one channel
	tEtbConfig.bySrcIp 	= ETB_EXI_TRGOUT0;		//exi group0 as trigger source
	tEtbConfig.bySrcIp1 = 0xff;						
	tEtbConfig.bySrcIp2 = 0xff;
	tEtbConfig.byDstIp 	= ETB_BT0_SYNCIN0;		//bt0 syncin0
	tEtbConfig.byDstIp1 = 0xff;
	tEtbConfig.byDstIp2 = 0xff;
	tEtbConfig.eTrgMode = ETB_HARDWARE_TRG;
	
	iRet = csi_etb_init();
	ch = csi_etb_ch_alloc(tEtbConfig.eChType);
	iRet |= csi_etb_ch_config(ch,&tEtbConfig);
			
	return iRet;
}

/** \brief etcb one trg more 
 * 
 *  \param[in] none
 *  \return error code
 */
int etcb_one_trg_more_demo(void)
{
	int iRet = 0;
	return iRet;
}

/** \brief etcb more trg one 
 * 
 *  \param[in] none
 *  \return error code
 */
int etcb_more_trg_one_demo(void)
{
	int iRet = 0;
	return iRet;
}