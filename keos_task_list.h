#ifndef KEOS_TASK_LIST_H_
#define KEOS_TASK_LIST_H_

// WARNING TASK_ID dolhen bit UNIKALNIM !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#define TASK_CONSOL_ID			        1               // TASK_ID (number 1-xxx)

#define TASK_XMODEM_ID			        2

#define TASK_MENU_ID			        3               // TASK_ID (number 1-xxx)

#define TASK_KEYBORD_ID			        4               // TASK_ID (number 1-xxx)
#define TASK_KEYBORD_PERIOD		        DELAY_100MS        // Period run task (po timers) [var = system tick]
extern struct task_run_time task_rt_keybord;

#define TASK_JTAG_ID			        5               // TASK_ID (number 1-xxx)


// List all  Task
//------------------------------------------------------------------------------
// structure for List all task this current systems.(ROM)
//------------------------------------------------------------------------------
extern const os_onetask_st os_task_list_array[];
//------------------------------------------------------------------------------

// Task of numbers (all)
extern const int TASK_LIST_NUMBER;

#endif