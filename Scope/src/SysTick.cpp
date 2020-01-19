extern "C" {
#include "gd32vf103.h"
}
#include <stdint.h>
#include "SysTick.h"
static SysTick *pSysTick = 0;
void SysTick::begin(uint32_t usPeriod)
{
    pSysTick = this;
    SystemClockSpeed = 108000000/8; // the GD32VF103 divides the high speed clock by 8
    Period = usPeriod;
    Period = Period * SystemClockSpeed;
    Period = Period / 1000000;
    NextInterruptTime = Period;
    // Zero the clock to start with
    *(volatile uint32_t *)(TIMER_CTRL_ADDR + TIMER_MTIME + 4)=0;
    *(volatile uint32_t *)(TIMER_CTRL_ADDR + TIMER_MTIME )=0;
    *(volatile uint32_t *)(TIMER_CTRL_ADDR + TIMER_MTIMECMP + 4)=NextInterruptTime >> 32;
    *(volatile uint32_t *)(TIMER_CTRL_ADDR + TIMER_MTIMECMP ) = NextInterruptTime & 0xffffffff;
    eclic_enable_interrupt(CLIC_INT_TMR);
}
void SysTick::OnInterrupt()
{
    // At each interrupt the Timer compare register has to be updated to the next
    // interrupt time.
    NextInterruptTime += Period;
    *(volatile uint32_t *)(TIMER_CTRL_ADDR + TIMER_MTIMECMP + 4)=NextInterruptTime >> 32;
    *(volatile uint32_t *)(TIMER_CTRL_ADDR + TIMER_MTIMECMP ) = NextInterruptTime & 0xffffffff;

    // LED blink (relies on the LED port bits being initialized elsewhere)
    static int State = 0;   
    if (State) 
    {
        GPIO_BOP(GPIOA) = GPIO_PIN_1;
        State = 0;
    }
    else
    {        
        GPIO_BC(GPIOA) = GPIO_PIN_1;
        State = 1;
    }
}
void SysTick::delay(uint32_t msdly)
{
// delay for thet specified number of milliseconds;
    uint64_t EndTime = msdly;
    EndTime = (EndTime * SystemCoreClock/8000);
    EndTime = EndTime + get_timer_value();

    while(get_timer_value() < EndTime);
}

// The interrupt handler for the internal timer.  C naming is required
// to allow it to over-ride the weak symbol defined in the startup code.
// Also have to insert assembler to back up and restore registers prior
// to calling C++ interrupt handler
// Based on initial investigations it would seem that context is saved
// before entry in to this function and restored afterwards i.e. this
// is not an interrupt service routine in the true sense - it is a 
// function called by another interrupt handler buried deeper in the 
// SDK.
extern "C" {
void eclic_mtip_handler()
{
    pSysTick->OnInterrupt();
}
}
