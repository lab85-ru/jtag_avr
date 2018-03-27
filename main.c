#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "stm32f10x_conf.h"
#include "stm32f10x.h"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include "main.h"
#include "env_config.h"
#include "hardware.h"
#include "sys_timer.h"
#include "i2c.h"
#include "keos.h"
#include "xprintf.h"
#include "task_consol.h"
#include "lcd.h"
#include "keybord.h"
#include "git_commit.h"

#include "diskio.h"
#include "ff.h"
#include "integer.h"

#ifndef DEBUG
#define DEBUG 0
#endif

const char txt_device_ver_soft[]={"SV:1.0.13"};		// версия прошивки софт
const char txt_device_ver_hard[]={"HV:1.0.13"};		// версия прошивки железа
const char txt_device_name[]={"JTAG-AVR-PROG STM32."};        // txt pole opisanie ustroystva

env_config_bin device_config;                           // structure for currents config device

DWORD fre_clust, fre_sect, tot_sect;
FRESULT f_result;
FATFS  fatfs;
FATFS *fs;
FIL f_file;

DWORD AccSize;				/* Work register for fs command */
WORD AccFiles, AccDirs;
FILINFO Finfo;

typedef struct {
	WORD	year;
	BYTE	month;
	BYTE	mday;
	BYTE	hour;
	BYTE	min;
	BYTE	sec;
} RTC1;

int files_found = 0;                                            // количество найденных файлов и помещеных в список
file_list_array_struct file_list_array_st;                      // массив для хранения списка найденных файлов

#define STRING_FOR_SPRINT_SIZE (128)
char string_for_sprint[ STRING_FOR_SPRINT_SIZE ];               // строка для преобразования в строку чисел и т.п.
backlight_state_t lcd_backlinght;                               // подсветка ЖКИ on-off
unsigned char keybord_get_press_key = 0;                        // заносим нажатые клавиши

// call prototype --------------------------------------------------------------
void tx_char(unsigned char ch);

/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support a real time clock.          */
/* This is not required in read-only configuration.        */
DWORD get_fattime ()
{
	RTC1 rtc;

	return	  ((DWORD)(rtc.year - 1980) << 25)
			| ((DWORD)rtc.month << 21)
			| ((DWORD)rtc.mday << 16)
			| ((DWORD)rtc.hour << 11)
			| ((DWORD)rtc.min << 5)
			| ((DWORD)rtc.sec >> 1);
}

////////////////////////////////////////////////////////////////////////////////
// Print directory files list
////////////////////////////////////////////////////////////////////////////////
FRESULT scan_files (char* path)
{
	DIR dirs;
	FRESULT res;
	int i;
	char *fn;

	res = f_opendir(&dirs, path);
	if (res == FR_OK) {
		i = strlen(path);
		while (((res = f_readdir(&dirs, &Finfo)) == FR_OK) && Finfo.fname[0]) {
			if (_FS_RPATH && Finfo.fname[0] == '.') continue;
#if _USE_LFN
			fn = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
			fn = Finfo.fname;
#endif
			if (Finfo.fattrib & AM_DIR) {
				AccDirs++;
				path[i] = '/'; strcpy(path+i+1, fn);
				res = scan_files(path);
				path[i] = '\0';
				if (res != FR_OK) break;
			} else {
				printf_d("%s/%s         %d\n", path, fn, Finfo.fsize);
                                
                                if (files_found <= FILE_LIST_ARRAY_SIZE ){
                                        strcpy( (char*)&file_list_array_st.file_name[ files_found ], fn);
                                        files_found++;
                                }else{
                                        printf_d("ERROR file_list_array_st id FULL.\n");
                                }
                                
				AccFiles++;
				AccSize += Finfo.fsize;
			}
		}
	}

	return res;
}

////////////////////////////////////////////////////////////////////////////////
// update files list + Print directory files list
////////////////////////////////////////////////////////////////////////////////
FRESULT update_files_list (char* path)
{
        files_found = 0;
        return scan_files (path);
}


////////////////////////////////////////////////////////////////////////////////
// disk_info FAT sectors + size
////////////////////////////////////////////////////////////////////////////////
void print_disk_info(DWORD fre_clust, FATFS *fs)
{
        printf_d("FAT type = %u\nBytes/Cluster = %lu\nNumber of FATs = %u\n"\
		"Root DIR entries = %u\nSectors/FAT = %lu\nNumber of clusters = %lu\n"\
		"FAT start (lba) = %lu\nDIR start (lba,clustor) = %lu\nData start (lba) = %lu\n\n",\
		fs->fs_type, (DWORD)fs->csize * (DWORD)fs->ssize, fs->n_fats,\
		fs->n_rootdir, fs->fsize, fs->n_fatent - 2,\
		fs->fatbase, fs->dirbase, fs->database);
        
        printf_d("\r\n%lu KB total disk space.\n%lu KB available.\n",
		((fs->n_fatent - 2) * fs->csize * fs->ssize)>>10, (fre_clust * fs->csize * fs->ssize)>>10);// /1024
}

