/***********************************************************************//** 
 * \file  csp_i2c.c
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

/* Includes ---------------------------------------------------------------*/
#include "csp_i2c.h"
#include "csp_common.h"

/* Private macro-----------------------------------------------------------*/
/* externs function--------------------------------------------------------*/
/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/

/** \brief i2c reg default init
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \return none
 */
void csp_i2c_default_init(csp_i2c_t *ptI2cBase)
{
	ptI2cBase->I2CENABLE = 0;
    ptI2cBase->IMCR  	 = 0;
    ptI2cBase->ICR   	 = 0x7FFF;
} 
/** \brief i2c interrupt config
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] eI2cInt: int num
 *  \param[in] bEnable: enable,disable
 *  \return none
 */ 
void csp_i2c_set_int(csp_i2c_t *ptI2cBase,i2c_int_e eI2cInt, bool bEnable)
{
	if(bEnable)
		ptI2cBase->IMCR |= eI2cInt;
	else
		ptI2cBase->IMCR &= ~eI2cInt;

}
/** \brief i2c mode config, master/slave
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] eMode: I2C_MASTER/I2C_SLAVE
 *  \return none
 */ 
void csp_i2c_set_mode(csp_i2c_t *ptI2cBase, i2c_mode_e eMode)
{
	if(eMode == I2C_MASTER)
	{
		ptI2cBase->CR |= eMode;
	}
	else if(eMode == I2C_SLAVE)
	{
		ptI2cBase->CR &=  ~I2C_MODE_MSK;
	}
}
/** \brief i2c addr bit mode config, master/slave 7/10bit
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] eAddrMode: i2c addr bit mode
 *  \param[in] eMode: I2C_MASTER,I2C_SLAVE
 *  \return none
 */ 
void csp_i2c_set_addr_mode(csp_i2c_t *ptI2cBase, i2c_addr_mode_e eAddrMode, i2c_mode_e eMode)
{
	if(eMode == I2C_MASTER)
	{
		if(eAddrMode == I2C_ADDR_7BIT)
		{
			ptI2cBase->CR &= ~I2C_ADDR_BIT_MASTER_MSK;
		}
		else if(eAddrMode == I2C_ADDR_10BIT)
		{
			ptI2cBase->CR |= (eAddrMode << I2C_ADDR_BIT_MASTER_POS);
		}
	}
	else if(eMode == I2C_SLAVE)
	{
		if(eAddrMode == I2C_ADDR_7BIT)
		{
			ptI2cBase->CR &= ~I2C_ADDR_BIT_SLAVE_MSK;
		}
		else if(eAddrMode == I2C_ADDR_10BIT)
		{
			ptI2cBase->CR |= (eAddrMode << I2C_ADDR_BIT_SLAVE_POS);
		}
	}
}
/** \brief set i2c sclh
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] hwSclH: clk high time
 *  \param[in] eSpeed: standard/fast
 *  \return none
 */ 
void csp_i2c_set_sclh(csp_i2c_t *ptI2cBase, uint16_t hwSclH, i2c_speed_e eSpeed)
{
	if(eSpeed == I2C_SPEED_STAN)
	{
		ptI2cBase->SS_SCLH = hwSclH;
	}
	else if(eSpeed == I2C_SPEED_FAST)
	{
		ptI2cBase->FS_SCLH = hwSclH;
	}
}
/** \brief set i2c scll
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] hwSclL: clklow time
 *  \param[in] eSpeed: standard/fast
 *  \return none
 */ 
void csp_i2c_set_scll(csp_i2c_t *ptI2cBase, uint16_t hwSclL, i2c_speed_e eSpeed)
{
	if(eSpeed == I2C_SPEED_STAN)
	{
		ptI2cBase->SS_SCLL = hwSclL;
	}
	else if(eSpeed == I2C_SPEED_FAST)
	{
		ptI2cBase->FS_SCLL = hwSclL;
	}
}
/** \brief i2c master mode config
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] eSpeed: fast,standard
 *  \param[in] eAddrBit: master addr bit mode(7/10bit)
 *  \param[in] hwTaddr: target addr of master mode
 *  \param[in] hwSclH: SCL high time 
 *  \param[in] hwSclL: SCL low time 
 *  \return none
 */ 
