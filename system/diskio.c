#include <xinu.h>


 
 
/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

/* Definitions for MMC/SDC command */
#define CMD0  (0)     /* GO_IDLE_STATE */
#define CMD1  (1)     /* SEND_OP_COND (MMC) */
#define ACMD41  (0x80+41) /* SEND_OP_COND (SDC) */
#define CMD8  (8)     /* SEND_IF_COND */
#define CMD9  (9)     /* SEND_CSD */
#define CMD10 (10)    /* SEND_CID */
#define CMD12 (12)    /* STOP_TRANSMISSION */
#define ACMD13  (0x80+13) /* SD_STATUS (SDC) */
#define CMD16 (16)    /* SET_BLOCKLEN */
#define CMD17 (17)    /* READ_SINGLE_BLOCK */
#define CMD18 (18)    /* READ_MULTIPLE_BLOCK */
#define CMD23 (23)    /* SET_BLOCK_COUNT (MMC) */
#define ACMD23  (0x80+23) /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24 (24)    /* WRITE_BLOCK */
#define CMD25 (25)    /* WRITE_MULTIPLE_BLOCK */
#define CMD32 (32)    /* ERASE_ER_BLK_START */
#define CMD33 (33)    /* ERASE_ER_BLK_END */
#define CMD38 (38)    /* ERASE */
#define CMD55 (55)    /* APP_CMD */
#define CMD58 (58)    /* READ_OCR */


/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC    0x01    /* MMC ver 3 */
#define CT_SD1    0x02    /* SD ver 1 */
#define CT_SD2    0x04    /* SD ver 2 */
#define CT_SDC    (CT_SD1|CT_SD2) /* SD */
#define CT_BLOCK  0x08    /* Block addressing */


/* Definitions of physical drive number for each drive */
#define DEV_RAM   0 /* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC   1 /* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB   2 /* Example: Map USB MSD to physical drive 2 */


DSTATUS Stat = STA_NOINIT;  /* Disk status */


BYTE CardType;      /* Card type flags */




#define sd_tx(d) transfer(d)//putc(SPI1,d)//transfer2(d)
#define sd_rx()  transfer(0xff)//getc(SPI1)   //transfer2(0xff)


BYTE wait_ready (void)  /* 1:Ready, 0:Timeout */
{
  uint16_t tmr;


  for (tmr = 5000; tmr; tmr--) {  /* Wait for ready in timeout of 500ms */
    if (sd_rx() == 0xFF) break;
    DELAY(100);
  }

  return tmr ? 1 : 0;
}

void deselect (void)
{
   //__syscall(XINU_PIN_SET,GPIOx(GPIO_A),4, 1);//
   hw_set_pin(GPIOx(GPIO_A), 4, 1);//pin_set(pin_a4);//spi_cs(1);
  sd_rx();  /* Dummy clock (force DO hi-z for multiple slave SPI) */
}

BYTE select (void)  /* 1:Successful, 0:Timeout */
{
    //__syscall(XINU_PIN_SET,GPIOx(GPIO_A),4, 0);//
    hw_set_pin(GPIOx(GPIO_A), 4, 0);//pin_reset(pin_a4);//spi_cs(0);
   sd_rx();  /* Dummy clock (force DO enabled) */
   if (wait_ready()) return 1; /* Wait for card ready */

   deselect();
   return 0; /* Timeout */
}



BYTE rcvr_datablock (
  BYTE *buff,     /* Data buffer to store received data */
  uint16_t btr      /* BYTE count (must be multiple of 4) */
)
{
  BYTE token;
  uint16_t tmr;
  //uint16_t block=512;
 // uint32 mask=disable();
  for (tmr = 2000; tmr; tmr--) {  /* Wait for data packet in timeout of 200ms */
    token = sd_rx();
    if (token != 0xFF) break;
    DELAY(100);
  }
  if (token != 0xFE) {
     ////restore(mask);
     return 0;  /* If not valid data token, retutn with error */
  }
  do
    *buff++ = sd_rx();    /* Receive the data block into buffer */
  while (--btr);
  sd_rx();          /* Discard CRC */
  sd_rx();

  ////restore(mask);
  return 1;         /* Return with success */
}



BYTE xmit_datablock (
  BYTE *buff, /* 512 BYTE data block to be transmitted */
  BYTE token      /* Data/Stop token */
)
{
  BYTE resp;
  uint16_t i;
  //uint16_t block=512;
 // uint32 mask=disable();

  if (!wait_ready()) {
    ////restore(mask);
    return 0;
  }

  sd_tx(token);     /* Xmit data token */
  if (token != 0xFD) {  /* Is data token */
    i = 512;
    do
      sd_tx(*buff++);       /* Xmit the data block  the MMC */
    while (--i);
    sd_rx();            /* CRC (Dummy) */
    sd_rx();
    resp = sd_rx();         /* Reveive data response */
    if ((resp & 0x1F) != 0x05){    /* If not accepted, return with error */
      ////restore(mask);
      return 0;
    }
  }

  ////restore(mask);
  return 1;
}




