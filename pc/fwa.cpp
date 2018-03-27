// fwa.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>

#define DUMP_STRING_LEN         16					            // длинна строки вывода дампа
#define FILE_HEADER_SIZE        16								// длинна заголовка файла


#define LINE_LEN 80												// количество символов в строке при считывание HEX файла

char *in_file_name_str;											// имя файла, входного для HEX
char *out_file_name_str;										// имя файла, вЫходного для BIN
char *signature_str;											// сигнатура CPU(строка)
char *fuse_str;													// FUSE CPU(строка)

#define BLOCK_WRITE_SIZE		(128)							// размер блока на запись файла

#define BUFF_FIRMWARE_BIN_SIZE	(1024*1024)
unsigned char buff_firmware_bin[ BUFF_FIRMWARE_BIN_SIZE ];				// буфер для хранения прошивки в бинарном виде
unsigned int firmware_len = 0;									// длинна двоичной прошивки в буфере

#define SIGNATURE_CPU_SIZE (3)
unsigned char signature_cpu[ SIGNATURE_CPU_SIZE ];


#define FUSE_CPU_SIZE (3)
unsigned char fuse_cpu[ FUSE_CPU_SIZE ];



//--function prototyps----------------------------------------------------------------
int ConvAsciiStringToBin(unsigned char *in_string, unsigned char *out_buf, unsigned char *len_out);
int LoadFileHex(char *in_filename, unsigned char *buff_out, unsigned int *firmware_len );
int SaveASMFile(char *out_filename, unsigned char *buff_in, unsigned int firmware_len, unsigned char *signature_cpu, unsigned char *fuse_cpu );
unsigned char ConvAsciiToBin (char in);
void dump_buff (const unsigned char *buff, unsigned int ofs, unsigned int cnt);
//------------------------------------------------------------------------------------





