/***********************************************************************//** 
 * \file  iic.c
 * \brief  csi iic driver
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-9-03 <td>V0.0  <td>ZJY   <td>initial
 * <tr><td> 2021-1-21 <td>V0.1  <td>ZJY   <td> modified in 110
  * <tr><td> 2021-1-22 <td>V0.2  <td>WNN   <td> move to 102
 * </table>
 * *********************************************************************
*/
#include <sys_clk.h>
#include <drv/iic.h>
#include <drv/irq.h>
#include <drv/gpio.h>
#include <drv/pin.h>
#include <drv/porting.h>
#include <drv/tick.h>

#include "csp_i2c.h"

/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/** \brief iic recie data,interrupt call 
 * 
 *  \param[in] args: pointer of iic dev
 *  \return none
 */ 
static void apt_iic_rx_handler(void *arg)
{
	CSI_PARAM_CHK_NORETVAL(arg);
    
	csi_iic_t *iic = (csi_iic_t *)arg;
    csp_i2c_t *iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
	
	uint32_t wIntStatus = csp_i2c_get_isr(iic_base);
	
	if(wIntStatus & I2C_RX_FULL_INT)
	{
		//csp_i2c_clr_all_int(iic_base);					//clear all int
		csp_i2c_clr_isr(iic_base, I2C_RX_FULL_INT);
		
		*iic->data = csp_i2c_get_data(iic_base);			//read data
		iic->size--;
		iic->data++;
		
		if(iic->size == 1)
			csp_i2c_set_data_cmd(iic_base, I2C_CMD_STOP | I2C_CMD_READ);	//send read and stop cmd 
		else if(iic->size > 0)
			csp_i2c_set_data_cmd(iic_base, I2C_CMD_READ);					//send read cmd 
		
	}
	
	if(wIntStatus & I2C_STOP_DET_INT)		//stop int
	{
		csp_i2c_wait_recv_fifo_empty(iic_base, 10000U);
		
		csp_i2c_clr_all_int(iic_base);		//clear all int
		csp_i2c_set_int(iic_base, I2C_RX_FULL_INT | I2C_STOP_DET_INT, false);		
		csp_i2c_dis(iic_base);				//disable iic
		
		if(iic->size != 0U)					//error
		{
			iic->state.error = 1U;
			iic->state.readable = 1U;
			
			if (iic->callback) 
                iic->callback(iic, IIC_EVENT_ERROR, iic->arg);
		}
		
		if(iic->size == 0U)					
		{
			iic->state.readable = 1U;
			if (iic->callback) 
                iic->callback(iic, IIC_EVENT_RECEIVE_COMPLETE, iic->arg);
		}
	}
}
/** \brief iic send data,interrupt call 
 * 
 *  \param[in] args: pointer of iic dev
 *  \return none
 */ 
static void apt_iic_tx_handler(void *arg)
{
	CSI_PARAM_CHK_NORETVAL(arg);
    
	csi_iic_t *iic = (csi_iic_t *)arg;
    csp_i2c_t *iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
	uint32_t wIntStatus = csp_i2c_get_isr(iic_base);
	
	if(wIntStatus & I2C_TX_EMPTY_INT)		//tx empty
	{
		//csp_i2c_clr_all_int(iic_base);		//clear all int
		csp_i2c_clr_isr(iic_base, I2C_TX_EMPTY_INT);
		
		if(iic->size  > 0)
		{
			if(iic->size == 1)
				csp_i2c_set_data_cmd(iic_base, *iic->data | I2C_CMD_STOP);		//send data of write and stop i2c
			else
				csp_i2c_set_data_cmd(iic_base, *iic->data);						//send data of write
		
			iic->size--;
			iic->data++;
		}
	}
	
	if(wIntStatus & I2C_STOP_DET_INT)		//stop int
	{
		csp_i2c_wait_send_fifo_empty(iic_base, 10000U);
		
		csp_i2c_clr_all_int(iic_base);		//clear all int
		csp_i2c_set_int(iic_base, I2C_TX_EMPTY_INT | I2C_STOP_DET_INT | I2C_TX_ABRT_INT | I2C_SCL_SLOW_INT,false);		
		csp_i2c_dis(iic_base);				//disable iic
		
		if(iic->size != 0U)					//error
		{
			iic->state.error = 1U;
			iic->state.writeable = 1U;
			
			if (iic->callback) 
                iic->callback(iic, IIC_EVENT_ERROR, iic->arg);
		}
		
		if(iic->size == 0U)					
		{
			iic->state.writeable = 1U;
			if (iic->callback) 
                iic->callback(iic, IIC_EVENT_SEND_COMPLETE, iic->arg);
		}
	}
	
	if(wIntStatus & I2C_SCL_SLOW_INT)								//I2C scl lock low
	{
		csp_i2c_clr_isr(iic_base, I2C_SCL_SLOW_INT);
		//rst the entire I2C system
	}
	
	if(wIntStatus & I2C_TX_ABRT_INT)								//I2C Transmit Abort
	{
		csp_i2c_clr_isr(iic_base, I2C_RESTART_DET_INT);
		if(csp_i2c_get_tx_abrt(iic_base) & TX_ABRT_SDA_S_LOW)		//iic SDA lock low
		{
			csp_i2c_recover_en(iic_base);							//recover sda
			while((csp_i2c_get_i2cenable(iic_base) >> I2C_RECOVER_POS) & 0x01ul) ;
			
			if(csp_i2c_get_status(iic_base) & I2C_REC_FAIL)			//recover sda false
			{
				//rst the entire I2C system
			}
		}
	}
}
/** \brief iic receive data,interrupt call 
 * 
 *  \param[in] args: pointer of iic dev
 *  \return none
 */ 
