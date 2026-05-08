/*!
    \file    main.c
    \brief   CPTIMER event count

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
#include "gd32m531r_eval.h"

uint16_t cnt_val;

/* configure the GPIO peripheral */
void gpio_config(void);
/* configure the EVIC peripheral */
void evic_config(void);
/* configure the CPTIMER peripheral */
void cptimer_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure GPIO */
    gpio_config();

    /* configure evic */
    evic_config();

    /* configure cptimer */
    cptimer_config();

    while(1) {
        cnt_val = cptimer_counter_read(CPTIMER0, CPTIMER_COUNTER0);
    }
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* enable GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOE);

    /* configure PE8 as input */
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_8);
}

/*!
    \brief      configure the EVIC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void evic_config(void)
{
    /* enable EVIC clock */
    rcu_periph_clock_enable(RCU_EVIC);

    /* set the event source for CPTIMER0 as GPIO input */
    evic_init(EVENT_INTERCONNECT_CPTIMER0, EVIC_SOURCE_SGIO_INPUT_DETECTION0);
    /* configure evic single I/O as PE8 */
    evic_single_io_config(EVIC_SGIO_PIN_8, EVIC_SGIO_PORT_E, SGIO0);
    /* configure evic single I/O input detection falling edge */
    evic_single_io_edge_detection_config(EVIC_SGIO_DETECTION_FALLING, SGIO0);
    /* select CPTIMER0 slave mode as event count */
    evic_cptimer_slave_mode_select(TARGET_CPTIMER0, EVIC_EVENT_COUNT);
}

/*!
    \brief      configure the CPTIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void cptimer_config(void)
{
    /* enable CPTIMER0 clock */
    rcu_periph_clock_enable(RCU_CPTIMER0);

    /* deinit a CPTIMER0 */
    cptimer_deinit(CPTIMER0);

    /* configure CPTIMER0 counter0 prescaler and period */
    cptimer_prescaler_config(CPTIMER0, CPTIMER_COUNTER0, 0U);
    cptimer_autoreload_value_config(CPTIMER0, CPTIMER_COUNTER0, 0xF);

    /* enable CPTIMER0 counter0 */
    cptimer_enable(CPTIMER0, CPTIMER_COUNTER0);
}
