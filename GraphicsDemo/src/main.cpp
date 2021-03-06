#include "led.h"
#include "display.h"
#include "spi.h"
display Display;
spi SPI;
extern "C" {
#include "gd32vf103.h"
}
led onboardRGB;
void delay(volatile uint32_t dly)
{
    while(dly--);
}
void setup()
{
    onboardRGB.begin();
}

uint16_t random_colour()
{
    static uint32_t shift_register=0xa5928241;
	// This is an unverified 31 bit PRBS generator
	// It should be maximum length but this has not been verified 
	uint32_t new_bit=0;
    new_bit= ((shift_register & (1<<27))>>27) ^ ((shift_register & (1<<30))>>30);
    new_bit= ~new_bit;
    new_bit = new_bit & 1;
    shift_register=shift_register << 1;
    shift_register=shift_register | (new_bit);
	
	return shift_register & 0xffff; // return 16 LSB's 
}
int main()
{
    int square_size=20;
    int xofs,yofs;
    setup();
    SPI.begin();
    Display.begin(&SPI);
    xofs = yofs = 0;
    xofs = 0;    
    while(1)
    {        
        Display.fillRectangle(xofs,yofs,square_size,square_size,random_colour());
        onboardRGB.red_on();        
        delay(10000);        
        onboardRGB.red_off();
        delay(10000);        
        onboardRGB.green_on();        
        delay(10000);         
        onboardRGB.green_off();
        delay(10000);        
        onboardRGB.blue_on();        
        delay(10000);         
        onboardRGB.blue_off();
        delay(10000);        
        
        xofs+=square_size;
        
        if (xofs>80-square_size)
        {
            xofs = 0;        
            yofs +=square_size;
            if (yofs > 160-square_size)
            {
                yofs=0;                
            }
        }
    }
}