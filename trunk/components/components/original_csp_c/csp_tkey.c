/*
  ******************************************************************************
  * @file    csp_tkey.c
  * @author  APT AE Team
  * @version V1.24
  * @date    2018/10/15
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

/* Includes ----------------------------------------------------------------*/
#include "csp_tkey.h"
#include "string.h"
#include "csp.h"
#include "soc.h"

/****************************************************
//TK scan freq define
*****************************************************/

uint32_t wTkeyScanFreq0=0x80007;		//513*4 k
uint32_t wTkeyScanFreq1=0x8000a;		//647*4 k
uint32_t wTkeyScanFreq2=0x8000c;		//785*4 k

//采样值上限=（senprd*16+15）*div，超过上限后扫描不能完成
//icon减小采样值增大
//senprd增加采样值增大
//trim频率较高后，需要配置div
/****************************************************
//TK variable define
*****************************************************/
volatile uint32_t wTkey = 1;
uint8_t 		byScanStep;
uint16_t 	hwScanCnt;
uint8_t 		byScan;
uint8_t 		byTkeySeque[17];
uint8_t 		byTkeyNumber;
uint32_t 	wKeyMap;
uint32_t 	wKeyMap0;
uint32_t 	wKeyMap1;
uint32_t 	wKeyMap2;
uint8_t 		byPressDebounce0[17];
uint8_t 		byReleaseDebounce0[17];
uint8_t 		byPressDebounce1[17];
uint8_t 		byReleaseDebounce1[17];
uint8_t 		byPressDebounce2[17];
uint8_t 		byReleaseDebounce2[17];
uint8_t 		byTkeyTrackCnt;
uint8_t 		byTkeyNegtiveBuild0[17];
uint8_t 		byTkeyNegtiveBuild1[17];
uint8_t 		byTkeyNegtiveBuild2[17];
uint8_t 		byTkeyPostiveBuild0[17];
uint8_t 		byTkeyPostiveBuild1[17];
uint8_t 		byTkeyPostiveBuild2[17];
uint32_t 	wTimeCnt;
uint32_t 	wTkeyMapTemp;
uint8_t		byBaseUpdata;
uint8_t		byBaseUpdata0;
uint8_t		byBaseUpdata1;
uint8_t		byBaseUpdata2;


/** \brief tkey clock configure
 * 
 *  \param[in] ptAdcBase: pointer of TKEY reg structure.
 *  \param[in] bNewState: ENABLE, DISABLE
 *  \param[in] byPckDivx: 0-63
 *  \param[in] byTckDivx: 0-255
 *  \return none
 */
void csp_tkey_clk_config(csp_tkey_t *ptTkeyBase,bool bNewState,uint8_t byPckDivx,uint8_t byTckDivx)
{
	if (bNewState != DISABLE)
	{
		ptTkeyBase->TCH_CCR  |= 0x01<<0;						
	}
	else
	{
		ptTkeyBase->TCH_CCR  &= 0xfffffffe;					
	}
	ptTkeyBase->TCH_CCR |= (byPckDivx<<2)|(byTckDivx<<8);
}


/** \brief tkey scan configure
 * 
 *  \param[in] ptAdcBase: pointer of TKEY reg structure.
 *  \param[in] iConx: 0-15
 *  \param[in] eDckDivx: 0-3
 *  \param[in] byScanTimex:SCANTIME
 *  \return none
 */
void csp_tkey_scan_config(csp_tkey_t *ptTkeyBase,uint8_t iConx,tkey_dckdiv_e eDckDivx,uint8_t byScanTimex)
{
	ptTkeyBase->TCH_SCCR&=0;
	ptTkeyBase->TCH_SCCR|=(iConx<<8)|(eDckDivx<<12);
	ptTkeyBase->TCH_SENPRD|=byScanTimex<<12;
}

/** \brief tkey interrupt enable/disable
 * 
 *  \param[in] ptAdcBase: pointer of TKEY reg structure.
 *  \param[in] eNewState: ENABLE, DISABLE
 *  \param[in] wTkeyIMSCRX: interrupt number
 *  \return none
 */
void csp_tkey_configinterrupt_cmd(csp_tkey_t *ptTkeyBase,bool bNewState,uint32_t wTkeyIMSCRX)
{
	if (bNewState != DISABLE)
	{
		ptTkeyBase->TCH_IER  |= wTkeyIMSCRX;						
	}
	else
	{
		ptTkeyBase->TCH_IER  &= ~wTkeyIMSCRX;					
	}
}

