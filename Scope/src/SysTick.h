#ifndef __SysTick_h
#define __SysTick_h
#include <stdint.h>
class SysTick{

public:
    SysTick() {};
    void begin(uint32_t usPeriod);
    void OnInterrupt();
    void delay(uint32_t msdly);
private:
    uint64_t SystemClockSpeed; 
    uint64_t NextInterruptTime;
    uint64_t Period;
};
#endif