/*
  ******************************************************************************
  * @file    apt32f102_tkey.c
  * @author  APT AE Team
  * @version V1.05
  * @date    2021/01/14
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
  
  
  /* Includes ------------------------------------------------------------------*/
#include "csp_tkey.h"
#include "csp_gpio.h"
#include "csp_uart.h"
#include "csp_syscon.h"
#include "csp_coret.h"
#include "csp_lpt.h"
#include "csp_etb.h"
#include "string.h"
#include "csp.h"
#include <drv/tkey.h>
#include "irq.h"
#include "porting.h"

/****************************************************
//TK scan freq define
*****************************************************/
uint32_t wTkeyScanFreq0=0x80007;		//513*4 k
uint32_t wTkeyScanFreq1=0x8000a;		//647*4 k
uint32_t wTkeyScanFreq2=0x8000d;		//785*4 k
//采样值上限=（senprd*16+15）*div，超过上限后扫描不能完成
//icon减小采样值增大
//senprd增加采样值增大
//trim频率较高后，需要配置div
uint16_t hwTkeyClkDiv[17]={2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};
/****************************************************
//TK variable define
*****************************************************/
volatile uint32_t wTkeyStateFlag = 1;
volatile uint8_t byScanStep,byScanStepTemp=3;
volatile uint8_t byTkeySeque[17];
volatile uint8_t byTkeyNum;
volatile uint16_t hwSamplingData0[17];
volatile uint16_t hwBaselineData0[17];
volatile uint16_t hwSamplingData1[17];
volatile uint16_t hwBaselineData1[17];
volatile uint16_t hwSamplingData2[17];
volatile uint16_t hwBaselineData2[17];
volatile int16_t nOffsetData0[17];
volatile int16_t nOffsetData1[17];
volatile int16_t nOffsetData2[17];
volatile uint16_t hwOffseData0Abs[17];
volatile uint16_t hwOffseData1Abs[17];
volatile uint16_t hwOffseData2Abs[17];
volatile uint32_t wKeyMap;
volatile uint32_t wKeyMap0;
volatile uint32_t wKeyMap1;
volatile uint32_t wKeyMap2;
volatile uint8_t byPressDebounce0[17];
volatile uint8_t byReleaseDebounce0[17];
volatile uint8_t byPressDebounce1[17];
volatile uint8_t byReleaseDebounce1[17];
volatile uint8_t byPressDebounce2[17];
volatile uint8_t byReleaseDebounce2[17];
volatile uint8_t byTkeyTrackCnt;
volatile uint8_t byTkeyNegtiveBuid0[17];
volatile uint8_t byTkeyNegtiveBuid1[17];
volatile uint8_t byTkeyNegtiveBuid2[17];
volatile uint8_t byTkeyPostiveBuid0[17];
volatile uint8_t byTkeyPostiveBuid1[17];
volatile uint8_t byTkeyPostiveBuid2[17];
volatile uint32_t wTimeCnt;
volatile uint32_t wKeyMapTemp;
volatile uint8_t byBaseUpdata;
volatile uint8_t	byBaseUpdata0;
volatile uint8_t	byBaseUpdata1;
volatile uint8_t	byBaseUpdata2;
volatile uint8_t byTkeyBaseOverFlag;
volatile uint8_t bySampSetoverFlag=1;
volatile uint16_t hwLptScanPendCnt;

volatile uint8_t byTkeyChNumber=0;


extern void	csi_tkey_scan_start(void);
extern void	csi_tkey_keymap_prog(void);
extern void	csi_tkey_baseline_tracking(void);
extern void	csi_tkey_result_prog(void);




