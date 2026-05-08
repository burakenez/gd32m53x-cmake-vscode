/*!
    \file    main.c
    \brief   DAC ADC convert demo

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
/* configure ADC peripheral */
void adc_config(void);
/* configure DAC peripheral */
void dac_config(void);
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
    /* configure DAC peripheral */
    dac_config();
    /* configure ADC peripheral */
    adc_config();

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
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable DAC clock */
    rcu_periph_clock_enable(RCU_DAC);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);

}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PC0 as ADC input */
    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);

    /* configure PD4 as DAC output */
    gpio_mode_set(GPIOD, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4);
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
    nvic_irq_enable(ADC0_IRQn, 0, 0);
}

/*!
    \brief      configure ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* reset ADC */
    adc_deinit(ADC0);
    /* configure ADC data alignment */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* configure ADC resolution */
    adc_resolution_config(ADC0, ADC_RESOLUTION_12B);

    /* configure the ADC scan mode */
    adc_group_scan_mode_config(ADC0, ADC_GROUP_PRI1_SCAN_CONTINUE);
    /* deselect ADC group channel */
    adc_group_channel_deselect(ADC0, ADC_GROUP_PRI1, ADC_CHANNEL_ALL);
    /* select ADC channel */
    adc_group_channel_config(ADC0, ADC_GROUP_PRI1, ADC_CHANNEL_IN0, 0x12);

    /* config end of Group_prix conversion round counts flag */
    adc_group_end_flag_round_config(ADC0, ADC_GROUP_PRI1, 0);
    /* clear the ADC flag */
    adc_flag_clear(ADC0, ADC_FLAG_EOC1RF);
    /* enable ADC interrupt */
    adc_interrupt_enable(ADC0, ADC_INT_EOC1RF);

    /* enable ADC interface */
    adc_enable(ADC0);
    /* enable ADC software trigger */
    adc_group_software_trigger_enable(ADC0, ADC_GROUP_PRI1);
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

    /* enable DAC output connect to pin */
    dac_connect_to_pin_enable(DAC0, DAC_OUT0);

    /* set DAC output data */
    dac_enable(DAC0, DAC_OUT0);
}