/** \brief tkey IO ENABLE
 * 
 *  \param[in] none
 *  \return none
 */
void csp_tkey_io_enable(void)
{
    uint8_t i;
	csp_gpio_t *ptGpioA0_base = NULL;
	csp_gpio_t *ptGpioB0_base = NULL;
	ptGpioA0_base = (csp_gpio_t *)APB_GPIOA0_BASE;
	ptGpioB0_base = (csp_gpio_t *)APB_GPIOB0_BASE;
	
	ptGpioA0_base->CONLR=(ptGpioA0_base->CONLR&0XFF0FFFFF) | 0x00900000;
	
	for(i=0;i<17;i++)
	{
		if((wTkeyIOEnable & (1<<i))!=0)
		{
			switch (i)
			{
				case 0:ptGpioB0_base->CONLR=(ptGpioB0_base->CONLR&0XFFFFFF0F) | 0x00000090;break;
				case 1:ptGpioA0_base->CONLR=(ptGpioA0_base->CONLR&0XFFFFFFF0) | 0x00000009;break;
				case 2:ptGpioA0_base->CONLR=(ptGpioA0_base->CONLR&0XFFFFFF0F) | 0x00000090;break;
				case 3:ptGpioA0_base->CONLR=(ptGpioA0_base->CONLR&0XFFFFF0FF) | 0x00000900;break;
				case 4:ptGpioA0_base->CONLR=(ptGpioA0_base->CONLR&0XFFFF0FFF) | 0x00009000;break;
				case 5:ptGpioA0_base->CONLR=(ptGpioA0_base->CONLR&0XFFF0FFFF) | 0x00090000;break;
				case 6:ptGpioB0_base->CONLR=(ptGpioB0_base->CONLR&0XFFFFF0FF) | 0x00000900;break;
				case 7:ptGpioB0_base->CONLR=(ptGpioB0_base->CONLR&0XFFFF0FFF) | 0x00009000;break;
				case 8:ptGpioA0_base->CONHR=(ptGpioA0_base->CONHR&0XFFFFFFF0) | 0x00000009;break;
				case 9:ptGpioA0_base->CONHR=(ptGpioA0_base->CONHR&0XFFFFFF0F) | 0x00000090;break;
				case 10:ptGpioA0_base->CONHR=(ptGpioA0_base->CONHR&0XFFFFF0FF) | 0x00000900;break;
				case 11:ptGpioA0_base->CONHR=(ptGpioA0_base->CONHR&0XFFFF0FFF) | 0x00009000;break;
				case 12:ptGpioA0_base->CONHR=(ptGpioA0_base->CONHR&0XFFF0FFFF) | 0x00090000;break;
				case 13:ptGpioA0_base->CONHR=(ptGpioA0_base->CONHR&0XFF0FFFFF) | 0x00900000;break;
				case 14:ptGpioB0_base->CONLR=(ptGpioB0_base->CONLR&0XFFFFFFF0) | 0x00000009;break;
				case 15:ptGpioB0_base->CONLR=(ptGpioB0_base->CONLR&0XFFF0FFFF) | 0x00090000;break;
				case 16:ptGpioB0_base->CONLR=(ptGpioB0_base->CONLR&0XFF0FFFFF) | 0x00900000;break;
		
			}
		}
	}
}

/** \brief tkey control register configure
 * 
 *  \param[in] ptAdcBase: pointer of TKEY reg structure.
 *  \param[in] wTchcn0: TCH_CON0
 *  \param[in] bySwqlenx: TKEY Swqlen
 *  \return none
 */
