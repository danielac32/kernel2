

#include <xinu.h>
#include <string.h>
#include <stdlib.h>
#include <elf.h>
#include <stdarg.h>


extern	void	start(void);	/* Start of Xinu code			*/
extern	void	*_end;		/* End of Xinu code			*/
extern	void testmem(void);
 


uint32 sys_time;
task_t *active_task;
queue_t ready_queue;
queue_t sleep_queue;
task_t task[MAX_NUMBER_OF_TASKS];
uint8 preempt;
uint8 ntask;
extern sem_t sem_fat32;
//sem_t test;


int  shell(int argc,char *argv[]);




void listdirectory(char *path)
{
    FL_DIR dirstat;
    //FL_LOCK(&_fs);
   // uint32 q=disable();
    printf("\r\nDirectory %s\r\n", path);

    if (syscall_opendir(path, &dirstat))
    {
        struct fs_dir_ent dirent;

        while (syscall_readdir(&dirstat, &dirent) == 0)
        {
            if (dirent.is_dir)
            {
                printf("%s <DIR>\r\n", dirent.filename);
            }
            else
            {
                printf("%s [%d bytes]\r\n", dirent.filename, dirent.size);
            }
        }

        syscall_closedir(&dirstat);
    }
     //FL_UNLOCK(&_fs);
   // restore(q);
}

void starting(){


	//syscall_attach_locks(sem_wait(&sem_fat32),sem_signal(&sem_fat32));
    if (syscall_sd_init() < 0){
	    printf("ERROR: Cannot init SD card\n");
	    return -1;
	}

	// Initialise File IO Library
	syscall_fs_init();

	// Attach media access functions to library
	
	if (syscall_attach_media(sd_readsector, sd_writesector) != FAT_INIT_OK)
	{
	    printf("ERROR: Failed to init file system\n");
	    return -1;
	}

	listdirectory("/");

	printf("loading shell...\n");
	ready(create((int *)shell,STACK_SIZE*2,0,"shell",0)); 
	kill(getpid());
	/*if (syscall_sd_init() < 0){
	    printf("ERROR: Cannot init SD card\n");
	    return -1;
	}

	// Initialise File IO Library
	syscall_fs_init();

	// Attach media access functions to library
	
	if (syscall_attach_media(sd_readsector, sd_writesector) != FAT_INIT_OK)
	{
	    printf("ERROR: Failed to init file system\n");
	    return -1;
	}
	// List the root directory
	syscall_listdirectory("/");
	//fl_listdirectory("/");*/
	
    //while(1){
    //	PEND_SV();
    //}
}


void nullprocess(){
	while(1)PEND_SV();
}

 

void	nulluser()
{	

init_sys();	 
uartinit();
init_mem();


//mount_fs();


sys_time = 0;
active_task = NULL;
for (int i = 0; i < MAX_NUMBER_OF_TASKS; ++i){
     task[i].state = PR_FREE;
}

preempt=QUANTUM;
ntask=0;
queue_init(&ready_queue);
queue_init(&sleep_queue);
//sem_init(&test,0);

/*if (sd_init() < 0){
    printf("ERROR: Cannot init SD card\n");
    return -1;
}

// Initialise File IO Library
fl_init();

// Attach media access functions to library
if (fl_attach_media(sd_readsector, sd_writesector) != FAT_INIT_OK)
{
    printf("ERROR: Failed to init file system\n");
    return -1;
}

// List the root directory
//syscall_listdirectory("/");
fl_listdirectory("/");*/




#if TASK_MODE == 1
ready(create((void *)nullprocess,512,1,"null",0)); 
ready(create((void *)starting,STACK_SIZE*3,1,"null",0)); 
//ready(create((int *)shell,STACK_SIZE*3,0,"shell",0)); 
enable();
init_systick(50,1);
#elif TASK_MODE == 2
int pid = create((void *)nullprocess,STACK_SIZE,1,"null",0);
//ready(pid);
task_t *prptr = &task[pid];
prptr->state = PR_CURR;


enable();
init_systick(50,1);


asm volatile ("mov r0, %0\n" : : "r" (prptr->sp));
asm volatile ("msr psp, r0");
asm volatile ("ldmia r0!, {r4-r11} ");
asm volatile ("msr psp, r0");
asm volatile ("mov r0, #2");
asm volatile ("msr control, r0");
asm volatile ("isb");
nullprocess();


PEND_SV(); 

#endif

for(;;);
}
 
 
