/***********************************************************************//** 
 * \file  csp_i2c.h
 * \brief  I2C description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-01 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-21 <td>V0.1  <td>ZJY   <td> modified in 110
  * <tr><td> 2021-1-22 <td>V0.2  <td>WNN   <td> move to 102
 * </table>
 * *********************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CSP_I2C_H
#define _CSP_I2C_H

/* Includes ------------------------------------------------------------------*/
#include <soc.h>

/// \struct csp_i2c_t
/// \brief I2C reg description     
typedef struct
{
    __IOM  uint32_t  CR;		//0x0000	I2C Control                           	 
    __IOM  uint32_t  TADDR;		//0x0004	I2C Target Address                     	 
    __IOM  uint32_t  SADDR;     //0x0008	I2C Slave Address                         
    __IOM  uint32_t  RSVD1;     //0x000C 
    __IOM  uint32_t  DATA_CMD;  //0x0010	I2C Rx/Tx Data Buffer and Command         
    __IOM  uint32_t  SS_SCLH;   //0x0014	I2C Standard Speed SCL High Count         
    __IOM  uint32_t  SS_SCLL;   //0x0018	I2C Standard Speed SCL Low  Count         
    __IOM  uint32_t  FS_SCLH;   //0x001C	I2C Fast mode and Fast Plus SCL High Count
    __IOM  uint32_t  FS_SCLL;   //0x0020	I2C Fast mode and Fast Plus SCL Low Count
    __IOM  uint32_t  RSVD2;		//0x0024                 
    __IOM  uint32_t  RSVD3;     //0x0028          	 
    __IOM  uint32_t  RX_FLSEL;  //0x002C 	I2C Receive FIFO Threshold            	 
    __IOM  uint32_t  TX_FLSEL;  //0x0030	I2C Transmit FIFO Threshold            	 
    __IOM  uint32_t  RX_FL;     //0x0034	I2C Receive  FIFO Level               	 
    __IOM  uint32_t  TX_FL;     //0x0038	I2C Transmit FIFO Level                	 
    __IOM  uint32_t  I2CENABLE; //0x003C 	I2C Enable                            	 
    __IM   uint32_t  STATUS;    //0x0040 	I2C Status                            	 
    __IOM  uint32_t  RSVD4;     //0x0044              	 
    __IOM  uint32_t  SDA_TSETUP;//0x0048	I2C SDA Setup Time                    	 
    __IOM  uint32_t  SDA_THOLD; //0x004C 	I2C SDA hold time length              	 
    __IOM  uint32_t  SPKLEN;    //0x0050 	I2C SS and FS Spike Suppression Limit 	 
    __IOM  uint32_t  RSVD5;		//0x0054
	__IM   uint32_t  MISR;      //0x0058 	I2C Interrupt Status           	 
    __IOM  uint32_t  IMCR;     	//0x005C 	I2C Interrupt Enable                  	 
    __IM   uint32_t  RISR;      //0x0060 	I2C Raw Interrupt Status            	     
    __OM   uint32_t  ICR;       //0x0064 	I2C Interrupt Status Clear                   	 
    __IOM  uint32_t  RSVD6;		//0x0068
    __IOM  uint32_t  SCL_TOUT; 	//0x006C 	I2C SCL Stuck at Low Timeout          	 
    __IOM  uint32_t  SDA_TOUT; 	//0x0070 	I2C SDA Stuck at Low Timeout          	 
    __IM   uint32_t  TX_ABRT; 	//0x0074 	I2C Transmit Abort Status             	 
    __IOM  uint32_t  GCALL;     //0x0078 	I2C ACK General Call                  	 
	__IOM  uint32_t  NACK; 		//0x007C 	I2C Generate SLV_DATA_NACK            	 
} csp_i2c_t;

/*****************************************************************************
************************** ssp Function defined *****************************
******************************************************************************/
#define I2C_RECV_TIMEOUT		(10000U)
#define I2C_SEND_TIMEOUT		(10000U)

typedef enum
{
	AADR_8BIT		= 0U,
    AADR_16BIT			         
}i2c_addr_size_e;

