#ifndef LED_included
#define LED_included

#include <stdbool.h>

extern void LED_init(void);
extern bool LED_is_on(void);
extern void set_LED(bool on_off);
extern void toggle_LED(void);

#endif /* !LED_included */
