/***********************************************************************//** 
 * \file  i2c.h
 * \brief  I2C head file 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-01 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-21 <td>V0.1  <td>ZJY   <td> modified in 110
  * <tr><td> 2021-1-22 <td>V0.2  <td>WNN   <td> move to 102
 * </table>
 * *********************************************************************
*/

#ifndef _DRV_IIC_H_
#define _DRV_IIC_H_

#include <stdint.h>
#include <stdbool.h>
#include <drv/common.h>
#include <drv/dma.h>
#include "csp.h"
#include "csp_i2c.h"




#ifdef __cplusplus
extern "C" {
#endif

typedef struct csi_iic_slave_config csi_iic_slave_config_t;

struct csi_iic_slave_config {
	uint16_t	hwSlaveAddr;			//IIC Slave address
	uint8_t		bySpeedMode;			//IIC Speed mode
	uint8_t		byAddrMode;				//IIC ADDR mode 7/10 bit
	uint16_t	hwInterrput;			//IIC INTERRPUT SET
	uint32_t	wSdaTimeout;			//IIC SDA timeout SET
	uint32_t	wSclTimeout;			//IIC SCL timeout SET
};

typedef struct csi_iic_master_config csi_iic_master_config_t;

struct csi_iic_master_config {
	uint8_t		bySpeedMode;			//IIC Speed mode
	uint8_t		byAddrMode;				//IIC ADDR mode 7/10 bit
	uint8_t		byReStart;				//IIC restart enable/disable
	uint16_t	hwInterrput;			//IIC INTERRPUT SET
	uint32_t	wSdaTimeout;			//IIC SDA timeout SET
	uint32_t	wSclTimeout;            //IIC SCL timeout SET
};


typedef struct {
	
	volatile uint8_t			*pbySlaveRxBuf;			//slave  receive buffer
	volatile uint8_t			*pbySlaveTxBuf;			//slave  send buffer
	uint16_t        hwRxSize;				//receive buffer size
	uint16_t        hwTxSize;				//send buffer size	
} csi_iic_slave_t;

extern csi_iic_slave_t g_tSlave;	

 

/**
  \enum        csi_iic_speed_t
  \brief       iic speed mode
 */
typedef enum {
    IIC_BUS_SPEED_STANDARD        = 1U,     ///< Standard Speed  (<=100kHz)
    IIC_BUS_SPEED_FAST,                     ///< Fast Speed      (<=400kHz)
    IIC_BUS_SPEED_FAST_PLUS,                ///< Fast plus Speed (<=  1MHz)
	
} csi_iic_speed_t;

/**
  \enum        csi_iic_address_mode_t
  \brief       iic address mode
 */
typedef enum {
    IIC_ADDRESS_7BIT               = 0U,    ///< 7-bit address mode
    IIC_ADDRESS_10BIT                       ///< 10-bit address mode
} csi_iic_addr_mode_t;


/**
  \enum        csi_iic_event_t
  \brief       iic event signaled by iic driver
 */
typedef enum {
    IIC_EVENT_SEND_COMPLETE        = 0U,     ///< Master/slave Send finished
    IIC_EVENT_RECEIVE_COMPLETE,              ///< Master/slave Receive finished
	IIC_EVENT_SLAVE_RECEIVE_ADDR_CMD,        ///< slave Receive RESTART DET
    IIC_EVENT_ERROR_OVERFLOW,                ///< Master/slave fifo overflow error
    IIC_EVENT_ERROR_UNDERFLOW,               ///< Master/slave fifo underflow error
    IIC_EVENT_ERROR                          ///< The receive buffer was completely filled to FIFO and more data arrived. That data is lost
} csi_iic_event_t;


/** \brief initialize iic slave
 * 
 *  \param[in] ptIicBase: pointer of iic register structure
 *  \param[in] ptIicSlaveCfg: pointer of iic slave config structure
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_iic_slave_init(csp_i2c_t *ptIicBase, csi_iic_slave_config_t *ptIicSlaveCfg);

/** \brief initialize iic slave
 * 
 *  \param[in] ptIicBase: pointer of iic register structure
 *  \param[in] ptIicMasterCfg: pointer of iic master config structure
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_iic_master_init(csp_i2c_t *ptIicBase, csi_iic_master_config_t *ptIicMasterCfg);

/** \brief enable iic 
 * 
 *  \param[in] ptIicBase: pointer of iic register structure
 *  \return none
 */ 
void csi_iic_enable(csp_i2c_t *ptIicBase);

/** \brief disable iic 
 * 
 *  \param[in] ptIicBase: pointer of iic register structure
 *  \return none
 */ 
void csi_iic_disable(csp_i2c_t *ptIicBase);

/** \brief  iic  master  write 1 byte data
 * 
 *  \param[in] ptIicBase: pointer of iic register structure
 * 	\param[in] wdevaddr: Addrress of slave device
 *  \param[in] wWriteAdds: Write address
 * 	\param[in] byWriteAddrNumByte: Write address length (unit byte)
 * 	\param[in] byData: Write data
 *  \return none
 */ 
void csi_iic_write_byte(csp_i2c_t *ptIicBase,uint32_t wdevaddr, uint32_t wWriteAdds, uint8_t byWriteAddrNumByte, uint8_t byData);

/** \brief  iic  master  write n byte data
 * 
 *  \param[in] ptIicBase: pointer of iic register structure
 * 	\param[in] wdevaddr: Addrress of slave device
 *  \param[in] wWriteAdds: Write address
 * 	\param[in] byWriteAddrNumByte: Write address length (unit byte)
 * 	\param[in] pbyIicData: pointer of Write data
 * 	\param[in] byNumByteToWrite: Write data length
 *  \return none
 */ 
void csi_iic_write_nbyte(csp_i2c_t *ptIicBase,uint32_t wdevaddr, uint32_t wWriteAdds, uint8_t byWriteAddrNumByte,volatile uint8_t *pbyIicData,uint8_t byNumByteToWrite);

/** \brief  iic  master  read 1 byte data
 * 
 *  \param[in] ptIicBase: pointer of iic register structure
 * 	\param[in] wdevaddr: Addrress of slave device
 *  \param[in] wReadAdds: Read address
 * 	\param[in] byReadAddrNumByte: Read address length (unit byte)
 *  \return Read data \ref uint8_t
 */ 
uint8_t csi_iic_read_byte(csp_i2c_t *ptIicBase,uint32_t wdevaddr, uint32_t wReadAdds, uint8_t byReadAddrNumByte);

/** \brief  iic  master  read n byte data
 * 
 *  \param[in] ptIicBase: pointer of iic register structure
 * 	\param[in] wdevaddr: Addrress of slave device
 *  \param[in] wReadAdds: Read address
 * 	\param[in] byReadAddrNumByte: Read address length (unit byte)
 * 	\param[in] pbyIicData: Read the address pointer of the data storage array
 * 	\param[in] byNumByteRead: Read data length
 *  \return none
 */ 
void csi_iic_read_nbyte(csp_i2c_t *ptIicBase,uint32_t wdevaddr, uint32_t wReadAdds, uint8_t byReadAddrNumByte,volatile uint8_t *pbyIicData,uint8_t byNumByteRead);


/** \brief  IIC slave handler
 * 
 *  \param[in] ptIicBase: pointer of iic register structure
 *  \return none
 */ 
void csi_iic_slave_receive_send(csp_i2c_t *ptIicBase);


/** \brief  iic  master  read n byte data
 * 
 * 	\param[in] pbyIicRxBuf: pointer of iic RX data buffer
 *  \param[in] hwIicRxSize: Rx buffer size
 * 	\param[in] pbyIicTxBuf: pointer of iic TX data buffer
 * 	\param[in] hwIicTxSize: Tx buffer size
 *  \return none
 */ 
void csi_iic_set_slave_buffer(volatile uint8_t *pbyIicRxBuf,uint16_t hwIicRxSize,volatile uint8_t *pbyIicTxBuf,uint16_t hwIicTxSize);


#ifdef __cplusplus
}
#endif

#endif /* _DRV_IIC_H_ */
