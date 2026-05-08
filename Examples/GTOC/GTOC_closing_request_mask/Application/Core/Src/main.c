/*!
    \file    main.c
    \brief   GTOC0 closing request mask

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

/* configure RCU peripheral */
void rcu_config(void);
/* configure GPIO peripheral */
void gpio_config(void);
/* configure GPTIMER0 peripheral */
void gptimer0_config(void);
/* configure GPTIMER1 peripheral */
void gptimer1_config(void);
/* configure GTOC peripheral */
void gtoc_config(void);
/* configure CMP peripheral */
void cmp_config(void);
/* configure DAC peripheral */
void dac_config(void);

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
    /* configure GPIO */
    gpio_config();
    /* configure CMP peripheral */
    cmp_config();
    /* configure DAC peripheral */
    dac_config();
    /* configure GPTIMER */
    gptimer0_config();
    gptimer1_config();
    /* GPTIMER0 enable */
    gptimer_enable(GPTIMER0);
    /* GPTIMER1 enable */
    gptimer_enable(GPTIMER1);
    /* configure GTOC peripheral */
    gtoc_config();

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
    /* enable GPTIMER0 clock */
    rcu_periph_clock_enable(RCU_GPTIMER0);
    rcu_periph_clock_enable(RCU_GPTIMER1);
    /* enable GPIOF clock */
    rcu_periph_clock_enable(RCU_GPIOF);
    /* enable GTOC clock */
    rcu_periph_clock_enable(RCU_GTOC);
    /* enable CMP clock */
    rcu_periph_clock_enable(RCU_CMP);
    /* enable DAC clock */
    rcu_periph_clock_enable(RCU_DAC);
}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PC4 as CMP0 input */
    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4);

    /* configure PF9 as GPTIMER0_CH1 output, PF10 as GPTIMER0_CH0 output */
    gpio_af_set(GPIOF, GPIO_AF_11, GPIO_PIN_9 | GPIO_PIN_10);
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_9 | GPIO_PIN_10);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_LOW, GPIO_PIN_9 | GPIO_PIN_10);
}

/*!
    \brief      configure DAC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_config(void)
{
    /* initialize DAC */
    dac_deinit(DAC0);
    /* DAC trigger config */
    dac_trigger_source_config(DAC0, DAC_OUT0, DAC_TRIGGER_SOFTWARE);
    /* DAC trigger enable */
    dac_trigger_enable(DAC0, DAC_OUT0);
    /* DAC wave mode config */
    dac_wave_mode_config(DAC0, DAC_OUT0, DAC_WAVE_DISABLE);

    /* enable DAC output connect to CMP */
    dac_connect_to_cmp_enable(DAC0, DAC_OUT0);

    /* set DAC output data */
    dac_data_set(DAC0, DAC_OUT0, DAC_ALIGN_12B_R, 0x5D1);
    dac_enable(DAC0, DAC_OUT0);
    /* enable DAC software trigger */
    dac_software_trigger_enable(DAC0, DAC_OUT0);
}

/*!
    \brief      configure CMP peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cmp_config(void)
{
    /* initialize CMP0 */
    cmp_deinit(CMP0);

    /* configure CMP0 mode */
    cmp_mode_init(CMP0, CMP_INVERTING_INPUT_DAC0_OUT0, CMP_HYSTERESIS_NO);
    /* configure CMP0 non-inverting input */
    cmp_noninverting_input_select(CMP0, CMP_NONINVERTING_INPUT_PC4_PC5_PC6_PD2);
    /* configure CMP0 output */
    cmp_output_init(CMP0, CMP_OUTPUT_POLARITY_NONINVERTED);
    /* CMP digital filter init */
    cmp_digital_filter_init(CMP0, CMP_SAMPLING_FREQUENCY_DIV8, CMP_SAMPLING_NUM_3_TIMES);

    /* enable CMP output */
    cmp_output_enable(CMP0);
    /* enable CMP0 */
    cmp_enable(CMP0);
    /* delay 1ms */
    delay_1ms(1);
}

