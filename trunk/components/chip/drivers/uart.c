/***********************************************************************//** 
 * \file  uart.c
 * \brief  csi uart driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-12 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-22 <td>V0.1  <td>ZJY   <td> support event UART_EVENT_RECEIVE_FIFO_READABLE on 110
 * <tr><td> 2021-1-22 <td>V0.2  <td>WNN   <td> addapted to 102
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
#include <drv/ringbuffer.h>
#include "csp.h"

/* Private macro-----------------------------------------------------------*/
#define UART_TIMEOUT    	0x10000000U
#define UART_IDX_NUM   		3
#define UART_RECV_MAX_LEN	128


volatile uint32_t uartctrlreg[UART_IDX_MASK+1];

#define CSP_UART_CTRL_REG_SET(ut,idx,val) (uartctrlreg[idx] = (val),(ut->CTRL = (val)))
#define CSP_UART_CTRL_REG_GET(ut,idx) (uartctrlreg[(idx)& UART_IDX_MASK])

/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
//static	uint32_t s_wRecvLen[UART_IDX_NUM]	 = {0, 0, 0};			
//static	uint32_t s_wPreRecvLen[UART_IDX_NUM] = {0, 0, 0};

csi_uart_devlist_t uartDevlist;


/** \brief initialize uart data structure
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] port_idx: uart index
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_uart_init(csi_uart_t *uart, uint32_t idx)
{
	csi_error_t ret=0;
	//CSI_PARAM_CHK(uart, CSI_ERROR);
	if(idx>=NUM_OF_UART) return CSI_ERR_UARTNO;
	/* link uart control to devbuf */
	uart->utNo = idx;
	uartDevlist.ut[idx] = uart;
    uint8_t no = uart->utNo;
	
	/* link ut to real peripheral */
	switch(idx){
		case 0:
			uart->utbase =UART0;
			break;
		case 1:
			uart->utbase =UART1;
			break;
		case 2:
			uart->utbase =UART2;
			break;
		default:
			break;
	}
	csp_uart_t * ut = uart->utbase;
    if(ut ==NULL) return CSI_ERR_UARTBASE;	
	
	/* uart peripheral clk enable */
	SYSCON->PCER0 = 0x1<< (8+idx);
	
	/* calculate baudrate divder param */
	uint32_t pclk = soc_get_pclk_freq();
	uint32_t baud = ((uart->BaudRate)&0xFFFFF);
	if(baud==0){
		uart->errcode =CSI_ERR_UARTPARAM;
		return CSI_ERR_UARTPARAM;
	}
	uint32_t bauddiv = pclk/baud;
	
	/* get parity setting */
	uart_ctrl_e parity =0;
	switch(uart->Parity)
	{
		case UART_PARITY_NONE:
			parity = UART_PARI_NONE;
			break;
		case UART_PARITY_EVEN:
			parity = UART_PARI_EVEN;
		case UART_PARITY_ODD:
		    parity = UART_PARI_ODD;
			break;
		default:
			parity= ((uart->Parity)&0x7)<<8;
			break;
	}
	
	switch(parity)
	{
		case UART_PARI_NONE:
		case UART_PARI_EVEN:
		case UART_PARI_ODD:
		case UART_PARI_ZERO:
		case UART_PARI_ONE:
			break;
		default:
			uart->errcode =CSI_ERR_UARTPARAM;
			return CSI_ERR_UARTPARAM;
			break;
	}
	
	/* init interrupt setting */
	volatile uint32_t intvalue=0;
	uint32_t tempvalue = uart->Interrupt;
	tempvalue = tempvalue & 0x800FF;
	intvalue = 	((tempvalue & 0xF)<<2) | ((tempvalue & 0x10)<<3) | ((tempvalue & 0x60)<<7) |((tempvalue & 0x80)<<11) | ((tempvalue & 0x80000));
	
	/* change uart registers */
	ut->DATA	= 0;  	
	ut->SR		= 0x1ff;   
	ut->CTRL	= 0;   
	ut->ISR		= 0xFFFFFFFF;
	ut->BRDIV	= bauddiv;
	uart->errcode =0;
	uart->state = UART_IDLE;
	
	/* wkmode configraion  */
	switch(uart->wkmode){
		case UART_WK_MD0:
			intvalue = intvalue & 0xBC;
			CSP_UART_CTRL_REG_SET(ut,no,intvalue|parity|RXIFLSEL_1_8);
			if(intvalue){
				NVIC_EnableIRQ(UART0_IRQn+no);
			}
			break;
		case UART_WK_MD1:
			intvalue = intvalue & 0x93080;
			CSP_UART_CTRL_REG_SET(ut,no,intvalue|parity|UART_FIFO_EN|RXIFLSEL_1_2);
			if(intvalue){
				NVIC_EnableIRQ(UART0_IRQn+no);
			}
			break;
		case UART_WK_MD2:
			break;
		case UART_WK_MD3:
			break;
		default:
			break;
	}
	return ret;
}


