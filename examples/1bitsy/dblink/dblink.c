/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Damjan Marion <damjan.marion@gmail.com>
 * Copyright (C) 2011 Mark Panajotovic <marko@electrontube.org>
 * Copyright (C) 2013 Chuck McManis <cmcmanis@mcmanis.com>
 * Copyright (C) 2015-2016 Piotr Esden-Tempski <piotr@esden.net>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stddef.h>

#include "../common/button_boot.h"

/* This version derived from fancy blink */

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

/* monotonically increasing number of milliseconds from reset
 * overflows every 49 days if you're wondering
 */
volatile uint32_t system_millis;

typedef void systick_handler(uint32_t msec_time);

#define MAX_HANDLERS 4
static systick_handler *handlers[MAX_HANDLERS];
static size_t handler_count = 0;
static uint32_t trigger_count = 0;

/* Called when systick fires */
void sys_tick_handler(void)
{
	system_millis++;
        for (size_t i = 0; i < handler_count; i++)
            (*handlers[i])(system_millis);
}

/* sleep for delay milliseconds */
static void msleep(uint32_t delay)
{
	uint32_t wake = system_millis + delay;
	while (wake > system_millis);
}

/*
 * systick_setup(void)
 *
 * This function sets up the 1khz "system tick" count. The SYSTICK counter is a
 * standard feature of the Cortex-M series.
 */
static void systick_setup(void)
{
	/* clock rate / 1000 to get 1mS interrupt rate */
	systick_set_reload(168000);
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_counter_enable();
	/* this done last */
	systick_interrupt_enable();
}

/* Set STM32 system clock to 168 MHz. */
static void clock_setup(void)
{
	rcc_clock_setup_hse_3v3(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

	/* Enable GPIOA clock. */
	rcc_periph_clock_enable(RCC_GPIOA);
}

static void gpio_setup(void)
{
	/* Set GPIO13-14 (in GPIO port G) to 'output push-pull'. */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
			GPIO8);
}

static void register_systick_handler(systick_handler *h)
{
    if (handler_count < MAX_HANDLERS)
        handlers[handler_count++] = h;
}

static void trigger_blink(void)
{
    trigger_count = 100;
}

static void update_blink(uint32_t msec_time)
{
    // button press: amp = 1, phase = 0
    // button release: 
    

    bool on = gpio_get(GPIOC, GPIO1) ? false : true;
    if (trigger_count) {
        on = true;
        trigger_count--;
    }
    if (msec_time & 0x21)
        on = false;
    if (on)
        gpio_clear(GPIOA, GPIO8); // low is LED on.
    else
        gpio_set(GPIOA, GPIO8);
}

static void ping_setup(void)
{
    rcc_periph_clock_enable(RCC_GPIOC);

    /* Set PC1 to 'input open-drain'. */
    gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO1);

    register_systick_handler(update_blink);
}

int main(void)
{
	button_boot();

	clock_setup();
	gpio_setup();
	systick_setup();
        ping_setup();

	gpio_set(GPIOA, GPIO8);

	/* Blink the LEDs (PG13 and PG14) on the board. */
	while (1) {
            msleep(1450);
            trigger_blink();
	}

	return 0;
}
