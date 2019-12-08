#include <stdint.h>
#define MAX_CB 10
typedef void (*fptr)(void);
class timer
{
public:

    timer() {};
    void begin();
    int attach(fptr Callback);
    int detach(fptr Callback);
    void OnInterrupt(void);
    //friend void TIMER6_IRQHandler(void);
private:
    fptr cb[MAX_CB];
};