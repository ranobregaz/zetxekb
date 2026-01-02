/***********************************************************************//** 
 * \file  uart.c
 * \brief  csi uart driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-12 <td>V0.0  <td>ZJY   <td>initial
 * </table>
 * *********************************************************************
*/
#include <sys_clk.h>
#include <drv/uart.h>
#include <drv/irq.h>
#include <drv/gpio.h>
#include <drv/pin.h>
#include <drv/porting.h>
#include <drv/tick.h>

#include "csp_uart.h"

/* Private macro------------------------------------------------------*/
#define UART_TIMEOUT    	0x10000000U
#define UART_IDX_NUM   		3
#define UART_RECV_MAX_LEN	128

/* externs function---------------------------------------------------*/
/* externs variablesr-------------------------------------------------*/
/* Private variablesr-------------------------------------------------*/
//static	uint32_t s_wRecvLen[UART_IDX_NUM]	 = {0, 0, 0};			
//static	uint32_t s_wPreRecvLen[UART_IDX_NUM] = {0, 0, 0};	

static uint32_t s_wCtrlRegBack = 0;	

/** \brief uart trends receive scan,asynchronism mode
 * 
 *  \param[in] uart: UART handle to operate
 *  \return none
 */ 
//void csi_uart_recv_dynamic_scan(csi_uart_t *uart)
//{
//	uint8_t byIdx = uart->dev.idx;
//	
//	if(s_wRecvLen[byIdx] > 0)								//receive data len > 0
//	{
//		if(s_wPreRecvLen[byIdx] == s_wRecvLen[byIdx])		//receive data end 		 
//		{
//			uart->arg = (void *)s_wPreRecvLen[byIdx];
//			uart->state.readable = 1U;
//			s_wRecvLen[byIdx] = 0;
//			s_wPreRecvLen[byIdx] = 0;
//			
//			if(uart->callback) 
//				uart->callback(uart, UART_EVENT_RECEIVE_DYNAMIC_COMPLETE, uart->arg);
//			else
//				uart->state.readable = 0U;
//		}
//		else
//			s_wPreRecvLen[byIdx] = s_wRecvLen[byIdx];				//receive go on
//	}
//}
/** \brief uart rx disable
 * 
 *  \param[in] UART handle to operate
 *  \return none
 */ 
//static void apt_uart_rx_dis(csi_uart_t *uart)
//{
//    csp_uart_t *uart_base  = (csp_uart_t *)HANDLE_REG_BASE(uart); 
//	csp_uart_set_ctrl(uart_base,((uint32_t)(uart->priv)) & (~UART_RX_MSK));	
//}

/** \brief uart rx ensable
 * 
 *  \param[in] UART handle to operate
 *  \return none
 */ 
//static void apt_uart_rx_en(csi_uart_t *uart)
//{
//    csp_uart_t *uart_base  = (csp_uart_t *)HANDLE_REG_BASE(uart);
//	csp_uart_set_ctrl(uart_base,((uint32_t)(uart->priv)) | (UART_RX_MSK));	
//}
/** \brief uart send data,interrupt mode
 * 
 *  \param[in] uart: UART handle to operate
 *  \return none
 */ 
//static void uart_intr_send_data(csi_uart_t *uart)
//{
//    csp_uart_t *uart_base  = (csp_uart_t *)HANDLE_REG_BASE(uart); 
//	
//	uart->tx_size--;
//	uart->tx_data++;
//	
//    if (uart->tx_size == 0U) 
//	{
//		uint32_t wCtrlBackup = (((uint32_t)(uart->priv)) & (~UART_TXDONE_INT)) | UART_RX_MSK;		//disable tx_done int
//		uart->priv = (void *)wCtrlBackup;
//		csp_uart_set_ctrl(uart_base,(uint32_t)(uart->priv));
//	
//        uart->state.writeable = 1U;
//       
//		if(uart->callback)
//            uart->callback(uart, UART_EVENT_SEND_COMPLETE, uart->arg);
//    }
//	else
//	{
//		csp_uart_set_data(uart_base, *uart->tx_data);
//	}
//}
/** \brief uart receive data,interrupt mode
 * 
 *  \param[in] uart: UART handle to operate
 *  \return none
 */ 