/****************************************************
//TK Deinit
*****************************************************/
void csi_tkey_reinit(void)
{
    TKEY->TCH_CON1 = 0x00000000;
	TKEY->TCH_CCR = 0x04010000;
	TKEY->TCH_CON0 = 0x00042000;
	TKEY->TCH_VALBUF = 0x00000000;
	TKEY->TCH_SENCNT = 0x00000000;
	TKEY->TCH_TCHCNT = 0x00000000;
	TKEY->TCH_THR = 0x00000000;
	TKEY->TCH_RISR = 0x00000000;
	TKEY->TCH_IER = 0x00000000;
	TKEY->TCH_ICR = 0x00000000;
	TKEY->TCH_RWSR= 0x00000000;
	TKEY->TCH_OVW_THR= 0x00000000;
	TKEY->TCH_OVF= 0x00000000;
	TKEY->TCH_OVT= 0x00000000;
	TKEY->TCH_SYNCR= 0x00000000;
	TKEY->TCH_EVTRG= 0x00000000;
	TKEY->TCH_EVPS= 0x00000000;
	TKEY->TCH_EVSWF= 0x00000000;
}
/****************************************************
//TK clock configure
*****************************************************/
void csi_tkey_clk_config(uint8_t NewState,uint8_t PCKDIVX,uint8_t TCKDIVX)
{
	if (NewState != DISABLE)
	{
		TKEY->TCH_CCR  |= 0x01<<0;						
	}
	else
	{
		TKEY->TCH_CCR  &= 0xfffffffe;
	}
	TKEY->TCH_CCR |= (PCKDIVX<<8)|(TCKDIVX<<2);
}
/****************************************************
//TK control register configure
*****************************************************/
void sci_tkey_con0_config(TKEY_HMEN_TypeDef HMENX,TKEY_MODE_TypeDef MODEX,uint8_t SWQLENX,uint16_t ECLVLX,TKEY_CKSPR_TypeDef CKSPRX,TKEY_CKRND_TypeDef CKRNDx,
					TKEY_CKFEQ_TypeDef CKFEQX,TKEY_RSSEL_TypeDef RSSELX,uint16_t PSELX,uint16_t FVRLVLX,TKEY_IDLEP_TypeDef IDLEPX,TKEY_DSR_TypeDef DSRX,
					TKEY_TSSTB_TypeDef TSSTBX,uint8_t OTHRCNTX)
{
	if(PSELX==TK_PSEL_FVR)
	{
		GPIOA0->CONLR=(GPIOA0->CONLR&0XFFFFF0FF) | 0x00000800;
		ADC0->ECR|=2;
		if(FVRLVLX==TK_FVR_4096V)
		{
			ADC0->CR=(ADC0->CR&0XFCFFC3F)|(0X03<<6)|(1<<24)|(1<<25);
		}
		if(FVRLVLX==TK_FVR_2048V)
		{
			ADC0->CR=(ADC0->CR&0XFCFFC3F)|(0X03<<6)|(1<<24)|(1<<25);
			ECLVLX=TK_EC_1V;							//fvr=2.048 只能选1V
		}
	}
	TKEY->TCH_CON0 &= 0X00000000;
	TKEY->TCH_CON0 |=HMENX|MODEX|(SWQLENX<<2)|(ECLVLX<<7)|CKSPRX|CKRNDx|CKFEQX|RSSELX|(PSELX<<13)|IDLEPX|DSRX|TSSTBX|(OTHRCNTX<<24);
}
/****************************************************
//TK scan configure
*****************************************************/
void csi_tkey_scan_config(uint8_t iconx,TKEY_DCKDIV_TypeDef DCKDIVX,uint8_t SCANTIMEX)
{
	TKEY->TCH_SCCR&=0;
	TKEY->TCH_SCCR|=(iconx<<8)|(DCKDIVX<<12);
	TKEY->TCH_SENPRD|=SCANTIMEX<<12;
}
/****************************************************
//TK INT configure
*****************************************************/
void csi_tkey_config_interrupt_cmd(uint8_t NewState,uint32_t TK_IMSCR_X)
{
	if (NewState != DISABLE)
	{
		TKEY->TCH_IER  |= TK_IMSCR_X;
	}
	else
	{
		TKEY->TCH_IER  &= ~TK_IMSCR_X;
	}
}
/****************************************************
//TK IO ENABLE
*****************************************************/
void csi_tkey_io_enable(void)
{
    uint8_t i;
	byTkeyChNumber = 0;
	GPIOA0->CONLR=(GPIOA0->CONLR&0XFF0FFFFF) | 0x00900000;
	for (i=0;i<17;i++)
	{
		if ((wTkeyIoEnable & (1<<i))!=0)
		{
			switch (i)
			{
				case 0 :GPIOB0->CONLR=(GPIOB0->CONLR&0XFFFFFF0F) | 0x00000090;break;
				case 1 :GPIOA0->CONLR=(GPIOA0->CONLR&0XFFFFFFF0) | 0x00000009;break;
				case 2 :GPIOA0->CONLR=(GPIOA0->CONLR&0XFFFFFF0F) | 0x00000090;break;
				case 3 :GPIOA0->CONLR=(GPIOA0->CONLR&0XFFFFF0FF) | 0x00000900;break;
				case 4 :GPIOA0->CONLR=(GPIOA0->CONLR&0XFFFF0FFF) | 0x00009000;break;
				case 5 :GPIOA0->CONLR=(GPIOA0->CONLR&0XFFF0FFFF) | 0x00090000;break;
				case 6 :GPIOB0->CONLR=(GPIOB0->CONLR&0XFFFFF0FF) | 0x00000900;break;
				case 7 :GPIOB0->CONLR=(GPIOB0->CONLR&0XFFFF0FFF) | 0x00009000;break;
				case 8 :GPIOA0->CONHR=(GPIOA0->CONHR&0XFFFFFFF0) | 0x00000009;break;
				case 9 :GPIOA0->CONHR=(GPIOA0->CONHR&0XFFFFFF0F) | 0x00000090;break;
				case 10:GPIOA0->CONHR=(GPIOA0->CONHR&0XFFFFF0FF) | 0x00000900;break;
				case 11:GPIOA0->CONHR=(GPIOA0->CONHR&0XFFFF0FFF) | 0x00009000;break;
				case 12:GPIOA0->CONHR=(GPIOA0->CONHR&0XFFF0FFFF) | 0x00090000;break;
				case 13:GPIOA0->CONHR=(GPIOA0->CONHR&0XFF0FFFFF) | 0x00900000;break;
				case 14:GPIOB0->CONLR=(GPIOB0->CONLR&0XFFFFFFF0) | 0x00000009;break;
				case 15:GPIOB0->CONLR=(GPIOB0->CONLR&0XFFF0FFFF) | 0x00090000;break;
				case 16:GPIOB0->CONLR=(GPIOB0->CONLR&0XFF0FFFFF) | 0x00900000;break;
		
			}		
			byTkeyChNumber++;
		}
	}
}