void csp_i2c_set_master(csp_i2c_t *ptI2cBase, i2c_speed_e eSpeed, i2c_addr_mode_e eAddrBit,
					uint16_t hwTaddr, uint16_t hwSclH, uint16_t hwSclL)
{
	ptI2cBase->I2CENABLE = (ptI2cBase->I2CENABLE & (~I2C_ENABLE_MSK));		//disable i2c
	ptI2cBase->CR = (ptI2cBase->CR & 0xfffff000) | I2C_MASTER | (eSpeed << I2C_SPEED_POS) | 
						(eAddrBit << I2C_ADDR_BIT_MASTER_POS) | (I2C_BUS_CLR_EN << I2C_BUS_CLR_POS) | (I2C_RESTART_EN << I2C_RESTART_POS);
	ptI2cBase->TADDR = (hwTaddr & 0xfff);
	
	if(eSpeed == I2C_SPEED_STAN)
	{
		ptI2cBase->SS_SCLH = hwSclH;
		ptI2cBase->SS_SCLL = hwSclL;
	}
	else if(eSpeed == I2C_SPEED_FAST)
	{
		ptI2cBase->FS_SCLH = hwSclH;
		ptI2cBase->FS_SCLL = hwSclL;
	}
}
/** \brief i2c slave mode config
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] eSpeed: fast,standard
 *  \param[in] eAddrBit: slave addr bit mode(7/10bit)
 *  \param[in] hwTaddr: target addr of master mode
 *  \param[in] hwSclH: SCL high time 
 *  \param[in] hwSclL: SCL low time 
 *  \return none
 */ 
 void csp_i2c_set_slave(csp_i2c_t *ptI2cBase, i2c_speed_e eSpeed, i2c_addr_mode_e eAddrBit,
					uint16_t hwSaddr, uint16_t hwSclH, uint16_t hwSclL)
{
	ptI2cBase->I2CENABLE = (ptI2cBase->I2CENABLE & (~I2C_ENABLE_MSK));		//disable i2c
	ptI2cBase->CR = (ptI2cBase->CR & 0xfffff000) | (eSpeed << I2C_SPEED_POS) | (eAddrBit << I2C_ADDR_BIT_SLAVE_POS) | 
				(I2C_STOPDET_IFADD << I2C_STOPDET_POS);
	ptI2cBase->SADDR = (hwSaddr & 0x3ff);
	
	if(eSpeed == I2C_SPEED_STAN)
	{
		ptI2cBase->SS_SCLH = hwSclH;
		ptI2cBase->SS_SCLL = hwSclL;
	}
	else if(eSpeed == I2C_SPEED_FAST)
	{
		ptI2cBase->FS_SCLH = hwSclH;
		ptI2cBase->FS_SCLL = hwSclL;
	}				
}
/** \brief i2c fifo threshold level set
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] byRxFL: rx fifo threshold level
 *  \param[in] byTxFL: rx fifo threshold level
 *  \return none
 */ 
void csp_i2c_set_flsel(csp_i2c_t *ptI2cBase, uint8_t byRxFL, uint8_t byTxFL)
{
	ptI2cBase->RX_FLSEL =  byRxFL; 
	ptI2cBase->TX_FLSEL =  byTxFL; 
}
/** \brief enable i2c
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \return none
 */ 
void csp_i2c_en(csp_i2c_t *ptI2cBase)
{
	ptI2cBase->I2CENABLE |= I2C_ENABLE;
	while((ptI2cBase->STATUS & I2C_ENABLE_SR) != I2C_ENABLE_SR);
}
/** \brief disable i2c
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \return none
 */ 
void csp_i2c_dis(csp_i2c_t *ptI2cBase)
{
	ptI2cBase->I2CENABLE = (ptI2cBase->I2CENABLE & (~ I2C_ENABLE_MSK));
	while((ptI2cBase->STATUS & I2C_ENABLE_SR) == I2C_ENABLE_SR);
}
/** \brief i2c wait receive fifo empty 
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] wTimeOut: time of timeout 
 *  \return data of read or error code
 */
void csp_i2c_wait_recv_fifo_empty(csp_i2c_t *ptI2cBase,uint32_t wTimeOut)
{
	//int iRet = 0;
	uint32_t wTimeCnt = 0;

	do{
		wTimeCnt ++;
		if(wTimeCnt >= wTimeOut)
			break;
		
	}while(csp_i2c_get_rx_fifo_num(ptI2cBase));
}
/** \brief i2c wait send fifo empty 
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] wTimeOut: time of timeout 
 *  \return data of read or error code
 */
void csp_i2c_wait_send_fifo_empty(csp_i2c_t *ptI2cBase,uint32_t wTimeOut)
{
	//int iRet = 0;
	uint32_t wTimeCnt = 0;
	
	do{
		wTimeCnt ++;
		if(wTimeCnt >= wTimeOut)
			break;;
		
	}while(csp_i2c_get_tx_fifo_num(ptI2cBase));
	
}
/** \brief i2c send wait
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] wTimeOut: time of timeout 
 *  \return data of read or error code
 */
