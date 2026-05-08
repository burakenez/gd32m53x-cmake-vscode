/*!
    \file    main.c
    \brief   evic interconnect

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

/* configure the EVIC peripheral */
void evic_config(void);
/* configure the CPTIMER peripheral */
void cptimer_config(void);
/* configure the CPTIMERW peripheral */
void cptimerw_config(void);

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

    /* configure evic */
    evic_config();

    /* configure cptimerw */
    cptimerw_config();

    /* configure cptimer */
    cptimer_config();

    while(1) {

    }
}

/*!
    \brief      configure the EVIC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void evic_config(void)
{
    /* enable EVIC clock */
    rcu_periph_clock_enable(RCU_EVIC);

    /* set the event source for CPTIMER0 as GPIO input */
    evic_init(EVENT_INTERCONNECT_CPTIMERW, EVIC_SOURCE_CPTIMER0_COUNTER0_OVERFLOW);

    /* select CPTIMER0 slave mode as */
    evic_cptimer_slave_mode_select(TARGET_CPTIMERW, EVIC_EVENT_COUNT);
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

    /* configure counter0 count at 10KHz clock and 100ms period */
    cptimer_prescaler_config(CPTIMER0, CPTIMER_COUNTER0, 18000U - 1U);
    cptimer_autoreload_value_config(CPTIMER0, CPTIMER_COUNTER0, 1000U - 1U);

    /* enable counter0/1 update interrupt */
    cptimer_interrupt_flag_clear(CPTIMER0, CPTIMER_INT_FLAG_CNT0UP);
    cptimer_interrupt_enable(CPTIMER0, CPTIMER_INT_CNT0UP);

    /* enable CPTIMER irq */
    nvic_irq_enable(CPTIMER0_IRQn, 0, 0);

    /* enable CPTIMER counter0/1 */
    cptimer_enable(CPTIMER0, CPTIMER_COUNTER0);
}

/*!
    \brief      configure the CPTIMERW peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cptimerw_config(void)
{
    cptimerw_init_parameter_struct initpara;

    /* enable CPTIMERW clock */
    rcu_periph_clock_enable(RCU_CPTIMERW);

    /* deinit a CPTIMERW */
    cptimerw_deinit();

    /* initialize CPTIMERW init parameter struct */
    cptimerw_struct_para_init(&initpara);

    /* initialize CPTIMERW counter */
    initpara.prescaler = 0U;
    initpara.period = 10U - 1U;
    initpara.width = CPTIMERW_CNT_WIDTH_16BIT;
    initpara.clockdivision = CPTIMERW_CKDIV_DIV1;
    initpara.clear_source = CPTIMERW_CNT_CLEAR_DISABLE;
    cptimerw_init(&initpara);

    /* enable CPTIMERW update interrupt */
    cptimerw_interrupt_flag_clear(CPTIMERW_INT_FLAG_UP);
    cptimerw_interrupt_enable(CPTIMERW_INT_FLAG_UP);

    /* enable CPTIMERW irq */
    nvic_irq_enable(CPTIMERW_IRQn, 0, 0);

    /* enable CPTIMERW counter */
    cptimerw_enable();
}