/****************************************************
//TK IO ENABLE
*****************************************************/
void csi_tkey_sampling_prog(void)
{
	uint8_t i,j=0;
	uint32_t wTkeyChFlag = wTkeyIoEnable;
	for(i=0;i!=byTkeyChNumber;i++)
	{
		for(;j!=17;j++)
    	{
    		if((wTkeyChFlag >> j)&0x1)
    		{
    			wTkeyChFlag&=~(0x1<<j);
    			break;
    		}

    	}
		if(byScanStep==1)hwSamplingData0[j]=TKEY->TCH_CHVAL[j];
		if(byScanStep==2)hwSamplingData1[j]=TKEY->TCH_CHVAL[j];
		if(byScanStep==0)hwSamplingData2[j]=TKEY->TCH_CHVAL[j];
		if(byBaseUpdata==1)
		{
			hwBaselineData0[j]=hwSamplingData0[j];
			hwBaselineData1[j]=hwSamplingData1[j];
			hwBaselineData2[j]=hwSamplingData2[j];
			
		}
		if(byBaseUpdata0 == 1)hwBaselineData0[j]=hwSamplingData0[j];
		if(byBaseUpdata1 == 1)hwBaselineData1[j]=hwSamplingData1[j];
		if(byBaseUpdata2 == 1)hwBaselineData2[j]=hwSamplingData2[j];
	}
	byBaseUpdata=0;
	byBaseUpdata0=0;
	byBaseUpdata1=0;
	byBaseUpdata2=0;
}

void csi_tkey_lpt_delinit(void)
{
	LPT->CEDR 		= 0xBE980000;
	LPT->RSSR 		= LPT_RESET_VALUE;
	LPT->PSCR 		= LPT_RESET_VALUE;
	LPT->CR	  		= 0X00010010;
	LPT->SYNCR 		= LPT_RESET_VALUE;
	LPT->PRDR 		= LPT_RESET_VALUE;
	LPT->CMP 		= LPT_RESET_VALUE;
	LPT->CNT 		= LPT_RESET_VALUE;
	LPT->TRGFCR 	= LPT_RESET_VALUE;
	LPT->TRGFWR 	= LPT_RESET_VALUE;
	LPT->EVTRG 		= LPT_RESET_VALUE;
	LPT->EVPS		= LPT_RESET_VALUE;
	LPT->EVSWF 		= LPT_RESET_VALUE;
	LPT->IMCR 		= LPT_RESET_VALUE;
	LPT->ICR 		= LPT_RESET_VALUE;
}

/****************************************************
//LPT LowPower Initial
*****************************************************/
void csi_tkey_lpt_init(void)
{
	LPT->CEDR 		= 0xbe9c0045;
	LPT->RSSR 		= 0;
	LPT->PSCR 		= 0;
	LPT->CR	  		= 0X00010010;
	LPT->SYNCR 		= 0;
	LPT->PRDR 		= hwLptScanPendCnt;
	LPT->CMP 		= hwLptScanPendCnt/2;
	LPT->TRGFCR		= 0x01;
	LPT->TRGFWR		= 0x50005;
	LPT->EVTRG		= 0x100002;
	LPT->RSSR		= 1;
}
/****************************************************
//LPT LowPower Initial
*****************************************************/
void csi_tkey_setup_sleep(void)
{
	if(byTkeyLowpowerMode==ENABLE)
	{
		SYSCON->PCER1 |=0X1<<9;
		csi_tkey_lpt_init();
		if(hwTkeyPselMode == TK_PSEL_FVR)
		{
			SYSCON->PWRKEY = 0xA67A6CC7;
			SYSCON->PWRCR  = 0x031F1F04;
		
		}else{
			SYSCON->PWRKEY = 0xA67A6CC7;
			SYSCON->PWRCR  = 0x021F1F04;
		}
		TKEY->TCH_SYNCR=0x00000024;
		csi_tkey_config_interrupt_cmd(DISABLE,TKEY_DNE);
		csi_tkey_config_interrupt_cmd(DISABLE,TKEY_TIME);
		csi_tkey_config_interrupt_cmd(ENABLE,TKEY_THR);
	}
}
/****************************************************
//LPT LowPower Initial
*****************************************************/
void csi_tkey_quit_sleep(void)
{
	csi_tkey_lpt_delinit();
	TKEY->TCH_SYNCR=0x00000000;
	csi_tkey_config_interrupt_cmd(ENABLE,TKEY_DNE);
	csi_tkey_config_interrupt_cmd(ENABLE,TKEY_TIME);
	csi_tkey_config_interrupt_cmd(DISABLE,TKEY_THR);
}
/*************************************************************/
//CORET Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/

