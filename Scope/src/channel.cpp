#include "channel.h"
#include "display.h"
channel::channel(uint16_t Colour)
{
    this->Colour = Colour;
    offset = 0;
    scale = 1;
    BufferInputIndex=0;
    for (uint32_t i = 0; i < CHANNEL_BUFFER_SIZE; i++)
    {
        Buffer[i]=0;
    }
}
void channel::draw(display & dsp)
{
    uint16_t YLocn;
    for (uint32_t i = 0; i < CHANNEL_BUFFER_SIZE; i++)
    {
        YLocn = /*dsp.getHeight()/2 +*/ offset + (Buffer[i] * scale);
        dsp.putPixel(i,YLocn,Colour);
    }
}
void channel::insert(uint16_t value)
{
    Buffer[BufferInputIndex] = value;
    BufferInputIndex = (BufferInputIndex + 1) % CHANNEL_BUFFER_SIZE;
}