/** \brief de-initialize uart interface
 * 
 *  \param[in] uart: UART handle to operate
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_uart_uninit(csi_uart_t *uart)
{
	csi_error_t ret=0;
	CSI_PARAM_CHK(uart, CSI_ERROR);
	ret = csi_uart_close(uart);
	return ret;
}

/** \brief uart open
 * 
 *  \param[in] uart: UART handle to operate
 *   \return error code \ref csi_error_t
 */
csi_error_t csi_uart_open(csi_uart_t *uart)
{
	csi_error_t ret=0;
	csp_uart_t * ut = uart->utbase;
	uint8_t no = uart->utNo;
	
	switch(uart->wkmode){
		case UART_WK_MD0:
			CSP_UART_CTRL_REG_SET(ut,no,CSP_UART_CTRL_REG_GET(ut,no)|(UART_TXEN | UART_RXEN));
			uart->state = UART_RW;
			break;
		case UART_WK_MD1:
			break;
		case UART_WK_MD2:
			break;
		case UART_WK_MD3:
			break;
		default:
			break;
	}
	return ret;
}


/** \brief uart close
 * 
 *  \param[in] uart: UART handle to operate
 *   \return error code \ref csi_error_t
 */
csi_error_t csi_uart_close(csi_uart_t *uart)
{
	csi_error_t ret=0;
	csp_uart_t * ut = uart->utbase;
	uint8_t no = uart->utNo;
	
	switch(uart->wkmode){
		case UART_WK_MD0:
			CSP_UART_CTRL_REG_SET(ut,no,CSP_UART_CTRL_REG_GET(ut,no)&(~(UART_TXEN | UART_RXEN)));
			uart->state = UART_STOP;
			break;
		case UART_WK_MD1:
			break;
		case UART_WK_MD2:
			break;
		case UART_WK_MD3:
			break;
		default:
			break;
	}
	return ret;

}


/** \brief uart send character
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] ch: the character to be send
 *  \param[in] blocking: blocking mode swtich
 *  \return  none
 */
void csi_uartmd_putc(csi_uart_t *uart, uint8_t ch,uint8_t blocking)
{
	csp_uart_t * ut = uart->utbase;
	
	if(blocking){
		switch(uart->wkmode){
			case UART_WK_MD0:
				// Wait for the send buf available
				while(((ut->SR) & UART_TX_FULL)) {};
				// Write the transmit buffer
				ut->DATA = ch;
				break;		

			case UART_WK_MD1:			
				break;
			case UART_WK_MD2:
				break;
			case UART_WK_MD3:
				break;
			default:
				break;
		}	

	}
	else{
		switch(uart->wkmode){
			case UART_WK_MD0:
				// Write the transmit buffer
				ut->DATA = ch;
				break;		

			case UART_WK_MD1:			
				break;
			case UART_WK_MD2:
				break;
			case UART_WK_MD3:
				break;
			default:
				break;
		}
	}

}

/** \brief uart get character
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] blocking: blocking mode swtich
 *  \return  the character to get
 */
uint8_t csi_uartmd_getc(csi_uart_t *uart,uint8_t blocking)
{
	uint8_t ret=0;
	csp_uart_t * ut = uart->utbase;
	if(blocking){
		switch(uart->wkmode){
			case UART_WK_MD0:
				while((ut->SR & UART_RX_FULL)==0x0){};
				ret = ut->DATA;				
				break;
			case UART_WK_MD1:
				break;
			case UART_WK_MD2:
				break;
			case UART_WK_MD3:
				break;
			default:
				break;
		}
	}
	else{
		switch(uart->wkmode){
			case UART_WK_MD0:
				ret = ut->DATA;			
				break;
			case UART_WK_MD1:
				break;
			case UART_WK_MD2:
				break;
			case UART_WK_MD3:
				break;
			default:
				break;
		}
	}
	return ret;
}


/**
  \brief       Get character in query mode.
  \param[in]   uart  uart handle to operate.
  \return      the character to get.
*/

uint8_t  csi_uart_getc(csi_uart_t *uart){
	return csi_uartmd_getc(uart,0);
}

/**
  \brief       Send character in query mode.
  \param[in]   uart uart handle to operate.
  \param[in]   ch   the character to be send.
*/
void csi_uart_putc(csi_uart_t *uart, uint8_t ch){
	csi_uartmd_putc(uart,ch,1);
}