int csp_i2c_wait_send(csp_i2c_t *ptI2cBase,uint32_t wTimeOut)
{
	int iRet = 0;
	uint32_t wTimeCnt = 0;
	
	while(!(csp_i2c_get_status(ptI2cBase) & I2C_BUSY))	//i2c busy, wait
	{
		wTimeCnt ++;
		if(wTimeCnt >= wTimeOut)
		{
			csp_i2c_dis(ptI2cBase);						//clear fifo and stop i2c	
			csp_i2c_en(ptI2cBase);
			return -1;
		}
	}
	
	wTimeCnt = 0;
	while(!(csp_i2c_get_status(ptI2cBase) & I2C_TFNF)) //i2c tx fifo full, wait
	{
		wTimeCnt ++;
		if(wTimeCnt >= wTimeOut)
		{
			csp_i2c_dis(ptI2cBase);						//clear fifo and stop i2c		
			csp_i2c_en(ptI2cBase);
			return -1;
		}
	}
	
	return iRet;
}
/** \brief i2c receive wait
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] wTimeOut: time of timeout 
 *  \return data of read or error code
 */
int csp_i2c_wait_recv(csp_i2c_t *ptI2cBase,uint32_t wTimeOut)
{
	int iRet = 0;
	uint32_t wTimeCnt = 0;
	
	do{
		wTimeCnt ++;
		if(wTimeCnt >= wTimeOut)
		{
			csp_i2c_dis(ptI2cBase);							//clear fifo and stop i2c	
			csp_i2c_en(ptI2cBase);
			return -1;
		}
	}while(!(csp_i2c_get_status(ptI2cBase) & I2C_BUSY));	//i2c busy, wait
	
	wTimeCnt = 0;
	do{
		wTimeCnt ++;
		if(wTimeCnt >= wTimeOut)
		{
			csp_i2c_dis(ptI2cBase);							//clear fifo and stop i2c	
			csp_i2c_en(ptI2cBase);
			return -1;
		}
	}while(!(csp_i2c_get_status(ptI2cBase) & I2C_RFNE));	//rx fifo empty, wait	
	
	return iRet;
}
/** \brief i2c write one byte
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] byData: data of write
 *  \return error/success(-1/0)
 */ 
int csp_i2c_write_byte(csp_i2c_t *ptI2cBase, uint8_t byData)
{
	int iRet = 0;
	
	csp_i2c_set_data_cmd(ptI2cBase, byData | I2C_CMD_STOP);	//send one byte and stop i2c
	iRet = csp_i2c_wait_send(ptI2cBase, I2C_SEND_TIMEOUT);	//wait send
	
	return iRet;
}
/** \brief i2c write n byte
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] pbyData: pointer of write data buf
 *  \param[in] wWriteNum: num of write
 *  \return write size or error code
 */ 
int csp_i2c_write_nbyte(csp_i2c_t *ptI2cBase, uint8_t *pbyData, uint32_t wWriteNum)
{
	int iRet = 0;
	uint32_t i;

	//csp_i2c_set_data_cmd(ptI2cBase, byWrAddr | I2C_CMD_WRITE);			//send addr of data
	for(i = 0; i < wWriteNum; i++)
	{
		if(i == (wWriteNum -1))
			csp_i2c_set_data_cmd(ptI2cBase, pbyData[i] | I2C_CMD_STOP);	//send data of write and stop i2c
		else
			csp_i2c_set_data_cmd(ptI2cBase, pbyData[i]);				//send data of write
	
		iRet = csp_i2c_wait_send(ptI2cBase, I2C_SEND_TIMEOUT); 			//wait send
		if(iRet < 0)	
			break;
	}
	
	if(iRet >= 0)
		iRet = i;
		
	return iRet;
}
/** \brief i2c read one byte
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \return data of read or error code
 */
int csp_i2c_read_byte(csp_i2c_t *ptI2cBase)
{
	int iRet = 0;
	uint8_t byVal;
	
	csp_i2c_set_data_cmd(ptI2cBase, I2C_CMD_READ | I2C_CMD_STOP);	//send read and stop cmd 
	
	iRet = csp_i2c_wait_recv(ptI2cBase, I2C_RECV_TIMEOUT);			//wait i2c recv
	byVal = csp_i2c_get_data(ptI2cBase);
	
	if(iRet >= 0)
		iRet = byVal;

	return iRet;
}
/** \brief i2c read n byte
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] pbyData: pointer of read data buf
 *  \param[in] wReadNum: num of read
 *  \return receive size or error code
 */