BYTE __send_cmd (    /* Returns R1 resp (bit7==1:Send failed) */
  BYTE cmd,   /* Command index */
  uint32_t arg   /* Argument */
)
{
  BYTE n, res;


  /* Select the card and wait for ready except to stop multiple block read */
  if (cmd != CMD12) {
    deselect();
    if (!select()) return 0xFF;
  }

  /* Send command packet */
  sd_tx(0x40 | cmd);        /* Start + Command index */
  sd_tx((BYTE)(arg >> 24));   /* Argument[31..24] */
  sd_tx((BYTE)(arg >> 16));   /* Argument[23..16] */
  sd_tx((BYTE)(arg >> 8));    /* Argument[15..8] */
  sd_tx((BYTE)arg);       /* Argument[7..0] */
  n = 0x01;           /* Dummy CRC + Stop */
  if (cmd == CMD0) n = 0x95;    /* Valid CRC for CMD0(0) + Stop */
  if (cmd == CMD8) n = 0x87;    /* Valid CRC for CMD8(0x1AA) Stop */
  sd_tx(n);

  /* Receive command response */
  if (cmd == CMD12) sd_rx();    /* Skip a stuff BYTE when stop reading */
  n = 10;               /* Wait for a valid response in timeout of 10 attempts */
  do
    res = sd_rx();
  while ((res & 0x80) && --n);

  return res;     /* Return with the response value */
}



BYTE send_cmd (    /* Returns R1 resp (bit7==1:Send failed) */
  BYTE cmd,   /* Command index */
  uint32_t arg   /* Argument */
)
{
  BYTE res;

  if (cmd & 0x80) { /* ACMD<n> is the command sequense of CMD55-CMD<n> */
      cmd &= 0x7F;
      res = __send_cmd(CMD55, 0);
      if (res > 1) return res;
  }
  return __send_cmd(cmd, arg);  /* Return with the response value */
}

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/
 

DWORD get_fattime(void)
{
  return  ((2013UL-1980) << 25)       // Year = 2006
      | (8UL << 21)       // Month = Feb
      | (30UL << 16)        // Day = 9
      | (13U << 11)       // Hour = 22
      | (00U << 5)        // Min = 30
      | (00U >> 1)        // Sec = 0
      ;
}


