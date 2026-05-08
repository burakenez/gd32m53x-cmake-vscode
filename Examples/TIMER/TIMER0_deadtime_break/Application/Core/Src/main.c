/*!
    \file    main.c
    \brief   TIMER0 output composite PWM and output match pulse demo for GD32M53x

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

/* configure the GPIO ports */
void gpio_config(void);
/* configure the TIMER peripheral */
void timer_config(void);

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* enable GPIOD clock */
    rcu_periph_clock_enable(RCU_GPIOD);

    /*configure PD9/PD12(TIMER0 CH0/MCH0) as alternate function*/
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9 | GPIO_PIN_12);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_9 | GPIO_PIN_12);
    gpio_af_set(GPIOD, GPIO_AF_1, GPIO_PIN_9 | GPIO_PIN_12);


    /* configure PD8(TIMER0 BRKIN) as alternate function */
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_af_set(GPIOD, GPIO_AF_7, GPIO_PIN_8);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    /* -----------------------------------------------------------------------
    TIMER0 configuration:
    - generate 2 complementary PWM signals.

    - TIMER0 frequency is fixed to systemcoreclock, TIMER0 prescaler is equal to
      179, so TIMER0 counter frequency is 1MHz.

    - the duty cycle is computed as the following description:
      the CH0 duty cycle is set to 40% so MCH0 is set to 60%.

    - the deadtime of CH0/MCH0:
      CH0 and MCH0 insert a dead time equal to ((64+36)/(180MHz/4/2) = 4.44us.

    - configure the break feature, BRKIN active at high level, and
      using the automatic output enable feature.

    - use the locking parameters level0.
    ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;
    timer_break_parameter_struct timer_breakpara;

    rcu_periph_clock_enable(RCU_TIMER0);
    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 179;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV4;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* CH0/1 & MCH0/1 configuration in PWM mode */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_ENABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_HIGH;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    /* config CH0/1 & MCH0/1 as output*/
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);

    /* config MCH0~3 output complementary */
    timer_multi_mode_channel_mode_config(TIMER0, TIMER_CH_0, TIMER_MCH_MODE_COMPLEMENTARY);

    /* CH0 configuration in PWM mode 0,duty cycle 40% */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 400);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);


    /* BREAK configuration */
    timer_break_struct_para_init(&timer_breakpara);
    timer_breakpara.runoffstate         = TIMER_ROS_STATE_ENABLE;
    timer_breakpara.ideloffstate        = TIMER_IOS_STATE_ENABLE;
    timer_breakpara.deadtime            = 164U;
    timer_breakpara.outputautostate     = TIMER_OUTAUTO_ENABLE;
    timer_breakpara.protectmode         = TIMER_CCHP_PROT_OFF;
    timer_breakpara.breakstate          = TIMER_BREAK_ENABLE;
    timer_breakpara.breakfilter         = 0U;
    timer_breakpara.breakpolarity       = TIMER_BREAK_POLARITY_HIGH;

    timer_break_config(TIMER0, &timer_breakpara);

    /* select TIMER0_BRKIN0 with TIMER0_BRKIN0 pin */
    timer_break_external_input_enable(TIMER0);
    /* select TIMER0_BRKIN0 will not be inverted */
    timer_break_external_input_polarity_config(TIMER0, TIMER_BRKIN_POLARITY_NONINVERTED);

    /* TIMER0 primary output function enable */
    timer_primary_output_config(TIMER0, ENABLE);

    /* auto-reload preload enable */
    timer_enable(TIMER0);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure the GPIO ports */
    gpio_config();
    /* configure the TIMER peripheral */
    timer_config();

    while(1);
}