static void apt_iic_slave_rx_handler(void *arg)
{
	CSI_PARAM_CHK_NORETVAL(arg);
    
	csi_iic_t *iic = (csi_iic_t *)arg;
    csp_i2c_t *iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
	
	uint32_t wIntStatus = csp_i2c_get_isr(iic_base);
	
	if(wIntStatus & I2C_RX_FULL_INT)
	{
		csp_i2c_clr_isr(iic_base, I2C_RX_FULL_INT);
	
		*(iic->data + iic->size) = csp_i2c_get_data(iic_base);	//read data
		iic->size++;
	}
	
	if(wIntStatus & I2C_STOP_DET_INT)		//stop int
	{
		csp_i2c_wait_send_fifo_empty(iic_base, 10000U);
		csp_i2c_clr_all_int(iic_base);		//clear all int
		
		if(iic->size == 0U)					//error
		{
			iic->state.error = 1U;
			iic->state.readable = 1U;
	
			if (iic->callback) 
                iic->callback(iic, IIC_EVENT_ERROR, iic->arg);
		}
		else
		{
			iic->state.readable = 1U;
			//iic->arg = (void *)iic->size; 
			iic->size = 0;
			
			if (iic->callback) 
                iic->callback(iic, IIC_EVENT_RECEIVE_COMPLETE, iic->arg);
		}
	}
}
/** \brief iic send data,interrupt call 
 * 
 *  \param[in] args: pointer of iic dev
 *  \return none
 */ 
static void apt_iic_slave_tx_handler(void *arg)
{
	CSI_PARAM_CHK_NORETVAL(arg);
    
	csi_iic_t *iic = (csi_iic_t *)arg;
    csp_i2c_t *iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
	uint32_t wIntStatus = csp_i2c_get_isr(iic_base);
	
	if(wIntStatus & I2C_RD_REQ_INT)			//tx empty
	{
		csp_i2c_clr_isr(iic_base, I2C_RD_REQ_INT);					//slave send data			
		
		csp_i2c_set_data_cmd(iic_base, *(iic->data + iic->size));	//send data of write
		iic->size++;
	}
	
	if(wIntStatus & I2C_RX_FULL_INT)								//slave receive data
	{
		csp_i2c_clr_isr(iic_base, I2C_RX_FULL_INT);
		*(iic->data + iic->size) = csp_i2c_get_data(iic_base);		//read data
		iic->size++;
	}
	
	if(wIntStatus & I2C_STOP_DET_INT)								//stop int
	{
		csp_i2c_wait_recv_fifo_empty(iic_base, 10000U); 
		
		csp_i2c_clr_all_int(iic_base);		//clear all int
		
		if(iic->size == 0U)					//error
		{
			iic->state.error = 1U;
			iic->state.writeable = 1U;
			iic->size = 0;
			
			if (iic->callback) 
                iic->callback(iic, IIC_EVENT_ERROR, iic->arg);
		}
		else
		{
			iic->state.writeable = 1U;
			//iic->arg = (void *)iic->size; 
			iic->size = 0;
			
			if (iic->callback) 
				iic->callback(iic, IIC_EVENT_SEND_COMPLETE, iic->arg);
		}
	}
	
	if(wIntStatus & I2C_RESTART_DET_INT)							//slave receive and send data restart 
	{
		csp_i2c_clr_isr(iic_base, I2C_RESTART_DET_INT);
		//iic->arg = (void *)iic->size;
		iic->size = 0;
		if (iic->callback) 											//slave restart det
			iic->callback(iic, IIC_EVENT_SLAVE_RECEIVE_ADDR_CMD, iic->arg);	
	}
	
	if(wIntStatus & I2C_SCL_SLOW_INT)								//I2C scl lock low
	{
		csp_i2c_clr_isr(iic_base, I2C_SCL_SLOW_INT);
		//rst the entire I2C system
	}	
	
	if(wIntStatus & I2C_TX_ABRT_INT)								//I2C Transmit Abort
	{
		csp_i2c_clr_isr(iic_base, I2C_RESTART_DET_INT);
		if(csp_i2c_get_tx_abrt(iic_base) & TX_ABRT_SDA_S_LOW)		//iic SDA lock low
		{
			csp_i2c_recover_en(iic_base);							//recover sda
			while((csp_i2c_get_i2cenable(iic_base) >> I2C_RECOVER_POS) & 0x01ul) ;
			
			if(csp_i2c_get_status(iic_base) & I2C_REC_FAIL)			//recover sda false
			{
				//rst the entire I2C system
			}
		}
	}
}

