/***********************************************************************//** 
 * \file  csp_tkey.h
 * \brief  TKEY description and static inline functions at register level 
 * \copyright Copyright (C) 2015-2020 @ APTCHIP
 * <table>
 * <tr><th> Date  <th>Version  <th>Author  <th>Description
 * <tr><td> 2020-12-09 <td>V0.0  <td>XB   <td>initial
 * </table>
 * *********************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CSP_TKEY_H
#define _CSP_TKEY_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <sys_clk.h>


#define TKEY_RESET_VALUE  	(0x00000000)


/**
@brief TKEY  bits Structure
*/
typedef volatile struct
{
   __IOM uint32_t   TCH_CCR;               /* Control Register */
   __IOM uint32_t   TCH_CON0;               /* Control Register */
   __IOM uint32_t   TCH_CON1;               /* Control Register */
   __IOM uint32_t   TCH_SCCR;               /* Hardmacro control    */
   __IOM uint32_t   TCH_SENPRD;            /* Sensing target value */
   __IOM uint32_t   TCH_VALBUF;            /* Reference value capture value*/
   __IOM uint32_t   TCH_SENCNT;            /* Sensing counter value*/
   __IOM uint32_t   TCH_TCHCNT;            /* Reference counter value*/
   __IOM uint32_t   TCH_THR;                /* Match Status           */
   __IOM uint32_t   Reserved0;				
   __IOM uint32_t   TCH_RISR;               /* Interrupt Enable       */
   __IOM uint32_t   TCH_IER;               /* Interrupt Clear        */
   __IOM uint32_t   TCH_ICR;            /* Sensing target value */
   __IOM uint32_t   TCH_RWSR;            /* Reference value capture value*/
   __IOM uint32_t   TCH_OVW_THR;            /* Sensing counter value*/
   __IOM uint32_t   TCH_OVF;            /* Reference counter value*/
   __IOM uint32_t   TCH_OVT;                /* Match Status           */
   __IOM uint32_t   TCH_SYNCR;               /* Interrupt Enable       */
   __IOM uint32_t   TCH_EVTRG;               /* Interrupt Clear        */
   __IOM uint32_t   TCH_EVPS;            /* Sensing target value */
   __IOM uint32_t   TCH_EVSWF;            /* Reference value capture value*/
   __IOM uint32_t   Reserved1[1003];
   __IOM uint32_t   TCH_CHVAL[18];          /* Reference value capture value */
   __IOM uint32_t   TCH_SEQCON[18];         /* SEQ Hardmacro control  */
}csp_tkey_t; 


/**
  * @brief  tkey mode register
  */
typedef enum
{
    TK_HM_DIS = 0<<0,
	TK_HM_EN = 1<<0,
}tkey_hmen_e;
/**
  * @brief  tkey mode register
  */
typedef enum
{
    TK_SINGLE = 0<<1,
	TK_SEQ = 1<<1,
}tkey_mode_e;

/**
  * @brief  tkey ec level register
  */
typedef enum
{
    TK_EC_1V = 0<<7,
	TK_EC_2V = 1<<7,
	TK_EC_3V = 2<<7,
	TK_EC_3_6V = 3<<7,
}tkey_eclvl_e;

/**
  * @brief  tkey ec level register
  */
typedef enum
{
    TK_FVR_2048V = 0,
	TK_FVR_4096V = 1,
}tkey_fvrlvl_e;

/**
  * @brief  tkey ckspr register
  */
typedef enum
{
    TK_CKSPR_DIS = 0<<9,
	TK_CKSPR_EN = 1<<9,
}tkey_ckspr_e;
/**
  * @brief  tkey CKRND register
  */
typedef enum
{
    TK_CKRND_DIS = 0<<10,
	TK_CKRND_EN = 1<<10,
}tkey_ckrnd_e;
/**
  * @brief  tkey CKFEQ register
  */
typedef enum
{
    TK_CKFEQ_LOW = 0<<11,
	TK_CKFEQ_HIGH = 1<<11,
}tkey_ckfeq_e;

/**
  * @brief  tkey RSSEL register
  */
typedef enum
{
    TK_RSSEL_OVW = 0<<12,
	TK_RSSEL_OverTHR = 1<<12,
}tkey_rssel_e;

/**
  * @brief  tkey PSEL register
  */
typedef enum
{
    TK_PSEL_FVR = 0<<13,
	TK_PSEL_AVDD = 1<<13,
}tkey_psel_e;

/**
  * @brief  tkey IDLEP register
  */
typedef enum
{
    TK_IDLEP_DIS = 0<<14,
	TK_IDLEP_EN = 1<<14,
}tkey_idlep_e;

/**
  * @brief  tkey DSR register
  */
typedef enum
{
    TK_DSR_Z = 0<<16,
	TK_DSR_LOW = 1<<16,
	TK_DSR_HIGH = 2<<16,
}tkey_dsr_e;

/**
  * @brief  tkey TSCANSTB register
  */
typedef enum
{
    TK_STB_1 = 0<<20,
	TK_STB_2 = 1<<20,
	TK_STB_3 = 2<<20,
	TK_STB_4 = 3<<20,
}tkey_tsstb_e;

/**
  * @brief  tkey OTHRCNT register
  */
typedef enum
{
    TK_OTHRCNT_1 = 0<<24,
	TK_OTHRCNT_2 = 1<<24,
	TK_OTHRCNT_3 = 2<<24,
	TK_OTHRCNT_4 = 3<<24,
	TK_OTHRCNT_5 = 4<<24,
	TK_OTHRCNT_6 = 5<<24,
	TK_OTHRCNT_7 = 6<<24,
	TK_OTHRCNT_8 = 7<<24,
}tkey_othrcnt_e;

