/* xsh_echo.c - xsh_echo */

#include <xinu.h>
#include <stdio.h>
#include <elf.h>

//#define FROM_ADDR_1  0x800c000
//#define FROM_ADDR_2  0x8010000


typedef struct arg
{
    //char *arg1[];
};


void dummy(char *s){
    while(*s){
        *s--;
    }
}
shellcmd xsh_elf(int nargs, char *args[])
{
    exec_img ximg;
    program_img point;
    int (*p)(int,char **);
    FIL file;
    uint32 lsize,l;
    uint8 *memory;
    FRESULT fr;
  

    for (int i = 0; i < nargs; ++i){
        //kprintf("%s\n",args[i] );
        dummy(args[i]);
    }

    fr = f_stat(args[1], NULL);
    if(fr != FR_OK){
      printf("FILE NOT FOUND\n");
      return SYSERR;
    }


    if (f_open(&file, args[1], FA_OPEN_ALWAYS | FA_READ | FA_WRITE)==FR_OK) {
        lsize = f_size(&file);

        printf("size %d\n", lsize );
        memory = (uint8 *)__syscall(XINU_MALLOC,lsize);
       
        if(memory==NULL){
            panic("error malloc\n");
            exit();
        }

        f_read(&file, memory, lsize,&l );
        f_close(&file);
        point.img = (void*)memory;
        int r = load_elf(&point, &ximg);
        if(r!=0)panic("error load elf\n");
        __syscall(XINU_FREE,memory);
        int pid = getpid();
        struct procent * prptr = &proctab[pid];
        prptr->elf = TRUE;
        prptr->img = ximg.start;

        p = (int *)ximg.entry;
        
        p(nargs-2,&args[2]);
    }else printf("error file\n");
    

    return 0;
}



/* xsh_echo.c - xsh_echo */
#if 0
#include <xinu.h>
#include <stdio.h>
#include <elf.h>

#define FROM_ADDR_1  0x800c000
#define FROM_ADDR_2  0x8010000



shellcmd xsh_elf(int nargs, char *args[])
{
    exec_img ximg;
    program_img point;
    int32   msg;
    int32   tmparg;

    int (*p)(int,char *);

    if(!strncmp(args[1],"1",1)){
       point.img = (void*)FROM_ADDR_1;
    }else if(!strncmp(args[1],"2",1)){
       point.img = (void*)FROM_ADDR_2;
    }
   

    int pid = getpid();
    int r = load_elf(&point, &ximg);
    //int pid =create((void *)ximg.entry, 4096, INITPRIO, "load", 1,"hola");
    struct procent * prptr = &proctab[pid];
    prptr->elf = TRUE;
    prptr->img = ximg.start;

    //if ((pid == SYSERR) ||(addargs(pid, nargs, args, nargs-2, args, &tmparg) == SYSERR) ) {
    //   printf("error\n");
    //   return;
    //}

    //msg = recvclr();
    //resume(pid);
    
    p = (int *)ximg.entry;
    int re = p(nargs-2,&args[2]);
    //free(ximg.start);
    
    //program_img prg1 = { .img = (void*)FROM_ADDR_1 };
    

    /*program_img point;

    point.img =(void*)FROM_ADDR_1;


    exec_img ximg;

    int r = load_elf(&point, &ximg);
    int (*p)(void)= (int *)ximg.entry;

    int re = p();
    printf("result %d\n", re);
    free(ximg.start);*/
    return 0;
}
#endif