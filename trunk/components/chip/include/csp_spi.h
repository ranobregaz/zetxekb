/***********************************************************************//** 
 * \file  csp_spi.h
 * \brief  SPI description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2021 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-8-27 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-12 <td>V0.1  <td>ZJY   <td>macro definition style, modify
 * <tr><td> 2021-5-31 <td>V0.2  <td>LQ    <td>macro definition style, modify
 * </table>
 * *********************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CSP_SPI_H
#define _CSP_SPI_H

/* Includes ------------------------------------------------------------------*/
#include <soc.h>

/// \struct csp_adc_t
/// \brief SPI reg description
typedef struct
{
	__IOM  uint32_t  CR0;		//0x0000	Control Register 0 			 
	__IOM  uint32_t  CR1;		//0x0004	Control Register 1 
	__IOM  uint32_t  DR; 		//0x0008	Receive/transmit  data register 
	__IM   uint32_t  SR;		//0x000C	Status register 
	__IOM  uint32_t  CPSR;		//0x0010	Clock prescale register         
	__IOM  uint32_t  IMSCR;		//0x0014	Interrupt set/clear register    
	__IM   uint32_t  RISR;		//0x0018	Raw interrupt status register   
	__IM   uint32_t  MISR;		//0x001C	Mask interrupt status register  
	__OM   uint32_t  ICR;		//0x0020	Interrupt clear register        
} csp_spi_t;

/*****************************************************************************
************************** ssp Function defined *****************************
******************************************************************************/
#define SPI_RECV_TIMEOUT		(0x10ff)
#define SPI_SEND_TIMEOUT		(0x1FFF)
#define SPI_TIMEOUT_DIS			(0x01)


//SPI Registers RST  Value
#define SPI_CR0_RST          	(0x00000000)    	//CR0 reset value     
#define SPI_CR1_RST          	(0x00000010)    	//CR1 reset value     
#define SPI_DR_RST          	(0x00000000)   	 	//DR reset value      
#define SPI_SR_RST          	(0x00000003)    	//SR reset value      
#define SPI_CPSR_RST          	(0x00000000)    	//CPSR reset value    
#define SPI_IMSCR_RST          	(0x00000000)   	 	//IMSCR reset value   
#define SPI_RISR_RST          	(0x00000008)    	//RISR reset value    
#define SPI_MISR_RST          	(0x00000000)    	//MISR reset value    
#define SPI_ICR_RST          	(0x00000000)    	//ICR reset value     

#define	SPI_RXTX_TIMEOUT		0x1ff
/*****************************************************************************
* SPICR0 : Control Register 0
******************************************************************************/
// SPICR0 : Control Register 0
//#define SPI_SPO  	   	(0x01ul << 6)				//SPICLK Polarity 	 
//#define SPI_SPH  	    (0x01ul << 7)				//SPICLK Phase    	 

#define	SPI_DSS_POS		(0)							//Data Size Select    
#define	SPI_DSS_MSK		(0x0Ful << SPI_DSS_POS)
#define SPI_DSS(val)	(((val-1) & 0x0Ful) << SPI_DSS_POS)	
typedef enum
{
	SPI_DSS_4		= 3,            
	SPI_DSS_5,            
	SPI_DSS_6,            
	SPI_DSS_7,            
	SPI_DSS_8,  
	SPI_DSS_9,      
	SPI_DSS_10,            
	SPI_DSS_11,            
	SPI_DSS_12,            
	SPI_DSS_13,            
	SPI_DSS_14,  
	SPI_DSS_15,  
	SPI_DSS_16
}spi_data_size_e;

#define	SPI_RFR_POS		(4)							//Frame Format 		
#define	SPI_RFR_MSK		(0x03ul << SPI_DSS_POS)
typedef enum
{
	SPI_RFR_MOTO	= 0,            
	SPI_RFR_1          
}spi_rfr_e;

#define SPI_SPO_H_POS	(6)							//SPICLK Polarity / Phase   
#define SPI_SPO_H_MSK	(0x03ul << SPI_SPO_H_POS)
typedef enum
{
	SPI_SPO0_SPH0	= (0x00ul),            
	SPI_SPO0_SPH1	= (0x02ul),  
    SPI_SPO1_SPH0	= (0x01ul),            
	SPI_SPO1_SPH1   = (0x03ul)            
}spi_spo_h_e;

