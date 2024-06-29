/***********************************************************************//** 
 * \file  FLASH_demo.c
 * \brief  Flash读写示例代码。建议将用户数据写在dataflash区（addr >= 0x10000000）
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-11-24 <td>V0.0  <td>WNN   <td>initial
 * </table>
 * *********************************************************************
*/


/* include ----------------------------------------------------------------*/
#include "eflash.h"
#include "soc.h"
#include "iostring.h"
#include "string.h"
/* externs function--------------------------------------------------------*/
/* private function--------------------------------------------------------*/

/* externs variablesr------------------------------------------------------*/
/* Private variablesr------------------------------------------------------*/
/* device instance---------------------------------------------------------*/
csi_eflash_t tEflash;

uint32_t wData[] = {0x1, 0x2, 0x33333333, 0x44444444, 0x55555555, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf,0x10, 0x11, 0x12, 0x13, 0x14};




#define SAVEDATA_LEN 20
#define SAVEDATA_ADD 0x10000000


/**
 \brief flash写操作示例。
  * \return      csi_error_t
*/
csi_error_t eflash_senario1(void)
{
	csi_error_t tRet = CSI_OK;
	uint32_t buff[SAVEDATA_LEN], i;
	
	csi_eflash_init(&tEflash, 0, NULL);
	
	csi_eflash_program(&tEflash, SAVEDATA_ADD-DFLASHBASE, wData, SAVEDATA_LEN);
	
	//有两种方法可以判断flash操作是否成功，二选一即可
	//1、 g_bFlashCheckPass  指示flash操作的整体结果，内部校验结果
	//2、 回读flash相应地址的数据，自己判断
	

	csi_eflash_read(&tEflash, SAVEDATA_ADD-DFLASHBASE, buff, SAVEDATA_LEN) ;
	for (i=0; i<SAVEDATA_LEN; i++) {
	if (buff[i] != wData[i])
		tRet =  CSI_ERROR;
	}
	if (g_bFlashCheckPass == 0) 
		tRet =  CSI_ERROR;
		
	return tRet;
}

/**
 \brief flash擦除操作示例。
  * \return      csi_error_t
*/
csi_error_t eflash_senario2(void)
{
	csi_error_t tRet = CSI_OK;
	
	csi_eflash_init(&tEflash, 0, NULL);
	
	tRet = csi_eflash_erase(&tEflash, 0x100000C4-DFLASHBASE, 30);
	//擦除0x10000000所在的page
	
	if (tRet == CSI_ERROR ||g_bFlashCheckPass == 0)							
		tRet =  CSI_ERROR;
	
	return tRet;
}

/**
 \brief flash写user option操作示例。
  * \return      csi_error_t
*/
csi_error_t eflash_senario3(void)
{
	csi_error_t tRet = CSI_OK;
	uint32_t buff[0];
	
	csi_eflash_init(&tEflash, 0, NULL);
	
	csi_eflash_wr_useroption(&tEflash, 0x55aa0005);		//写用户选项。"55aa"表示启动时禁止IWDT，"5"代表将PA0.2设为外部复位管脚。
	while (csi_eflash_get_status(&tEflash).busy);		//如果busy = 0，说明flash已经写完。不需要死等，可以在合适的时候查询即可。
	
	
	csi_eflash_read(&tEflash, USEROPTION_ADDR, buff, 1) ;	//flash读操作，地址USEROPTION_ADDR, 数据长度1， 读回到buff
	if (buff[0] != 0x55aa0005 || g_bFlashCheckPass == 0) 
		tRet =  CSI_ERROR;
	
	return tRet;
}

/**
 \brief flash更改默认SWD口操作示例。
  * \return      csi_error_t
*/
csi_error_t eflash_senario4(void)
{
	csi_eflash_init(&tEflash, 0, NULL);
	
	//!!!下面的代码将修改上电时SWD口,如果写入值和之前不同，复位后即失去和仿真器的连接!!!
	 /* 					SWD       	pin number
		* SWD_GRP0 = 2    	PA0.6(CLK)   	2
		* 				  	PA0.7(DIO)		3
		* SWD_GRP0 = 1	   	PA0.0(DIO)		19
		* 					PA0.1(CLK)		20
		* SWD_GRP0 = 0    	PA0.5(CLK)		10
		* 					PA0.12(DIO)		11
		*/
	csi_eflash_swd_remap(&tEflash,SWD_GRP2);
	
	return CSI_OK;
}