int csp_i2c_read_nbyte(csp_i2c_t *ptI2cBase, uint8_t *pbyData, uint32_t wReadNum)
{
	int iRet = 0;
	uint32_t i;
	
	for(i = 0; i < wReadNum; i++)
	{
		if(i == (wReadNum - 1))
			csp_i2c_set_data_cmd(ptI2cBase, I2C_CMD_STOP | I2C_CMD_READ);	//send read and stop cmd 
		else
			csp_i2c_set_data_cmd(ptI2cBase, I2C_CMD_READ);					//send read cmd 
		
		iRet = csp_i2c_wait_recv(ptI2cBase, I2C_RECV_TIMEOUT);				//wait i2c recv
		*(pbyData + i) = csp_i2c_get_data(ptI2cBase);						//read data
		
		if(iRet < 0)
			break;
	}
	
	if(iRet >= 0)
		iRet = i;
		
	return iRet;
}

/** \brief i2c read one byte
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] pbySend: pointer of read data buf
 *  \param[in] wWriteNum: num of read
 *  \param[in] pbyRecv: pointer of read data buf
 *  \param[in] wReadNum: num of read
 *  \return receive size or error code
 */
int csp_i2c_write_read_nbyte(csp_i2c_t *ptI2cBase, uint8_t *pbySend, uint32_t wWriteNum, uint8_t *pbyRecv, uint32_t wReadNum)
{
	int iRet = 0;
	uint32_t i;
	
	for(i = 0; i < wWriteNum; i++)
	{
		if(i == (wWriteNum -1))
			csp_i2c_set_data_cmd(ptI2cBase, pbySend[i] | I2C_CMD_RESTART1);	//send data of write and restart iic
		else
			csp_i2c_set_data_cmd(ptI2cBase, pbySend[i]);					//send data of write
	}
	
	iRet = csp_i2c_read_nbyte(ptI2cBase, pbyRecv, wReadNum);
	
		iRet = i;
		
	return iRet;
}

/** \brief i2c write mem(eeprom)one byte
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] byWrAddr: mem addr of write data
 *  \param[in] eAddrSize: mem addr size, 8bit/16bit
 *  \param[in] byData: data of write
 *  \return error/success(-1/0)
 */ 
int csp_i2c_write_mem_byte(csp_i2c_t *ptI2cBase, uint16_t hwWrAddr, i2c_addr_size_e eAddrSize, uint8_t byData)
{
	int iRet = 0;
	
	if(eAddrSize == AADR_8BIT)
		csp_i2c_set_data_cmd(ptI2cBase, (uint8_t)hwWrAddr);			//send addr of one byte
	else
	{
		csp_i2c_set_data_cmd(ptI2cBase, (uint8_t)(hwWrAddr >> 8));	//send addr of high byte
		csp_i2c_set_data_cmd(ptI2cBase, (uint8_t)hwWrAddr);			//send addr of low byte
	}
	
	csp_i2c_set_data_cmd(ptI2cBase, byData | I2C_CMD_STOP);			//send data of data and stop i2c
	iRet = csp_i2c_wait_send(ptI2cBase, I2C_SEND_TIMEOUT);			//wait send
	
	return iRet;
}
/** \brief i2c write n byte
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] hwWrAddr: mem addr of write data
 *  \param[in] eAddrSize: mem addr size, 8bit/16bit
 *  \param[in] pbyData: pointer of write data buf
 *  \param[in] wWriteNum: num of write
 *  \return write size or error code
 */ 
int csp_i2c_write_mem_nbyte(csp_i2c_t *ptI2cBase, uint16_t hwWrAddr, i2c_addr_size_e eAddrSize, uint8_t *pbyData, uint32_t wWriteNum)
{
	int iRet = 0;
	uint32_t i;

	if(eAddrSize == AADR_8BIT)
		csp_i2c_set_data_cmd(ptI2cBase, (uint8_t)hwWrAddr);				//send addr of one byte
	else
	{
		csp_i2c_set_data_cmd(ptI2cBase, (uint8_t)(hwWrAddr >> 8));		//send addr of high byte
		csp_i2c_set_data_cmd(ptI2cBase, (uint8_t)hwWrAddr);				//send addr of low byte
	}
	
	for(i = 0; i < wWriteNum; i++)
	{
		if(i == (wWriteNum -1))
			csp_i2c_set_data_cmd(ptI2cBase, pbyData[i] | I2C_CMD_STOP);	//send data of write and stop i2c
		else
			csp_i2c_set_data_cmd(ptI2cBase, pbyData[i]);				//send data of write
		
		iRet = csp_i2c_wait_send(ptI2cBase, I2C_SEND_TIMEOUT); 			//wait send
		if(iRet < 0)	
			break;
	}
	
	if(iRet >= 0)
		iRet = i;
		
	return iRet;
}
/** \brief i2c read mem(eeprom)one byte
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] byRdAddr: addr of read data
 *  \param[in] eAddrSize: mem addr mode, 8bit/16bit
 *  \return data of read or error code
 */
