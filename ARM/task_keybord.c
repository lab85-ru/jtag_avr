#include "keos.h"
#include "keybord.h"

extern unsigned char keybord_get_press_key;

void task_keybord( void )
{
        keybord_get_press_key = read_keybord();
        if (keybord_get_press_key)
                os_task_push( TASK_MENU_ID );
}