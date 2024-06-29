/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     soc.h
 * @brief    For pin
 * @version  V1.0
 * @date     11. Mar 2020
 ******************************************************************************/

#ifndef _DRV_PIN_H_
#define _DRV_PIN_H_

#include <drv/common.h>
#include <soc.h>
#include <drv/gpio.h>




typedef enum{
    PIN_UART_TX		= 0U,
    PIN_UART_RX,
    PIN_UART_CTS,
    PIN_UART_RTS
}csi_pin_uart_t;

typedef enum{
    PIN_IIC_SCL		= 0U,
    PIN_IIC_SDA
}csi_pin_iic_t;

typedef enum{
    PIN_SPI_MISO	= 0U,
    PIN_SPI_MOSI,
    PIN_SPI_SCK,
    PIN_SPI_CS
}csi_pin_spi_t;

typedef enum{
    PIN_I2S_MCLK	= 0U,
    PIN_I2S_SCLK,
    PIN_I2S_WSCLK,
    PIN_I2S_SDA
}csi_pin_i2s_t;

/**
 * \enum     csi_pin_speed_e
 * \brief    GPIO pin speed define
 */
//typedef enum {
//    PIN_SPEED_LV0 	= 0U,
//    PIN_SPEED_LV1,
//} csi_pin_speed_e;
typedef csi_gpio_speed_e	csi_pin_speed_e;

/**
 * \enum     csi_pin_drive_e
 * \brief    GPIO pin drive define
 */
//typedef enum {
//    PIN_DRIVE_LV0 	= 0U,
//    PIN_DRIVE_LV1,
//} csi_pin_drive_e;
typedef csi_gpio_drive_e	csi_pin_drive_e;

/**
 * \enum     csi_pin_input_mode_e
 * \brief    GPIO pin input mode
 */
//typedef enum {
//	PIN_INPUT_CMOS 	= 0U,
//	PIN_INPUT_TTL1,
//	PIN_INPUT_TTL2
//}csi_pin_input_mode_e;

typedef csi_gpio_input_mode_e	csi_pin_input_mode_e;
/**
 * \enum     csi_pin_output_mode_e
 * \brief    GPIO pin output mode
 */
//typedef enum {
//	PIN_PUSH_PULL 	= 0U,		//push-pull
//	PIN_OPEN_DRAIN,				//open drain
//}csi_pin_output_mode_e;
typedef csi_gpio_output_mode_e 	csi_pin_output_mode_e;

/**
 * \enum     csi_pin_pull_mode_e
 * \brief    GPIO pin pull mode
 */
//typedef enum
//{
//	PIN_PULLNONE	= 0U,   			
//  PIN_PULLUP,						
//	PIN_PULLDOWN						
//}csi_pin_pull_mode_e;

typedef csi_gpio_pull_mode_e	csi_pin_pull_mode_e;

/**
 * \enum     csi_gpio_irq_mode_e
 * \brief    GPIO irq triger type
 */
//typedef enum {
//    PIN_IRQ_RISING_EDGE  = 0,    		///< Interrupt mode for rising edge
//    PIN_IRQ_FALLING_EDGE,        		///< Interrupt mode for falling edge
//    PIN_IRQ_BOTH_EDGE          			///< Interrupt mode for both edge
//} csi_pin_irq_mode_e;
typedef csi_gpio_irq_mode_e		csi_pin_irq_mode_e;


typedef struct {
    pin_name_e  pin_name;
    uint8_t     idx;        ///< ctrl idx.    e.g: ADC0 channel 1, idx = 0, channel = 1
    uint8_t     channel;    ///< channel idx. e.g: same as the previous line
    pin_func_e  pin_func;
} csi_pinmap_t;

extern uint32_t target_pin_to_devidx(pin_name_e pin_name, const csi_pinmap_t *pinmap);
extern uint32_t target_pin_to_channel(pin_name_e pin_name,const csi_pinmap_t *pinmap);
extern pin_name_e target_gpio_to_pin(uint8_t gpio_idx, uint8_t channel,const csi_pinmap_t *pinmap);

/**
  \brief       Set pin mux function
  \param[in]   ePinName    pin name, defined in soc.h
  \param[in]   ePinFunc    pin function, defined in soc.h
  \return      \ref  csi_error_t
*/
csi_error_t csi_pin_set_mux(pin_name_e ePinName, pin_func_e ePinFunc);

/**
  \brief       Get pin function
  \param[in]   ePinName    pin name, defined in soc.h
  \return      pin function
*/
pin_func_e csi_pin_get_mux(pin_name_e ePinName);

/**
  \brief       Set pin mode
  \param[in]   ePinName    pin name, defined in soc.h
  \param[in]   ePullMode   pull mode, pull none/pull up/pull down
  \return      \ref  csi_error_t
*/
csi_error_t csi_pin_pull_mode(pin_name_e ePinName, csi_pin_pull_mode_e ePullMode);

/**
  \brief       Set pin speed
  \param[in]   ePinName    pin name, defined in soc.h
  \param[in]   eSpeed      io speed
  \return      \ref  csi_error_t
*/
csi_error_t csi_pin_speed(pin_name_e ePinName, csi_pin_speed_e eSpeed);

/**
  \brief       Set pin drive
  \param[in]   ePinName    pin name, defined in soc.h
  \param[in]   eDrive      io drive
  \return      \ref  csi_error_t
*/
csi_error_t csi_pin_drive(pin_name_e ePinName, csi_pin_drive_e eDrive);

