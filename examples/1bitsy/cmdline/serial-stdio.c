#include "serial-stdio.h"

#include <stdio.h>
#include <unistd.h>

#include "tty.h"

static ssize_t serial_read(void *cookie, char *buffer, size_t size)
{
    tty *tp = cookie;
    size_t n = 0;
    while (n < size) {
        int c = tty_read_char(tp);
        if (c == -1 && !n)
            continue;
        buffer[n++] = c;
        if (c == '\n')
            break;
    }
    return n;
}

static ssize_t serial_write(void *cookie, const char *buffer, size_t size)
{
    tty *tp = cookie;
    tty_write_chars(tp, buffer, size);
    return size;
}

void serial_stdio_init(tty *tp)
{
    static cookie_io_functions_t stdin_fns = {
        .read  = serial_read,
        .write = NULL,
        .seek  = NULL,
        .close = NULL,
    };
    stdin = fopencookie(tp, "r", stdin_fns);

    static cookie_io_functions_t stdout_fns = {
        .read  = NULL,
        .write = serial_write,
        .seek  = NULL,
        .close = NULL,
    };
    stdout = fopencookie(tp, "w", stdout_fns);
    int r = setvbuf(stdout, tp->write_buffer, _IOLBF, sizeof tp->write_buffer);

    static cookie_io_functions_t stderr_fns = {
        .read  = NULL,
        .write = serial_write,
        .seek  = NULL,
        .close = NULL,
    };
    stderr = fopencookie(tp, "w", stderr_fns);
    r = setvbuf(stderr, NULL, _IONBF, 0);
    printf("r = %d\n", r);
}
