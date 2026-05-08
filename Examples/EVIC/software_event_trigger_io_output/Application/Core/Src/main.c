/*!
    \file    main.c
    \brief   EVIC software event trigger I/O output

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
/* configure EVIC peripheral */
void evic_config(void);


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
    /* configure GPIO peripheral */
    gpio_config();
    /* configure EVIC peripheral */
    evic_config();

    while(1) {
        /* evic software event generation */
        evic_software_event_generation();
        /* delay 100 millisecond */
        delay_1ms(100);
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
}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PF12 as output */
    gpio_mode_set(GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO_PIN_12);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_12);
}

/*!
    \brief      configure the EVIC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void evic_config(void)
{
    /* configure software event to trigger I/O output */
    evic_init(EVENT_INTERCONNECT_SGIO0, EVIC_SOURCE_SOFTWARE);

    /* configure evic single I/O */
    evic_single_io_config(EVIC_SGIO_PIN_12, EVIC_SGIO_PORT_F, SGIO0);
    /* configure evic single I/O output level */
    evic_single_io_output_level_config(EVIC_SGIO_OUTPUT_INVERTED, SGIO0);

    /* enable software event register write */
    evic_register_write_enable();
    /* enable software event generation bit write */
    evic_bit_write_enable();
}
