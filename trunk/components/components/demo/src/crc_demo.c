/***********************************************************************//** 
 * \file  crc_demo.c
 * \brief  CRC_DEMO description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2021-5-11 <td>V0.0 <td>ZJY     <td>initial
 * </table>
 * *********************************************************************
*/
#include <string.h>
#include <drv/bt.h>
#include <drv/pin.h>

#include "demo.h"
#include "crc.h"


/* private function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
uint8_t byTransData[] = {0x4, 0x3, 0x2, 0x1, 0x8, 0x44, 0x33, 0x22, 0x11, 0x44, 0x33, 0x22, 0x11, 0x44, 0x33, 0x22, 0x11} ; 
/* device instance------------------------------------------------------*/



/**
  \brief  CRC Example 1:
   *CRC result can refer to http://www.ip33.com/crc.html
   *If there is a time gap between the arrival of the data from the two processes, 
   *the CRC conversion can be continued using the result of the first process as the initial value of the second process.
  \return      csi_error_t
*/
int crc_demo(void)
{
	volatile uint32_t temp1 = 0;
	int iRet = 0;
	
	csi_crc_init();                              //CRC module initialization
	
	temp1 = csi_crc32_be(0xffffffff, byTransData, 3);	//initialization value 0xffffffff, data length 3
	if (temp1 != 0xcb6faba1)                            //Determine the result of conversion
		iRet = -1;
	
	temp1 = csi_crc16_ccitt(0x0, byTransData, 16);      //initialization value 0x0, data length 16
	if (temp1 != 0x1C40)                                //Determine the result of conversion
		iRet = -1;

	temp1 = csi_crc16(0x0, byTransData, 5);             //initialization value 0x0, data length 5
	if (temp1 != 0x1250)                                //Determine the result of conversion
		iRet = -1;
		
	temp1 = csi_crc16_itu(0x0,  byTransData, 3);        //initialization value 0x0, data length 3
	if (temp1 != 0xa9d1)                                //Determine the result of conversion
		iRet = -1;
	
	while(1);
	return iRet;
}