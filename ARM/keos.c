#include <stdint.h>
#include "stm32f10x.h"
#include "keos.h"
//#include "keos_task_list.h"
#include "sys_timer.h"


extern volatile uint32_t sys_tick_counter;

// global OS variable
//static uint32_t sys_timer_counter = 0;
os_task_q_st os_task_q;

//extern int TASK_LIST_NUMBER;


//---------------------------------------------------------------------------
// Измерение заданного временного интервала (для виртуального таймера)
//---------------------------------------------------------------------------
tir_e time_interval_calcul(uint32_t time_last, uint32_t timer_interval)
{
        uint32_t ts;    // currnet sys time 
        uint32_t delta; // delta = currnet sys time - time last
        
        ts = get_sys_counter();       
        if (ts >= time_last){
                delta = ts - time_last;
        }else{
                delta = (uint32_t)0xffffffffUL - time_last + ts;
        }
        
        if (delta >= timer_interval)
                return TIR_OUT;
        else
                return TIR_WAIT;
        
}

//------------------------------------------------------------------------------
// начальная инициализация для внутрених переменых ОС
//------------------------------------------------------------------------------
void os_init( void )
{
	// Timer init
	sys_tick_counter = 0;

	// init os task queue structure reset
	os_task_q.head = 0;
	os_task_q.tail = 0;
}
//------------------------------------------------------------------------------
// задержка в системных тиках
//------------------------------------------------------------------------------
void os_delay(uint32_t delay)
{
        uint32_t t = get_sys_counter();

        while (time_interval_calcul(t, delay) == TIR_WAIT){
                //for (int i = 0; i< 10000; i++)
                __WFI();
                __NOP();
        }

}

//--------------------------------------------------------
//vichislenie svobodnogo mesta v ocheredi zadach
//--------------------------------------------------------
int os_task_queue_free(os_task_q_st *q)
{
	if (q->head == q->tail)
		return OS_TASK_QUEUE_SIZE;
	if (q->head > q->tail)
		return (q->head - q->tail);

	return OS_TASK_QUEUE_SIZE-(q->tail - q->head);
}

//--------------------------------------------------------
// nahohdenie kolichestva zadach v ocheredi zadach na zapusk
//--------------------------------------------------------
int os_task_queue_is_present(os_task_q_st *q)
{
	return OS_TASK_QUEUE_SIZE - os_task_queue_free(q);
}

//--------------------------------------------------------
// pomeshenie zadachi v ochered na vipolnennie
//--------------------------------------------------------
int os_task_push_q(os_task_q_st *q, int task_id)
{
	if ( os_task_queue_free(q) >= 2 ){
		q->queue[q->tail] = task_id;
                q->tail ++;
		if (q->tail == OS_TASK_QUEUE_SIZE)
			q->tail = 0;
		return 0;
	}
	return -1; // ERROR net mesta v ocheridi
}

//--------------------------------------------------------
// pomeshenie zadachi v ochered na vipolnennie
// po task ID
//--------------------------------------------------------
int os_task_push(int task_id)
{
	return os_task_push_q( &os_task_q, task_id);
}

//--------------------------------------------------------
// vibrat zadachu iz ocheridi
//--------------------------------------------------------
int os_task_pop(os_task_q_st *q, int *task_id)
{
	int tn;

	if (os_task_queue_is_present(q) > 0){
		tn = q->queue[q->head];
		q->head ++;
		if (q->head == OS_TASK_QUEUE_SIZE)
			q->head = 0;
		*task_id = tn;
		return 0;
	}
	return -1;
}
//--------------------------------------------------------
// cpu sleep
//--------------------------------------------------------
void os_sleep( void )
{
        __WFI();
        __NOP();
}

//--------------------------------------------------------
// Event OS kernel scheduler
// while(1);
//--------------------------------------------------------
void os_scheduler(void)//*os_task_list_st os_task_list_structure
{
	int tid;				// nomer zadachi na vipolnenie TASK_ID
	struct task_run_time *rt;
	void (*task_call)() = 0;
	int task_nums, i;

	//printf("KEOS SCHEDULER runs.\n");

	task_nums = TASK_LIST_NUMBER;//sizeof(os_task_list_array) / sizeof(struct os_task_structure); // kolichestvo zadach v spiske

//	while(1){
		// obrabotka i zapusk vseh zadach iz ocheridi
		while (os_task_pop(&os_task_q, &tid) == 0){	// izvlech zadachu esli est zadachi v ocheridi
			for (i=0;i<task_nums;i++){
				if (tid == os_task_list_array[i].task_id_numer){
					task_call = os_task_list_array[i].task_call;			// poluchit ukazatel na *task_call iz spika zadach po task_id nomeru zadachi (task_list[ task_id ])
					(*task_call)();										// run task
					break;
				}
			}
		}
	
	// obrabotka i zapusk vseh zadach po timer
	//obhoz i vipolnenie vseh zadach po skisku
	for (i=0;i<task_nums;i++){
		rt = os_task_list_array[i].rt;
		if (rt){
			if ( time_interval_calcul(rt->prev_time_of_start, rt->period) == TIR_OUT){
				rt->prev_time_of_start = get_sys_counter();			// update time run task
				task_call = os_task_list_array[i].task_call;
				(*task_call)();
			}
		}
	}
	
		os_sleep();									// cpu sleep(wait interupts)
//	}//while(1)
}