void csi_tkey_timer_handler(void)
{
    
	if(byTkeyBaseOverFlag==1)								//baseline scan success
	{
		csi_tkey_scan_start();
		csi_tkey_keymap_prog();
		csi_tkey_baseline_tracking();
		csi_tkey_result_prog();
		if((wKeyMap!=0)&&(wTkeyLongPressTime!=0))
		{
			wTimeCnt++;
			if(wTimeCnt>(wTkeyLongPressTime*100))		//cnt base 10ms*100=1s
			{
				byBaseUpdata=1;
				wTimeCnt=0;
			}
		}
	}
}


/*************************************************************/
//TKEY Interrupt
//EntryParameter:NONE
//ReturnValue:NONE
/*************************************************************/

void csi_tkey_handler(void)
{
    // ISR content ...
	if(wTkeyStateFlag==0)
	{
		wTkeyStateFlag=1;
		if(byTkeyBaseOverFlag==1)
		{
			byScanStep++;
			if(byScanStep==3)byScanStep=0;
			csi_tkey_sampling_prog();
			bySampSetoverFlag=1;
		}
	}
	if((TKEY->TCH_RISR&TKEY_SINDNE)==TKEY_SINDNE)
	{
		TKEY->TCH_ICR = TKEY_SINDNE;
	}
	if((TKEY->TCH_RISR&TKEY_DNE)==TKEY_DNE)
	{
		TKEY->TCH_ICR = TKEY_DNE;
	}
	if((TKEY->TCH_RISR&TKEY_THR)==TKEY_THR)
	{
		TKEY->TCH_ICR = TKEY_THR;
	}
	if((TKEY->TCH_RISR&TKEY_FLW)==TKEY_FLW)
	{
		TKEY->TCH_ICR = TKEY_FLW;
	}
	if((TKEY->TCH_RISR&TKEY_OVW)==TKEY_OVW)
	{
		TKEY->TCH_ICR = TKEY_OVW;
	}
	if((TKEY->TCH_RISR&TKEY_TIME)==TKEY_TIME)
	{
		TKEY->TCH_ICR = TKEY_TIME;
	}

}


