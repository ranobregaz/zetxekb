/*
  ******************************************************************************
  * @file    csp_tkey_parameter.h
  * @author  APT AE Team
  * @version V1.01
  * @date    2020/11/11
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
#ifndef  _DRV_TKEY_PARAMETER_H
#define _DRV_TKEY_PARAMETER_H
  
#include "csp_tkey.h"
#include <stdint.h> 
/****************************************************
//TK parameter define
*****************************************************/



uint32_t 	wTkeyIOEnable  = 0x1ffff;				 //TK IO使能    第几位对应TOUCH几如： bit0 -->  touch 0      bit16 --> touch16

uint16_t 	hwTkeyDiv[17]=  {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};	//TK扫描分频1~3  
uint16_t 	hwTkeyIcon[17]={4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4};	//TK扫描Icon0~7	//icon减小采样值增大
uint16_t 	hwTkeySenprd[17]={70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70,70};	//TK扫描周期	//senprd增加采样值增大
uint16_t 	hwTkeyTriggerLevel[17]={100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100};		//门槛值

uint8_t  	byPressDebounceData=2;		//按下去抖1~5
uint8_t  	byReleaseDebounceData=2;	//释放去抖1~5
uint8_t  	byMultiTimesFilter=20;		//倍数滤波>=4
uint8_t  	byTkeyMode=1;				// 1=multi key
uint8_t  	byValidKeyNum=4;			//最大按下有效个数

uint8_t	 	byBaseSpeed=30;									//基线更新速度
uint8_t	 	byTekyLongpressTime=16;					//longpress rebuild time = _TK_longpress_time1*1s  0=disable
uint32_t 	wTkeyBaseCnt=19999;							//基线更新时间 =  (定时器时间*wTkeyBaseCnt ) 

/****************************************************
//TK function define
*****************************************************/
uint32_t 	wTkeyPselMode = TK_PSEL_AVDD;		//tk power sel:TK_PSEL_FVR/TK_PSEL_AVDD
uint32_t 	wTkeyFvrLevel = TK_FVR_4096V;		//FVR level:TK_FVR_2048V/TK_FVR_4096V
uint32_t 	wTkeyECLevel = TK_EC_3_6V;		//C0 voltage sel:TK_EC_1V/TK_EC_2V/TK_EC_3V/TK_EC_3_6V
uint32_t 	wTkeyCon0 = 0;	
uint16_t 	hwSamplData0[17];
uint16_t 	hwBaselineData0[17];
uint16_t 	hwSamplData1[17];
uint16_t 	hwBaselineData1[17];
uint16_t 	hwSamplData2[17];
uint16_t 	hwBaselineData2[17];
int16_t 		nOffsetData0[17];
int16_t 		nOffsetData1[17];
int16_t 		nOffsetData2[17];
/****************************************************
//TK parameter description
*****************************************************/
//采样值上限=（senprd*16+15）*div，超过上限后扫描可能不能完成
//div若调整过小，可能造成采样错误，在合适的div分频下采样值差异不大
//icon减小采样值增大
//senprd增加采样值增大
//trim频率较高后，需要配置div

#endif