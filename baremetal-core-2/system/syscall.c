#include <xinu.h>


/*
int syscall0(int code)
{
	int result;

	asm volatile("ldr r0, %0" : : "m" (code));
	asm volatile("svc #0");
	asm volatile("str r0, %0" : "=m" (result));
	
	return result;
}

 int syscall1(int code, int arg1)
{
	int result;

	asm volatile("ldr r0, %0" : : "m" (code));
	asm volatile("ldr r1, %0" : : "m" (arg1));
	asm volatile("svc #1");
	asm volatile("str r0, %0" : "=m" (result));
	
	return result;
}

int syscall2(int code, int arg1, int arg2)
{
	int result;

	asm volatile("ldr r0, %0" : : "m" (code));
	asm volatile("ldr r1, %0" : : "m" (arg1));
	asm volatile("ldr r2, %0" : : "m" (arg2));
	asm volatile("svc 0");
	asm volatile("str r0, %0" : "=m" (result));
	
	return result;
}

int syscall3(int code, int arg1, int arg2, int arg3)
{
	int result;

	asm volatile("ldr r0, %0" : : "m" (code));
	asm volatile("ldr r1, %0" : : "m" (arg1));
	asm volatile("ldr r2, %0" : : "m" (arg2));
	asm volatile("ldr r3, %0" : : "m" (arg3));
	asm volatile("svc #0");
	asm volatile("str r0, %0" : "=m" (result));
	
	return result;
}

int syscall4(int code, int arg1, int arg2, int arg3, int arg4)
{
	int result;

	asm volatile("ldr r0, %0" : : "m" (code));
	asm volatile("ldr r1, %0" : : "m" (arg1));
	asm volatile("ldr r2, %0" : : "m" (arg2));
	asm volatile("ldr r3, %0" : : "m" (arg3));
	asm volatile("ldr r4, %0" : : "m" (arg4));
	asm volatile("svc #0");
	asm volatile("str r0, %0" : "=m" (result));
	
	return result;
}*/

int syscall_putc(int c) {
   __syscall(XINU_PUTC, c);
   return OK;
}
int syscall_getc() {
   return __syscall(XINU_GETC);
}

int syscall_sema_init(void *sem,int value){
	//sem_init(sem,value);
	return __syscall(XINU_SEMA_INIT,sem,value);
}
int syscall_sema_signal(void *sem){
    //sem_signal(sem);
    return __syscall(XINU_SEMA_SIGNAL,sem);
}
int syscall_sema_wait(void *sem){
	//sem_wait(sem);
    return __syscall(XINU_SEMA_WAIT,sem);
}
int syscall_ready(int pid){
    return __syscall(XINU_READY,pid);
}

void* syscall_malloc(int size){
    return (void *)__syscall(XINU_MALLOC,size);
}

int syscall_free(void* p){
    return __syscall(XINU_FREE,p);
}
int syscall_pin_mode(void* p,int pin,int cfg){
    return __syscall(XINU_PIN_MODE,p,pin,cfg);
}
int syscall_pin_set(void* p,int pin,int state){
    return __syscall(XINU_PIN_SET,p,pin,state);
}
int syscall_pin_get(void* p,int pin){
    return __syscall(XINU_PIN_GET,p,pin);
}
int syscall_kill(int pid){
    return __syscall(XINU_KILL,pid);
}
int syscall_sleep(int delay){
    return __syscall(XINU_SLEEP,delay);
}
int syscall_create(int (*procaddr)(),int ssize,int priority,char *name){
	return __syscall(XINU_CREATE,procaddr,ssize, priority,name);
}
int syscall_getpid(){
	return __syscall(XINU_GETPID);
}
//int syscall_read() {
  // return __syscall(XINU_READ);
//}

void *syscall_struct_task(int pid){
return (void *)__syscall(XINU_STRUCT_TASK,pid);
}

int syscall_num_task(){
	return __syscall(XINU_NUM_TASK);
}



int syscall_sd_init(void){
	return __syscall(XINU_SD_INIT);
}
void  syscall_fs_init(void){
	 __syscall(XINU_FS_INIT);
}


int   syscall_attach_media(void *w, void *s){
	return __syscall(XINU_ATTACH_MEDIA,w,s);
}

void   syscall_attach_locks(void *rd, void *wr){
	 __syscall(XINU_ATTACH_LOCKS,rd,wr);
}


void* syscall_fopen(char *path,char *modifiers){
	return (void *) __syscall(XINU_FOPEN,path,modifiers);
}

int   syscall_fseek(void *file , long offset , int origin ){
	return __syscall(XINU_FSEEK,file,offset,origin);
}

int   syscall_fread(void * data, int size, int count, void *file ){
	return __syscall(XINU_FREAD,data,size,file);
}

void  syscall_fclose(void *file){
	__syscall(XINU_FCLOSE,file);
}
long  syscall_ftell(void *f){
   return __syscall(XINU_FTELL,f);
}

