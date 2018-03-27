/*
 * task_consol.h
 *
 *  Created on: 29.10.2010
 *      Author: �������������
 */

#ifndef TASK_CONSOL_H_
#define TASK_CONSOL_H_

#define FBUFF_SIZE      512

#define CONSOL_BUF_SIZE 	(134)// 133 for x-modex-128

typedef struct{
        char buf[ CONSOL_BUF_SIZE ];    // �����, ������ �������� �� ��������� ������
        unsigned char n;                // ���������� ���� � ������
        unsigned char i;                 // ������� ������� ������������ ����� �� ������ ��� x-modem, �.�. ������� �����
        char string[ CONSOL_BUF_SIZE ]; // ����� ������ buf ��� ������������ �������
}consol_structure;

typedef enum {FLAG_CONSOL, FLAG_XMODEM} consol_flag_enum;        // ����� � ������� ��������� �������

int search_arg(char *sin, char **argv);
void consol_init( void );
void consol_out_promt( void );
void consol_clear_buf( void );
void task_consol( void );
void print_byte_to_bit(uint8_t b);
void dump_buff (const char *buff, unsigned int ofs, unsigned int cnt);


#endif /* TASK_CONSOL_H_ */
