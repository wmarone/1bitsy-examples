// C/POSIX headers
#include <stdio.h>

// libopencm3 headers
#include <libopencm3/stm32/rcc.h>

// application headers
#include "cdcacm.h"
#include "led.h"
#include "tty-stdio.h"
#include "systick.h"
#include "tty.h"

static tty my_tty;

static void usb_to_tty(const char *buf, size_t size)
{
    tty_receive_chars(&my_tty, buf, size);
}

static void tty_to_usb(tty *tp, const char *buf, size_t size)
{
    (void)tp;
    cdcacm_send_chars(buf, size);
}

static void setup(void)
{
    // rcc
    rcc_clock_setup_hse_3v3(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

    // systick
    systick_init();

    // LED
    LED_init();

    // tty
    tty_init(&my_tty);

    // USB CDC/ACM
    cdcacm_init();

    // TTY stdio
    tty_stdio_init(&my_tty);

    // connect TTY to USB.
    cdcacm_register_receive_callback(usb_to_tty);
    tty_register_send_callback(&my_tty, tty_to_usb);

    // wait for user to connect.
    cdcacm_open();
}

// Sample application just uses Standard I/O.
static void run(void)
{
    printf("Welcome to Pointlessness!\n");
    printf("You can type things.\n");
    while (1) {
        char line[20];          // short, easily overflowed.

        printf("> ");
        fflush(stdout);
        if (!fgets(line, sizeof line, stdin))
            fprintf(stderr, "error\n");
        else {
            printf("You said, \"");
            for (char *s = line; *s; s++)
                if (*s != '\n')
                    putchar(*s);
            printf("\"\n");
        }
    }
}

int main(void)
{
    setup();
    run();
}
