/*!
    \file    main.c
    \brief   UART transmit and receive interrupt

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

#include "gd32m53x_it.h"
#include "gd32m531r_eval.h"

#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE            (ARRAYNUM(transmitter_buffer) - 1)

uint8_t transmitter_buffer[] = "\n\rUART interrupt test\n\r";
uint8_t receiver_buffer[32];
uint8_t transfersize = TRANSMIT_SIZE;
uint8_t receivesize = 32;
__IO uint8_t txcount = 0;
__IO uint16_t rxcount = 0;

void com_uart_init(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* UART interrupt configuration */
    nvic_irq_enable(UART3_IRQn, 0, 0);

    /* initialize the com */
    com_uart_init();

    /* enable UART TBE interrupt */
    uart_interrupt_enable(UART3, UART_INT_TBE);

    /* wait until UART send the transmitter_buffer */
    while(txcount < transfersize) {
    }

    while(RESET == uart_flag_get(UART3, UART_FLAG_TC)) {
    }

    uart_interrupt_enable(UART3, UART_INT_RBNE);

    /* wait until UART receive the receiver_buffer */
    while(rxcount < receivesize) {
    }
    if(rxcount == receivesize) {
        printf("\n\rUART receive successfully!\n\r");
    }

    while(1) {
    }
}

/*!
    \brief      initialize the UART configuration of the com
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com_uart_init(void)
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOF);
    /* enable UART clock */
    rcu_periph_clock_enable(RCU_UART3);

    /* connect port to UART TX */
    gpio_af_set(GPIOF, GPIO_AF_7, GPIO_PIN_13);
    /* connect port to UART RX */
    gpio_af_set(GPIOF, GPIO_AF_7, GPIO_PIN_14);

    /* configure UART TX as alternate function push-pull */
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_13);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_13);

    /* configure UART RX as alternate function push-pull */
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_14);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_14);

    /* UART configure */
    uart_deinit(UART3);
    uart_word_length_set(UART3, UART_WL_8BIT);
    uart_stop_bit_set(UART3, UART_STB_1BIT);
    uart_parity_config(UART3, UART_PM_NONE);
    uart_baudrate_set(UART3, 115200U);
    uart_receive_config(UART3, UART_RECEIVE_ENABLE);
    uart_transmit_config(UART3, UART_TRANSMIT_ENABLE);

    uart_enable(UART3);
}
