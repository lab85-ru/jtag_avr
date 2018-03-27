/*
 * task_consol.c
 *
 *  Created on: 29.10.2010
 *      Author: Администратор
 */

#include <string.h>
#include <time.h>

#include "main.h"
#include "xmodem.h"
#include "task_consol.h"
#include "stm32f10x_iwdg.h"
#include "ff.h"
#include "diskio.h"
   
   
   
extern consol_flag_enum consol_flag_e;
extern consol_structure consol_st;
extern FRESULT f_result;
//FATFS  fatfs;
//FATFS *fs;// = &fatfs;
extern FIL f_file;

//int argc;        
extern char *argv[];

unsigned int count = 0;                 // счетчик принятых байт
unsigned int data_wr_count = 0;         // счетчик записаных байт


// т.к. нет возможности определить конец файла передоваемого по протоколу хмодем
// т.к. неизветстно данный блок последний или нет, необходимо или задавать длинну принимаемого файла
// или сохранять его весь предварительно(нет возможности т.к. память ограничена)
// то предлагается следующий вариант реализации:
// кеш буфер линейный разделен на две равные половины
// каждый пакет пишится в буфер, как только буфер заполняется сбрасывается на диск только предпоследний буфер
// Последний буфер сбрасывается на диск перед закрытием файла и поиском кода 1A и усечение файла.
//
char xcache_buf[ 2 * XMODEM_BUFFER_SIZE ];                             // кеш буфер
int flag_xcache_buf[2];                                                         // флаг буфер заполнен-свободен
int xcache_buf_num;                                                             // номер текущего буфера куда будет производится запись


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// чтение символов из промежуточного буфера uart-xmodem
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int rx_char(void)
{
        char c;
        
        //while 
        //if (consol_st.n == 0 || consol_st.n == consol_st.i) return -1;         // wait rx char from uart
        if (consol_st.n == 0) return -1;         // wait rx char from uart
        
        if (consol_st.n != consol_st.i){
        
                c = consol_st.buf[ consol_st.i ];
                consol_st.i++;
        
                if (consol_st.n == consol_st.i){
                        __disable_interrupt();
                        consol_st.n = 0; 
                        consol_st.i = 0;
                        __enable_interrupt();
                }
                return  (int)c;
        }
        return -1;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// функция записи данных из пакета x-modem
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int write_data(unsigned char* buffer, int size)
{
        UINT bw;         /* File read/write count */
        FRESULT res;
        
        if ((flag_xcache_buf[0] != 0) && (flag_xcache_buf[1] != 0)){
                res = f_write(&f_file, &xcache_buf[ xcache_buf_num * XMODEM_BUFFER_SIZE ], XMODEM_BUFFER_SIZE, &bw);                /* Write it to the dst file */
                flag_xcache_buf[ xcache_buf_num ] = 0;
                data_wr_count = data_wr_count + XMODEM_BUFFER_SIZE;
                if (res || bw < XMODEM_BUFFER_SIZE) return -1;                              /* error or disk full */
        }
        
        memcpy( &xcache_buf[ xcache_buf_num * XMODEM_BUFFER_SIZE ], buffer, size);      // скопировали в кеш буфер данные из пакета
        flag_xcache_buf[ xcache_buf_num ] = 1;                                          // пометили данный буфер занят
        
        xcache_buf_num++;                                                               // переключаем текущий буфер на следующий
        xcache_buf_num = xcache_buf_num & 1;
          
        count = count + size;
        return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// функция усечения буфера поиск кода 1A
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int cut_buf(char *buffer, int buff_size)
{
        int size_no_1a;
        int i;

        // поиск конца файла по коду 0x1A
        if (*(buffer + (buff_size - 1)) == CTRLZ){           // последний символ
                for(i=(buff_size-1); i>0; i--){
                        if (*(buffer + i) != CTRLZ){
                                size_no_1a = i + 1;        // i + 1 т.к. и указвает на полследний символ не = CTRLZ
                                break;
                        }
                }
        }else size_no_1a = buff_size;
        
        return size_no_1a;
}
  
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// обработка даннных пришедших с консоли
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void task_xmodem( void )
{
        long res;
        FRESULT f_res;
        int buf_data_len;
        UINT bw;         /* File read/write count */

        xmodemInit( tx_char, rx_char);
        consol_flag_e = FLAG_XMODEM;
        
        count = 0;
        data_wr_count = 0;
        
        flag_xcache_buf[0] = 0;                                                 // сброс кеша
        flag_xcache_buf[1] = 0;
        xcache_buf_num = 0;

        if (strlen(argv[1]) == 0){
                consol_flag_e = FLAG_CONSOL;
                consol_clear_buf();                     // очишаем буфер консоли иначе будет повторная загрузка
                return;
        }

        
        f_result = f_open( &f_file, argv[1], FA_CREATE_ALWAYS | FA_WRITE);
        printf_d("f_open= ");
        print_result(f_result);
        if (f_result != FR_OK){
                consol_flag_e = FLAG_CONSOL;
                consol_clear_buf();                     // очишаем буфер консоли иначе будет повторная загрузка
                return;
        }
        
        res = xmodemReceive( write_data );
        
        if ((flag_xcache_buf[0] == 0) && (flag_xcache_buf[1] == 0))
                goto txm_error_wr;

        // сбрасываем кеш на диск
        if ((flag_xcache_buf[0] != 0) && (flag_xcache_buf[1] != 0)){
                f_res = f_write(&f_file, &xcache_buf[ xcache_buf_num * XMODEM_BUFFER_SIZE ], XMODEM_BUFFER_SIZE, &bw);                /* Write it to the dst file */
                count = count + XMODEM_BUFFER_SIZE;
                data_wr_count = data_wr_count + XMODEM_BUFFER_SIZE;

                flag_xcache_buf[ xcache_buf_num ] = 0;
                xcache_buf_num++;
                xcache_buf_num = xcache_buf_num & 1;
                if (f_res || bw < XMODEM_BUFFER_SIZE) goto txm_error_wr;                              /* error or disk full */

                buf_data_len = cut_buf(&xcache_buf[ xcache_buf_num * XMODEM_BUFFER_SIZE ], XMODEM_BUFFER_SIZE);
                
                f_res = f_write(&f_file, &xcache_buf[ xcache_buf_num * XMODEM_BUFFER_SIZE ], buf_data_len, &bw);                /* Write it to the dst file */
                count = count + XMODEM_BUFFER_SIZE;
                data_wr_count = data_wr_count + buf_data_len;
                flag_xcache_buf[ xcache_buf_num ] = 0;
                if (f_res || bw < buf_data_len) goto txm_error_wr;                              /* error or disk full */
                
        }else{
                xcache_buf_num++;
                xcache_buf_num = xcache_buf_num & 1;
                buf_data_len = cut_buf(&xcache_buf[ xcache_buf_num * XMODEM_BUFFER_SIZE ], XMODEM_BUFFER_SIZE);
                f_res = f_write(&f_file, &xcache_buf[ xcache_buf_num * XMODEM_BUFFER_SIZE ], buf_data_len, &bw);                /* Write it to the dst file */
                count = count + XMODEM_BUFFER_SIZE;
                data_wr_count = data_wr_count + buf_data_len;
                if (f_res || bw < buf_data_len) goto txm_error_wr;                              /* error or disk full */
        }
        
txm_error_wr:        
        f_close( &f_file );
        
        os_delay(DELAY_3S);
        
        printf_d("Xmodem result code = %d\r\n",res);
        printf_d("Write data to file = %d\r\n",data_wr_count);
        if (res != count)
                printf_d("\r\n !!! ERROR WRITE FILE !!!\r\n");
        
        consol_flag_e = FLAG_CONSOL;
        consol_clear_buf();                     // очишаем буфер консоли иначе будет повторная загрузка
}