/** 
  \brief 	   set gpio pin input mode
  \param[in]   ePinName		gpio pin name, defined in soc.h.
  \param[in]   eInputMode	INPUT_CMOS/INPUT_TTL
  \return 	   error code \ref csi_error_t
 */ 
csi_error_t csi_pin_input_mode(pin_name_e ePinName, csi_pin_input_mode_e eInputMode);

/** \brief set gpio pin out mode
  \param[in]   ePinName		gpio pin name, defined in soc.h.
  \param[in]   eOutMode		push-pull/open drain
  \return error code \ref csi_error_t
 */ 
csi_error_t csi_pin_output_mode(pin_name_e ePinName, csi_pin_output_mode_e eOutMode);

///** \brief config gpio pull up or pull down
//  \param[in] pin_name: gpio pin name
//  \param[in] pull_mode: pull up/down
//  \return error code \ref csi_error_t
// */ 
//csi_error_t csi_pin_pull_mode(pin_name_e pin_name, csi_pin_pull_t pull_mode);

/** \brief  gpio external interrupt setting that supports extanded IO
  \param[in] ptGpio: GPIO port handle
  \param[in] wPinMask: pin mask,0x0001~0xffff
  \param[in] eGrp: exi group:EXI_GRP0 - 19
  \param[in] ePinTrg: gpio pin int trigger mode: PIN_TRG_IRT/PIN_TRG_IFT/PIN_TRG_BOTH
  \return error code \ref csi_error_t
 */
//csi_error_t csi_pin_irq_mode(pin_name_e ePinName, csi_exi_grp_e eGrp, csi_gpio_irq_mode_t ePinTrg);

/** \brief get gpio pin num
  \param[in] ePinName: gpio pin name, defined in soc.h.
  \return pin num
 */
uint8_t csi_pin_get_num(pin_name_e ePinName);

/** \brief  gpio toggle
  \param[in] pin_name: gpio pin name
  \return error code \ref csi_error_t
 */
csi_error_t csi_pin_toggle(pin_name_e ePinName);

/** \brief config pin irq mode(assign exi group)
 * 
 *  \param[in] ePinName: pin name
 *  \param[in] eExiGrp: exi group; EXI_GRP0 ~EXI_GRP19
 *  \param[in] eTrgEdge: rising edge; falling edge;	both edge;
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_pin_irq_mode(pin_name_e ePinName, csi_exi_grp_e eExiGrp, csi_pin_irq_mode_e eTrgEdge);

/** \brief pinirq enable
 * 
 *  \param[in] ePinName: pin mask,0x0001~0xffff
 *  \param[in] bEnable: true or false
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_pin_irq_enable(pin_name_e ePinName, csi_exi_grp_e eExiGrp, bool bEnable);


__ALWAYS_STATIC_INLINE uint32_t csi_pin_get_gpio_devidx(pin_name_e pin_name)
{
    extern const csi_pinmap_t gpio_pinmap[];
    return target_pin_to_devidx(pin_name, gpio_pinmap);
}

__ALWAYS_STATIC_INLINE uint32_t csi_pin_get_uart_devidx(pin_name_e pin_name)
{
    extern const csi_pinmap_t uart_pinmap[];
    return target_pin_to_devidx(pin_name, uart_pinmap);
}

__ALWAYS_STATIC_INLINE uint32_t csi_pin_get_iic_devidx(pin_name_e pin_name)
{
    extern const csi_pinmap_t iic_pinmap[];
    return target_pin_to_devidx(pin_name, iic_pinmap);
}

__ALWAYS_STATIC_INLINE uint32_t csi_pin_get_spi_devidx(pin_name_e pin_name)
{
    extern const csi_pinmap_t spi_pinmap[];
    return target_pin_to_devidx(pin_name, spi_pinmap);
}

__ALWAYS_STATIC_INLINE uint32_t csi_pin_get_i2s_devidx(pin_name_e pin_name)
{
    extern  const csi_pinmap_t i2s_pinmap[];
    return target_pin_to_devidx(pin_name, i2s_pinmap);
}

/**
  \brief       Get channel by pin
  \param[in]   pin_name    pin name, defined in soc.h
  \return      channel
*/
__ALWAYS_STATIC_INLINE uint32_t csi_pin_get_adc_channel(pin_name_e pin_name)
{
    extern const csi_pinmap_t adc_pinmap[];
    return target_pin_to_channel(pin_name, adc_pinmap);
}

__ALWAYS_STATIC_INLINE uint32_t csi_pin_get_pwm_channel(pin_name_e pin_name)
{
    extern const csi_pinmap_t pwm_pinmap[];
    return target_pin_to_channel(pin_name, pwm_pinmap);
}

__ALWAYS_STATIC_INLINE uint32_t csi_pin_get_gpio_channel(pin_name_e pin_name)
{
    extern const csi_pinmap_t gpio_pinmap[];
    return target_pin_to_channel(pin_name, gpio_pinmap);
}

/**
  \brief       Get pin name by gpio ctrl idx and channel
  \param[in]   gpio_idx    idx, defined in soc.h
  \param[in]   channel     channel, defined in soc.h
  \return      pin name
*/
__ALWAYS_STATIC_INLINE pin_name_e csi_pin_get_pinname_by_gpio(uint8_t gpio_idx, uint8_t channel)
{
    extern const csi_pinmap_t gpio_pinmap[];
    return target_gpio_to_pin(gpio_idx,channel,gpio_pinmap);
}

#endif /* _DRV_PIN_H_ */