/**
    \brief      configure GPTIMER0 peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void gptimer0_config(void)
{
    /* ----------------------------------------------------------------------------------
    GPTIMER0 configuration: generate 2 PWM signals with 2 different duty cycles:

    GPTIMER0_CH0 duty cycle = ((10000+6000)/ 24000)* 100 = 66.7%
    GPTIMER0_CH1 duty cycle = ((4000+8000)/ 24000)* 100 = 50%
    ----------------------------------------------------------------------- -------------*/
    gptimer_parameter_struct gptimer_initpara;
    gptimer_oc_parameter_struct gptimer_ocpara;

    gptimer_deinit(GPTIMER0);

    /* GPTIMER0 configuration */
    gptimer_struct_para_init(&gptimer_initpara);
    gptimer_initpara.clock_source      = GPTIMER_CLOCK_SOURCE_CK_GPTIMER;
    gptimer_initpara.clock_polarity    = GPTIMER_CLOCK_POLARITY_RISING;
    gptimer_initpara.prescaler         = 0;
    gptimer_initpara.counterdirection  = GPTIMER_COUNTER_DOWN;
    gptimer_initpara.alignedmode       = GPTIMER_COUNTER_CENTER;
    gptimer_initpara.period            = 12000;
    gptimer_initpara.clockdivision     = GPTIMER_CKDIV_DIV1;
    gptimer_initpara.repetitioncounter = 0;
    gptimer_init(GPTIMER0, &gptimer_initpara);

    /* GPTIMER0 output configuration */
    gptimer_channel_output_struct_para_init(&gptimer_ocpara);
    gptimer_ocpara.chxcv_up_output_level         = GPTIMER_MATCH_OUTPUT_HIGH;
    gptimer_ocpara.chxcv_down_output_level       = GPTIMER_MATCH_OUTPUT_LOW;
    gptimer_ocpara.chxcomv_add_up_output_level   = GPTIMER_MATCH_OUTPUT_HIGH;
    gptimer_ocpara.chxcomv_add_down_output_level = GPTIMER_MATCH_OUTPUT_LOW;
    gptimer_ocpara.output_stop_output_level = GPTIMER_OUTPUT_STOP_OUTPUT_LOW;
    gptimer_channel_output_config(GPTIMER0, GPTIMER_CH1, &gptimer_ocpara);
    gptimer_channel_output_config(GPTIMER0, GPTIMER_CH0, &gptimer_ocpara);

    /* CH0 and CH1 configuration in asymmetric PWM mode */
    gptimer_channel_output_compare_value_config(GPTIMER0, GPTIMER_CH0, 2000);
    gptimer_channel_output_compare_value_config(GPTIMER0, GPTIMER_CH1, 4000);
    gptimer_channel_output_additional_compare_value_config(GPTIMER0, GPTIMER_CH0, 6000);
    gptimer_channel_output_additional_compare_value_config(GPTIMER0, GPTIMER_CH1, 8000);

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

    /* configure stop output source */
    gptimer_stop_output_set_select(GPTIMER0, GPTIMER_OUTPUT_STOP_GTOC0);
}

/**
    \brief      configure GPTIMER1 peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void gptimer1_config(void)
{
    gptimer_parameter_struct gptimer_initpara;
    gptimer_oc_parameter_struct gptimer_ocpara;

    gptimer_deinit(GPTIMER1);

    /* GPTIMER1 configuration */
    gptimer_struct_para_init(&gptimer_initpara);
    gptimer_initpara.clock_source      = GPTIMER_CLOCK_SOURCE_CK_GPTIMER;
    gptimer_initpara.clock_polarity    = GPTIMER_CLOCK_POLARITY_RISING;
    gptimer_initpara.prescaler         = 31;
    gptimer_initpara.counterdirection  = GPTIMER_COUNTER_DOWN;
    gptimer_initpara.alignedmode       = GPTIMER_COUNTER_CENTER;
    gptimer_initpara.period            = 12000;
    gptimer_initpara.clockdivision     = GPTIMER_CKDIV_DIV1;
    gptimer_initpara.repetitioncounter = 0;
    gptimer_init(GPTIMER1, &gptimer_initpara);

    /* GPTIMER0 output configuration */
    gptimer_channel_output_struct_para_init(&gptimer_ocpara);
    gptimer_ocpara.chxcv_up_output_level         = GPTIMER_MATCH_OUTPUT_HIGH;
    gptimer_ocpara.chxcv_down_output_level       = GPTIMER_MATCH_OUTPUT_LOW;
    gptimer_ocpara.chxcomv_add_up_output_level   = GPTIMER_MATCH_OUTPUT_HIGH;
    gptimer_ocpara.chxcomv_add_down_output_level = GPTIMER_MATCH_OUTPUT_LOW;
    gptimer_channel_output_config(GPTIMER1, GPTIMER_CH1, &gptimer_ocpara);

    /* CH1 configuration in asymmetric PWM mode */
    gptimer_channel_output_compare_value_config(GPTIMER1, GPTIMER_CH1, 10000);
    gptimer_channel_output_additional_compare_value_config(GPTIMER1, GPTIMER_CH1, 10000);

    /* enable channel shadow registers */
    gptimer_channel_output_shadow_config(GPTIMER1, GPTIMER_CH1, GPTIMER_OC_SHADOW_ENABLE);
    gptimer_channel_output_additional_shadow_config(GPTIMER1, GPTIMER_CH1, GPTIMER_OC_SHADOW_ENABLE);

    /* configure the channel output direction and state */
    gptimer_channel_io_direction_config(GPTIMER1, GPTIMER_CH1, GPTIMER_CHANNEL_OUTPUT);
    gptimer_channel_io_state_config(GPTIMER1, GPTIMER_CH1, GPTIMER_CHANNEL_ENABLE);
}

/*!
    \brief      configure GTOC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gtoc_config(void)
{
    /* mask GTOC extended closing request */
    gtoc_extended_closing_request_mask(GTOC0, GTOC_ECRMKSEL_GPTIMER1_CH1);
    /* configure GTOC extended closing request */
    gtoc_extended_closing_request_config(GTOC0, GTOC_ECR_SOURCE_CMP0, GTOC_VALID_LEVEL_HIGH);
    /* enable GTOC extended closing request */
    gtoc_extended_closing_request_enable(GTOC0);
}
