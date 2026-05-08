/*!
    \file    main.c
    \brief   EVIC select single I/O as trigger source to TIMER1 ITI3 example

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
    /* enable GPIOE clock */
    rcu_periph_clock_enable(RCU_GPIOE);
    /* enable DAC clock */
    rcu_periph_clock_enable(RCU_TIMER1);
    /* enable EVIC clock */
    rcu_periph_clock_enable(RCU_EVIC);
    /* enable SYSCFG clock */
    rcu_periph_clock_enable(RCU_SYSCFG);
}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PF8 as TIMER1_CH3 output */
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_8);
    gpio_af_set(GPIOF, GPIO_AF_1, GPIO_PIN_8);

    /* configure PE12 as input */
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_12);
}

/*!
    \brief      configure the EVIC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void evic_config(void)
{
    /* configure single I/O input detection 0 as TIMER1 trigger source */
    evic_init(EVENT_INTERCONNECT_TIMER1, EVIC_SOURCE_SGIO_INPUT_DETECTION0);
    /* lock the event interconnect register */
    evic_register_lock_set(EVENT_INTERCONNECT_TIMER1);

    /* configure evic single I/O */
    evic_single_io_config(EVIC_SGIO_PIN_12, EVIC_SGIO_PORT_E, SGIO0);
    /* configure evic single I/O input detection edge */
    evic_single_io_edge_detection_config(EVIC_SGIO_DETECTION_RISING, SGIO0);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    /* extern input trigger TIMER1 ---------------------------------------
       TIMER1 is slave for EVIC which seclect extern signal as trigger source,
       - PWM mode is used.
       - The ITI3(TIMER1) is used as input trigger.

      -------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    /* TIMER1 deinit */
    timer_deinit(TIMER1);
    /* TIMER1 configuration */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 2999;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 9999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* configure TIMER channel output function */
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER1, TIMER_CH_3, &timer_ocinitpara);

    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_3, 4999);
    /* CH3 configuration in PWM mode 0 */
    timer_channel_output_mode_config(TIMER1, TIMER_CH_3, TIMER_OC_MODE_PWM0);
    /* configure TIMER channel output shadow function */
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    /* select TIMER input trigger source: internal trigger 3(ITI3) */
    timer_input_trigger_source_select(TIMER1, TIMER_SMCFG_TRGSEL_ITI3);
    /* slave mode selection*/
    timer_slave_mode_select(TIMER1, TIMER_SLAVE_MODE_EVENT);
}
