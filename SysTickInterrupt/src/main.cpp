// Longan Nano Systick Interrupt demo that makes use of the system cycle counter to
// generate periodic interrupts.  
#include "led.h"
extern "C" {
#include "gd32vf103.h"
}
#include "SysTick.h"
SysTick stk;
led LED;  // Include LED object for feedback purposes

void delay(volatile uint32_t dly)
{
    while(dly--);
}
int main()
{
    int Test=0;
    LED.begin();
    eclic_global_interrupt_enable();
    stk.begin(1000000);  // start the systick timer going
    
    while(1)
    {
       // asm(" wfi ");   // sleep - note: if sleep is deep then systick timer stops
                        // so the CPU will not wake.  If you need a deep sleep then
                        // use one of the external timers.
        Test++;
        if (Test == 10)
        {
            LED.blue_on();
        }
        if (Test == 20)
        {
            LED.blue_off();
            Test = 0;
        }
        delay(1000000);
    }
    return 0;
}