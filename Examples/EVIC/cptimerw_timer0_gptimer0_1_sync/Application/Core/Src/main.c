/*!
    \file    main.c
    \brief   cptimerw_timer0_gptimer0_1_sync

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
/* configure the CPTIMERW peripheral */
void cptimerw_config(void);
/* configure the GPTIMER peripheral */
void gptimer_config(void);

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
    /* configure the GPTIMER peripheral */
    gptimer_config();
    /* configure the CPTIMERW peripheral */
    cptimerw_config();

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
    /* enable GPIOF clock */
    rcu_periph_clock_enable(RCU_GPIOF);
    /* enable GPIOD clock */
    rcu_periph_clock_enable(RCU_GPIOD);
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable TIMER0 clock */
    rcu_periph_clock_enable(RCU_TIMER0);
    /* enable GPTIMER0 clock */
    rcu_periph_clock_enable(RCU_GPTIMER0);
    /* enable GPTIMER1 clock */
    rcu_periph_clock_enable(RCU_GPTIMER1);
    /* enable CPTIMERW clock */
    rcu_periph_clock_enable(RCU_CPTIMERW);
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
    /* configure PC8 as GPTIMER1_CH0 output */
    gpio_af_set(GPIOC, GPIO_AF_11, GPIO_PIN_8);
    gpio_mode_set(GPIOC, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_8);
    gpio_output_options_set(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_8);

    /* configure PF10 as GPTIMER0_CH0 output */
    gpio_af_set(GPIOF, GPIO_AF_11, GPIO_PIN_10);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_LOW, GPIO_PIN_10);

    /* configure PD9 as TIMER0_CH0 output */
    gpio_af_set(GPIOD, GPIO_AF_1, GPIO_PIN_9);
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_LOW, GPIO_PIN_9);
}

/*!
    \brief      configure the EVIC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void evic_config(void)
{
    evic_init(EVENT_INTERCONNECT_TIMER0, EVIC_SOURCE_CPTIMERW_OCH0_COMPARE_MATCH);

    evic_init(EVENT_INTERCONNECT_GPTIMER_TRIGIN0, EVIC_SOURCE_CPTIMERW_OCH0_COMPARE_MATCH);

    evic_init(EVENT_INTERCONNECT_GPTIMER_TRIGIN1, EVIC_SOURCE_CPTIMERW_OCH1_COMPARE_MATCH);
}

/*!
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    /* TIMER1 deinit */
    timer_deinit(TIMER0);
    /* TIMER1 configuration */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 179;
    timer_initpara.alignedmode       = TIMER_COUNTER_CENTER_UP;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 500;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* configure TIMER channel output function */
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);

    /* configure TIMER channel output pulse value */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 250);
    /* CH3 configuration in PWM mode 0 */
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    /* configure TIMER channel output shadow function */
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    /* select TIMER input trigger source: internal trigger 3(ITI3) */
    timer_input_trigger_source_select(TIMER0, TIMER_SMCFG_TRGSEL_ITI3);
    /* slave mode selection*/
    timer_slave_mode_select(TIMER0, TIMER_SLAVE_MODE_RESTART_EVENT);
    /* configure TIMER primary output function*/
    timer_primary_output_config(TIMER0, ENABLE);

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

    /* deinit a CPTIMERW */
    cptimerw_deinit();

    /* initialize CPTIMERW init parameter struct */
    cptimerw_struct_para_init(&initpara);

    /* initialize CPTIMERW counter */
    initpara.prescaler = 180U - 1U;
    initpara.period = 1000U - 1U;
    initpara.width = CPTIMERW_CNT_WIDTH_16BIT;
    initpara.clockdivision = CPTIMERW_CKDIV_DIV1;
    initpara.clear_source = CPTIMERW_CNT_CLEAR_DISABLE;
    cptimerw_init(&initpara);

    /* configure CPTIMERW channel compare value */
    cptimerw_channel_output_compare_value_config(CPTIMERW_OCH0, 100U);
    cptimerw_channel_output_compare_value_config(CPTIMERW_OCH1, 600U);

    /* enable compare channel 0 */
    cptimerw_channel_output_state_config(CPTIMERW_OCH0, CPTIMERW_OCX_ENABLE);
    /* enable compare channel 1 */
    cptimerw_channel_output_state_config(CPTIMERW_OCH1, CPTIMERW_OCX_ENABLE);

    /* enable CPTIMERW counter */
    cptimerw_enable();
}

