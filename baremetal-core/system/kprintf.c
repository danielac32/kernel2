/* kprintf.c -  kputc, kgetc, kprintf */
/* Kprintf uses a direct method to write to serial
 * Used for debugging purposes. Should in normal cases
 * use printf that does a syscall
 * */

#include <xinu.h>
#include <stdarg.h>

/*------------------------------------------------------------------------
 * kputc - use polled I/O to write a character to the console serial line
 *------------------------------------------------------------------------
 */
uint32 kputc(
	  byte	c			/* character to write		*/
	)
{
	uint32 mask;
	volatile struct uart_csreg * uptr = (struct uart_csreg *)0x40011400;
	mask = disable();

	if (c == '\n') {
              while(!(uptr->sr & UART_TC));
	     uptr->dr = 0x0D; // return line
	    //USART6->DR = 0x0D;
        //while(!(USART6->SR &  USART_SR_TXE));
      	}
              while(!(uptr->sr & UART_TC));
	          uptr->dr = c;
        //USART6->DR = c;
        //while(!(USART6->SR &  USART_SR_TXE));


	restore(mask);
	return OK;
}

/*------------------------------------------------------------------------
 * kgetc - use polled I/O to read a character from the console serial line
 *------------------------------------------------------------------------
 */
uint32 kgetc(void)
{
	// Not used
}

extern	void	_doprnt(char *, va_list, int (*)(int));

/*------------------------------------------------------------------------
 * kprintf  -  use polled I/O to print formatted output on the console
 *------------------------------------------------------------------------
 */
uint32 kprintf(char *fmt, ...)
{
    va_list ap;
    //sem_wait(&uart_tx);
    va_start(ap, fmt);
    _doprnt(fmt, ap, (int (*)(int))kputc);
    va_end(ap);
    //sem_signal(&uart_tx);
    return OK;
}



int32 printf(const char *fmt, ...){
   va_list ap;
    sem_wait(&uart_tx);
    va_start(ap, fmt);
    _doprnt(fmt, ap, kputch);
    va_end(ap);
    sem_signal(&uart_tx);
}