//static void uart_intr_recv_data(csi_uart_t *uart)
//{
//	csp_uart_t *uart_base  = (csp_uart_t *)HANDLE_REG_BASE(uart); 
//	
//	uint8_t byIdx = uart->dev.idx;
//	
//	if ((uart->rx_data == NULL) || (uart->rx_size == 0U))
//	{
//		#if defined(CONFIG_UART_DYNAMIC)	//uart dynamic length receive
//			if(uart->rx_data)
//			{
//				if(s_wRecvLen[byIdx] < UART_RECV_MAX_LEN)
//					uart->rx_data[s_wRecvLen[byIdx] ++] = csp_uart_get_data(uart_base);
//				else
//					csp_uart_get_data(uart_base);
//			}
//			else
//			{
//				if (uart->callback) 
//					uart->callback(uart, UART_EVENT_RECEIVE_FIFO_READABLE, uart->arg);
//				else
//					csp_uart_get_data(uart_base);
//			}
//		#else
//			if(uart->callback) 
//				uart->callback(uart, UART_EVENT_RECEIVE_FIFO_READABLE, uart->arg);
//			else
//				csp_uart_get_data(uart_base);
//		#endif
//	}
//	else
//	{
//		*uart->rx_data = csp_uart_get_data(uart_base);
//		uart->rx_size--;
//		uart->rx_data++;
//		
//		if(uart->rx_size == 0)
//		{
//			uart->state.readable = 1U;
//			uart->rx_data = NULL;
//			//uart->rx_data = uart->rx_data - (uint32_t)uart->arg;
//			s_wRecvLen[byIdx] = 0x00;									
//			s_wPreRecvLen[byIdx] = 0x00;	
//			
//			if(uart->callback) 
//				uart->callback(uart, UART_EVENT_RECEIVE_COMPLETE, uart->arg);
//		}
//	}
//}
/** \brief uart interrupt handle function
 * 
 *  \param[in] UART handle to operate
 *  \param[in] args: dev of uart
 *  \return none
 */ 
