/*!
    \file    main.c
    \brief   ADC temperature vrefint demo

    \version 2026-03-04, V1.0.0, firmware for GD32M53x
*/

/*
    Copyright (c) 2026, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission

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
#include "main.h"
#include "gd32m531r_eval.h"

#define TEMPERATURE_TS_CAL_N40              (0x1FFFF7D8)
#define TEMPERATURE_TS_CAL2_105             (0x1FFFF7FA)

__IO float temperature;
__IO float vref_value;

/* configure the different system clocks */
void rcu_config(void);
/* configure the ADC peripheral */
void adc_config(void);

/*!
    \brief      toggle the led every 500ms
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    float V_n40 = 0;
    float V_105 = 0;

    /* peripheral clocks configuration */
    rcu_config();
    /* configure systick */
    systick_config();
    /* ADC configuration */
    adc_config();
    /* USART configuration */
    gd_eval_com_init(EVAL_COM);
    /* obtain the chip calibration value. */
    V_n40 = REG16(TEMPERATURE_TS_CAL_N40)* 5.0f / 4095;
    V_105 = REG16(TEMPERATURE_TS_CAL2_105)* 5.0f / 4095;

    while(1) {
        /* ADC software trigger enable */
        adc_group_software_trigger_enable(ADC2, ADC_GROUP_PRI1);
        /* delay a time in milliseconds */
        delay_1ms(2000);

        /* value convert */
        temperature = (adc_channel_data_read(ADC2, ADC_CHANNEL_TEMPERATURE) * 5.0f / 4095 - V_n40) * (-40-105) / (V_n40 - V_105) - 40;
        vref_value = (adc_channel_data_read(ADC2, ADC_CHANNEL_VINT) * 5.0f / 4095);

        /* value print */
        printf(" the temperature data is %5.1f degrees Celsius\r\n", temperature);
        printf(" the reference voltage data is %5.3fV \r\n", vref_value);
        printf(" \r\n");
    }
}

/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC2);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CK_ADCPRE_PCLK2, RCU_CK_ADCPRE_DIV6);
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void adc_config(void)
{
    /* reset ADC */
    adc_deinit(ADC2);
    /* configure ADC data alignment */
    adc_data_alignment_config(ADC2, ADC_DATAALIGN_RIGHT);
    /* configure ADC resolution */
    adc_resolution_config(ADC2, ADC_RESOLUTION_12B);

    /* configure the ADC scan mode */
    adc_group_scan_mode_config(ADC2, ADC_GROUP_PRI1_SCAN_ONCE);
    /* deselect ADC group channel */
    adc_group_channel_deselect(ADC2, ADC_GROUP_PRI1, ADC_CHANNEL_ALL);
    /* select ADC channel */
    adc_group_channel_config(ADC2, ADC_GROUP_PRI1, ADC_CHANNEL_TEMPERATURE, 0xFF);
    adc_group_channel_config(ADC2, ADC_GROUP_PRI1, ADC_CHANNEL_VINT, 0x12);

    /* enable ADC synchronous trigger */
    adc_group_external_trigger_disable(ADC2, ADC_GROUP_PRI1);

    /* enable ADC interface */
    adc_enable(ADC2);
}
