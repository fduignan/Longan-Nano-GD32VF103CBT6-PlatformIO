extern "C" {
#include "gd32vf103.h"
}
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

void delay(volatile uint32_t dly)
{
    while(dly--)
    {

    }
}
extern "C" {
void EXTI10_15_IRQHandler()
{
    // This interrupt response handler's job is to
    // drive GPIOC bit 13 high thus turning the RED LED off
    {
        EXTI_PD = EXTI_13;
        GPIO_BOP(GPIOC) = GPIO_PIN_13;         
    }    
    

}
}
void exti_init(void)
{
    // Configures PC13 to generate an interrupt on a falling edge
    /* enable the AF clock */
    rcu_periph_clock_enable(RCU_AF);
    /* enable and set key EXTI interrupt to the specified priority */
    eclic_global_interrupt_enable();
    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
    eclic_irq_enable(EXTI10_15_IRQn, 1, 1);
    /* connect key EXTI line to key GPIO pin */
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOC, GPIO_PIN_SOURCE_13);
    /* configure key EXTI line */
    exti_init(EXTI_13, EXTI_INTERRUPT, EXTI_TRIG_FALLING);
    exti_interrupt_flag_clear(EXTI_13);
}

int main()
{

    led_init();
    exti_init();
    while(1)
    {
        delay(1000);
        // Drop GPIOC bit 13 low.  This should turn on the
        // red LED and also trigger an interrupt;        
        GPIO_BC(GPIOC) = GPIO_PIN_13;        

    }
    return 0;
}