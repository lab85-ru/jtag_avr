#include "main.h"
#include "ff.h"
#include "diskio.h"
#include "i2c.h"
#include "jtag.h"
#include "hardware.h"
#include "task_menu.h"
#include "lcd.h"


extern FIL f_file;
extern volatile FRESULT f_result;
extern file_list_array_struct file_list_array_st;
extern int file_numer; 

//------------------------------------------------------------------------------
void task_jtag(void)
{

        f_result = f_open( &f_file, file_list_array_st.file_name[ file_numer - 1 ], FA_READ);
        printf_d("f_open= ");
        print_result(f_result);
        if (f_result != FR_OK){
                printf_d("Error open files = %s\r\n",file_list_array_st.file_name[ file_numer - 1 ]);
                lcd_clear_display(0);
                lcd_set_cursor_position(0,1,1);
                lcd_write_string(0, "Error: Open File.   ");
                wait_key_press();
                return;
        }

        prog_jtag(&f_file);
                
        f_close(&f_file);  
        
        lcd_clear_display(0);
        os_task_push( TASK_MENU_ID );
}

