

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

sem_t test;

int  blink(int argc,char *argv[]);
int  blink2(int argc,char *argv[]);
int  blink3(int argc,char *argv[]);


void nullprocess(){
    while(1){

    }
}


void __puts(char *str){
    sem_wait(&test);
    printf("%s\n", str);
    sem_signal(&test);
}

int  blink(int argc,char *argv[]){
    //
    int u=0;
  


    for (int i = 0; i < argc; ++i)
    {
        printf("%s\n",argv[i] );
    }

 
    while(1){
         //__puts("blink1\n");


         printf("blink1\n");
         sleep(200);
         //if (++u==5){
           // sem_signal(&test);
         //}
    }
    return 0;
}

int blink2(int argc,char *argv[]){
    for (int i = 0; i < argc; ++i)
    {
       printf("%s\n",argv[i] );
    }

    while(1){
          

        printf("   blink2\n");
        //sem_wait(&test);
        //__puts("   blink2\n");
        sleep(200);

        
    }
    return 0;
}





int blink3(int argc,char *argv[]){
    for (int i = 0; i < argc; ++i)
    {
        printf("%s\n",argv[i] );
    }
    char command[128];

    char *pstate[]  = {     /* names for process states */
        "free ", "curr ", "ready", "recv ", "sleep", "susp ",
        "wait ", "rtime"};
    while(1){
         gets(command);
         if(!strncmp(command,"ps",2)){
             for (int i = 0; i < MAX_NUMBER_OF_TASKS; ++i){
                  task_t *t=&task[i];
                  if (t->state == PR_FREE) {  /* skip unused slots    */
                      continue;
                  }
                  printf("%d %s %s %d %08x\n",
                  i, t->name, pstate[(int)t->state],
                  t->prstklen,t->prstkbase);
            }
         }

         //printf("->%s\n",command );

        //kputch(kgetch());

        //kprintf("     blink3\n");
        //sleep(200);

        
    }
    return 0;
}


void	nulluser()
{	
	 
uartinit();
//meminit();
init_mem();
mount_fs();


sys_time = 0;
active_task = 0;
for (int i = 0; i < MAX_NUMBER_OF_TASKS; ++i){
     task[i].state = PR_FREE;
}

preempt=QUANTUM;
ntask=0;
queue_init(&ready_queue);
queue_init(&sleep_queue);
sem_init(&test,1);

enable();
ready(create((void *)nullprocess,STACK_SIZE,1,"null",0)); 
ready(create((int *)blink,STACK_SIZE,1,"blink",3,"elf", "shell.elf", "CONSOLE")); 
ready(create((int *)blink2,STACK_SIZE,1,"blink",1,"12345")); 
ready(create((int *)blink3,STACK_SIZE,1,"blink",0)); 




init_systick(50,1);

for(;;){

}

}
 
 