/******************************************************************************
* CR : I2C Control Register 
******************************************************************************/
//#define I2C_RESTART_EN     (0x01ul << 5)   //I2C Restart Enable           
//#define I2C_RESTART_DIS    (0x00ul << 5)   //I2C Restart Disable          
//#define I2C_STOPDET_IFADD  (0x01ul << 7)   //I2C STOPDET If Addressed     
//#define I2C_STOPDET_ALS    (0x00ul << 7)   //I2C STOPDET Always           
//#define I2C_TX_EMPTY_CTRL  (0x01ul << 8)   //I2C TX_EMPTY Control         
//#define I2C_TX_EMPTY_DONE  (0x00ul << 8)   //I2C TX_EMPTY and Send Done   
//#define I2C_RX_HOLD_CTRL   (0x01ul << 9)   //I2C Rx Hold Ctrl @FIFO Full  
//#define I2C_RX_HOLD_NONE   (0x00ul << 9)   //I2C Rx Hold None @FIFO Full  
//#define I2C_BUSCLR_EN      (0x01ul <<11)   //I2C Enable Bus Clr Feature   
//#define I2C_BUSCLR_DIS     (0x00ul <<11)   //I2C Disable Bus Clr Feature  

#define	I2C_MODE_POS		(0)			//I2C Mode: Master/Slave           
#define	I2C_MODE_MSK		(0x41ul << I2C_MODE_POS)
typedef enum
{
	I2C_MASTER			= (0x41ul),
    I2C_SLAVE			= (0x00ul)           
}i2c_mode_e;

#define	I2C_SPEED_POS		(1)			//I2C Speed Mode: Standard/Fast    
#define	I2C_SPEED_MSK		(0x03ul << I2C_SPEED_POS)
typedef enum
{
	I2C_SPEED_STAN		= (0x01ul),
	I2C_SPEED_FAST		= (0x02ul)          
}i2c_speed_e;

#define	I2C_ADDR_BIT_SLAVE_POS		(3)	//I2C Addr Bit Num                 
#define	I2C_ADDR_BIT_SLAVE_MSK		(0x01ul << I2C_ADDR_BIT_SLAVE_POS)
#define	I2C_ADDR_BIT_MASTER_POS		(4)
#define	I2C_ADDR_BIT_MASTER_MSK		(0x01ul << I2C_ADDR_BIT_MASTER_POS)
typedef enum
{
	I2C_ADDR_7BIT	= 0,
	I2C_ADDR_10BIT 
}i2c_addr_mode_e;

#define	I2C_RESTART_POS		(5)			//I2C Restart Enable  
#define	I2C_RESTART_MSK		(0x01ul << I2C_RESTART_POS)
typedef enum
{
	I2C_RESTART_DIS		= 0,
	I2C_RESTART_EN
}i2c_restart_e;

#define	I2C_STOPDET_POS		(7)			//I2C STOP DET
#define	I2C_STOPDET_MSK		(0x01ul << I2C_STOPDET_POS)
typedef enum
{
	I2C_STOPDET_ALS		= 0,
	I2C_STOPDET_IFADD
}i2c_stop_det_e;

#define	I2C_TX_EMPTY_POS	(8)			//I2C TX_EMPTY Control
#define	I2C_TX_EMPTY_MSK	(0x01ul << I2C_TX_EMPTY_POS)
typedef enum
{
	I2C_TX_EMPTY_CTRL	= 0,
	I2C_TX_EMPTY_DONE
}i2c_tx_empty_e;

#define	I2C_RXFULL_HLD_POS	(9)			//I2C Rx Full Hold Ctr
#define	I2C_RXFULL_HLD_MSK	(0x01ul << I2C_RXFULL_HLD_POS)
typedef enum
{
	I2C_RXFULL_HLD_NULL	= 0,
	I2C_RXFULL_HLD_DONE
}i2c_rxfull_hld_e;

