#include "main.h"
#include "lcd.h"
#include "keybord.h"
#include "keos.h"

extern unsigned char keybord_get_press_key;
extern file_list_array_struct file_list_array_st;
extern int files_found;
extern char string_for_sprint[];


const char txt_menu_top[]=      {"<-    ->    prog    "};     // вернее меню
const char txt_menu_prog[]=     {"     start      exit"};     // вернее меню
const char txt_menu_stop[]=     {"stop                "};     // вернее меню

const char txt_wait_key_press[]={"    Press any key.  "};

typedef enum {MP_TOP, MP_PROG, MP_STOP} menu_position_enum;

menu_position_enum menu_position_e = MP_TOP;

int file_numer = 1;                                            // номер файла из списка для программирования и вывода на ЖКИ

void task_menu( void )
{
        switch(menu_position_e){
        case MP_TOP:
                
                if (keybord_get_press_key & KEY_3){             // prog
                        menu_position_e = MP_PROG;
                        os_task_push( TASK_MENU_ID );                           // положение меню изменилось нужно вызвать саму себя для обновления содеожимого ЖКИ
                }
                if (keybord_get_press_key & KEY_1){             // <-
                        if (file_numer != 1)
                                file_numer--;
                        else
                                file_numer = files_found;
                }
                if (keybord_get_press_key & KEY_2){             // ->
                        if (file_numer != files_found )
                                file_numer++;
                        else
                                file_numer = 1;
                }
                lcd_set_cursor_position(0,1,1);                                         // печатаем на жки название меню
                lcd_write_string(0, "Select File Firmware");

                lcd_set_cursor_position(0,1,2);
                lcd_write_string(0, "                    ");                            // чистим место от мусора, (имя предыдущего файла могло оказатся длинней)
                lcd_set_cursor_position(0,1,2);                                         // печатаем на жки имя файла для прграмирования
                lcd_write_string(0, "Files: ");
                lcd_write_string(0, file_list_array_st.file_name[ file_numer - 1 ] );
                        
                lcd_set_cursor_position(0,1,3);                                         // печатаем на жки номер текущего файла из возможных
                lcd_write_string(0, "< ");
                xsprintf(string_for_sprint, "%d", file_numer);
                lcd_write_string(0,string_for_sprint);
                lcd_write_string(0, " / ");
                xsprintf(string_for_sprint, "%d", files_found);
                lcd_write_string(0,string_for_sprint);
                lcd_write_string(0, " >");

                lcd_set_cursor_position(0,1,4);                                         // печатаем на жки MENU
                lcd_write_string(0, txt_menu_top);
        break;

        case MP_PROG:

                if (keybord_get_press_key & KEY_4){             // exit
                        menu_position_e = MP_TOP;
                        os_task_push( TASK_MENU_ID );                           // положение меню изменилось нужно вызвать саму себя для обновления содеожимого ЖКИ
                }
                if (keybord_get_press_key & KEY_2){             // start
                        menu_position_e = MP_PROG;
                        os_task_push( TASK_JTAG_ID );                           
                }
                lcd_set_cursor_position(0,1,1);                                         // печатаем на жки название меню
                lcd_write_string(0, "---Write Firmware---");
                
                lcd_set_cursor_position(0,1,2);                                         // печатаем на жки имя файла для прграмирования
                lcd_write_string(0, "Files: ");
                lcd_write_string(0, file_list_array_st.file_name[ file_numer - 1 ] );

                lcd_set_cursor_position(0,1,3);
                lcd_write_string(0, "                    ");
                        
                lcd_set_cursor_position(0,1,4);                                         // печатаем на жки MENU
                lcd_write_string(0, txt_menu_prog);
        break;

        case MP_STOP:
  
        break;
        }
        
        keybord_get_press_key = 0;              // сбросс нажатых клавиш иначе зацикливаемся
}

void wait_key_press(void)
{
        lcd_set_cursor_position(0,1,4);                                         
        lcd_write_string(0, txt_wait_key_press);
        
        while(read_keybord()==0);
        delay_ms(20);
        while(read_keybord()!=0);
        delay_ms(200);
}