/**
    \brief      configure GPTIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void gptimer_config(void)
{
    gptimer_parameter_struct gptimer_initpara;
    gptimer_oc_parameter_struct gptimer_ocpara;

    gptimer_counter_reset_source_parameter_struct reset_para;
    gptimer_counter_reset_source_struct_para_init(&reset_para);

    gptimer_deinit(GPTIMER0);
    gptimer_deinit(GPTIMER1);

    /* GPTIMER1 configuration */
    gptimer_struct_para_init(&gptimer_initpara);
    gptimer_initpara.clock_source      = GPTIMER_CLOCK_SOURCE_CK_GPTIMER;
    gptimer_initpara.clock_polarity    = GPTIMER_CLOCK_POLARITY_RISING;
    gptimer_initpara.prescaler         = 179;
    gptimer_initpara.counterdirection  = GPTIMER_COUNTER_DOWN;
    gptimer_initpara.alignedmode       = GPTIMER_COUNTER_CENTER;
    gptimer_initpara.period            = 500;
    gptimer_initpara.clockdivision     = GPTIMER_CKDIV_DIV1;
    gptimer_initpara.repetitioncounter = 0;
    gptimer_init(GPTIMER0, &gptimer_initpara);
    gptimer_init(GPTIMER1, &gptimer_initpara);

    /* GPTIMER1 output configuration */
    gptimer_channel_output_struct_para_init(&gptimer_ocpara);
    gptimer_ocpara.chxcv_up_output_level         = GPTIMER_MATCH_OUTPUT_HIGH;
    gptimer_ocpara.chxcv_down_output_level       = GPTIMER_MATCH_OUTPUT_LOW;
    gptimer_ocpara.chxcomv_add_up_output_level   = GPTIMER_MATCH_OUTPUT_HIGH;
    gptimer_ocpara.chxcomv_add_down_output_level = GPTIMER_MATCH_OUTPUT_LOW;
    gptimer_ocpara.period_end_output_level       = GPTIMER_PERIOD_END_OUTPUT_LOW;
    gptimer_ocpara.cnt_enable_disable_oc_en      = GPTIMER_CNT_ENABLE_DISABLE_OC_KEEP;
    gptimer_ocpara.cnt_enable_output_level       = GPTIMER_CNT_ENABLE_OUTPUT_LOW;
    gptimer_ocpara.cnt_disable_output_level      = GPTIMER_CNT_DISABLE_OUTPUT_LOW;
    gptimer_ocpara.output_stop_output_level      = GPTIMER_OUTPUT_STOP_OUTPUT_LOW;
    gptimer_ocpara.force_duty_output_mode        = GPTIMER_COMPARE_DUTY_OUTPUT;
    gptimer_ocpara.force_duty_end_output_level   = GPTIMER_FORCE_DUTY_END_BASE_OUTPUT_COMPARE;
    gptimer_channel_output_config(GPTIMER0, GPTIMER_CH0, &gptimer_ocpara);
    gptimer_channel_output_config(GPTIMER1, GPTIMER_CH0, &gptimer_ocpara);

    reset_para.reset_source_evsel0 = GPTIMER_EVSEL0_TRG_ENABLE;
    gptimer_counter_reset_source_config(GPTIMER0, &reset_para);
    reset_para.reset_source_evsel1 = GPTIMER_EVSEL1_TRG_ENABLE;
    reset_para.reset_source_evsel0 = GPTIMER_EVSEL0_TRG_DISABLE;
    gptimer_counter_reset_source_config(GPTIMER1, &reset_para);


    /* CH0 configuration in PWM mode */
    gptimer_channel_output_compare_value_config(GPTIMER0, GPTIMER_CH0, 250);
    gptimer_channel_output_additional_compare_value_config(GPTIMER0, GPTIMER_CH0, 250);
    gptimer_channel_output_compare_value_config(GPTIMER1, GPTIMER_CH0, 250);
    gptimer_channel_output_additional_compare_value_config(GPTIMER1, GPTIMER_CH0, 250);

    /* enable channel shadow registers */
    gptimer_channel_output_shadow_config(GPTIMER0, GPTIMER_CH0, GPTIMER_OC_SHADOW_ENABLE);
    gptimer_channel_output_additional_shadow_config(GPTIMER0, GPTIMER_CH0, GPTIMER_OC_SHADOW_ENABLE);
    gptimer_channel_output_shadow_config(GPTIMER1, GPTIMER_CH0, GPTIMER_OC_SHADOW_ENABLE);
    gptimer_channel_output_additional_shadow_config(GPTIMER1, GPTIMER_CH0, GPTIMER_OC_SHADOW_ENABLE);

    /* configure the channel output direction and state */
    gptimer_channel_io_direction_config(GPTIMER0, GPTIMER_CH0, GPTIMER_CHANNEL_OUTPUT);
    gptimer_channel_io_state_config(GPTIMER0, GPTIMER_CH0, GPTIMER_CHANNEL_ENABLE);
    gptimer_channel_io_direction_config(GPTIMER1, GPTIMER_CH0, GPTIMER_CHANNEL_OUTPUT);
    gptimer_channel_io_state_config(GPTIMER1, GPTIMER_CH0, GPTIMER_CHANNEL_ENABLE);

    /* GPTIMER enable */
    gptimer_enable(GPTIMER0);
    gptimer_enable(GPTIMER1);
}
