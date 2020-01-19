#ifndef __CHANNEL_H
#define __CHANNEL_H
#include <stdint.h>
#include "display.h"
// Drawing of channel data:
/* Two basic approaches:
    1:  Don't remember any channel data and simply overwrite the
        screen each time you want to draw.  
    2:  Remember the channel data and "undraw" before draw.
        This also allows dot-joining
Third possible approach is to have a screen buffer and do DMA
A bit concerned that this might cause interrupt jitter though.
Whatever the mechanism it would be nice to avoid too much flicker
on the display.
*/
// In order to avoid dynamic memory allocation for channel data
// a fixed size array has to be declared here.  
#if SCREEN_WIDTH > SCREEN_HEIGHT
    #define CHANNEL_BUFFER_SIZE SCREEN_WIDTH
#else
    #define CHANNEL_BUFFER_SIZE SCREEN_HEIGHT
#endif

class channel
{
public:
    channel(uint16_t Colour);
    void draw(display & dsp);
    void insert(uint16_t value);
private:
    uint32_t offset;
    uint32_t scale;
    uint16_t Buffer[CHANNEL_BUFFER_SIZE];
    uint32_t BufferInputIndex;
    uint16_t Colour;
};
#endif