/** \brief initialize iic data structure
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] port_idx: iic index
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_iic_init(csi_iic_t *iic, uint32_t idx)
{
	CSI_PARAM_CHK(iic, CSI_ERROR);

	csi_error_t ret = CSI_OK;
    csp_i2c_t *iic_base;
	
	if (target_get(DEV_IIC_TAG, idx, &iic->dev) != CSI_OK)
		ret = CSI_ERROR;
	else
	{
		iic->state.writeable = 1U;
		iic->state.readable  = 1U;
		iic->state.error     = 0U;
		iic->send = NULL;
		iic->receive = NULL;
		iic->rx_dma = NULL;
		iic->tx_dma = NULL;
		iic->callback = NULL;
		
		iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
		
		csi_clk_enable(&iic->dev);				//iic peripheral clk en
		csp_i2c_default_init(iic_base); 		//reset iic registers
		csp_i2c_set_scl_tout(iic_base, 0xffff);	
		csp_i2c_set_sda_tout(iic_base, 0xffff);
		csp_i2c_dis(iic_base);					//disale iic
	}
	
	return ret;
}
/** \brief de-initialize spi interface
 * 
 *  \param[in] iic: Handle of iic instance
 *  \return none
 */ 
void csi_iic_uninit(csi_iic_t *iic)
{
	CSI_PARAM_CHK_NORETVAL(iic);
	
	csp_i2c_t *iic_base;
    iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
	
	iic->state.error     = 0U;
    iic->state.writeable = 0U;
    iic->state.readable  = 0U;
    iic->send            = NULL;
    iic->receive         = NULL;
    iic->rx_dma          = NULL;
    iic->tx_dma          = NULL;
	
	csp_i2c_default_init(iic_base);
	csi_irq_disable((uint32_t)iic->dev.irq_num);
    csi_irq_detach((uint32_t)iic->dev.irq_num);
}
/** \brief  register spi interrupt callback function
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] callback: spi interrupt handle function
 *  \param[in] arg: para
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_iic_attach_callback(csi_iic_t *iic, void *callback, void *arg)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    csi_error_t ret = CSI_OK;
	
    iic->callback = callback;
    iic->arg = arg;
	
    return ret;
}
/** \brief cancel iic callback function
 * 
 *  \param[in] iic: Handle of iic instance
 *  \return none
 */ 
void csi_iic_detach_callback(csi_iic_t *iic)
{
	CSI_PARAM_CHK_NORETVAL(iic);
    
	iic->callback 	= NULL;
    iic->arg 		= NULL;
    iic->send 		= NULL;
    iic->receive	= NULL;
	iic->rx_dma     = NULL;
    iic->tx_dma     = NULL;
	
	csi_irq_disable((uint32_t)iic->dev.irq_num);
    csi_irq_detach((uint32_t)iic->dev.irq_num);
}
/** \brief config iic master or slave mode
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] mode: iic mode, master/slave
 *  \return error code \ref csi_error_t
 */ 
