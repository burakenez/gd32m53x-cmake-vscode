/*!
    \file    main.c
    \brief   system clock switch example

    \version 2026-03-04, V1.0.0, firmware for GD32M53x
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32m53x.h"
#include <stdio.h>
#include "gd32m531r_eval.h"

/* The following is to prevent Vcore fluctuations caused by frequency switching.
   It is strongly recommended to include it to avoid issues caused by self-removal. */
#define RCU_MODIFY_DE_2(__delay)  do{                                     \
                                      volatile uint32_t i,reg;            \
                                      if(0 != __delay){                   \
                                          /* Insert a software delay */   \
                                          for(i=0; i<__delay; i++){       \
                                          }                               \
                                          reg = RCU_CFG0;                 \
                                          reg &= ~(RCU_CFG0_AHBPSC);     \
                                          reg |= RCU_AHB_CKSYS_DIV2;     \
                                          /* AHB = SYSCLK/2 */           \
                                          RCU_CFG0 = reg;                \
                                          /* Insert a software delay */  \
                                          for(i=0; i<__delay; i++){      \
                                          }                              \
                                          reg = RCU_CFG0;                \
                                          reg &= ~(RCU_CFG0_AHBPSC);     \
                                          reg |= RCU_AHB_CKSYS_DIV4;     \
                                          /* AHB = SYSCLK/4 */           \
                                          RCU_CFG0 = reg;                \
                                          /* Insert a software delay */  \
                                          for(i=0; i<__delay; i++){      \
                                          }                              \
                                      }                                  \
                                  }while(0)

/* The following is to prevent Vcore fluctuations caused by frequency switching.
   It is strongly recommended to include it to avoid issues caused by self-removal. */
#define RCU_MODIFY_UP_2(__delay)  do{                                     \
                                      volatile uint32_t i,reg;            \
                                      if(0U != (__delay)){                   \
                                          /* Insert a software delay */   \
                                          for(i=0U; i<(__delay); i++){       \
                                          }                               \
                                          reg = RCU_CFG0;                 \
                                          reg &= ~(RCU_CFG0_AHBPSC);     \
                                          reg |= RCU_AHB_CKSYS_DIV2;     \
                                          /* AHB = SYSCLK/2 */           \
                                          RCU_CFG0 = reg;                \
                                          /* Insert a software delay */  \
                                          for(i=0U; i<(__delay); i++){      \
                                          }                              \
                                          reg = RCU_CFG0;                \
                                          reg &= ~(RCU_CFG0_AHBPSC);     \
                                          reg |= RCU_AHB_CKSYS_DIV1;     \
                                          /* AHB = SYSCLK/1 */           \
                                          RCU_CFG0 = reg;                \
                                      }                                  \
                                  }while(0)

static void _delay(uint32_t timeout);
static void switch_system_clock_to_72m_hxtal(void);
static void switch_system_clock_to_180m_irc32m(void);
/* software delay to prevent the impact of Vcore fluctuations.
   It is strongly recommended to include it to avoid issues caused by self-removal. */
static void _soft_delay_(uint32_t time)
{
    __IO uint32_t i;
    for(i=0; i<time*10; i++){
    }
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    /* initialize the USART */
    gd_eval_com_init(EVAL_COM);
    printf("\r\nCK_SYS switch test demo\r\n");

    /* disable the USART */
    uart_disable(EVAL_COM);

    /* switch system clock to 72MHz by HXTAL */
    switch_system_clock_to_72m_hxtal();
    gd_eval_com_init(EVAL_COM);
    /* print out the clock frequency of system */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));

    _delay(1000);

    /* switch system clock to 180MHz by IRC32M */
    switch_system_clock_to_180m_irc32m();
    gd_eval_com_init(EVAL_COM);
    /* print out the clock frequency of system */
    printf("\r\nCK_SYS is %d", rcu_clock_freq_get(CK_SYS));

    while(1) {
    }
}

/*!
    \brief      delay function
    \param[in]  timeout: time out
    \param[out] none
    \retval     none
*/
static void _delay(uint32_t timeout)
{
    __IO uint32_t i, j;
    for(i = 0; i < timeout; i++) {
        for(j = 0; j < 500; j++) {
        }
    }
}

/*!
    \brief      switch system clock to 72M by HXTAL
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void switch_system_clock_to_72m_hxtal(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;
    __IO uint32_t reg_temp;

    /* open the protection */
    SYSCFG_PRCFG = 0x00000A503;
#if (HXTAL_VALUE > 10000000)
    /* enable HXTALSCAL: HXTAL > 10MHz*/
    RCU_CTL |= RCU_CTL_HXTALSCAL;
