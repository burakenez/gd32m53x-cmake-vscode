/*!
    \file    main.c
    \brief   UART DMA receive by IDLE interrupt

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

#define UART3_DATA_ADDRESS      ((uint32_t)&UART_DATA(UART3))

uint8_t rxbuffer[256];
uint8_t rx_count = 0;
uint8_t tx_count = 0;
__IO uint8_t receive_flag = 0;

void dma_config(void);
void uart_config(void);
void nvic_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_on(LED1);
    nvic_config();

    /* initialize DMA */
    dma_config();

    /* initialize UART */
    uart_config();

    /*wait IDLEF set and clear it*/
    while(RESET == uart_flag_get(UART3, UART_FLAG_IDLE)) {
    }
    uart_flag_clear(UART3, UART_FLAG_IDLE);
    uart_interrupt_enable(UART3, UART_INT_IDLE);

    while(1) {
        if(1 == receive_flag) {
            for(tx_count = 0; tx_count < rx_count; tx_count++) {
                while(RESET == uart_flag_get(UART3, UART_FLAG_TBE)) {
                }
                uart_data_transmit(UART3, rxbuffer[tx_count]);
            }
            receive_flag = 0;
        }
    }
}

/*!
    \brief      configure UART DMA
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_init_struct;
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMAMUX);

    /* initialize DMA channel 1 */
    dma_deinit(DMA0, DMA_CH1);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.request      = DMA_REQUEST_UART3_RX;
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr  = (uint32_t)rxbuffer;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number       = 256;
    dma_init_struct.periph_addr  = (uint32_t)UART3_DATA_ADDRESS;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH1, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH1);
    dma_memory_to_memory_disable(DMA0, DMA_CH1);
    /* disable the DMAMUX_MUXCH1 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH1);

    /* enable DMA channel 1 */
    dma_channel_enable(DMA0, DMA_CH1);
}

/*!
    \brief      configure UART
    \param[in]  none
    \param[out] none
    \retval     none
*/
void uart_config(void)
{
    /* enable GPIO clock */
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
    uart_baudrate_set(UART3, 115200U);
    uart_receive_config(UART3, UART_RECEIVE_ENABLE);
    uart_transmit_config(UART3, UART_TRANSMIT_ENABLE);
    uart_dma_receive_config(UART3, UART_RECEIVE_DMA_ENABLE);
    uart_dma_transmit_config(UART3, UART_TRANSMIT_DMA_ENABLE);
    uart_enable(UART3);
}

/*!
    \brief      configure NVIC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_irq_enable(UART3_IRQn, 0, 0);
}
