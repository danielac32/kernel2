
#include <xinu.h>
//#include <string.h>
//#include <syscall.h>


extern	void	start(void);	/* Start of Xinu code			*/
extern	void	*_end;		/* End of Xinu code			*/

/* Function prototypes */

extern	void main(void);	/* Main is the first process created	*/
static	void sysinit(); 	/* Internal system initialization	*/
extern	void meminit(void);	/* Initializes the free memory list	*/
extern 	int32 initintc(void);
//extern int sys__call(int num,...);
//void startup(int, ...);		/* Process to finish startup tasks	*/


/* Declarations of major kernel variables */

struct	procent	proctab[NPROC];	/* Process table			*/
struct	sentry	semtab[NSEM];	/* Semaphore table			*/
struct	memblk	memlist;	/* List of free memory blocks		*/

/* Active system status */

int	prcount;		/* Total number of live processes	*/
pid32	currpid;		/* ID of currently executing process	*/
//bool start_null=false;
/* Control sequence to reset the console colors and cusor positiion	*/

#define	CONSOLE_RESET	" \033[0m\033[2J\033[;H"


/*unsigned char mem[]={
 0x80, 0xb5, 0x00, 0xaf, 0x4f, 0xf4, 0x64, 0x72, 0x0d, 0x21, 0x03, 0x20, 0x00, 0xf0, 0x1f, 0xf8, 
 0x01, 0x22, 0x0d, 0x21, 0x04, 0x20, 0x00, 0xf0, 0x1a, 0xf8, 0xc8, 0x21, 0x09, 0x20, 0x00, 0xf0, 
 0x16, 0xf8, 0x00, 0x22, 0x0d, 0x21, 0x04, 0x20, 0x00, 0xf0, 0x11, 0xf8, 0x64, 0x21, 0x09, 0x20, 
 0x00, 0xf0, 0x0d, 0xf8, 0xec, 0xe7, 0xef, 0xf3, 0x03, 0x80, 0x72, 0xb6, 0x70, 0x47, 0x80, 0xf3, 
 0x03, 0x88, 0x62, 0xb6, 0x70, 0x47, 0x62, 0xb6, 0x70, 0x47, 0x72, 0xb6, 0xfe, 0xe7, 0x00, 0xdf, 
 0xc0, 0x46, 0x70, 0x47, 0x64, 0xdf, 0x70, 0x47,                        };

*/


 
void nullprocess(void) {
	__syscall(XINU_TEST2,shell,4096/2, "shell");
	//__syscall(XINU_TEST2,mount_filesystem,4096/2, "FatFs");
	while(1);
	
}


void	nulluser()
{	
	struct	memblk	*memptr;	/* Ptr to memory block		*/
	uint32	free_mem;		/* Total amount of free memory	*/

	/* Initialize the system */
    
	sysinit();

	/* Output Xinu memory layout */
	free_mem = 0;
	for (memptr = memlist.mnext; memptr != NULL;
						memptr = memptr->mnext) {
		free_mem += memptr->mlength;
	}
	kprintf("%10d bytes of free memory.  Free list:\n", free_mem);
	for (memptr=memlist.mnext; memptr!=NULL;memptr = memptr->mnext) {
	    kprintf("           [0x%08X to 0x%08X]\n",
		(uint32)memptr, ((uint32)memptr) + memptr->mlength - 1);
	}

	kprintf("%10d bytes of Xinu code.\n",
		(uint32)&etext - (uint32)&text);
	kprintf("           [0x%08X to 0x%08X]\n",
		(uint32)&text, (uint32)&etext - 1);
	kprintf("%10d bytes of data.\n",
		(uint32)&ebss - (uint32)&data);
	kprintf("           [0x%08X to 0x%08X]\n\n",
		(uint32)&data, (uint32)&ebss - 1);



    FATFS FatFs;
    FRESULT res;
	DIR dirs;
	FILINFO Finfo;


	if ((res = f_mount(&FatFs, "", 1)) != FR_OK) {
	    kprintf("mount error! res=%d\n", res);
	    return;//return 1;
	}
	kprintf("mount successfully\n");

    char *p2 = malloc(100);
    strcpy(p2,"/");
	if ((res = f_opendir(&dirs,p2)) == FR_OK) {
	     for (;;){
	         res = f_readdir(&dirs, &Finfo);
	         if(res != FR_OK || Finfo.fname[0] == 0)break;
	         kprintf("%s %d\n", Finfo.fname,Finfo.fsize);
	     }
	 }else{
	    kprintf("error dir\n");
	 }
	free(p2);


    /* Initialize the Null process entry */	
	int pid = create((void *)nullprocess, 256, 10, "Null process", 0, NULL);
	struct procent * prptr = &proctab[pid];
	prptr->prstate = PR_CURR;
	/* Enable interrupts */
	enable();
	/* Initialize the real time clock */
	clkinit();
    __syscall(XINU_NULLPROCESS,prptr);
	for(;;);

}

/*------------------------------------------------------------------------
 *
 * sysinit  -  Initialize all Xinu data structures and devices
 *
 *------------------------------------------------------------------------
 */


 
static	void	sysinit()
{
	int32	i;
	struct	procent	*prptr;		/* Ptr to process table entry	*/
	struct	sentry	*semptr;	/* Ptr to semaphore table entry	*/
	/* Platform Specific Initialization */

	platinit();

	//kprintf(CONSOLE_RESET);
	for (int i = 0; i < 10; ++i)
	{
		kprintf("\n");
	}
	
	meminit();
	prcount = 0;

	Defer.ndefers = 0;

	/* Initialize process table entries free */

	for (i = 0; i < NPROC; i++) {
		prptr = &proctab[i];
		prptr->prstate = PR_FREE;
		prptr->prname[0] = NULLCH;
		prptr->prstkbase = NULL;
		prptr->prprio = 0;
	}

	for (i = 0; i < NSEM; i++) {
		semptr = &semtab[i];
		semptr->sstate = S_FREE;
		semptr->scount = 0;
		semptr->squeue = newqueue();
	}

	
	readylist = newqueue();

	
	for (i = 0; i < NDEVS; i++) {
		init(i);
	}
	return;
}

/*
void delay_ms(uint32 s)
{
    for(s; s>0; s--){
		// Reset the timer
		TIM3->EGR |= 0x0001;
		// Wait until timer reaches to 1000
		// It is 1000 becuase timer is running at 1 MHz and 1000 will
		//   generate 1 milli-second
 		while(TIM3->CNT < 1000);
	}
}
*/

void OTG_FS_IRQHandler(void)
{
  //USBD_OTG_ISR_Handler(&g_usb_dev);
}
