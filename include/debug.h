/* debug.h */

#define DEBUG_ASCII 0x01
#define DEBUG_HEX   0x02

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