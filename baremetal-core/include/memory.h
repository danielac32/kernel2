/* memory.h - roundmb, truncmb, freestk */

#define MAXADDR		0x20010000		/* 64kB SRAM */
#define HANDLERSTACK	1024			/* Size reserved for stack in Handler mode */


/* Added by linker */

extern	unsigned int	text;			/* Start of text segment	*/
extern	unsigned int	etext;			/* End of text segment		*/
extern	unsigned int	data;			/* Start of data segment	*/
extern	unsigned int	edata;			/* End of data segment		*/
extern	unsigned int	bss;			/* Start of bss segment		*/
extern	unsigned int	ebss;			/* End of bss segment		*/
extern	unsigned int	end;			/* End of program		*/


/*
#define RDSR        5
#define WRSR        1
#define READ        3
#define WRITE       2

extern void write_sram(uint32 address, uint8 data_byte);
extern uint8 read_sram(uint32 address);
*/




#define UNIT      4u // smallest allocation unit (4 bytes)
typedef uint32 moff_t;
#define ALIGN(to, x)  ((x+(to)-1) & ~((to)-1))


#define KMALLOC_START  (void*)&end
#define KMALLOC_LENGTH (MAXADDR) - (size_t)(&end)





void malloc_init(void *start, int length);
void malloc_debug();
int malloc_test();

#define KUNIT sizeof(struct kmalloc_chunk)

#define KMALLOC_STATE_FREE 0xa1a1a1a1
#define KMALLOC_STATE_USED 0xbfbfbfbf

struct kmalloc_chunk {
	int state;
	int length;
	struct kmalloc_chunk *next;
	struct kmalloc_chunk *prev;
};

extern struct kmalloc_chunk *head;








