#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "funcs.h"

/* Set STM32 to 168 MHz. */
static void clock_setup(void)
{
    rcc_clock_setup_hse_3v3(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

    /* Enable GPIOA clock. */
    rcc_periph_clock_enable(RCC_GPIOA);
}

static void gpio_setup(void)
{
    /* Set GPIO8 (in GPIO port A) to 'output push-pull'. */
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT,
                    GPIO_PUPD_NONE, GPIO8);
}

// Set up so stdin and stdout go to USB, stderr goes to USART.

int main(void)
{
    int i;

    clock_setup();
    gpio_setup();

    gpio_set(GPIOA, GPIO8);

    zork_main();

    /* Blink the LED (PA8) on the board. */
    while (1) {
        /* Toggle LED. */
        gpio_toggle(GPIOA, GPIO8);
        for (i = 0; i < 6000000; i++) { /* Wait a bit. */
            __asm__("nop");
        }
    }

    return 0;
}

// http://tldp.org/HOWTO/Program-Library-HOWTO/miscellaneous.html#INIT-AND-FINI-OBSOLETE
extern void _fini(void);
extern void _fini(void)
{}
