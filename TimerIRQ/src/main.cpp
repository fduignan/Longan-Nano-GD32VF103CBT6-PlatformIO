#include <stdio.h>
extern "C" {
#include "gd32vf103.h"
}
#include "led.h"
#include "timer.h"
timer T;
led LED;
volatile uint32_t seconds = 0;

void OnTimer()
{

    static int count=0;
    count++;
    if (count >=500)
    {
        count = 0;
        static int State = 0;
        if (State)
        {
            seconds++;
            LED.red_on();
            State = 0;
        }
        else 
        {
            LED.red_off();
            State = 1;
        }
    }
}
void delay(volatile uint32_t dly)
{
    while(dly--)
    {

    }
}
void initUART()
{
 /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);
    /* connect port to USARTx_Tx */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);


    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 9600U);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);
}
int main()
{
    LED.begin();
    T.begin();
    T.attach(OnTimer);
    initUART();
    int old_seconds = 0;
    while(1)
    {
        asm(" wfi ");
        if (old_seconds != seconds)
        {
            printf("%d\n",seconds);
            old_seconds = seconds;
        }
    }
    return 0;
}