/*!
    \file    main.c
    \brief   clock output via CKOUT pin example

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
#include "systick.h"
#include <stdio.h>
#include "gd32m531r_eval.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    /* configure systick */
    systick_config();
    /* enable peripheral clock */
    rcu_periph_clock_enable(RCU_GPIOF);
    /* configure clock output pin */
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_12);
    gpio_af_set(GPIOF, GPIO_AF_0, GPIO_PIN_12);

    /* configure CKOUT to output system clock */
    rcu_ckout_config(RCU_CKOUTSRC_CKSYS, RCU_CKOUT_DIV8);
    delay_1ms(2000);

    /* enable IRC32M clock */
    rcu_osci_on(RCU_IRC32M);
    if(ERROR == rcu_osci_stab_wait(RCU_IRC32M)) {
        while(1) {
        }
    }
    /* configure CKOUT to output IRC32M clock */
    rcu_ckout_config(RCU_CKOUTSRC_IRC32M, RCU_CKOUT_DIV2);
    delay_1ms(2000);

    /* enable IRC32K clock */
    rcu_osci_on(RCU_IRC32K);
    if(ERROR == rcu_osci_stab_wait(RCU_IRC32K)) {
        while(1) {
        }
    }
    /* configure CKOUT to output IRC32K clock */
    rcu_ckout_config(RCU_CKOUTSRC_IRC32K, RCU_CKOUT_DIV1);
    delay_1ms(2000);

    /* enable HXTAL clock */
    rcu_osci_on(RCU_HXTAL);
    if(ERROR == rcu_osci_stab_wait(RCU_HXTAL)) {
        while(1) {
        }
    }
    /* configure CKOUT to output HXTAL clock */
    rcu_ckout_config(RCU_CKOUTSRC_HXTAL, RCU_CKOUT_DIV1);
    delay_1ms(2000);

    /* enable PLL clock */
    rcu_osci_on(RCU_PLL_CK);
    if(ERROR == rcu_osci_stab_wait(RCU_PLL_CK)) {
        while(1) {
        }
    }
    /* configure CKOUT to output PLL clock */
    rcu_ckout_config(RCU_CKOUTSRC_CKPLL_DIV8, RCU_CKOUT_DIV4);
    delay_1ms(2000);

    while(1) {
    }
}