#define	I2C_BUS_CLR_POS		(11)		//I2C Enable Bus Clr 
#define	I2C_BUS_CLR_MSK		(0x01ul << I2C_BUS_CLR_POS)
typedef enum
{
	I2C_BUS_CLR_DIS		= 0,
	I2C_BUS_CLR_EN
}i2c_bus_clr_e;
/******************************************************************************
* TADDR : I2C Target Address 
******************************************************************************/
typedef enum
{
	TADDR_NORMAL_0		= (0x00ul << 10),
	TADDR_NORMAL_1		= (0x01ul << 10),
	TADDR_GCALL			= (0x02ul << 10),
	TADDR_STARTB		= (0x03ul << 10)
}i2c_taddr_special_e;

/******************************************************************************
* DATA_CMD : I2C Data and Command Register 
******************************************************************************/
//#define I2C_CMD_READ       (0x01ul << 8)   //I2C Read Command             
//#define I2C_CMD_WRITE      (0x00ul << 8)   //I2C Write Command            
//#define I2C_CMD_STOP       (0x01ul << 9)   //I2C Stop after this byte     
//#define I2C_CMD_NONESTOP   (0x00ul << 9)   //I2C None Stop                
//#define I2C_CMD_RESTART0   (0x00ul <<10)   //I2C Restart Mode0            
//#define I2C_CMD_RESTART1   (0x01ul <<10)   //I2C Restart Mode1            

#define	I2C_CMD_POS		(8)
#define	I2C_CMD_MSK		(0x07ul << I2C_CMD_POS)
typedef enum
{
	I2C_CMD_WRITE      	= (0x00ul << 8),	//I2C Read Command  
	I2C_CMD_READ		= (0x01ul << 8),	//I2C Write Command   
	I2C_CMD_STOP        = (0x01ul << 9),	//I2C Stop after this byte 
	I2C_CMD_NONESTOP    = (0x00ul << 9),	//I2C None Stop  
	I2C_CMD_RESTART0    = (0x00ul << 10),	//I2C Restart Mode0  
	I2C_CMD_RESTART1    = (0x01ul << 10)	//I2C Restart Mode1 
}i2c_cmd_e;

#define I2C_DATA_POS       (0)  
#define I2C_DATA_MSK       (0xFFul << I2C_DATA_POS) 
#define I2C_DATA(val)      (((val) & 0xFFul) << I2C_DATA_POS)   

/*****************************************************************************
* ENABLE : I2C Enable Register
******************************************************************************/                     
#define I2C_ENABLE_POS      (0)   			//I2C Enable/Disable          
#define I2C_ENABLE_MSK      (0x01ul << I2C_ENABLE_POS)   			
typedef enum
{
	I2C_DISABLE			= 0,
	I2C_ENABLE
}i2c_enable_e;

#define I2C_ABORT_POS		(1)   			//I2C Abort Transfer          
#define I2C_ABORT_MSK       (0x01ul << I2C_ENABLE_POS)   			
typedef enum
{
	I2C_ABORT_DIS		= 0,
	I2C_ABORT_EN
}i2c_abort_e;

#define I2C_RECOVER_POS		(3)   			//I2C Enable Stuck Recovery    
#define I2C_RECOVER_MSK     (0x01ul << I2C_RECOVER_POS)   			
typedef enum
{
	I2C_RECOVER_DIS		= 0,
	I2C_RECOVER_EN
	
}i2c_recover_e;

/*****************************************************************************
* STATUS : I2C STATUS Register
******************************************************************************/                     
typedef enum
{
	I2C_BUSY		= (0x01ul << 0),   //I2C Activity                	
	I2C_TFNF        = (0x01ul << 1),   //I2C Transmit FIFO Not Full 	    
	I2C_TFE         = (0x01ul << 2),   //I2C Transmit FIFO Empty    	    
	I2C_RFNE        = (0x01ul << 3),   //I2C Receive FIFO Not Empty      
	I2C_RFF         = (0x01ul << 4),   //I2C Receive FIFO Full           
	I2C_MST_BUSY    = (0x01ul << 5),   //I2C Master FSM Activity         
	I2C_SLV_BUSY    = (0x01ul << 6),   //I2C Slave  FSM Activity         
	I2C_REC_FAIL    = (0x01ul << 11),  //I2C Recovery FAIL               
	I2C_ENABLE_SR   = (0x01ul << 12),  //I2C Enable Status               
	I2C_ABORT_SR    = (0x01ul << 13),  //I2C Abort  Status               
	I2C_SDA_REC_SR  = (0x01ul << 15)   //I2C SDA Recover Status          
}i2c_status_e;

