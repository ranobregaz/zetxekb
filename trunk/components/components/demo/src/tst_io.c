/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     tst_io.c
 * @brief    input/output for test
 * @version  V0.1
 * @date     2020-01-01
 ******************************************************************************/
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "stdlib.h"
#include "common.h"
#include "uart.h"
#include "soc.h"
#include "ringbuffer.h"
#include "board_config.h"
#include "tst_io.h"
#include "drv/pin.h"

static csi_uart_t               g_uart;
static ringbuffer_t             g_io_ringbuffer;
static tst_uart_config_t g_uart_config;
static uint8_t g_io_rx_buf[TST_IO_BUFFER_SZ] = {0};

void tst_io_config_default(void)
{


    //g_uart_config.uart_rx.pin_port = TST_UART_RX_PIN_PORT;
    g_uart_config.uart_rx.pin_idx  = TST_UART_RX_PIN_IDX;
    g_uart_config.uart_rx.pin_func = TST_UART_RX_PIN_FUNC;

    //g_uart_config.uart_tx.pin_port = TST_UART_TX_PIN_PORT;
    g_uart_config.uart_tx.pin_idx  = TST_UART_TX_PIN_IDX;
    g_uart_config.uart_tx.pin_func = TST_UART_TX_PIN_FUNC;

    g_uart_config.uart_id = TST_UART_IDX;
    g_uart_config.uart_attr.baudrate = 115200;
    g_uart_config.uart_attr.data_bits = UART_DATA_BITS_8;
    g_uart_config.uart_attr.parity    = UART_PARITY_NONE;
    g_uart_config.uart_attr.stop_bits = UART_STOP_BITS_1;
	
	g_io_ringbuffer.buffer = g_io_rx_buf;
	g_io_ringbuffer.size = sizeof(g_io_rx_buf);

    return;
}


/**
 * \brief              init io device
 * \param              uart_config  device param set by user
 *                     if NULL,use default param.
 * \return             err code
 */
csi_error_t tst_io_init(void)
{
    csi_error_t ret = CSI_OK;
	
	/* uart dev control init  */	
	tst_io_config_default();

    /* uart gpio mux config */
	
    // rx pin-mux config
    csi_pin_set_mux(  g_uart_config.uart_rx.pin_idx,\
                      g_uart_config.uart_rx.pin_func);
    // tx pin-mux config
    csi_pin_set_mux(g_uart_config.uart_tx.pin_idx,\
                      g_uart_config.uart_tx.pin_func); 

	g_uart.utNo = g_uart_config.uart_id;				  
	g_uart.BaudRate = g_uart_config.uart_attr.baudrate;
    g_uart.DataBits = g_uart_config.uart_attr.data_bits;
    g_uart.Parity = g_uart_config.uart_attr.parity;
    g_uart.Stopbits = g_uart_config.uart_attr.stop_bits;
	g_uart.Interrupt = UART_RX_INT_FLG | UART_PAR_ERR_FLG ;
	g_uart.wkmode =0;
	g_uart.ringbuf = &g_io_ringbuffer;

	ret = csi_uart_attach_int_callback((csi_uart_t*)&g_uart,csi_uart_creat_event_default, csi_uart_handle_event_default,NULL);
    if(ret != CSI_OK)
    {
        return ret;
    }		  

    /* init io handle */
    ret = csi_uart_init((csi_uart_t*)&g_uart,g_uart.utNo);
    if(ret != CSI_OK)
    {
        return ret;
    }
	
	/* open uart */
	ret = csi_uart_open((csi_uart_t*)&g_uart);
   
    return ret;
}

/**
 * \brief              send data to io
 * \param[in]          buf      data buf
 * \param[in]          buf_len  lenght of data
 * \return             err code
 */

csi_error_t tst_io_send(char *buf, uint32_t buf_len)
{
    uint32_t send_num;
    send_num = csi_uart_send(&g_uart, (const void *) buf, buf_len, 0xFFFF);
    return (send_num == buf_len)?CSI_OK:CSI_ERROR;
}

/**
 * \brief              receive data from io
 * \param[in]          buf      point to data buf
 * \param[in]          buf_len  lenght of data
 * \return             data bytes readed
 */

uint32_t tst_io_rcv(char* buf,uint32_t buf_len)
{
    csi_error_t err =0;
	err = csi_uart_receive_async(&g_uart,buf,buf_len);
	if(err) return 0;
	else{
		return buf_len;
	}
    return 0;
}

