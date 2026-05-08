/*!
    \file    main.c
    \brief   GPTIMER0 additional interrupt skipping

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
#include "main.h"
#include "gd32m531r_eval.h"

uint32_t ch0_count = 0;
uint32_t ch1_count = 0;

/* configure RCU peripheral */
void rcu_config(void);
/* configure GPIO peripheral */
void gpio_config(void);
/* configure GPTIMER peripheral */
void gptimer_config(void);
/* configure the NVIC */
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
    /* enable peripheral clock */
    rcu_config();
    /* initialize USART */
    gd_eval_com_init(EVAL_COM);
    /* configure NVIC peripheral */
    nvic_config();
    /* configure GPIO */
    gpio_config();
    /* configure GPTIMER */
    gptimer_config();

    while(1) {
        delay_1ms(1000);
        /* print the result */
        printf("the number of incoming interrupt of channel0: %d \r\n", ch0_count);
        printf("the number of incoming interrupt of channel1: %d \r\n", ch1_count);
        printf("**************************************\r\n");
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
    /* enable GPTIMER0 clock */
    rcu_periph_clock_enable(RCU_GPTIMER0);
    /* enable GPIOF clock */
    rcu_periph_clock_enable(RCU_GPIOF);
}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PF9 as GPTIMER0_CH1 output, PF10 as GPTIMER0_CH0 output */
    gpio_af_set(GPIOF, GPIO_AF_11, GPIO_PIN_9 | GPIO_PIN_10);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9 | GPIO_PIN_10);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_LOW, GPIO_PIN_9 | GPIO_PIN_10);
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

    gptimer_deinit(GPTIMER0);

    /* GPTIMER0 configuration */
    gptimer_struct_para_init(&gptimer_initpara);
    gptimer_initpara.clock_source      = GPTIMER_CLOCK_SOURCE_CK_GPTIMER;
    gptimer_initpara.clock_polarity    = GPTIMER_CLOCK_POLARITY_RISING;
    gptimer_initpara.prescaler         = 179;
    gptimer_initpara.counterdirection  = GPTIMER_COUNTER_DOWN;
    gptimer_initpara.alignedmode       = GPTIMER_COUNTER_CENTER;
    gptimer_initpara.period            = 11999;
    gptimer_initpara.clockdivision     = GPTIMER_CKDIV_DIV1;
    gptimer_initpara.repetitioncounter = 0;
    gptimer_init(GPTIMER0, &gptimer_initpara);

    /* GPTIMER0 output configuration */
    gptimer_channel_output_struct_para_init(&gptimer_ocpara);
    gptimer_ocpara.chxcv_up_output_level         = GPTIMER_MATCH_OUTPUT_HIGH;
    gptimer_ocpara.chxcv_down_output_level       = GPTIMER_MATCH_OUTPUT_KEEP;
    gptimer_ocpara.chxcomv_add_up_output_level   = GPTIMER_MATCH_OUTPUT_KEEP;
    gptimer_ocpara.chxcomv_add_down_output_level = GPTIMER_MATCH_OUTPUT_LOW;
    gptimer_channel_output_config(GPTIMER0,  GPTIMER_CH0, &gptimer_ocpara);
    gptimer_channel_output_config(GPTIMER0, GPTIMER_CH1, &gptimer_ocpara);

    /* configure additional interrupt skipping */
    gptimer_additional_interrupt_skipping_config(GPTIMER0, GPTIMER_ADDINT_SKIP_COUNT1, 2, 0, GPTIMER_ADDINT_SKIP_OVERFLOW);
    gptimer_additional_interrupt_skipping_time_select(GPTIMER0, GPTIMER_ADDINT_SKIP_EVENT_CH0CV, GPTIMER_ADDINT_REQ_CNT1_ZERO);

    /* CH0 and CH1 configuration in asymmetric PWM mode */
    gptimer_channel_output_compare_value_config(GPTIMER0, GPTIMER_CH0, 4000);
    gptimer_channel_output_compare_value_config(GPTIMER0, GPTIMER_CH1, 4000);
    gptimer_channel_output_additional_compare_value_config(GPTIMER0, GPTIMER_CH0, 4000);
    gptimer_channel_output_additional_compare_value_config(GPTIMER0, GPTIMER_CH1, 4000);

    /* enable channel shadow registers */
    gptimer_channel_output_shadow_config(GPTIMER0, GPTIMER_CH0, GPTIMER_OC_SHADOW_ENABLE);
    gptimer_channel_output_shadow_config(GPTIMER0, GPTIMER_CH1, GPTIMER_OC_SHADOW_ENABLE);
    gptimer_channel_output_additional_shadow_config(GPTIMER0, GPTIMER_CH0, GPTIMER_OC_SHADOW_ENABLE);
    gptimer_channel_output_additional_shadow_config(GPTIMER0, GPTIMER_CH1, GPTIMER_OC_SHADOW_ENABLE);

    /* configure the channel output direction and state */
    gptimer_channel_io_direction_config(GPTIMER0, GPTIMER_CH0, GPTIMER_CHANNEL_OUTPUT);
    gptimer_channel_io_direction_config(GPTIMER0, GPTIMER_CH1, GPTIMER_CHANNEL_OUTPUT);
    gptimer_channel_io_state_config(GPTIMER0, GPTIMER_CH0, GPTIMER_CHANNEL_ENABLE);
    gptimer_channel_io_state_config(GPTIMER0, GPTIMER_CH1, GPTIMER_CHANNEL_ENABLE);

    /* enable gptimer interrupt */
    gptimer_interrupt_flag_clear(GPTIMER0, GPTIMER_INT_FLAG_CH0);
    gptimer_interrupt_flag_clear(GPTIMER0, GPTIMER_INT_FLAG_CH1);
    gptimer_interrupt_enable(GPTIMER0, GPTIMER_INT_CH0);
    gptimer_interrupt_enable(GPTIMER0, GPTIMER_INT_CH1);

    /* GPTIMER0 enable */
    gptimer_enable(GPTIMER0);
}

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable(GPTIMER0_IRQn, 0, 0);
}