//static void apt_uart_irqhandler(void *args)
//{
//	csi_uart_t *uart = (csi_uart_t *)args;
//    csp_uart_t *uart_base  = (csp_uart_t *)HANDLE_REG_BASE(uart); 
//
//	uint32_t wIntState = csp_uart_get_isr(uart_base);
//	
//	
//	if(wIntState & UART_RXFIFO_INT_S)						//uart rx		
//	{
//		csp_uart_clr_isr(uart_base,UART_RXFIFO_INT_S);		//clear interrupt status
//		uart_intr_recv_data(uart);					
//	}
//	
//	if(wIntState & UART_TXDONE_INT_S)						//uart tx			
//	{
//		csp_uart_clr_isr(uart_base,UART_TXDONE_INT_S);		//clear interrupt status
//		uart_intr_send_data(uart);	
//	}
//	
//	if(wIntState & UART_RXFIFO_OV_INT_S)					//rx fifo over
//	{
//		csp_uart_clr_isr(uart_base, UART_RXFIFO_OV_INT_S);	//clear interrupt status
//		if(uart->callback) 
//				uart->callback(uart, UART_EVENT_ERROR_OVERFLOW, uart->arg);
//	} 
//	
//	if(wIntState & UART_PAR_ERR_S)						//parity error
//	{
//		csp_uart_clr_isr(uart_base, UART_PAR_ERR_S);	//clear interrupt status
//		if(uart->callback) 
//				uart->callback(uart, UART_EVENT_ERROR_PARITY, uart->arg);
//	} 
//	
//	//csp_uart_clr_isr(uart_base,0xbf);					//clear other status
//		
//}
/** \brief Initialize uart data structure
 * 
 *  \param[in] ptUartBase: pointer of uart register structure
 *  \param[in] ptUartCfg: pointer of uart parameter config structure
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_uart_init(csp_uart_t *ptUartBase, csi_uart_config_t *ptUartCfg)
{
	csi_error_t ret = CSI_OK;
	uart_parity_e eParity = PARITY_NONE;
	uint32_t wBrDiv;
	
	csi_clk_enable((uint32_t *)ptUartBase);				//uart peripheral clk enable
	
	wBrDiv = soc_get_pclk_freq()/ptUartCfg->wBaudRate;	
	if(wBrDiv < 16)
		wBrDiv = 16;
	csp_uart_set_brdiv(ptUartBase, wBrDiv);				//set uart baud rate 
	
	switch(ptUartCfg->byParity)							//set uart parity bits
	{
		case UART_PARITY_NONE:
			break;
		case UART_PARITY_ODD:
			eParity = PARITY_ODD;
			break;
		case PARITY_EVEN:
			eParity = PARITY_EVEN;
			break;
		default:
			ret = CSI_ERROR;
			break;
	}
	
	//uart databits = 8; fixed, can not be configured 
	//uart stopbits = 1; fixed, can not be configured 
	//set (parity/fx fifo = 1_8/fifo enable)
	s_wCtrlRegBack = (eParity << UART_PARITY_POS) | (FIFO_RX_1_8 << UART_RXFIFO_POS) | (UART_FIFO_EN << UART_FIFO_POS);
	csp_uart_set_ctrl(ptUartBase, s_wCtrlRegBack);		//set uart ctrl reg 
	
	if(ptUartCfg->wInter)
	{
		s_wCtrlRegBack |= ptUartCfg->wInter;
		csp_uart_set_ctrl(ptUartBase, s_wCtrlRegBack);	//enable uart xxx interrupt
		csi_irq_enable((uint32_t *)ptUartBase);			//enable uart irq			
	}
	return ret;
}
/** \brief start(enable) uart rx/tx
 * 
 *  \param[in] ptUartBase: pointer of uart register structure
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_uart_start(csp_uart_t *ptUartBase)
{
	s_wCtrlRegBack |= ((UART_TX_EN << UART_TX_POS) | (UART_RX_EN << UART_RX_POS));
	csp_uart_set_ctrl(ptUartBase, s_wCtrlRegBack);		//enable uart rx/tx 
	
	return CSI_OK;
}
/** \brief stop(disable) uart rx/tx
 * 
 *  \param[in] ptUartBase: pointer of uart register structure
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_uart_stop(csp_uart_t *ptUartBase)
{
	s_wCtrlRegBack &= (~UART_TX_MSK | ~UART_RX_MSK);
	csp_uart_set_ctrl(ptUartBase, s_wCtrlRegBack);		//disable uart rx/tx 
	return CSI_OK;
}
/** \brief de-initialize uart interface
 * 
 *  \param[in] uart: UART handle to operate
 *  \return none
 */
void csi_uart_uninit(csi_uart_t *uart)
{
//    CSI_PARAM_CHK_NORETVAL(uart);
//
//    csp_uart_t *uart_base  = (csp_uart_t *)HANDLE_REG_BASE(uart); 
//	
//	uart->rx_size = 0U;
//    uart->tx_size = 0U;
//    uart->rx_data = NULL;
//    uart->tx_data = NULL;
//
//	csp_uart_set_int(uart_base, UART_TX_INT, false);
//	csp_uart_set_int(uart_base, UART_RXFIFO_INT, false);
//	
//    csi_irq_disable((uint32_t)(uart->dev.irq_num));
//    csi_irq_detach((uint32_t)(uart->dev.irq_num));
}
/** \brief uart send character
 * 
 *  \param[in] ptUartBase: pointer of uart register structure
 *  \param[in] byData: the character to be send
 *  \return  none
 */