/****************************************************
//TK get press number
*****************************************************/
uint8_t csi_get_key_number(void)
{
    int count = 0;
    int i = 0;
    for (; i<32; i++)
    {
        if (((wKeyMap >> i) & 1) == 1)
            count++;
    }
    return count;
}
/****************************************************
//TK baseline program
*****************************************************/
void csi_tkey_baseline_prog(void)
{
	uint8_t i,j=0,k;
	uint8_t jj=0;
	uint32_t wTkeyChFlag = wTkeyIoEnable;
	
	for(k=0;k<=5;k++)
	{
		
		for (jj=0;jj<=2;jj++)
		{
			switch(byScanStep)
			{
				case 0:*(uint32_t *)0x40011240 = wTkeyScanFreq0;byScanStep++;break;
				case 1:*(uint32_t *)0x40011240 = wTkeyScanFreq1;byScanStep++;break;
				case 2:*(uint32_t *)0x40011240 = wTkeyScanFreq2;byScanStep=0;break;
			}
			TKEY->TCH_CON1   = 0X01;
			wTkeyStateFlag=0;
			while(wTkeyStateFlag == 0);
			nop;
			//if(k==5){}
			j=0;
			wTkeyChFlag = wTkeyIoEnable;
			for (i=0;i!=byTkeyChNumber;i++)
			{
				for(;j!=17;j++)
				{
					if((wTkeyChFlag >> j)&0x1)
					{
						wTkeyChFlag&=~(0x1<<j);
						break;
					}

				}
				if(byScanStep==1)
				{
					hwBaselineData0[j]=TKEY->TCH_CHVAL[j];
					hwSamplingData0[j]=TKEY->TCH_CHVAL[j];
				}
				if(byScanStep==2)
				{
					hwBaselineData1[j]=TKEY->TCH_CHVAL[j];
					hwSamplingData1[j]=TKEY->TCH_CHVAL[j];
				}
				if(byScanStep==0)
				{
					hwBaselineData2[j]=TKEY->TCH_CHVAL[j];
					hwSamplingData2[j]=TKEY->TCH_CHVAL[j];
				}
			}
			
		}
	}
	byTkeyBaseOverFlag=1;
	byScanStep=0;
}
/****************************************************
//TK Scan Start
*****************************************************/
void csi_tkey_scan_start(void)
{
	if(byScanStepTemp==byScanStep)return;
	switch(byScanStep)
	{
		case 0:*(uint32_t *)0x40011240 = wTkeyScanFreq0;break;//513
		case 1:*(uint32_t *)0x40011240 = wTkeyScanFreq1;break;//647
		case 2:*(uint32_t *)0x40011240 = wTkeyScanFreq2;break;//785
	}
	byScanStepTemp=byScanStep;
	if(bySampSetoverFlag==1)
	{
		TKEY->TCH_CON1  = 0X01;
		wTkeyStateFlag=0;
		bySampSetoverFlag=0;
	}
}
/****************************************************
//TK get key map
*****************************************************/
void csi_tkey_keymap_prog(void)
{
	uint8_t i,j=0;
	uint32_t wTkeyChFlag = wTkeyIoEnable;
	for(i=0;i!=byTkeyChNumber;i++)
	{
		for(;j!=17;j++)
    	{
    		if((wTkeyChFlag >> j)&0x1)
    		{
    			wTkeyChFlag&=~(0x1<<j);
    			break;
    		}

    	}	
		nOffsetData0[j]=hwSamplingData0[j]-hwBaselineData0[j];
		nOffsetData1[j]=hwSamplingData1[j]-hwBaselineData1[j];
		nOffsetData2[j]=hwSamplingData2[j]-hwBaselineData2[j];
		if(nOffsetData0[j]<0)
		{
			hwOffseData0Abs[j]=0;
		}
		else
		{
			hwOffseData0Abs[j]=nOffsetData0[j];
		}
		if(nOffsetData1[j]<0)
		{
			hwOffseData1Abs[j]=0;
		}
		else
		{
			hwOffseData1Abs[j]=nOffsetData1[j];
		}
		if(nOffsetData2[j]<0)
		{
			hwOffseData2Abs[j]=0;
		}
		else
		{
			hwOffseData2Abs[j]=nOffsetData2[j];
		}
		//=================wKeyMap0============================
		if(byMultiTimesFilter>=4)
		{
			if((nOffsetData0[j]>hwTkeyTriggerLevel[j])&&(nOffsetData0[j]<(hwTkeyTriggerLevel[j]*byMultiTimesFilter)))
			{
				byPressDebounce0[j]++;
				byReleaseDebounce0[j]=0;
				byTkeyPostiveBuid0[j]=0;
				byTkeyNegtiveBuid0[j]=0;
				if(byPressDebounce0[j]>byPressDebounce)
				{
					if((byKeyMode==0)&&(wKeyMap0==0))
					{
						wKeyMap0|=0x01<<j;
					}
					if(byKeyMode==1)
					{
						wKeyMap0|=0x01<<j;
					}
					byPressDebounce0[j]=0;
				}
			}
			else if((nOffsetData0[j])>(hwTkeyTriggerLevel[j]*byMultiTimesFilter))
			{
				wKeyMap0&=~(0x01<<j);
			}
		}
		else
		{
			if(nOffsetData0[j]>hwTkeyTriggerLevel[j])
			{
				byPressDebounce0[j]++;
				byReleaseDebounce0[j]=0;
				byTkeyPostiveBuid0[j]=0;
				byTkeyNegtiveBuid0[j]=0;
				if(byPressDebounce0[j]>byPressDebounce)
				{
					if((byKeyMode==0)&&(wKeyMap0==0))
					{
						wKeyMap0|=0x01<<j;
					}
					if(byKeyMode==1)
					{
						wKeyMap0|=0x01<<j;
					}
					byPressDebounce0[j]=0;
				}
			}
		}
		if(nOffsetData0[j]<(hwTkeyTriggerLevel[j]*4/5))
		{
			byReleaseDebounce0[j]++;
			byPressDebounce0[j]=0;
			if(byReleaseDebounce0[j]>byReleaseDebounce)
			{
				wKeyMap0&=~(0x01<<j);
				byReleaseDebounce0[j]=0;
			}
		}
		//=====================wKeyMap1======================
		if(byMultiTimesFilter>=4)
		{
			if((nOffsetData1[j]>hwTkeyTriggerLevel[j])&&(nOffsetData1[j]<(hwTkeyTriggerLevel[j]*byMultiTimesFilter)))
			{
				byPressDebounce1[j]++;
				byReleaseDebounce1[j]=0;
				byTkeyPostiveBuid1[j]=0;
				byTkeyNegtiveBuid1[j]=0;
				if(byPressDebounce1[j]>byPressDebounce)
				{
					if((byKeyMode==0)&&(wKeyMap1==0))
					{
						wKeyMap1|=0x01<<j;
					}
					if(byKeyMode==1)
					{
						wKeyMap1|=0x01<<j;
					}
					byPressDebounce1[j]=0;
				}
			}
			else if((nOffsetData1[j])>(hwTkeyTriggerLevel[j]*byMultiTimesFilter))
			{
				wKeyMap1&=~(0x01<<j);
			}
		}
		else
		{
			if(nOffsetData1[j]>hwTkeyTriggerLevel[j])
			{
				byPressDebounce1[j]++;
				byReleaseDebounce1[j]=0;
				byTkeyPostiveBuid1[j]=0;
				byTkeyNegtiveBuid1[j]=0;
				if(byPressDebounce1[j]>byPressDebounce)
				{
					if((byKeyMode==0)&&(wKeyMap1==0))
					{
						wKeyMap1|=0x01<<j;
					}
					if(byKeyMode==1)
					{
						wKeyMap1|=0x01<<j;
					}
					byPressDebounce1[j]=0;
				}
			}
		}
		if(nOffsetData1[j]<(hwTkeyTriggerLevel[j]*4/5))
		{
			byReleaseDebounce1[j]++;
			byPressDebounce1[j]=0;
			if(byReleaseDebounce1[j]>byReleaseDebounce)
			{
				wKeyMap1&=~(0x01<<j);
				byReleaseDebounce1[j]=0;
			}
		}
		//========================wKeyMap2======================
		if(byMultiTimesFilter>=4)
		{
			if((nOffsetData2[j]>hwTkeyTriggerLevel[j])&&(nOffsetData2[j]<(hwTkeyTriggerLevel[j]*byMultiTimesFilter)))
			{
				byPressDebounce2[j]++;
				byReleaseDebounce2[j]=0;
				byTkeyPostiveBuid2[j]=0;
				byTkeyNegtiveBuid2[j]=0;
				if(byPressDebounce2[j]>byPressDebounce)
				{
					if((byKeyMode==0)&&(wKeyMap2==0))
					{
						wKeyMap2|=0x01<<j;
					}
					if(byKeyMode==1)
					{
						wKeyMap2|=0x01<<j;
					}
					byPressDebounce2[j]=0;
				}
			}
			else if((nOffsetData2[j])>(hwTkeyTriggerLevel[j]*byMultiTimesFilter))
			{
				wKeyMap2&=~(0x01<<j);
			}
		}
		else
		{
			if(nOffsetData2[j]>hwTkeyTriggerLevel[j])
			{
				byPressDebounce2[j]++;
				byReleaseDebounce2[j]=0;
				byTkeyPostiveBuid2[j]=0;
				byTkeyNegtiveBuid2[j]=0;
				if(byPressDebounce2[j]>byPressDebounce)
				{
					if((byKeyMode==0)&&(wKeyMap2==0))
					{
						wKeyMap2|=0x01<<j;
					}
					if(byKeyMode==1)
					{
						wKeyMap2|=0x01<<j;
					}
					byPressDebounce2[j]=0;
				}
			}
		}
		if(nOffsetData2[j]<(hwTkeyTriggerLevel[j]*4/5))
		{
			byReleaseDebounce2[j]++;
			byPressDebounce2[j]=0;
			if(byReleaseDebounce2[j]>byReleaseDebounce)
			{
				wKeyMap2&=~(0x01<<j);
				byReleaseDebounce2[j]=0;
			}
		}
	}
}
/****************************************************
//TK baseline tracking
*****************************************************/
void csi_tkey_baseline_tracking(void)
{
	uint8_t i,j=0;
	uint32_t wTkeyChFlag = wTkeyIoEnable;
	byTkeyTrackCnt++;
	if (byTkeyTrackCnt>=byBaseSpeed)
	{
		byTkeyTrackCnt=0;
		//======================================
		if(wKeyMap0==0)
		{
			j=0;
			for(i=0;i!=byTkeyChNumber;i++)
			{
				for(;j!=17;j++)
				{
					if((wTkeyChFlag >> j)&0x1)
					{
						wTkeyChFlag&=~(0x1<<j);
						break;
					}

				}	
				if((nOffsetData0[j]<0)&&((hwBaselineData0[j]-hwSamplingData0[j])>=(hwTkeyTriggerLevel[j]))&&((hwBaselineData0[j]-hwSamplingData0[j])<(hwTkeyTriggerLevel[j]*3)))
				{
					byTkeyNegtiveBuid0[j]++;
					if(byTkeyNegtiveBuid0[j]>3)
					{
						byBaseUpdata0=1;
						byTkeyNegtiveBuid0[j]=0;
					}
				}
				if((nOffsetData0[j]>0)&&(hwSamplingData0[j]-hwBaselineData0[j])>=(hwTkeyTriggerLevel[j]*4))
				{
					byTkeyPostiveBuid0[j]++;
					if(byTkeyPostiveBuid0[j]>3)
					{
						byBaseUpdata0=1;
						byTkeyPostiveBuid0[j]=0;
					}
				}
				else if((nOffsetData0[j]<0)&&((hwBaselineData0[j]-hwSamplingData0[j])>=(hwTkeyTriggerLevel[j]*3)))
				{
					byBaseUpdata0=1;
					nop;
				}
				if((nOffsetData0[j]<0)&&((hwBaselineData0[j]-hwSamplingData0[j])<hwTkeyTriggerLevel[j]))
				{
					hwBaselineData0[j]-=1;
				}
				if((nOffsetData0[j]<0)&&((hwBaselineData0[j]-hwSamplingData0[j])>=(hwTkeyTriggerLevel[j]/2)))
				{
					hwBaselineData0[j]-=2;
				}
				if ((nOffsetData0[j]>0)&&(nOffsetData0[j]<(hwTkeyTriggerLevel[j]/2)))
				{
					hwBaselineData0[j]+=1;
				}
				if ((nOffsetData0[j]>0)&&(nOffsetData0[j]<hwTkeyTriggerLevel[j])&&(nOffsetData0[j]>=(hwTkeyTriggerLevel[j]/2)))
				{
					hwBaselineData0[j]+=2;
				}
			}
		}
		///==================================
		if(wKeyMap1==0)
		{
			j=0;
			wTkeyChFlag = wTkeyIoEnable;
			for(i=0;i!=byTkeyChNumber;i++)
			{
				
				for(;j!=17;j++)
				{
					if((wTkeyChFlag >> j)&0x1)
					{
						wTkeyChFlag&=~(0x1<<j);
						break;
					}

				}
				if((nOffsetData1[j]<0)&&((hwBaselineData1[j]-hwSamplingData1[j])>=(hwTkeyTriggerLevel[j]))&&((hwBaselineData1[j]-hwSamplingData1[j])<(hwTkeyTriggerLevel[j]*3)))
				{
					byTkeyNegtiveBuid1[j]++;
					if(byTkeyNegtiveBuid1[j]>3)
					{
						byBaseUpdata1=1;
						byTkeyNegtiveBuid1[j]=0;
					}
				}
				if((nOffsetData1[j]>0)&&(hwSamplingData1[j]-hwBaselineData1[j])>=(hwTkeyTriggerLevel[j]*4))
				{
					byTkeyPostiveBuid1[j]++;
					if(byTkeyPostiveBuid1[j]>3)
					{
						byBaseUpdata1=1;
						byTkeyPostiveBuid1[j]=0;
					}
				}
				else if((nOffsetData1[j]<0)&&((hwBaselineData1[j]-hwSamplingData1[j])>=(hwTkeyTriggerLevel[j]*3)))
				{
					byBaseUpdata1=1;
					nop;
				}
				if((nOffsetData1[j]<0)&&((hwBaselineData1[j]-hwSamplingData1[j])<hwTkeyTriggerLevel[j]))
				{
					hwBaselineData1[j]--;
				}
				if((nOffsetData1[j]<0)&&((hwBaselineData1[j]-hwSamplingData1[j])>=(hwTkeyTriggerLevel[j]/2)))
				{
					hwBaselineData1[j]-=2;
				}
				if ((nOffsetData1[j]>0)&&(nOffsetData1[j]<(hwTkeyTriggerLevel[j]/2)))
				{
					hwBaselineData1[j]+=1;
				}
				if ((nOffsetData1[j]>0)&&(nOffsetData1[j]<hwTkeyTriggerLevel[j])&&(nOffsetData1[j]>=(hwTkeyTriggerLevel[j]/2)))
				{
					hwBaselineData1[j]+=2;
				}
			}
		}
		//======================================
		if(wKeyMap2==0)
		{
			j=0;
			wTkeyChFlag = wTkeyIoEnable;
			for(i=0;i!=byTkeyChNumber;i++)
			{
				for(;j!=17;j++)
				{
					if((wTkeyChFlag >> j)&0x1)
					{
						wTkeyChFlag&=~(0x1<<j);
						break;
					}

				}
				if((nOffsetData2[j]<0)&&((hwBaselineData2[j]-hwSamplingData2[j])>=(hwTkeyTriggerLevel[j]))&&((hwBaselineData2[j]-hwSamplingData2[j])<(hwTkeyTriggerLevel[j]*3)))
				{
					byTkeyNegtiveBuid2[j]++;
					if(byTkeyNegtiveBuid2[j]>3)
					{
						byBaseUpdata2=1;
						byTkeyNegtiveBuid2[j]=0;
					}
				}
				if((nOffsetData2[j]>0)&&(hwSamplingData2[j]-hwBaselineData2[j])>=(hwTkeyTriggerLevel[j]*4))
				{
					byTkeyPostiveBuid2[j]++;
					if(byTkeyPostiveBuid2[j]>3)
					{
						byBaseUpdata2=1;
						byTkeyPostiveBuid2[j]=0;
					}
				}
				else if((nOffsetData2[j]<0)&&((hwBaselineData2[j]-hwSamplingData2[j])>=(hwTkeyTriggerLevel[j]*3)))
				{
					byBaseUpdata2=1;
					nop;
				}
				if((nOffsetData2[j]<0)&&((hwBaselineData2[j]-hwSamplingData2[j])<hwTkeyTriggerLevel[j]))
				{
					hwBaselineData2[j]-=1;
				}
				if((nOffsetData2[j]<0)&&((hwBaselineData2[j]-hwSamplingData2[j])>=(hwTkeyTriggerLevel[j]/2)))
				{
					hwBaselineData2[j]-=2;
				}
				if ((nOffsetData2[j]>0)&&(nOffsetData2[j]<(hwTkeyTriggerLevel[j]/2)))
				{
					hwBaselineData2[j]+=1;
				}
				if ((nOffsetData2[j]>0)&&(nOffsetData2[j]<hwTkeyTriggerLevel[j])&&(nOffsetData2[j]>=(hwTkeyTriggerLevel[j]/2)))
				{
					hwBaselineData2[j]+=2;
				}
			}
		}
	}
}
/****************************************************
//TK result program
*****************************************************/
void csi_tkey_result_prog(void)
{
	uint32_t wKeyMapTemp;
	wKeyMapTemp=wKeyMap0|wKeyMap1|wKeyMap2;
	if(wKeyMapTemp!=0)
	{
		if((wKeyMap0==wKeyMap1)&&(wKeyMap1==wKeyMap2))		//三组频率按键一致
		{
			wKeyMap=wKeyMap0;							//0,1,2
		}
		else if((wKeyMap1!=0)&&(wKeyMap2!=0)&&(wKeyMap1==wKeyMap2))
		{
			wKeyMap=wKeyMap1;							//1,2
			if(wKeyMap0!=wKeyMap)wKeyMap0=0;
		}
		else if((wKeyMap0!=0)&&(wKeyMap1!=0)&&(wKeyMap0==wKeyMap1))
		{
			wKeyMap=wKeyMap0;							//0,1
			if(wKeyMap2!=wKeyMap)wKeyMap2=0;
		}
		else if((wKeyMap0!=0)&&(wKeyMap2!=0)&&(wKeyMap0==wKeyMap2))
		{
			wKeyMap=wKeyMap0;							//0,2
			if(wKeyMap1!=wKeyMap)wKeyMap1=0;
		}
		else 
		{
			wKeyMap=0;
		}
	}
	else 
	{
		wKeyMap=0;
	}
	if(wKeyMap!=0)
	{
		if(wKeyMap!=wKeyMapTemp)
		{
			wKeyMapTemp=wKeyMap;
		}
		if((csi_get_key_number()>byValidKeyNum))
		{
			wKeyMap=0;
			wKeyMapTemp=0;
		}
	}
	else
	{
		wKeyMapTemp=0;
		wTimeCnt=0;
	}
}
/****************************************************
//TK get press number
*****************************************************/
uint8_t csi_get_key_seq(void)
{                  
    int count = 0;
    int i = 0,j=0;
    for (; i<17; i++)
    {
        if (((wTkeyIoEnable >> i) & 1) == 1)
		{
            count++;
			byTkeySeque[j]=i;
			j++;
		}
    }
    return count;
}
void csi_tkey_chxval_seqxcon_clr(void)
{
	uint8_t i;
	for(i=0;i<18;i++)
	{
		TKEY->TCH_CHVAL[i]=0;
		TKEY->TCH_SEQCON[i]=0;
	}
}
//**************************************************
void csi_tkey_init(void)
{
	uint8_t i;
	SYSCON->PCER0 |=0X1<<6;
	*(uint32_t *)0x400111FC = 0x69966996;			//使能频率调整
	csi_tkey_parameter_init();
	if(byTkeyLowpowerMode==DISABLE)
	{
		wTkeyScanFreq0=0x80007;		//513*4 k
		wTkeyScanFreq1=0x8000a;		//647*4 k
		wTkeyScanFreq2=0x8000d;		//785*4 k
	}
	if(byTkeyLowpowerMode==ENABLE)
	{
		wTkeyScanFreq0=0x8000a;		//513*4 k
		wTkeyScanFreq1=0x8000a;		//647*4 k
		wTkeyScanFreq2=0x8000a;		//785*4 k
		TKEY->TCH_THR=byTkeyWakeupLevel/5;
		switch (byTkeypowerLevel)
		{
			case 0: hwLptScanPendCnt=600;break;
			case 1: hwLptScanPendCnt=1500;break;
			case 2: hwLptScanPendCnt=3000;break;
			case 3: hwLptScanPendCnt=4500;break;
			case 4: hwLptScanPendCnt=6000;break;
			default: hwLptScanPendCnt=3000;break;
		}
	}
	csi_tkey_io_enable();
	csi_tkey_clk_config(ENABLE,0,0);
	csi_tkey_chxval_seqxcon_clr();
	byTkeyNum=csi_get_key_seq();
	sci_tkey_con0_config(TK_HM_EN,TK_SEQ,(byTkeyNum-1),hwTkeyECLevel,TK_CKSPR_DIS,TK_CKRND_DIS,TK_CKREQ_HIGH,
				TK_RSSEL_OverTHR,hwTkeyPselMode,hwTkeyFVRLevel,TK_IDLEP_DIS,TK_DSR_HIGH,TK_STB_2,1);
	csi_tkey_config_interrupt_cmd(ENABLE,TKEY_DNE);
	csi_tkey_config_interrupt_cmd(ENABLE,TKEY_TIME);
	csi_irq_enable((uint32_t *)TKEY);
	soc_pm_config_wakeup_source(WKUP_TCH, ENABLE);
	for(i=0;i<byTkeyNum;i++)
	{
		TKEY->TCH_SEQCON[i]=(hwTkeyClkDiv[byTkeySeque[i]]<<24)|(hwTkeyIcon[byTkeySeque[i]]<<20)|(byTkeySeque[i]<<15)|(byTkeyScanTime[byTkeySeque[i]]<<12)|(hwTkeySenprd[byTkeySeque[i]]<<0);
	}
	csi_tkey_baseline_prog();
}

