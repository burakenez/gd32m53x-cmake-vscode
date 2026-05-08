/*!
    \file    main.c
    \brief   UART HalfDuplex transmitter and receiver

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
#include <stdio.h>
#include "gd32m531r_eval.h"

#define ARRAYNUM(arr_nanme)      (uint32_t)(sizeof(arr_nanme) / sizeof(*(arr_nanme)))
#define TRANSMIT_SIZE0           (ARRAYNUM(transmitter_buffer0) - 1)
#define TRANSMIT_SIZE1           (ARRAYNUM(transmitter_buffer1) - 1)

uint8_t transmitter_buffer0[] = "\n\ra uart half-duplex test example!\n\r";
uint8_t transmitter_buffer1[] = "\n\ra uart half-duplex test example!\n\r";
uint8_t receiver_buffer0[TRANSMIT_SIZE1];
uint8_t receiver_buffer1[TRANSMIT_SIZE0];
uint8_t transfersize0 = TRANSMIT_SIZE0;
uint8_t transfersize1 = TRANSMIT_SIZE1;
__IO uint8_t txcount0 = 0;
__IO uint16_t rxcount0 = 0;
__IO uint8_t txcount1 = 0;
__IO uint16_t rxcount1 = 0;
ErrStatus state1 = ERROR;
ErrStatus state2 = ERROR;

ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length);
void com_uart_init(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    com_uart_init();

    /* configure UART transmitter and receiver */
    uart_transmit_config(UART2, UART_TRANSMIT_ENABLE);
    uart_receive_config(UART2, UART_RECEIVE_DISABLE);
    uart_transmit_config(UART3, UART_TRANSMIT_DISABLE);
    uart_receive_config(UART3, UART_RECEIVE_ENABLE);

    /* UART2 transmit and UART3 receive*/
    uart_data_receive(UART3);
    while(transfersize0--) {
        /* wait until end of transmit */
        while(RESET == uart_flag_get(UART2, UART_FLAG_TBE)) {
        }
        uart_data_transmit(UART2, transmitter_buffer0[txcount0++]);

        while(RESET == uart_flag_get(UART3, UART_FLAG_RBNE)) {
        }
        /* store the received byte in the receiver_buffer1 */
        receiver_buffer1[rxcount0++] = uart_data_receive(UART3);
    }

    /* configure UART transmitter and receiver */
    uart_transmit_config(UART2, UART_TRANSMIT_DISABLE);
    uart_receive_config(UART2, UART_RECEIVE_ENABLE);
    uart_transmit_config(UART3, UART_TRANSMIT_ENABLE);
    uart_receive_config(UART3, UART_RECEIVE_DISABLE);

    uart_data_receive(UART2);
    /* UART3 transmit and UART2 receive*/
    while(transfersize1--) {
        /* wait until end of transmit */
        while(RESET == uart_flag_get(UART3, UART_FLAG_TBE)) {
        }
        uart_data_transmit(UART3, transmitter_buffer1[txcount1++]);

        while(RESET == uart_flag_get(UART2, UART_FLAG_RBNE)) {
        }
        /* store the received byte in the receiver_buffer0 */
        receiver_buffer0[rxcount1++] = uart_data_receive(UART2);
    }

    /* compare the received data with the send ones */
    state1 = memory_compare(transmitter_buffer0, receiver_buffer1, TRANSMIT_SIZE0);
    state2 = memory_compare(transmitter_buffer1, receiver_buffer0, TRANSMIT_SIZE1);
    if(SUCCESS == state1) {
        /* if the data transmitted from UART2 and received by UART3 are the same */
        gd_eval_led_on(LED1);
    } else {
        /* if the data transmitted from UART2 and received by UART3 are not the same */
        gd_eval_led_off(LED1);
    }
    if(SUCCESS == state2) {
        /* if the data transmitted from UART3 and received by UART2 are the same */
        gd_eval_led_on(LED2);
    } else {
        /* if the data transmitted from UART3 and received by UART2 are not the same */
        gd_eval_led_off(LED2);
    }
    while(1) {
    }
}

/*!
    \brief      configure UART
    \param[in]  none
    \param[out] none
    \retval     none
*/
void com_uart_init(void)
{
    /* enable UART and GPIOF clock */
    rcu_periph_clock_enable(RCU_GPIOF);
    rcu_periph_clock_enable(RCU_UART2);
    rcu_periph_clock_enable(RCU_UART3);

    /* configure the UART2 TX pin and UART3 TX pin */
    gpio_af_set(GPIOF, GPIO_AF_6, GPIO_PIN_8);
    gpio_af_set(GPIOF, GPIO_AF_7, GPIO_PIN_11);
    /* configure UART2 TX as alternate function open drain */
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_OD, GPIO_OSPEED_HIGH, GPIO_PIN_8);
    /* configure UART3 TX as alternate function open drain */
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_11);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_OD, GPIO_OSPEED_HIGH, GPIO_PIN_11);

    /* UART2 and UART3 baudrate configuration */
    uart_baudrate_set(UART2, 115200);
    uart_baudrate_set(UART3, 115200);

    /* enable UART2 half duplex Mode */
    uart_halfduplex_enable(UART2);
    /* enable UART3 half duplex Mode */
    uart_halfduplex_enable(UART3);

    /* enable UART */
    uart_enable(UART2);
    uart_enable(UART3);
}

/*!
    \brief      memory compare function
    \param[in]  src : source data
    \param[in]  dst : destination data
    \param[in]  length : the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t *src, uint8_t *dst, uint16_t length)
{
    while(length--) {
        if(*src++ != *dst++) {
            return ERROR;
        }
    }
    return SUCCESS;
}
