/* stdio.h - definintions and constants for standard I/O functions */



#define DEBUG_ASCII 0x01
#define DEBUG_HEX   0x02

/* Definintion of standard input/ouput/error used with shell commands */
 


/* Prototypes for formatted output functions */

extern	int32	fprintf(int, char *, ...);
extern	int32	printf(const char *, ...);
extern	int32	sprintf(char *, char *, ...);

#define gets(buffer) gets_base(kgetch, buffer)


extern void    hexdump(
     void   *buffer,        /* Addresss of memory area  */
     uint32 length,         /* Length in bytes      */
     bool8  canon           /* Print in ASCII or hex    */
    );
 

extern void    hexdump2(
     void   *buffer,        /* Addresss of memory area  */
     uint32 length,         /* Length in bytes      */
     uint32 offset,
     bool8  canon           /* Print in ASCII or hex    */
    );