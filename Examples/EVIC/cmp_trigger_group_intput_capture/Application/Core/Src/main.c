/*!
    \file    main.c
    \brief   CMP trigger EVIC group intput capture

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
/* configure EVIC peripheral */
void evic_config(void);
/* configure CMP peripheral */
void cmp_config(void);
/* configure DAC peripheral */
void dac_config(void);

uint8_t data;

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
    /* configure SYSTICK peripheral */
    systick_config();
    /* initialize the com */
    gd_eval_com_init(EVAL_COM);
    /* configure GPIO peripheral */
    gpio_config();
    /* configure EVIC peripheral */
    evic_config();
    /* configure DAC peripheral */
    dac_config();
    /* configure CMP peripheral */
    cmp_config();

    while(1) {
        /* delay 100 millisecond */
        delay_1ms(100);
        /* get group holding data value */
        data = evic_data_get(EVIC_GRPIO_PORT_F);
        /* print valid data */
        printf("\r /**** GROUP F Capture Demo ****/\r\n");
        printf("the data is 0x%2X\n", data);
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
    /* enable EVIC clock */
    rcu_periph_clock_enable(RCU_EVIC);
    /* enable DAC clock */
    rcu_periph_clock_enable(RCU_DAC);
    /* enable CMP clock */
    rcu_periph_clock_enable(RCU_CMP);
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

    /* configure PF10, PF11 and PF12 as input */
    gpio_mode_set(GPIOF, GPIO_MODE_INPUT, GPIO_PUPD_PULLDOWN, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12);
}

/*!
    \brief      configure the EVIC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void evic_config(void)
{
    /* configure CMP0 OUT to trigger I/O output */
    evic_init(EVENT_INTERCONNECT_INGRPF, EVIC_SOURCE_CMP0_OUT);

    /* configure evic group member */
    evic_group_member_config(GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12, EVIC_GRPIO_PORT_F, ENABLE);
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
