/***********************************************************************//** 
 * \file  csp_uart.h
 * \brief  UART description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-20 <td>V0.0  <td>ZJY   <td>initial
 * </table>
 * *********************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CSP_UART_H
#define _CSP_UART_H

/* Includes ------------------------------------------------------------------*/
#include <soc.h>

/// \struct csp_uart_t
/// \brief UART reg description
typedef struct
{
    __IOM  uint32_t  DATA;		//0x0000	Write and Read Data Register  
    __IOM  uint32_t  SR;        //0x0004 	Status Register               
    __IOM  uint32_t  CTRL;      //0x0008 	Control Register              
    __IOM  uint32_t  ISR;       //0x000C 	Interrupt Status Register     
    __IOM  uint32_t  BRDIV;     //0x0010 	Baud Rate Generator Register  
    __IOM  uint32_t  DMACR;		//0x0014	DMA Control Register
} csp_uart_t; 

/*****************************************************************************
************************** UART Para Macro defined ***************************
******************************************************************************/
#define UART_RESET_VALUE	(0x00000000)
#define UART_RX_TIMEOUT		(0x10ff)
#define UART_TX_TIMEOUT		(0x1FFF)
#define UART_BUF_SIZE			(36)

/******************************************************************************
* SR : UART Status Register
******************************************************************************/
// SR : UART Status Register											
typedef enum{
	UART_TX_FULL		= (0x01ul << 0),		// Transmitter full        
	UART_RX_FULL		= (0x01ul << 1),		// Receiver full           
	UART_TX_OVER     	= (0x01ul << 2),		// Transmitter buff over   
	UART_RX_OVER     	= (0x01ul << 3), 		// Receiver buff over      
	UART_PARITY     	= (0x01ul << 4), 
	UART_TFE			= (0x01ul << 5),	
	UART_TNF			= (0x01ul << 6),	
	UART_RNE			= (0x01ul << 7),	
	UART_RFF			= (0x01ul << 8)	
}uart_sr_e;

/******************************************************************************
* CTRL : UART Control Register	
******************************************************************************/
// CTRL : UART Control Register		
#define	UART_TX_POS			(0)			// Transmitter Enable/disable 	 
#define	UART_TX_MSK			(0x01ul << UART_TX_POS)								
typedef enum{
	UART_TX_DIS			= 0,
	UART_TX_EN	
}tx_ctrl_e;

#define	UART_RX_POS			(1)			// Receiver Enable/disable         
#define	UART_RX_MSK			(0x01ul << UART_RX_POS)		
typedef enum{
	UART_RX_DIS			= 0,
	UART_RX_EN
}rx_ctrl_e;

#define	UART_PARITY_POS		(8)			// Uart Parity select			       
#define	UART_PARITY_MSK		(0x07ul << UART_PARITY_POS)
typedef enum{
	PARITY_NONE			= 0x00,
	PARITY_EVEN			= 0x04,
	PARITY_ODD    		= 0x05,
	PARITY_SPACE     	= 0x06,
	PARITY_MARK     	= 0x07
}uart_parity_e;

#define	UART_FIFO_POS		(11)		// Receiver FIFO level            
#define	UART_FIFO_MSK		(0x01ul << UART_FIFO_POS)		
typedef enum{
	UART_FIFO_DIS		= 0,
	UART_FIFO_EN
}uart_fifo_e;

#define	UART_RXFIFO_POS		(14)		// Receiver FIFO level            
#define	UART_RXFIFO_MSK		(0x07ul << UART_RXFIFO_POS)		
typedef enum{
	FIFO_RX_1_8			= 1,
	FIFO_RX_1_4			= 2,
	FIFO_RX_1_2			= 4
}uart_fifo_bit_e;

#define	UART_DBUG_POS		(31)		// Receiver FIFO level            
#define	UART_DBUG_MSK		(0x01ul << UART_DBUG_POS)		
typedef enum{
	UART_DBUG_DIS		= 0,
	UART_DBUG_EN
}uart_dbug_e;

typedef enum{
	UART_NO_INT     	= (0x00ul << 0),
	UART_TX_INT     	= (0x01ul << 2),
	UART_RX_INT     	= (0x01ul << 3),
	UART_TX_IOV     	= (0x01ul << 4),
	UART_RX_IOV     	= (0x01ul << 5),
	UART_PARITY_ERR     = (0x01ul << 7),
	UART_TXFIFO_INT     = (0x01ul << 12),
	UART_RXFIFO_INT     = (0x01ul << 13),
	UART_RXFIFO_OVER	= (0x01ul << 18),
	UART_TX_DONE 		= (0x01ul << 19),
}uart_int_e;

/******************************************************************************
* ISR : UART Interrupt Status Register	
******************************************************************************/
// ISR : UART Interrupt Status Register									
typedef enum{
	UART_TX_INT_S   	= (0x01ul << 0),	//Transmitter INT Status      
	UART_RX_INT_S		= (0x01ul << 1),	//Receiver INT Status          
	UART_TX_IOV_S     	= (0x01ul << 2),	//Transmitter Over INT Status  
	UART_RX_IOV_S     	= (0x01ul << 3),	//Receiver Over INT Status   
	UART_PAR_ERR_S     	= (0x01ul << 4),	//Parity Error Status  
	UART_TX_FIFO_S     	= (0x01ul << 5),	//Transmitter FIFO INT Status      
	UART_RX_FIFO_S     	= (0x01ul << 6),	//Receiver FIFO INT Status 
	UART_RXFIFO_IOV_S   = (0x01ul << 7),	//Receiver FIFO Over INT Status 
	UART_TX_DONE_S     	= (0x01ul << 19)	//Transmitter Complete INT Status
}uart_isr_e;

