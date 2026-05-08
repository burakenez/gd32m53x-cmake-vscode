/*!
    \file    main.c
    \brief   WWDGT delay feed demo

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
#include "gd32m531r_eval.h"

void wwdgt_config(void);

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

    /* configure LED1 and LED2 */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);

    /* turn off LED1 and LED2 */
    gd_eval_led_off(LED1);
    gd_eval_led_off(LED2);

    /* delay */
    delay_1ms(150U);

    /* check if the system has resumed from WWDGT reset */
    if(RESET != rcu_flag_get(RCU_FLAG_WWDGTRST)) {
        /* WWDGTRST flag set */
        gd_eval_led_on(LED1);
        /* clear the WWDGTRST flag */
        rcu_all_reset_flag_clear();

        while(1) {
        }
    }

    /* enable WWDGT clock */
    rcu_periph_clock_enable(RCU_WWDGT);

    /*
     *  set WWDGT clock = (PCLK1 (90MHz)/4096)/8 = 2747Hz (~364 us)
     *  set counter value to 16383
     *  set window start value to 15000
     *  set window end position value to 12888((3/4)*0x3FFF)
     *  refresh window is: ~364 * (16383-15000)= 503.412ms < refresh window < ~364 * (16383-12288) = 1490.580ms.
    */
    wwdgt_config();

    while(1) {
        /* toggle LED2 */
        gd_eval_led_toggle(LED2);
        /* insert 26 ms delay */
        delay_1ms(1000U);
        /* update WWDGT counter */
        wwdgt_counter_update(16383U);
    }
}

/*!
    \brief      configure and start the wwdgt
    \param[in]  none
    \param[out] none
    \retval     none
*/
void wwdgt_config(void)
{
    wwdgt_cfg_parameter_struct wwdgt_cfg_struct;

    /* initialize the wwdgt */
    wwdgt_deinit();
    wwdgt_struct_para_init(&wwdgt_cfg_struct);

    wwdgt_cfg_struct.counter            = 16383U;
    wwdgt_cfg_struct.reset_control      = WWDGT_RESET_ENABLE;
    wwdgt_cfg_struct.ewie_control       = WWDGT_EWIE_DISABLE;
    wwdgt_cfg_struct.window             = 15000U;
    wwdgt_cfg_struct.window_end_position = WWDGT_CFG_WEPS_THREE_QUARTER;
    wwdgt_cfg_struct.prescaler          = WWDGT_CFG_PSC_DIV8;
    wwdgt_cfg_init(&wwdgt_cfg_struct);
}
