/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     sys_console.c
 * @brief    CSI Source File for console
 * @version  V1.0
 * @date     2019-12-19
 ******************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <drv/pin.h>
#include "sys_console.h"

#include <drv/uart.h>

csi_uart_t console_uart;

int32_t console_init(sys_console_t *handle)
{
//    int32_t ret = 0;
//
//    extern csi_uart_t console_uart;
//
//    csi_pin_set_mux(handle->tx.pin, handle->tx.func);
//    csi_pin_set_mux(handle->rx.pin, handle->rx.func);
//
//    ret = csi_uart_init(&console_uart, handle->uart_id);
//
//    /* config the UART */
//    ret = csi_uart_baud(&console_uart, handle->baudrate);
//    if (ret < 0) {
//        return -1;
//    }
//    ret = csi_uart_format(&console_uart, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1);
//    if (ret < 0) {
//        return -1;
//    }
//
//    handle->uart = &console_uart;

    return 0;
}

int fputc(int ch, FILE *stream)
{
    (void)stream;

    if (ch == '\n') {
        csi_uart_putc(&console_uart, (uint8_t)'\r');
    }

    csi_uart_putc(&console_uart, (uint8_t)ch);

    return 0;
}

int fgetc(FILE *stream)
{
    uint8_t ch;
    (void)stream;

    ch = csi_uart_getc(&console_uart);

    return (int)ch;
}