void csi_uart_putc(csp_uart_t *ptUartBase, uint8_t byData)
{
	while((csp_uart_get_sr(ptUartBase) & UART_TX_FULL));		
	csp_uart_set_data(ptUartBase, byData);
}
/** \brief uart get character
 * 
 *  \param[in] uart: UART handle to operate
 *  \return  the character to get
 */
uint8_t csi_uart_getc(csp_uart_t *ptUartBase)
{
	while(!(csp_uart_get_sr(ptUartBase) & UART_RNE));
	return csp_uart_get_data(ptUartBase);
}
/** \brief send data to uart transmitter, this function is blocking.
 * 
 *  \param[in] ptUartBase: pointer of uart register structure
 *  \param[in] pData: pointer to buffer with data to send to uart transmitter.
 *  \param[in] wSize: number of data to send (byte).
 *  \param[in] wTimeOut: (wTimeOut = 0: send with timeout hangdle); (wTimeOut > 0: send without timeout hangdle).
 *  \return  the num of data which is sent successfully or CSI_ERROR
 */
int32_t csi_uart_send(csp_uart_t *ptUartBase, const void *pData, uint32_t wSize, uint32_t wTimeOut)
{
	uint8_t  *pbySend = (uint8_t *)pData;
	int32_t  i, wCount = wTimeOut;
	
	if(wTimeOut)			//send with timeout 
	{
		for(i = 0; i < wSize; i++)
		{
			while((csp_uart_get_sr(ptUartBase) & UART_TX_FULL) && wCount--);
			
			if(wCount)
				csp_uart_set_data(ptUartBase, *(pbySend+i));
			else
				return i;
				
			wCount = wTimeOut;
		}
	}
	else					//send without timeout 
	{
		for(i = 0; i < wSize; i++)
		{
			while((csp_uart_get_sr(ptUartBase) & UART_TX_FULL));
			csp_uart_set_data(ptUartBase, *(pbySend+i));
		}
	}
	
	
	return i;
	
}
csi_error_t apt_uart_send_intr(csi_uart_t *uart, const void *data, uint32_t size)
{
//	csp_uart_t *uart_base = (csp_uart_t *)HANDLE_REG_BASE(uart);
//	uint32_t wCtrlBackup;
//	
//    uart->tx_data = (uint8_t *)data;
//    uart->tx_size = size;
//	//uart->arg = (void *)size;		//return len
//	
//	wCtrlBackup = (((uint32_t)(uart->priv)) | UART_TXDONE_INT) & (~UART_RX_MSK);				//enable tx int, disable rx
//	uart->priv = (void *)wCtrlBackup;
//	csp_uart_set_ctrl(uart_base,(uint32_t)(uart->priv));	
	
    return CSI_OK;
}
/** \brief send data to uart transmitter, asynchronism mode
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] data: pointer to buffer with data to send to UART transmitter.
 *  \param[in] size: number of data to send (byte).
 *  \return  error code \ref csi_error_t
 */
