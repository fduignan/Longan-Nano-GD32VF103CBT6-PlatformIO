// simple two-channel analogue oscilloscope for the longan-nano RISC-V board (with display)
// The user can increase and decrease the sampling rate (and pause the display)
// The scope loads the input source a bit so it would benifit from a good analogue front-end.
// Not intended to be a "real" scope - just a demonstrator
// Triggering is not implemented either :)
#include <stdio.h>
#include "SysTick.h"
#include "led.h"
#include "display.h"
#include "spi.h"
#include "channel.h"
extern "C" {
#include "gd32vf103.h"
}
SysTick stk;
led LED;
display Display;
spi SPI;
channel ch0(RGBToWord(0xff,0xff,0));
channel ch1(RGBToWord(0xff,0xf,0xf));
uint32_t SampleRate=200000;
// Will use ADC0 and ADC1 for simultaneous sampling of both channels
// Sampling will be triggered by Timer 2 TRGO update event.  
// End-Of-Conversion signal will be polled to see when data is ready
// When data is ready, it will be passed on to the channel object
// for display.
/* Supported sample rates:
   500kHz, 200kHz, 100kHz, 10kHz, 1kHz, 100Hz, 10Hz.

*/
void setSampleRate(uint32_t SampleRate)
{
  uint16_t PrescalerValue;
  uint16_t ReloadValue;
  switch (SampleRate) {
    case 500000:{
      PrescalerValue = 0;
      ReloadValue = 109; 
      break;
    }    
    case 200000:{
      PrescalerValue = 0;
      ReloadValue = 540;
      break;
    }
    case 100000:{
      PrescalerValue = 0;
      ReloadValue = 1080;
      break;
    }
    case 10000:{
      PrescalerValue = 9;
      ReloadValue = 1080;
      break;
    }
    case 1000:{
      PrescalerValue = 99;
      ReloadValue = 1080;
      break;
    }
    case 100:{
      PrescalerValue = 999;
      ReloadValue = 1080;
      break;
    }
    case 10:{
      PrescalerValue = 9999;
      ReloadValue = 1080;
      break;
    }            
    default :{ // 1 Hz sampling is default
      PrescalerValue = 9999;
      ReloadValue = 10800;
    }
  }
  TIMER_PSC(TIMER2) = PrescalerValue;
  TIMER_CAR(TIMER2) = ReloadValue;
}
void increaseSampleRate()
{
  switch (SampleRate)
  {
    case 10: SampleRate = 100; break;
    case 100: SampleRate = 1000; break;
    case 1000: SampleRate = 10000; break;
    case 10000: SampleRate = 100000; break;
    case 100000: SampleRate = 200000; break;
    case 200000: SampleRate = 500000; break;
  }
  setSampleRate(SampleRate);
}
void decreaseSampleRate()
{
 switch (SampleRate)
  {
    case 100: SampleRate = 10; break;
    case 1000: SampleRate = 100; break;
    case 10000: SampleRate = 1000; break;
    case 100000: SampleRate = 10000; break;
    case 200000: SampleRate = 100000; break;
    case 500000: SampleRate = 200000; break;
  }
  setSampleRate(SampleRate);

}
void initADC()
{
  // Turn on the various peripheral clocks
  rcu_periph_clock_enable(RCU_GPIOA); 
  rcu_periph_clock_enable(RCU_TIMER2);
  rcu_periph_clock_enable(RCU_ADC0);
  rcu_periph_clock_enable(RCU_ADC1);
  // ADC has a maximum clock frequency of 14MHz.  
  // System clock speed is 108MHz will divide 
  // this down by 8 to give a frequency of
  // 108/8 = 13.5MHz
  rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);

  // Set the timer going
  timer_parameter_struct timer_parameters;

  // Timer input clock frequency is 108MHz.  Will
  // divide this down by 54 to give a base clock
  // of 2Mhz.  This will define the maxim
  timer_deinit(TIMER2);
  /* initialize TIMER init parameter struct */
  timer_struct_para_init(&timer_parameters);
  /* TIMER0 configuration */
  timer_parameters.prescaler         = 10-1; // maximum 65535
  timer_parameters.alignedmode       = TIMER_COUNTER_EDGE;
  timer_parameters.counterdirection  = TIMER_COUNTER_UP;
  timer_parameters.period            = 10800;  // Maximum 65535
  timer_parameters.clockdivision     = TIMER_CKDIV_DIV1;
  // Now have to configure Channel 0 to pulse the ADC
  timer_oc_parameter_struct output_compare_parameters;
  timer_channel_output_struct_para_init(&output_compare_parameters);
  timer_master_output_trigger_source_select(TIMER2,TIMER_TRI_OUT_SRC_UPDATE);
  timer_init(TIMER2, &timer_parameters);
  timer_enable(TIMER2);

  // ADC Configuration : Pins are marked A0 and A3 on Sipeed Longan Nano board
  gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0); 
  gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_3); 


  /* reset ADC */
  adc_deinit(ADC0);
  adc_deinit(ADC1);
  /* ADC mode config */
  adc_mode_config(ADC_DAUL_REGULAL_PARALLEL); 
  /* ADC continous function enable */
  adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE); 
  adc_special_function_config(ADC1, ADC_SCAN_MODE, ENABLE);   

  /* ADC data alignment config */
  adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
  adc_data_alignment_config(ADC1, ADC_DATAALIGN_RIGHT);  
  /* ADC channel length config */
  adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 2);
  adc_channel_length_config(ADC1, ADC_REGULAR_CHANNEL, 2);

  // Going for 10 bit resolution to yield 1MHz sampling
  // Total clock cycles per sample = 10.5 + 1.5 = 12 clocks
  // Clock speed = 13.5MHz so Sample period = 889ns
  adc_resolution_config(ADC0,ADC_RESOLUTION_10B);
  adc_resolution_config(ADC1,ADC_RESOLUTION_10B);

  // Configure fastest sample & hold time -> implies low impedance signal source
  adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_0, ADC_SAMPLETIME_1POINT5);
  adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_3, ADC_SAMPLETIME_1POINT5);
  adc_regular_channel_config(ADC1, 0, ADC_CHANNEL_3, ADC_SAMPLETIME_1POINT5);
  adc_regular_channel_config(ADC1, 1, ADC_CHANNEL_0, ADC_SAMPLETIME_1POINT5);

  /* ADC trigger config */
  adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_T2_TRGO);
  adc_external_trigger_source_config(ADC1, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_NONE);
  /* ADC external trigger enable */
  adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);
  adc_external_trigger_config(ADC1, ADC_REGULAR_CHANNEL, ENABLE);    

  /* Configure word length */
  adc_resolution_config(ADC0,ADC_RESOLUTION_12B);
  adc_resolution_config(ADC1,ADC_RESOLUTION_12B);

  /* enable ADC interface */
  adc_enable(ADC0);
  /* ADC calibration and reset calibration */
  adc_calibration_enable(ADC0);
  /* enable ADC interface */    
  adc_enable(ADC1);
  stk.delay(1);    
  /* ADC calibration and reset calibration */    
  adc_calibration_enable(ADC1);    
  stk.delay(1);    
}
void initUART()
{
 /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);
    /* connect port to USARTx_Tx */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);


    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 9600U);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);
}
int main()
{
    int ch0Data,ch1Data;
    int index = 0;

    // Initialize the buttons for the user interface
    gpio_init(GPIOB, GPIO_MODE_IPU,GPIO_OSPEED_2MHZ, GPIO_PIN_10);
    gpio_init(GPIOB, GPIO_MODE_IPU,GPIO_OSPEED_2MHZ, GPIO_PIN_11);

    SPI.begin();
    LED.begin();
    stk.begin(1000);
    Display.begin(&SPI);
    Display.exchangeXY();  // This puts 0,0 at bottom left and swaps X,Y
                           // making the screen 160 wide by 80 high
    Display.fillRectangle(0,0,160,80,RGBToWord(0x0,0x0,0));
    Display.drawRectangle(0,0,159,79,RGBToWord(0xff,0xff,0xff));
    initADC();
    initUART();
    setSampleRate(500000);
    
    while(1)
    {
        /*stk.delay(500);
        LED.blue_on();
        stk.delay(500);
        LED.blue_off();*/
        if (adc_flag_get(ADC0,ADC_FLAG_EOC)==SET)
        {
          LED.blue_on();
          ch0Data = (adc_regular_data_read(ADC0));

          ch1Data = (adc_regular_data_read(ADC1));
          
          ch0.insert(ch0Data/64);
          ch1.insert(ch1Data/64);          
          LED.blue_off();
          index++;
          if (index >= 160)
          {
            index = 0;
            Display.fillRectangle(1,1,158,78,RGBToWord(0x0,0x0,0));
            Display.drawRectangle(0,0,159,79,RGBToWord(0xff,0xff,0xff));
            ch0.draw(Display);
            ch1.draw(Display);
            //printf("%d,%d\n",ch0Data,ch1Data);
          }
          if (gpio_input_bit_get(GPIOB,GPIO_PIN_10))
          {
              LED.green_off();
              
          }
          else
          {
              LED.green_on();
              increaseSampleRate();
              printf("SampleRate = %d\n",SampleRate);
              while(gpio_input_bit_get(GPIOB,GPIO_PIN_10)==0);
          }
          if (gpio_input_bit_get(GPIOB,GPIO_PIN_11))
          {
              LED.red_off();
              
          }
          else
          {
              LED.red_on();
              decreaseSampleRate();
              printf("SampleRate = %d\n",SampleRate);
              while(gpio_input_bit_get(GPIOB,GPIO_PIN_11)==0);
          }
        }
        
    }
}