void csp_tkey_con0_config(csp_tkey_t *ptTkeyBase,uint32_t wTchcn0,uint8_t bySwqlenx)
{
	csp_adc_t *ptAdcBase = NULL;
	csp_gpio_t *ptGpioA0Base = NULL;
	ptGpioA0Base = (csp_gpio_t *)APB_GPIOA0_BASE;
	ptAdcBase = (csp_adc_t *)APB_ADC0_BASE;
	
	if(wTkeyPselMode==TK_PSEL_FVR)
	{
		ptGpioA0Base->CONLR=(ptGpioA0Base->CONLR&0XFFFFF0FF) | 0x00000800;
		ptAdcBase->ECR=2;
		if(wTkeyFvrLevel==TK_FVR_4096V)
		{
			ptAdcBase->CR=(1<<1)|(3<<6)|(1<<24)|(1<<25);
		}
		if(wTkeyFvrLevel==TK_FVR_2048V)
		{
			ptAdcBase->CR=(1<<1)|(3<<6)|(1<<24)|(0<<25);						
			wTchcn0 &= (~(0x00000180));
			wTchcn0 |= TK_EC_1V;	//fvr=2.048 只能选1V
		}
	}
	ptTkeyBase->TCH_CON0 &= 0X00000000;
	ptTkeyBase->TCH_CON0 |= wTchcn0|(bySwqlenx<<2);
}

uint32_t sleep_cnt;



/** \brief tkey get press number
 * 
 *  \param[in] NONE
 *  \return key number
 */
uint8_t csp_get_key_number(void)
{                  
    int count = 0;
    int i = 0;
    for (; i<17; i++)
    {
        if (((wKeyMap >> i) & 1) == 1)
            count++;
    }
    return count;
}


/** \brief tkey baseline program
 * 
 *  \param[in] ptAdcBase: pointer of TKEY reg structure.
 *  \return none
 */
void csp_tkey_baseline_prog(csp_tkey_t *ptTkeyBase)
{
	uint8_t i,j,k;
	for(k=0;k<=5;k++)
	{
		for (j=0;j<=2;j++)
		{
			switch(byScanStep)
			{
				case 0:*(uint32_t *)0x40011240 = wTkeyScanFreq0;byScanStep++;break;
				case 1:*(uint32_t *)0x40011240 = wTkeyScanFreq1;byScanStep++;break;
				case 2:*(uint32_t *)0x40011240 = wTkeyScanFreq2;byScanStep=0;break;
			}
			ptTkeyBase->TCH_CON1   = 0X01;
			wTkey=0;
			while(wTkey == 0);
			nop;
			for (i=0;i<17;i++)
			{
				if(byScanStep==1)hwBaselineData0[i]=ptTkeyBase->TCH_CHVAL[i];
				if(byScanStep==2)hwBaselineData1[i]=ptTkeyBase->TCH_CHVAL[i];
				if(byScanStep==0)hwBaselineData2[i]=ptTkeyBase->TCH_CHVAL[i];
			}
		}
	}
	nop;
	nop;
}







/** \brief TKey sampling program
 *
 *  \param[in] ptAdcBase: pointer of TKEY reg structure.
 *  \return none
 */
 
void csp_tkey_sampling_prog(csp_tkey_t *ptTkeyBase)
{
	uint8_t i;
	switch(byScanStep)
	{
		case 0:*(uint32_t *)0x40011240 = wTkeyScanFreq0;byScanStep++;break;//877
		case 1:*(uint32_t *)0x40011240 = wTkeyScanFreq1;byScanStep++;break;//877
		case 2:*(uint32_t *)0x40011240 = wTkeyScanFreq2;byScanStep=0;break;//877
	}	
	
	hwScanCnt=0;
	wTkey=0;
	ptTkeyBase->TCH_CON1   = 0X01;
	byScan=1;
	
	while(wTkey == 0);
	byScan=0;hwScanCnt=0;
	for (i=0;i<17;i++)
	{
		if(byScanStep==1)hwSamplData0[i]=ptTkeyBase->TCH_CHVAL[i];
		if(byScanStep==2)hwSamplData1[i]=ptTkeyBase->TCH_CHVAL[i];
		if(byScanStep==0)hwSamplData2[i]=ptTkeyBase->TCH_CHVAL[i];
		if(byBaseUpdata==1)
		{

			hwBaselineData0[i]=hwSamplData0[i];
			hwBaselineData1[i]=hwSamplData1[i];
			hwBaselineData2[i]=hwSamplData2[i];

		}
		if(byBaseUpdata0 == 1)hwBaselineData0[i]=hwSamplData0[i];
		if(byBaseUpdata1 == 1)hwBaselineData1[i]=hwSamplData1[i];
		if(byBaseUpdata2 == 1)hwBaselineData2[i]=hwSamplData2[i];
	
		
	}
	byBaseUpdata=0;
	byBaseUpdata0=0;
	byBaseUpdata1=0;
	byBaseUpdata2=0;
	

}



