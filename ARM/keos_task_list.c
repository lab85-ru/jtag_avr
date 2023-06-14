#include "keos.h"
#include "task_consol.h"
#include "task_xmodem.h"
#include "task_menu.h"
#include "task_keybord.h"
#include "task_jtag.h"

struct task_run_time task_rt_keybord =              {0, TASK_KEYBORD_PERIOD };

// List all  Task
//------------------------------------------------------------------------------
// structure for List all task this current systems.(ROM)
//------------------------------------------------------------------------------
const os_onetask_st os_task_list_array[] = {
	{TASK_CONSOL_ID,      task_consol,	        0                       },
	{TASK_XMODEM_ID,      task_xmodem,	        0                       },
   	{TASK_MENU_ID,        task_menu,	        0                       },
   	{TASK_KEYBORD_ID,     task_keybord,	        &task_rt_keybord        },
   	{TASK_JTAG_ID,        task_jtag,	        0                       }

};
//------------------------------------------------------------------------------

// Task of numbers (all)
const int TASK_LIST_NUMBER = (sizeof(os_task_list_array) / sizeof(struct os_task_structure));


