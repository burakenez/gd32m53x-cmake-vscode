/*!
    \file    main.c
    \brief   comparator obtain brightness example

    \version 2026-03-04, V1.0.0, demo for GD32M53x
*/

/*
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
#include "gd32m531r_eval.h"
#include "systick.h"

/* configure RCU peripheral */
void rcu_config(void);
/* configure GPIO peripheral */
void gpio_config(void);
/* configure CMP peripheral */
void cmp_config(void);
/* configure EXTI peripheral */
void exti_config(void);
/* configure DAC peripheral */
void dac_config(void);
/* configure NVIC peripheral */
void nvic_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure systick */
    systick_config();

    /* initialize LED2 */
    gd_eval_led_init(LED2);

    /* configure RCU peripheral */
    rcu_config();
    /* configure NVIC peripheral */
    nvic_config();
    /* configure GPIO peripheral */
    gpio_config();
    /* configure DAC peripheral */
    dac_config();
    /* configure EXTI peripheral */
    exti_config();
    /* configure CMP peripheral */
    cmp_config();

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
    rcu_periph_clock_enable(RCU_GPIOC);
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
    /* configure PC1 as CMP1 input */
    gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1);
}

/*!
    \brief      configure NVIC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void nvic_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    nvic_irq_enable(CMP1_EXTI_IRQn, 1, 1);
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
    /* initialize CMP1 */
    cmp_deinit(CMP1);

    /* configure CMP1 mode */
    cmp_mode_init(CMP1, CMP_INVERTING_INPUT_DAC0_OUT0, CMP_HYSTERESIS_NO);
    /* configure CMP1 non-inverting input */
    cmp_noninverting_input_select(CMP1, CMP_NONINVERTING_INPUT_PC0_PC1_PC2_PC3);
    /* configure CMP1 output */
    cmp_output_init(CMP1, CMP_OUTPUT_POLARITY_NONINVERTED);
    /* CMP digital filter init */
    cmp_digital_filter_init(CMP1, CMP_SAMPLING_FREQUENCY_DIV8, CMP_SAMPLING_NUM_3_TIMES);

    /* enable CMP output */
    cmp_output_enable(CMP1);
    /* enable CMP0 */
    cmp_enable(CMP1);
    /* delay 1ms */
    delay_1ms(1);
}

/*!
    \brief      configure EXTI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void exti_config(void)
{
    /* clear exti line19 interrupt flag */
    exti_flag_clear(EXTI_19);
    /* initialize exti line19 */
    exti_init(EXTI_19, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    /* enable exti line19 interrupt */
    exti_interrupt_enable(EXTI_19);
}
