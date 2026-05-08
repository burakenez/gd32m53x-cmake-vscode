/*!
    \file    main.c
    \brief   calculate sin with coprocessor example

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
#include "gd32m531r_eval.h"

/* the reset value of TMU_CS register */
#define TMU_CS_REG_RESET_VALUE    0x00000050U
/* the value of pi */
#define DEMO_PI             (3.14159265f)

uint32_t tmu_cs_reg = 0U;

/* the first input data: angle */
float theta = DEMO_PI / 2.0f;
/* the second inputdata: modulus */
float m = 100.0f;
/* the calculation results: result=m*sin(theta)*/
float result = 0;

/* TMU input data in q31 format */
uint32_t in_data_q31[2] = {0};
/* TMU output data in q31 format */
uint32_t out_data_q31[2] = {0};
/* TMU output data in floating point format */
float out_data_f32[2] = {0};

/* function declaration */
/* configure RCU */
void rcu_config(void);
/* configure TMU input and output data in q31 format */
void tmu_config_q31(void);
/* configure TMU input and output data in floating point format */
void tmu_config_f32(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    uint16_t scaling_factor = 128U;
    tmu_parameter_struct tmu_q31_struct;
    tmu_parameter_struct tmu_f32_struct;

    /* configure TMU peripheral */
    tmu_q31_struct.mode = TMU_MODE_SIN;
    tmu_q31_struct.scale = TMU_SCALING_FACTOR_1;
    tmu_q31_struct.output_floating = TMU_OUTPUT_FLOAT_DISABLE;
    tmu_q31_struct.input_floating = TMU_INPUT_FLOAT_DISABLE;
    tmu_q31_struct.dma_read = TMU_READ_DMA_DISABLE;
    tmu_q31_struct.dma_write = TMU_WRITE_DMA_DISABLE;
    tmu_q31_struct.read_times = TMU_READ_TIMES_2;
    tmu_q31_struct.write_times = TMU_WRITE_TIMES_2;
    tmu_q31_struct.output_width = TMU_OUTPUT_WIDTH_32;
    tmu_q31_struct.input_width = TMU_INPUT_WIDTH_32;

    tmu_cs_reg = (tmu_q31_struct.mode | tmu_q31_struct.scale | TMU_CS_REG_RESET_VALUE | \
                  tmu_q31_struct.output_floating | tmu_q31_struct.input_floating | \
                  tmu_q31_struct.dma_read | tmu_q31_struct.dma_write | \
                  tmu_q31_struct.read_times | tmu_q31_struct.write_times | \
                  tmu_q31_struct.output_width | tmu_q31_struct.input_width);

    /* enable TMU clock */
    rcu_periph_clock_enable(RCU_TMU);
    /* configure systick */
    systick_config();
    /* configure COM port */
    gd_eval_com_init(EVAL_COM);
    printf("\n TMU m*Sin(x) mode test by Coprocessor \n");
    /* software processes the input data */
    in_data_q31[0] = (uint32_t)((int32_t)(theta / DEMO_PI * 0x80000000U));
    in_data_q31[1] = (uint32_t)((int32_t)(m / scaling_factor * 0x80000000U));
    /* reset the TMU */
    tmu_deinit();
    /* configure TMU input and output data in q31 format */
    TMU_CP_CS_WRITE(tmu_cs_reg);
    /* write data to start TMU */
    TMU_CP_TWO_DATA_WRITE(in_data_q31[0], in_data_q31[1]);
    /* read two output data */
    TMU_CP_TWO_DATA_READ(out_data_q31[0], out_data_q31[1]);
    /* software processes the output data */
    result = scaling_factor * (float)((int32_t)out_data_q31[0]) / 0x80000000U;
    printf("\n in q31 format: %3.2f*sin(%3.2f)=%3.2f\n", m, theta, result);

    tmu_cs_reg = 0U;
    /* configure TMU peripheral */
    tmu_f32_struct.mode = TMU_MODE_SIN;
    tmu_f32_struct.scale = TMU_SCALING_FACTOR_1;
    tmu_f32_struct.output_floating = TMU_OUTPUT_FLOAT_ENABLE;
    tmu_f32_struct.input_floating = TMU_INPUT_FLOAT_ENABLE;
    tmu_f32_struct.dma_read = TMU_READ_DMA_DISABLE;
    tmu_f32_struct.dma_write = TMU_WRITE_DMA_DISABLE;
    tmu_f32_struct.read_times = TMU_READ_TIMES_2;
    tmu_f32_struct.write_times = TMU_WRITE_TIMES_2;
    tmu_f32_struct.output_width = TMU_OUTPUT_WIDTH_32;
    tmu_f32_struct.input_width = TMU_INPUT_WIDTH_32;

    tmu_cs_reg = (tmu_f32_struct.mode | tmu_f32_struct.scale | TMU_CS_REG_RESET_VALUE | \
                  tmu_f32_struct.output_floating | tmu_f32_struct.input_floating | \
                  tmu_f32_struct.dma_read | tmu_f32_struct.dma_write | \
                  tmu_f32_struct.read_times | tmu_f32_struct.write_times | \
                  tmu_f32_struct.output_width | tmu_f32_struct.input_width);

    /* reset the TMU */
    tmu_deinit();
    /* configure TMU input and output data in floating point format */
    TMU_CP_CS_WRITE(tmu_cs_reg);
    /* write data to start TMU */
    TMU_CP_TWO_DATA_WRITE(theta / DEMO_PI, m);
    /* read two output data */
    TMU_CP_TWO_DATA_READ(out_data_f32[0], out_data_f32[1]);

    printf("\n in floating point format: %3.2f*sin(%3.2f)=%3.2f\n", m, theta, out_data_f32[0]);

    while(1) {
    }
}
