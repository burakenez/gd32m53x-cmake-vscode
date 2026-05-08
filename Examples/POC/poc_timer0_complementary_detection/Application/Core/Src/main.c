/*!
    \file    main.c
    \brief   POC timer0 complementary detection

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
#include "gd32m531r_eval.h"

/* configure RCU peripheral */
void rcu_config(void);
/* configure GPIO peripheral */
void gpio_config(void);
/* configure POC peripheral */
void poc_config(void);
/* configure TIMER0 peripheral */
void timer0_config(void);
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
    /* initialize LED3 */
    gd_eval_led_init(LED3);
    /* configure RCU peripheral */
    rcu_config();
    /* configure NVIC peripheral */
    nvic_config();
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
    /* enable GPIOD clock */
    rcu_periph_clock_enable(RCU_GPIOD);
    /* enable POC clock */
    rcu_periph_clock_enable(RCU_POC);
    /* enable GPTIMER0 clock */
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
    /* set GPIO pin bit */
    gpio_bit_reset(GPIOD, GPIO_PIN_9 | GPIO_PIN_12);

    /*configure PD10/PD13(TIMER0 CH1/MCH1) as alternate function*/
    gpio_mode_set(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10 | GPIO_PIN_13);
    gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_10 | GPIO_PIN_13);
    gpio_af_set(GPIOD, GPIO_AF_1, GPIO_PIN_10 | GPIO_PIN_13);
    /* set GPIO pin bit */
    gpio_bit_reset(GPIOD, GPIO_PIN_10 | GPIO_PIN_13);
}

/*!
    \brief      configure POC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void poc_config(void)
{
    poc_complementary_detection_struct para;
    /* initialize POC complementary detection struct */
    poc_complementary_detection_struct_para_init(&para);

    para.polarityselen = TIMER_POLARITY_SELECTION_ENABLE;
    para.ch0polarity = CH0_POLARITY_LOW;
    para.mch0polarity = MCH0_POLARITY_LOW;
    para.ch1polarity = CH1_POLARITY_HIGH;
    para.mch1polarity = MCH1_POLARITY_HIGH;
    para.ccdreqstatus = TIMER_CCDREQ_ENABLE;
    /* configure TIMER0 complementary channel detection */
    poc_timer0_complementary_detection_config(&para);

    /* clear POC interrupt flag */
    poc_interrupt_flag_clear(POC_INT_FLAG_TIMER0_CCIF);
    /* enable POC interrupt */
    poc_interrupt_enable(POC_INT_TIMER0CC);

    /* select output disable mode for TIMER0 */
    poc_timer0_output_disable_mode_select(POC_GPIO_OUT, POC_GPIO_OUT, POC_TIMER_CHANNEL_OUT, POC_TIMER_CHANNEL_OUT);
}

/**
    \brief      configure TIMER0 peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer0_config(void)
{
    /* -----------------------------------------------------------------------
     TIMER0 configuration to:
     generate 2 complementary PWM signals with 2 different duty cycles:
     - TIMER0 frequency is fixed to 180MHz, TIMER0 prescaler is equal to 180,
        so TIMER0 counter frequency is 1MHz, the PWM frequency = 1KHz.

        the four duty cycles are computed as the following description:
        the channel 0 duty cycle is set to 20%, so multi mode channel 0 is set to 80%.
        the channel 1 duty cycle is set to 40%, so multi mode channel 1 is set to 60%.
     - CH0/MCH0,CH1/MCH1, and CH2/MCH2 are configured in PWM mode 0.
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

    /* config MCH0~3 output complementary */
    timer_multi_mode_channel_mode_config(TIMER0, TIMER_CH_0, TIMER_MCH_MODE_COMPLEMENTARY);
    timer_multi_mode_channel_mode_config(TIMER0, TIMER_CH_1, TIMER_MCH_MODE_COMPLEMENTARY);

    /* CH0 configuration in PWM mode 0, duty cycle 20% */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 200);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    /* CH1 configuration in PWM mode 0, duty cycle 40% */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_1, 400);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_1, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);

    /* BREAK configuration */
    timer_break_struct_para_init(&timer_breakpara);
    timer_breakpara.runoffstate         = TIMER_ROS_STATE_ENABLE;
    timer_breakpara.ideloffstate        = TIMER_IOS_STATE_ENABLE;
    timer_breakpara.deadtime            = 255U;
    timer_breakpara.outputautostate     = TIMER_OUTAUTO_ENABLE;
    timer_breakpara.protectmode         = TIMER_CCHP_PROT_OFF;
    timer_breakpara.breakstate          = TIMER_BREAK_DISABLE;
    timer_breakpara.breakfilter         = 0U;
    timer_breakpara.breakpolarity       = TIMER_BREAK_POLARITY_HIGH;
    timer_break_config(TIMER0, &timer_breakpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    timer_primary_output_config(TIMER0, ENABLE);

    /* auto-reload preload enable */
    timer_enable(TIMER0);
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
    nvic_irq_enable(POC_IRQn, 1, 1);
}