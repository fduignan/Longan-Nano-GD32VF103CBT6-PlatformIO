extern "C" {
#include <gd32vf103.h>
#include <gd32vf103_spi.h>
}
#include "spi.h"
// SPI0 is wired to the LCD display on the LonganNano board.
void spi::begin()
{
   	rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_SPI0);
    spi_parameter_struct spi_init_struct;
    spi_struct_para_init(&spi_init_struct);
    /* SPI0 configuration 8-bit full duplex, software control of slave select, master more*/
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_8;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;    
    spi_init(SPI0, &spi_init_struct);

	//spi_crc_polynomial_set(SPI0,7);  /* what does this do? */
    spi_crc_off(SPI0);
	spi_enable(SPI0);
}
uint8_t spi::writeData8(uint8_t data)
{       
    spi_i2s_data_transmit(SPI0,data);                    
    while(spi_i2s_flag_get(SPI0,SPI_FLAG_TRANS)); // wait for transmit to complete
    return 0;
}
uint16_t spi::writeData16(uint16_t data) 
{
    spi_i2s_data_transmit(SPI0,data>>8);
    while(spi_i2s_flag_get(SPI0,SPI_FLAG_TRANS)); // wait for transmit to complete
    spi_i2s_data_transmit(SPI0,data & 0xff);
    while(spi_i2s_flag_get(SPI0,SPI_FLAG_TRANS)); // wait for transmit to complete
    return 0;
}