#ifndef __led_h
#define __led_h
extern "C" {
    #include <gd32vf103.h>
}
class led {
public:
    led() {};
    void begin() {
    /* enable the led clock */
        rcu_periph_clock_enable(RCU_GPIOC);
        rcu_periph_clock_enable(RCU_GPIOA);

        /* configure led GPIO port */ 
        gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
        gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
        gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

        GPIO_BOP(GPIOC) = GPIO_PIN_13;
        GPIO_BOP(GPIOA) = GPIO_PIN_1;
        GPIO_BOP(GPIOA) = GPIO_PIN_2;
    }
    /*!
    Turn on the red part of the RGB LED
    by clearing PC13
    */
    void red_on()
    {
        GPIO_BC(GPIOC) = GPIO_PIN_13;
    }
    /*!
        Turn off the red part of the RGB LED
        by setting PC13
    */
    void red_off()
    {
        GPIO_BOP(GPIOC) = GPIO_PIN_13;
    }
    /*!
        Turn on the blue part of the RGB LED
        by clearing PA1
    */
    void green_on()
    {
        GPIO_BC(GPIOA) = GPIO_PIN_1;
    }
    /*!
        Turn off the green part of the RGB LED
        by setting PA1
    */
    void green_off()
    {
        GPIO_BOP(GPIOA) = GPIO_PIN_1;
    }
    /*!
        Turn on the blue part of the RGB LED
        by clearing PA2
    */
    void blue_on()
    {
        GPIO_BC(GPIOA) = GPIO_PIN_2;
    }
    /*!
        Turn off the blue part of the RGB LED
        by setting PA2
    */
    void blue_off()
    {
        GPIO_BOP(GPIOA) = GPIO_PIN_2;
    }

};
#endif