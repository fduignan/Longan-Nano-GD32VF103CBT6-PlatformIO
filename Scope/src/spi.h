#ifndef __spi_h
#define __spi_h
class spi {

public:
    spi() {};
    void begin();
    uint8_t writeData8(uint8_t data);
    uint16_t writeData16(uint16_t data);
};
#endif