////////////////////////////////////////////////////////////////////////////////
// MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN
////////////////////////////////////////////////////////////////////////////////
int main()
{
	    xdev_out(tx_char);              // setup for xprintf function output

	    SystemInit();  	                // this sets up the oscilator
        init_hardware();                // init hardware
        sys_timer_setup();              // init sys counter
        
        printf_d("\r\n---Device start---\r\n");
        printf_d("%s\r\n",txt_device_name);
        printf_d("%s\r\n",txt_device_ver_soft);
        printf_d("%s\r\n",txt_device_ver_hard);
        
        consol_init();                  // init consol + out promt

        lcd_backlinght = BACKLINGTH_ON;
        lcd_init(0);
        lcd_clear_display(0);
        lcd_set_backlight(0, lcd_backlinght);
        lcd_show_cursor(0);
        lcd_set_cursor_position(0,1,1);
        lcd_write_string(0,txt_device_name);
        lcd_set_cursor_position(0,1,2);
        lcd_write_string(0,txt_device_ver_soft);
        lcd_set_cursor_position(0,1,3);
        lcd_write_string(0,txt_device_ver_hard);
        lcd_set_cursor_position(0,1,4);
        lcd_write_string(0,"GIT=");
        lcd_fputc(0,git_commit_str[0]);
        lcd_fputc(0,git_commit_str[1]);
        lcd_fputc(0,git_commit_str[2]);
        lcd_fputc(0,git_commit_str[3]);
        lcd_fputc(0,git_commit_str[4]);
        lcd_fputc(0,git_commit_str[5]);
        lcd_fputc(0,git_commit_str[6]);
        lcd_fputc(0,git_commit_str[7]);

        delay_ms(4000);
        
       	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET){
		/* IWDGRST flag set */
		printf_d("\r\nHard Reset From WatchDog....\r\n");
		/* Clear reset flags */
		RCC_ClearFlag();
	}

        lcd_clear_display(0);
        
        lcd_set_cursor_position(0,1,1);
        lcd_write_string(0,"Mount Fat:");
        
        f_result = f_mount(0, &fatfs);
        printf_d("f_mount= ");
        print_result(f_result);
        
        /* Get volume information and free clusters of drive 1 */
        f_result = f_getfree("0:", &fre_clust, &fs);
        printf_d("f_getfree= ");
        print_result(f_result);
    
        if (f_result == FR_NO_FILESYSTEM){
                lcd_write_string(0,"Error.");
                lcd_set_cursor_position(0,1,2);
                lcd_write_string(0,"Create Fat:");

                f_result = f_mkfs(0, 1, FLASH_PAGE_SIZE);                                  // create file systems, disk=0, sdf, 2048 byte for sector
                printf_d("f_mkfs= ");
                print_result(f_result);
                f_result = f_getfree("0:", &fre_clust, &fs);
                printf_d("f_getfree= ");
                print_result(f_result);
                if (f_result == FR_NO_FILESYSTEM ){
                        lcd_write_string(0,"Error.");
                        printf_d("\r\n !!! Error create fat, CRICAL ERROR systems stop !!!\r\n");
                        while(1);
                }
        }else {lcd_write_string(0,"Ok.");}
        print_disk_info(fre_clust, fs);
        
        update_files_list("0:");
        printf_d("\r\nFound Files = %d\r\n", files_found);
        
        lcd_set_cursor_position(0,1,2);                                         // печатаем на жки сколько найдено файлов
        lcd_write_string(0,"Found files:");
        xsprintf(string_for_sprint, "%d", files_found);
        lcd_write_string(0,string_for_sprint);
        
        delay_ms(2000);
        
        lcd_clear_display(0);
        os_init();

        os_task_push( TASK_MENU_ID );                                           // запускаем задачу вывода меню
        
        while(1)
                os_scheduler();
}
////////////////////////////////////////////////////////////////////////////////
// tx char to com port
////////////////////////////////////////////////////////////////////////////////
void tx_char(unsigned char ch)
{
        /* Loop until the end of transmission */
        while (USART_GetFlagStatus(H_COM1, USART_FLAG_TC) == RESET);
        /* Place your implementation of fputc here */
        /* e.g. write a character to the USART */
        USART_SendData(H_COM1, (uint8_t) ch);
//      return ch;
}
////////////////////////////////////////////////////////////////////////////////
// out txt-hex byte to consol
////////////////////////////////////////////////////////////////////////////////
void hex_out(uint8_t *buf, uint32_t len)
{
	uint8_t temp;
	uint32_t i;

	printf_d("\r\n");
	for (i=0; i<len; i++){
		temp = *(buf + i);
		printf_d("%002X ", temp);
	}
	printf_d("\r\n");
}
////////////////////////////////////////////////////////////////////////////////
// delay ms
////////////////////////////////////////////////////////////////////////////////
void delay_ms(uint32_t i)
{
        if (i != 0)
                os_delay(i);
}
////////////////////////////////////////////////////////////////////////////////
// delay primerno 5 us
////////////////////////////////////////////////////////////////////////////////
void delay_5us(void)
{
        for(int i=0;i<50;i++){
                  asm volatile("nop");
        }
}