/** \brief Tkey get key map
 *
 *  \param[in] none
 *  \return none
 */
void csp_tkey_keymap_prog(void)
{
	uint8_t i;
	for(i=0;i<17;i++)
	{
		nOffsetData0[i]=hwSamplData0[i]-hwBaselineData0[i];
		nOffsetData1[i]=hwSamplData1[i]-hwBaselineData1[i];
		nOffsetData2[i]=hwSamplData2[i]-hwBaselineData2[i];
		//=================wKeyMap0============================
		if(byMultiTimesFilter>=4)
		{
			if((nOffsetData0[i]>hwTkeyTriggerLevel[i])&&(nOffsetData0[i]<(hwTkeyTriggerLevel[i]*byMultiTimesFilter)))
			{
				byPressDebounce0[i]++;
				byReleaseDebounce0[i]=0;
				byTkeyPostiveBuild0[i]=0;
				byTkeyNegtiveBuild0[i]=0;
				if(byPressDebounce0[i]>byPressDebounceData)
				{
					if((byTkeyMode==0)&&(wKeyMap0==0))
					{
						wKeyMap0|=0x01<<i;
					}
					if(byTkeyMode==1)
					{
						wKeyMap0|=0x01<<i;
					}
					byPressDebounce0[i]=0;
				}
			}
			else if((nOffsetData0[i])>(hwTkeyTriggerLevel[i]*byMultiTimesFilter))
			{
				wKeyMap0&=~(0x01<<i);
			}
		}
		else
		{
			if(nOffsetData0[i]>hwTkeyTriggerLevel[i])
			{
				byPressDebounce0[i]++;
				byReleaseDebounce0[i]=0;
				byTkeyPostiveBuild0[i]=0;
				byTkeyNegtiveBuild0[i]=0;
				if(byPressDebounce0[i]>byPressDebounceData)
				{
					if((byTkeyMode==0)&&(wKeyMap0==0))
					{
						wKeyMap0|=0x01<<i;
					}
					if(byTkeyMode==1)
					{
						wKeyMap0|=0x01<<i;
					}
					byPressDebounce0[i]=0;
				}
			}
		}
		if(nOffsetData0[i]<(hwTkeyTriggerLevel[i]*4/5))
		{
			byReleaseDebounce0[i]++;
			byPressDebounce0[i]=0;
			if(byReleaseDebounce0[i]>byReleaseDebounceData)
			{
				wKeyMap0&=~(0x01<<i);
				byReleaseDebounce0[i]=0;
			}
		}
		//=====================wKeyMap1======================
		if(byMultiTimesFilter>=4)
		{
			if((nOffsetData1[i]>hwTkeyTriggerLevel[i])&&(nOffsetData1[i]<(hwTkeyTriggerLevel[i]*byMultiTimesFilter)))
			{
				byPressDebounce1[i]++;
				byReleaseDebounce1[i]=0;
				byTkeyPostiveBuild1[i]=0;
				byTkeyNegtiveBuild1[i]=0;
				if(byPressDebounce1[i]>byPressDebounceData)
				{
					if((byTkeyMode==0)&&(wKeyMap1==0))
					{
						wKeyMap1|=0x01<<i;
					}
					if(byTkeyMode==1)
					{
						wKeyMap1|=0x01<<i;
					}
					byPressDebounce1[i]=0;
				}
			}
			else if((nOffsetData1[i])>(hwTkeyTriggerLevel[i]*byMultiTimesFilter))
			{
				wKeyMap1&=~(0x01<<i);
			}
		}
		else
		{
			if(nOffsetData1[i]>hwTkeyTriggerLevel[i])
			{
				byPressDebounce1[i]++;
				byReleaseDebounce1[i]=0;
				byTkeyPostiveBuild1[i]=0;
				byTkeyNegtiveBuild1[i]=0;
				if(byPressDebounce1[i]>byPressDebounceData)
				{
					if((byTkeyMode==0)&&(wKeyMap1==0))
					{
						wKeyMap1|=0x01<<i;
					}
					if(byTkeyMode==1)
					{
						wKeyMap1|=0x01<<i;
					}
					byPressDebounce1[i]=0;
				}
			}
		}
		if(nOffsetData1[i]<(hwTkeyTriggerLevel[i]*4/5))
		{
			byReleaseDebounce1[i]++;
			byPressDebounce1[i]=0;
			if(byReleaseDebounce1[i]>byReleaseDebounceData)
			{
				wKeyMap1&=~(0x01<<i);
				byReleaseDebounce1[i]=0;
			}
		}
		//========================wKeyMap2======================
		if(byMultiTimesFilter>=4)
		{
			if((nOffsetData2[i]>hwTkeyTriggerLevel[i])&&(nOffsetData2[i]<(hwTkeyTriggerLevel[i]*byMultiTimesFilter)))
			{
				byPressDebounce2[i]++;
				byReleaseDebounce2[i]=0;
				byTkeyPostiveBuild2[i]=0;
				byTkeyNegtiveBuild2[i]=0;
				if(byPressDebounce2[i]>byPressDebounceData)
				{
					if((byTkeyMode==0)&&(wKeyMap2==0))
					{
						wKeyMap2|=0x01<<i;
					}
					if(byTkeyMode==1)
					{
						wKeyMap2|=0x01<<i;
					}
					byPressDebounce2[i]=0;
				}
			}
			else if((nOffsetData2[i])>(hwTkeyTriggerLevel[i]*byMultiTimesFilter))
			{
				wKeyMap2&=~(0x01<<i);
			}
		}
		else
		{
			if(nOffsetData2[i]>hwTkeyTriggerLevel[i])
			{
				byPressDebounce2[i]++;
				byReleaseDebounce2[i]=0;
				byTkeyPostiveBuild2[i]=0;
				byTkeyNegtiveBuild2[i]=0;
				if(byPressDebounce2[i]>byPressDebounceData)
				{
					if((byTkeyMode==0)&&(wKeyMap2==0))
					{
						wKeyMap2|=0x01<<i;
					}
					if(byTkeyMode==1)
					{
						wKeyMap2|=0x01<<i;
					}
					byPressDebounce2[i]=0;
				}
			}
		}
		if(nOffsetData2[i]<(hwTkeyTriggerLevel[i]*4/5))
		{
			byReleaseDebounce2[i]++;
			byPressDebounce2[i]=0;
			if(byReleaseDebounce2[i]>byReleaseDebounceData)
			{
				wKeyMap2&=~(0x01<<i);
				byReleaseDebounce2[i]=0;
			}
		}
		
	}
}

