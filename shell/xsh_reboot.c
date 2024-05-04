/* xsh_echo.c - xsh_echo */

#include <xinu.h>
#include <stdio.h>


#define VARADDR 0x0800C000 // address that will hold the variable
#define VAR     0x12345670 // variable value


/*------------------------------------------------------------------------
 * xhs_echo - write argument strings to stdout
 *------------------------------------------------------------------------
 */
 
shellcmd xsh_reboot(int nargs, char *args[])
{


    char    buf[20];    /* Input line (large enough for */
    int32   len;
    char c;
    while(1){

        //int c = __syscall(XINU_GETC,0);
        //printf("%c\n", c);

       // len = __syscall(XINU_READ,0, &buf, sizeof(buf));//
       // printf("->>%d %s\n", len,buf);

        len=0;
        memset(buf,0,20);
        
        do{
            c =__syscall(XINU_GETC,0);
            buf[len]=c;
            len++;
        }while(c!='\n');

        buf[len] = '\0';
        printf("->>%d %s\n", len,buf);
     
    }



    //char *buff=malloc(200);
    
    //sprintf(buff,"hola como estas %d \n",888);
    
    //__syscall(XINU_TEST2,"hola como estas %d \n",888);
    //__syscall(XINU_TEST2,buff);
   	
   	//printf2("hola como estas %d \n",888);
    //printf("%s\n",buff );
    //free(buff);


    /*unlock_flash();
    erase_flash_sector(10);
    write_flash(0x08010000, 0xbebecafe);

    ///write_flash(VARADDR, 0xbebecafe);
        // do read write
    lock_flash();

    unlock_flash();

    uint32 r = read_flash(0x08010000);
    printf2("%08x\n", r);*/

   // uint32 r = syscall3(XINU_TEST2,0,0,0);
    //printf("%d\n", r);
	return 0;
}
