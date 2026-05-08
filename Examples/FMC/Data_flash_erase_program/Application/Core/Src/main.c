/*!
    \file    main.c
    \brief   data flash program, erase example

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

#define FMC_PAGE_SIZE                ((uint16_t)0x400U)
#define FMC_WRITE_START_ADDR         ((uint32_t)0x08805000U)
#define FMC_WRITE_END_ADDR           ((uint32_t)0x08806000U)

#define FMC_PROGRAM_TYPE_FOURWORDS   ((uint8_t)0x00U)
#define FMC_PROGRAM_TYPE_FAST        ((uint8_t)0x01U)

#define DOUBLE_WORDS_CNT_IN_ROW      8

uint32_t address = 0x00U;
uint64_t data0   = 0x0123456789abcdefU;
uint64_t data1   = 0x08192a3b4c5d6e7fU;
led_typedef_enum lednum = LED3;

/* calculate the number of page to be programmed/erased */
uint32_t PageNum = (FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) / FMC_PAGE_SIZE;
/* calculate the number of page to be programmed/erased */
uint32_t WordNum = ((FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) >> 2);

/* data buffer for fast programming */
static uint64_t data_buffer[DOUBLE_WORDS_CNT_IN_ROW] = {
    0x0000000000000000U, 0x1111111111111111U, 0x2222222222222222U, 0x3333333333333333U,
    0x4444444444444444U, 0x5555555555555555U, 0x6666666666666666U, 0x7777777777777777U
};

/* clear all pending flags */
void fmc_flags_clear(void);
/* erase fmc pages from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR */
void fmc_erase_pages(void);
/* program fmc from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR */
void fmc_program(uint8_t program_type);
/* check fmc erase result */
void fmc_erase_pages_check(void);
/* check fmc program result */
void fmc_program_check(uint8_t program_type);

/*!
    \brief      clear all pending flags
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_flags_clear(void)
{
    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_END);
    fmc_flag_clear(FMC_FLAG_WPERR);
    fmc_flag_clear(FMC_FLAG_PGSERR);
    fmc_flag_clear(FMC_FLAG_PGAERR);
    fmc_flag_clear(FMC_FLAG_PGERR);
}

/*!
    \brief      erase fmc pages from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_pages(void)
{
    uint32_t EraseCounter;

    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flags_clear();

    /* erase the flash pages */
    for(EraseCounter = 0; EraseCounter < PageNum; EraseCounter++) {
        fmc_page_erase(FMC_WRITE_START_ADDR + (FMC_PAGE_SIZE * EraseCounter));
        fmc_flags_clear();
    }

    /* lock the main FMC after the erase operation */
    fmc_lock();
}

/*!
    \brief      program fmc from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  program_type: program type selection
      \arg        FMC_PROGRAM_TYPE_FOURWORDS: program 4 words at a time
      \arg        FMC_PROGRAM_TYPE_FAST: fast program mode
    \param[out] none
    \retval     none
*/
void fmc_program(uint8_t program_type)
{
    /* unlock the flash program/erase controller */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flags_clear();

    address = FMC_WRITE_START_ADDR;

    /* program flash */
    while(address < FMC_WRITE_END_ADDR) {
        if(FMC_PROGRAM_TYPE_FOURWORDS == program_type) {
            fmc_fourword_program(address, data0, data1);
            address += 4 * sizeof(uint32_t);
        } else if(FMC_PROGRAM_TYPE_FAST == program_type) {
            fmc_fast_program(address, data_buffer);
            address += DOUBLE_WORDS_CNT_IN_ROW * sizeof(uint64_t);
        }
        fmc_flags_clear();
    }

    /* lock the main FMC after the program operation */
    fmc_lock();
}

/*!
    \brief      check fmc erase result
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_pages_check(void)
{
    uint32_t i;
    uint32_t *ptrd;

    ptrd = (uint32_t *)FMC_WRITE_START_ADDR;

    /* check flash whether has been erased */
    for(i = 0; i < WordNum; i++) {
        if(0xFFFFFFFF != (*ptrd)) {
            lednum = LED1;
            gd_eval_led_on(lednum);
            break;
        } else {
            ptrd++;
        }
    }
}

/*!
    \brief      check fmc program result
    \param[in]  program_type: program type that was used for programming
      \arg        FMC_PROGRAM_TYPE_FOURWORDS: check 4 words programming result
      \arg        FMC_PROGRAM_TYPE_FAST: check fast programming result
    \param[out] none
    \retval     none
*/
void fmc_program_check(uint8_t program_type)
{
    uint32_t i;
    uint64_t *ptrd;
    ptrd = (uint64_t *)FMC_WRITE_START_ADDR;

    if(FMC_PROGRAM_TYPE_FOURWORDS == program_type) {
        /* check flash whether has been programmed */
        for(i = 0; i < WordNum / 4; i++) {
            if(((*ptrd++) != data0) || ((*ptrd++) != data1)) {
                lednum = LED2;
                gd_eval_led_on(lednum);
                break;
            }
        }
    } else if(FMC_PROGRAM_TYPE_FAST == program_type) {
        /* check flash whether has been programmed */
        for(i = 0; i < WordNum / 2; i++) {
            if((*ptrd) != data_buffer[i % DOUBLE_WORDS_CNT_IN_ROW]) {
                lednum = LED2;
                gd_eval_led_on(lednum);
                break;
            } else {
                ptrd++;
            }
        }
    }
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize led on the board */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);

    /* step1: erase pages and check if it is successful. If not, light the LED1. */
    fmc_erase_pages();
    fmc_erase_pages_check();

    /* step2: program and check if it is successful. If not, light the LED2. */
    fmc_program(FMC_PROGRAM_TYPE_FOURWORDS);
    fmc_program_check(FMC_PROGRAM_TYPE_FOURWORDS);

    /* step3: erase pages and check if it is successful. If not, light the LED1. */
    fmc_erase_pages();
    fmc_erase_pages_check();

    /* step4: fast program and check if it is successful. If not, light the LED2. */
    fmc_program(FMC_PROGRAM_TYPE_FAST);
    fmc_program_check(FMC_PROGRAM_TYPE_FAST);

    /* if all the operations are successful, light the LED3. */
    if(LED3 == lednum) {
        gd_eval_led_on(LED3);
    }

    while(1) {
    }
}
