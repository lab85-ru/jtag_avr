/*
 * task_consol.c
 *
 *  Created on: 29.10.2010
 *      Author: јдминистратор
 */

#include <string.h>
#include <time.h>

#include "main.h"


#include "stm32f10x_iwdg.h"
#include "main.h"
#include "task_consol.h"
#include "consol.h"
#include "env_config.h"
#include "git_commit.h"
#include "ff.h"
#include "diskio.h"
#include "i2c.h"
#include "jtag.h"
#include "hardware.h"

#ifndef DEBUG
#define DEBUG 0
#endif

extern void task_list( void );
extern struct queue_buffer consol_rx;
extern env_config_bin device_config;

extern DWORD fre_clust, fre_sect, tot_sect;
extern volatile FRESULT f_result;
extern FATFS  fatfs;
extern FATFS *fs;
extern FIL f_file;

void print_byte_to_bit(uint8_t b);


consol_structure consol_st;
volatile consol_flag_enum consol_flag_e;

#define ARGV_SIZE       16                      // количество параметров в строке
int argc;                                       // количество параметров найденых в строке 
char *argv[ ARGV_SIZE ];                        // массив указателей на параметры
 
char fbuff[ FBUFF_SIZE ];                       // буфер дл€ чтени€ файла
int fbuff_n;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// поиск и подсчет параметров строке 
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int search_arg(char *sin, char **argv)
{
        int n;
        char *f, *s;
        
        s = sin;
        
        if (strlen(s) == 0)
                return 0;
        
        argv[0] = s;                                       // строка не пуста€ значит есть хот€бы один параметр
        n = 1;
        
        while ( (f = strchr( s , (int)' ')) != NULL){
                *f = '\0';
                argv[n] = f + 1;
                n++;
                s = f + 1;
        }
                 
        return n;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// начальна€ инициализаци€ консоли
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void consol_init( void )
{
        consol_flag_e = FLAG_CONSOL;
        __disable_interrupt();
        memset(consol_st.buf, 0, sizeof(consol_st.buf));
        consol_st.n = 0;
        consol_st.i = 0;
        memset(consol_st.string, 0, CONSOL_BUF_SIZE);
        __enable_interrupt();
        consol_out_promt();        
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// вывод приглаши€ консоли
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void consol_out_promt( void )
{
        printf_d("\n\r%s",promt);        
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ќчистка буфера консоли
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void consol_clear_buf( void )
{
        int i;
        __disable_interrupt();
        for(i=0;i<CONSOL_BUF_SIZE;i++)
                consol_st.string[i] = 0;
        __enable_interrupt();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// обработка даннных пришедших с консоли
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void task_consol( void )
{
        int i;
        argc = search_arg( consol_st.string, argv );
        if (DEBUG){
                printf_d("\r\nargc=%d\r\n", argc);
                for(i=0;i<argc;i++)
                        printf_d("\r\nargv[%d]=%s\r\n",i, argv[i]);
        }
  
        if (argc == 0){
                consol_out_promt();
                consol_clear_buf();
                return;
        } else if ((consol_st.string[ 0 ] == KEY_ENTER) && (strlen(consol_st.string) == 1) ){ // пуста€ строка
                consol_out_promt();
                consol_clear_buf();
                return;
        }else if (strcmp(argv[0], cmd_help1)== 0 || strcmp(argv[0], cmd_help2)==0){
                printf_d("%s",txt_help);
	}else if (strcmp(argv[0], cmd_version)== 0 ){
		printf_d("%s\n\r",txt_device_name);
		printf_d("%s\n\r",txt_device_ver_soft);
		printf_d("%s\n\r",txt_device_ver_hard);
		printf_d("GIT version = %s\r\n",git_commit_str);
	}else if (strcmp(argv[0], cmd_root)== 0 ){
		printf_d("%s\n\r",txt_root_help);
	}else if (strcmp(argv[0], cmd_reboot)== 0 ){
		printf_d("\n\rReboot CPU...\n\r");
                __disable_interrupt();
                SysTick->CTRL  = 0;// остановили системный таймер теперь должен сработать вачдог
		while(1);
	}else if (strcmp(argv[0], cmd_xmodem)== 0){
                if (argc != 2){
                        printf_d("\r\n !!! ERROR format command !!!\r\n");
                        consol_clear_buf();
                        return;
                }
                printf_d("\r\n Start load X-modem.\r\n");
                os_task_push( TASK_XMODEM_ID );
                return;
        }else if (strcmp(argv[0], cmd_mkfs) == 0){
                printf_d("\r\n======= Create NEW file system + FORMAT disk...=======\r\n");
                f_result = f_mkfs(0, 1, FLASH_PAGE_SIZE);                                  // create file systems, disk=0, sdf, 2048 byte for sector
                printf_d("f_mkfs= ");
                print_result(f_result);
                f_result = f_getfree("0:", &fre_clust, &fs);
                printf_d("f_getfree= ");
                print_result(f_result);
                if (f_result == FR_NO_FILESYSTEM ){//FR_OK 
                        printf_d("\r\n !!! CRICAL ERROR !!!\r\n");
                        while(1);
                }
                print_disk_info(fre_clust, fs);
        }else if (strcmp(argv[0], cmd_free) == 0){
                f_result = f_getfree("0:", &fre_clust, &fs);
                printf_d("f_getfree= ");
                print_result(f_result);
                if (f_result == FR_NO_FILESYSTEM ){//FR_OK 
                        printf_d("\r\n !!! CRICAL ERROR !!!\r\n");
                        while(1);
                }
                print_disk_info(fre_clust, fs);
        }else if (strcmp(argv[0], cmd_ls) == 0){
                printf_d("\r\nPATH=0:\r\n\r\n");
                f_result = update_files_list ("0:\0");
                printf_d("\r\n");
                printf_d("f_ls= ");
                print_result(f_result);
                f_result = f_getfree("0:", &fre_clust, &fs);
                print_disk_info(fre_clust, fs);
        }else if (strcmp(argv[0], cmd_rm) == 0){
                if (argc != 2){
                        printf_d("Error format command.\r\n");
                        consol_clear_buf();
                        return;
                }
                f_result = f_unlink(argv[1]);
                printf_d("f_rm= ");
                print_result(f_result);
        }else if (strcmp(argv[0], cmd_dump) == 0){
                if (argc != 2){
                        printf_d("Error format command.\r\n");
                        consol_clear_buf();
                        return;
                }
                f_result = f_open( &f_file, argv[1], FA_READ);
                printf_d("f_open= ");
                print_result(f_result);
                if (f_result != FR_OK){
                        printf_d("Error open files = %s\r\n",argv[1]);
                        consol_clear_buf();                                     // очишаем буфер консоли иначе будет повторна€ загрузка
                        return;
                }
                while((f_result = f_read(&f_file, fbuff, FBUFF_SIZE, (UINT*)&fbuff_n )) == FR_OK && (fbuff_n>0) ){
                        dump_buff(fbuff, 0, fbuff_n);
                }
                f_close(&f_file);
        }else if (strcmp(argv[0], cmd_jtag) == 0){
                if (argc != 2){
                        printf_d("Error format command.\r\n");
                        consol_clear_buf();
                        return;
                }
                f_result = f_open( &f_file, argv[1], FA_READ);
                printf_d("f_open= ");
                print_result(f_result);
                if (f_result != FR_OK){
                        printf_d("Error open files = %s\r\n",argv[1]);
                        consol_clear_buf();                                     // очишаем буфер консоли иначе будет повторна€ загрузка
                        return;
                }

                prog_jtag(&f_file);
                
                f_close(&f_file);
        }else printf_d("\r\n ERROR: COMMAND NOT FOUND.\r\n");
        
        consol_out_promt();
        consol_clear_buf();
        return;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ¬ыводим байт по битам на консоль
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void print_byte_to_bit(uint8_t b)
{
        uint8_t i;

        for (i=0x80; i!=0; i=i>>1)
                if ( (b & i) > 0)   
                        printf_d("1 ");
                else
                        printf_d("0 ");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ¬ыводим hex байт
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DUMP_STRING_LEN         16              // длинна строки вывода дампа
void dump_buff (const char *buff, unsigned int ofs, unsigned int cnt)
{
	int i,k,m,a;
        char c;

        for (k=0; k<cnt; k=k+DUMP_STRING_LEN){
                if ((cnt-k) < DUMP_STRING_LEN)
                        m = cnt-k;
                else
                        m = DUMP_STRING_LEN;
                for(i = 0; i < m; i++)
                        printf_d(" %02X", buff[k+i]);

                printf_d(" ");
                
                if (m != DUMP_STRING_LEN)
                        for(a=0; a<(DUMP_STRING_LEN - m); a++)
                                printf_d("   ");
                
                for(i = 0; i < m; i++){
                        c = ((buff[k+i] >= ' ' && buff[k+i] <= '~') ? buff[k+i] : '.');
                        printf_d("%c",c);
                }
                
                printf_d("\n\r");
        }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
