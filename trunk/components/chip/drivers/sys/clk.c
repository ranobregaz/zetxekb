
/***********************************************************************//** 
 * \file  clk.c
 * \brief  clock management for all peripherals
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-10 <td>V0.0 <td>WNN   <td>initial
 * <tr><td> 2021-5-13 <td>V0.0 <td>ZJY   <td>initial
 * </table>
 * *********************************************************************
*/
#include <drv/clk.h>
#include <drv/pin.h>

#include "csp.h"
#include "board_config.h"

extern csi_clkmap_t clk_map[];

/** \brief clock enable
 * 
 *  Enable peripheral clock in SYSCON
 * 
 *  \param[in] pIpBase: pointer of peripheral Base address
 *  \return none.
 */
void csi_clk_enable(uint32_t *pIpBase)
{
    csi_clkmap_t *ptMap = clk_map;

    while(ptMap->wRegBase) 
	{
        if((uint32_t)pIpBase == ptMap->wRegBase) 
		{
            soc_clk_enable((clk_module_t)ptMap->wModule);
            break;
        }
		
        ptMap++;
    }
	
//	switch((uint32_t)pIpBase)
//	{
//		case APB_IFC_BASE:
//			csp_pcer0_clk_en(SYSCON, (uint32_t)IFC_SYS_CLK);		//IFC clk enable
//			break;
//		case APB_ADC0_BASE:
//			csp_pcer0_clk_en(SYSCON, (uint32_t)ADC_SYS_CLK);		//ADC clk enable
//			break;
//		case APB_TKEY_BASE:
//			csp_pcer0_clk_en(SYSCON, (uint32_t)TOUCH_SYS_CLK);		//TKEY clk enable
//			break;
//		case APB_ETCB_BASE:
//			csp_pcer0_clk_en(SYSCON, (uint32_t)ETCB_SYS_CLK);		//ETCB clk enable
//			break;
//		case APB_UART0_BASE:
//			csp_pcer0_clk_en(SYSCON, (uint32_t)UART0_SYS_CLK);		//UART0 clk enable
//			break;
//		case APB_UART1_BASE:
//			csp_pcer0_clk_en(SYSCON, (uint32_t)UART1_SYS_CLK);		//UART1 clk enable
//			break;
//		case APB_UART2_BASE:
//			csp_pcer0_clk_en(SYSCON, (uint32_t)UART2_SYS_CLK);		//UART2 clk enable
//			break;
//		case APB_SPI0_BASE:
//			csp_pcer0_clk_en(SYSCON, (uint32_t)SPI_SYS_CLK);		//SPI clk enable
//			break;
//		case APB_SIO0_BASE:
//			csp_pcer0_clk_en(SYSCON, (uint32_t)SIO_SYS_CLK);		//SIO clk enable
//			break;
//		case APB_I2C0_BASE:
//			csp_pcer0_clk_en(SYSCON, (uint32_t)I2C_SYS_CLK);		//I2C clk enable
//			break;
//		case APB_WWDT_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)WWDT_SYS_CLK - 32);	//WWDT clk enable
//			break;
//		case APB_RTC_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)RTC_SYS_CLK - 32);	//RTC clk enable
//			break;
//		case APB_LPT_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)LPT_SYS_CLK - 32);	//LPT clk enable
//			break;
//		case APB_CNTA_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)CNTA_SYS_CLK - 32);	//CNTA clk enable
//			break;
//		case APB_BT0_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)BT0_SYS_CLK - 32);	//BT0 clk enable
//			break;
//		case APB_BT1_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)BT1_SYS_CLK - 32);	//BT1 clk enable
//			break;
//		case APB_GPTA0_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)GPT_SYS_CLK - 32);	//GPT clk enable
//			break;
//		case APB_EPT0_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)EPT_SYS_CLK - 32);	//EPT clk enable
//			break;
//		default:
//			break;
//	}
}

/** \brief clock disable
 * 
 *  Disable peripheral clock in SYSCON
 * 
 *  \param[in] pIpBase: pointer of peripheral Base address
 *  \return none.
 */
