#ifndef SYSTICK_included
#define SYSTICK_included

#include <stdint.h>

extern void systick_init(void);
extern uint32_t time_millis(void);
extern void delay(uint32_t millis);

#endif /* !SYSTICK_included */
