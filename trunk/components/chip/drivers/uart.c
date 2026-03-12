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
#include <drv/ringbuffer.h>
#include <drv/tick.h>

/* Private macro------------------------------------------------------*/
/* externs function---------------------------------------------------*/
/* externs variablesr-------------------------------------------------*/
/* Private variablesr-------------------------------------------------*/
csi_uart_transfer_t g_tUartTran[UART_IDX_NUM];	
static uint32_t s_wCtrlRegBack = 0;	


/** \brief get uart idx 
 * 
 *  \param[in] ptUartBase: pointer of uart register structure
 *  \return uart id number(0~3)
 */ 
static uint8_t apt_get_uart_idx(csp_uart_t *ptUartBase)
{
	switch((uint32_t)ptUartBase)
	{
		case APB_UART0_BASE:
			return 0;
		case APB_UART1_BASE:
			return 1;

		case APB_UART2_BASE:
			return 2;
		default:
			return 0xff;		//error
	}
}
/** \brief uart interrupt handle function
 * 
 *  \param[in] ptUartBas: epointer of uart register structure
 *  \param[in] byIdx: uart id number(0~2)
 *  \return none
 */ 
void apt_uart_irqhandler(csp_uart_t *ptUartBase,uint8_t byIdx)
{
	volatile uint32_t wIntState = csp_uart_get_isr(ptUartBase);
	
	switch(wIntState & 0x080040)			//get RXFIFO/TXDONE interrupt
	{
		case UART_RXFIFO_INT_S:				//rx fifo interrupt; recommended use RXFIFO interrupt
		
			if(csp_uart_get_sr(ptUartBase) & UART_RNE)
			{
				uint8_t byData = csp_uart_get_data(ptUartBase);
				ringbuffer_byte_in(g_tUartTran[byIdx].ptRingBuf, byData);
			}
			break;
		case UART_TXDONE_INT_S:				//tx send complete; recommended use TXDONE interrupt
		
			csp_uart_clr_isr(ptUartBase,UART_TXDONE_INT_S);						//clear interrupt status
			g_tUartTran[byIdx].hwTxSize --;
			g_tUartTran[byIdx].pbyTxData ++;
			
			if(g_tUartTran[byIdx].hwTxSize == 0)		
				g_tUartTran[byIdx].bySendStat = UART_STATE_DONE;				//send complete
			else
				csp_uart_set_data(ptUartBase, *g_tUartTran[byIdx].pbyTxData);	//send data
				
			break;
		default:
			break;
	}
}
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
/** \brief send data to uart transmitter, this function is polling(sync mode).
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

/** \brief send data to uart transmitter, this function is interrupt mode(async mode)
 * 
 *  \param[in] ptUartBase: pointer of uart register structure
 *  \param[in] pData: pointer to buffer with data to send to uart transmitter.
 *  \param[in] wSize: number of data to send (byte).
 *  \return  error code \ref csi_error_t
 */
csi_error_t csi_uart_send_intr(csp_uart_t *ptUartBase, const void *pData, uint32_t wSize)
{
	csi_error_t ret = CSI_ERROR;
	uint8_t byIdx = apt_get_uart_idx(ptUartBase);

	if(wSize == 0 || byIdx >= UART_IDX_NUM) 
		return CSI_ERROR;
	
	g_tUartTran[byIdx].pbyTxData =(uint8_t *)pData;
	g_tUartTran[byIdx].hwTxSize = wSize;
	
	if(g_tUartTran[byIdx].bySendStat == UART_STATE_SEND)					//uart sending?
		ret = CSI_ERROR;
	else
	{
		g_tUartTran[byIdx].bySendStat = UART_STATE_SEND;					//set uart send status, sending
		csp_uart_set_data(ptUartBase, *g_tUartTran[byIdx].pbyTxData);		//start uart tx,send first byte
	}
	
	return ret;
}
/** \brief receive data to uart transmitter, this function is polling(sync).
 * 
 *  \param[in] ptUartBase: UART handle to operate
 *  \param[in] pData: pointer to buffer with data to be received.
 *  \param[in] wSize: number of data to receive (byte).
 *  \param[in] wTimeOut: the timeout between bytes(ms). 
 *  \return  the num of data which is send successfully
 */