void csi_clk_disable(uint32_t *pIpBase)
{
    csi_clkmap_t *ptMap = clk_map;

	while(ptMap->wRegBase)
	{
        if((uint32_t)pIpBase == ptMap->wRegBase) 
		{
            soc_clk_disable((clk_module_t)ptMap->wModule);
            break;
        }
        ptMap++;
    }
	
//	switch((uint32_t)pIpBase)
//	{
//		case APB_IFC_BASE:
//			csp_pder0_clk_dis(SYSCON, (uint32_t)IFC_SYS_CLK);		//IFC clk disable
//			break;
//		case APB_ADC0_BASE:
//			csp_pder0_clk_dis(SYSCON, (uint32_t)ADC_SYS_CLK);		//ADC clk disable
//			break;
//		case APB_TKEY_BASE:
//			csp_pder0_clk_dis(SYSCON, (uint32_t)TOUCH_SYS_CLK);		//TKEY clk disable
//			break;
//		case APB_ETCB_BASE:
//			csp_pder0_clk_dis(SYSCON, (uint32_t)ETCB_SYS_CLK);		//ETCB clk disable
//			break;
//		case APB_UART0_BASE:
//			csp_pder0_clk_dis(SYSCON, (uint32_t)UART0_SYS_CLK);		//UART0 clk disable
//			break;
//		case APB_UART1_BASE:
//			csp_pder0_clk_dis(SYSCON, (uint32_t)UART1_SYS_CLK);		//UART1 clk disable
//			break;
//		case APB_UART2_BASE:
//			csp_pder0_clk_dis(SYSCON, (uint32_t)UART2_SYS_CLK);		//UART2 clk disable
//			break;
//		case APB_SPI0_BASE:
//			csp_pder0_clk_dis(SYSCON, (uint32_t)SPI_SYS_CLK);		//SPI clk disable
//			break;
//		case APB_SIO0_BASE:
//			csp_pder0_clk_dis(SYSCON, (uint32_t)SIO_SYS_CLK);		//SIO clk disable
//			break;
//		case APB_I2C0_BASE:
//			csp_pder0_clk_dis(SYSCON, (uint32_t)I2C_SYS_CLK);		//I2C clk disable
//			break;
//		case APB_WWDT_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)WWDT_SYS_CLK - 32);	//WWDT clk disable
//			break;
//		case APB_RTC_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)RTC_SYS_CLK - 32);	//RTC clk disable
//			break;
//		case APB_LPT_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)LPT_SYS_CLK - 32);	//LPT clk disable
//			break;
//		case APB_CNTA_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)CNTA_SYS_CLK - 32);	//CNTA clk disable
//			break;
//		case APB_BT0_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)BT0_SYS_CLK - 32);	//BT0 clk disable
//			break;
//		case APB_BT1_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)BT1_SYS_CLK - 32);	//BT1 clk disable
//			break;
//		case APB_GPTA0_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)GPT_SYS_CLK - 32);	//GPT clk disable
//			break;
//		case APB_EPT0_BASE:
//			csp_pder1_clk_dis(SYSCON, (uint32_t)EPT_SYS_CLK - 32);	//EPT clk disable
//			break;
//		default:
//			break;
//	}
}

/** \brief emosc enable
 * 
 *  enable external main oscillator in SYSCON
 * 
 *  \param none
 *  \return csi_error_t
 */
csi_error_t csi_emosc_enable(uint32_t wFreq)
{
	
	if ((csi_pin_get_mux(XIN_PIN) != XIN_PIN_FUNC) || (csi_pin_get_mux(XOUT_PIN) != XOUT_PIN_FUNC))
		return CSI_ERROR;
	
	if (wFreq > 20000000)
		csp_set_em_gain(SYSCON, 0x1f);
	else
		csp_set_em_gain(SYSCON, 0x7);
		
	csp_set_em_gain(SYSCON, 0x1f);
	
	if (wFreq == 32768) {
		csp_set_em_lfmd(SYSCON, ENABLE);
		SYSCON->OSTR = (SYSCON->OSTR) & ((~0x3ff)|(0xff));
	}
	

	SYSCON->GCER = EMOSC;
	//wait for EMOSC to stable
	while(!(csp_get_ckst(SYSCON)& EMOSC));
	return CSI_OK;
}

/** \brief emosc disable
 * 
 *  disable external main oscillator in SYSCON
 * 
 *  \param none
 *  \return csi_error_t.
 */