/*****************************************************************************
* RISR/MISR/IMSCR/ICR : I2C Interrupt Mask/Status Register
******************************************************************************/  
typedef enum
{
	I2C_RX_UNDER_INT		= (0x01ul << 0),   //I2C Interrupt Status  
	I2C_RX_OVER_INT         = (0x01ul << 1),   //I2C Interrupt Status   
	I2C_RX_FULL_INT         = (0x01ul << 2),   //I2C Interrupt Status         
	I2C_TX_OVER_INT         = (0x01ul << 3),   //I2C Interrupt Status         
	I2C_TX_EMPTY_INT        = (0x01ul << 4),   //I2C Interrupt Status         
	I2C_RD_REQ_INT          = (0x01ul << 5),   //I2C Interrupt Status         
	I2C_TX_ABRT_INT         = (0x01ul << 6),   //I2C Interrupt Status         
	I2C_RX_DONE_INT         = (0x01ul << 7),   //I2C Interrupt Status         
	I2C_BUSY_INT       	 	= (0x01ul << 8),   //I2C Interrupt Status         
	I2C_STOP_DET_INT        = (0x01ul << 9),   //I2C Interrupt Status         
	I2C_START_DET_INT       = (0x01ul <<10),   //I2C Interrupt Status         
	I2C_GEN_CALL_INT        = (0x01ul <<11),   //I2C Interrupt Status         
	I2C_RESTART_DET_INT     = (0x01ul <<12),   //I2C Interrupt Status               
	I2C_SCL_SLOW_INT        = (0x01ul <<14)    //I2C Interrupt Status         
}i2c_int_e;

/*****************************************************************************
* SS_SCLH : Standard Speed SCL High Count  
******************************************************************************/  
#define	I2C_SS_SCLH_POS		(0)
#define	I2C_SS_SCLH_MSK		(0xFFFFul << I2C_SS_SCLH_POS)

/*****************************************************************************
* SS_SCLL : Standard Speed SCL Low  Count  
******************************************************************************/ 
#define	I2C_SS_SCLL_POS		(0)
#define	I2C_SS_SCLL_MSK		(0xFFFFul << I2C_SS_SCLL_POS)

/*****************************************************************************
* FS_SCLH : Fast mode and Fast Plus SCL High Count  
******************************************************************************/  
#define	I2C_FS_SCLH_POS		(0)
#define	I2C_FS_SCLH_MSK		(0xFFFFul << I2C_FS_SCLH_POS)

/*****************************************************************************
* FS_SCLL : Fast mode and Fast Plus SCL Low  Count  
******************************************************************************/ 
#define	I2C_FS_SCLL_POS		(0)
#define	I2C_FS_SCLL_MSK		(0xFFFFul << I2C_FS_SCLL_POS)

/*****************************************************************************
* RX_FLSEL : I2C Receive FIFO Threshold
******************************************************************************/  
#define	I2C_RX_FLSEL_POS	(0)
#define	I2C_RX_FLSEL_MSK	(0x07ul << I2C_RX_FLSEL_POS)

/*****************************************************************************
* TX_FLSEL : I2C Transmit FIFO Threshold    
******************************************************************************/  
#define	I2C_TX_FLSEL_POS	(0)
#define	I2C_TX_FLSEL_MSK	(0x07ul << I2C_TX_FLSEL_POS)

/*****************************************************************************
* RX_FL : I2C Receive  FIFO Level  
******************************************************************************/ 
#define	I2C_RX_FL_POS		(0)
#define	I2C_RX_FL_MSK		(0x0Ful << I2C_RX_FL_POS) 
/*****************************************************************************
* TX_FL : I2C Transmit FIFO Level 
******************************************************************************/  
#define	I2C_TX_FL_POS		(0)
#define	I2C_TX_FL_MSK		(0x0Ful << I2C_TX_FL_POS) 