csi_error_t csi_uart_send_async(csi_uart_t *uart, const void *data, uint32_t size)
{
	csi_error_t ret;
//    CSI_PARAM_CHK(uart, CSI_ERROR);
//    CSI_PARAM_CHK(data, CSI_ERROR);
//    CSI_PARAM_CHK(uart->callback, CSI_ERROR);
//    CSI_PARAM_CHK(uart->send, CSI_ERROR);
//
//    csi_error_t ret;
//	csp_uart_t *uart_base = (csp_uart_t *)HANDLE_REG_BASE(uart);
//	
//	if(size == 0)				
//		return CSI_ERROR;
//	
//    ret = uart->send(uart, data, size);
//
//    if (ret == CSI_OK) {
//        uart->state.writeable = 0U;
//		
//		csp_uart_set_data(uart_base, *uart->tx_data);			//start uart tx,send first byte
//    }
	
    return ret;
}
/** \brief receive data to uart transmitter, this function is blocking.
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] data: pointer to buffer with data to be received.
 *  \param[in] size: number of data to receive (byte).
 *  \param[in] timeout: the timeout between bytes(ms). DO NOT support now
 *  \return  the num of data which is sent successfully or CSI_ERROR
 */
 int32_t csi_uart_receive(csi_uart_t *uart, void *data, uint32_t size, uint32_t timeout)
 {
	CSI_PARAM_CHK(uart, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
	
	csp_uart_t *uart_base = (csp_uart_t *)HANDLE_REG_BASE(uart);
	
	uint8_t  *pbyRecv = (uint8_t *)data;
	int32_t  wRecvNum = 0;
	
	
	//disable rx_fifo int	
	if((uint32_t)(uart->priv) & UART_RXFIFO_INT)							
 		csp_uart_set_ctrl(uart_base,((uint32_t)(uart->priv)) & (~UART_RXFIFO_INT));	
	
//	if(timeout == 0)
//	{
//		uint32_t wTimeOver = 500;
//		uint32_t wRecvStart = wTimeOver; 
//		while(wRecvNum < size)
//		{
//			while(!csp_uart_fifo_get_ready(uart_base) && wRecvStart --)		//fifo empty? wait
//			{
//				if(wRecvStart == 0) 
//				{
//					//enable rx_fifo int	
//					if((uint32_t)(uart->priv) & UART_RXFIFO_INT)							
//						csp_uart_set_ctrl(uart_base,(uint32_t)(uart->priv));
//						
//					return wRecvNum;
//				}
//			}
//			pbyRecv[wRecvNum ++] = csp_uart_get_data(uart_base);
//			wRecvStart = wTimeOver;
//		}
//	}
//	else
	{
		uint32_t wRecvStart = csi_tick_get_ms();	
		while(wRecvNum < size)
		{
			while(!csp_uart_fifo_get_ready(uart_base))				//fifo empty? wait	
			{
				if((csi_tick_get_ms() - wRecvStart) >= timeout) 
				{
					//enable rx_fifo int	
					if((uint32_t)(uart->priv) & UART_RXFIFO_INT)							
						csp_uart_set_ctrl(uart_base,(uint32_t)(uart->priv));

					return wRecvNum;
				}
			}
			pbyRecv[wRecvNum ++] = csp_uart_get_data(uart_base);
			wRecvStart = csi_tick_get_ms();	
		}
	}
	
//	while(csp_uart_fifo_get_ready(uart_base))			//clear fifo
//	{
//		csp_uart_get_data(uart_base);
//	}

	//enable rx_fifo int	
	if((uint32_t)(uart->priv) & UART_RXFIFO_INT)							
		csp_uart_set_ctrl(uart_base,(uint32_t)(uart->priv));
	
	return wRecvNum;
 }
 
csi_error_t apt_uart_receive_intr(csi_uart_t *uart, void *data, uint32_t num)
{
//	uint32_t wCtrlBackup;
//	csp_uart_t *uart_base = (csp_uart_t *)HANDLE_REG_BASE(uart);
//	
//    uart->rx_data = (uint8_t *)data;
//    uart->rx_size = num;
//	//uart->arg = (void *)num;		//return len
//	
//	wCtrlBackup = ((uint32_t)(uart->priv)) | UART_RXFIFO_INT;				//enable rx int
//	uart->priv = (void *)wCtrlBackup;
//	csp_uart_set_ctrl(uart_base,(uint32_t)(uart->priv));	
	
    return CSI_OK;
}
/** \brief receive data to uart transmitter, asynchronism mode
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] data: pointer to buffer with data to send to UART transmitter.
 *  \param[in] size: number of data to send (byte).
 *  \return  error code \ref csi_error_t
 */
csi_error_t csi_uart_receive_async(csi_uart_t *uart, void *data, uint32_t size)
{
	csi_error_t ret;
//    CSI_PARAM_CHK(uart, CSI_ERROR);
//    CSI_PARAM_CHK(data, CSI_ERROR);
//    CSI_PARAM_CHK(uart->callback, CSI_ERROR);
//    CSI_PARAM_CHK(uart->receive, CSI_ERROR);
//
//    csi_error_t ret;
//	uint8_t byIdx = uart->dev.idx;
//	
//    ret = uart->receive(uart, data, size);
//
//    if (ret == CSI_OK) {
//        uart->state.readable = 0U;
//		s_wRecvLen[byIdx] = 0x00;
//		s_wPreRecvLen[byIdx] = 0x00;	
//    }

    return ret;
}
/** \brief  register uart interrupt callback function
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] callback: uart interrupt handle function
 *  \param[in] arg: para
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_uart_attach_callback(csi_uart_t *uart, void  *callback, void *arg)
{
	CSI_PARAM_CHK(uart, CSI_ERROR);
	
//	uint32_t wCtrlBackup;
//	csp_uart_t *uart_base = (csp_uart_t *)HANDLE_REG_BASE(uart);
//	
//    uart->callback = callback;
//    uart->arg = arg;
//    uart->send = apt_uart_send_intr;
//    uart->receive = apt_uart_receive_intr;
//	
//    csi_irq_attach((uint32_t)(uart->dev.irq_num), &apt_uart_irqhandler, &uart->dev);
//    csi_irq_enable((uint32_t)(uart->dev.irq_num));
//
//	wCtrlBackup = ((uint32_t)(uart->priv)) | UART_RXFIFO_INT | UART_RXFIFO_OV_INT | UART_PAR_ERR_INT;		//enable rx int
//	uart->priv = (void *)wCtrlBackup;
//	csp_uart_set_ctrl(uart_base,(uint32_t)(uart->priv));	
	
    return CSI_OK;
}
/** \brief cancel uart callback function
 * 
 *  \param[in] uart: UART handle to operate
 *  \return none
 */ 
void csi_uart_detach_callback(csi_uart_t *uart)
{
    CSI_PARAM_CHK_NORETVAL(uart);

//    csp_uart_t *uart_base;
//    uart_base = (csp_uart_t *)HANDLE_REG_BASE(uart);
//
//    uart->callback  = NULL;
//    uart->arg = NULL;
//    uart->send = NULL;
//    uart->receive = NULL;
//	
//	//disable tx rx int
//    csp_uart_set_ctrl(uart_base,(((uint32_t)(uart->priv)) & (~UART_RXFIFO_INT) & (~UART_TXDONE_INT)));	
//	
//    csi_irq_disable((uint32_t)(uart->dev.irq_num));
//    csi_irq_detach((uint32_t)(uart->dev.irq_num));
}
/** \brief get the state of uart device
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] state: the state of uart device
 *  \return none
 */ 
//csi_error_t csi_uart_get_state(csi_uart_t *uart, csi_state_t *state)
//{
//    CSI_PARAM_CHK(uart, CSI_ERROR);
//    CSI_PARAM_CHK(state, CSI_ERROR);
//
//    *state = uart->state;
//
//    return CSI_OK;
//}
/** \brief       Link DMA channel to uart device.
 * 
 *  \param[in]   uart     uart handle to operate.
 *  \param[in]   tx_dma   the DMA channel handle for send, when it is NULL means to unlink the channel.
 *  \param[in]   rx_dma   the DMA channel handle for receive, when it is NULL means to unlink the channel.
 *  \return      error code.
*/
//csi_error_t csi_uart_link_dma(csi_uart_t *uart, csi_dma_ch_t *tx_dma, csi_dma_ch_t *rx_dma)
//{
//	CSI_PARAM_CHK_NORETVAL(uart);
//	
//	return CSI_UNSUPPORTED;
//}