#define SPI_SCR_POS		(8)							//Serial Clock Rate   			
#define SPI_SCR_MSK		(0xFFul << SPI_SCR_POS)						
#define SPI_SCR(val)	(((val) & 0xFFul) << SPI_SCR_POS)	

/******************************************************************************
* SPICR1 : Control Register 1
******************************************************************************/
// SPICR1 : Control Register 0
//#define SPI_SSE  	    (0x01ul << 1)		//SSerial Port Enable 		 
//#define SPI_SOD  	    (0x01ul << 3)		//Slave Mode Output Disable   
//#define SPI_LPDM  	(0x01ul << 7)		//Slave Mode Output Disable   
//#define SPI_LPTXOE  	(0x01ul << 8)		//Slave Mode Output Disable   

#define	SPI_LBM_POS		(0)					//Loopback mode 				 				
#define	SPI_LBM_MSK		(0x01ul << SPI_LBM_POS)
typedef enum
{
	SPI_LBM_DIS		= 0,            
	SPI_LBM_EN   
}spi_lbm_e;

#define	SPI_SSE_POS		(1)								 				
#define	SPI_SSE_MSK		(0x01ul << SPI_SSE_POS)
typedef enum
{
	SPI_DIS		= 0,            
	SPI_EN   
}spi_sse_e;

#define	SPI_MODE_POS	(2)					//Master / Slave Mode Select  	
#define	SPI_MODE_MSK	(0x01ul << SPI_MODE_POS)
typedef enum
{
	SPI_MODE_MASTER	= 0,            
	SPI_MODE_SLAVE      
}spi_mode_e;

#define	SPI_SOD_POS		(3)					
#define	SPI_SOD_MSK		(0x01ul << SPI_SOD_POS)
typedef enum
{
	SPI_SOD_TXEN	= 0,            
	SPI_SOD_TXDIS      
}spi_sod_e;

#define	SPI_RXIFL_POS	(4)					//RX INT FIFO Level Select    
#define	SPI_RXIFL_MSK	(0x07ul << SPI_RXIFL_POS)
#define SPI_RXIFL(val)	(((val) & 0x07ul) << SPI_RXIFL_POS)
typedef enum
{
	SPI_RXFIFO_1_8	= (0x01ul),            
	SPI_RXFIFO_1_4  = (0x02ul),        
	SPI_RXFIFO_1_2  = (0x04ul)   
}spi_rxifl_e;

#define	SPI_LPMD_POS		(7)				
#define	SPI_LPMD_MSK		(0x01ul << SPI_LPMD_POS)
typedef enum
{
	SPI_LPMD_NOR	= 0,            
	SPI_LPMD_SINGLE      
}spi_lpmd_e;

#define	SPI_LPTXOE_POS		(8)					
#define	SPI_LPTXOE_MSK		(0x01ul << SPI_LPTXOE_POS)
typedef enum
{
	SPI_LPTXOE_DIS	= 0,            
	SPI_LPTXOE_EN      
}spi_lptxoe_e;

/******************************************************************************
* SPIDR : Data Register
******************************************************************************/
// SPIDR : Data Register
#define	SPI_DATA_POS		(0)				//Transmit/Receive FIFO 		 
#define	SPI_DATA_MSK		(0xFFFFul << SPI_RXFIFO_POS)

/******************************************************************************
* SPISR : Status Register
******************************************************************************/
// SPISR : Status Register
typedef enum
{
	SPI_TFE		= (0x01ul << 0), 			//Transmit FIFO Empty                    
	SPI_TNF     = (0x01ul << 1), 			//Transmit FIFO is not Full          
	SPI_RNE     = (0x01ul << 2),			//Receive is not Empty             
	SPI_RFF		= (0x01ul << 3),   			//Receive FIFO Full                    
	SPI_BSY     = (0x01ul << 4) 			//PrimeCell SPI Busy Flag            
}spi_sr_e;

/******************************************************************************
* CPSR : Clock prescale register
******************************************************************************/
// CPSR : Clock prescale register
#define	SPI_CPSDVSR_POS		(0)				//Clock Prescale Devisor		 
#define	SPI_CPSDVSR_MSK		(0xFFul << SPI_CPSDVSR_POS)
#define SPI_CPSDVSR(val)	(((val) & 0xFFul) << SPI_CPSDVSR_POS)

