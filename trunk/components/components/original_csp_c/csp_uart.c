/***********************************************************************//** 
 * \file  csp_uart.c
 * \brief  UART description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-20 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-22 <td>V0.1  <td>WNN   <td>delete dma related
 * </table>
 * *********************************************************************
*/

/* Includes ---------------------------------------------------------------*/
#include "csp_uart.h"
#include "csp_common.h"
#include "csp_etb.h"
#include "csp.h"

/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
volatile uint8_t UartRxBuf[UART_BUF_SIZE+4];


/** \brief uart reg default init
 * 
 *  \param[in] ptUartBase: pointer of UART reg structure.
 *  \return none
 */  
void csp_uart_default_init(csp_uart_t *ptUartBase)
{
	//ptUartBase->DATA	= UART_RESET_VALUE;  	
	ptUartBase->SR		= 0x1c;   
	ptUartBase->CTRL	= UART_RESET_VALUE;   
	ptUartBase->ISR		= 0x800FF;
	ptUartBase->BRDIV	= UART_RESET_VALUE;   
}
/** \brief uart ctrl Register init
 * 
 *  \param[in] ptUartBase: pointer of UART reg structure.
 *  \param[in] eTxCtrl: tx enable,disable
 *  \param[in] eRxCtrl: rx enable,disable
 *  \param[in] eParity: parity
 *  \return none
 */
void csp_uart_ctrl_init(csp_uart_t *ptUartBase, tx_ctrl_e eTxCtrl, rx_ctrl_e eRxCtrl,uart_parity_e eParity, uart_int_e eUartInt)
{
	ptUartBase->CTRL = eTxCtrl | (eRxCtrl << UART_RX_POS) | (UART_FIFO_EN << UART_FIFO_POS); 
	ptUartBase->CTRL |= (eParity << UART_PARITY_POS) | (FIFO_RX_1_8 << UART_RXFIFO_POS) | eUartInt;	
}

/** \brief uart interrupt enable/disable
 * 
 *  \param[in] ptUartBase: pointer of UART reg structure.
 *  \param[in] eUartInt: interrupt number
 *  \param[in] bEnable: ENABLE, DISABLE
 *  \return none
 */
void csp_uart_set_int(csp_uart_t *ptUartBase,uart_int_e eUartInt,bool bEnable)
{
	if(bEnable)
		ptUartBase->CTRL |= eUartInt;
	else
		ptUartBase->CTRL &= ~eUartInt;
}
/** \brief set uart baudrate
 * 
 *  \param[in] ptUartBase: pointer of UART reg structure.
 *  \param[in] wBaud: baudrate
 *  \param[in] wUartFreq: uart clk
 *  \return none
 */
void csp_uart_set_baudrate(csp_uart_t *ptUartBase,uint32_t wBaud, uint32_t wUartFreq)
{
	uint32_t wBrDiv = (wUartFreq/wBaud);
	if(wBrDiv < 16)
		wBrDiv = 16;
	ptUartBase->BRDIV = wBrDiv;
}
/** \brief uart send ready?
 * 
 *  \param[in] ptUartBase: pointer of UART reg structure.
 *  \return ready, TRUE; no ready, FALSE
 */ 
uint8_t csp_uart_put_ready(csp_uart_t *ptUartBase)
{
	if(csp_uart_get_sr(ptUartBase) & UART_TX_FULL)
		return FALSE;			//busy
	else
		return TRUE;			//idle
}

/** \brief uart receive date ready?
 * 
 *  \param[in] ptUartBase: pointer of UART reg structure.
 *  \return ready, TRUE; no ready, FALSE
 */ 
uint8_t csp_uart_get_ready(csp_uart_t *ptUartBase)
{
	if(csp_uart_get_sr(ptUartBase) & UART_RX_FULL)
		return TRUE;			//buff full
	else
		return FALSE;			//buff empty
}


/** \brief uart receive date fifo ready?
 * 
 *  \param[in] ptUartBase: pointer of UART reg structure.
 *  \return ready, TRUE; no ready, FALSE
 */ 
uint8_t csp_uart_fifo_get_ready(csp_uart_t *ptUartBase)
{
	if(csp_uart_get_sr(ptUartBase) & UART_RNE)
		return TRUE;			//fifo no empty
	else
		return FALSE;			//fifo empty
}

/** \brief uart put char
 * 
 *  \param[in] ptUartBase: pointer of UART reg structure.
 *  \param[in] byByte: data of send
 *  \return none
 */ 
void csp_uart_put_char(csp_uart_t *ptUartBase,uint8_t byByte)
{
	csp_uart_set_data(ptUartBase, byByte);				//Write the transmit buffer
	
	while(!csp_uart_put_ready(ptUartBase));				//Wait for the transmit done
	
	csp_uart_clr_isr(ptUartBase,UART_TX_INT_S);			//Clear status bit for the next transmit
}

/** \brief uart put char
 * 
 *  \param[in] ptUartBase: pointer of UART reg structure.
 *  \param[in] byByte: data of send
 *  \return none
 */ 
uint8_t csp_uart_get_char(csp_uart_t *ptUartBase)
{
	
	while(!csp_uart_get_ready(ptUartBase));
	
	return csp_uart_get_data(ptUartBase);		//(uint8_t)ptUartBase->DATA;
}

/** \brief send data of uart transmitter, this function is blocking.
 * 
 *  \param[in] uart: pointer of UART reg structure.
 *  \param[in] data: pointer to buffer data to send to UART transmitter.
 *  \param[in] size: number of data to send (byte).
 *  \return  error code \ref csi_error_t
 */
uint32_t csp_uart_send(csp_uart_t *ptUartBase, uint8_t *byPdata, uint32_t size)
{
	uint32_t i,j = UART_TX_TIMEOUT;
	
	for(i = 0; i < size; i++)
	{	
		while(!csp_uart_put_ready(ptUartBase) && j --)
		csp_uart_clr_isr(ptUartBase,UART_TX_INT_S);		//Clear status bit for the next transmit	
		
		if(j == 0)
			break;
		else
			csp_uart_set_data(ptUartBase, *(byPdata+i));
	}
	return i;
}
/** \brief uart vic int enbale
 * 
 *  \param[in] ptUartBase: pointer of UART reg structure.
 *  \return none
 */ 
void csp_uart_vic_irq_en(csp_uart_t *ptUartBase)
{
	irqn_type_e eIrqNum;
	switch((uint32_t)ptUartBase)
	{
		case APB_UART0_BASE:
			eIrqNum = UART0_IRQn;
			break;
		case APB_UART1_BASE:
			eIrqNum = UART1_IRQn;
			break;
		case APB_UART2_BASE:
			eIrqNum = UART2_IRQn;
			break;
		default:
			eIrqNum = UART0_IRQn;
			break;
	}
	
	ptUartBase->ISR = 0x0f;
	NVIC_EnableIRQ(eIrqNum);
}