csi_error_t csi_iic_mode(csi_iic_t *iic, csi_iic_mode_t mode)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
	
    csp_i2c_t *iic_base;
    csi_error_t ret = CSI_ERROR;
    iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
   
	csp_i2c_dis(iic_base);		//disale iic

    //This register can be written only when the I2C is disabled
    if (mode == IIC_MODE_MASTER) 
	{
        iic->mode = IIC_MODE_MASTER;
		csp_i2c_set_mode(iic_base, I2C_MASTER);
		csp_i2c_restart_en(iic_base);
		csp_i2c_busclr_en(iic_base);
        ret = CSI_OK;
    } 
	else if (mode == IIC_MODE_SLAVE) 
	{
        iic->mode = IIC_MODE_SLAVE;
		csp_i2c_set_mode(iic_base, I2C_SLAVE);
		csp_i2c_restart_en(iic_base);
        ret = CSI_OK;
    }

    return ret;
}
/** \brief config iic addr mode,7bit or 10bit
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] addr_mode: iic addr mode, 7bit/10bit
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_iic_addr_mode(csi_iic_t *iic, csi_iic_addr_mode_t addr_mode)
{
	CSI_PARAM_CHK(iic, CSI_ERROR);
	
    csp_i2c_t *iic_base;
	csi_error_t ret = CSI_ERROR;
    iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
	
	csp_i2c_dis(iic_base);		//disale iic
	
	//This register can be written only when the I2C is disabled 
    if (addr_mode == IIC_ADDRESS_7BIT) 
	{
		csp_i2c_set_addr_mode(iic_base,I2C_ADDR_7BIT,I2C_SLAVE);
        csp_i2c_set_addr_mode(iic_base,I2C_ADDR_7BIT,I2C_MASTER);
        ret = CSI_OK;
    } 
	else if (addr_mode == IIC_ADDRESS_10BIT) 
	{
		csp_i2c_set_addr_mode(iic_base,I2C_ADDR_10BIT,I2C_SLAVE);
        csp_i2c_set_addr_mode(iic_base,I2C_ADDR_10BIT,I2C_MASTER);
        ret = CSI_OK;
    }
	
	return ret;
} 
/** \brief config iic speed
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] speed: iic speed mode 100k/400k
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_iic_speed(csi_iic_t *iic, csi_iic_speed_t speed)
{
	CSI_PARAM_CHK(iic, CSI_ERROR);
	
	
    csp_i2c_t *iic_base;
	csi_error_t ret = CSI_ERROR;
    iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
	uint32_t wIicClk;
	uint16_t hwSclL = 8;
	uint16_t hwSclH = 6;
	uint16_t hwHold = 2;
	
	csp_i2c_dis(iic_base);		//disable iic
	
	//This register can be written only when the I2C is disabled
    if(speed == IIC_BUS_SPEED_STANDARD) 
	{
        csp_i2c_set_speed(iic_base, I2C_SPEED_STAN);
        wIicClk = soc_get_pclk_freq() / 1000000U;
		if(0 == wIicClk)
		{
			hwSclH = 8;
			hwSclL = 8;
		}
		else
		{
			//if((soc_get_pclk_freq() % 1000000U))
			//{
			//	hwSclH = (wIicClk + 1) * 5;
			//	hwSclL = (wIicClk + 1) * 5;
			//}
			//else
			{
				hwSclH = wIicClk *5;
				hwSclL = wIicClk *5;
			}
		}
        csp_i2c_set_sclh(iic_base, hwSclH, I2C_SPEED_STAN);
		csp_i2c_set_scll(iic_base, hwSclL, I2C_SPEED_STAN);
		
        ret = CSI_OK;
    } 
	else if(speed == IIC_BUS_SPEED_FAST)
	{
		csp_i2c_set_speed(iic_base, I2C_SPEED_FAST);
        wIicClk = soc_get_pclk_freq() / 1000000U;
		if(0 == wIicClk)
		{
			hwSclH = 8;
			hwSclL = 8;
		}
		else
		{
			//if((soc_get_pclk_freq() % 1000000U))
			//{
			//	hwSclH = (wIicClk + 1) * 125 /100;
			//	hwSclL = (wIicClk + 1) * 125 /100;
			//}
			//else
			{
				hwSclH = wIicClk  * 125 / 100;
				hwSclL = wIicClk  * 125 / 100;
			}
		}
			
        csp_i2c_set_sclh(iic_base, hwSclH, I2C_SPEED_FAST);
		csp_i2c_set_scll(iic_base, hwSclL, I2C_SPEED_FAST);
        ret = CSI_OK;
    }
	
	hwHold = hwSclH / 3;
	if(hwHold < 2)
		hwHold = 2;
	csp_i2c_sda_setup(iic_base,hwHold);
	csp_i2c_sda_hold(iic_base,hwHold,hwHold);
	
	return ret;
}
/** \brief config iic own addr
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] own_addr: own addr(own addr of slave mode)
 *  \return error code \ref csi_error_t
 */
csi_error_t csi_iic_own_addr(csi_iic_t *iic, uint32_t own_addr)
{
	CSI_PARAM_CHK(iic, CSI_ERROR);
	
    csp_i2c_t *iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
	
	csp_i2c_dis(iic_base);		//disale iic
	
	csp_i2c_set_saddr(iic_base, own_addr);		//set own addr of slave mode
	
	return CSI_OK;
}
/** \brief send data as IIC Master
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] devaddr: addrress of slave device
 *  \param[in] data: pointer to send data buffer
 *  \param[in] size: size of data items to send
 *  \param[in] timout: unit of time delay
 *  \return error code or send real size
 */
int32_t csi_iic_master_send(csi_iic_t *iic, uint32_t devaddr, const void *data, uint32_t size, uint32_t timeout)
{
	CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
	
	//csi_error_t ret = CSI_OK;

	int32_t  iSendCnt = 0;
	uint8_t  *pbyData = (void *)data;
	
	if(0 == size || pbyData == NULL)
		iSendCnt = (int32_t)CSI_ERROR;
	else
	{	
		csp_i2c_t *iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
		csp_i2c_dis(iic_base);					//disable iic
		csp_i2c_set_taddr(iic_base, devaddr);  
		csp_i2c_en(iic_base);					//enable iic
		
		
		iSendCnt = csp_i2c_write_nbyte(iic_base, pbyData, size);
	}
	
	if(iSendCnt < 0)
		iSendCnt = (int32_t)CSI_TIMEOUT;
	
	return iSendCnt;
}
/** \brief receive data as IIC Master.
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] devaddr: addrress of slave device
 *  \param[in] data: pointer to buffer for data to receive from iic receiver
 *  \param[in] size: size of data items to send
 *  \param[in] timout: unit of time delay
 *  \return error code or send real size
 */