/*****************************************************************************
* SDA_TSETUP : I2C SDA Setup Time   
******************************************************************************/  
#define	I2C_SETUP_POS		(0)
#define	I2C_SETUP_MSK		(0xFFul << I2C_SETUP_POS)

/*****************************************************************************
* SDA_THOLD : I2C SDA hold time length    
******************************************************************************/ 
#define I2C_TXHOLD_POS		(0)   			
#define I2C_TXHOLD_MSK      (0xFFFFul << I2C_TXHOLD_POS)   			

#define I2C_RXHOLD_POS		(16)   			
#define I2C_RXHOLD_MSK      (0x00FFul << I2C_RXHOLD_POS)  

/*****************************************************************************
* SPKLEN : I2C SS and FS Spike Suppression Limit   
******************************************************************************/  
#define	I2C_SPKLEN_POS		(0)
#define	I2C_SPKLEN_MSK		(0xFFul << I2C_SPKLEN_POS)

/*****************************************************************************
* SCL/SDA TOUT : I2C SCL/SDA Stuck Time Out
******************************************************************************/   
#define I2C_SCL_TOUT_POS    (0)
#define I2C_SCL_TOUT_MSK    (0xFFFFFFFFul << I2C_SCL_TOUT_POS) 
#define I2C_SDA_TOUT_POS    (0)
#define I2C_SDA_TOUT_MSK    (0xFFFFFFFFul << I2C_SDA_TOUT_POS)    
#define I2C_SCL_TOUT(val)   (((val) & 0xFFFFFFFFul) << 0) //SCL Stuck Time Out 
#define I2C_SDA_TOUT(val)   (((val) & 0xFFFFFFFFul) << 0) //SDA Stuck Time Out 

/*****************************************************************************
* TX_ABRT : I2C Transmit Abort Status
******************************************************************************/                     
typedef enum
{
	TX_ABRT_7ADDR_NACK		= (0x01ul << 0),
	TX_ABRT_10ADDR1_NACK	= (0x01ul << 1),
	TX_ABRT_10ADDR2_NACK	= (0x01ul << 2),
	TX_ABRT_TXDATA_NACK		= (0x01ul << 3),
	TX_ABRT_GCALL_NACK		= (0x01ul << 4),
	TX_ABRT_GCALL_READ		= (0x01ul << 5),
	TX_ABRT_SBYTE_ACK		= (0x01ul << 7),
	TX_ABRT_SBYTE_NRSTRT	= (0x01ul << 9),
	TX_ABRT_10B_RD_NRSTRT	= (0x01ul << 10),
	TX_ABRT_MASTER_DIS		= (0x01ul << 11),
	TX_ABRT_ARB_LOST		= (0x01ul << 12),
	TX_ABRT_SLVFLUSH_TX		= (0x01ul << 13),
	TX_ABRT_SLV_ARBLOST		= (0x01ul << 14),
	TX_ABRT_SLVRD_INTX		= (0x01ul << 15),
	TX_ABRT_USER_ABRT		= (0x01ul << 16),
	TX_ABRT_SDA_S_LOW		= (0x01ul << 17)
}i2c_tx_abrt_e;

/*****************************************************************************
* GCALL : I2C General Call Register
******************************************************************************/                     
#define I2C_ACK_GCALL_POS	(0) 			//Uses ACK to answer General Call    			
#define I2C_ACK_GCALL_MSK	(0x01ul << I2C_ACK_GCALL_POS)   			
typedef enum
{
	I2C_ACK_GCALL_DIS	= 0,
	I2C_ACK_GCALL_EN
}i2c_ack_gcall_e;

/*****************************************************************************
* NACK : I2C Slave NACK Control Register
******************************************************************************/                     
//#define I2C_NACK_DATA     (0x01ul << 0)   //Generate a NACK After received byte 
//#define I2C_NACK_NORMAL   (0x00ul << 0)   //Generate NACK/ACK as normal         
#define I2C_NACK_POS		(0) 			//Uses ACK to answer General Call    			
#define I2C_NACK_MSK		(0x01ul << I2C_NACK_POS)   			
typedef enum
{
	I2C_NACK_NORMAL	= 0,
	I2C_NACK_DATA
}i2c_nack_e;

