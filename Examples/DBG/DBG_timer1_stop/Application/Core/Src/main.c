/*!
    \file    main.c
    \brief   UART print example

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

void gpio_configuration(void);
void timer_configuration(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{

    /* disable timer1 hold */
    dbg_periph_disable(DBG_TIMER1_HOLD);

    rcu_periph_clock_enable(RCU_GPIOF);

    /* configure gpio */
    gpio_configuration();

    gd_eval_led_init(LED1);

    systick_config();

    /* configure timer */
    timer_configuration();

    /* keep timer1 counter when the mcu is in debug mode */
    dbg_periph_enable(DBG_TIMER1_HOLD);

    while(1) {
        /* toggle LED1 output status every second */
        gd_eval_led_toggle(LED1);
        /* you can set breakpoint here, then look over the register in timer1 */
        delay_1ms(1000);
    }
}

/*!
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_configuration(void)
{
    /*Configure PF10 PF9 PF8(TIMER1 CH1 CH2 CH3) as alternate function*/
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_10);

    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_9);

    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_8);

    gpio_af_set(GPIOF, GPIO_AF_1, GPIO_PIN_10);
    gpio_af_set(GPIOF, GPIO_AF_1, GPIO_PIN_9);
    gpio_af_set(GPIOF, GPIO_AF_1, GPIO_PIN_8);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_configuration(void)
{

    /* TIMER1 configuration: generate 3 PWM signals with 3 different duty cycles:
    TIMER1CLK = 180 / 180 = 1MHz

    TIMER1 Channel1 duty cycle = 4000/ 16000  = 25%
    TIMER1 Channel2 duty cycle = 8000/ 16000  = 50%
    TIMER1 Channel3 duty cycle = 12000/ 16000 = 75% */

    timer_parameter_struct timer_initpara;
    timer_oc_parameter_struct timer_ocintpara;

    rcu_periph_clock_enable(RCU_TIMER1);

    /* TIMER1  configuration */
    timer_deinit(TIMER1);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 179;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 15999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    /* CH1,CH2 and CH3 configuration in PWM mode */
     timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER1, TIMER_CH_1, &timer_ocintpara);
    timer_channel_output_config(TIMER1, TIMER_CH_2, &timer_ocintpara);
    timer_channel_output_config(TIMER1, TIMER_CH_3, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, 3999);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_2, 7999);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);

    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_3, 11999);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_3, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    timer_enable(TIMER1);
}
