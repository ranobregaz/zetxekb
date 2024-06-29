
/***********************************************************************//** 
 * \file  irq_port.c
 * \brief  CSI Source File for irq port
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-13 <td>V0.0 <td>ZJY     <td>initial
 * </table>
 * *********************************************************************
*/
//#include <csi_config.h>
#include <stdbool.h>
#include <irq.h>

extern csi_irqmap_t irq_map[];

/** \brief irq enable
 * 
 *  Enable irq in INTERRUPT
 * 
 *  \param[in] pIpBase: pointer of devices Base address
 *  \return none.
 */
void csi_irq_enable(uint32_t *pIpBase)
{
	csi_irqmap_t *ptMap = irq_map;

    while(ptMap->wRegBase) 
	{
        if((uint32_t)pIpBase == ptMap->wRegBase) 
		{
			csi_vic_enable_irq((int32_t)ptMap->wIrqNum);
            break;
        }
		
        ptMap++;
    }
	
//	switch((uint32_t)pIpBase)
//	{
//		case CK801_ADDR_BASE:
//			csi_vic_enable_irq(CORET_IRQn);			//CORET interrupt
//			break;
//		case APB_SYS_BASE:
//			csi_vic_enable_irq(SYSCON_IRQn);		//SYSCON interrupt
//			break;
//		case APB_IFC_BASE:
//			csi_vic_enable_irq(IFC_IRQn);			//IFC interrupt
//			break;
//		case APB_ADC0_BASE:
//			csi_vic_enable_irq(ADC_IRQn);			//ADC interrupt
//			break;
//		case APB_EPT0_BASE:
//			csi_vic_enable_irq(EPT0_IRQn);			//EPT0 interrupt
//			break;
//		case APB_WWDT_BASE:
//			csi_vic_enable_irq(WWDT_IRQn);			//WWDT interrupt
//			break;
//		case APB_GPTA0_BASE:
//			csi_vic_enable_irq(GPT0_IRQn);			//GPT0 interrupt
//			break;
//		case APB_RTC_BASE:
//			csi_vic_enable_irq(RTC_IRQn);			//RTC interrupt
//			break;
//		case APB_UART0_BASE:
//			csi_vic_enable_irq(UART0_IRQn);			//UART0 interrupt
//			break;
//		case APB_UART1_BASE:
//			csi_vic_enable_irq(UART1_IRQn);			//UART1 interrupt
//			break;
//		case APB_UART2_BASE:
//			csi_vic_enable_irq(UART2_IRQn);			//UART2 interrupt
//			break;
//		case APB_I2C0_BASE:
//			csi_vic_enable_irq(I2C_IRQn);			//I2C interrupt
//			break;
//		case APB_SPI0_BASE:
//			csi_vic_enable_irq(SPI_IRQn);			//SPI interrupt
//			break;
//		case APB_SIO0_BASE:
//			csi_vic_enable_irq(SIO_IRQn);			//SIO interrupt
//			break;
//		case APB_CNTA_BASE:
//			csi_vic_enable_irq(CNTA_IRQn);			//CNTA interrupt
//			break;
//		case APB_TKEY_BASE:
//			csi_vic_enable_irq(TKEY_IRQn);			//TKEY interrupt
//			break;
//		case APB_LPT_BASE:
//			csi_vic_enable_irq(LPT_IRQn);			//LPT interrupt
//			break;
//		case APB_BT0_BASE:
//			csi_vic_enable_irq(BT0_IRQn);			//BT0(TOMER0) interrupt
//			break;
//		case APB_BT1_BASE:
//			csi_vic_enable_irq(BT1_IRQn);			//BT1(TOMER1) interrupt
//			break;
//		default:
//			break;
//	}
}

/** \brief irq disable
 * 
 *  Disable irq in INTERRUPT
 * 
 *  \param[in] pIpBase: pointer of devices Base address
 *  \return none.
 */