/** \brief send data to uart transmitter, this function is blocking.
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] data: pointer to buffer with data to send to UART transmitter.
 *  \param[in] size: number of data to send (byte).
 *  \param[in] timeout: the timeout between bytes(ms). DO NOT support now
 *  \return  the num of data which is sent successfully or CSI_ERROR
 */
int32_t csi_uart_send(csi_uart_t *uart, const void *data, uint32_t size, uint32_t timeout)
{
	int32_t ret=0;
	uint32_t timecnt =0;
	uint8_t *buf = (uint8_t *)data;
	switch(uart->wkmode){
		case UART_WK_MD0:
			for(int i=0; i<size; i++) {				
				csi_uartmd_putc(uart,buf[i],1);
				timecnt++;
				if(timecnt>=timeout){
					uart->errcode = CSI_TIMEOUT;
					return i;
				}
			}
			ret = size;		
			break;
		case UART_WK_MD1:
			break;
		case UART_WK_MD2:
			break;
		case UART_WK_MD3:
			break;
		default:
			break;
	}
	return ret;	
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
	csi_error_t ret=CSI_OK;
	csp_uart_t * ut = uart->utbase;
	uint8_t *buf = (uint8_t *)data;
	switch(uart->wkmode){
		case UART_WK_MD0:
			for(int i=0; i<size; i++) {				
				csi_uartmd_putc(uart,buf[i],0);
			}
			ret = size;		
			break;
		case UART_WK_MD1:
			break;
		case UART_WK_MD2:
			break;
		case UART_WK_MD3:
			break;
		default:
			break;
	}	
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
	int32_t ret=0;	
	uint32_t timecnt =0;
	uint8_t *buf = data;
	switch(uart->wkmode){
		case UART_WK_MD0:
			for(int i=0; i<size; i++) {				
				buf[i] = csi_uartmd_getc(uart,1);
				timecnt++;
				if(timecnt>=timeout){
					uart->errcode = CSI_TIMEOUT;
					return i;
				}
			}
			ret = size;	
			break;
		case UART_WK_MD1:
			break;
		case UART_WK_MD2:
			break;
		case UART_WK_MD3:
			break;
		default:
			break;
	}
	return ret;
 }
 
/** \brief receive data to uart transmitter, asynchronism mode
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] data: pointer to buffer with data to send to UART transmitter.
 *  \param[in] size: number of data need receive (byte).
 *  \return    data len.
*/
csi_error_t csi_uart_receive_async(csi_uart_t *uart, void *data, uint32_t size)
{
		csi_error_t ret =0;
	uint32_t datalen =0;
	csp_uart_t * ut = uart->utbase;
	uint8_t *buf = data;
	switch(uart->wkmode){
		case UART_WK_MD0:			
			datalen = ringbuffer_len(uart->ringbuf);
			
			if(datalen > size)
				datalen = size;
			
			if(datalen)
				ringbuffer_out(uart->ringbuf,buf,size);
			else
				ret = CSI_ERR_UARTRXEMPTY;
				
			break;
		case UART_WK_MD1:
			break;
		case UART_WK_MD2:
			break;
		case UART_WK_MD3:
			break;
		default:
			break;
	}
	return ret;
}


/** \brief get the state of uart device
 * 
 *  \param[in] uart: UART handle to operate
 *  \param[in] state: the state of uart device
 *  \return none
 */ 
csi_error_t csi_uart_get_state(csi_uart_t *uart, csi_uart_state_t *state)
{
	csi_error_t ret=0;
	state[0] = uart->state;
	ret = uart->errcode;
	return ret;
}


/**
  \brief       Enable uart power manage.
  \param[in]   uart   uart handle to operate.
  \param[in]   param  pointer to param for power management.
  \return      error code.
*/
csi_error_t csi_uart_pm(csi_uart_t *uart, uint8_t *param){
	csi_error_t ret=0;
	uint8_t no= uart->utNo;	
	switch(no){
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		default:
			break;
	}
	return ret;
	
}

/**
  \brief       Enable uart power manage.
  \param[in]   uart   uart handle to operate.
  \param[in]   param  pointer to param for performance control.
  \return      error code.
*/
csi_error_t csi_uart_performance_control(csi_uart_t *uart, uint8_t *param){
	csi_error_t ret=0;
	uint8_t no= uart->utNo;		
	switch(no){
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		default:
			break;
	}
	return ret;
}

/** \brief get uart interrupt isr value
 * 
 *  \param[in] UART handle to operate
 *  \return uart isr reg value
 */ 
uint32_t csi_uart_get_isr(csi_uart_t *uart)
{
	uint32_t ret =0;
	switch(uart->utNo){
		case 0:
			ret = UART0->ISR;
			break;
		case 1:
			ret = UART1->ISR;
			break;
		case 2:
			ret = UART2->ISR;
			break;		
		default:
			break;
	}
	return ret;
}

