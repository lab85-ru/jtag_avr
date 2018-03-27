/*
 * task_consol.c
 *
 *  Created on: 29.10.2010
 *      Author: �������������
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

unsigned int count = 0;                 // ������� �������� ����
unsigned int data_wr_count = 0;         // ������� ��������� ����


// �.�. ��� ����������� ���������� ����� ����� ������������� �� ��������� ������
// �.�. ����������� ������ ���� ��������� ��� ���, ���������� ��� �������� ������ ������������ �����
// ��� ��������� ��� ���� ��������������(��� ����������� �.�. ������ ����������)
// �� ������������ ��������� ������� ����������:
// ��� ����� �������� �������� �� ��� ������ ��������
// ������ ����� ������� � �����, ��� ������ ����� ����������� ������������ �� ���� ������ ������������� �����
// ��������� ����� ������������ �� ���� ����� ��������� ����� � ������� ���� 1A � �������� �����.
//
char xcache_buf[ 2 * XMODEM_BUFFER_SIZE ];                             // ��� �����
int flag_xcache_buf[2];                                                         // ���� ����� ��������-��������
int xcache_buf_num;                                                             // ����� �������� ������ ���� ����� ������������ ������


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������ �������� �� �������������� ������ uart-xmodem
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
// ������� ������ ������ �� ������ x-modem
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
        
        memcpy( &xcache_buf[ xcache_buf_num * XMODEM_BUFFER_SIZE ], buffer, size);      // ����������� � ��� ����� ������ �� ������
        flag_xcache_buf[ xcache_buf_num ] = 1;                                          // �������� ������ ����� �����
        
        xcache_buf_num++;                                                               // ����������� ������� ����� �� ���������
        xcache_buf_num = xcache_buf_num & 1;
          
        count = count + size;
        return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ������� �������� ������ ����� ���� 1A
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int cut_buf(char *buffer, int buff_size)
{
        int size_no_1a;
        int i;

        // ����� ����� ����� �� ���� 0x1A
        if (*(buffer + (buff_size - 1)) == CTRLZ){           // ��������� ������
                for(i=(buff_size-1); i>0; i--){
                        if (*(buffer + i) != CTRLZ){
                                size_no_1a = i + 1;        // i + 1 �.�. � �������� �� ���������� ������ �� = CTRLZ
                                break;
                        }
                }
        }else size_no_1a = buff_size;
        
        return size_no_1a;
}
  
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������� ������� ��������� � �������
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
        
        flag_xcache_buf[0] = 0;                                                 // ����� ����
        flag_xcache_buf[1] = 0;
        xcache_buf_num = 0;

        if (strlen(argv[1]) == 0){
                consol_flag_e = FLAG_CONSOL;
                consol_clear_buf();                     // ������� ����� ������� ����� ����� ��������� ��������
                return;
        }

        
        f_result = f_open( &f_file, argv[1], FA_CREATE_ALWAYS | FA_WRITE);
        printf_d("f_open= ");
        print_result(f_result);
        if (f_result != FR_OK){
                consol_flag_e = FLAG_CONSOL;
                consol_clear_buf();                     // ������� ����� ������� ����� ����� ��������� ��������
                return;
        }
        
        res = xmodemReceive( write_data );
        
        if ((flag_xcache_buf[0] == 0) && (flag_xcache_buf[1] == 0))
                goto txm_error_wr;

        // ���������� ��� �� ����
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
        consol_clear_buf();                     // ������� ����� ������� ����� ����� ��������� ��������
}

