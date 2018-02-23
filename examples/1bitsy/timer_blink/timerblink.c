/*
 * This file is part of the 1bitsy-examples project.
 *
 * Copyright (C) 2018 Will Marone <wmarone@gmail.com>
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

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

/*

This is a simple example of how to use the stm32f4's hardware counters
to provide a periodic interrupt. This was created to investigate how
the counters worked, while trying to figure out something better than
a software delay counter.

*/

/* Simple LED setup */
static void setup_led(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);
    gpio_clear(GPIOA, GPIO8);
}

/* Configure the timer and the IRQs for TIM2, a general purpose 32-bit timer. */
static void setup_timer2(void)
{
    /* Enable TIM2 clock. */
    rcc_periph_clock_enable(RCC_TIM2);

    /* Enalbe TIM2 IRQ */
    nvic_enable_irq(NVIC_TIM2_IRQ);

    /* Reset the timer */
    timer_reset(TIM2);

    /* Set the timer mode (no divider, edge, and count up */
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

    /* Some notes on these two lines:
     *
     * - The 1BitSy is clocked at 168MHz with a 25MHz external oscillator
     * - Timer 2 is attached to APB1 for its clock
     * - APB1 is clocked at 42MHz, 1/4 of the primary clock.
     * - Timer 2 doubles this via the PLL, for an 84MHz internal tick
     * - Prescaler and period are <value> - 1
     * - A prescaler of 83 is a flat divide to 1MHz
     * - Combined with a period of 999999 gives you a 1Hz tick
     * - Play with the period and you can create arbitrary duration ticks from
     *   1usec to several seconds, an advantage of using a 32-bit counter.
     */
    timer_set_prescaler(TIM2, 83);
    timer_set_period(TIM2, 999999);

    /* Activate the update event (update happens whenever the period elapses) */
    timer_enable_update_event(TIM2);

    /* This seems redundant given nvic_enable_irq, but it explicitly enables
     * the update event to trigger NVIC_TIM2_IRQ. This is shared with all
     * TIM2 interrupts, and timer_get_flag must be checked if you enable other
     * events.
     */
    timer_enable_irq(TIM2, TIM_DIER_UIE);

    /* Go! */
    timer_enable_counter(TIM2);
}

/* tim2_isr is defined in include/libopencm3/stm32/f4/nvic.h and is set up in
 * lib/stm32/f4/vector_nvic.c
 *
 * It is declared weak, and points to blocking_handler by default. Simply
 * declaring it is all you need.
 */
void tim2_isr(void)
{
    /* Clear the flag to deassert the interrupt, otherwise the NVIC will just
     * drop us right back in the ISR.
     */
    timer_clear_flag(TIM2, TIM_SR_UIF);

    /* Blinky */
    gpio_toggle(GPIOA, GPIO8);
}

int main(void)
{
    rcc_clock_setup_hse_3v3(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

    setup_led();
    setup_timer2();

    while (1)
    {
        __asm__("NOP");            
    }
}
