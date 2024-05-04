/* xsh_echo.c - xsh_echo */

#if 0
#include <xinu.h>
#include <stdio.h>
#include <elf.h>

typedef struct {
    uint8_t app_name[16];
    uint32_t size;
    uint32_t entry;
    uint8_t bin[];
}tinf_t;

#define FROM_ADDR_1  0x800c000

shellcmd xsh_bin(int nargs, char *args[])
{



	//program_img point;

    //point.img =(void*)FROM_ADDR_1;
    int (*p)(int,char **);
    tinf_t *tinf=(tinf_t*)FROM_ADDR_1;

    printf("\nname: %s\n", tinf->app_name);
    printf("size: %d\n", tinf->size);
    printf("entry: %x\n", tinf->entry);
    
    uint8 *memory = (uint8 *)malloc(tinf->size);

    memcpy(memory,tinf->bin,tinf->size);

     
    //hexdump(tinf->bin,tinf->size,TRUE);

    p = &memory[tinf->entry] |1;

    int re = p(nargs-2,&args[2]);


   // exec_img ximg;

    //int r = load_elf(&point, &ximg);
    

   // int (*p)(void)= (int *)ximg.entry;

    //int re = p();
    //printf("result %d\n", re);
    //free(ximg.start);

    /*int (*p)(int,char **);
    FIL file;
    uint32 lsize,l;
    void *memory;
    FRESULT fr;
    program_img *ximg;
    fr = f_stat(args[1], NULL);
    if(fr != FR_OK){
      printf("FILE NOT FOUND\n");
      return SYSERR;
    }

    if (f_open(&file, args[1], FA_OPEN_ALWAYS | FA_READ | FA_WRITE)==FR_OK) {
        lsize = f_size(&file);

        printf("size %d\n", lsize );
        memory = (void *)malloc(lsize+1);

        if(memory==NULL){
            panic("error malloc\n");
            exit();
        }

        f_read(&file, memory, lsize,&l );
        f_close(&file);

        int pid = getpid();
        struct procent * prptr = &proctab[pid];
        prptr->elf = TRUE;


        ximg->img=memory;
        prptr->img = memory;
        p = (uint32)(ximg->img) | 0x1;

        printf("%x\n", (uint32)(ximg->img));
        int re = p(nargs-2,&args[2]);
    }else printf("error file\n");
*/




	return 0;
}
#endif