int32_t csi_iic_master_receive(csi_iic_t *iic, uint32_t devaddr, void *data, uint32_t size, uint32_t timeout)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
	
	//csi_error_t ret = CSI_OK;
	int32_t iRecvCnt = 0;
	uint8_t *pbyData = (void *)data;
	
    if(0 == size || pbyData == NULL)
		iRecvCnt = (int32_t)CSI_ERROR;
	else
	{
		csp_i2c_t *iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
		csp_i2c_dis(iic_base);					//disable iic
		csp_i2c_set_taddr(iic_base, devaddr);  
		csp_i2c_en(iic_base);					//enable iic
		
		iRecvCnt = csp_i2c_read_nbyte(iic_base, pbyData,size);
	}
	
	if(iRecvCnt < 0)
		iRecvCnt = (int32_t)CSI_TIMEOUT;
		
	return iRecvCnt;
}
/** \brief send data and receive data as IIC Master.
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] devaddr: addrress of slave device
 *  \param[in] send_data: pointer to buffer for data to send from iic receiver
 *  \param[in] send_size: size of data items to send
 *  \param[in] recv_data: pointer to buffer for data to receive from iic receiver
 *  \param[in] recv_size: size of data items to receive
 *  \param[in] timout: unit of time delay
 *  \return error code or send real size
 */
int32_t csi_iic_master_send_receive(csi_iic_t *iic, uint32_t devaddr, const void *send_data, uint32_t send_size, void *recv_data, uint32_t recv_size, uint32_t timeout)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(send_data, CSI_ERROR);
	CSI_PARAM_CHK(recv_data, CSI_ERROR);
	
	//csi_error_t ret = CSI_OK;
	int32_t iRecvCnt = 0;
	int32_t i = 0;
	uint8_t *pbySend = (void *)send_data;
	uint8_t *pbyRecv = (void *)recv_data;
	
	
    if(0 == send_size || pbySend == NULL || 0 == recv_size  || pbyRecv == NULL)
		iRecvCnt = (int32_t)CSI_ERROR;
	else
	{
		csp_i2c_t *iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
		csp_i2c_dis(iic_base);					//disable iic
		csp_i2c_set_taddr(iic_base, devaddr);  
		csp_i2c_en(iic_base);					//enable iic
		
		for(i = 0; i < send_size; i++)
		{
			if(i == (send_size -1))
				csp_i2c_set_data_cmd(iic_base, pbySend[i] | I2C_CMD_RESTART0);	//send data of write and restart iic
			else
				csp_i2c_set_data_cmd(iic_base, pbySend[i]);						//send data of write
		}
		
		iRecvCnt = csp_i2c_read_nbyte(iic_base, pbyRecv, recv_size);
	}
	
	if(iRecvCnt < 0)
		iRecvCnt = (int32_t)CSI_TIMEOUT;
		
	return iRecvCnt;
}

//
static csi_error_t iic_master_send_intr(csi_iic_t *iic, uint32_t devaddr, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    
	csi_error_t ret = CSI_OK;

    if ((data == NULL) || (size == 0U)) 
        ret = CSI_ERROR;
	else 
	{
        if (iic->state.writeable == 0U) 
            ret = CSI_BUSY;
        else 
		{
            csp_i2c_t *iic_base;
            iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);

            iic->data = (uint8_t *)data;
            iic->size = size;
			iic->state.writeable = 0U;

			csp_i2c_dis(iic_base);						//disable iic
			csp_i2c_set_taddr(iic_base, devaddr);  
			csp_i2c_clr_all_int(iic_base);
			csp_i2c_set_int(iic_base,I2C_TX_EMPTY_INT | I2C_STOP_DET_INT | I2C_TX_ABRT_INT | I2C_SCL_SLOW_INT ,true);
			csp_i2c_set_tx_flsel(iic_base,1);
			csp_i2c_en(iic_base);						//enable iic
        }
    }

    return ret;
}
/** \brief send data as IIC Master, non-blocking mode
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] devaddr: addrress of slave device
 *  \param[in] data: pointer to send data buffer
 *  \param[in] size: size of data items to send
 *  \return error code or send real size
 */