int   syscall_feof(void *f){
return __syscall(XINU_FEOF,f);
}
int   syscall_remove(char * filename){
return __syscall(XINU_REMOVE,filename);
}
void* syscall_opendir(char* path, void *dir){
return (void*)__syscall(XINU_OPENDIR,path,dir);
}
int  syscall_readdir(void *dirls, void *entry){
return __syscall(XINU_READDIR,dirls,entry);
}
int  syscall_closedir(void* dir){
return __syscall(XINU_CLOSEDIR,dir);
}
int   syscall_createdirectory(char *path){
	return __syscall(XINU_CREATEDIRECTORY,path);
}
int   syscall_is_dir(char *path){
	return __syscall(XINU_IS_DIR,path);
}


int   syscall_fflush(void *file){
	return __syscall(XINU_FFLUSH,file);
}
int   syscall_fgetc(void *file){
	return __syscall(XINU_FGETC,file);
}
char *  syscall_fgets(char *s, int n, void *f){
	return (char *)__syscall(XINU_FGETCS,s,n,f);
}
int   syscall_fputc(int c, void *file){
	return __syscall(XINU_FPUTC,c,file);
}
int   syscall_fputs(char * str, void *file){
	return __syscall(XINU_FPUTCS,str,file);
}
int   syscall_fwrite(void * data, int size, int count, void *file ){
	return __syscall(XINU_FWRITE,data,size,file);
}
int   syscall_fgetpos(void *file , uint32 * position){
	return __syscall(XINU_FGETPOS,file,position);
}

//void  syscall_listdirectory(char *path){
	//__syscall(XINU_LIST_DIR,path);
//}

/*
void* syscall_fopen(char *path,char *modifiers){

}
void  syscall_fclose(void *file){

}
int   syscall_fflush(void *file){

}
int   syscall_fgetc(void *file){

}
char *  syscall_fgets(char *s, int n, void *f){

}
int   syscall_fputc(int c, void *file){

}
int   syscall_fputs(char * str, void *file){

}
int   syscall_fwrite(void * data, int size, int count, void *file ){

}
int   syscall_fread(void * data, int size, int count, void *file ){

}
int   syscall_fseek(void *file , long offset , int origin ){

}
int   syscall_fgetpos(void *file , uint32 * position){

}
long  syscall_ftell(void *f){

}
int   syscall_feof(void *f){

}
int   syscall_remove(char * filename){

}
void* syscall_opendir(char* path, FL_DIR *dir){

}
int  syscall_readdir(FL_DIR *dirls, fl_dirent *entry){

}
int  syscall_closedir(FL_DIR* dir){

}

// Extensions
void  syscall_listdirectory(char *path){

}
int   syscall_createdirectory(char *path){

}
int   syscall_is_dir(char *path){

}
int   syscall_format(uint32 volume_sectors,char *name){

}
int   syscall_attach_media(void *rd, void *wr){

}*/