#endif

    /* It is strongly recommended to use it to avoid issues caused by self-removal. */
    RCU_MODIFY_DE_2(0x50);
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    /* It is strongly recommended to use it to avoid issues caused by self-removal. */
    _soft_delay_(100);
    /* reset HXTALEN, CKMEN, PLLEN, HXTALSCAL bits */
    RCU_CTL &= ~(RCU_CTL_PLLEN | RCU_CTL_CKMEN | RCU_CTL_HXTALEN | RCU_CTL_HXTALBPS | RCU_CTL_HXTALSCAL);
    /* disable all interrupts */
    RCU_INT = 0x00DD0000U;
    /* reset CFG0, CFG1, CFG2 registers */
    RCU_CFG0 = 0x001E0000U;
    RCU_CFG1 = 0x00000000U;
    RCU_CFG2 = 0x18000000U;
    /* reset HXTALEN, CKMEN, PLLEN, HXTALSCAL bits */
    RCU_CTL &= ~(RCU_CTL_PLLEN | RCU_CTL_CKMEN | RCU_CTL_HXTALEN | RCU_CTL_HXTALBPS | RCU_CTL_HXTALSCAL);

    /* enable HXTAL */
    RCU_CTL |= RCU_CTL_HXTALEN;

    /* wait until HXTAL is stable or the startup time is longer than HXTAL_STARTUP_TIMEOUT */
    do {
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_HXTALSTB);
    } while((0U == stab_flag) && (HXTAL_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_HXTALSTB)) {
        while(1) {
        }
    }

    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT(2);

    /* HXTAL is stable */
    /* AHB = SYSCLK/4 */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV4;
    /* APB2 = AHB/1 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB/2 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;

    /* CK_PLL = (CK_PREDIV) * 18 = 72 MHz */
    RCU_CFG0 &= ~(RCU_CFG0_PLLSEL | RCU_CFG0_PLLMF);
    RCU_CFG0 |= (RCU_PLLSRC_HXTAL | RCU_PLL_MUL18);

    /* CK_PREDIV = (CK_HXTAL)/2 = 4 MHz */
    RCU_CFG1 &= ~(RCU_CFG1_PREDV);
    RCU_CFG1 |= (RCU_PREDV_DIV2);

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)) {
    }

    reg_temp = RCU_CFG0;
    /* select PLL as system clock */
    reg_temp &= ~RCU_CFG0_SCS;
    reg_temp |= RCU_CKSYSSRC_PLL;
    RCU_CFG0 = reg_temp;

    /* wait until PLL is selected as system clock */
    while(RCU_SCSS_PLL != (RCU_CFG0 & RCU_CFG0_SCSS)) {
    }

    RCU_MODIFY_UP_2(0x50);
}

/*!
    \brief      switch system clock to 180M by IRC32M
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void switch_system_clock_to_180m_irc32m(void)
{
    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;
    __IO uint32_t reg_temp;

    /* open the protection */
    SYSCFG_PRCFG = 0x00000A503;
    /* enable IRC32M */
    RCU_CTL |= RCU_CTL_IRC32MEN;

    /* wait until IRC32M is stable or the startup time is longer than IRC32M_STARTUP_TIMEOUT */
    do {
        timeout++;
        stab_flag = (RCU_CTL & RCU_CTL_IRC32MSTB);
    } while((0U == stab_flag) && (IRC32M_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL & RCU_CTL_IRC32MSTB)) {
        while(1) {
        }
    }

    /* It is strongly recommended to use it to avoid issues caused by self-removal. */
    RCU_MODIFY_DE_2(0x50);
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    /* It is strongly recommended to use it to avoid issues caused by self-removal. */
    _soft_delay_(100);
    /* reset HXTALEN, CKMEN, PLLEN bits */
    RCU_CTL &= ~(RCU_CTL_PLLEN | RCU_CTL_CKMEN | RCU_CTL_HXTALEN | RCU_CTL_HXTALBPS);
    /* disable all interrupts */
    RCU_INT = 0x00DD0000U;
    /* reset CFG0, CFG1, CFG2 registers */
    RCU_CFG0 = 0x001E0400U;
    RCU_CFG1 = 0x00000000U;
    RCU_CFG2 = 0x18000000U;
    /* reset HXTALEN, CKMEN, PLLEN, HXTALSCAL bits */
    RCU_CTL &= ~(RCU_CTL_PLLEN | RCU_CTL_CKMEN | RCU_CTL_HXTALEN | RCU_CTL_HXTALBPS | RCU_CTL_HXTALSCAL);

    //The WSCNT bits in the FMC_WS register needs to be configured correctly depend on the AHB clock frequency when reading the flash memory
    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT(6);

    /* IRC32M is stable */
    /* AHB = SYSCLK/4 */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV4;
    /* APB2 = AHB/1 */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB/2 */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;

    /* CK_PLL = (CK_IRC32M/8) * 45 = 180 MHz */
    RCU_CFG1 &= ~RCU_CFG1_PREDV;
    RCU_CFG1 |= RCU_PREDV_DIV8;

    RCU_CFG0 &= ~(RCU_CFG0_PLLSEL | RCU_CFG0_PLLMF);
    RCU_CFG0 |= (RCU_PLLSRC_IRC32M | RCU_PLL_MUL45);

    /* enable PLL */
    RCU_CTL |= RCU_CTL_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL & RCU_CTL_PLLSTB)) {
    }

    reg_temp = RCU_CFG0;
    /* select PLL as system clock */
    reg_temp &= ~RCU_CFG0_SCS;
    reg_temp |= RCU_CKSYSSRC_PLL;
    RCU_CFG0 = reg_temp;

    /* wait until PLL is selected as system clock */
    while(RCU_SCSS_PLL != (RCU_CFG0 & RCU_CFG0_SCSS)) {
    }

    RCU_MODIFY_UP_2(0x50);
}