csi_error_t csi_iic_master_send_async(csi_iic_t *iic, uint32_t devaddr, const void *data, uint32_t size)
{
	CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
    
	csi_error_t ret = CSI_OK;
    csi_iic_master_send_async_t send_func;
	
	if(iic->send) 
	{
        send_func = iic->send;
        send_func(iic, devaddr, data, size);
    } 
	else
	{
		csi_irq_attach((uint32_t)iic->dev.irq_num, &apt_iic_tx_handler, &iic->dev);
		csi_irq_enable((uint32_t)iic->dev.irq_num);
        ret = iic_master_send_intr(iic, devaddr, data, size);
	}
	
	return ret;
}
//
static csi_error_t iic_master_receive_intr(csi_iic_t  *iic, uint32_t devaddr, void *data, uint32_t size)
{
	CSI_PARAM_CHK(iic, CSI_ERROR);
	CSI_PARAM_CHK(data, CSI_ERROR);
	 
	csi_error_t ret = CSI_OK;
	 
	if ((data == NULL) || (size == 0U)) 
        ret = CSI_ERROR;
	else
	{
		csp_i2c_t *iic_base;
		iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
		
		iic->data = (uint8_t *)data;
		iic->size = size;
		iic->state.readable      = 0U;
		iic->priv = (void *)size;       				//< iic->priv used to record the transmission value
		
		csp_i2c_dis(iic_base);							//disable iic
		csp_i2c_set_taddr(iic_base, devaddr);  
		csp_i2c_clr_all_int(iic_base);
		csp_i2c_set_int(iic_base,I2C_RX_FULL_INT | I2C_STOP_DET_INT,true);
		csp_i2c_set_rx_flsel(iic_base,0);
		
		csp_i2c_en(iic_base);							//enable iic
		if(iic->size < 2)
		{
			ret = csp_i2c_read_nbyte(iic_base, iic->data, 1);
			csp_i2c_set_int(iic_base,I2C_RX_FULL_INT | I2C_STOP_DET_INT,false);		
			csp_i2c_dis(iic_base);
		}
		else
		{
			csp_i2c_set_data_cmd(iic_base, I2C_CMD_READ);		//send read cmd 
			//while(!(csp_i2c_get_status(iic_base) & I2C_RFNE));
			//while(!(csp_i2c_get_status(iic_base) & I2C_BUSY));
			//csp_i2c_set_data_cmd(iic_base, I2C_CMD_READ);		//send read cmd 
		}
	}
	
	return ret;
}
/** \brief receive data as IIC Master, non-blocking mode
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] devaddr: addrress of slave device
 *  \param[in] data: pointer to receive data buffer
 *  \param[in] size: size of data items to receive
 *  \return error code or send real size
 */
csi_error_t csi_iic_master_receive_async(csi_iic_t  *iic, uint32_t devaddr, void *data, uint32_t size)
{
	CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
	csi_error_t ret = CSI_OK;
	
	csi_irq_attach((uint32_t)iic->dev.irq_num, &apt_iic_rx_handler, &iic->dev);
    csi_irq_enable((uint32_t)iic->dev.irq_num);
    ret = iic_master_receive_intr(iic, devaddr, data, size);
	
	return ret;
}
/** \brief send data as IIC Master
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] devaddr: addrress of slave device
 *  \param[in] memaddr: internal addr of device
 *  \param[in] memaddr_size: Internal addr mode of device
 *  \param[in] data: pointer to send data buffer
 *  \param[in] size: size of data items to send
 *  \param[in] timout: unit of time delay
 *  \return error code or send real size
 */
int32_t csi_iic_mem_send(csi_iic_t *iic, uint32_t devaddr, uint16_t memaddr, csi_iic_mem_addr_size_t memaddr_size, const void *data, uint32_t size, uint32_t timeout)
{
	CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
	
	//csi_error_t ret = CSI_OK;
	int32_t iSendCnt = 0;
	uint8_t *pbyData = (void *)data;
	
	if(0 == size || pbyData == NULL)
		iSendCnt = (int32_t)CSI_ERROR;
	else
	{
		csp_i2c_t *iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
		csp_i2c_dis(iic_base);					//disable iic
		csp_i2c_set_taddr(iic_base, devaddr);  
		csp_i2c_en(iic_base);					//disable iic
		
		iSendCnt = csp_i2c_write_mem_nbyte(iic_base, memaddr, (i2c_addr_size_e)memaddr_size, pbyData, size);
		
	}
		
	if(iSendCnt < 0)
		iSendCnt = (int32_t)CSI_TIMEOUT;
		
	return  iSendCnt;
}
/** \brief receive data as IIC Master
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] devaddr: addrress of slave device
 *  \param[in] memaddr: internal addr of device
 *  \param[in] memaddr_size: Internal addr mode of device
 *  \param[in] data: pointer to send data buffer
 *  \param[in] size: size of data items to send
 *  \param[in] timout: unit of time delay
 *  \return error code or send real size
 */
int32_t csi_iic_mem_receive(csi_iic_t *iic, uint32_t devaddr, uint16_t memaddr, csi_iic_mem_addr_size_t memaddr_size, void *data, uint32_t size, uint32_t timeout)
{
	CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
	
    //csi_error_t ret = CSI_OK;
	int32_t iRecvCnt = 0;
	uint8_t *pbyData = (void *)data;
	
	if(0 == size || pbyData == NULL)
		iRecvCnt = (int32_t)CSI_ERROR;
	else
	{
		csp_i2c_t *iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
		csp_i2c_dis(iic_base);					//disable iic
		csp_i2c_set_taddr(iic_base, devaddr);  
		csp_i2c_en(iic_base);					//disable iic
		
		iRecvCnt = csp_i2c_read_mem_nbyte(iic_base, memaddr, (i2c_addr_size_e)memaddr_size, pbyData,size);
	}
	
	if(iRecvCnt < 0)
		iRecvCnt = (int32_t)CSI_TIMEOUT;
	
	return iRecvCnt;
}
/** \brief send data as IIC Slave
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] data: pointer to send data buffer
 *  \param[in] size: size of data items to send
 *  \param[in] timout: unit of time delay
 *  \return error code or send real size
 */
