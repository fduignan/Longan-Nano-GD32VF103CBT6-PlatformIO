#include <stdint.h>
#include "display.h"
#include "spi.h"
#include "systick.h"
extern "C" {
#include "gd32vf103.h"
}

/*
    The LCD display on the sipeed longan nano seems to be an ST7735 with a resolution
    of 160 x 80
    It's an SPI device connected as follows:
    SCK   PA5   (SCK0)
    MOSI  PA7   (MOSI0)
    RS    PB0
    RST   PB1
    CS    PB2
*/
void display::begin(spi *SPI)
{      		

    this->SPI = SPI;
    // Turn on Port A 
    rcu_periph_clock_enable(RCU_GPIOA);
    // Turn on Port B
    rcu_periph_clock_enable(RCU_GPIOB);

    //  Configure the various control pins
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2);
    SPI->begin();    

    // Initialization sequence from Longan sample code 
    RSTHigh(); // Drive reset high
    delay_1ms(25); // wait   
    RSTLow(); // Drive reset low
    delay_1ms(250); // wait   
    RSTHigh(); // Drive reset high
    delay_1ms(25); // wait    
    	writeCommand(0x11);	// turn off sleep mode
	delay_1ms(100);

	writeCommand(0x21);	// display inversion mode
	writeCommand(0xB1);	// Set the frame frequency of the full colors normal mode
						// Frame rate=fosc/((RTNA x 2 + 40) x (LINE + FPA + BPA +2))
						// fosc = 850kHz
	writeData8(0x05);	// RTNA
	writeData8(0x3A);	// FPA
	writeData8(0x3A);	// BPA

	writeCommand(0xB2);	// Set the frame frequency of the Idle mode
						// Frame rate=fosc/((RTNB x 2 + 40) x (LINE + FPB + BPB +2))
						// fosc = 850kHz
	writeData8(0x05);	// RTNB
	writeData8(0x3A);	// FPB
	writeData8(0x3A);	// BPB

	writeCommand(0xB3);	// Set the frame frequency of the Partial mode/ full colors
	writeData8(0x05);  
	writeData8(0x3A);
	writeData8(0x3A);
	writeData8(0x05);
	writeData8(0x3A);
	writeData8(0x3A);

	writeCommand(0xB4);
	writeData8(0x03);

	writeCommand(0xC0);
	writeData8(0x62);
	writeData8(0x02);
	writeData8(0x04);

	writeCommand(0xC1);
	writeData8(0xC0);

	writeCommand(0xC2);
	writeData8(0x0D);
	writeData8(0x00);

	writeCommand(0xC3);
	writeData8(0x8D);
	writeData8(0x6A);   

	writeCommand(0xC4);
	writeData8(0x8D); 
	writeData8(0xEE); 

	writeCommand(0xC5);  /*VCOM*/
	writeData8(0x0E);    

	writeCommand(0xE0);
	writeData8(0x10);
	writeData8(0x0E);
	writeData8(0x02);
	writeData8(0x03);
	writeData8(0x0E);
	writeData8(0x07);
	writeData8(0x02);
	writeData8(0x07);
	writeData8(0x0A);
	writeData8(0x12);
	writeData8(0x27);
	writeData8(0x37);
	writeData8(0x00);
	writeData8(0x0D);
	writeData8(0x0E);
	writeData8(0x10);

	writeCommand(0xE1);
	writeData8(0x10);
	writeData8(0x0E);
	writeData8(0x03);
	writeData8(0x03);
	writeData8(0x0F);
	writeData8(0x06);
	writeData8(0x02);
	writeData8(0x08);
	writeData8(0x0A);
	writeData8(0x13);
	writeData8(0x26);
	writeData8(0x36);
	writeData8(0x00);
	writeData8(0x0D);
	writeData8(0x0E);
	writeData8(0x10);

	writeCommand(0x3A);	// define the format of RGB picture data
	writeData8(0x05);	// 16-bit/pixel

	writeCommand(0x29);	// Display On
    fillRectangle(0,0,SCREEN_WIDTH, SCREEN_HEIGHT, 0xffff);  // black out the screen    
}
void display::putPixel(uint16_t x, uint16_t y, uint16_t colour)
{
    openAperture(x, y, x + 1, y + 1);
    writeData16(colour); 
}
void display::putImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *Image)
{
    uint16_t Colour;
    openAperture(x, y, x + width - 1, y + height - 1);
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            Colour = *(Image++);
            writeData16(Colour);
        }
    }
}
void display::drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t Colour)
{
    drawLine(x,y,x+w,y,Colour);
    drawLine(x,y,x,y+h,Colour);
    drawLine(x+w,y,x+w,y+h,Colour);
    drawLine(x,y+h,x+w,y+h,Colour);
}
void display::fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t Colour)
{
    openAperture(x, y, x + width - 1, y + height - 1);
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            writeData16(Colour);
        }
    }
}
void display::drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour)
{
// Reference : https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);
    if (radius > x0)
        return; // don't draw even parially off-screen circles
    if (radius > y0)
        return; // don't draw even parially off-screen circles
        
    if ((x0+radius) > SCREEN_WIDTH)
        return; // don't draw even parially off-screen circles
    if ((y0+radius) > SCREEN_HEIGHT)
        return; // don't draw even parially off-screen circles    
    while (x >= y)
    {
        putPixel(x0 + x, y0 + y, Colour);
        putPixel(x0 + y, y0 + x, Colour);
        putPixel(x0 - y, y0 + x, Colour);
        putPixel(x0 - x, y0 + y, Colour);
        putPixel(x0 - x, y0 - y, Colour);
        putPixel(x0 - y, y0 - x, Colour);
        putPixel(x0 + y, y0 - x, Colour);
        putPixel(x0 + x, y0 - y, Colour);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}
void display::fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour)
{
// Reference : https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
// Similar to drawCircle but fills the circle with lines instead
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    if (radius > x0)
        return; // don't draw even parially off-screen circles
    if (radius > y0)
        return; // don't draw even parially off-screen circles
        
    if ((x0+radius) > SCREEN_WIDTH)
        return; // don't draw even parially off-screen circles
    if ((y0+radius) > SCREEN_HEIGHT)
        return; // don't draw even parially off-screen circles        
    while (x >= y)
    {
        drawLine(x0 - x, y0 + y,x0 + x, y0 + y, Colour);        
        drawLine(x0 - y, y0 + x,x0 + y, y0 + x, Colour);        
        drawLine(x0 - x, y0 - y,x0 + x, y0 - y, Colour);        
        drawLine(x0 - y, y0 - x,x0 + y, y0 - x, Colour);        

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}
void display::drawLineLowSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour)
{
    // Reference : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm    
  int dx = x1 - x0;
  int dy = y1 - y0;
  int yi = 1;
  if (dy < 0)
  {
    yi = -1;
    dy = -dy;
  }
  int D = 2*dy - dx;
  
  int y = y0;

  for (int x=x0; x <= x1;x++)
  {
    putPixel(x,y,Colour);    
    if (D > 0)
    {
       y = y + yi;
       D = D - 2*dx;
    }
    D = D + 2*dy;
    
  }
}