//csi_error_t eflash_senario(void)
//{
//	csi_error_t tRet = CSI_OK;
//	uint32_t buff[SAVEDATA_LEN], i, wWriteTime, wAddr, wLen, wWriteLen;
//	
//	csi_eflash_init(&tEflash, 0, NULL);
//	
//	/*csi_eflash_wr_useroption(&tEflash, 0x55aa0005);		//写用户选项。"55aa"表示启动时禁止IWDT，"5"代表将PA0.2设为外部复位管脚。
//	while (csi_eflash_get_status(&tEflash).busy);		//如果busy = 0，说明flash已经写完。不需要死等，可以在合适的时候查询即可。
//	
//	//有两种方法可以判断flash操作是否成功，二选一即可
//	//1、 g_bFlashCheckPass  指示flash操作的整体结果，内部校验结果
//	//2、 回读flash相应地址的数据，自己判断
//	
//	csi_eflash_read(&tEflash, USEROPTION_ADDR, buff, 1) ;	//flash读操作，地址USEROPTION_ADDR, 数据长度1， 读回到buff
//	if (buff[0] != 0x55aa0005 || g_bFlashCheckPass == 0) 
//		tRet =  CSI_ERROR;*/
//	
//	csi_eflash_wr_1word(&tEflash, 0x10000000, 0x22288111);	//flash写1个word操作
//	while (csi_eflash_get_status(&tEflash).busy);
//	csi_eflash_read(&tEflash, 0x10000000, buff, 1) ;
//	if (buff[0] != 0x22288111 || g_bFlashCheckPass == 0)
//		tRet =  CSI_ERROR;
//		
//		
///*	wAddr = SAVEDATA_ADD;								//将wData数组的前SAVEDATA_LEN个数据写入flash，起始地址=SAVEDATA_ADD
//	wLen = SAVEDATA_LEN;
//	
//	if (wLen%16 == 0)
//		wWriteTime = SAVEDATA_LEN/16;
//	else
//		wWriteTime = SAVEDATA_LEN/16 +1 ;
//	for (i=0; i<wWriteTime; i++)
//	{
//		
//		if (wLen <17)
//			wWriteLen = wLen;
//		else
//			wWriteLen = 16;
//		csi_eflash_program(&tEflash, wAddr, &wData[16*i], wWriteLen);		
//		while (csi_eflash_get_status(&tEflash).busy);
//		wAddr = wAddr + (i+1)*64;
//		wLen = wLen - (i+1)*16;
//	}*/
//	
//	
//	
//	
//	
//	csi_eflash_erase(&tEflash, 0x10000010, 33);
//	//擦除0x10000000所在的page
//	
//	if (g_bFlashCheckPass == 0)							//第一种判断方法
//		tRet =  CSI_ERROR;
//	
//	
//	
//
//	if (tRet == CSI_ERROR)
//		my_printf("flash test error\n");
//	else
//		my_printf("flash test pass \n");
//	while(1);
//	return tRet;
//}

void eflash_test(void)
{
	csi_error_t ret = CSI_OK;
	uint32_t wBuf[96];
	uint8_t byLen = 96;
	uint32_t wStAdd = 4;
	uint8_t byRptTime = 3;
	csi_eflash_init(&tEflash, 0, NULL);
	
	csi_eflash_erase(&tEflash, 0, 1);
	for (uint32_t i=0;i<byRptTime;i++) {
        
        ret = csi_eflash_program(&tEflash,wStAdd,(void *)wData,byLen);
        //TEST_CASE_ASSERT(ret == CSI_OK, "eflash program failed");
		if (ret)
			my_printf("feflash program failed\n");
        ret = csi_eflash_read(&tEflash,wStAdd,(void *)wBuf,96);
        if (ret)
			my_printf("eflash read failed");
        ret = memcmp((const void *)wData, (const void *)wBuf, byLen);
        if (ret)
			my_printf("flash test error\n");
    }
}