int32_t csi_iic_slave_send(csi_iic_t *iic, const void *data, uint32_t size, uint32_t timeout)
{
	CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
	/*
    csi_error_t ret = CSI_OK;
	int32_t  iSendCnt = 0;
	uint8_t  *pbyData = (void *)data;
	
	if(0 == size || pbyData == NULL)
		iSendCnt = (int32_t)CSI_ERROR;
	else
	{
		csp_i2c_t *iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
		csp_i2c_en(iic_base);					//enable iic
		
		while(1)
		{
			if(csp_i2c_get_risr(iic_base) & I2C_RD_REQ_INT)
			{
				csp_i2c_clr_all_int(iic_base);
				csp_i2c_set_data_cmd(iic_base, *pbyData);				//send data of write
		
				ret = csp_i2c_wait_send(iic_base, I2C_SEND_TIMEOUT);	//wait send
				if(ret <0)
					break;
				
				pbyData ++;
				iSendCnt ++;
				if(iSendCnt == size)
					break;
			}
		}
	}
	
	if(ret < 0)
		iSendCnt = (int32_t)CSI_TIMEOUT;
		
	return iSendCnt;
	*/
	return CSI_UNSUPPORTED;
}
/** \brief receive data as IIC Slave.
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] data: pointer to receive data buffer
 *  \param[in] size: size of data items to receive
 *  \param[in] timout: unit of time delay-
 *  \return error code or receive real size
 */
int32_t csi_iic_slave_receive(csi_iic_t *iic, void *data, uint32_t size, uint32_t timeout)
{
	CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
	/*
    csi_error_t ret = CSI_OK;
	int32_t  iRecvCnt = 0;
	uint8_t  *pbyData = (void *)data;
	
	if(0 == size || pbyData == NULL)
		iRecvCnt = (int32_t)CSI_ERROR;
	else
	{
		csp_i2c_t *iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
		csp_i2c_en(iic_base);		//enable iic
		
		while(1)
		{
			ret = csp_i2c_wait_recv(iic_base,I2C_SEND_TIMEOUT);
			*pbyData = csp_i2c_get_data(iic_base);
			pbyData ++;
			iRecvCnt ++;
			
			if(ret < 0)
				break;
		}
	}
	
	if(ret < 0)
		iRecvCnt = (int32_t)CSI_TIMEOUT;
	
	return iRecvCnt;
	 */
	return CSI_UNSUPPORTED;
}

static csi_error_t iic_slave_send_intr(csi_iic_t *iic, const void *data, uint32_t size)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    
	csi_error_t ret = CSI_OK;

    if ((data == NULL) || (size == 0U)) 
        ret = CSI_ERROR;
    else 
	{
        if (iic->state.writeable == 0U)
            ret = CSI_BUSY;
        else 
		{
			
			csp_i2c_t *iic_base;
            iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);

            iic->data = (uint8_t *)data;
            iic->size = 0;
			iic->state.writeable = 0U;

			csp_i2c_dis(iic_base);						//disable iic
			csp_i2c_clr_all_int(iic_base);
			csp_i2c_set_int(iic_base,I2C_RD_REQ_INT | I2C_RX_FULL_INT | I2C_STOP_DET_INT | I2C_RESTART_DET_INT | I2C_TX_ABRT_INT | I2C_SCL_SLOW_INT,true);
			csp_i2c_set_rx_flsel(iic_base,0);
			csp_i2c_set_tx_flsel(iic_base,0);
			csp_i2c_en(iic_base);						//enable iic
        }
    }

    return ret;
}
/** \brief transmitting data as IIC Slave, non-blocking mode
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] data: pointer to send data buffer
 *  \param[in] size: size of data items to send
 *  \return \ref csi_error_t
 */
csi_error_t csi_iic_slave_send_async(csi_iic_t *iic, const void *data, uint32_t size)
{
	CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
    
	csi_error_t ret = CSI_OK;
    csi_iic_slave_send_async_t send_func;
	
	if (iic->send) 
	{
        send_func = iic->send;
        send_func(iic, data, size);
    } 
	else 
	{
        csi_irq_attach((uint32_t)iic->dev.irq_num, &apt_iic_slave_tx_handler, &iic->dev);
        csi_irq_enable((uint32_t)iic->dev.irq_num);
        ret = iic_slave_send_intr(iic, data, size);
    }
	
	return ret;
}

