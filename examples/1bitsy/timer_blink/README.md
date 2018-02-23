# README

This is a simple example of how to use the stm32f4's hardware counters
to provide a periodic interrupt. This was created to investigate how
the counters worked, while trying to figure out something better than
a software delay counter.

It's intended for the 1BitSquared 1BitSy STM32F415 eval board.

When run, it will blink the LED on and off with a 1Hz cadence. This blink
is driven by the TIM2 counter firing off an interrupt once per second,
and using software to toggle the GPIO. The main loop sets everything up
then goes into a dead loop.

## Board connections

*none required*