/******************************************************************************
********************** IIC External Functions Declaration *********************
******************************************************************************/
extern void csp_i2c_en(csp_i2c_t *ptI2cBase);
extern void csp_i2c_dis(csp_i2c_t *ptI2cBase);
extern void csp_i2c_default_init(csp_i2c_t *ptI2cBase);
extern void csp_i2c_set_mode(csp_i2c_t *ptI2cBase, i2c_mode_e eMode);
extern void csp_i2c_set_int(csp_i2c_t *ptI2cBase,i2c_int_e eI2cInt,bool bEnable);
extern void csp_i2c_set_sclh(csp_i2c_t *ptI2cBase, uint16_t hwSclH, i2c_speed_e eSpeed);
extern void csp_i2c_set_scll(csp_i2c_t *ptI2cBase, uint16_t hwSclL, i2c_speed_e eSpeed);
extern void csp_i2c_set_addr_mode(csp_i2c_t *ptI2cBase, i2c_addr_mode_e eAddrMode, i2c_mode_e eMode);
extern void csp_i2c_set_master(csp_i2c_t *ptI2cBase, i2c_speed_e eSpeed, i2c_addr_mode_e eMasterAddr,
					uint16_t hwTaddr, uint16_t hwSclH, uint16_t hwSclL);
extern void csp_i2c_set_slave(csp_i2c_t *ptI2cBase, i2c_speed_e eSpeed, i2c_addr_mode_e eSlaveAddr,
					uint16_t hwSaddr, uint16_t hwSclH, uint16_t hwSclL);
extern void csp_i2c_set_flsel(csp_i2c_t *ptI2cBase, uint8_t byRxFL, uint8_t byTxFL);
extern void csp_i2c_wait_recv_fifo_empty(csp_i2c_t *ptI2cBase,uint32_t wTimeOut);		
extern void csp_i2c_wait_send_fifo_empty(csp_i2c_t *ptI2cBase,uint32_t wTimeOut);	
extern int  csp_i2c_wait_send(csp_i2c_t *ptI2cBase,uint32_t wTimeOut);	
extern int  csp_i2c_wait_recv(csp_i2c_t *ptI2cBase,uint32_t wTimeOut);	
extern int  csp_i2c_write_byte(csp_i2c_t *ptI2cBase, uint8_t byData);
extern int  csp_i2c_write_nbyte(csp_i2c_t *ptI2cBase, uint8_t *pbyData, uint32_t wWriteNum);
extern int  csp_i2c_write_mem_byte(csp_i2c_t *ptI2cBase, uint16_t hwWrAddr, i2c_addr_size_e eAddrSize, uint8_t byData);
extern int  csp_i2c_write_mem_nbyte(csp_i2c_t *ptI2cBase, uint16_t hwWrAddr, i2c_addr_size_e eAddrSize, uint8_t *pbyData, uint32_t wWriteNum);
extern int  csp_i2c_read_byte(csp_i2c_t *ptI2cBase);
extern int  csp_i2c_read_nbyte(csp_i2c_t *ptI2cBase, uint8_t *pbyData, uint32_t wReadNum);
extern int  csp_i2c_write_read_nbyte(csp_i2c_t *ptI2cBase, uint8_t *pbySend, uint32_t wWriteNum, uint8_t *pbyRecv, uint32_t wReadNum);

extern int  csp_i2c_read_mem_byte(csp_i2c_t *ptI2cBase,uint16_t hwRdAddr, i2c_addr_size_e eAddrSize);
extern int  csp_i2c_read_mem_nbyte(csp_i2c_t *ptI2cBase,uint16_t hwRdAddr, i2c_addr_size_e eAddrSize, uint8_t *pbyData, uint32_t wReadNum);

