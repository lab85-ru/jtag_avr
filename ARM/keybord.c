#include "i2c.h"
#include "keybord.h"
#include "main.h"

////////////////////////////////////////////////////////////////////////////////
// read key is pressed
////////////////////////////////////////////////////////////////////////////////
unsigned char read_keybord(void)
{
        unsigned char d,x;
        
        pcf8574_read( 0, U27_ADDR, &d);
        d = d & KEY_MASK;
        
        if (d == KEY_MASK)              // нет нажатых конопок
                return 0;
        
        // нажата кнопка
        delay_ms(20);
                
        pcf8574_read( 0, U27_ADDR, &d);
        d = d & KEY_MASK;
        
        if (d == KEY_MASK)              // нет нажатых конопок ШУМ
                return 0;

        x = d;                          // запомнили нажатые конопки
        
        // ждем когда отпустит кнопку
        do{
                delay_ms(20);
                pcf8574_read( 0, U27_ADDR, &d);
                d = d & KEY_MASK;
        }while (d != KEY_MASK);
        
        return (~x) & KEY_MASK;
}