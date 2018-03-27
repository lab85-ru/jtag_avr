/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include <string.h>
#include <stdint.h>
#include "stm32f10x.h"
#include "main.h"
#include "ff.h"

#ifndef DEBUG_IOCTL
#define DEBUG_IOCTL 0
#endif

static volatile
DSTATUS Stat = STA_NOINIT;	/* Disk status */

FLASH_Status FLASHStatus;

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
        /* Initialization succeded */
	Stat &= ~STA_NOINIT;		/* Clear STA_NOINIT */
	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
        return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	        /* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..128) */
)
{
        memcpy(buff, (void const*)(FLASH_WRITE_START_ADDR + (FLASH_PAGE_SIZE * sector)), FLASH_PAGE_SIZE * count);
        return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	        /* Data to be written */
	DWORD sector,		        /* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..128) */
)
{
	//DRESULT res;
	//int result;
        int i;
        uint32_t flash_wr_adr, n;
        
        /* Unlock the Flash Bank1 Program Erase controller */
	FLASH_Unlock();

	/* Clear All pending flags */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
        
        if (DEBUG_IOCTL) printf_d("disk_write sector=%d,  count=%d\r\n",sector, count);

        //hex_out( (uint8_t*)buff, FLASH_PAGE_SIZE);

        for (i=0;i<count;i++){
                if (DEBUG_IOCTL) printf_d("ADDR start = %x\r\n",FLASH_WRITE_START_ADDR + ((sector + i) * FLASH_PAGE_SIZE));
                __disable_interrupt();
                FLASHStatus = FLASH_ErasePage( FLASH_WRITE_START_ADDR + ((sector + i) * FLASH_PAGE_SIZE) );
                __enable_interrupt();
                //hex_out( (uint8_t*)(FLASH_WRITE_START_ADDR + (sector * FLASH_PAGE_SIZE)), FLASH_PAGE_SIZE);
        }
        
        for (i=0;i<count;i++){
                n = 0;
		flash_wr_adr = FLASH_WRITE_START_ADDR + ((sector + i) * FLASH_PAGE_SIZE);
		while((n < FLASH_PAGE_SIZE) && (FLASHStatus == FLASH_COMPLETE)){
                  
                        __disable_interrupt();
                        FLASHStatus = FLASH_ProgramWord(flash_wr_adr, *(uint32_t*)&buff[ n ]);// write one word 4-bytes !!!!
                        __enable_interrupt();

                        flash_wr_adr += 4;
                        n += 4;
                }
                    
                //hex_out( (uint8_t*)(FLASH_WRITE_START_ADDR + (sector * FLASH_PAGE_SIZE)), FLASH_PAGE_SIZE);
        }
        return RES_OK;
        
//	return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
        
	
        //DRESULT res;
	//int result;
          
        if (DEBUG_IOCTL) 
                printf_d("disk_ioctl cmd=%d\r\n",cmd);
        
	switch (cmd) {
        case CTRL_SYNC:		/* Flush disk cache (for write functions) */
                return RES_OK;
        
        case GET_SECTOR_COUNT:	/* Get media size (for only f_mkfs()) */
                *(DWORD*)buff = (FLASH_WRITE_END_ADDR - FLASH_WRITE_START_ADDR) / FLASH_PAGE_SIZE;
                return RES_OK;

        case GET_SECTOR_SIZE:	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
                *(WORD*)buff = FLASH_PAGE_SIZE;
                return RES_OK;
          
        case GET_BLOCK_SIZE:		/* Get erase block size (for only f_mkfs()) */
                *(DWORD*)buff = 1;
                return RES_OK;
          
        case CTRL_ERASE_SECTOR:	/* Force erased a block of sectors (for only _USE_ERASE) */
                return RES_OK;
        default:
                return RES_OK;
        }
//	return RES_PARERR;
}
#endif

void print_result(FRESULT res)
{
        switch (res){
        case FR_OK:
                printf_d("(0) Succeeded.\r\n");
                break;
	case FR_DISK_ERR:
                printf_d("(1) A hard error occurred in the low level disk I/O layer.\r\n");
                break;
	case FR_INT_ERR:
                printf_d("(2) Assertion failed.\r\n");
                break;
	case FR_NOT_READY:
                printf_d("(3) The physical drive cannot work.\r\n");
                break;
	case FR_NO_FILE:
                printf_d("(4) Could not find the file.\r\n");
                break;
	case FR_NO_PATH:
                printf_d("(5) Could not find the path.\r\n");
                break;
	case FR_INVALID_NAME:
                printf_d("(6) The path name format is invalid.\r\n");
                break;
	case FR_DENIED:
                printf_d("(7) Access denied due to prohibited access or directory full.\r\n");
                break;
	case FR_EXIST:
                printf_d("(8) Access denied due to prohibited access.\r\n");
                break;
	case FR_INVALID_OBJECT:
                printf_d("(9) The file/directory object is invalid.\r\n");
                break;
	case FR_WRITE_PROTECTED:
                printf_d("(10) The physical drive is write protected.\r\n");
                break;
	case FR_INVALID_DRIVE:
                printf_d("(11) The logical drive number is invalid.\r\n");
                break;
	case FR_NOT_ENABLED:
                printf_d("(12) The volume has no work area.\r\n");
                break;
	case FR_NO_FILESYSTEM:
                printf_d("(13) There is no valid FAT volume.\r\n");
                break;
	case FR_MKFS_ABORTED:
                printf_d("(14) The f_mkfs() aborted due to any parameter error.\r\n");
                break;
	case FR_TIMEOUT:
                printf_d("(15) Could not get a grant to access the volume within defined period.\r\n");
                break;
	case FR_LOCKED:
                printf_d("(16) The operation is rejected according to the file sharing policy.\r\n");
                break;
	case FR_NOT_ENOUGH_CORE:
                printf_d("(17) LFN working buffer could not be allocated.\r\n");
                break;
	case FR_TOO_MANY_OPEN_FILES:
                printf_d("(18) Number of open files > _FS_SHARE.\r\n");
                break;
	case FR_INVALID_PARAMETER:
                printf_d("(19) Given parameter is invalid.\r\n");
                break;
        }
}