int csp_i2c_read_mem_byte(csp_i2c_t *ptI2cBase,uint16_t hwRdAddr, i2c_addr_size_e eAddrSize)
{
	int iRet = 0;
	uint8_t byVal;
	
	if(eAddrSize == AADR_8BIT)
	{
		csp_i2c_set_data_cmd(ptI2cBase, (uint8_t)hwRdAddr | I2C_CMD_RESTART1);	//send addr of one byte
		csp_i2c_set_data_cmd(ptI2cBase, I2C_CMD_READ | I2C_CMD_STOP);			//send read and stop cmd 
	}
	else
	{
		csp_i2c_set_data_cmd(ptI2cBase, (uint8_t)(hwRdAddr >> 8));				//send addr of high byte
		csp_i2c_set_data_cmd(ptI2cBase, (uint8_t)hwRdAddr);						//send addr of low byte
		csp_i2c_set_data_cmd(ptI2cBase, I2C_CMD_RESTART1 | I2C_CMD_READ | I2C_CMD_STOP);//send addr of low byte
	}
	
	iRet = csp_i2c_wait_recv(ptI2cBase, I2C_RECV_TIMEOUT);						//wait i2c recv
	byVal = csp_i2c_get_data(ptI2cBase);
	
	if(iRet >= 0)
		iRet = byVal;

	return iRet;
}
/** \brief i2c read mem(eeprom) n byte
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] byRdAddr: addr of read data
 *  \param[in] eAddrSize: mem addr long, 8bit/16bit
 *  \param[in] pbyData: pointer of read data buf
 *  \param[in] wReadNum: num of read
 *  \return data of read or error code
 */
int csp_i2c_read_mem_nbyte(csp_i2c_t *ptI2cBase,uint16_t hwRdAddr, i2c_addr_size_e eAddrSize, uint8_t *pbyData, uint32_t wReadNum)
{
	int iRet = 0;
	uint32_t i;
	
	if(eAddrSize == AADR_8BIT)
	{
		csp_i2c_set_data_cmd(ptI2cBase, (uint8_t)hwRdAddr | I2C_CMD_RESTART0);	//send addr of one byte
		//csp_i2c_set_data_cmd(ptI2cBase, I2C_CMD_READ | I2C_CMD_STOP);			//send read and stop cmd 
	}
	else
	{
		csp_i2c_set_data_cmd(ptI2cBase, (uint8_t)(hwRdAddr >> 8));				//send addr of high byte
		csp_i2c_set_data_cmd(ptI2cBase, (uint8_t)hwRdAddr);						//send addr of low byte
		csp_i2c_set_data_cmd(ptI2cBase, I2C_CMD_RESTART0 | I2C_CMD_READ);		//send addr of low byte
	}
	
	for(i = 0; i < wReadNum; i++)
	{
		if(i == (wReadNum - 1))
			csp_i2c_set_data_cmd(ptI2cBase, I2C_CMD_STOP | I2C_CMD_READ);	//send read and stop cmd 
		else
			csp_i2c_set_data_cmd(ptI2cBase, I2C_CMD_READ);					//send read cmd 
		
		iRet = csp_i2c_wait_recv(ptI2cBase, I2C_RECV_TIMEOUT);				//wait i2c recv
		*(pbyData + i) = csp_i2c_get_data(ptI2cBase);						//read data
		
		if(iRet < 0)
			break;
	}
	
	if(iRet >= 0)
		iRet = i;
		
	return iRet;
}
/** \brief i2c slave receive
 * 
 *  \param[in] ptI2cBase: pointer of I2C reg structure
 *  \param[in] byRdAddr: addr of read data
 *  \param[in] pbyData: pointer of read data buf
 *  \param[in] wWriteNum: num of read
 *  \return error
 */
void csp_i2c_slave_recv(csp_i2c_t *ptI2cBase)
{
	
}