/******************************************************************************
********************* IIC inline Functions Declaration ************************
******************************************************************************/
static inline void csp_i2c_enable(csp_i2c_t *ptI2cBase)
{
	ptI2cBase->I2CENABLE |= I2C_ENABLE;
}
static inline void csp_i2c_disable(csp_i2c_t *ptI2cBase)
{
	ptI2cBase->I2CENABLE = I2C_DISABLE;
}
static inline void csp_i2c_set_speed(csp_i2c_t *ptI2cBase, i2c_speed_e eSpeed)
{
	ptI2cBase->CR = (ptI2cBase->CR & (~I2C_SPEED_MSK)) | (eSpeed << I2C_SPEED_POS);
}
static inline void csp_i2c_set_cr(csp_i2c_t *ptI2cBase, uint32_t wCr)
{
	ptI2cBase->CR = (ptI2cBase->CR&0XFFFFF000)| wCr;
}

static inline void csp_i2c_set_saddr(csp_i2c_t *ptI2cBase, uint32_t wSaddr)
{
	ptI2cBase->SADDR = (wSaddr & 0x3ff);
}
static inline void csp_i2c_set_taddr(csp_i2c_t *ptI2cBase, uint32_t wTAddr)
{
	ptI2cBase->TADDR = (wTAddr & 0xfff);
}
//
static inline void csp_i2c_set_data_cmd(csp_i2c_t *ptI2cBase, uint16_t hwDataCmd)
{
	ptI2cBase->DATA_CMD = hwDataCmd;
}
static inline uint8_t csp_i2c_get_data(csp_i2c_t *ptI2cBase)
{
	return (uint8_t)(ptI2cBase->DATA_CMD & 0xff);
}
//
static inline void csp_i2c_sda_setup(csp_i2c_t *ptI2cBase, uint8_t bySetTime)
{
	ptI2cBase->SDA_TSETUP = bySetTime;
}
static inline void csp_i2c_sda_hold(csp_i2c_t *ptI2cBase, uint16_t hwTxHold, uint8_t byRxHold)
{
	ptI2cBase->SDA_THOLD = hwTxHold | (byRxHold << I2C_RXHOLD_POS);
}
//
static inline void csp_i2c_set_spklen(csp_i2c_t *ptI2cBase, uint8_t bySpkLen)
{
	ptI2cBase->SPKLEN = bySpkLen;
}
//
static inline void csp_i2c_set_rx_flsel(csp_i2c_t *ptI2cBase, uint8_t byRxFL)
{
	ptI2cBase->RX_FLSEL = (byRxFL & I2C_RX_FLSEL_MSK); 
}
static inline void csp_i2c_set_tx_flsel(csp_i2c_t *ptI2cBase,uint8_t byTxFL)
{
	ptI2cBase->TX_FLSEL = (byTxFL & I2C_TX_FLSEL_MSK); 
}
static inline uint8_t csp_i2c_get_rx_fifo_num(csp_i2c_t *ptI2cBase)
{
	return (uint8_t)(ptI2cBase->RX_FL & I2C_RX_FL_MSK);
}
static inline uint8_t csp_i2c_get_tx_fifo_num(csp_i2c_t *ptI2cBase)
{
	return (uint8_t)(ptI2cBase->TX_FL & I2C_TX_FL_MSK);
}

static inline void csp_i2c_set_ss_sclh(csp_i2c_t *ptI2cBase, uint32_t wSsSclH)
{
	ptI2cBase->SS_SCLH = wSsSclH; 
}
static inline void csp_i2c_set_ss_scll(csp_i2c_t *ptI2cBase, uint32_t wSsSclL)
{
	ptI2cBase->SS_SCLL = wSsSclL; 
}
static inline void csp_i2c_set_fs_sclh(csp_i2c_t *ptI2cBase, uint32_t wFsSclH)
{
	ptI2cBase->FS_SCLH = wFsSclH; 
}
static inline void csp_i2c_set_fs_scll(csp_i2c_t *ptI2cBase, uint32_t wFsSclL)
{
	ptI2cBase->FS_SCLL = wFsSclL; 
}

