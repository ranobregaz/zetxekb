
/***********************************************************************//** 
 * \file  irq_port.c
 * \brief  CSI Source File for irq port
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-8 <td>V0.0  <td>ZJY   <td>initial
 * </table>
 * *********************************************************************
*/
//#include <csi_config.h>
#include <stdbool.h>
#include <csi_core.h>

void soc_irq_enable(uint32_t irq_num)
{
#ifdef CONFIG_SYSTEM_SECURE
    csi_vic_enable_sirq((int32_t)irq_num);
#else
    csi_vic_enable_irq((int32_t)irq_num);
#endif
}

void soc_irq_disable(uint32_t irq_num)
{
#ifdef CONFIG_SYSTEM_SECURE
    csi_vic_disable_sirq((int32_t)irq_num);
#else
    csi_vic_disable_irq((int32_t)irq_num);
#endif
}

bool soc_irq_is_enabled(uint32_t irq_num)
{
    bool ret;

    if (csi_vic_get_enabled_irq((int32_t)irq_num)) {
        ret = true;
    } else {
        ret = false;
    }

    return ret;
}

void soc_irq_priority(uint32_t irq_num, uint32_t priority)
{
    csi_vic_set_prio((int32_t)irq_num, priority);
}

void soc_irq_enable_wakeup(uint32_t irq_num)
{
    csi_vic_set_wakeup_irq((int32_t)irq_num);
}

void soc_irq_disable_wakeup(uint32_t irq_num)
{
    csi_vic_clear_wakeup_irq((int32_t)irq_num);
}

uint32_t soc_irq_get_irq_num(void)
{
#ifdef __CSKY__
    return ((__get_PSR() >> 16U) & 0xFFU) - 32U;
#elif defined(__riscv)
    return (__get_MCAUSE() & 0x3FF);
#endif
}
