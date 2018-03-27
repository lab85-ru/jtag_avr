#ifndef KEOS_H_
#define KEOS_H_

#include <stdint.h>

#define DELAY_100MS     (100) // const delay for 100 msec
#define DELAY_300MS     (300) // const delay for 300 msec

#define DELAY_1S        (1000) // const delay for 1 sec
#define DELAY_3S        (3000) // const delay for 3 sec
#define DELAY_5S        (5000) // const delay for 5 sec
#define DELAY_10S       (10000) // const delay for 10 sec
#define DELAY_20S       (20000) // const delay for 20 sec

typedef enum {TIR_WAIT, TIR_OUT} tir_e;                          // result return for delta intervals

// queue for task size
#define OS_TASK_QUEUE_SIZE	(8)

typedef struct os_task_queue_structure{
	int queue[OS_TASK_QUEUE_SIZE];
	int head;
	int tail;
}os_task_q_st;

// structura sohranaem vrema poslednego zapuska zadachi
// i interval cherez kotoriy zadacha dolhna zapuskatsa
struct task_run_time{
	int prev_time_of_start;
	int period;
};


// structure for one task
typedef struct os_task_structure{
	int task_id_numer;
	void (*task_call)();
	struct task_run_time *rt;
}os_onetask_st;


#include "keos_task_list.h"


tir_e time_interval_calcul(uint32_t time_last, uint32_t timer_interval);

// Kernel Event OS (system calls)
void os_init( void );
void os_delay(uint32_t delay);
int os_task_queue_free(os_task_q_st *q);
int os_task_queue_is_present(os_task_q_st *q);
int os_task_push_q(os_task_q_st *q, int task_id);
int os_task_push(int task_id);
int os_task_pop(os_task_q_st *q, int *task_id);
void os_sleep( void );
void os_scheduler(void);




#endif