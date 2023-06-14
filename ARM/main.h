/*
 * main.h
 *
 *  Created on: 21.01.2011
 *      Author: Администратор
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>
#include "keos.h"
#include "xprintf.h"
#include "ff.h"

#define printf_d        xprintf
#define log_out   printf_d
   
void tx_char(unsigned char ch);
void hex_out(uint8_t *buf, uint32_t len);
FRESULT scan_files (char* path);
void print_disk_info(DWORD fre_clust, FATFS *fs);
void delay_ms(uint32_t i);
void delay_10us(void);
void delay_5us(void);

extern const char txt_device_ver_soft[];
extern const char txt_device_ver_hard[];
extern const char txt_device_name[];

// строка имя файла + точка + расширение + \0
typedef char file_name_t[8+1+3+1];                      // (char) file name string

// структура для хранения списка файлов найденых на диске
#define FILE_LIST_ARRAY_SIZE            (16)                    // максимальное количество файлов в списке
typedef struct {
        file_name_t file_name[ FILE_LIST_ARRAY_SIZE ];
} file_list_array_struct;

#endif /* MAIN_H_ */
