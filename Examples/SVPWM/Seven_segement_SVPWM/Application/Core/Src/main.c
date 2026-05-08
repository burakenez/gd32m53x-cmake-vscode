/*!
    \file    main.c
    \brief   Seven segment SVPWM demo for GD32M53x

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
#include <stdio.h>
#include "main.h"
#include "gd32m531r_eval.h"
#include "math.h"

#define PERIOD 5625U

/* constants */
#define ONE_BY_SQRT3                        (0.577350f)                         /* the value of 1/sqrt(3) */
#define TWO_BY_SQRT3                        (2.0f * 0.577350f)                  /* the value of 2/sqrt(3) */
#define SQRT3_BY_2                          (0.866025f)                         /* the value of sqrt(3)/2 */

void svpwm_config(void);
void gpio_config(void);
void software_svpwm(float alpha, float beta);

/* sector enum */
typedef enum {
    ONE = 1,                                                                    /* sector one */
    TWO,                                                                        /* sector two */
    THREE,                                                                      /* sector three */
    FOUR,                                                                       /* sector four */
    FIVE,                                                                       /* sector five */
    SIX                                                                         /* sector six */
} SECTOR;

SECTOR software_sector, hareware_sector;

uint16_t software_ta, software_tb, software_tc;
uint16_t hardware_ta, hardware_tb, hardware_tc;

/*!
    \brief      svpwm of software
    \param[in]  alpha the voltage of alpha axis
    \param[in]  beta the voltage of beta axis
    \param[out] none
    \retval     none
*/
void software_svpwm(float alpha, float beta)
{
    alpha = alpha * 1.5f;
    beta = beta * 1.5f;
    /* sector recognization */
    if(beta >= 0.0f) {
        if(alpha >= 0.0f) {
            /* quadrant I */
            if(ONE_BY_SQRT3 * beta > alpha) {
                software_sector = TWO;
            } else {
                software_sector = ONE;
            }
        } else {
            /* quadrant II */
            if(-ONE_BY_SQRT3 * beta > alpha) {
                software_sector = THREE;
            } else {
                software_sector = TWO;
            }
        }
    } else {
        if(alpha >= 0.0f) {
            /* quadrant IV */
            if(-ONE_BY_SQRT3 * beta > alpha) {
                software_sector = FIVE;
            } else {
                software_sector = SIX;
            }
        } else {
            /* quadrant III */
            if(ONE_BY_SQRT3 * beta > alpha) {
                software_sector = FOUR;
            } else {
                software_sector = FIVE;
            }
        }
    }

    switch(software_sector) {
    /* sector 1: vector 100 and vector 110 */
    case ONE: {
            /* the time of vectors */
            uint32_t t4 = (uint32_t)((alpha - ONE_BY_SQRT3 * beta) * PERIOD);
            uint32_t t6 = (uint32_t)((TWO_BY_SQRT3 * beta) * PERIOD);

            /* TIMER PWM timings */
            software_tc = (PERIOD - t4 - t6) / 2;
            software_tb = software_tc + t6;
            software_ta = software_tb + t4;
            break;
        }

    /* sector 2: vector 110 and vector 010 */
    case TWO: {
            /* the time of vectors */
            uint32_t t6 = (uint32_t)((alpha + ONE_BY_SQRT3 * beta) * PERIOD);//>> 15;
            uint32_t t2 = (uint32_t)((-alpha + ONE_BY_SQRT3 * beta) * PERIOD);
            software_tc = (PERIOD - t6 - t2) / 2;
            software_ta = software_tc + t6;
            software_tb = software_ta + t2;
            break;
        }

    /* sector 3: vector 010 and vector 011 */
    case THREE: {
            /* the time of vectors */
            uint32_t t2 = (uint32_t)((TWO_BY_SQRT3 * beta) * PERIOD);
            uint32_t t3 = (uint32_t)((-alpha - ONE_BY_SQRT3 * beta) * PERIOD);

            /* TIMER PWM timings */
            software_ta = (PERIOD - t2 - t3) / 2;
            software_tc = software_ta + t3;
            software_tb = software_tc + t2;
            break;
        }

    /* sector 4: vector 011 and vector 001 */
    case FOUR: {
            /* the time of vectors */
            uint32_t t3 = (uint32_t)((-alpha + ONE_BY_SQRT3 * beta) * PERIOD);
            uint32_t t1 = (uint32_t)((-TWO_BY_SQRT3 * beta) * PERIOD);

            /* TIMER PWM timings */
            software_ta = (PERIOD - t3 - t1) / 2;
            software_tb = software_ta + t3;
            software_tc = software_tb + t1;
            break;
        }

    /* sector 5: vector 001 and vector 101 */
    case FIVE: {
            /* the time of vectors */
            uint32_t t5 = (uint32_t)((-alpha - ONE_BY_SQRT3 * beta) * PERIOD);
            uint32_t t1 = (uint32_t)((alpha - ONE_BY_SQRT3 * beta) * PERIOD);

            /* TIMER PWM timings */
            software_tb = (PERIOD - t5 - t1) / 2;
            software_ta = software_tb + t1;
            software_tc = software_ta + t5;
            break;
        }

    /* sector 6: vector 101 and vector 100 */
    case SIX: {
            /* the time of vectors */
            uint32_t t5 = (uint32_t)((-TWO_BY_SQRT3 * beta) * PERIOD);
            uint32_t t4 = (uint32_t)((alpha + ONE_BY_SQRT3 * beta) * PERIOD);

            /* TIMER PWM timings */
            software_tb = (PERIOD - t5 - t4) / 2;
            software_tc = software_tb + t5;
            software_ta = software_tc + t4;
            break;
        }
    default:
        break;
    }
}

/*!
    \brief      configure the svpwm peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void svpwm_config(void)
{
    rcu_periph_clock_enable(RCU_SVPWM);
    svpwm_parameter_struct svpwm_parameter;
    svpwm_struct_para_init(&svpwm_parameter);
    svpwm_parameter.switch_mode = SVPWM_SWITCH_MODE1;
    svpwm_parameter.working_mode = SVPWM_MODE_SEVEN_SEGMENT;
    svpwm_parameter.period_count = PERIOD;
    svpwm_init(&svpwm_parameter);
}


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    systick_config();
    svpwm_config();
    int i = 0;
    float sin_data, cos_data;
    int error = 0;
    while(1) {
        for(i = 0 ; i < 200; i++) {
            sin_data = 0.5 * sin((float)i / 200.0f * 2 * 3.1415926f);
            cos_data = 0.5 * cos((float)i / 200.0f * 2 * 3.1415926f);
            software_svpwm(sin_data, cos_data);
            delay_1ms(1);
            svpwm_alpha_beta_write(sin_data, cos_data);
            svpwm_enable();
            while(SET != svpwm_flag_get(SVPWM_FLAG_OSF));
            svpwm_ta_tb_tc_read(&hardware_ta, &hardware_tb, &hardware_tc);
            hareware_sector = (SECTOR)svpwm_sector_read();
            if((((int32_t)hardware_ta - (int32_t)software_ta) <= -2) || (((int32_t)hardware_ta - (int32_t)software_ta) >= 2)) {
                error ++;
            }
            if((((int32_t)hardware_tb - (int32_t)software_tb) <= -2) || (((int32_t)hardware_tb - (int32_t)software_tb) >= 2)) {
                error ++;
            }
            if((((int32_t)hardware_tc - (int32_t)software_tc) <= -2) || (((int32_t)hardware_tc - (int32_t)software_tc) >= 2)) {
                error ++;
            }
        }

    }
}

