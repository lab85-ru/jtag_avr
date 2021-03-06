#include "stm32f10x.h"
#include "stm32f10x_iwdg.h"
#include "sys_timer.h"


volatile unsigned int sys_tick_counter = 0; // ������� ���������� �������

//------------------------------------------------------------------------------
// ���������� ������� �������� ���������� �������
//------------------------------------------------------------------------------
unsigned int get_sys_counter( void )
{
	return sys_tick_counter;
}

//------------------------------------------------------------------------------
// ���������� ���������� �� ���������� �������
//------------------------------------------------------------------------------
void sys_tick_handler( void )
{
	/* Reload IWDG counter */
	IWDG_ReloadCounter();
        sys_tick_counter++;
}

//------------------------------------------------------------------------------
// ��������� ���������� �������
// configCPU_CLOCK_HZ �������� ������� ����
// configTICK_RATE_HZ ������� ����� �������
//------------------------------------------------------------------------------
void sys_timer_setup( void )
{
        SysTick_Config( (configCPU_CLOCK_HZ) / (configTICK_RATE_HZ) );
}

