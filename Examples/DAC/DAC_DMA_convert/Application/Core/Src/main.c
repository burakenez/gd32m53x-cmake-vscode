/*!
    \file    main.c
    \brief   DAC DMA convert demo

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

#define CONVERT_NUM          (10)
#define DAC0_R12DH_ADDRESS    (0x40007408)
const uint32_t convertarr[CONVERT_NUM] = {0x00, 0x333, 0x666, 0x999, 0xCCC, 0xFFF, 0xCCC, 0x999, 0x666, 0x333};

/* configure RCU peripheral */
void rcu_config(void);
/* configure GPIO peripheral */
void gpio_config(void);
/* configure EVIC peripheral */
void evic_config(void);
/* configure DMA peripheral */
void dma_config(void);
/* configure DAC peripheral */
void dac_config(void);
/* configure TIMER peripheral */
void timer1_config(void);

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
    /* configure DMA peripheral */
    dma_config();
    /* configure DAC peripheral */
    dac_config();
    /* configure TIMER peripheral */
    timer1_config();

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
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA0);
    /* enable DAC clock */
    rcu_periph_clock_enable(RCU_DAC);
    /* enable TIMER clock */
    rcu_periph_clock_enable(RCU_TIMER1);
    /* enable EVIC clock */
    rcu_periph_clock_enable(RCU_EVIC);
    /* enable DMAMUX clock */
    rcu_periph_clock_enable(RCU_DMAMUX);
}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PD4 as DAC output */
    gpio_mode_set(GPIOD, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4);
}

/*!
    \brief      configure DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_struct;
    /* clear all the interrupt flags */
    dma_flag_clear(DMA0, DMA_CH4, DMA_INTF_GIF);
    dma_flag_clear(DMA0, DMA_CH4, DMA_INTF_FTFIF);
    dma_flag_clear(DMA0, DMA_CH4, DMA_INTF_HTFIF);
    dma_flag_clear(DMA0, DMA_CH4, DMA_INTF_ERRIF);

    /* configure the DMA0 channel 4 */
    dma_struct.periph_addr  = DAC0_R12DH_ADDRESS;
    dma_struct.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
    dma_struct.memory_addr  = (uint32_t)convertarr;
    dma_struct.memory_width = DMA_MEMORY_WIDTH_32BIT;
    dma_struct.number       = CONVERT_NUM;
    dma_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_struct.request      = DMA_REQUEST_TIMER1_UP;
    dma_init(DMA0, DMA_CH4, &dma_struct);

    dma_circulation_enable(DMA0, DMA_CH4);
    dma_channel_enable(DMA0, DMA_CH4);
}

/*!
    \brief      configure the EVIC
    \param[in]  none
    \param[out] none
    \retval     none
*/
void evic_config(void)
{
    evic_init(EVENT_INTERCONNECT_DAC0_OUT0_COV, EVIC_SOURCE_TIMER1_TRGO);
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
    dac_trigger_source_config(DAC0, DAC_OUT0, DAC_TRIGGER_EXTERNAL);
    /* DAC trigger enable */
    dac_trigger_enable(DAC0, DAC_OUT0);
    /* DAC wave mode config */
    dac_wave_mode_config(DAC0, DAC_OUT0, DAC_WAVE_DISABLE);
    /* enable DAC output connect to pin */
    dac_connect_to_pin_enable(DAC0, DAC_OUT0);

    /* DAC enable */
    dac_enable(DAC0, DAC_OUT0);
}

/*!
    \brief      configure TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer1_config(void)
{
    timer_parameter_struct timer_initpara;

    /* TIMER deinitialize */
    timer_deinit(TIMER1);

    /* TIMER configuration */
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 179;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;

    /* initialize TIMER init parameter struct */
    timer_init(TIMER1, &timer_initpara);

    /* enable the TIMER DMA */
    timer_dma_enable(TIMER1, TIMER_DMA_UPD);

    /* TIMER master mode output trigger source: Update event */
    timer_master_output_trigger_source_select(TIMER1, TIMER_TRI_OUT_SRC_UPDATE);

    /* enable TIMER */
    timer_enable(TIMER1);
}
