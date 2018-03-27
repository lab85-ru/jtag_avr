/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "stm32f10x_it.h"
#include "hardware.h"
#include "main.h"
#include "sys_timer.h"
//#include "stm32_fault.h"
#include "xprint_fault.h"
#include "task_consol.h"
#include "consol.h"
    
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//extern device_status_t device_status;
extern consol_flag_enum consol_flag_e;
extern consol_structure consol_st;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/
/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
/*	asm volatile (
        	"TST LR, #4           \n"
	        "ITE EQ               \n"
        	"MRSEQ R0, MSP        \n"
	        "MRSNE R0, PSP        \n"
        	"B nmi_fault_handler_c    "
	);
  */
	while (1);
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
/*	asm volatile (
        	"TST LR, #4           \n"
	        "ITE EQ               \n"
        	"MRSEQ R0, MSP        \n"
	        "MRSNE R0, PSP        \n"
        	"B hard_fault_handler_c    "
	);
  */
	while (1);
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
/*	asm volatile (
        	"TST LR, #4           \n"
	        "ITE EQ               \n"
        	"MRSEQ R0, MSP        \n"
	        "MRSNE R0, PSP        \n"
        	"B mem_manage_fault_handler_c    "
	);*/
	while (1);
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
/* 	asm volatile (
        	"TST LR, #4           \n"
	        "ITE EQ               \n"
        	"MRSEQ R0, MSP        \n"
	        "MRSNE R0, PSP        \n"
        	"B bus_fault_handler_c    "
	);*/
	while (1);
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
/*	asm volatile (
        	"TST LR, #4           \n"
	        "ITE EQ               \n"
        	"MRSEQ R0, MSP        \n"
	        "MRSNE R0, PSP        \n"
        	"B usage_fault_handler_c    "
	);
  */
	while (1);
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
	//vPortSVCHandler();
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
/*  	asm volatile (
        	"TST LR, #4           \n"
	        "ITE EQ               \n"
        	"MRSEQ R0, MSP        \n"
	        "MRSNE R0, PSP        \n"
        	"B debug_monitor_fault_handler_c    "
	);*/
	while (1);

}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
	//xPortPendSVHandler();
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	sys_tick_handler();
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
//------------------------------------------------------------------------------
// Consol
//------------------------------------------------------------------------------
void USART1_IRQHandler(void)
{
	uint8_t c;
        int i;

	if(USART_GetITStatus(H_COM1, USART_IT_RXNE) != RESET){
		c = (USART_ReceiveData(H_COM1) & 0xFF);
                switch( consol_flag_e ){
                case FLAG_XMODEM:
                        if ( consol_st.n < CONSOL_BUF_SIZE ){
                                consol_st.buf[ consol_st.n ] = c;
                                consol_st.n++;
                                break;
                        }
                case FLAG_CONSOL:
               		if ( consol_st.n < CONSOL_BUF_SIZE && c != KEY_BACKSPACE && c != KEY_ENTER){
                                consol_st.buf[ consol_st.n ] = c;
                                consol_st.n++;
                                if (USART_GetFlagStatus(H_COM1, USART_FLAG_TC) != RESET)        // выдаем обратно символ если уарт не занят
                                        USART_SendData(H_COM1, c);
                        }else consol_st.buf[ CONSOL_BUF_SIZE-1 ] = 0;                           // самый последний символ 0 (конец строки, для защиты от переполнения при работе со строковыми функциями )
                        if (c == KEY_BACKSPACE){
                                if (consol_st.n != 0) consol_st.n--;
                        }
                        if (c == KEY_ENTER){
                                if ( strlen(consol_st.string) == 0){
                                        for(i=0;i<consol_st.n;i++)
                                                consol_st.string[i] = consol_st.buf[i];
                                        consol_st.string[ consol_st.n ] = 0;                    // за место enter добавляем в конец строки \0
                                        consol_st.n = 0;
                                        os_task_push( TASK_CONSOL_ID );
                                }
                        }
                        break;
                default:
                        break;
                }
	}

	if(USART_GetITStatus(H_COM1, USART_IT_TXE) != RESET){   
		USART_ITConfig(H_COM1, USART_IT_TXE, DISABLE);
	}
        
}