/******************************************************************************
* SPIIMSC : SPIIMSC : SPIMIS : SPIICR
******************************************************************************/
// SPIIMSC : Interrupt mask set and clear register
// SPIRIS : Raw interrupt status register
// SPIMIS : Masked interrupt status register
// SPIICR : Interrupt clear register
typedef enum
{
	SPI_NONE_INT		= (0x00ul << 0),   		//none interrupt 
	SPI_ROTIM_INT		= (0x01ul << 0), 		//Receive Overrun Interrupt              
	SPI_RTIM_INT    	= (0x01ul << 1), 		//Receive Timeout Interrupt          
	SPI_RXIM_INT   		= (0x01ul << 2),		//Receive FIFO Interrupt           
	SPI_TXIM_INT		= (0x01ul << 3),   		//Transmit FIFO interrupt 
}spi_int_e;


/******************************************************************************
********************** SPI External Functions Declaration *********************
******************************************************************************/
//extern void csp_spi_default_init(csp_spi_t *ptSpiBase);
//extern void csp_spi_set_int(csp_spi_t *ptSpiBase,spi_int_e eSpiInt,bool bEnable);
//extern void csp_spi_set_clk_div(csp_spi_t *ptSpiBase,uint8_t wScr,uint8_t wCpsdvsr);
//extern void csp_spi_set_slave(csp_spi_t *ptSpiBase,spi_data_size_e eSize,spi_spo_h_e eSph, spi_rxifl_e eRxfifo, spi_sod_e eSod);
//extern void csp_spi_set_master(csp_spi_t *ptSpiBase,spi_data_size_e eSize,spi_spo_h_e eSpoH, spi_rxifl_e eRxfifo, spi_lbm_e eLbm);
//extern void csp_spi_write_byte(csp_spi_t *ptSpiBase,uint16_t hwData);

//extern uint16_t csp_spi_read_byte(csp_spi_t *ptSpiBase,uint16_t hwWdata);
//extern int16_t  csp_spi_read_write_byte(csp_spi_t *ptSpiBase,uint16_t hwWdata);

//extern uint8_t  csp_spi_busy(csp_spi_t *ptSpiBase);
//extern uint8_t  csp_spi_read_ready(csp_spi_t *ptSpiBase);
//extern uint8_t  csp_spi_write_ready(csp_spi_t *ptSpiBase);