DSTATUS disk_initialize (
  BYTE pdrv   /* Physical drive nmuber (0) */
)
{
 BYTE n, cmd, ty, ocr[4];
  uint16_t tmr;
 

  if (pdrv){
      
    kprintf("error disk\n");
     return STA_NOINIT;    /* Supports only single drive */
  }

  if (Stat & STA_NODISK){
     
     kprintf("error disk\n");
     return Stat; /* No card in the socket */
  }
    


  //spi_init();

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  //RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
 // __syscall(XINU_PIN_MODE,GPIOx(GPIO_A),5,GPIOCFG_MODE_ALT | GPIO_AF5_SPI1 | GPIOCFG_OSPEED_VHIGH | GPIOCFG_OTYPE_PUPD | GPIOCFG_PUPD_PUP);//
 hw_cfg_pin(GPIOx(GPIO_A),   5/*sck_pin*/,    GPIOCFG_MODE_ALT | GPIO_AF5_SPI1 | GPIOCFG_OSPEED_VHIGH | GPIOCFG_OTYPE_PUPD | GPIOCFG_PUPD_PUP);
 // __syscall(XINU_PIN_MODE,GPIOx(GPIO_A),7,GPIOCFG_MODE_ALT | GPIO_AF5_SPI1 | GPIOCFG_OSPEED_VHIGH | GPIOCFG_OTYPE_PUPD | GPIOCFG_PUPD_PUP);//
 hw_cfg_pin(GPIOx(GPIO_A),   6/*miso_pin*/,   GPIOCFG_MODE_ALT | GPIO_AF5_SPI1 | GPIOCFG_OSPEED_VHIGH | GPIOCFG_OTYPE_PUPD | GPIOCFG_PUPD_PUP);
 // __syscall(XINU_PIN_MODE,GPIOx(GPIO_A),6,GPIOCFG_MODE_ALT | GPIO_AF5_SPI1 | GPIOCFG_OSPEED_VHIGH | GPIOCFG_OTYPE_PUPD | GPIOCFG_PUPD_PUP);//
 hw_cfg_pin(GPIOx(GPIO_A),   7/*mosi_pin*/,   GPIOCFG_MODE_ALT | GPIO_AF5_SPI1 | GPIOCFG_OSPEED_VHIGH | GPIOCFG_OTYPE_PUPD | GPIOCFG_PUPD_PUP);
  

  //hw_cfg_pin(GPIOx(GPIO_A),  5,   GPIOCFG_MODE_OUT | GPIOCFG_OSPEED_VHIGH  | GPIOCFG_OTYPE_PUPD | GPIOCFG_PUPD_PUP);
  //hw_cfg_pin(GPIOx(GPIO_A),  7,  GPIOCFG_MODE_OUT | GPIOCFG_OSPEED_VHIGH  | GPIOCFG_OTYPE_PUPD | GPIOCFG_PUPD_PUP);
  //hw_cfg_pin(GPIOx(GPIO_A),  6,  GPIOCFG_MODE_INP | GPIOCFG_OSPEED_VHIGH  | GPIOCFG_OTYPE_PUPD | GPIOCFG_PUPD_PUP);

  //__syscall(XINU_PIN_MODE,GPIOx(GPIO_A),4,GPIOCFG_MODE_OUT | GPIOCFG_OSPEED_VHIGH  | GPIOCFG_OTYPE_PUPD | GPIOCFG_PUPD_PUP);//
  hw_cfg_pin(GPIOx(GPIO_A), 4/*cs*/,GPIOCFG_MODE_OUT | GPIOCFG_OSPEED_VHIGH  | GPIOCFG_OTYPE_PUPD | GPIOCFG_PUPD_PUP);
  //__syscall(XINU_PIN_SET,GPIOx(GPIO_A),4, 1);//
  hw_set_pin(GPIOx(GPIO_A), 4, 1);
 
 
  //SPI1->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE | SPI_INIT_CLK_DIV | SPI_CR1_MSTR;

    
  select();

  DELAY(100);

  for (n = 10; n; n--) sd_rx(); /* 80 dummy clocks */

  ty = 0;
  if (send_cmd(CMD0, 0) == 1) {     /* Enter Idle state */
    if (send_cmd(CMD8, 0x1AA) == 1) { /* SDv2? */
      for (n = 0; n < 4; n++) ocr[n] = sd_rx(); /* Get trailing return value of R7 resp */
      if (ocr[2] == 0x01 && ocr[3] == 0xAA) {   /* The card can work at vdd range of 2.7-3.6V */
        for (tmr = 1000; tmr; tmr--) {      /* Wait for leaving idle state (ACMD41 with HCS bit) */
          if (send_cmd(ACMD41, 1UL << 30) == 0) break;
          DELAY(10);
        }
        if (tmr && send_cmd(CMD58, 0) == 0) {   /* Check CCS bit in the OCR */
          for (n = 0; n < 4; n++) ocr[n] = sd_rx();
          ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;  /* SDv2 */
        }
      }
    } else {              /* SDv1 or MMCv3 */
      if (send_cmd(ACMD41, 0) <= 1)   {
        ty = CT_SD1; cmd = ACMD41;  /* SDv1 */
      } else {
        ty = CT_MMC; cmd = CMD1;  /* MMCv3 */
      }
      for (tmr = 1000; tmr; tmr--) {      /* Wait for leaving idle state */
        if (send_cmd(cmd, 0) == 0) break;
        DELAY(10);
      }
      if (!tmr || send_cmd(CMD16, 512) != 0)  /* Set R/W block length to 512 */
        ty = 0;
    }
  }
  CardType = ty;
  deselect();

  if (ty) {     /* Initialization succeded */
    Stat &= ~STA_NOINIT;    /* Clear STA_NOINIT */
     
  }
  

  struct spi_csreg *spiptr;
  spiptr = (struct spi_csreg *)SPI1_BASE;
  spiptr->cr1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE | SPI_TRANSFER_CLK_DIV | SPI_CR1_MSTR;



  return Stat;
}


