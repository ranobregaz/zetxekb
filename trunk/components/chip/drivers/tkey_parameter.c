/*
  ******************************************************************************
  * @file    apt32f102_tkey_parameter.c
  * @author  APT AE Team
  * @version V1.05
  * @date    2020/11/24
  ******************************************************************************
  *THIS SOFTWARE WHICH IS FOR ILLUSTRATIVE PURPOSES ONLY WHICH PROVIDES 
  *CUSTOMER WITH CODING INFORMATION REGARDING THEIR PRODUCTS.
  *APT CHIP SHALL NOT BE HELD RESPONSIBILITY ADN LIABILITY FOR ANY DIRECT, 
  *INDIRECT DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF 
  *SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION 
  *CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.AND APT CHIP RESERVES 
  *THE RIGHT TO MAKE CHANGES IN THE SOFTWARE WITHOUT NOTIFICATION
  ******************************************************************************
  */
#include "csp_tkey.h"
#include <drv/tkey.h>

void csi_tkey_parameter_init(void)
{

	wTkeyIoEnable = 0xf<<10;		
	hwTkeySenprd[0]  = 80;					//TCH0 scan period = TCH0 sens
	hwTkeySenprd[1]  = 80;					//TCH1 scan period = TCH1 sens
	hwTkeySenprd[2]  = 80;					//TCH2 scan period = TCH2 sens
	hwTkeySenprd[3]  = 80;					//TCH3 scan period = TCH3 sens
	hwTkeySenprd[4]  = 80;					//TCH4 scan period = TCH4 sens
	hwTkeySenprd[5]  = 80;					//TCH5 scan period = TCH5 sens
	hwTkeySenprd[6]  = 80;					//TCH6 scan period = TCH6 sens
	hwTkeySenprd[7]  = 80;					//TCH7 scan period = TCH7 sens
	hwTkeySenprd[8]  = 80;					//TCH8 scan period = TCH8 sens
	hwTkeySenprd[9]  = 80;					//TCH9 scan period = TCH9 sens
	hwTkeySenprd[10] = 80;					//TCH10 scan period = TCH10 sens
	hwTkeySenprd[11] = 80;					//TCH11 scan period = TCH11 sens
	hwTkeySenprd[12] = 80;					//TCH12 scan period = TCH12 sens
	hwTkeySenprd[13] = 80;					//TCH13 scan period = TCH13 sens
	hwTkeySenprd[14] = 80;					//TCH14 scan period = TCH14 sens
	hwTkeySenprd[15] = 80;					//TCH15 scan period = TCH15 sens
	hwTkeySenprd[16] = 80;					//TCH16 scan period = TCH16 sens
	
	hwTkeyTriggerLevel[0]  = 60;			//TCH0 TK_Trigger level
	hwTkeyTriggerLevel[1]  = 60;			//TCH1 TK_Trigger level
	hwTkeyTriggerLevel[2]  = 60;			//TCH2 TK_Trigger level
	hwTkeyTriggerLevel[3]  = 60;			//TCH3 TK_Trigger level
	hwTkeyTriggerLevel[4]  = 60;			//TCH4 TK_Trigger level
	hwTkeyTriggerLevel[5]  = 60;			//TCH5 TK_Trigger level
	hwTkeyTriggerLevel[6]  = 60;			//TCH6 TK_Trigger level
	hwTkeyTriggerLevel[7]  = 60;			//TCH7 TK_Trigger level
	hwTkeyTriggerLevel[8]  = 60;			//TCH8 TK_Trigger level
	hwTkeyTriggerLevel[9]  = 60;			//TCH9 TK_Trigger level
	hwTkeyTriggerLevel[10] = 60;			//TCH10 TK_Trigger level
	hwTkeyTriggerLevel[11] = 60;			//TCH11 TK_Trigger level
	hwTkeyTriggerLevel[12] = 60;			//TCH12 TK_Trigger level
	hwTkeyTriggerLevel[13] = 60;			//TCH13 TK_Trigger level
	hwTkeyTriggerLevel[14] = 60;			//TCH14 TK_Trigger level
	hwTkeyTriggerLevel[15] = 60;			//TCH15 TK_Trigger level
	hwTkeyTriggerLevel[16] = 60;			//TCH16 TK_Trigger level
	
	byPressDebounce = 5;			//Press debounce 1~10
	byReleaseDebounce = 5;			//Release debounce 1~10
	byKeyMode = 1;						//Key mode 0=single key 1=multi key
	byMultiTimesFilter = 0;				//MultiTimes Filter,>4 ENABLE <4 DISABLE
	byValidKeyNum = 4;					//Valid Key number when touched
	byBaseSpeed = 10;					//baseline update speed
	wTkeyLongPressTime = 16;				//longpress rebuild time = _TK_longpress_time1*1s  0=disable
/****************************************************
//TK low power function define
*****************************************************/
	byTkeyLowpowerMode = ENABLE;//DISABLE;	//touch key can goto sleep when TK lowpower mode enable
	byTkeypowerLevel = 2;					//0=20ms 1=50ms 2=100ms 3=150ms 4=200ms,Scan interval when sleep
	byTkeyWakeupLevel = 60;					//touch key Trigger level in sleep
/****************************************************
//TK special parameter define
*****************************************************/
	hwTkeyPselMode = TK_PSEL_AVDD;		//tk power sel:TK_PSEL_FVR/TK_PSEL_AVDD   when select TK_PSEL_FVR PA0.2(TCH3) need a 104 cap
	hwTkeyFVRLevel = TK_FVR_4096V;		//FVR level:TK_FVR_2048V/TK_FVR_4096V
	hwTkeyECLevel = TK_EC_1V;			//C0 voltage sel:TK_EC_1V/TK_EC_2V/TK_EC_3V/TK_EC_3_6V
	hwTkeyIcon[0]  = 4;					//TCH0 TK Scan icon
	hwTkeyIcon[1]  = 4;					//TCH1 TK Scan icon
	hwTkeyIcon[2]  = 4;					//TCH2 TK Scan icon
	hwTkeyIcon[3]  = 4;					//TCH3 TK Scan icon
	hwTkeyIcon[4]  = 4;					//TCH4 TK Scan icon
	hwTkeyIcon[5]  = 4;					//TCH5 TK Scan icon
	hwTkeyIcon[6]  = 4;					//TCH6 TK Scan icon
	hwTkeyIcon[7]  = 4;					//TCH7 TK Scan icon
	hwTkeyIcon[8]  = 4;					//TCH8 TK Scan icon
	hwTkeyIcon[9]  = 4;					//TCH9 TK Scan icon
	hwTkeyIcon[10] = 4;					//TCH10 TK Scan icon
	hwTkeyIcon[11] = 4;					//TCH11 TK Scan icon
	hwTkeyIcon[12] = 4;					//TCH12 TK Scan icon
	hwTkeyIcon[13] = 4;					//TCH13 TK Scan icon
	hwTkeyIcon[14] = 4;					//TCH14 TK Scan icon
	hwTkeyIcon[15] = 4;					//TCH15 TK Scan icon
	hwTkeyIcon[16] = 4;					//TCH16 TK Scan icon
/*************************************************************
* 扫描超时时间设置  0禁止设置  1->1ms   2->1.5ms  3->2ms   4->3ms  5->5ms   6->10ms  7->100ms
**********************************************************/
	byTkeyScanTime[0]  = 0;				//TCH0  扫描时间设置
	byTkeyScanTime[1]  = 0;				//TCH1  扫描时间设置
	byTkeyScanTime[2]  = 0;				//TCH2  扫描时间设置
	byTkeyScanTime[3]  = 0;				//TCH3  扫描时间设置
	byTkeyScanTime[4]  = 0;				//TCH4  扫描时间设置
	byTkeyScanTime[5]  = 0;				//TCH5  扫描时间设置
	byTkeyScanTime[6]  = 0;				//TCH6  扫描时间设置
	byTkeyScanTime[7]  = 0;				//TCH7  扫描时间设置
	byTkeyScanTime[8]  = 0;				//TCH8  扫描时间设置
	byTkeyScanTime[9]  = 0;				//TCH9  扫描时间设置
	byTkeyScanTime[10] = 0;				//TCH10 扫描时间设置
	byTkeyScanTime[11] = 0;				//TCH11 扫描时间设置
	byTkeyScanTime[12] = 0;				//TCH12 扫描时间设置
	byTkeyScanTime[13] = 0;				//TCH13 扫描时间设置
	byTkeyScanTime[14] = 0;				//TCH14 扫描时间设置
	byTkeyScanTime[15] = 0;				//TCH15 扫描时间设置
	byTkeyScanTime[16] = 0;				//TCH16 扫描时间设置
	
	
}