/** \brief  Tkey  baseline tracking
 *
 *  \param[in] none
 *  \return none
 */
void csp_tkey_baseline_tracking(void)
{
	uint8_t i;
	byTkeyTrackCnt++;
	if (byTkeyTrackCnt>=5)
	{
		byTkeyTrackCnt=0;
		//======================================
		if(wKeyMap0==0)
		{
			for(i=0;i<17;i++)
			{
				if((nOffsetData0[i]<0)&&((hwBaselineData0[i]-hwSamplData0[i])>=(hwTkeyTriggerLevel[i]))&&((hwBaselineData0[i]-hwSamplData0[i])<(hwTkeyTriggerLevel[i]*3)))
				{
					byTkeyNegtiveBuild0[i]++;
					if(byTkeyNegtiveBuild0[i]>byBaseSpeed)
					{
						byBaseUpdata0=1;
						byTkeyNegtiveBuild0[i]=0;
					}
				}
				if((nOffsetData0[i]>0)&&(hwSamplData0[i]-hwBaselineData0[i])>=(hwTkeyTriggerLevel[i]*4))
				{
					byTkeyPostiveBuild0[i]++;
					if(byTkeyPostiveBuild0[i]>byBaseSpeed)
					{
						byBaseUpdata0=1;
						byTkeyPostiveBuild0[i]=0;
					}
				}
				else if((nOffsetData0[i]<0)&&((hwBaselineData0[i]-hwSamplData0[i])>=(hwTkeyTriggerLevel[i]*2)))
				{
					byBaseUpdata0=1;
					nop;
				}
				if((nOffsetData0[i]<0)&&((hwBaselineData0[i]-hwSamplData0[i])<hwTkeyTriggerLevel[i]))
				{
					hwBaselineData0[i]-=1;
				}
				if((nOffsetData0[i]<0)&&((hwBaselineData0[i]-hwSamplData0[i])>=(hwTkeyTriggerLevel[i]/2)))
				{
					hwBaselineData0[i]-=2;
				}
				if ((nOffsetData0[i]>0)&&(nOffsetData0[i]<(hwTkeyTriggerLevel[i]/2)))
				{
					hwBaselineData0[i]+=1;
				}
				if ((nOffsetData0[i]>0)&&(nOffsetData0[i]<hwTkeyTriggerLevel[i])&&(nOffsetData0[i]>=(hwTkeyTriggerLevel[i]/2)))
				{
					hwBaselineData0[i]+=2;
				}
			}
		}
		///==================================
		if(wKeyMap1==0)
		{
			for(i=0;i<17;i++)
			{
				if((nOffsetData1[i]<0)&&((hwBaselineData1[i]-hwSamplData1[i])>=(hwTkeyTriggerLevel[i]))&&((hwBaselineData1[i]-hwSamplData1[i])<(hwTkeyTriggerLevel[i]*3)))
				{
					byTkeyNegtiveBuild1[i]++;
					if(byTkeyNegtiveBuild1[i]>byBaseSpeed)
					{
						byBaseUpdata1=1;
						byTkeyNegtiveBuild1[i]=0;
					}
				}
				if((nOffsetData1[i]>0)&&(hwSamplData1[i]-hwBaselineData1[i])>=(hwTkeyTriggerLevel[i]*4))
				{
					byTkeyPostiveBuild1[i]++;
					if(byTkeyPostiveBuild1[i]>byBaseSpeed)
					{
						byBaseUpdata1=1;
						byTkeyPostiveBuild1[i]=0;
					}
				}
				else if((nOffsetData1[i]<0)&&((hwBaselineData1[i]-hwSamplData1[i])>=(hwTkeyTriggerLevel[i]*2)))
				{
					byBaseUpdata1=1;
					nop;
				}
				if((nOffsetData1[i]<0)&&((hwBaselineData1[i]-hwSamplData1[i])<hwTkeyTriggerLevel[i]))
				{
					hwBaselineData1[i]--;
				}
				if((nOffsetData1[i]<0)&&((hwBaselineData1[i]-hwSamplData1[i])>=(hwTkeyTriggerLevel[i]/2)))
				{
					hwBaselineData1[i]-=2;
				}
				if ((nOffsetData1[i]>0)&&(nOffsetData1[i]<(hwTkeyTriggerLevel[i]/2)))
				{
					hwBaselineData1[i]+=1;
				}
				if ((nOffsetData1[i]>0)&&(nOffsetData1[i]<hwTkeyTriggerLevel[i])&&(nOffsetData1[i]>=(hwTkeyTriggerLevel[i]/2)))
				{
					hwBaselineData1[i]+=2;
				}
			}
		}
		//======================================
		if(wKeyMap2==0)
		{
			for(i=0;i<17;i++)
			{
				if((nOffsetData2[i]<0)&&((hwBaselineData2[i]-hwSamplData2[i])>=(hwTkeyTriggerLevel[i]))&&((hwBaselineData2[i]-hwSamplData2[i])<(hwTkeyTriggerLevel[i]*3)))
				{
					byTkeyNegtiveBuild2[i]++;
					if(byTkeyNegtiveBuild2[i]>byBaseSpeed)
					{
						byBaseUpdata2=1;
						byTkeyNegtiveBuild2[i]=0;
					}
				}
				if((nOffsetData2[i]>0)&&(hwSamplData2[i]-hwBaselineData2[i])>=(hwTkeyTriggerLevel[i]*4))
				{
					byTkeyPostiveBuild2[i]++;
					if(byTkeyPostiveBuild2[i]>byBaseSpeed)
					{
						byBaseUpdata2=1;
						byTkeyPostiveBuild2[i]=0;
					}
				}
				else if((nOffsetData2[i]<0)&&((hwBaselineData2[i]-hwSamplData2[i])>=(hwTkeyTriggerLevel[i]*2)))
				{
					byBaseUpdata2=1;
					nop;
				}
				if((nOffsetData2[i]<0)&&((hwBaselineData2[i]-hwSamplData2[i])<hwTkeyTriggerLevel[i]))
				{
					hwBaselineData2[i]-=1;
				}
				if((nOffsetData2[i]<0)&&((hwBaselineData2[i]-hwSamplData2[i])>=(hwTkeyTriggerLevel[i]/2)))
				{
					hwBaselineData2[i]-=2;
				}
				if ((nOffsetData2[i]>0)&&(nOffsetData2[i]<(hwTkeyTriggerLevel[i]/2)))
				{
					hwBaselineData2[i]+=1;
				}
				if ((nOffsetData2[i]>0)&&(nOffsetData2[i]<hwTkeyTriggerLevel[i])&&(nOffsetData2[i]>=(hwTkeyTriggerLevel[i]/2)))
				{
					hwBaselineData2[i]+=2;
				}
			}
		}
	}
}