void display::drawLineHighSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour)
{
        // Reference : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

  int dx = x1 - x0;
  int dy = y1 - y0;
  int xi = 1;
  if (dx < 0)
  {
    xi = -1;
    dx = -dx;
  }  
  int D = 2*dx - dy;
  int x = x0;

  for (int y=y0; y <= y1; y++)
  {
    putPixel(x,y,Colour);
    if (D > 0)
    {
       x = x + xi;
       D = D - 2*dy;
    }
    D = D + 2*dx;
  }
}
void display::drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Colour)
{
    // Reference : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    if ( iabs(y1 - y0) < iabs(x1 - x0) )
    {
        if (x0 > x1)
        {
            drawLineLowSlope(x1, y1, x0, y0, Colour);
        }
        else
        {
            drawLineLowSlope(x0, y0, x1, y1, Colour);
        }
    }
    else
    {
        if (y0 > y1) 
        {
            drawLineHighSlope(x1, y1, x0, y0, Colour);
        }
        else
        {
            drawLineHighSlope(x0, y0, x1, y1, Colour);
        }
        
    }
}


void display::openAperture(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{   
    // open up an area for drawing on the display
    // For some strange reason, the LCD on the Sipeed Longan Nano has a 26 pixel
    // offset on the x dimension and 1 pixel on the y-dimension
    x1 += 26;
    x2 += 26;
    y1++;
    y2++;
    writeCommand(0x2A); // Set X limits
    RSHigh();
    CSLow();    
    SPI->writeData8(x1>>8);
    SPI->writeData8(x1&0xff);    
    SPI->writeData8(x2>>8);
    SPI->writeData8(x2&0xff);     
    CSHigh();
    writeCommand(0x2B); // Set Y limits
    RSHigh();
    CSLow();    
    SPI->writeData8(y1>>8);
    SPI->writeData8(y1&0xff);    
    SPI->writeData8(y2>>8);
    SPI->writeData8(y2&0xff); 
    CSHigh();
    writeCommand(0x2c); // put display in to data write mode
}
void display::RSLow()
{   // drive D/C pin low
    gpio_bit_reset(GPIOB,GPIO_PIN_0);
    
}
void display::RSHigh()
{   // drive D/C pin high
    gpio_bit_set(GPIOB,GPIO_PIN_0);
}
void display::RSTLow()
{   // Drive reset low
    gpio_bit_reset(GPIOB,GPIO_PIN_1);
}
void display::RSTHigh()
{   // Drive reset high
    gpio_bit_set(GPIOB,GPIO_PIN_1);
}
void display::CSLow()
{   // Drive chip select low
    gpio_bit_reset(GPIOB,GPIO_PIN_2);
}
void display::CSHigh()
{   // Drive chip select high
    gpio_bit_set(GPIOB,GPIO_PIN_2);
}
void display::writeCommand(uint8_t Cmd)
{
    RSLow();
    CSLow();
    SPI->writeData8(Cmd);
    CSHigh();
}
void display::writeData8(uint8_t Data)
{
    RSHigh();   
    CSLow();
    SPI->writeData8(Data);
    CSHigh();
}
void display::writeData16(uint16_t Data)
{
    
    RSHigh();
    CSLow();
    SPI->writeData16(Data);
    CSHigh();
}