csi_error_t csi_emosc_disable(void)
{
	if ((SYSCON->SCLKCR & SYSCLK_SRC_MSK) == SC_EMOSC)
		return CSI_ERROR;
	else
	{
		SYSCON->GCDR = EMOSC;
		return CSI_OK;
	}
	
//	if (csp_emosc_disable(SYSCON_REG_BASE) == CSP_FAIL)
//		return CSI_ERROR;
//	else
//		return CSI_OK;
}

/** \brief imosc enable
 * 
 *  enable internal main oscillator in SYSCON
 * 
 *  \param byFre. 	0 - 5MHz
 * 					1 - 4MHz
 * 					2 - 2MHz
					3 - 131KHz
 *  \return csi_error_t
 */
csi_error_t csi_imosc_enable(uint8_t byFre)
{
	csp_set_imosc_fre(SYSCON, byFre);
	SYSCON->GCER = IMOSC;
	//wait for IMOSC to stable
	while(!(csp_get_ckst(SYSCON)& IMOSC));
	return CSI_OK;
	
//	csp_imosc_enable(SYSCON_REG_BASE, byFre);
//	return CSI_OK;
}

/** \brief imosc disable
 * 
 *  disable internal main oscillator in SYSCON
 * 
 *  \param none
 *  \return csi_error_t.
 */
csi_error_t csi_imosc_disable(void)
{
	if ((SYSCON->SCLKCR & SYSCLK_SRC_MSK) == SC_IMOSC)
		return CSI_ERROR;
	else{
		SYSCON->GCDR = IMOSC;
		return CSI_OK;
	}
	
//	if (csp_imosc_disable(SYSCON_REG_BASE) == CSP_FAIL)
//		return CSI_ERROR;
//	else
//		return CSI_OK;
}

 /** \brief hfosc enable
 * 
 *  enable high frequency oscillator in SYSCON
 * 
 *  \param byFre. 	0 - 48MHz
 * 					1 - 24MHz
 * 					2 - 12MHz
					3 - 6KHz
 *  \return csi_error_t.
 */
csi_error_t csi_hfosc_enable(uint8_t byFre)
{
	csp_set_hfosc_fre(SYSCON, byFre);	
	SYSCON->GCER = HFOSC;
	//wait for HFOSC to stable
	while(!(csp_get_ckst(SYSCON)& HFOSC));
	return CSI_OK;
	
//	csp_hfosc_enable(SYSCON_REG_BASE, byFre);
//	return CSI_OK;
}

 /** \brief hfosc disable
 * 
 *  disable high frequency oscillator in SYSCON
 * 
 *  \param none
 *  \return csi_error_t
 */
csi_error_t csi_hfosc_disable(void)
{
	if ((SYSCON->SCLKCR & SYSCLK_SRC_MSK) == SC_HFOSC)
		return CSI_ERROR;
	else
	{
		SYSCON->GCDR = HFOSC;
		return CSI_OK;
	}
	
//	if (csp_hfosc_disable(SYSCON_REG_BASE) == CSP_FAIL)
//		return CSI_ERROR;
//	else
//		return CSI_OK;
}

/** \brief isosc enable
 * 
 *  enable internal sub oscillator in SYSCON
 * 
 *  \param none
 *  \return csi_error_t
 */
csi_error_t csi_isosc_enable(void)
{
	SYSCON->GCER = ISOSC;
	//wait for ISOSC to stable
	while(!(csp_get_ckst(SYSCON)& ISOSC));
	return CSI_OK;
	
//	csp_isosc_enable(SYSCON_REG_BASE);
//	return CSI_OK;
}

/** \brief isosc disable
 * 
 *  disable internal sub oscillator in SYSCON
 * 
 *  \param none
 *  \return csi_error_t.
 */
csi_error_t csi_isosc_disable(void)
{
	if((SYSCON->SCLKCR & SYSCLK_SRC_MSK) == SC_ISOSC || csp_iwdt_rd_st(SYSCON))
		return CSP_FAIL;
	else 
	{
		SYSCON->GCDR = ISOSC;
		return CSP_SUCCESS;
	}	
//	if (csp_isosc_disable(SYSCON_REG_BASE) == CSP_FAIL)
//		return CSI_ERROR;
//	else
//		return CSI_OK;
}