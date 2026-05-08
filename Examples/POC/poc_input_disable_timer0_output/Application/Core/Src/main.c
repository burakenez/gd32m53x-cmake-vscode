/*!
    \file    main.c
    \brief   POC input disable timer0 output

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

/* configure RCU peripheral */
void rcu_config(void);
/* configure GPIO peripheral */
void gpio_config(void);
/* configure POC peripheral */
void poc_config(void);
/* configure TIMER0 peripheral */
void timer0_config(void);

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

    /* configure RCU peripheral */
    rcu_config();
    /* configure GPIO peripheral */
    gpio_config();
    /* configure POC peripheral */
    poc_config();
    /* configure TIMER0 peripheral */
    timer0_config();

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
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable GPIOD clock */
    rcu_periph_clock_enable(RCU_GPIOD);
    /* enable POC clock */
    rcu_periph_clock_enable(RCU_POC);
    /* enable TIMER0 clock */
    rcu_periph_clock_enable(RCU_TIMER0);
}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /*configure PD9/PD12(TIMER0 CH0/MCH0) as alternate function*/
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9 | GPIO_PIN_12);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_9 | GPIO_PIN_12);
    gpio_af_set(GPIOD, GPIO_AF_1, GPIO_PIN_9 | GPIO_PIN_12);

    /*configure PD10/PD11(TIMER0 CH1/MCH1) as alternate function*/
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10 | GPIO_PIN_13);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_10 | GPIO_PIN_13);
    gpio_af_set(GPIOD, GPIO_AF_1, GPIO_PIN_10 | GPIO_PIN_13);

    /*configure PD11/PD14(TIMER0 CH2/MCH2) as alternate function*/
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11 | GPIO_PIN_14);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_11 | GPIO_PIN_14);
    gpio_af_set(GPIOD, GPIO_AF_1, GPIO_PIN_11 | GPIO_PIN_14);
}

/*!
    \brief      configure POC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void poc_config(void)
{
    poc_request_struct request;
    poc_request_struct_para_init(&request);
    /* configure POC_INn disabling request status */
    poc_input_dreq_status_config(POC_IN5, POC_INn_DREQ_ENABLE);
    /* configure POC_IN5 input polarity */
    poc_input_polarity_config(POC_IN5, POC_INPUT_POLARITY_NONINVERTED);
    /* configure PA1 as POC input */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_1);
    gpio_af_set(GPIOA, GPIO_AF_4, GPIO_PIN_1);
    /* configure POC_IN5 input detection */
    poc_input_detection_config(POC_IN5, POC_EDGE_DETECTION_DIV1, POC_SAMPLING_NUM_16_TIMES);

    /* request select target timer */
    request.req_comparator = TARGET_TIMER_NOT_SELECTED;
    request.req_pocin0 = TARGET_TIMER_NOT_SELECTED;
    request.req_pocin1 = TARGET_TIMER_NOT_SELECTED;
    request.req_pocin2 = TARGET_TIMER_NOT_SELECTED;
    request.req_pocin3 = TARGET_TIMER_NOT_SELECTED;
    request.req_pocin4 = TARGET_TIMER_NOT_SELECTED;
    request.req_pocin5 = TARGET_TIMER_SELECTED;
    poc_request_select(&request, TARGET_TIMER0);

    /* clear the POC flag */
    poc_flag_clear(POC_FLAG_IN5IF);

    /* select output disable mode for TIMER0 */
    poc_timer0_output_disable_mode_select(POC_HIGH_IMPEDANCE_OUT, POC_HIGH_IMPEDANCE_OUT, POC_HIGH_IMPEDANCE_OUT, POC_TIMER_CHANNEL_OUT);
}

/**
    \brief      configure TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer0_config(void)
{
    /* -----------------------------------------------------------------------
    TIMER0 configuration to:
    generate 3 complementary PWM signals with 3 different duty cycles:
    - TIMER0 frequency is fixed to 180MHz, TIMER0 prescaler is equal to 180,
       so TIMER0 counter frequency is 1MHz, the PWM frequency = 1KHz.

       the four duty cycles are computed as the following description:
       the channel 0 duty cycle is set to 20%, so multi mode channel 0 is set to 80%.
       the channel 1 duty cycle is set to 40%, so multi mode channel 1 is set to 60%.
       the channel 2 duty cycle is set to 60%, so multi mode channel 2 is set to 40%.
    - CH0/MCH0,CH1/MCH1 and CH2/MCH2 are configured in PWM mode 0.
    ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 179;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* CH0~3 & MCH0~3 configuration in PWM mode */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_ENABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_HIGH;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    /* config CH0~3 & MCH0~3 as output*/
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);
    timer_channel_output_config(TIMER0, TIMER_CH_1, &timer_ocinitpara);
    timer_channel_output_config(TIMER0, TIMER_CH_2, &timer_ocinitpara);

    /* config MCH0~3 output complementary */
    timer_multi_mode_channel_mode_config(TIMER0, TIMER_CH_0, TIMER_MCH_MODE_COMPLEMENTARY);
    timer_multi_mode_channel_mode_config(TIMER0, TIMER_CH_1, TIMER_MCH_MODE_COMPLEMENTARY);
    timer_multi_mode_channel_mode_config(TIMER0, TIMER_CH_2, TIMER_MCH_MODE_COMPLEMENTARY);

    /* CH0 configuration in PWM mode 0, duty cycle 20% */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 200);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    /* CH1 configuration in PWM mode 0, duty cycle 40% */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, 400);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    /* CH2 configuration in PWM mode 0, duty cycle 60% */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_2, 600);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_2, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_2, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    timer_primary_output_config(TIMER0, ENABLE);

    /* auto-reload preload enable */
    timer_enable(TIMER0);
}
