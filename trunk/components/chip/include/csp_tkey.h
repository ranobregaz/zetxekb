/*
  ******************************************************************************
  * @file    apt32f102_tkey.h
  * @author  APT AE Team
  * @version V1.01
  * @date    2019/04/05
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

#ifndef _CSP_TKEY_H
#define _CSP_TKEY_H

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
}TKEY_HMEN_TypeDef;
/**
  * @brief  tkey mode register
  */
typedef enum
{
    TK_SINGLE = 0<<1,
	TK_SEQ = 1<<1,
}TKEY_MODE_TypeDef;
/**
  * @brief  tkey ckspr register
  */
typedef enum
{
    TK_CKSPR_DIS = 0<<9,
	TK_CKSPR_EN = 1<<9,
}TKEY_CKSPR_TypeDef;
/**
  * @brief  tkey CKRND register
  */
typedef enum
{
    TK_CKRND_DIS = 0<<10,
	TK_CKRND_EN = 1<<10,
}TKEY_CKRND_TypeDef;
/**
  * @brief  tkey CKFEQ register
  */
typedef enum
{
    TK_CKFEQ_LOW = 0<<11,
	TK_CKREQ_HIGH = 1<<11,
}TKEY_CKFEQ_TypeDef;

/**
  * @brief  tkey RSSEL register
  */
typedef enum
{
    TK_RSSEL_OVW = 0<<12,
	TK_RSSEL_OverTHR = 1<<12,
}TKEY_RSSEL_TypeDef;
/**
  * @brief  tkey IDLEP register
  */
typedef enum
{
    TK_IDLEP_DIS = 0<<14,
	TK_IDLEP_EN = 1<<14,
}TKEY_IDLEP_TypeDef;

/**
  * @brief  tkey DSR register
  */
typedef enum
{
    TK_DSR_Z = 0<<16,
	TK_DSR_LOW = 1<<16,
	TK_DSR_HIGH = 2<<16,
}TKEY_DSR_TypeDef;

/**
  * @brief  tkey TSCANSTB register
  */
typedef enum
{
    TK_STB_1 = 0<<20,
	TK_STB_2 = 1<<20,
	TK_STB_3 = 2<<20,
	TK_STB_4 = 3<<20,
}TKEY_TSSTB_TypeDef;

/**
  * @brief  tkey OTHRCN register
  */
typedef enum
{
    TK_DCKDIV_0 = 0<<12,
	TK_DCKDIV_2 = 1<<12,
	TK_DCKDIV_4 = 2<<12,
	TK_DCKDIV_8 = 3<<12,
}TKEY_DCKDIV_TypeDef;
#define     TK_PSEL_FVR  0
#define 	TK_PSEL_AVDD 1
#define 	TK_FVR_2048V 0
#define		TK_FVR_4096V 1
#define 	TK_EC_1V 	 0
#define		TK_EC_2V 	 1
#define		TK_EC_3V 	 2
#define		TK_EC_3_6V   3


//****************************************************************
#define TK_CLK_EN 	(TKEY->CLKEN|=0X01)
#define TK_CLK_DIS 	(TKEY->CLKEN&=0XFFE)
#define TK_SCANTIME_DIS (0<<12)
#define TK_SCANTIME_1ms (1<<12)
#define TK_SCANTIME_1_5ms (2<<12)
#define TK_SCANTIME_2ms (3<<12)
#define TK_SCANTIME_3ms (4<<12)
#define TK_SCANTIME_5ms (5<<12)
#define TK_SCANTIME_10ms (6<<12)
#define TK_SCANTIME_100ms (7<<12)
  
#define TKEY_TCHEN(val) (val)     /**< TKEY CH Enable         */
#define TKEY_ICON(val)  (((val) & 0x0Ful) << 8)  
#define TKEY_START      (0x01ul << 0) 

#define TKEY_SINDNE      (0x01ul << 0)
#define TKEY_DNE      (0x01ul << 1)
#define TKEY_THR      (0x01ul << 2)
#define TKEY_FLW      (0x01ul << 3)
#define TKEY_OVW      (0x01ul << 4)
#define TKEY_TIME      (0x01ul << 5)



#endif		