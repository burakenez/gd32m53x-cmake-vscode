/*!
    \file    main.c
    \brief   low voltage detector interrupt

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

/* configure LVD1 */
void lvd1_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* enable clock */
    rcu_periph_clock_enable(RCU_SYSCFG);
    exti_init(EXTI_16, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
    exti_interrupt_flag_clear(EXTI_16);
    nvic_irq_enable(LVD1_IRQn, 0, 0);

    /* configure LVD1 */
    lvd1_config();

    /* initialize LED1 */
    gd_eval_led_init(LED1);
    gd_eval_led_on(LED1);
    while(1) {
    }
}

/*!
    \brief      configure LVD1
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lvd1_config(void)
{
    /* enable clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* enable register write function */
    syscfg_register_write_enable(SYSCFG_WRITE_PROTECTION_REG2);
    /* select low voltage detector threshold */
    pmu_lvd_level_select(PMU_LVD_1, PMU_LVDT_2);
    /* low voltage detector interrupt occurs when the voltage falls below vdetx */
    pmu_lvd_mode_select(PMU_LVD_1, PMU_LVD_INTERRUPT);
    /* select the interrupt type for the LVD1 */
    pmu_lvd_interrupt_type(PMU_LVD_1, PMU_LVD_INT_MASKABLE);
    /* select the interrupt generation condition for the LVD1 */
    pmu_lvd_interrupt_select(PMU_LVD_1, PMU_LVD_INT_BOTH);
    /* enable LVD1 */
    pmu_lvd_enable(PMU_LVD_1);
    /* enable LVD1 comparison result output */
    pmu_lvd_output_enable(PMU_LVD_1);
    /* enable LVD1 interrupt */
    pmu_lvd_interrupt_reset_enable(PMU_LVD_1);
}
