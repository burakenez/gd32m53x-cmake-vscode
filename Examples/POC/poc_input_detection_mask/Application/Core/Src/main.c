/*!
    \file    main.c
    \brief   POC input detection mask

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
/* configure TIMER1 peripheral */
void timer1_config(void);
/* configure NVIC peripheral */
void nvic_config(void);

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
    /* configure NVIC peripheral */
    nvic_config();
    /* configure GPIO peripheral */
    gpio_config();
    /* configure POC peripheral */
    poc_config();
    /* configure TIMER1 peripheral */
    timer1_config();

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
    /* enable GPIOE clock */
    rcu_periph_clock_enable(RCU_GPIOE);
    /* enable GPIOF clock */
    rcu_periph_clock_enable(RCU_GPIOF);
    /* enable TIMER1 clock */
    rcu_periph_clock_enable(RCU_TIMER1);
    /* enable POC clock */
    rcu_periph_clock_enable(RCU_POC);
}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /*configure PF10(TIMER1 CH1) as alternate function*/
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_10);
    gpio_af_set(GPIOF, GPIO_AF_1, GPIO_PIN_10);
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
    poc_input_dreq_status_config(POC_IN1, POC_INn_DREQ_ENABLE);
    /* configure POC_IN1 input polarity */
    poc_input_polarity_config(POC_IN1, POC_INPUT_POLARITY_INVERTED);
    /* configure PE14 as POC input */
    gpio_mode_set(GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO_PIN_14);
    gpio_af_set(GPIOE, GPIO_AF_6, GPIO_PIN_14);
    /* configure POC_IN1 input detection */
    poc_input_detection_config(POC_IN1, POC_LEVEL_DETECTION_DIV2, POC_SAMPLING_NUM_4_TIMES);

    /* request select target timer */
    request.req_comparator = TARGET_TIMER_NOT_SELECTED;
    request.req_pocin0 = TARGET_TIMER_NOT_SELECTED;
    request.req_pocin1 = TARGET_TIMER_SELECTED;
    request.req_pocin2 = TARGET_TIMER_NOT_SELECTED;
    request.req_pocin3 = TARGET_TIMER_NOT_SELECTED;
    request.req_pocin4 = TARGET_TIMER_NOT_SELECTED;
    request.req_pocin5 = TARGET_TIMER_NOT_SELECTED;
    poc_request_select(&request, TARGET_TIMER1);

    poc_flag_clear(POC_FLAG_IN1IF);

    /* mask POC_IN1 input detection */
    poc_input_detection_mask(POC_IN1, POC_MASK_SOURCE_TIMER1_CH0);
    /* select output disable mode for TIMER0 */
    poc_timer1_output_disable_mode_select(POC_TIMER_CHANNEL_OUT, POC_HIGH_IMPEDANCE_OUT, POC_TIMER_CHANNEL_OUT, POC_TIMER_CHANNEL_OUT);
}

/**
    \brief      configure TIMER1 peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer1_config(void)
{
    /* ---------------------------------------------------------------------------
    TIMER1 configuration: output compare toggle mode:
    TIMER1 frequency = systemcoreclock / 900 = 200KHz
    CH1 update rate = TIMER1 counter frequency / CH1VAL = 200000/4000 = 50 Hz
    ----------------------------------------------------------------------------*/
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    timer_deinit(TIMER1);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 899;
    timer_initpara.alignedmode       = TIMER_COUNTER_CENTER_UP;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 3999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    /* CH1 configuration in PWM1 mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER1, TIMER_CH_0, &timer_ocintpara);
    timer_channel_output_config(TIMER1, TIMER_CH_1, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_0, 3000);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_0, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    timer_channel_output_pulse_value_config(TIMER1, TIMER_CH_1, 1000);
    timer_channel_output_mode_config(TIMER1, TIMER_CH_1, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER1, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    /* clear TIMER interrupt flag */
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_CH0);
    /* enable the TIMER interrupt */
    timer_interrupt_enable(TIMER1, TIMER_INT_CH0);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    timer_enable(TIMER1);
}

/*!
    \brief      configure NVIC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable(TIMER1_IRQn, 1, 1);
}
