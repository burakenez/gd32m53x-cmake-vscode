/*!
    \file    main.c
    \brief   timer1 overflow trigger group output

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

/* configure RCU peripheral */
void rcu_config(void);
/* configure GPIO peripheral */
void gpio_config(void);
/* configure EVIC peripheral */
void evic_config(void);
/* configure the TIMER peripheral */
void timer_config(void);


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure RCU peripheral */
    rcu_config();
    /* configure GPIO peripheral */
    gpio_config();
    /* configure EVIC peripheral */
    evic_config();
    /* configure the TIMER peripheral */
    timer_config();

    while(1) {
    }
}

/*!
    \brief      configure RCU peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOD clock */
    rcu_periph_clock_enable(RCU_GPIOF);
    /* enable DAC clock */
    rcu_periph_clock_enable(RCU_TIMER1);
    /* enable EVIC clock */
    rcu_periph_clock_enable(RCU_EVIC);
}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PF8, PF9 as output */
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_8 | GPIO_PIN_9);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_8 | GPIO_PIN_9);
}

/*!
    \brief      configure the EVIC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void evic_config(void)
{
    /* configure TIMER1 overflow as trigger source */
    evic_init(EVENT_INTERCONNECT_OUTGRPF, EVIC_SOURCE_TIMER1_OVERFLOW);
    /* lock the event interconnect register */
    evic_register_lock_set(EVENT_INTERCONNECT_OUTGRPF);

    /* configure evic group member */
    evic_group_member_config(GPIO_PIN_8 | GPIO_PIN_9, EVIC_GRPIO_PORT_F, ENABLE);
    /* configure evic group output level */
    evic_group_output_level_config(EVIC_GRPIO_OUTPUT_CIRCULAR_DATA, EVIC_GRPIO_PORT_F);
    /* set group holding data value */
    evic_data_set(0x02, EVIC_GRPIO_PORT_F);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    timer_parameter_struct timer_initpara;

    /* TIMER1 deinit */
    timer_deinit(TIMER1);
    /* TIMER1 configuration */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 179;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 9999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    /* enable a TIMER */
    timer_enable(TIMER1);

}
