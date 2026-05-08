/*!
    \file    main.c
    \brief   CPTIMER periodly interrupt

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
#include "gd32m531r_eval.h"

/* configure the CPTIMER peripheral */
void cptimer_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize LED1/2 */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    /* configure cptimer */
    cptimer_config();

    while(1) {

    }
}

/*!
    \brief      configure the CPTIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cptimer_config(void)
{
    /* enable CPTIMER0 clock */
    rcu_periph_clock_enable(RCU_CPTIMER0);

    /* deinit a CPTIMER0 */
    cptimer_deinit(CPTIMER0);

    /* configure counter0 count at 10KHz clock and 200ms period */
    cptimer_prescaler_config(CPTIMER0, CPTIMER_COUNTER0, 18000U - 1U);
    cptimer_autoreload_value_config(CPTIMER0, CPTIMER_COUNTER0, 2000U - 1U);

    /* configure counter1 count at 10KHz clock and 400ms period */
    cptimer_prescaler_config(CPTIMER0, CPTIMER_COUNTER1, 18000U - 1U);
    cptimer_autoreload_value_config(CPTIMER0, CPTIMER_COUNTER1, 4000U - 1U);

    /* enable counter0/1 update interrupt */
    cptimer_interrupt_flag_clear(CPTIMER0, CPTIMER_INT_FLAG_CNT0UP | CPTIMER_INT_FLAG_CNT1UP);
    cptimer_interrupt_enable(CPTIMER0, CPTIMER_INT_CNT0UP | CPTIMER_INT_CNT1UP);

    /* enable CPTIMER irq */
    nvic_irq_enable(CPTIMER0_IRQn, 0, 0);

    /* enable CPTIMER counter0/1 */
    cptimer_enable(CPTIMER0, CPTIMER_COUNTER0);
    cptimer_enable(CPTIMER0, CPTIMER_COUNTER1);
}