//
static inline void csp_i2c_set_scl_tout(csp_i2c_t *ptI2cBase, uint32_t wSclTout)
{
	ptI2cBase->SCL_TOUT = wSclTout; 
}
static inline void csp_i2c_set_sda_tout(csp_i2c_t *ptI2cBase, uint32_t wSdaTout)
{
	ptI2cBase->SDA_TOUT = wSdaTout; 
}
//
static inline void csp_i2c_set_ack_gcall(csp_i2c_t *ptI2cBase, i2c_ack_gcall_e eAckGcall)
{
	ptI2cBase->GCALL = (ptI2cBase->GCALL & ~I2C_ACK_GCALL_MSK) | eAckGcall;
}
static inline void csp_i2c_set_nack(csp_i2c_t *ptI2cBase, i2c_nack_e eNack)
{
	ptI2cBase->NACK = (ptI2cBase->NACK  & ~I2C_NACK_MSK) | eNack;
}

//
static inline uint16_t csp_i2c_get_status(csp_i2c_t *ptI2cBase)
{
	return (uint16_t)ptI2cBase->STATUS;
}
static inline uint16_t csp_i2c_get_risr(csp_i2c_t *ptI2cBase)
{
	return (uint16_t)ptI2cBase->RISR;
}
static inline uint16_t csp_i2c_get_isr(csp_i2c_t *ptI2cBase)
{
	return (uint16_t)ptI2cBase->MISR;
}

static inline uint32_t csp_i2c_get_i2cenable(csp_i2c_t *ptI2cBase)
{
	return ptI2cBase->I2CENABLE;
}

static inline uint32_t csp_i2c_get_tx_abrt(csp_i2c_t *ptI2cBase)
{
	return ptI2cBase->TX_ABRT;
}

static inline void csp_i2c_clr_isr(csp_i2c_t *ptI2cBase,i2c_int_e eIntNum)
{
	ptI2cBase->ICR = eIntNum;
}
static inline void csp_i2c_clr_all_int(csp_i2c_t *ptI2cBase)
{
	ptI2cBase->ICR = 0x7fff;
}
static inline void csp_i2c_set_imcr(csp_i2c_t *ptI2cBase,uint32_t wIntNum)
{
	ptI2cBase->IMCR = wIntNum;
}
static inline void csp_i2c_imcr_enable(csp_i2c_t *ptI2cBase,uint32_t wIntNum)
{
	ptI2cBase->IMCR |= wIntNum;
}
static inline void csp_i2c_imcr_disable(csp_i2c_t *ptI2cBase,uint32_t wIntNum)
{
	ptI2cBase->IMCR &= (~wIntNum);
}
//
static inline void csp_i2c_restart_en(csp_i2c_t *ptI2cBase)
{
	ptI2cBase->CR |= (I2C_RESTART_EN << I2C_RESTART_POS);
}
static inline void csp_i2c_restart_dis(csp_i2c_t *ptI2cBase)
{
	ptI2cBase->CR &= ~I2C_RESTART_MSK;
}
//
static inline void csp_i2c_busclr_en(csp_i2c_t *ptI2cBase)
{
	ptI2cBase->CR |= (I2C_BUS_CLR_EN << I2C_BUS_CLR_POS);
}
static inline void csp_i2c_busclr_dis(csp_i2c_t *ptI2cBase)
{
	ptI2cBase->CR &= I2C_BUS_CLR_MSK;
}

static inline void csp_i2c_abort_en(csp_i2c_t *ptI2cBase)
{
	ptI2cBase->I2CENABLE |= (I2C_ABORT_EN << I2C_ABORT_POS);
}
static inline void csp_i2c_recover_en(csp_i2c_t *ptI2cBase)
{
	ptI2cBase->I2CENABLE |= (I2C_RECOVER_EN << I2C_RECOVER_POS);
}
static inline void csp_i2c_recover_dis(csp_i2c_t *ptI2cBase)
{
	ptI2cBase->I2CENABLE = (ptI2cBase->I2CENABLE & ~I2C_RECOVER_MSK);
}
static inline void csp_i2c_stop(csp_i2c_t *ptI2cBase)
{
	ptI2cBase->DATA_CMD |= I2C_CMD_STOP;
}
//

static inline void csp_i2c_vic_int_en(void)
{
	NVIC_EnableIRQ(I2C_IRQn); 
}
static inline void csp_i2c_vic_int_dis(void)
{
	NVIC_DisableIRQ(I2C_IRQn); 
}

#endif
