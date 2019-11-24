/*!
    \file  main.c
    \brief running led
    
    \version 2019-6-5, V1.0.0, firmware for GD32VF103
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.
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

#include "gd32vf103.h"
#include "systick.h"
#include <stdio.h>

/* RGB LED is connected as follows:
    PC13 : Red
    PA1  : Green
    PA2  : Blue
*/



void led_init()
{
    /* enable the led clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOA);

    /* configure led GPIO port */ 
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    GPIO_BOP(GPIOC) = GPIO_PIN_13;
    GPIO_BOP(GPIOA) = GPIO_PIN_1;
    GPIO_BOP(GPIOA) = GPIO_PIN_2;
}
/*!
    Turn on the red part of the RGB LED
    by clearing PC13
*/
void red_on()
{
    GPIO_BC(GPIOC) = GPIO_PIN_13;
}
/*!
    Turn off the red part of the RGB LED
    by setting PC13
*/
void red_off()
{
    GPIO_BOP(GPIOC) = GPIO_PIN_13;
}
/*!
    Turn on the blue part of the RGB LED
    by clearing PA1
*/
void green_on()
{
    GPIO_BC(GPIOA) = GPIO_PIN_1;
}
/*!
    Turn off the green part of the RGB LED
    by setting PA1
*/
void green_off()
{
    GPIO_BOP(GPIOA) = GPIO_PIN_1;
}
/*!
    Turn on the blue part of the RGB LED
    by clearing PA2
*/
void blue_on()
{
    GPIO_BC(GPIOA) = GPIO_PIN_2;
}
/*!
    Turn off the blue part of the RGB LED
    by setting PA2
*/
void blue_off()
{
    GPIO_BOP(GPIOA) = GPIO_PIN_2;
}
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    led_init();
    red_off();
    green_off();
    blue_off();
    while(1){        
        red_on();        
        delay_1ms(100);        
        red_off();
        delay_1ms(100);
        green_on();        
        delay_1ms(100);        
        green_off();
        delay_1ms(100);
        blue_on();        
        delay_1ms(100);        
        blue_off();
        delay_1ms(100);
    }
}