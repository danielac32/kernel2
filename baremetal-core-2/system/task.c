#include <xinu.h>



local int   newpid(void)
{
    uint32  i;          /* iterate through all processes*/
//  static  pid32 nextpid = 1;  /* position in table to try or  */
     int nextpid = 0;  /* position in table to try or  */

    for (i = 0; i < MAX_NUMBER_OF_TASKS; i++) {
        if(task[i].state == PR_FREE) {
            return nextpid++;
        } else {
            nextpid++;
        }
    }
    return (int) -1;
}

int getpid(){
  return active_task->pid;
}


void load_context(void* psp) {
  asm volatile(
  "mov sp, %0   \n"
  "pop  {r4-r11} \n"
  "bx %1      \n" : : "r" (psp), "r" (0xFFFFFFF9)
  );
}

void* save_context() {
  void *reg;
  asm volatile(
  "push {r4-r11} \n"
  "mov %0, sp \n" : "=r" (reg)
  );
  return reg;
}


static void *cmcm_push_context(void) {
  void *psp;

  // copies registers to the PSP stack
  // additional registers are pushed in hardware
  asm volatile("MRS %0, msp\n"
          "STMDB %0!, {r4-r11}\n"
          "MSR msp, %0\n"
          : "=r" (psp));

  return psp;
}

static void cmcm_pop_context(void *psp) {
  // loads registers with contents of the PSP stack
  // additional registers are popped in hardware
  asm volatile("LDMFD %0!, {r4-r11}\n"
          "MSR msp, %0\n"
          : : "r" (psp));
  asm volatile("bx %0" : : "r" (0xFFFFFFF9));
}


#if TASK_MODE == 1
  void __attribute__((naked)) pendsv_handler()  {

      void* reg;
      reg = cmcm_push_context();//
      //reg = save_context();
      active_task->sp = reg;

      if (active_task != 0 && active_task->state == PR_CURR) {
          queue_push(&ready_queue, active_task);
      }
      active_task = queue_pop(&ready_queue);
      preempt = QUANTUM;
      cmcm_pop_context(active_task->sp);//
      //load_context(active_task->sp);
  }
#elif TASK_MODE == 2


void __attribute__((naked)) pendsv_handler()  {
void* reg;
asm volatile("mrs r0, psp");
asm volatile("stmdb r0!, {r4-r11}");
asm volatile("mov %0, r0" : "=r" (reg));
//reg = save_context();
active_task->sp = reg;

if (active_task != 0 && active_task->state == PR_CURR) {
    queue_push(&ready_queue, active_task);
}
active_task = queue_pop(&ready_queue);
preempt = QUANTUM;

asm volatile ("mov r0, %0" : : "r" (active_task->sp));
    asm volatile("ldmia r0!, {r4-r11} ");
    asm volatile ("msr psp, r0");
    asm volatile (
                   "ldr r0, =0xFFFFFFFD" "\n\t"
                   "mov lr, r0" "\n\t"
                   "bx lr");
}
#endif


void SysTick_Handler(void){
  #if TASK_MODE == 1
    sys_time += 1;
    task_t *tmp = queue_peek(&sleep_queue);

    
    while (tmp) {
        if (sys_time >= tmp->param) { // si se cumplio el tiempo sleep
            tmp = queue_pop(&sleep_queue);//sacalo del queue sleep
            
            ready(tmp->pid);
           // tmp->state = PR_CURR;// cambia el status
           // queue_push(&ready_queue, tmp);
            tmp = queue_peek(&sleep_queue);
        } else {
            break;
        }

    }
    if((--preempt) == 0) {
        preempt = QUANTUM;
        PEND_SV(); 
    }
  #elif TASK_MODE == 2

  #endif
}



local void killer(){
      int pid = getpid();
      kill(pid);
}

uint32 *prepare_task_stack(uint32 *task_stack,uint32 size,void *entry, uint32 nargs, void *parg){
  task_stack += size - sizeof(context_t); /* End of task_stack, minus what we are about to push */
  context_t *ctx = (context_t*)task_stack;

  ctx->r0 = (uint32) nargs;
  ctx->r1 = (uint32) parg;
  ctx->lr = (uint32) killer;
  ctx->pc = (uint32) entry;
  ctx->psr = (uint32) 0x01000000; /* PSR Thumb bit */
  return task_stack;
}

int create(
      int       (*procaddr)(),  /* procedure address        */
      int   ssize,      /* stack size in bytes      */
      int       priority,   /* process priority > 0     */
      char      *name,      /* name (for debugging)     */
      int   nargs,      /* number of args that follow   */
      ...
    ){
va_list ap;
uint32 mask=disable();
int pid=newpid();//getNextPID();
task_t *prptr;

uint32 *saddr=(uint32*)malloc(ALIGN(4, ssize));

if(saddr==NULL){
   kprintf("create error malloc %s\n",name);
   while(1);
}
prptr = &task[pid];
prptr->elf=FALSE;
prptr->state = PR_SUSP;
prptr->prstkbase=saddr;
prptr->prstklen=ssize;
strcpy(prptr->name,name);

prptr->pid=pid;
ntask++;

va_start(ap, nargs);
for (int j=0; j < nargs; j++){
     prptr->parg[j] = (void *)va_arg(ap, long);
}
va_end(ap);
prptr->sp =  prepare_task_stack(saddr,ssize,procaddr,nargs,&prptr->parg[0]);
restore(mask);
//kprintf("pid: %d\n",pid );
return pid;
}


void ready(int pid){
task_t *prptr;
prptr =& task[pid];
prptr->state = PR_CURR;
queue_push(&ready_queue, prptr); 
}

void sleep(uint32 ticks) {
uint32 mask=disable();
  if (ticks) {
      active_task->state = PR_SLEEP;
      active_task->param = ticks*1000 +sys_time;//*1000;
      queue_pushsort(&sleep_queue, active_task);
  }
  restore(mask);
  PEND_SV();
//restore(mask);
}

void kill(int pid){

  uint32 mask=disable();


  task_t *prptr;
  prptr =& task[pid];

  free(prptr->prstkbase);
  
  if(prptr->elf == TRUE){
     free(prptr->img);//free(prptr->img);
  }


  switch(prptr->state){
        case PR_CURR:
             prptr->state=PR_FREE;
             restore(mask);
             PEND_SV();
        break;
        case PR_SLEEP:
             {
              task_t *tmp = queue_peek(&sleep_queue);
              while (tmp) {
                if (tmp->pid==prptr->pid) {
                    tmp = queue_pop(&sleep_queue);
                    tmp->state = PR_FREE;
                    kprintf("killed  %d--%s\n", pid,prptr->name);
                    break;
                }
              }
              restore(mask);
              PEND_SV();
             }
        break;
        default:
            prptr->state = PR_FREE;
  }
  restore(mask);
}