/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
  BYTE pdrv   /* Physical drive nmuber (0) */
)
{
   if (pdrv) return STA_NOINIT;  /* Supports only single drive */
  return Stat;
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

/*
DRESULT read_flash(BYTE* buff, DWORD sector,BYTE len){
send_cmd(CMD17, sector);
rcvr_datablock(buff, len);
return RES_OK;
}


DRESULT write_flash(BYTE* buff, DWORD sector,BYTE len){
send_cmd(CMD24, sector);
xmit_datablock((BYTE*)buff, len);
}
 */

DRESULT disk_read (
  BYTE pdrv,      /* Physical drive nmuber (0) */
  BYTE *buff,     /* Pointer to the data buffer to store read data */
  DWORD sector,   /* Start sector number (LBA) */
  UINT count      /* Sector count (1..128) */
)
{
 //BYTE cmd;


  if (pdrv || !count) return RES_PARERR;
  if (Stat & STA_NOINIT) return RES_NOTRDY;
   
  
  if (!(CardType & CT_BLOCK)) sector *= 512;  /* Convert to BYTE address if needed */

  if (count == 1) { /* Single sector read */
    if ((send_cmd(CMD17, sector) == 0)  /* READ_SINGLE_BLOCK */
      && rcvr_datablock(buff, 512)) {
      count = 0;
    }
  }
  else {        /* Multiple sector read */
    if (send_cmd(CMD18, sector) == 0) { /* READ_MULTIPLE_BLOCK */
      do {
        if (!rcvr_datablock(buff, 512)) break;
        buff += 512;
      } while (--count);
      send_cmd(CMD12, 0);       /* STOP_TRANSMISSION */
    }
  }
  deselect();
 
  return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
  BYTE pdrv,      /* Physical drive nmuber (0) */
  const BYTE *buff, /* Pointer to the data to be written */
  DWORD sector,   /* Start sector number (LBA) */
  UINT count      /* Sector count (1..128) */
)
{
  if (pdrv || !count) return RES_PARERR;
  if (Stat & STA_NOINIT) return RES_NOTRDY;
  if (Stat & STA_PROTECT) return RES_WRPRT;
 
  if (!(CardType & CT_BLOCK)) sector *= 512;  /* Convert to BYTE address if needed */

  if (count == 1) { /* Single block write */
    if ((send_cmd(CMD24, sector) == 0)  /* WRITE_BLOCK */
      && xmit_datablock((BYTE*)buff, 0xFE))
      count = 0;
  }
  else {        /* Multiple block write */
    if (CardType & CT_SDC) send_cmd(ACMD23, count);
    if (send_cmd(CMD25, sector) == 0) { /* WRITE_MULTIPLE_BLOCK */
      do {
        if (!xmit_datablock((BYTE*)buff, 0xFC)) break;
        buff += 512;
      } while (--count);
      if (!xmit_datablock(0, 0xFD)) count = 1; /* STOP_TRAN token */
    }
  }
  deselect();
 
  return count ? RES_ERROR : RES_OK;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
  BYTE drv,   /* Physical drive number (0) */
  BYTE cmd,   /* Control command code */
  void *buff    /* Pointer to the conrtol data */
)
{
  DRESULT res;
  BYTE n, csd[16];
  DWORD *dp, st, ed, csize;


  if (drv) return RES_PARERR;         /* Check parameter */
  if (Stat & STA_NOINIT) return RES_NOTRDY; /* Check if drive is ready */

  res = RES_ERROR;

  switch (cmd) {
  case CTRL_SYNC :    /* Wait for end of internal write process of the drive */
    if (select()) res = RES_OK;
    break;

  case GET_SECTOR_COUNT : /* Get drive capacity in unit of sector (DWORD) */
    if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
      if ((csd[0] >> 6) == 1) { /* SDC ver 2.00 */
        csize = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
        *(DWORD*)buff = csize << 10;
      } else {          /* SDC ver 1.XX or MMC ver 3 */
        n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
        csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
        *(DWORD*)buff = csize << (n - 9);
      }
      res = RES_OK;
    }
    break;

  case GET_BLOCK_SIZE : /* Get erase block size in unit of sector (DWORD) */
    if (CardType & CT_SD2) {  /* SDC ver 2.00 */
      if (send_cmd(ACMD13, 0) == 0) { /* Read SD status */
        sd_tx(0xFF);
        if (rcvr_datablock(csd, 16)) {        /* Read partial block */
          for (n = 64 - 16; n; n--) sd_tx(0xFF); /* Purge trailing data */
          *(DWORD*)buff = 16UL << (csd[10] >> 4);
          res = RES_OK;
        }
      }
    } else {          /* SDC ver 1.XX or MMC */
      if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {  /* Read CSD */
        if (CardType & CT_SD1) {  /* SDC ver 1.XX */
          *(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
        } else {          /* MMC */
          *(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
        }
        res = RES_OK;
      }
    }
    break;

  case CTRL_TRIM :  /* Erase a block of sectors (used when _USE_ERASE == 1) */
    if (!(CardType & CT_SDC)) break;        /* Check if the card is SDC */
    if (disk_ioctl(drv, MMC_GET_CSD, csd)) break; /* Get CSD */
    if (!(csd[0] >> 6) && !(csd[10] & 0x40)) break; /* Check if sector erase can be applied to the card */
    dp = buff; st = dp[0]; ed = dp[1];        /* Load sector block */
    if (!(CardType & CT_BLOCK)) {
      st *= 512; ed *= 512;
    }
    if (send_cmd(CMD32, st) == 0 && send_cmd(CMD33, ed) == 0 && send_cmd(CMD38, 0) == 0 && wait_ready()) /* Erase sector block */
      res = RES_OK; /* FatFs does not check result of this command */
    break;

  default:
    res = RES_PARERR;
  }

  deselect();

  return res;
}
#endif


