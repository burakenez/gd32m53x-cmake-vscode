/*!
    \file    main.c
    \brief   obtain conversion results through DMA in scan continue mode example

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
#include <stdio.h>

#define ADC0_GP1DMAR_ADDRESS      (uint32_t)&ADC_GP1DMAR(ADC0)

uint16_t result_buffer[4] = {0};

/* configure the system clocks */
void rcu_config(void);
/* GPIO configuration */
void gpio_config(void);
/* configure DMA and DMAMUX channel */
void dma_config(void);
/* ADC configuration */
void adc_config(void);

/*!
    \brief      configure the system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CK_ADCPRE_PCLK2, RCU_CK_ADCPRE_DIV6);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_DMAMUX);
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
    rcu_periph_clock_enable(RCU_GPIOC);

    /* config the GPIO as analog mode */
    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4);
}

/*!
    \brief      configure the DMA channel
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_init_struct;

    /* initialize DMA channel 0 */
    dma_deinit(DMA0, DMA_CH0);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.request      = DMA_REQUEST_ADC0_A;
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr  = (uint32_t)result_buffer;
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_init_struct.number       = 4;
    dma_init_struct.periph_addr  = ADC0_GP1DMAR_ADDRESS;
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_init_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_init_struct);

    /* configure DMA mode */
    dma_circulation_enable(DMA0, DMA_CH0);
    dma_memory_to_memory_disable(DMA0, DMA_CH0);
    /* disable the DMAMUX_MUXCH0 synchronization mode */
    dmamux_synchronization_disable(DMAMUX_MUXCH0);

    /* enable DMA channel 0 */
    dma_channel_enable(DMA0, DMA_CH0);
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
    adc_group_channel_config(ADC0, ADC_GROUP_PRI1, ADC_CHANNEL_IN1, 0x12);
    adc_group_channel_config(ADC0, ADC_GROUP_PRI1, ADC_CHANNEL_IN2, 0x12);
    adc_group_channel_config(ADC0, ADC_GROUP_PRI1, ADC_CHANNEL_IN4, 0x12);

    /* configure DMA function */
    adc_group_dma_mode_enable(ADC0, ADC_GROUP_PRI1);
    adc_group_dma_request_after_last_enable(ADC0, ADC_GROUP_PRI1);

    /* enable ADC interface */
    adc_enable(ADC0);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    /* system clocks configuration */
    rcu_config();
    /* configure systick */
    systick_config();
    /* configure the DMA channel */
    dma_config();
    /* GPIO configuration */
    gpio_config();
    /* ADC configuration */
    adc_config();

    /* initialize the LEDs, USART */
    gd_eval_led_init(LED1);
    gd_eval_com_init(EVAL_COM);

    /* enable ADC software trigger */
    adc_group_software_trigger_enable(ADC0, ADC_GROUP_PRI1);

    while(1) {
        gd_eval_led_on(LED1);
        delay_1ms(500);
        gd_eval_led_off(LED1);
        delay_1ms(500);
        /* print the result */
        printf("the result of ADC0_IN0 is %d \r\n", result_buffer[0]);
        printf("the result of ADC0_IN1 is %d \r\n", result_buffer[1]);
        printf("the result of ADC0_IN2 is %d \r\n", result_buffer[2]);
        printf("the result of ADC0_IN4 is %d \r\n", result_buffer[3]);
        printf("**************************************\r\n");
    }
}
