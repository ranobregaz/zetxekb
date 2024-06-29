/***********************************************************************//** 
 * \file  CRC_demo.c
 * \brief  CRC 示例代码
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-24 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/

/* include ----------------------------------------------------------------*/
#include "crc.h"
/* externs function--------------------------------------------------------*/
extern uint32_t csi_crc32_be_try(uint32_t crc, uint8_t* data, uint32_t size);
extern uint32_t cal_crc32_be(uint32_t *pwData, uint32_t len);
extern uint16_t crc_16(uint8_t *data, uint16_t len);
/* private function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
uint8_t byTransData[] = {0x4, 0x3, 0x2, 0x1, 0x8, 0x44, 0x33, 0x22, 0x11, 0x44, 0x33, 0x22, 0x11, 0x44, 0x33, 0x22, 0x11} ; 
/* device instance------------------------------------------------------*/



/**
  \brief  CRC示例一：
   *  CRC结果可以参考 http://www.ip33.com/crc.html
   * 如果两次处理的数据到达有时间间隔，可以将第一次处理的结果作为第二次的初始值，继续CRC转换。
  \return      csi_error_t
*/
csi_error_t crc_senario(void)
{
	volatile uint32_t temp1;
	csi_error_t tRet = CSI_OK;
	csi_crc_init();
	
	temp1 = csi_crc32_be(0xffffffff, byTransData, 3);	//初始值0xffffffff, 数据长度3
	if (temp1 != 0xcb6faba1)
		tRet = CSI_ERROR;
	
	temp1 = csi_crc16_ccitt(0x0, byTransData, 16);
	if (temp1 != 0x28f5)
		tRet = CSI_ERROR;

	temp1 = csi_crc16(0x0, byTransData, 5);
	if (temp1 != 0x1250)
		tRet = CSI_ERROR;
		
	temp1 = csi_crc16_itu(0x0,  byTransData, 3);
	if (temp1 != 0xa9d1)
		tRet = CSI_ERROR;
	
	while(1);
	return tRet;
}