static csi_error_t iic_slave_receive_intr(csi_iic_t *iic, void *data, uint32_t size)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
   
 csi_error_t ret = CSI_OK;

    if ((data == NULL))
        ret =  CSI_ERROR;
    else 
	{
        if (iic->state.readable == 0U)
            ret = CSI_BUSY;
        else 
		{
			csp_i2c_t *iic_base;
			iic_base = (csp_i2c_t *)HANDLE_REG_BASE(iic);
		
			iic->data = (uint8_t *)data;
			iic->size = 0;
			iic->state.readable = 0U;
		
			csp_i2c_dis(iic_base);					//disable iic
			csp_i2c_clr_all_int(iic_base);
			csp_i2c_set_int(iic_base,I2C_RX_FULL_INT | I2C_STOP_DET_INT ,true);
			csp_i2c_set_rx_flsel(iic_base,0);
			csp_i2c_en(iic_base);					//enable iic
        }
    }

    return ret;
}
/** \brief receive data as IIC Slave, non-blocking mode
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] data: pointer to receive data buffer
 *  \param[in] size: size of data items to receive
 *  \return \ref csi_error_t
 */
 csi_error_t csi_iic_slave_receive_async(csi_iic_t *iic, void *data, uint32_t size)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(data, CSI_ERROR);
    CSI_PARAM_CHK(size, CSI_ERROR);
    
	csi_error_t ret = CSI_OK;
    csi_iic_slave_receive_async_t receive_func;

    if (iic->receive) 
	{
        receive_func = iic->receive;
        receive_func(iic, data, size);
    } 
	else 
	{
        csi_irq_attach((uint32_t)iic->dev.irq_num, &apt_iic_slave_rx_handler, &iic->dev);
        csi_irq_enable((uint32_t)iic->dev.irq_num);
        iic_slave_receive_intr(iic, data, size);
    }

    return ret;
}

/** \brief       config iic stop to generate
  * \param[in]   iic        iic handle to operate.
  * \param[in]   enable     Transfer operation is pending - Stop condition will not be generated.
  * \return      \ref csi_error_t
*/
csi_error_t csi_iic_xfer_pending(csi_iic_t *iic, bool enable)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    csi_error_t ret = CSI_UNSUPPORTED;
    
	return ret;
}

/** \brief       link DMA channel to iic device
 * 
 * \param[in]   iic handle to operate.
 * \param[in]   tx_dma the DMA channel handle for send, when it is NULL means to unlink the channel
 * \param[in]   rx_dma the DMA channel handle for receive, when it is NULL means to unlink the channel
 * \return      \ref csi_error_t
*/
csi_error_t csi_iic_link_dma(csi_iic_t *iic, csi_dma_ch_t *tx_dma, csi_dma_ch_t *rx_dma)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    csi_error_t ret = CSI_UNSUPPORTED;
	/*
    if (iic->callback != NULL) {
        if (iic->mode == IIC_MODE_MASTER) {
            //the hardware only support master send with dma and master receive only use interrupt
            if (tx_dma != NULL) {
                tx_dma->parent = iic;
                ret = csi_dma_ch_alloc(tx_dma, -1, -1);

                if (ret == CSI_OK) {
                    csi_dma_ch_attach_callback(tx_dma, dw_iic_dma_event_cb, NULL);
                    iic->tx_dma = tx_dma;
                    iic->send = iic_master_send_dma;
                } else {
                    tx_dma->parent = NULL;
                }
            } else {
                if (iic->tx_dma) {
                    csi_dma_ch_detach_callback(iic->tx_dma);
                    csi_dma_ch_free(iic->tx_dma);
                    iic->send = iic_master_send_intr;
                    iic->tx_dma = NULL;
                }

                ret = CSI_OK;
            }
        } else if (iic->mode == IIC_MODE_SLAVE) {
            //the hardware only support slave receive with dma and slave send only use interrupt
            if (rx_dma != NULL) {
                rx_dma->parent = iic;
                ret = csi_dma_ch_alloc(rx_dma, -1, -1);

                if (ret == CSI_OK) {
                    iic->rx_dma = rx_dma;
                    csi_dma_ch_attach_callback(rx_dma, dw_iic_dma_event_cb, NULL);
                    iic->receive = iic_slave_receive_dma;
                } else {
                    rx_dma->parent = NULL;
                }
            } else {
                if (iic->rx_dma) {
                    csi_dma_ch_detach_callback(iic->rx_dma);
                    csi_dma_ch_free(iic->rx_dma);
                    iic->receive = iic_slave_receive_intr;
                    iic->rx_dma = NULL;
                }

                ret = CSI_OK;
            }
        }
    }
	*/
	
    return ret;
}

/** \brief get transfer status; This function is non-blocking,\ref iic_event_e is signaled when transfer completes or error happens.
 * 
 *  \param[in] iic: Handle of iic instance
 *  \param[in] state: state iic state
 *  \return \ref csi_error_t
 */
csi_error_t csi_iic_get_state(csi_iic_t *iic, csi_state_t *state)
{
    CSI_PARAM_CHK(iic, CSI_ERROR);
    CSI_PARAM_CHK(state, CSI_ERROR);
    csi_error_t ret = CSI_OK;
    memcpy(state, &(iic->state), sizeof(csi_state_t));
    
	return ret;
}