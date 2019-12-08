extern "C" {
#include "gd32vf103.h"
}
#include "timer.h"
static timer * ptimer=0;

void timer::begin()
{
    ptimer = this;
    /* ----------------------------------------------------------------------------
    TIMER6 Configuration: 
    Want the timer to run with an interrupt frequency of 1kHz.
    SystemCoreClock = 
    TIMER6CLK = SystemCoreClock/5400 = 20KHz.
    TIMER6 configuration is timing mode, and the timing is 20000/20 = 1000
    ---------------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER6);

    timer_deinit(TIMER6);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER6 configuration */
    timer_initpara.prescaler         = 5399;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 20;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER6, &timer_initpara);
    timer_interrupt_enable(TIMER6, TIMER_INT_UP);
    timer_enable(TIMER6);
    eclic_global_interrupt_enable();
    eclic_enable_interrupt(TIMER6_IRQn);
    // zero out the callback array       
    for (int i=0;i<MAX_CB;i++)
    {
        cb[i] = 0;
    }
}
int timer::attach(fptr Callback)
{

    for (int i=0;i < MAX_CB;i++)
    {
        if (cb[i]==0) 
        {
            // found a free slot
            cb[i]=Callback;
            return 0;
        }
    }
    return -1; // no room left
}
int timer::detach(fptr Callback)
{
    for (int i=0;i < MAX_CB;i++)
    {
        if (cb[i]==Callback) 
        {
            // found the slot so empty it
            cb[i]=0;
            return 0;
        }
    }   
    return -1; // didn't find that one

}
void timer::OnInterrupt(void)
{
    for (int i=0;i < MAX_CB;i++)
    { // look through callback array and call the non-zero entries.
        if (cb[i]!=0)
        {
            cb[i]();
        }
    }
}
extern "C" {
    void TIMER6_IRQHandler(void)
{
    if(timer_interrupt_flag_get(TIMER6, TIMER_INT_UP)==SET)
    {
        /* clear Timer 6 update interrupt bit */
        timer_interrupt_flag_clear(TIMER6, TIMER_INT_UP);
    }
    if (ptimer)
    {
        ptimer->OnInterrupt();
    }
}
}