/******************************************************************************
* BRDIV : BRDIV register 
******************************************************************************/
// BRDIV : BRDIV register 
#define UART_BRDIV_POS		(0) 
#define	UART_BRDIV_MSK		(0x000FFFFF << UART_BRDIV_POS)	

/******************************************************************************
* DMACR : DMA Control register 
******************************************************************************/
#define	UART_RDMA_EN_POS		(0)		
#define	UART_RDMA_EN_MSK		(0x01ul << UART_RDMA_EN_POS)	
typedef enum{
	UART_RDMA_DIS		= 0,
	UART_RDMA_EN
}uart_rdma_en_e;

#define	UART_TDMA_EN_POS		(1)	
#define	UART_TDMA_EN_MSK		(0x01ul << UART_TDMA_EN_POS)	
typedef enum{
	UART_TDMA_DIS		= 0,
	UART_TDMA_EN
}uart_tdma_en_e;

#define	UART_RDMA_MD_POS	(2)			 
#define	UART_RDMA_MD_MSK	(0x01ul << UART_RDMA_MD_POS)	
typedef enum{
	UART_RDMA_FIFO_NSPACE = 0,
	UART_RDMA_FIFO_TRG
}uart_rdma_md_e;

#define	UART_TDMA_MD_POS	(3)			 
#define	UART_TDMA_MD_MSK	(0x01ul << UART_TDMA_MD_POS)	
typedef enum{
	UART_TDMA_FIFO_NFULL		= 0,
	UART_TDMA_FIF0_T
}uart_tdma_md_e;

/******************************************************************************
********************* ADC12 External Functions Declaration ********************
******************************************************************************/
extern void csp_uart_vic_irq_en(csp_uart_t *ptUartBase);
extern void csp_uart_default_init(csp_uart_t *ptUartBase);
extern void csp_uart_set_int(csp_uart_t *ptUartBase,uart_int_e eUartInt,bool bEnable);
extern void csp_uart_set_baudrate(csp_uart_t *ptUartBase,uint32_t wBaud, uint32_t wUartFreq);
extern void csp_uart_put_char(csp_uart_t *ptUartBase,uint8_t byByte);
extern void csp_uart_ctrl_init(csp_uart_t *ptUartBase, tx_ctrl_e eTxCtrl, rx_ctrl_e eRxCtrl,uart_parity_e eParity, uart_int_e eUartInt);

extern uint8_t  csp_uart_get_char(csp_uart_t *ptUartBase);
extern uint8_t  csp_uart_put_ready(csp_uart_t *ptUartBase);
extern uint8_t  csp_uart_get_ready(csp_uart_t *ptUartBase);
extern uint8_t  csp_uart_fifo_get_ready(csp_uart_t *ptUartBase);
extern uint32_t csp_uart_send(csp_uart_t *ptUartBase, uint8_t *byPdata, uint32_t size);

extern void csp_uart_send_dma(csp_uart_t *ptUartBase, void *byPdata, uint32_t size);
extern void csp_uart_recv_dma(csp_uart_t *ptUartBase, void *byPdata, uint32_t size);

/******************************************************************************
********************* ADC12 inline Functions Declaration **********************
******************************************************************************/
static inline void csp_uart_set_tx(csp_uart_t *ptUartBase, tx_ctrl_e eTxCtrl) 
{
	ptUartBase->CTRL = (ptUartBase->CTRL & (~UART_TX_MSK)) | eTxCtrl;
}
static inline void csp_uart_set_rx(csp_uart_t *ptUartBase, rx_ctrl_e eRxCtrl) 
{
	ptUartBase->CTRL = (ptUartBase->CTRL & (~UART_RX_MSK)) | (eRxCtrl << UART_RX_POS);
}
static inline void csp_uart_set_parity(csp_uart_t *ptUartBase, uart_parity_e eParity)
{
	ptUartBase->CTRL |= (eParity << UART_PARITY_POS);
}
//
static inline uint8_t csp_uart_get_data(csp_uart_t *ptUartBase)
{
	return (uint8_t)ptUartBase->DATA; 
}
static inline void csp_uart_set_data(csp_uart_t *ptUartBase, uint8_t byByte)
{
	ptUartBase->DATA = byByte;
}
//
static inline uint32_t csp_uart_get_sr(csp_uart_t *ptUartBase)
{
	return (uint32_t)(ptUartBase->SR);
}
static inline void csp_uart_clr_sr(csp_uart_t *ptUartBase)
{
	ptUartBase->SR = 0x01ff;
}
static inline uint32_t csp_uart_get_isr(csp_uart_t *ptUartBase)
{
	return (uint32_t)(ptUartBase->ISR);
}
static inline void csp_uart_clr_isr(csp_uart_t *ptUartBase, uart_isr_e eIstatus)
{
	ptUartBase->ISR = eIstatus;
}
//
static inline uint32_t csp_uart_get_ctrl(csp_uart_t *ptUartBase)
{
	return ptUartBase->CTRL;
}
static inline void csp_uart_set_ctrl(csp_uart_t *ptUartBase, uint32_t wVal)
{
	ptUartBase->CTRL = wVal;
}

#endif
