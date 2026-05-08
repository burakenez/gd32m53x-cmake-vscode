/*!
    \file    main.c
    \brief   TIMER0 output asymmetric PWM and adc trigger for GD32M53x

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

/* configure the TIMER peripheral */
void timer_config(void);
/* configure the GPIO ports */
void gpio_config(void);

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
    /* enable GPIOF clock */
    rcu_periph_clock_enable(RCU_GPIOF);

    /*configure PD9(TIMER0 CH0) as alternate function*/
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_9);
    gpio_af_set(GPIOD, GPIO_AF_1, GPIO_PIN_9);

    /*configure PD10(TIMER0 CH1) as alternate function*/
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_10);
    gpio_af_set(GPIOD, GPIO_AF_1, GPIO_PIN_10);

    /*configure PF10(ADCSM1) as alternate function*/
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_10);
    gpio_af_set(GPIOF, GPIO_AF_5, GPIO_PIN_10);
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
    - CH0, CH1 and CH2 output asymmetric PWM waveform.
    TIMER0 frequency = CK_TIMER/180 = 1MHz. The PWM frequency is 1MHz/(100*2) = 5KHz.
    - CH0 configuration in asymmetric PWM mode, the high level duty cycle 30% ((40+20)/200).
    - CH1 configuration in asymmetric PWM mode, the high level duty cycle 70% ((100-40+100-20)/200).
    TIMER0_TRGA configuration:
    - Compare time��count up only
    - Repetiton value: 1
    - ADCCR1 register upadate event: TIMER_UPDATE_NEXT_OVERFLOW
    - ADC trigger signal monitoring function: use ADCSM1 pin and frequency is 1.25kHz (5kHz/2/2).
    ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER0);
    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 179;
    timer_initpara.alignedmode       = TIMER_COUNTER_CENTER_BOTH;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 100;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* TIMER0 CH1/CH1/CH2 output configuration */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);
    timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocinitpara);

    /* CH0 configuration in composite PWM mode 0 */
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_ASYMMETRIC_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
    /* the up counter matches 40 */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 40);
    /* the down counter matches 20 */
    timer_channel_additional_compare_value_config(TIMER0, TIMER_CH_0, 20);

    /* CH1 configuration in composite PWM mode 1 */
    timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_ASYMMETRIC_PWM1);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);
    /* the up counter matches 40 */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, 40);
    /* the down counter matches 20 */
    timer_channel_additional_compare_value_config(TIMER0, TIMER_CH_1, 20);

    /* configure ADCCR1 regiseter value: 50 */
    timer_trigger_adc_compare_value_config(TIMER0, TIMER_ADC_COMPARE1, 50);
    /* enable TIMER0 adc compare regiseter shadow function */
    timer_trigger_adc_compare_value_shadow_enable(TIMER0, TIMER_ADCTL_ADTPREEN1);
    /* select ADCCR1 register update event: TIMER_UPDATE_NEXT_OVERFLOW*/
    timer_register_update_event_select(TIMER0, TIMER_ADCCR1_UPSEL, TIMER_UPDATE_NEXT_OVERFLOW);
    /* configure the TIMER trigger ADC repetition regiseter value: 1 */
    timer_trigger_adc_repetition_value_config(TIMER0, TIMER_ADC_REPA, 1);
    /* select adc repetition_decrement source: TIMER_ADC_REP_DEC_UNDERFLOW */
    timer_trigger_adc_repetition_decrement_select(TIMER0, TIMER_ADC_REPA, TIMER_ADC_REP_DEC_UNDERFLOW);
    /* enable compare produce ADC signal: TIMER_ADCRCTL_ADT1UPGTEN */
    timer_trigger_adc_compare_enable(TIMER0, TIMER_ADCRCTL_ADT1UPGTEN);
    /* configure ADC trigger signal monitoring function: ADCSM1 */
    timer_trigger_adc_monitor_config(TIMER0, SYSCFG_TADSRCFG_ADCSM1, SYSCFG_ADCSM1_TIMER0_TRGA, ENABLE);

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