/******************************************************************************
********************* SPI inline Functions Declaration **********************
******************************************************************************/
static inline void csp_spi_en(csp_spi_t *ptSpiBase)
{
	ptSpiBase->CR1 |= (SPI_EN << SPI_SSE_POS);
}
static inline void csp_spi_dis(csp_spi_t *ptSpiBase)
{
	ptSpiBase->CR1 &= ~SPI_SSE_MSK;
}
//
static inline uint16_t csp_spi_get_data(csp_spi_t *ptSpiBase)
{
	return (uint16_t)(ptSpiBase->DR);
}
static inline void csp_spi_set_data(csp_spi_t *ptSpiBase,uint16_t hwData)
{
	ptSpiBase->DR = hwData;
}
//
static inline void csp_spi_set_mode(csp_spi_t *ptSpiBase,spi_mode_e eMode)
{
	ptSpiBase->CR1 = (ptSpiBase->CR1 & (~SPI_MODE_MSK)) | (eMode << SPI_MODE_POS);
}
static inline uint8_t csp_spi_get_mode(csp_spi_t *ptSpiBase)
{
	return (uint8_t)((ptSpiBase->CR1 & SPI_MODE_MSK) >> SPI_MODE_POS);
}
static inline void csp_spi_set_spo_sph(csp_spi_t *ptSpiBase, spi_spo_h_e eSpoH)
{
	ptSpiBase->CR0 = (ptSpiBase->CR0 & (~SPI_SPO_H_MSK)) | (eSpoH << SPI_SPO_H_POS);
}
static inline void csp_spi_set_data_size(csp_spi_t *ptSpiBase,spi_data_size_e eSize)
{
	ptSpiBase->CR0 = (ptSpiBase->CR0 & (~SPI_DSS_MSK)) | (eSize << SPI_DSS_POS);
}
static inline uint8_t csp_spi_get_data_size(csp_spi_t *ptSpiBase)
{
	return (uint8_t)(ptSpiBase->CR0 & SPI_DSS_MSK);
}
static inline void csp_spi_set_rxifl(csp_spi_t *ptSpiBase,spi_rxifl_e eRxIfl)
{
	ptSpiBase->CR1  = (ptSpiBase->CR1 & (~SPI_RXIFL_MSK)) | (eRxIfl << SPI_RXIFL_POS);
}
static inline void csp_spi_set_lbm(csp_spi_t *ptSpiBase, spi_lbm_e eLbmSel)
{
	ptSpiBase->CR1  = (ptSpiBase->CR1 & (~SPI_LBM_MSK)) | (eLbmSel << SPI_LBM_POS);
}
static inline void csp_spi_master_single(csp_spi_t *ptSpiBase, spi_lpmd_e eLpmd, spi_lptxoe_e eLptxoe)
{
	ptSpiBase->CR1  = (ptSpiBase->CR1 & ~(SPI_LPMD_MSK | SPI_LPTXOE_MSK)) | (eLpmd << SPI_LPMD_POS) | (eLptxoe << SPI_LPTXOE_POS);
}
//
static inline uint32_t csp_spi_get_sr(csp_spi_t *ptSpiBase)
{
	return (uint32_t)(ptSpiBase->SR);
}
static inline uint32_t csp_spi_get_isr(csp_spi_t *ptSpiBase)
{
	return (uint32_t)(ptSpiBase->MISR);
}
static inline void csp_spi_clr_isr(csp_spi_t *ptSpiBase, spi_int_e eSpiInt)
{
	ptSpiBase->ICR = eSpiInt;
}
//
static inline void csp_spi_vic_int_en(void)
{
	NVIC_EnableIRQ(SPI_IRQn); 
}
static inline void csp_spi_vic_int_dis(void)
{
	NVIC_DisableIRQ(SPI_IRQn); 
}
static inline void csp_spi_wakeup_en(void)
{
	NVIC_SetWakeupIRQ(SPI_IRQn);
}
static inline void csp_spi_wakeup_dis(void)
{
	NVIC_ClearWakeupIRQ(SPI_IRQn);
}

//lin add
static inline uint8_t csp_spi_get_rxifl(csp_spi_t *ptSpiBase)
{
	return ptSpiBase->CR1 & (SPI_RXIFL_MSK) ;
}

static inline void csp_spi_set_clk_div(csp_spi_t *ptSpiBase,uint8_t wScr,uint8_t wCpsdvsr)
{
	ptSpiBase->CR0 = (ptSpiBase->CR0 & (~SPI_SCR_MSK)) | SPI_SCR(wScr);
	ptSpiBase->CPSR  = wCpsdvsr;	
}

static inline uint8_t csp_spi_write_ready(csp_spi_t *ptSpiBase)
{	
	if(csp_spi_get_sr(ptSpiBase) & SPI_TNF)
		return TRUE;			//no full
	else
		return FALSE;			//full
}

static inline uint8_t csp_spi_busy(csp_spi_t *ptSpiBase)
{
	if(csp_spi_get_sr(ptSpiBase) & SPI_BSY)
		return TRUE;			//busy
	else
		return FALSE;			//idle
}

static inline void csp_spi_default_init(csp_spi_t *ptSpiBase)
{
	ptSpiBase->CR0	 = SPI_CR0_RST;
    ptSpiBase->CR1   = SPI_CR1_RST;
    //ptSpiBase->DR    = SPI_DR_RST;
    //ptSpiBase->SR    = SPI_SR_RST;
    ptSpiBase->CPSR  = SPI_CPSR_RST;
    ptSpiBase->IMSCR = SPI_IMSCR_RST;
   //ptSpiBase->RISR  = SPI_RISR_RST;
    //ptSpiBase->MISR  = SPI_MISR_RST;
    ptSpiBase->ICR	 = SPI_ICR_RST; 
}

static inline void csp_spi_set_int(csp_spi_t *ptSpiBase,spi_int_e eSpiInt,bool bEnable)
{
	if(bEnable)
		ptSpiBase->IMSCR |= eSpiInt;
	else
		ptSpiBase->IMSCR &= ~eSpiInt;

}

static inline uint8_t csp_spi_read_ready(csp_spi_t *ptSpiBase)
{
	if(csp_spi_get_sr(ptSpiBase) & SPI_RNE)
		return TRUE;			//no empty
	else
		return FALSE;			//empty
}
#endif