int32_t csi_uart_receive(csp_uart_t *ptUartBase, void *pData, uint32_t wSize, uint32_t wTimeOut)
{
	uint8_t  *pbyRecv = (uint8_t *)pData;
	int32_t  wRecvNum = 0;
	
	if(wTimeOut)
	{
		uint32_t wRecvStart = csi_tick_get_ms();	
		while(wRecvNum < wSize)
		{
			while(!(csp_uart_get_sr(ptUartBase) & UART_RNE))			//fifo empty? wait	
			{
				if((csi_tick_get_ms() - wRecvStart) >= wTimeOut) 
					return wRecvNum;
			}
			pbyRecv[wRecvNum ++] = csp_uart_get_data(ptUartBase);
			wRecvStart = csi_tick_get_ms();	
		}
	}
	else
	{
		while(wRecvNum < wSize)
		{
			pbyRecv[wRecvNum ++] = csi_uart_getc(ptUartBase);
		}
	}
	
	return wRecvNum;
}
/** \brief receive data to uart transmitter, assign length; this function is interrupt mode(async),
 * 
 *  \param[in] ptUartBase: UART handle to operate
 *  \param[in] pData: pointer to buffer with data to be received.
 *  \param[in] wSize: number of data to receive (byte).
 *  \param[in] wTimeOut: the timeout between bytes(ms). 
 *  \return  the num of data which is send successfully
 */
int32_t csi_uart_recv_intr(csp_uart_t *ptUartBase, void *pData, uint32_t wSize)
{
	
	uint8_t  byIdx = apt_get_uart_idx(ptUartBase);
	uint16_t hwRecvLen = ringbuffer_len(g_tUartTran[byIdx].ptRingBuf);
	
	hwRecvLen = (hwRecvLen > wSize)? wSize: hwRecvLen;
	if(hwRecvLen)
		ringbuffer_out(g_tUartTran[byIdx].ptRingBuf, pData, hwRecvLen);
		
	return hwRecvLen;
}

/** \brief receive data to uart transmitter, dynamic receive(string); this function is interrupt mode(async).
 * 
 *  \param[in] ptUartBase: UART handle to operate
 *  \param[in] pData: pointer to buffer with data to be received.
 *  \param[in] wTimeOut: the timeout between bytes(ms). 
 *  \return  the num of data which is send successfully
 */
int32_t csi_uart_recv_dynamic(csp_uart_t *ptUartBase, void *pData)
{
	
	uint8_t  byIdx = apt_get_uart_idx(ptUartBase);
	uint16_t hwRecvLen = ringbuffer_len(g_tUartTran[byIdx].ptRingBuf);
	
	if(hwRecvLen)
	{
		memcpy(pData, (void *)g_tUartTran[byIdx].ptRingBuf->pbyBuf, hwRecvLen);		//read receive data
		//ringbuffer_out(g_tUartTran[byIdx].ptRingBuf, pData, hwRecvLen);		
		ringbuffer_reset(g_tUartTran[byIdx].ptRingBuf);								//reset ringbuffer					
	}
		
	return hwRecvLen;
}


/** \brief receive data to uart transmitter, asynchronism mode
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] data: pointer to buffer with data to send to UART transmitter.
 *  \param[in] size: number of data to send (byte).
 *  \return  error code \ref csi_error_t
 */
//csi_error_t csi_uart_receive_async(csi_uart_t *uart, void *data, uint32_t size)
//{
//	csi_error_t ret;
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

//    return ret;
//}
/** \brief get the state of uart receive state
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] state: the state of uart device
 *  \return none
 */ 
csi_uart_state_e csi_uart_get_send_status(csp_uart_t *ptUartBase)
{
	uint8_t byIdx = apt_get_uart_idx(ptUartBase);
    
	return g_tUartTran[byIdx].bySendStat;
}
/** \brief get the state of uart receive state
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] state: the state of uart device
 *  \return none
 */ 
void csi_uart_clr_send_status(csp_uart_t *ptUartBase)
{
	uint8_t byIdx = apt_get_uart_idx(ptUartBase);
    
	g_tUartTran[byIdx].bySendStat = UART_STATE_IDLE;
}
/** \brief get the state of uart receive state
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] state: the state of uart device
 *  \return none
 */ 
//csi_uart_state_e csi_uart_get_recv_status(csp_uart_t *ptUartBase)
//{
//	uint8_t byIdx = apt_get_uart_idx(ptUartBase);
//    
//	return g_tUartTran[byIdx].byRecvStat;
//}
/** \brief clr the state of uart receive state
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] state: the state of uart device
 *  \return none
 */ 
//void csi_uart_clr_recv_status(csp_uart_t *ptUartBase)
//{
//	uint8_t byIdx = apt_get_uart_idx(ptUartBase);
//	g_tUartTran[byIdx].bySendStat= UART_STATE_IDLE;
//}