/** \brief clr uart interrupt isr flag
 * 
 *  \param[in] UART handle to operate
 *  \param[in] UART isr bits
 *  \return none
 */
void csi_uart_clr_isr(csi_uart_t *uart,uint32_t value)
{
	switch(uart->utNo){
		case 0:
			UART0->ISR = value;
			break;
		case 1:
			UART1->ISR = value;
			break;
		case 2:
			UART2->ISR = value;
			break;		
		default:
			break;
	}
}


/** \brief uart interrupt event generation 
 * 
 *  \param[in] UART handle to operate
 *  \return none
 */ 
void csi_uart_creat_event_default(csi_uart_t *uart)
{
	uint32_t wIntState = csi_uart_get_isr(uart);
	uint32_t maskremap[9]={UART_RX_INT_FLG,UART_RX_FIFO_FLG,0x1<<1,0x1<<2,0x1<<3,0x1<<4,0x1<<5,0x1<<7,0x1<<19};
	int i = 0;
	uint32_t bitmask;
	uart->Event =0;
	while(wIntState>0){
		bitmask = maskremap[i];		
		if(wIntState&bitmask){
			switch(bitmask){
				case UART_RX_FIFO_FLG:	
					uart->Event = (0x1<< UART_EVENT_RECEIVE_FIFO_READABLE);
					break;
				case UART_RX_INT_FLG:
					uart->Event = (0x1<< UART_EVENT_RECEIVE_COMPLETE);
					break;
				case UART_TX_INT_FLG:				        
				case UART_TX_IOV_FLG:
				case UART_RX_IOV_FLG:  
				case UART_PAR_ERR_FLG:
				case UART_TX_FIFO_FLG:
					break;
				case UART_RXFIFO_IOV_FLG:
				case UART_TX_DONE_FLG:
					break;			
				default:
					break;
			}
			wIntState &= (~bitmask);
			csi_uart_clr_isr(uart,bitmask);
			UART1->ISR =0x2;
		}
		i++;	
	}
}

/** \brief uart interrupt event handler default
 * 
 *  \param[in] UART handle to operate
 *  \return none
 */ 
void csi_uart_handle_event_default(csi_uart_t *uart, csi_uart_event_t event)
{
    #define  TST_IO_RCV_BUF_LEN 8
    uint8_t  temp_buf[TST_IO_RCV_BUF_LEN];	
	uint32_t evtbuf=0;
	csp_uart_t * ut = uart->utbase;
	uint8_t fifolen = ((CSP_UART_CTRL_REG_GET(ut,uart->utNo)>>14) & 0x7);
	evtbuf = uart->Event;
	event =0;
	while(evtbuf>0){
		if(evtbuf&0x1){
			switch (event)
			{
				case UART_EVENT_RECEIVE_COMPLETE:
					temp_buf[0] = ut->DATA;
					ringbuffer_in(uart->ringbuf,temp_buf, 1);
					break;
				case UART_EVENT_RECEIVE_FIFO_READABLE:
					switch(fifolen){
						case 1:
						case 2:
						case 4:
							for(int i=0; i<fifolen; i++) {				
								temp_buf[i] = ut->DATA;
							}
							/* write data to io buf */
							ringbuffer_in(uart->ringbuf,temp_buf,fifolen);	
							break;
						default:
							break;
					}
					break;
				case UART_EVENT_ERROR_OVERFLOW:
				case UART_EVENT_ERROR_PARITY:
				case UART_EVENT_ERROR_FRAMING:
					break;
				default:
					break;
			}		
		}
		evtbuf>>=1;
		event++;
	}
	uart->Event =0;
}

/** \brief uart interrupt handle function
 * 
 *  \param[in] UART handle to operate
 *  \param[in] devno: dev of uart
 *  \return none
 */ 
void csi_uart_irqhandler(uint8_t devno)
{	
	csi_uart_t *uart = uartDevlist.ut[devno];
	uart->EventGen_callback(uart);
	uart->Event_callback(uart);
}

/**
  \brief       Attach the interrupt callback handler to UART.
  \param[in]   uart       operate handle.
  \param[in]   evtcreat   event creat function.
  \param[in]   evtcallback   event callback function.
  \param[in]   arg        user can define it by himself as callback's param.
  \return      error code.
*/
csi_error_t csi_uart_attach_int_callback(csi_uart_t *uart, void *evtcreat, void *evtcallback,void *arg){
	if(evtcreat==NULL || evtcallback ==NULL) return CSI_ERR_UARTPARAM;
	uart->EventGen_callback	= evtcreat;
	uart->Event_callback	= evtcallback;
	uart->eventArg = arg;
	return CSI_OK;	
}