//=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN=MAIN
int main(int argc, char *argv[])
{
	int i;

	printf("FirmWare Assembler for JTAG-AVR Programmer v1.0 2013.\r\n");

	if (argc == 0){
		printf("fwa -I <input filename> -O <output filename> -S <signature 3 byte> -F <fuse 3 byte>\r\n");
		return 0;
	}

	if (argc < 9){
		printf("\r\nError: parametrs.\r\n");
		return 0;
	}

	for (i=0; i<argc; i++){
		//printf("argument [%d] = %s\r\n",i ,argv[i]);
		if ( !strcmp(argv[i],"-I") && i!=argc){
			in_file_name_str = argv[i + 1];
			i++;
		}else if ( !strcmp(argv[i],"-O") && i!=argc){
			out_file_name_str = argv[i + 1];
			i++;
		}else if ( !strcmp(argv[i],"-S") && i!=argc){
			signature_str = argv[i + 1];
			i++;
		}else if ( !strcmp(argv[i],"-F") && i!=argc){
			fuse_str = argv[i + 1];
			i++;
		}

	}

	if (strlen(signature_str)!= SIGNATURE_CPU_SIZE*2){
		printf("\r\nError: signature cpu length.\r\n");
		return -1;
	}
	for(i=0;i<SIGNATURE_CPU_SIZE;i++){
		signature_cpu[i] = (ConvAsciiToBin( *(signature_str+i*2))<<4) | ConvAsciiToBin(*(signature_str+i*2+1));
	}
	//printf("signature %s = %X %X %X\r\n",signature_str, signature_cpu[0], signature_cpu[1], signature_cpu[2]);

	if (strlen(fuse_str)!= FUSE_CPU_SIZE*2){
		printf("\r\nError: fuse cpu length.\r\n");
		return -1;
	}
	for(i=0;i<FUSE_CPU_SIZE;i++){
		fuse_cpu[i] = (ConvAsciiToBin( *(fuse_str+i*2))<<4) | ConvAsciiToBin(*(fuse_str+i*2+1));
	}
	//printf("fuse %s = %X %X %X\r\n",fuse_str, fuse_cpu[0], fuse_cpu[1], fuse_cpu[2]);


	memset(buff_firmware_bin, 0xff, BUFF_FIRMWARE_BIN_SIZE);			// чистим буфер от мусора(заносим значения FF)

	if (LoadFileHex(in_file_name_str, buff_firmware_bin, &firmware_len) != 0 ) {printf("Error: Firmware File not Load....\n");return -1;}
	printf("Firmware len = %d\r\n",firmware_len);

/*	for(unsigned int k=0; k<firmware_len; k=k+DUMP_STRING_LEN)
		dump_buff (buff_firmware_bin + k, 0, DUMP_STRING_LEN);
*/	

	if (SaveASMFile(out_file_name_str, buff_firmware_bin, firmware_len, signature_cpu, fuse_cpu) != 0){printf("Error: Write asm file.\r\n");return -1;}

	return 0;
}
//=END MAIN=END MAIN=END MAIN=END MAIN=END MAIN=END MAIN=END MAIN=END MAIN=END MAIN=END MAIN=END MAIN=END MAIN=END MAIN=END MAIN=END MAIN


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Save Asm File
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int SaveASMFile(char *out_filename, unsigned char *buff_in, unsigned int firmware_len, unsigned char *signature_cpu, unsigned char *fuse_cpu )
{
	FILE *file_stream;
	unsigned int i = 0;
	unsigned int n = 0;
	unsigned int l = 0;
	size_t m;

	unsigned char f_header[FILE_HEADER_SIZE];

	memset(f_header, 0, FILE_HEADER_SIZE);
	memcpy(f_header, signature_cpu, SIGNATURE_CPU_SIZE);
	memcpy(f_header + SIGNATURE_CPU_SIZE, fuse_cpu, FUSE_CPU_SIZE);

	file_stream  = fopen( (const char*)out_filename, "wb" );
	if( file_stream == NULL ) return -1;

	l = fwrite(f_header, 1, FILE_HEADER_SIZE, file_stream);
	if (l < 0){printf("Error: Write to file.\r\n");return -1;}

	printf("Write header to asm file =%d\r\n",l);

	// записываем прошивку
	while(i != firmware_len){
		if((firmware_len - i) < BLOCK_WRITE_SIZE)
			m = firmware_len - i;
		else
			m = BLOCK_WRITE_SIZE; 

		n = fwrite(buff_in + i, 1, m, file_stream);
		if (n < 0){printf("Error: Write to file.\r\n");return -1;}
		i = i + n;
	}

	printf("Write firmware to asm file =%d\r\n",i);
	printf("Result asm file len =%d\r\n",i+l);

	fclose(file_stream);
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Load Hex File
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int LoadFileHex(char *in_filename, unsigned char *buff_out, unsigned int *firmware_len )
{
	FILE *file_stream;
	unsigned char s[LINE_LEN];						// длинна строки в символах считываеммая из файла
	int i;
	//int k;
	unsigned char in_string[LINE_LEN];				// текущая строка из файла в BIN виде кроме символов ":" $ Cr Lf !!!!
	unsigned char crc_byte = 0;						// для подсчета контрольной суммы строки НЕХ файла
	unsigned char len_array = 0;					// длинна строки байт после преобразования из TXT->BIN
	unsigned int adr_buffer;
	
	unsigned int FlashSegment  = 0;
	unsigned int FlashAdr = 0;
	
	file_stream  = fopen( (const char*)in_filename, "r" );
	if( file_stream == NULL ) return -1;
	
	printf("Load firmware File (format HEX) = %s ",in_filename);
	while( fgets( (char*)s, LINE_LEN, file_stream) != NULL )
	{
		//printf("strin = %s",s);
		if (ConvAsciiStringToBin( &s[0], &in_string[0], &len_array ) != 0 ) { printf("\nError: Input file format.\n");return -1;}

/*		for (i=0; i<len_array; i++)
			printf("%X\r\n",in_string);
*/	

		// проверка контрольной суммы полученной строки
		crc_byte = 0;
		for (i=0; i<len_array; i++)
			crc_byte = crc_byte + in_string[i];

		if	(crc_byte != 0) {printf("\nError: Input string(Hex) CRC.\n");return -1;}

		// смотрим и в зависимости от типа записи строки выполняем действия
		// в бинарной строке это будет 4 байт или 3 адрес начиная с 0 !!!!!!

		switch (in_string[3])
		{
				case 0x00:{	// двоичные данные
							// заносим в буфер Flash
							FlashAdr = (in_string[1] * 256) + in_string[2];
							adr_buffer = FlashSegment + FlashAdr;
							for (i=0; i<in_string[0]; i++){ 
								*(buff_out + adr_buffer + i) = in_string[i + 4];	//FlashBufBin[adr_buffer+i] = in_string[i+4]; 
								*firmware_len = adr_buffer + i;						// подсчитываем длинну прошивки по самой последней записи
							}
							break;
						  }
				case 0x01:{	// Конец файла
							printf("Ok !\n");
							break;
						  }
				case 0x02:{	// Сегментный адресс
							FlashSegment = ((in_string[4]*256) + in_string[5]) << 4;
							break;
						  }
				case 0x04:{	// Линейный адресс
							break;
						  }
		default: {}	
		}
	}

	*firmware_len = *firmware_len + 1; // +1 т.к. длинна прошивки считается от 0 то необходимо прибавить 1

	fclose(file_stream);
	return 0;
}
//==========================================================================================================
// конвертация текстовой строки в BIN
//==========================================================================================================
int ConvAsciiStringToBin(unsigned char *in_string, unsigned char *out_buf, unsigned char *len_out)
{
	int		counter_in=0, counter_out=0;
	unsigned char	a1, a2, a3;

	a1 = *(in_string + counter_in);

	if (counter_in == 0 )
		if ( a1 != ':') {printf("Error Format Hex Files !!!\n");return -1;} 

	counter_in++;

	while ( a1 != 0x0A )	// входная строка текстовая значит ищем конец строки ! заменил 00 на 0A тк перебирает до опупения !!!!
	{

		a1 = *(in_string + counter_in);
		a2 = *(in_string + counter_in +1);

		a3 = (ConvAsciiToBin(a1)<<4) | ConvAsciiToBin(a2);
		*(out_buf + counter_out) = a3;

		counter_in = counter_in + 2;
		counter_out++;

		a1 = *(in_string + counter_in);

	}
	
	*len_out = counter_out;

	return 0;
}
//==========================================================================================================
// конвертация Ascii символа в BIN
//==========================================================================================================
unsigned char ConvAsciiToBin (char in)
{
	char out;	

	if (in >= 0x41) out = in-0x37;
	else out = 0x0f & in;

	return out;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Выводим hex байт
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void dump_buff (const unsigned char *buff, unsigned int ofs, unsigned int cnt)
{
	unsigned int i,k,m,a;
    unsigned char c;

	//printf_d("%08lX:", ofs);
        
        for (k=0; k<cnt; k=k+DUMP_STRING_LEN){
                if ((cnt-k) < DUMP_STRING_LEN)
                        m = cnt-k;
                else
                        m = DUMP_STRING_LEN;
                for(i = 0; i < m; i++)
                        printf(" %02X", buff[k+i]);

                printf(" ");
                
                if (m != DUMP_STRING_LEN)
                        for(a=0; a<(DUMP_STRING_LEN - m); a++)
                                printf("   ");
                
                for(i = 0; i < m; i++){
                        c = ((buff[k+i] >= ' ' && buff[k+i] <= '~') ? buff[k+i] : '.');
                        printf("%c",c);
                }
                
                printf("\n\r");
        }
}
