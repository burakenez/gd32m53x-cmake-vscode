/*!
    \file    readme.txt
    \brief   running LED

    \version 2026-03-04, V1.0.0, demo for GD32M53x
*/

/*
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

  This demo is based on the GD32M531R-EVAL-V1.2 board, it shows how to configure the CAN
peripheral to send and receive CAN frames in normal mode. The frames are sent and print
transmit data by pressing Wakeup Key. When the frames are received, the receive data
will be printed and the LED2 will toggle. A frame with extended frame ID: 0xaabb and DLC
of 8 is required.

  If the print function is needed, make sure that there are no jumper caps connected
to JP1 and JP12. Then connect PF10 to pin 1 of JP18 and PF9 to pin 3 of JP18 using DuPont
wire. Meanwhile, modify the definition of the EVAL_COM macro in the gd32m53x_eval.h file to
EVAL_COM1. Connect CN3 to the PC.

  To ensure the power supply of the CAN transceiver, the development board must be powered
by CN2(GD-Link), and the SW1 needs to be ON state.

  JP19 must be connected.

  This example is tested with at least two GD32M531R-EVAL-V1.2 boards. The same program
example is loaded in all boards. User must connect L pin to L pin and H pin to H pin of
JP20 on the two boards for sending and receiving frames.
