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
 *  \return uart id number(0~2) or error(0xff)
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
/** \brief uart receive a bunch of data, dynamic scan
 * 
 *  \param[in] byIdx: uart id number(0~2)
 *  \return none
 */ 
void csi_uart_recv_dynamic_scan(uint8_t byIdx)
{
	if(g_tUartTran[byIdx].ptRingBuf->hwDataLen > 0)
	{
		if(g_tUartTran[byIdx].hwRxSize == g_tUartTran[byIdx].ptRingBuf->hwDataLen)
		{
			g_tUartTran[byIdx].hwRxSize = 0;
			g_tUartTran[byIdx].byRecvStat = UART_STATE_DONE;
		}
		else 
			g_tUartTran[byIdx].hwRxSize = g_tUartTran[byIdx].ptRingBuf->hwDataLen;
	}
}
/** \brief uart interrupt handle function
 * 
 *  \param[in] ptUartBas: pointer of uart register structure
 *  \param[in] byIdx: uart id number(0~2)
 *  \return none
 */ 
void apt_uart_irqhandler(csp_uart_t *ptUartBase,uint8_t byIdx)
{
	switch(csp_uart_get_isr(ptUartBase) & 0x080040)			//get RXFIFO/TXDONE interrupt
	{
		case UART_RXFIFO_INT_S:								//rx fifo interrupt; recommended use RXFIFO interrupt
		
			//if(csp_uart_get_sr(ptUartBase) & UART_RNE)	//rx fifo no empty		
			{
				//uint8_t byData = csp_uart_get_data(ptUartBase);
				//ringbuffer_byte_in(g_tUartTran[byIdx].ptRingBuf, byData);
				if(g_tUartTran[byIdx].ptRingBuf->hwDataLen < g_tUartTran[byIdx].ptRingBuf->hwSize)	//the same as previous line of code 
				{
					g_tUartTran[byIdx].ptRingBuf->pbyBuf[g_tUartTran[byIdx].ptRingBuf->hwWrite] = csp_uart_get_data(ptUartBase);
					g_tUartTran[byIdx].ptRingBuf->hwWrite = (g_tUartTran[byIdx].ptRingBuf->hwWrite + 1) % g_tUartTran[byIdx].ptRingBuf->hwSize;
					g_tUartTran[byIdx].ptRingBuf->hwDataLen ++;
				}
			}
			break;
		case UART_TXDONE_INT_S:													//tx send complete; recommended use TXDONE interrupt
		
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
/** \brief initialize uart parameter structure
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
	uint8_t byIdx;
	
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
	
	//get uart rx/tx mode 
	byIdx = apt_get_uart_idx(ptUartBase);
	g_tUartTran[byIdx].byRecvMode = ptUartCfg->byRxMode;			
	g_tUartTran[byIdx].bySendMode = ptUartCfg->byTxMode;
	
	//uart databits = 8 and stopbits = 1; fixed, can not be configured 
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
void csi_uart_start(csp_uart_t *ptUartBase)
{
	s_wCtrlRegBack |= ((UART_TX_EN << UART_TX_POS) | (UART_RX_EN << UART_RX_POS));
	csp_uart_set_ctrl(ptUartBase, s_wCtrlRegBack);		//enable uart rx/tx 
}
/** \brief stop(disable) uart rx/tx
 * 
 *  \param[in] ptUartBase: pointer of uart register structure
 *  \return error code \ref csi_error_t
 */ 
void csi_uart_stop(csp_uart_t *ptUartBase)
{
	s_wCtrlRegBack &= (~UART_TX_MSK | ~UART_RX_MSK);
	csp_uart_set_ctrl(ptUartBase, s_wCtrlRegBack);		//disable uart rx/tx 
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
 *  \param[in] uart: pointer of uart register structure
 *  \return  the character to get
 */
uint8_t csi_uart_getc(csp_uart_t *ptUartBase)
{
	while(!(csp_uart_get_sr(ptUartBase) & UART_RNE));
	return csp_uart_get_data(ptUartBase);
}
/** \brief send data from uart, this function is polling(sync mode).
 * 
 *  \param[in] ptUartBase: pointer of uart register structure
 *  \param[in] pData: pointer to buffer with data to send to uart transmitter.
 *  \param[in] hwSize: number of data to send (byte).
 *  \param[in] wTimeOut: the timeout between bytes(ms), unit: ms 
 *  \return  the num of data which is send successfully or CSI_ERROR/CSI_OK
 */
int16_t csi_uart_send(csp_uart_t *ptUartBase, const void *pData, uint16_t hwSize)
{
	int32_t i; 
	uint8_t *pbySend = (uint8_t *)pData;
	uint8_t byIdx = apt_get_uart_idx(ptUartBase);
	
	if(NULL == pData || 0 == hwSize)
		return 0;
	
	switch(g_tUartTran[byIdx].bySendMode)
	{
		case UART_TX_MODE_POLL:						//return the num of data which is send							
			for(i = 0; i < hwSize; i++)
			{
				while((csp_uart_get_sr(ptUartBase) & UART_TX_FULL));			
				csp_uart_set_data(ptUartBase, *(pbySend+i));
			}
			return i;
			
		case UART_TX_MODE_INT:						//return CSI_ERROR or CSI_OK
			if(hwSize == 0 || byIdx >= UART_IDX_NUM) 
				return CSI_ERROR;
				
			g_tUartTran[byIdx].pbyTxData =(uint8_t *)pData;
			g_tUartTran[byIdx].hwTxSize = hwSize;
	
			if(g_tUartTran[byIdx].bySendStat == UART_STATE_SEND)				//uart sending?
				return CSI_ERROR;
			else
			{
				g_tUartTran[byIdx].bySendStat = UART_STATE_SEND;				//set uart send status, sending
				csp_uart_set_data(ptUartBase, *g_tUartTran[byIdx].pbyTxData);	//start uart tx,send first byte
			}
			return CSI_OK;
			
		default:
			return CSI_ERROR;
	}
}

/** \brief send data from uart, this function is interrupt mode(async mode)
 * 
 *  \param[in] ptUartBase: pointer of uart register structure
 *  \param[in] pData: pointer to buffer with data to send to uart transmitter.
 *  \param[in] hwSize: number of data to send (byte).
 *  \return  error code \ref csi_error_t
 */
csi_error_t csi_uart_send_async(csp_uart_t *ptUartBase, const void *pData, uint16_t hwSize)
{
	csi_error_t ret = CSI_ERROR;
	uint8_t byIdx = apt_get_uart_idx(ptUartBase);

	if(hwSize == 0 || NULL == pData) 
		return CSI_ERROR;
	
	g_tUartTran[byIdx].pbyTxData =(uint8_t *)pData;
	g_tUartTran[byIdx].hwTxSize = hwSize;
	
	if(g_tUartTran[byIdx].bySendStat == UART_STATE_SEND)					//uart sending?
		ret = CSI_ERROR;
	else
	{
		g_tUartTran[byIdx].bySendStat = UART_STATE_SEND;					//set uart send status, sending
		csp_uart_set_data(ptUartBase, *g_tUartTran[byIdx].pbyTxData);		//start uart tx,send first byte
	}
	
	return ret;
}
/** \brief receive data from uart, this function is polling(sync).
 * 
 *  \param[in] ptUartBase: pointer of uart register structure
 *  \param[in] pData: pointer to buffer with data to be received.
 *  \param[in] hwSize: number of data to receive (byte).
 *  \param[in] wTimeOut: the timeout between bytes(ms), unit: ms 
 *  \return  the num of data which is receive successfully
 */
int16_t csi_uart_receive(csp_uart_t *ptUartBase, void *pData, uint16_t hwSize, uint32_t wTimeOut)
{
	uint8_t *pbyRecv = (uint8_t *)pData;
	uint8_t byIdx = apt_get_uart_idx(ptUartBase);
	int16_t hwRecvNum = 0;
	
	if(NULL == pData)
		return 0;
	
	switch(g_tUartTran[byIdx].byRecvMode)
	{
		case UART_RX_MODE_POLL:
			if(wTimeOut)				//handle with wTimeOut
			{
				uint32_t wRecvStart = csi_tick_get_ms();	
				while(hwRecvNum < hwSize)
				{
					while(!(csp_uart_get_sr(ptUartBase) & UART_RNE))			//fifo empty? wait	
					{
						if((csi_tick_get_ms() - wRecvStart) >= wTimeOut) 
							return hwRecvNum;
					}
					pbyRecv[hwRecvNum ++] = csp_uart_get_data(ptUartBase);
					wRecvStart = csi_tick_get_ms();	
				}
			}
			else							//handle without wTimeOut
			{
				while(hwRecvNum < hwSize)
				{
					while(!(csp_uart_get_sr(ptUartBase) & UART_RNE));			//fifo empty? wait	
					pbyRecv[hwRecvNum ++] = csi_uart_getc(ptUartBase);
				}
			}
			
			break;
		case UART_RX_MODE_INT_FIX:			//receive assign length data, handle without wTimeOut
			
			//read ringbuffer, multiple processing methods 
			//allow users to modify 
			hwRecvNum = ringbuffer_len(g_tUartTran[byIdx].ptRingBuf);
			switch(hwSize)
			{
				case 0:						
					return 0;
				case 1:						//single byte receive(read)
					if(hwRecvNum)
						hwRecvNum = ringbuffer_byte_out(g_tUartTran[byIdx].ptRingBuf, pData);
					else
						hwRecvNum = 0;
					break;
				default:					//Multibyte  receive(read)
					if(hwRecvNum >= hwSize)
						hwRecvNum = ringbuffer_out(g_tUartTran[byIdx].ptRingBuf, pData, hwSize);
					else
						hwRecvNum = 0;
					break;
			}
			
//			if(hwRecvNum >= hwSize)
//				ringbuffer_out(g_tUartTran[byIdx].ptRingBuf, pData, hwRecvNum);
//			else
//				hwRecvNum = 0;

//			hwRecvNum = (hwRecvNum > hwSize)? hwSize: hwRecvNum;
//			if(hwRecvNum)
//				ringbuffer_out(g_tUartTran[byIdx].ptRingBuf, pData, hwRecvNum);
				
			break;
		case UART_RX_MODE_INT_DYN:			//receive dynamic length data, handle without (wTimeOut and hwSize)
			 hwRecvNum = ringbuffer_len(g_tUartTran[byIdx].ptRingBuf);
			if(hwRecvNum)
			{
				memcpy(pData, (void *)g_tUartTran[byIdx].ptRingBuf->pbyBuf, hwRecvNum);		//read receive data
				//ringbuffer_out(g_tUartTran[byIdx].ptRingBuf, pData, hwRecvNum);			//the same as previous line of code 
				ringbuffer_reset(g_tUartTran[byIdx].ptRingBuf);								//reset ringbuffer	
				g_tUartTran[byIdx].byRecvStat = UART_STATE_IDLE;							//set uart receive status for idle				
			}
			break;
		default:
			hwRecvNum = 0;
			break;
	}
	
	return hwRecvNum;
}
/** \brief receive data from uart , assign length; this function is interrupt mode(async),
 * 
 *  \param[in] ptUartBase: pointer of uart register structure
 *  \param[in] pData: pointer to buffer with data to be received.
 *  \param[in] hwSize: number of data to receive (byte).
 *  \return  the num of data which is receive successfully
 */
int16_t csi_uart_recv_async(csp_uart_t *ptUartBase, void *pData, uint16_t hwSize)
{
	uint8_t  byIdx = apt_get_uart_idx(ptUartBase);
	uint16_t hwRecvNum = ringbuffer_len(g_tUartTran[byIdx].ptRingBuf);
	
	if(NULL == pData)
		return 0;
		
	switch(hwSize)
	{
		case 0: 	
			return 0;
		case 1:					//single byte receive(read)
			if(hwRecvNum)
				hwRecvNum = ringbuffer_byte_out(g_tUartTran[byIdx].ptRingBuf, pData);
			else
				hwRecvNum = 0;
			break;
		default:				//Multibyte  receive(read)
			if(hwRecvNum >= hwSize)
				hwRecvNum = ringbuffer_out(g_tUartTran[byIdx].ptRingBuf, pData, hwSize);
			else
				hwRecvNum = 0;
			break;
	}
		
//	hwRecvNum = (hwRecvNum > hwSize)? hwSize: hwRecvNum;
//	if(hwRecvNum)
//		ringbuffer_out(g_tUartTran[byIdx].ptRingBuf, pData, hwRecvNum);
		
	return hwRecvNum;
}

/** \brief receive data from uart, dynamic length receive; this function is interrupt mode(async).
 * 
 *  \param[in] ptUartBase: pointer of uart register structure
 *  \param[in] pData: pointer to buffer with data to be received.
 *  \param[in] wTimeOut: the timeout between bytes(ms). 
 *  \return  the num of data which is receive successfully
 */
int16_t csi_uart_recv_dynamic(csp_uart_t *ptUartBase, void *pData)
{
	
	uint8_t  byIdx = apt_get_uart_idx(ptUartBase);
	uint16_t hwRecvNum = ringbuffer_len(g_tUartTran[byIdx].ptRingBuf);
	
	if(hwRecvNum)
	{
		memcpy(pData, (void *)g_tUartTran[byIdx].ptRingBuf->pbyBuf, hwRecvNum);		//read receive data
		//ringbuffer_out(g_tUartTran[byIdx].ptRingBuf, pData, hwRecvNum);			//the same as previous line of code 
		ringbuffer_reset(g_tUartTran[byIdx].ptRingBuf);								//reset ringbuffer para
		g_tUartTran[byIdx].byRecvStat = UART_STATE_IDLE;							//set uart receive status for idle
	}
		
	return hwRecvNum;
}
/** \brief get the status of uart send 
 * 
 *  \param[in] uart: pointer of uart register structure
 *  \return he status of uart send
 */ 
csi_uart_state_e csi_uart_get_send_status(csp_uart_t *ptUartBase)
{
	uint8_t byIdx = apt_get_uart_idx(ptUartBase);
    
	return g_tUartTran[byIdx].bySendStat;
}
/** \brief clr the status of uart send
 * 
 *  \param[in] uart: pointer of uart register structure
 *  \return none
 */ 
void csi_uart_clr_send_status(csp_uart_t *ptUartBase)
{
	uint8_t byIdx = apt_get_uart_idx(ptUartBase);
    
	g_tUartTran[byIdx].bySendStat = UART_STATE_IDLE;
}
/** \brief get the status of uart receive 
 * 
 *  \param[in] uart: pointer of uart register structure
 *  \return the status of uart receive 
 */ 
csi_uart_state_e csi_uart_get_recv_status(csp_uart_t *ptUartBase)
{
	uint8_t byIdx = apt_get_uart_idx(ptUartBase);
	return g_tUartTran[byIdx].byRecvStat;
}
/** \brief clr the status of uart receive 
 * 
 *  \param[in] uart: pointer of uart register structure
 *  \return none
 */ 
void csi_uart_clr_recv_status(csp_uart_t *ptUartBase)
{
	uint8_t byIdx = apt_get_uart_idx(ptUartBase);
	g_tUartTran[byIdx].byRecvStat= UART_STATE_IDLE;
}


