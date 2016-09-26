#include "led.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#define LED_PORT GPIOA
#define LED_PIN  GPIO8
#define RCC_LED_PORT RCC_GPIOA

void LED_init(void)
{
    rcc_periph_clock_enable(RCC_LED_PORT);
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
}

bool LED_is_on(void)
{
    return gpio_get(LED_PORT, LED_PIN) ? true : false;
}

void set_LED(bool on_off)
{
    if (on_off)
        gpio_set(LED_PORT, LED_PIN);
    else
        gpio_clear(LED_PORT, LED_PIN);
}

void toggle_LED(void)
{
    set_LED(!LED_is_on());
}