void csi_irq_disable(uint32_t *pIpBase)
{
	csi_irqmap_t *ptMap = irq_map;

    while(ptMap->wRegBase) 
	{
        if((uint32_t)pIpBase == ptMap->wRegBase) 
		{
			csi_vic_disable_irq((int32_t)ptMap->wIrqNum);
            break;
        }
		
        ptMap++;
    }
	
//	switch((uint32_t)pIpBase)
//	{
//		case CK801_ADDR_BASE:
//			csi_vic_disable_irq(CORET_IRQn);		//CORET interrupt
//			break;
//		case APB_SYS_BASE:
//			csi_vic_disable_irq(SYSCON_IRQn);		//SYSCON interrupt
//			break;
//		case APB_IFC_BASE:
//			csi_vic_disable_irq(IFC_IRQn);			//IFC interrupt
//			break;
//		case APB_ADC0_BASE:
//			csi_vic_disable_irq(ADC_IRQn);			//ADC interrupt
//			break;
//		case APB_EPT0_BASE:
//			csi_vic_disable_irq(EPT0_IRQn);			//EPT0 interrupt
//			break;
//		case APB_WWDT_BASE:
//			csi_vic_disable_irq(WWDT_IRQn);			//WWDT interrupt
//			break;
//		case APB_GPTA0_BASE:
//			csi_vic_disable_irq(GPT0_IRQn);			//GPT0 interrupt
//			break;
//		case APB_RTC_BASE:
//			csi_vic_disable_irq(RTC_IRQn);			//RTC interrupt
//			break;
//		case APB_UART0_BASE:
//			csi_vic_disable_irq(UART0_IRQn);		//UART0 interrupt
//			break;
//		case APB_UART1_BASE:
//			csi_vic_disable_irq(UART1_IRQn);		//UART1 interrupt
//			break;
//		case APB_UART2_BASE:
//			csi_vic_disable_irq(UART2_IRQn);		//UART2 interrupt
//			break;
//		case APB_I2C0_BASE:
//			csi_vic_disable_irq(I2C_IRQn);			//I2C interrupt
//			break;
//		case APB_SPI0_BASE:
//			csi_vic_disable_irq(SPI_IRQn);			//SPI interrupt
//			break;
//		case APB_SIO0_BASE:
//			csi_vic_disable_irq(SIO_IRQn);			//SIO interrupt
//			break;
//		case APB_CNTA_BASE:
//			csi_vic_disable_irq(CNTA_IRQn);			//CNTA interrupt
//			break;
//		case APB_TKEY_BASE:
//			csi_vic_disable_irq(TKEY_IRQn);			//TKEY interrupt
//			break;
//		case APB_LPT_BASE:
//			csi_vic_disable_irq(LPT_IRQn);			//LPT interrupt
//			break;
//		case APB_BT0_BASE:
//			csi_vic_disable_irq(BT0_IRQn);			//BT0(TOMER0) interrupt
//			break;
//		case APB_BT1_BASE:
//			csi_vic_disable_irq(BT1_IRQn);			//BT1(TOMER1) interrupt
//			break;
//		default:
//			break;
//	}
}

//void soc_irq_enable(uint32_t irq_num)
//{
//#ifdef CONFIG_SYSTEM_SECURE
//    csi_vic_enable_sirq((int32_t)irq_num);
//#else
//    csi_vic_enable_irq((int32_t)irq_num);
//#endif
//}
//
//void soc_irq_disable(uint32_t irq_num)
//{
//#ifdef CONFIG_SYSTEM_SECURE
//    csi_vic_disable_sirq((int32_t)irq_num);
//#else
//    csi_vic_disable_irq((int32_t)irq_num);
//#endif
//}
//
//bool soc_irq_is_enabled(uint32_t irq_num)
//{
//    bool ret;
//
//    if (csi_vic_get_enabled_irq((int32_t)irq_num)) {
//        ret = true;
//    } else {
//        ret = false;
//    }
//
//    return ret;
//}
//
//void soc_irq_priority(uint32_t irq_num, uint32_t priority)
//{
//    csi_vic_set_prio((int32_t)irq_num, priority);
//}
//
//void soc_irq_enable_wakeup(uint32_t irq_num)
//{
//    csi_vic_set_wakeup_irq((int32_t)irq_num);
//}
//
//void soc_irq_disable_wakeup(uint32_t irq_num)
//{
//    csi_vic_clear_wakeup_irq((int32_t)irq_num);
//}
//
//uint32_t soc_irq_get_irq_num(void)
//{
//#ifdef __CSKY__
//    return ((__get_PSR() >> 16U) & 0xFFU) - 32U;
//#elif defined(__riscv)
//    return (__get_MCAUSE() & 0x3FF);
//#endif
//}