void  svccall_handler_c2(uint32 *sp){
//void __attribute__((naked)) svccall_handler_c(uint32 *sp){
//int svccall_handler_c2(uint32 *sp){
SYS_ENTRY();//uint32 mask=disable();
//uint32 svc_number = ((char *)sp[6])[-2];

/*uint32 * sp;
	asm volatile (
	"tst lr, #4" "\n\t"
	"ite eq" "\n\t"
	"mrseq %0, msp" "\n\t"
	"mrsne %0, psp" "\n\t"
	//"push {lr}" "\n\t"
	"push {r0-r11}" : "=r" (sp));
uint32 svc_arg1 = sp[0];*/


    //uint32 svc_number = ((char *)sp[6])[-2];
	uint32 svc_arg1 = sp[0];
	//uint32 svc_arg2 = sp[1];
    //task_t *p;
    //kprintf("%d %d %d\n",svc_number,svc_arg1,svc_arg2);
    //sem_wait(&sem_syscall);
    switch(svc_arg1){

    	/*case XINU_NULLPROCESS:
			p = (task_t *) sp[1]; 
	  		asm volatile ("mov r0, %0" : : "r" (p->sp));
        	asm volatile ("ldmia r0!, {r4-r11} ");
         	asm volatile ("msr psp, r0");
         	asm volatile ("ldr lr, =0xFFFFFFFD");
            asm volatile ("bx lr");
		break;*/
		//case XINU_LIST_DIR:
		//	{
				//fl_listdirectory(sp[0]);
		//		printf("%s\n",sp[1] );
		//	}
		//break;
		case XINU_FOPEN:
			sp[0]=fopen(sp[1],sp[2]);
		break;
		case XINU_FSEEK:
			sp[0]=fseek(sp[1], sp[2], sp[3]);
		break;
		case XINU_FREAD:
			sp[0]=fread(sp[1],sp[2],1, sp[3]); 
		break;
		case XINU_FCLOSE:
			fclose(sp[1]); 
		break;
		case XINU_FTELL:
            sp[0]=ftell(sp[1]);
		break;
		case XINU_FEOF:
			sp[0]=feof(sp[1]);
		break;
		case XINU_REMOVE:
			sp[0]=remove(sp[1]);
		break;
		case XINU_OPENDIR:
			sp[0]=fl_opendir(sp[1],sp[2]);
		break;
		case XINU_READDIR:
			sp[0]=fl_readdir(sp[1], sp[2]);
		break;
		case XINU_CREATEDIRECTORY:
			sp[0]=mkdir(sp[1]);
		break;
		case XINU_IS_DIR:
			sp[0]=fl_is_dir(sp[1]);
		break;
		case XINU_CLOSEDIR:
			sp[0]=fl_closedir(sp[1]);
		break;

		case XINU_FFLUSH:
			sp[0]=fflush(sp[1]);
		break;
		case XINU_FGETC:
			sp[0]=fgetc(sp[1]);
		break;
		case XINU_FGETCS:
			sp[0]=fgets(sp[1], sp[2],sp[3]);
		break;
		case XINU_FPUTC:
			sp[0]=fputc(sp[1],sp[2]);
		break;
		case XINU_FPUTCS:
			sp[0]=fputs(sp[1], sp[2]);
		break;
		case XINU_FWRITE:
			sp[0]=fwrite(sp[1], sp[2], 1, sp[3] );
		break;
		case XINU_FGETPOS:
			sp[0]=fgetpos(sp[1] , sp[2]);
		break;
		case XINU_ATTACH_LOCKS:
			fl_attach_locks(sp[1],sp[2]);
		break;
		case XINU_ATTACH_MEDIA:
		     sp[0]=fl_attach_media((fn_diskio_read*)sp[1], (fn_diskio_write*)sp[2]);
		break;
    	case XINU_SD_INIT:
    		sp[0]=sd_init();
    	break;
    	case XINU_FS_INIT:
    		fl_init();
    	break;
    	case XINU_STRUCT_TASK:
    		sp[0]= &task[sp[1]];
    	break;
    	case XINU_NUM_TASK:
    		sp[0]=MAX_NUMBER_OF_TASKS;
    	break;
    	case XINU_GETPID:
    		{
    			#if TASK_MODE == 1
    			sp[0]=getpid();
    			#endif
    		}
    	break;
    	case XINU_KMALLOC_CHUNK:
	    	#if MALLOC_MODE == 1
	    		sp[0]=head;
	    	#endif
    	break;
    	case XINU_READY:
            #if TASK_MODE == 1
    		ready((uint32)sp[1]);
    		#endif
    	break;
    	case XINU_CREATE:
    		{
    			#if TASK_MODE == 1
    			sp[0] = create(sp[1],sp[2], sp[3],sp[4],0);
    			#endif
    		}
    	break;
    	 case XINU_KILL:
    	{
    		#if TASK_MODE == 1
    		kill((uint32)sp[1]);
    		#endif
    	}
	    break;
	    case XINU_FREE:
			{
				free(sp[1]);
			}
		break;
		case XINU_MALLOC:
			{
				sp[0] = malloc((uint32)sp[1]);	
			}
		break;
    	case XINU_PIN_MODE://pin mode
		{
		 	GPIO_TypeDef *p;
			p = (GPIO_TypeDef *)sp[1];
	     	hw_cfg_pin(p,sp[2],sp[3]);//hw_cfg_pin((GPIO_TypeDef*)args[1],    args[2],     args[3]);
		 }
		break;
		case XINU_PIN_SET://pin set
			 {
			 	GPIO_TypeDef *p;
				p = (GPIO_TypeDef *)sp[1];
				hw_set_pin(p, sp[2],sp[3]);
				//kprintf("pin set %d %d\n", sp[2],sp[3]);
			 }
		break;
		case XINU_PIN_GET://pin mode
		 {
		 	GPIO_TypeDef *p;
		    p = (GPIO_TypeDef *)sp[1];
		 	sp[0] = hw_get_pin(p,sp[2]);
		 	//kprintf("pin get %d %d\n",sp[2],ret);
		 }
	    break;
		case XINU_SLEEP:
		{
			#if TASK_MODE == 1
			sleep((uint32)sp[1]);
			#endif
		}
	    break;
	    case XINU_GETC:
		{
		  sp[0]=kgetch();
		}
		break;
		case XINU_PUTC:
	         kputch(sp[1]);
		break;
		case XINU_SEMA_INIT:
			sem_init(sp[1],(uint32)sp[0]);
		break;
		case XINU_SEMA_SIGNAL:
			sem_signal((uint32)sp[1]);
		break;
        case XINU_SEMA_WAIT:
        	sem_wait(sp[1]);
        break;

		default:
		  kprintf("error call %d",svc_arg1);

    }
    //sem_signal(&sem_syscall);
    SYS_EXIT();//restore(mask);
}

