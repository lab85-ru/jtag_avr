

// Define _ASSERT_ENABLE_ just below to detect SPI errors, such
// as timeout, ...
#define _ASSERT_ENABLE_
#ifdef _ASSERT_ENABLE_
#include <assert.h>
#endif

#include <string.h>


#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "main.h"
#include "i2c.h"
#include "lcd.h"


#ifndef LCD_DEBUG
#define LCD_DEBUG 0
#endif

#if !defined LCD_DIP204 && !defined LCD_FDCC1602 
#error "LCD NOT DEFINED TO PROJECT !!!"
#endif


#define LCD_LEDON	(0)
#define LCD_E		(1)
#define LCD_RS		(2)
#define LCD_RW		(3)
#define LCD_D4		(4)
#define LCD_D5		(5)
#define LCD_D6		(6)
#define LCD_D7		(7)


/****************************** global functions *****************************/
void lcd_write_byte(int fd, unsigned char c )
{
	unsigned char a,lcd_dc_port,temp;

	if (pcf8574_read(fd, U26_ADDR, &a) )
		if (LCD_DEBUG) printf("Error pcf8574_read.\n");
	a &= 0x0f;
	
	temp = a;

	lcd_dc_port = (c & 0xf0) | a;
	if (pcf8574_write(fd, U26_ADDR, lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_write.\n");

	delay_ms(1);

	lcd_dc_port |= (1<<LCD_E);
	if (pcf8574_write(fd, U26_ADDR, lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_write.\n");

	delay_ms(1);

	lcd_dc_port &= ~(1<<LCD_E);
	if (pcf8574_write(fd, U26_ADDR, lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_write.\n");

	delay_ms(4);

	a = temp;
	lcd_dc_port = ((c<<4) & 0xf0) | a;
	if (pcf8574_write(fd, U26_ADDR, lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_write.\n");

	delay_ms(1);

	lcd_dc_port |= (1<<LCD_E);
	if (pcf8574_write(fd, U26_ADDR, lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_write.\n");

	delay_ms(1);

	lcd_dc_port &= ~(1<<LCD_E);
	if (pcf8574_write(fd, U26_ADDR, lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_write.\n");

	delay_ms(4);

}

//;=================================================================================
void lcd_write_tetrada(int fd, unsigned char c)
{
	unsigned char a, lcd_dc_port;

	if (pcf8574_read(fd, U26_ADDR, &a) )
		if (LCD_DEBUG) printf("Error pcf8574_read.\n");
	a &= 0x0f;

	lcd_dc_port = (c & 0xf0) | a;
	if (pcf8574_write(fd, U26_ADDR, lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_write.\n");

	delay_ms(4*2);

	lcd_dc_port |= (1<<LCD_E);
	if (pcf8574_write(fd, U26_ADDR, lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_write.\n");

	delay_ms(4*2);

	lcd_dc_port &= ~(1<<LCD_E);
	if (pcf8574_write(fd, U26_ADDR, lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_write.\n");

	delay_ms(4*2);

}

void lcd_init(int fd)
{
unsigned char lcd_dc_port;
	
	lcd_dc_port = (1<<LCD_D4)|(1<<LCD_D5)|(1<<LCD_D6)|(1<<LCD_D7);
	
	if (pcf8574_write(fd, U26_ADDR, lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_write.\n");

	delay_ms(20);

#ifdef LCD_FDCC1602
	lcd_write_tetrada(fd, 0x30 );

	delay_ms(5);

	lcd_write_tetrada(fd, 0x30 );

	delay_ms(5);

	lcd_write_tetrada(fd, 0x30 );

	delay_ms(5);

	lcd_write_tetrada(fd, 0x20 );

	delay_ms(5);

	lcd_write_byte(fd, 0x28 );

	delay_ms(5);

	lcd_write_byte(fd, 0x0c );


	delay_ms(5);

	lcd_write_byte(fd, 0x06 );

	delay_ms(5);

	lcd_write_byte(fd, 0x01 );
	delay_ms(5);

	lcd_write_byte(fd, 0x02 );
	delay_ms(5);
#endif	
	
#ifdef LCD_DIP204
	lcd_write_tetrada(fd, 0x30 );

	delay_ms(5);

	lcd_write_tetrada(fd, 0x30 );

	delay_ms(5);

	lcd_write_tetrada(fd, 0x30 );

	delay_ms(5);

	lcd_write_tetrada(fd, 0x20 );

	delay_ms(5);

	lcd_write_byte(fd, 0x28 );

	delay_ms(5);

	lcd_write_byte(fd, 0x0c );

	delay_ms(5);

	lcd_write_byte(fd, 0x06 );
	delay_ms(5);

	lcd_write_byte(fd, 0x24 );

	delay_ms(5);

	lcd_write_byte(fd, 0x09 );

	delay_ms(5);

	lcd_write_byte(fd, 0x28 );

	delay_ms(5);

	lcd_write_byte(fd, 0x01 );
	delay_ms(5);

	lcd_write_byte(fd, 0x02 );
	delay_ms(5);
#endif	
}


void lcd_set_backlight(int fd, backlight_state_t led)
{
	unsigned char a;

	if (pcf8574_read(fd, U26_ADDR, &a) )
		log_out("Error pcf8574_read.\n");
	if (LCD_DEBUG) printf("u26_ADDR read = %x\n",a);	
	
 	switch(led){
 	case BACKLINGTH_ON:
 		if (LCD_DEBUG) log_out("------------LCD BACKLINGTH_ON.----------------");
		a &= ~(1<<LCD_LEDON);
		if (pcf8574_write(fd, U26_ADDR, a) )
			log_out("Error pcf8574_write.\n");
 	break;
 	case BACKLINGTH_OFF:
 		if (LCD_DEBUG) log_out("------------LCD BACKLINGTH_OFF.----------------");
		a |= (1<<LCD_LEDON);
		if (pcf8574_write(fd, U26_ADDR, a) )
			log_out("Error pcf8574_write.\n");
 	break;
 	}
}


void lcd_show_cursor(int fd)
{
}


void lcd_hide_cursor(int fd)
{
}

void lcd_set_cursor_position(int fd, unsigned char x, unsigned char y)
{
	unsigned char pos, lcd_dc_port, addr;

	if (x!=0)
		x--; 

#ifdef LCD_DIP204

	switch(y){
	case 1:
		addr = 0;
		break;
	case 2:
		addr = 0x20;
		break;
	case 3:
		addr = 0x40;
		break;
	case 4:
		addr = 0x60;
		break;
	default:
		return;
	}

	pos = addr | x;

	if (LCD_DEBUG)
		printf("LCD DDRAM coordinate x=%d y=%d ADDR=%x\n",x,y,pos);
	
#endif	
	
#ifdef LCD_FDCC1602
	pos = (((y<<4) & 0x20)<<1) + x;
#endif

	pos |= 0x80;

	if (pcf8574_read(fd, U26_ADDR, &lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_read.\n");
		
	lcd_dc_port &= ~(1<<LCD_RS);
	if (pcf8574_write(fd, U26_ADDR, lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_write.\n");
	
	lcd_write_byte(fd, pos );
}


void lcd_clear_display(int fd)
{
	unsigned char lcd_dc_port;
	
	if (pcf8574_read(fd, U26_ADDR, &lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_read.\n");

	lcd_dc_port &= ~(1<<LCD_RS);
	if (pcf8574_write(fd, U26_ADDR, lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_write.\n");
	
	lcd_write_byte(fd, 0x01 );

	delay_ms(5);

	lcd_write_byte(fd, 0x02 );
}


void lcd_write_string(int fd, const char *str)
{
	unsigned char lcd_dc_port,x;
	int count = 0; // 

	if (pcf8574_read(fd, U26_ADDR, &lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_read.\n");

	while( (x = *( str + count )) != 0){
		lcd_dc_port |= (1<<LCD_RS);
		if (pcf8574_write(fd, U26_ADDR, lcd_dc_port) )
			if (LCD_DEBUG) printf("Error pcf8574_write.\n");
		
		lcd_write_byte(fd, x );
		count++;
	}
}


void lcd_printf_string(int fd, const char *format, ...)
{
}

void lcd_fputc(int fd, char c)
{
	unsigned char lcd_dc_port;
	
	if (pcf8574_read(fd, U26_ADDR, &lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_read.\n");

	lcd_dc_port |= (1<<LCD_RS);
	if (pcf8574_write(fd, U26_ADDR, lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_write.\n");

	delay_ms(1);

	lcd_write_byte(fd, c );

	lcd_dc_port &= ~(1<<LCD_RS);
	if (pcf8574_write(fd, U26_ADDR, lcd_dc_port) )
		if (LCD_DEBUG) printf("Error pcf8574_write.\n");
}
