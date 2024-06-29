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

/** \brief etcb test
 * 
 *  \param[in] none
 *  \return error code
 */
int etcb_demo(void)
{
	int iRet = 0;
	uint8_t ch;
	csi_etb_config_t tEtbConfig;	
	etb_trg_mode_e eEtbDemo = ETB_DEMO_ONE_TRG_ONE;
	
	switch(eEtbDemo)
	{
		case ETB_DEMO_ONE_TRG_ONE:
			
			//para init config
			tEtbConfig.ch_type = ETB_ONE_TRIGGER_ONE;		//one channel trigger one channel
			tEtbConfig.src_ip = ETB_EXI_TRGOUT0;			//exi group0 as trigger source
			tEtbConfig.src_ip1 = 0xff;						
			tEtbConfig.src_ip2 = 0xff;
			tEtbConfig.dst_ip = ETB_BT0_SYNCIN0;			//bt0 syncin0
			tEtbConfig.dst_ip1 = 0xff;
			tEtbConfig.dst_ip2 = 0xff;
			tEtbConfig.trig_mode = ETB_HARDWARE_TRG;
			
			iRet = csi_etb_init();
			ch = csi_etb_ch_alloc(tEtbConfig.ch_type);
			iRet |= csi_etb_ch_config(ch,&tEtbConfig);
			
			break;
		case ETB_DEMO_ONE_TRG_MORE:
			break;;
		case ETB_DEMO_MORE_TRG_ONE:
			break;
		default:
			iRet = CSI_ERROR;
			break;
	}
			
	return iRet;
}