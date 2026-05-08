/*!
    \file    main.c
    \brief   DAC concurrent mode output voltage demo

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
    /* configure RCU peripheral */
    rcu_config();
    /* configure GPIO peripheral */
    gpio_config();
    /* configure EVIC peripheral */
    evic_config();
    /* configure DAC peripheral */
    dac_config();

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
    /* enable DAC clock */
    rcu_periph_clock_enable(RCU_DAC);
    /* enable EVIC clock */
    rcu_periph_clock_enable(RCU_EVIC);
}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PD4 and PD5 as DAC output */
    gpio_mode_set(GPIOD, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5);

    /* configure PE12 as input */
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_PIN_12);
}

/*!
    \brief      configure the EVIC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void evic_config(void)
{
    /* configure single I/O input detection 0 as DAC trigger source */
    evic_init(EVENT_INTERCONNECT_DAC0_OUT0_EN, EVIC_SOURCE_SGIO_INPUT_DETECTION0);
    evic_init(EVENT_INTERCONNECT_DAC0_OUT1_EN, EVIC_SOURCE_SGIO_INPUT_DETECTION0);

    /* configure evic single I/O */
    evic_single_io_config(EVIC_SGIO_PIN_12, EVIC_SGIO_PORT_E, SGIO0);
    /* configure evic single I/O input detection edge */
    evic_single_io_edge_detection_config(EVIC_SGIO_DETECTION_RISING, SGIO0);
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
    /* DAC trigger disable */
    dac_trigger_disable(DAC0, DAC_OUT0);
    dac_trigger_disable(DAC0, DAC_OUT1);
    /* DAC wave mode config */
    dac_wave_mode_config(DAC0, DAC_OUT0, DAC_WAVE_DISABLE);
    dac_wave_mode_config(DAC0, DAC_OUT1, DAC_WAVE_DISABLE);

    /* enable DAC output connect to pin */
    dac_connect_to_pin_enable(DAC0, DAC_OUT0);
    dac_connect_to_pin_enable(DAC0, DAC_OUT1);

    /* set DAC output data */
    dac_concurrent_data_set(DAC0, DAC_ALIGN_12B_R, 0x7F0, 0x1F0);
}
