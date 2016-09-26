// C/POSIX headers
#include <stdio.h>
#include <stdlib.h>

// libopencm3 headers
#include <libopencm3/stm32/rcc.h>
// #include <libopencm3/stm32/gpio.h>
// #include <libopencm3/usb/usbd.h>

// application headers
#include "cdcacm.h"
#include "led.h"
#include "serial-stdio.h"
#include "systick.h"
#include "tty.h"

// static void cdcacm_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
// {
// 	(void)ep;

// 	char buf[64];
// 	int len = usbd_ep_read_packet(usbd_dev, 0x01, buf, 64);

// 	if (len) {
// 		gpio_toggle(GPIOA, GPIO8);
// 		while (usbd_ep_write_packet(usbd_dev, 0x82, buf, len) == 0);
// 	}
// }

// static void cdcacm_set_config(usbd_device *usbd_dev, uint16_t wValue)
// {
// 	(void)wValue;

// 	usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64,
// 			cdcacm_data_rx_cb);
// 	usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, NULL);
// 	usbd_ep_setup(usbd_dev, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);

// 	usbd_register_control_callback(
// 				usbd_dev,
// 				USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
// 				USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
// 				cdcacm_control_request);
// }

// int main(void)
// {
// #if 0
// 	usbd_device *usbd_dev;
// #endif

// 	rcc_clock_setup_hse_3v3(&rcc_hse_25mhz_3v3[RCC_CLOCK_3V3_168MHZ]);

// 	rcc_periph_clock_enable(RCC_GPIOA);

// 	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);

// #if 0
// 	// rcc_periph_clock_enable(RCC_OTGFS);

// 	// gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,
// 	// 		GPIO9 | GPIO11 | GPIO12);
// 	// gpio_set_af(GPIOA, GPIO_AF10, GPIO9 | GPIO11 | GPIO12);

// 	// usbd_dev = usbd_init(&otgfs_usb_driver, &dev, &config,
// 	// 		usb_strings, 3,
// 	// 		usbd_control_buffer, sizeof(usbd_control_buffer));

// 	// usbd_register_set_config_callback(usbd_dev, cdcacm_set_config);

// 	// while (1) {
// 	// 	usbd_poll(usbd_dev);
// 	// }
// #else
//         cdcacm_setup();
// #endif
// }

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

    // stdio
    serial_stdio_init(&my_tty);

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