/*
void hard_fault_handler_c(unsigned int * hardfault_args)
{
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;

	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);

	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);

	xprint_str("\r\n[Hard fault handler]\r\n");

	xprint_str("R0 = ");
	xprint_hexuint32(stacked_r0);
	xprint_str("\r\n");

	xprint_str("R1 = ");
	xprint_hexuint32(stacked_r1);
	xprint_str("\r\n");

	xprint_str("R2 = ");
	xprint_hexuint32(stacked_r2);
	xprint_str("\r\n");

	xprint_str("R3 = ");
	xprint_hexuint32(stacked_r3);
	xprint_str("\r\n");

	xprint_str("R12 = ");
	xprint_hexuint32(stacked_r12);
	xprint_str("\r\n");

	xprint_str("LR [R14] subroutine call return address = ");
	xprint_hexuint32(stacked_lr);
	xprint_str("\r\n");

	xprint_str("PC [R15] program counter = ");
	xprint_hexuint32(stacked_pc);
	xprint_str("\r\n");

	xprint_str("PSR = "); 
	xprint_hexuint32(stacked_psr); 
	xprint_str("\r\n");
	while(1);
}

void nmi_fault_handler_c(unsigned int * hardfault_args)
{
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;

	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);

	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);

	xprint_str("\r\n[NMI fault handler]\r\n");

	xprint_str("R0 = ");
	xprint_hexuint32(stacked_r0);
	xprint_str("\r\n");

	xprint_str("R1 = ");
	xprint_hexuint32(stacked_r1);
	xprint_str("\r\n");

	xprint_str("R2 = ");
	xprint_hexuint32(stacked_r2);
	xprint_str("\r\n");

	xprint_str("R3 = ");
	xprint_hexuint32(stacked_r3);
	xprint_str("\r\n");

	xprint_str("R12 = ");
	xprint_hexuint32(stacked_r12);
	xprint_str("\r\n");

	xprint_str("LR [R14] subroutine call return address = ");
	xprint_hexuint32(stacked_lr);
	xprint_str("\r\n");

	xprint_str("PC [R15] program counter = ");
	xprint_hexuint32(stacked_pc);
	xprint_str("\r\n");

	xprint_str("PSR = "); 
	xprint_hexuint32(stacked_psr); 
	xprint_str("\r\n");
	while(1);
}
*/
/*
void mem_manage_fault_handler_c(unsigned int * hardfault_args)
{
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;

	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);

	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);

	xprint_str("\r\n[MemManage fault handler]\r\n");

	xprint_str("R0 = ");
	xprint_hexuint32(stacked_r0);
	xprint_str("\r\n");

	xprint_str("R1 = ");
	xprint_hexuint32(stacked_r1);
	xprint_str("\r\n");

	xprint_str("R2 = ");
	xprint_hexuint32(stacked_r2);
	xprint_str("\r\n");

	xprint_str("R3 = ");
	xprint_hexuint32(stacked_r3);
	xprint_str("\r\n");

	xprint_str("R12 = ");
	xprint_hexuint32(stacked_r12);
	xprint_str("\r\n");

	xprint_str("LR [R14] subroutine call return address = ");
	xprint_hexuint32(stacked_lr);
	xprint_str("\r\n");

	xprint_str("PC [R15] program counter = ");
	xprint_hexuint32(stacked_pc);
	xprint_str("\r\n");

	xprint_str("PSR = "); 
	xprint_hexuint32(stacked_psr); 
	xprint_str("\r\n");
	while(1);
}

void bus_fault_handler_c(unsigned int * hardfault_args)
{
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;

	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);

	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);

	xprint_str("\r\n[Bus fault handler]\r\n");

	xprint_str("R0 = ");
	xprint_hexuint32(stacked_r0);
	xprint_str("\r\n");

	xprint_str("R1 = ");
	xprint_hexuint32(stacked_r1);
	xprint_str("\r\n");

	xprint_str("R2 = ");
	xprint_hexuint32(stacked_r2);
	xprint_str("\r\n");

	xprint_str("R3 = ");
	xprint_hexuint32(stacked_r3);
	xprint_str("\r\n");

	xprint_str("R12 = ");
	xprint_hexuint32(stacked_r12);
	xprint_str("\r\n");

	xprint_str("LR [R14] subroutine call return address = ");
	xprint_hexuint32(stacked_lr);
	xprint_str("\r\n");

	xprint_str("PC [R15] program counter = ");
	xprint_hexuint32(stacked_pc);
	xprint_str("\r\n");

	xprint_str("PSR = "); 
	xprint_hexuint32(stacked_psr); 
	xprint_str("\r\n");
	while(1);
}

void usage_fault_handler_c(unsigned int * hardfault_args)
{
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;

	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);

	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);

	xprint_str("\r\n[Usage fault handler]\r\n");

	xprint_str("R0 = ");
	xprint_hexuint32(stacked_r0);
	xprint_str("\r\n");

	xprint_str("R1 = ");
	xprint_hexuint32(stacked_r1);
	xprint_str("\r\n");

	xprint_str("R2 = ");
	xprint_hexuint32(stacked_r2);
	xprint_str("\r\n");

	xprint_str("R3 = ");
	xprint_hexuint32(stacked_r3);
	xprint_str("\r\n");

	xprint_str("R12 = ");
	xprint_hexuint32(stacked_r12);
	xprint_str("\r\n");

	xprint_str("LR [R14] subroutine call return address = ");
	xprint_hexuint32(stacked_lr);
	xprint_str("\r\n");

	xprint_str("PC [R15] program counter = ");
	xprint_hexuint32(stacked_pc);
	xprint_str("\r\n");

	xprint_str("PSR = "); 
	xprint_hexuint32(stacked_psr); 
	xprint_str("\r\n");
	while(1);
}
*/
/*
void debug_monitor_fault_handler_c(unsigned int * hardfault_args)
{
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;

	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);

	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);

	xprint_str("\r\n[Debug Monitor fault handler]\r\n");

	xprint_str("R0 = ");
	xprint_hexuint32(stacked_r0);
	xprint_str("\r\n");

	xprint_str("R1 = ");
	xprint_hexuint32(stacked_r1);
	xprint_str("\r\n");

	xprint_str("R2 = ");
	xprint_hexuint32(stacked_r2);
	xprint_str("\r\n");

	xprint_str("R3 = ");
	xprint_hexuint32(stacked_r3);
	xprint_str("\r\n");

	xprint_str("R12 = ");
	xprint_hexuint32(stacked_r12);
	xprint_str("\r\n");

	xprint_str("LR [R14] subroutine call return address = ");
	xprint_hexuint32(stacked_lr);
	xprint_str("\r\n");

	xprint_str("PC [R15] program counter = ");
	xprint_hexuint32(stacked_pc);
	xprint_str("\r\n");

	xprint_str("PSR = "); 
	xprint_hexuint32(stacked_psr); 
	xprint_str("\r\n");
	while(1);
}

void svc_fault_handler_c(unsigned int * hardfault_args)
{
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;

	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);

	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);

	xprint_str("\r\n[SVC fault handler]\r\n");

	xprint_str("R0 = ");
	xprint_hexuint32(stacked_r0);
	xprint_str("\r\n");

	xprint_str("R1 = ");
	xprint_hexuint32(stacked_r1);
	xprint_str("\r\n");

	xprint_str("R2 = ");
	xprint_hexuint32(stacked_r2);
	xprint_str("\r\n");

	xprint_str("R3 = ");
	xprint_hexuint32(stacked_r3);
	xprint_str("\r\n");

	xprint_str("R12 = ");
	xprint_hexuint32(stacked_r12);
	xprint_str("\r\n");

	xprint_str("LR [R14] subroutine call return address = ");
	xprint_hexuint32(stacked_lr);
	xprint_str("\r\n");

	xprint_str("PC [R15] program counter = ");
	xprint_hexuint32(stacked_pc);
	xprint_str("\r\n");

	xprint_str("PSR = "); 
	xprint_hexuint32(stacked_psr); 
	xprint_str("\r\n");
	while(1);
}

void pend_svc_fault_handler_c(unsigned int * hardfault_args)
{
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;

	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);

	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);

	xprint_str("\r\n[PendSVC fault handler]\r\n");

	xprint_str("R0 = ");
	xprint_hexuint32(stacked_r0);
	xprint_str("\r\n");

	xprint_str("R1 = ");
	xprint_hexuint32(stacked_r1);
	xprint_str("\r\n");

	xprint_str("R2 = ");
	xprint_hexuint32(stacked_r2);
	xprint_str("\r\n");

	xprint_str("R3 = ");
	xprint_hexuint32(stacked_r3);
	xprint_str("\r\n");

	xprint_str("R12 = ");
	xprint_hexuint32(stacked_r12);
	xprint_str("\r\n");

	xprint_str("LR [R14] subroutine call return address = ");
	xprint_hexuint32(stacked_lr);
	xprint_str("\r\n");

	xprint_str("PC [R15] program counter = ");
	xprint_hexuint32(stacked_pc);
	xprint_str("\r\n");

	xprint_str("PSR = "); 
	xprint_hexuint32(stacked_psr); 
	xprint_str("\r\n");
	while(1);
}
*/


/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
