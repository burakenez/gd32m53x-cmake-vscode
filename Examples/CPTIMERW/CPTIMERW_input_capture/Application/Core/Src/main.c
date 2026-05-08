/*!
    \file    main.c
    \brief   input capture

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
#include "systick.h"
#include "gd32m531r_eval.h"

extern __IO uint16_t fre;
/* configure the GPIO peripheral */
void gpio_config(void);
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
    /* systick config */
    systick_config();

    /* init com */
    gd_eval_com_init(EVAL_COM);

    /* configure GPIO */
    gpio_config();

    /* configure cptimerw */
    cptimerw_config();

    while(1) {
        delay_1ms(1000);
        printf("\r /**** CPTIMERW Input Capture Demo ****/\r\n");
        printf("the frequence is %d\n", fre);
    }
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* enable CPTIMERW clock */
    rcu_periph_clock_enable(RCU_GPIOF);

    /* configure PF13:CPTIMERW_IC0 */
    gpio_af_set(GPIOF, GPIO_AF_0, GPIO_PIN_13);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13);
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
    cptimerw_ic_parameter_struct icpara;

    /* enable CPTIMERW clock */
    rcu_periph_clock_enable(RCU_CPTIMERW);

    /* deinit a CPTIMERW */
    cptimerw_deinit();

    /* initialize CPTIMERW init parameter struct */
    cptimerw_struct_para_init(&initpara);

    /* initialize CPTIMERW counter count at 1MHz */
    initpara.prescaler = 180U - 1U;
    initpara.period = 0xFFFFFFFF;
    initpara.width = CPTIMERW_CNT_WIDTH_32BIT;
    initpara.clockdivision = CPTIMERW_CKDIV_DIV1;
    initpara.clear_source = CPTIMERW_CNT_CLEAR_ICH0;
    cptimerw_init(&initpara);

    /* config CPTIMERW CPTIMERW input capture */
    /* initialize CPTIMERW channel input capture parameter struct */
    cptimerw_channel_input_struct_para_init(&icpara);

    /* configure CPTIMERW input capture parameter */
    icpara.icfilter = 5U;
    icpara.icedge = CPTIMERW_IC_FALLING_EDGE;
    cptimerw_input_capture_config(CPTIMERW_ICH0, &icpara);

    /* enable CPTIMERW update interrupt */
    cptimerw_interrupt_flag_clear(CPTIMERW_INT_FLAG_ICH0);
    cptimerw_interrupt_enable(CPTIMERW_INT_ICH0);

    /* enable CPTIMERW irq */
    nvic_irq_enable(CPTIMERW_IRQn, 0, 0);

    /* enable CPTIMERW counter */
    cptimerw_enable();
}
