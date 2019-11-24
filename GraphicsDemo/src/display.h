#ifndef __display_h
#define __display_h
#include <stdint.h>
#include "spi.h"
// Assumption: Top left of display is 0,0
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 160
// Define a macro to allow easy definition of colours
// Format of colour value: <BGND 1 bit><Red 5 bits><Green 5 bits><Blue 5 bits>
//#define RGBToWord( R,  G,  B)  (  ((G&0xf8) << (11-3)) | ((R&0xfc) << (5-2)) | ((B&0xf8)>>3) )
#define RGBToWord( R,  G,  B)  (  ((B&0xf8) << (11-3)) | ((G&0xfc) << (5-2)) | ((R&0xf8)>>3) )
class display
{
public:
    display(){};
    void begin(spi *SPI);
    void putPixel(uint16_t x, uint16_t y, uint16_t colour);
    void putImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *Image);
    void drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t Colour);
    void fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t Colour);
    void drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour);
    void fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour);
    void drawLineLowSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour);
    void drawLineHighSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour);
    void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Colour);
    void openAperture(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2); 

private:
    void RSLow();
    void RSHigh();
    void resetDisplay();
    void writeCommand(uint8_t Cmd);
    void writeData8(uint8_t Data);
    void writeData16(uint16_t Data);                      
    void RSTLow();
    void RSTHigh();            
    void CSLow();
    void CSHigh();
    int iabs(int x) // simple integer version of abs for use by graphics functions
    {
        if (x < 0)
            return -x;
        else
            return x;
    }
    spi *SPI;
};
#endif