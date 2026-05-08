/*!
    \file    main.c
    \brief   scan once basic operation example

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
#include "main.h"
#include "gd32m531r_eval.h"

extern uint16_t result_ch0;
extern uint16_t result_ch1;
extern uint16_t result_ch2;
extern uint16_t result_ch4;

/* configure the system clocks */
void rcu_config(void);
/* GPIO configuration */
void gpio_config(void);
/* TIMER configuration */
void timer_config(void);
/* ADC configuration */
void adc_config(void);
/* configure the nested vectored interrupt controller */
void nvic_configuration(void);

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
    /* enable TIMER clock */
    rcu_periph_clock_enable(RCU_TIMER0);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CK_ADCPRE_PCLK2, RCU_CK_ADCPRE_DIV6);
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
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    timer_parameter_struct timer_initpara;

    /* TIMER configuration */
    timer_initpara.prescaler         = 8399;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 9999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    timer_trigger_adc_flow_enable(TIMER0, TIMER_ADCTL_ADTSOF);
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
    adc_group_scan_mode_config(ADC0, ADC_GROUP_PRI1_SCAN_ONCE);
    /* deselect ADC group channel */
    adc_group_channel_deselect(ADC0, ADC_GROUP_PRI1, ADC_CHANNEL_ALL);
    /* select ADC channel */
    adc_group_channel_config(ADC0, ADC_GROUP_PRI1, ADC_CHANNEL_IN0, 0x12);
    adc_group_channel_config(ADC0, ADC_GROUP_PRI1, ADC_CHANNEL_IN1, 0x12);
    adc_group_channel_config(ADC0, ADC_GROUP_PRI1, ADC_CHANNEL_IN2, 0x12);
    adc_group_channel_config(ADC0, ADC_GROUP_PRI1, ADC_CHANNEL_IN4, 0x12);

    /* enable ADC synchronous trigger */
    adc_group_external_trigger_enable(ADC0, ADC_GROUP_PRI1);
    adc_group_extern_trigger_edge_config(ADC0, ADC_GROUP_PRI1, ADC_EXTERNAL_TRIG_RISING_EDGE);
    adc_group_synchronous_trigger_enable(ADC0, ADC_GROUP_PRI1);
    /* configure ADC external trigger source */
    adc_group_synchronous_trigger_source_config(ADC0, ADC_GROUP_PRI1, ADC_SYNCTRIG_SOURCE_TIMER0_TRGOF);

    /* config end of Group_prix conversion round counts flag */
    adc_group_end_flag_round_config(ADC0, ADC_GROUP_PRI1, 0);
    /* clear the ADC flag */
    adc_flag_clear(ADC0, ADC_FLAG_EOC1RF);
    /* enable ADC interrupt */
    adc_interrupt_enable(ADC0, ADC_INT_EOC1RF);

    /* enable ADC interface */
    adc_enable(ADC0);
}

/*!
    \brief      configure the nested vectored interrupt controller
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_configuration(void)
{
    nvic_irq_enable(ADC0_IRQn, 0, 0);
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
    /* GPIO configuration */
    gpio_config();
    /* TIMER configuration */
    timer_config();
    /* ADC configuration */
    adc_config();
    /* configure the nested vectored interrupt controller */
    nvic_configuration();

    /* initialize the LEDs, USART */
    gd_eval_led_init(LED1);
    gd_eval_com_init(EVAL_COM);

    /* enable a TIMER */
    timer_enable(TIMER0);

    while(1) {
        gd_eval_led_on(LED1);
        delay_1ms(500);
        gd_eval_led_off(LED1);
        delay_1ms(500);
        /* print the result */
        printf("the result of ADC0_IN0 is %d \r\n", result_ch0);
        printf("the result of ADC0_IN1 is %d \r\n", result_ch1);
        printf("the result of ADC0_IN2 is %d \r\n", result_ch2);
        printf("the result of ADC0_IN4 is %d \r\n", result_ch4);
        printf("*******************************\r\n");
    }
}