/**
  * @brief  tkey OTHRCN register
  */
typedef enum
{
    TK_DCKDIV_0 = 0<<12,
	TK_DCKDIV_2 = 1<<12,
	TK_DCKDIV_4 = 2<<12,
	TK_DCKDIV_8 = 3<<12,
}tkey_dckdiv_e;

//****************************************************************
#define TK_CLK_EN 			(TKEY->CLKEN|=0X01)
#define TK_CLK_DIS 			(TKEY->CLKEN&=0XFFE)
#define TK_SCANTIME_DIS 	(0<<12)
#define TK_SCANTIME_1ms 	(1<<12)
#define TK_SCANTIME_1_5ms 	(2<<12)
#define TK_SCANTIME_2ms 	(3<<12)
#define TK_SCANTIME_3ms 	(4<<12)
#define TK_SCANTIME_5ms 	(5<<12)
#define TK_SCANTIME_10ms 	(6<<12)
#define TK_SCANTIME_100ms 	(7<<12)
  
#define TKEY_TCHEN(val) 	(val)     /**< TKEY CH Enable         */
#define TKEY_ICON(val)  	(((val) & 0x0Ful) << 8)  
#define TKEY_START      	(0x01ul << 0) 

#define TKEY_SINDNE      	(0x01ul << 0)
#define TKEY_DNE      		(0x01ul << 1)
#define TKEY_THR     		(0x01ul << 2)
#define TKEY_FLW     		(0x01ul << 3)
#define TKEY_OVW      		(0x01ul << 4)
#define TKEY_TIME      		(0x01ul << 5)


/****************************************************
//TK parameter define
*****************************************************/
uint32_t 	wTkeyIOEnable;				 //TK IO使能    第几位对应TOUCH几如： bit0 -->  touch 0      bit16 --> touch16

uint16_t 	hwTkeyDiv[17];	//TK扫描分频1~3  
uint16_t 	hwTkeyIcon[17];	//TK扫描Icon0~7	//icon减小采样值增大
uint16_t 	hwTkeySenprd[17];	//TK扫描周期	//senprd增加采样值增大
uint16_t 	hwTkeyTriggerLevel[17];		//门槛值

uint8_t  	byPressDebounceData;		//按下去抖1~5
uint8_t  	byReleaseDebounceData;	//释放去抖1~5
uint8_t  	byMultiTimesFilter;		//倍数滤波>=4
uint8_t  	byTkeyMode;				// 1=multi key
uint8_t  	byValidKeyNum;			//最大按下有效个数

uint8_t	 	byBaseSpeed;									//基线更新速度
uint8_t	 	byTekyLongpressTime;					//longpress rebuild time = _TK_longpress_time1*1s  0=disable
uint32_t 	wTkeyBaseCnt;							//基线更新时间 =  (定时器时间*wTkeyBaseCnt ) 

/****************************************************
//TK function define
*****************************************************/
uint32_t 	wTkeyPselMode;		//tk power sel:TK_PSEL_FVR/TK_PSEL_AVDD
uint32_t 	wTkeyFvrLevel;		//FVR level:TK_FVR_2048V/TK_FVR_4096V
uint32_t 	wTkeyECLevel;		//C0 voltage sel:TK_EC_1V/TK_EC_2V/TK_EC_3V/TK_EC_3_6V
uint32_t 	wTkeyCon0;	
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

/******************************************************************************
************************** Exported functions ************************
******************************************************************************/
extern uint16_t hwSamplData0[17];
extern uint16_t hwBaselineData0[17];
extern int16_t nOffsetData0[17];
extern uint32_t dwKeyMap;

extern void csp_tkey_clk_config(csp_tkey_t *ptTkeyBase,bool bNewState,uint8_t byPckDivx,uint8_t byTckDivx);
extern void csp_tkey_scan_config(csp_tkey_t *ptTkeyBase,uint8_t iConx,tkey_dckdiv_e eDckDivx,uint8_t byScanTimex);
extern void csp_tkey_configinterrupt_cmd(csp_tkey_t *ptTkeyBase,bool bNewState,uint32_t wTkeyIMSCRX);
extern void csp_tkey_con0_config(csp_tkey_t *ptTkeyBase,uint32_t wTchcn0,uint8_t bySwqlenx);
extern void csp_tkey_baseline_prog(csp_tkey_t *ptTkeyBase);
extern void csp_tkey_init(csp_tkey_t *ptTkeyBase);
extern void csp_tkey_seqxcon_set(csp_tkey_t *ptTkeyBase,uint8_t byNumber);
extern void csp_tkey_prgm(csp_tkey_t *ptTkeyBase);
/******************************************************************************
********************* TKEY inline Functions Declaration **********************
******************************************************************************/
static inline void csp_tkey_set_thr(csp_tkey_t *ptTkeyBase, uint32_t wThr)
{
	ptTkeyBase->TCH_THR = wThr&0x0000ffff;
}
static inline void csp_tkey_start(csp_tkey_t *ptTkeyBase)
{
	ptTkeyBase->TCH_CON1 = 0X01;
}
static inline uint32_t csp_tkey_get_risr(csp_tkey_t *ptTkeyBase)
{
	return (uint32_t)ptTkeyBase->TCH_RISR;

}
static inline void csp_tkey_clr_risr(csp_tkey_t *ptTkeyBase, uint32_t wThr)
{
	ptTkeyBase->TCH_ICR |= wThr ;

}

#endif