/** \brief  Tkey result program
 *
 *  \param[in] none
 *  \return none
 */
void csp_tkey_result_prog(void)
{
	uint32_t Key_Map_temp;
	Key_Map_temp=wKeyMap0|wKeyMap1|wKeyMap2;
	if(Key_Map_temp!=0)
	{
		if((wKeyMap0==wKeyMap1)&&(wKeyMap1==wKeyMap2))		//三组频率按键一致
		{
			wKeyMap=wKeyMap0;								//0,1,2
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
		if(wKeyMap!=wTkeyMapTemp)
		{
			wTkeyMapTemp=wKeyMap;
		}
		if((csp_get_key_number()>byValidKeyNum))
		{
			wKeyMap=0;
			wTkeyMapTemp=0;
		}
	}
	else
	{
		wTkeyMapTemp=0;
		wTimeCnt=0;
	}
}
/****************************************************
//TK get press number
*****************************************************/
/** \brief tkey get press number
 * 
 *  \param[in] none
 *  \return Tkey seq
 */
uint8_t csp_get_key_seq(void)
{
    int count = 0;
    int i = 0,j=0;
    for (; i<17; i++)
    {
        if (((wTkeyIOEnable >> i) & 1) == 1)
		{
            count++;
			byTkeySeque[j]=i;
			j++;
		}
    }
    return count;
}


//**************************************************
/** \brief tkey init
 * 
 *  \param[in] ptAdcBase: pointer of TKEY reg structure.
 *  \return none
 */
void csp_tkey_init(csp_tkey_t *ptTkeyBase)
{

	*(uint32_t *)0x400111FC = 0x69966996;			//使能频率调整
	ptTkeyBase->TCH_CON1   = 0X0000;
	csp_tkey_io_enable();
	
	byTkeyNumber=csp_get_key_seq();
	wTkeyCon0 =(uint32_t) (TK_HM_EN|TK_SEQ|wTkeyECLevel|TK_CKSPR_DIS|TK_CKRND_DIS|TK_CKFEQ_HIGH|TK_RSSEL_OVW|wTkeyPselMode|TK_IDLEP_DIS|TK_DSR_LOW|TK_STB_2|TK_OTHRCNT_5);	
	csp_tkey_con0_config(ptTkeyBase,wTkeyCon0,(byTkeyNumber-1));
	//TKEY->TCH_CON0 &= 0XFFF0FFFF;	  //关闭二极管滤波
	csp_tkey_seqxcon_set(ptTkeyBase,byTkeyNumber);
	

	
}

//**************************************************
/** \brief tkey SEQCON set
 * 
 *  \param[in] ptAdcBase: pointer of TKEY reg structure.
 *  \param[in] eAdcInt: tkey number
 *  \return none
 */
void csp_tkey_seqxcon_set(csp_tkey_t *ptTkeyBase,uint8_t byNumber)
{
	uint8_t i;
	memset((void *)ptTkeyBase->TCH_CHVAL,0,4*32);
	memset((void *)ptTkeyBase->TCH_SEQCON,0,4*32);
	for(i=0;i<byNumber;i++)
	{
		ptTkeyBase->TCH_SEQCON[i]=(hwTkeyDiv[byTkeySeque[i]]<<24)|(hwTkeyIcon[byTkeySeque[i]]<<20)|(byTkeySeque[i]<<15)|(hwTkeySenprd[byTkeySeque[i]]<<0);
	}
}

/****************************************************
//TK test main
*****************************************************/
/** \brief tkey test main
 * 
 *  \param[in] ptAdcBase: pointer of TKEY reg structure.
 *  \return none
 */
void csp_tkey_prgm(csp_tkey_t *ptTkeyBase)
{
	csp_tkey_sampling_prog(ptTkeyBase);
	csp_tkey_keymap_prog();
	csp_tkey_baseline_tracking();
	csp_tkey_result_prog();
}
