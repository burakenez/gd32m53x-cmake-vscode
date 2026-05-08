/*!
    \file    main.c
    \brief   led spark with systick, UART print and key example

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

typedef enum {
    CKOUT0_SYSTEMCLOCK_CKOUT = 0U,
    CKOUT0_PLL_DIV8_CKOUT,
    CKOUT0_HXTAL_CKOUT,
    CKOUT0_IRC32M_CKOUT,
    CKOUT0_IRC32K_CKOUT,
    CKOUT0_CKOUT_MAX,
} clock_output_enum;

void rcu_config(void);
void gpio_config(void);
void all_led_init(void);
void all_led_off(void);
void clock_output_config(void);
void clock_output_select(uint8_t seq);

extern __IO uint8_t g_button_press_flag;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    uint8_t func_seq = 0;

    /* initialize the USART */
    gd_eval_com_init(EVAL_COM);

    printf("\r\n /=========== Gigadevice Clock output Demo ===========/ \r\n");
    printf("press USER key to select clock output source \r\n");

    /* initialize the LEDs */
    all_led_init();
    /* initialize the clock output */
    clock_output_config();
    /* initialize the USER key */
    gd_eval_key_init(KEY_USER, KEY_MODE_EXTI);

    while(1) {
        if(1 == g_button_press_flag) {
            /* if the button is pressed */
            g_button_press_flag = 0;
            /* control the led */
            all_led_off();
            gd_eval_led_on((led_typedef_enum)(func_seq % 4));
            /* select the clock output mode */
            clock_output_select(func_seq);

            func_seq++;
            func_seq %= CKOUT0_CKOUT_MAX;
        }
    }
}

/*!
    \brief      initialize the LEDs
    \param[in]  none
    \param[out] none
    \retval     none
*/
void all_led_init(void)
{
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);
}

/*!
    \brief      all LEDs turn off
    \param[in]  none
    \param[out] none
    \retval     none
*/
void all_led_off(void)
{
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);
    gd_eval_led_off(LED3);
    gd_eval_led_off(LED4);
}

/*!
    \brief      configure clock output function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void clock_output_config(void)
{
    /* peripheral clock enable */
    rcu_periph_clock_enable(RCU_GPIOF);

    rcu_osci_on(RCU_IRC32M);
    if(ERROR == rcu_osci_stab_wait(RCU_IRC32M)) {
        printf("RCU_IRC32M rcu_osci_stab_wait timeout! \r\n");
        while(1);
    }

    /* configure clock output pin */
    gpio_mode_set(GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_12);
    gpio_output_options_set(GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_HIGH, GPIO_PIN_12);
    gpio_af_set(GPIOF, GPIO_AF_0, GPIO_PIN_12);
}

/*!
    \brief      select type of clock output
    \param[in]  none
    \param[out] none
    \retval     none
*/
void clock_output_select(uint8_t seq)
{
    switch(seq) {
    case CKOUT0_SYSTEMCLOCK_CKOUT:
        rcu_ckout_config(RCU_CKOUTSRC_CKSYS, RCU_CKOUT_DIV16);
        printf("CK_OUT0: system clock divided by 16 \r\n");
        break;
    case CKOUT0_PLL_DIV8_CKOUT:
        /* enable PLL clock */
        rcu_osci_on(RCU_PLL_CK);
        if(ERROR == rcu_osci_stab_wait(RCU_PLL_CK)) {
            while(1) {
            }
        }
        rcu_ckout_config(RCU_CKOUTSRC_CKPLL_DIV8, RCU_CKOUT_DIV4);
        printf("CK_OUT0: PLL clock / 8 divided by 4 \r\n");
        break;
    case CKOUT0_HXTAL_CKOUT:
        /* enable HXTAL clock */
        rcu_osci_on(RCU_HXTAL);
        if(ERROR == rcu_osci_stab_wait(RCU_HXTAL)) {
            while(1) {
            }
        }
        rcu_ckout_config(RCU_CKOUTSRC_HXTAL, RCU_CKOUT_DIV1);
        printf("CK_OUT0: HXTAL clock \r\n");
        break;
    case CKOUT0_IRC32M_CKOUT:
        rcu_ckout_config(RCU_CKOUTSRC_IRC32M, RCU_CKOUT_DIV2);
        printf("CK_OUT0: IRC32M clock divided by 2 \r\n");
        break;
    case CKOUT0_IRC32K_CKOUT:
        rcu_osci_on(RCU_IRC32K);
        if(ERROR == rcu_osci_stab_wait(RCU_IRC32K)) {
            while(1) {
            }
        }
        rcu_ckout_config(RCU_CKOUTSRC_IRC32K, RCU_CKOUT_DIV1);
        printf("CK_OUT0: IRC32K clock \r\n");
        break;

    default:
        printf("clock output select parameter error! \r\n");
        while(1);
    }
}
