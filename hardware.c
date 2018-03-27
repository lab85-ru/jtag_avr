/*
 * prvsetuphardware.c
 *
 *  Created on: 06.09.2010
 *      Author: Администратор
 */

#include <stdint.h>
#include <string.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "main.h"
#include "hardware.h"
#include "i2c.h"
#include "env_config.h"

#ifndef DEBUG
#define DEBUG 0
#endif

USART_TypeDef* const COM_USART[COMn] = {H_COM1, H_COM2};

GPIO_TypeDef* const COM_TX_PORT[COMn] = {H_COM1_TX_GPIO_PORT, H_COM2_TX_GPIO_PORT};
GPIO_TypeDef* const COM_RX_PORT[COMn] = {H_COM1_RX_GPIO_PORT, H_COM2_RX_GPIO_PORT};
const uint32_t COM_USART_CLK[COMn] = {H_COM1_CLK, H_COM2_CLK};
const uint32_t COM_TX_PORT_CLK[COMn] = {H_COM1_TX_GPIO_CLK, H_COM2_TX_GPIO_CLK};
const uint32_t COM_RX_PORT_CLK[COMn] = {H_COM1_RX_GPIO_CLK, H_COM2_RX_GPIO_CLK};
const uint16_t COM_TX_PIN[COMn] = {H_COM1_TX_PIN, H_COM2_TX_PIN};
const uint16_t COM_RX_PIN[COMn] = {H_COM1_RX_PIN, H_COM2_RX_PIN};

void init_hardware(void)
{
        //int i;
        GPIO_InitTypeDef  GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;

        i2c_init();
        
        // set out gpio JTAG_TCK
	RCC_APB2PeriphClockCmd(JTAG_TCK_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = JTAG_TCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(JTAG_TCK_GPIO_PORT, &GPIO_InitStructure);

        // set out gpio JTAG_TDI
	RCC_APB2PeriphClockCmd(JTAG_TDI_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = JTAG_TDI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(JTAG_TDI_GPIO_PORT, &GPIO_InitStructure);

        // set out gpio JTAG_TMS
	RCC_APB2PeriphClockCmd(JTAG_TMS_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = JTAG_TMS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(JTAG_TMS_GPIO_PORT, &GPIO_InitStructure);

        // set IN gpio JTAG_TDO
	RCC_APB2PeriphClockCmd(JTAG_TDO_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = JTAG_TDO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(JTAG_TDO_GPIO_PORT, &GPIO_InitStructure);

        // set out gpio JTAG_VCC
	RCC_APB2PeriphClockCmd(JTAG_VCC_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = JTAG_VCC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(JTAG_VCC_GPIO_PORT, &GPIO_InitStructure);
        
        // UART Init CONSOL
        USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;

	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	stm32_uart_init(COM1);
	stm32_uart_set(COM1, &USART_InitStructure, ENABLE);

        printf_d("Start IWDG(watchdog) interval 280ms.\r\n");
	/* IWDG timeout equal to 280 ms (the timeout may varies due to LSI frequency dispersion) */
	/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	/* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */
	IWDG_SetPrescaler(IWDG_Prescaler_32);

	/* Set counter reload value to 349 */
	IWDG_SetReload(349);

	/* Reload IWDG counter */
	IWDG_ReloadCounter();

	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG_Enable();
}

void com_init(uint8_t port_num, uint32_t baud)
{
	USART_InitTypeDef USART_InitStructure;

	if (port_num<=4){

		USART_InitStructure.USART_BaudRate = baud;//115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

		if (baud)
			stm32_uart_set( (COM_TypeDef)port_num, &USART_InitStructure, ENABLE);
		else
			stm32_uart_set( (COM_TypeDef)port_num, &USART_InitStructure, DISABLE);
	}
}



void com_tx_start( uint8_t port_num )
{
	if (port_num<=4){
		if (DEBUG) printf_d("com_tx_start port=%d\r\n",port_num);
		USART_ITConfig( COM_USART[ port_num ], USART_IT_TXE, ENABLE);// start tx
	}
}




void nvic_config_com1(FunctionalState st)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	#ifdef SOFTOFFSET
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x20000);
	#endif
	/* Enable the USART1 Interrupt */
  	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = st;
  	NVIC_Init(&NVIC_InitStructure);
}

void nvic_config_com2(FunctionalState st)
{
  	NVIC_InitTypeDef NVIC_InitStructure;
	#ifdef SOFTOFFSET
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x20000);
	#endif
  	/* Enable the USART2 Interrupt */
  	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = st;
  	NVIC_Init(&NVIC_InitStructure);
}



/**
  * Configures COM port.
  * set ENABLE-DISABLE
  */
void stm32_uart_set(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct, FunctionalState st)
{
	if( USART_InitStruct->USART_BaudRate != 0 && st == ENABLE ){
		/* USART configuration */
		USART_Init(COM_USART[COM], USART_InitStruct);
		/* Enable USART */
		USART_Cmd(COM_USART[COM], ENABLE);
		/* Enable Int to RX char */
		USART_ITConfig(COM_USART[COM], USART_IT_RXNE, ENABLE);
		return;
	}

	/* DISABLE Int to TX-RX char */
  	USART_ITConfig( COM_USART[ COM ], USART_IT_RXNE, DISABLE);
	USART_ITConfig( COM_USART[ COM ], USART_IT_TXE, DISABLE);
	USART_Cmd(COM_USART[ COM ], DISABLE);
	return;
}


void stm32_uart_init(COM_TypeDef COM)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  switch (COM){
  case COM1:
	  nvic_config_com1(ENABLE);
	  /* Enable GPIO clock */
	  RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM] | RCC_APB2Periph_AFIO, ENABLE);
	  /* Enable UART clock */
	  RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
	  /* Configure USART Tx as alternate function push-pull */
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

	  /* Configure USART Rx as input floating */
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
	  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

	  /* USART configuration */
	  //USART_Init(COM_USART[COM], USART_InitStruct);
	  /* Enable USART */
	  //USART_Cmd(COM_USART[COM], ENABLE);
	  /* Enable Int to RX char */
	  //USART_ITConfig(COM_USART[COM], USART_IT_RXNE, ENABLE);
	  break;
  case COM2:
	  nvic_config_com2(ENABLE);
	  /* Enable GPIO clock */
	  RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM] | RCC_APB2Periph_AFIO, ENABLE);
	  /* Enable UART clock */
	  RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);//RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
	  /* Configure USART Tx as alternate function push-pull */
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

	  /* Configure USART Rx as input floating */
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
	  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

	  /* USART configuration */
	  //USART_Init(COM_USART[COM], USART_InitStruct);
	  /* Enable USART */
	  //USART_Cmd(COM_USART[COM], ENABLE);
	  /* Enable Int to RX char */
	  //USART_ITConfig(COM_USART[COM], USART_IT_RXNE, ENABLE);
	  break;
  default:
	  return;
	  break;
  }

}
