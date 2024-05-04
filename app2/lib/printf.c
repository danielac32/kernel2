/* printf.c - printf*/

#include <kernel.h>
#include <stdio.h>
#include <stdarg.h>
#include <syscall.h>
#include <stdlib.h>

extern void _fdoprnt(char *, va_list, int (*)(int, char), int);

 __attribute__((naked))
void xinu_syscall(int nr, int arg1, int arg2, int arg3) 
{
    asm volatile("push {r0-r11}");
    asm volatile("push {lr}");
    asm volatile("svc 1"); 
    asm volatile("pop {lr}");
    asm volatile("pop {r0-r11}");
    asm volatile("bx lr");
}

static int xinu_putc(int dev, char c) {
    xinu_syscall(XINU_PUTC, dev, c, 0 );
    return 0;
}

int _printf(
      const char        *fmt,
      ...
    )
{
    va_list ap;

    va_start(ap, fmt);
    _fdoprnt((char *)fmt, ap, xinu_putc, stdout);
    va_end(ap);

    return 0;
}


 