#include "systick.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

static volatile uint32_t milliseconds;

void sys_tick_handler(void)
{
    milliseconds++;
}

void systick_init(void)
{
    /* clock rate / 1000 to get 1mS interrupt rate */
    systick_set_reload(168000);
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_counter_enable();
    /* this done last */
    systick_interrupt_enable();
}

uint32_t time_millis(void)
{
    return milliseconds;
}

void delay(uint32_t millis)
{
    uint32_t wake = milliseconds + millis;
    while ((int32_t)wake - (int32_t)milliseconds > 0)
        continue;
}
