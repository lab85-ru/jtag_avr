#ifndef SYS_TIMER_H_
#define SYS_TIMER_H_

#define configCPU_CLOCK_HZ      72000000U
#define configTICK_RATE_HZ      1000U



unsigned int get_sys_counter( void );
void sys_tick_handler( void );
void sys_timer_setup( void );


#endif