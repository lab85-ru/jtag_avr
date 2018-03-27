/*
 * prvsetuphardware.h
 *
 *  Created on: 06.09.2010
 *      Author: Администратор
 */

#ifndef INIT_HARDWARE_H_
#define INIT_HARDWARE_H_
#include <stdint.h>
#include "stm32f10x.h"

//------------------------------------------------------------------------------
// port for JTAG IO

//out
#define JTAG_TCK_PIN                         GPIO_Pin_10
#define JTAG_TCK_GPIO_PORT                   GPIOB
#define JTAG_TCK_GPIO_CLK                    RCC_APB2Periph_GPIOB

// input
#define JTAG_TDO_PIN                         GPIO_Pin_13
#define JTAG_TDO_GPIO_PORT                   GPIOB
#define JTAG_TDO_GPIO_CLK                    RCC_APB2Periph_GPIOB
        
// out
#define JTAG_TMS_PIN                         GPIO_Pin_12
#define JTAG_TMS_GPIO_PORT                   GPIOB
#define JTAG_TMS_GPIO_CLK                    RCC_APB2Periph_GPIOB

// out
#define JTAG_TDI_PIN                         GPIO_Pin_11
#define JTAG_TDI_GPIO_PORT                   GPIOB
#define JTAG_TDI_GPIO_CLK                    RCC_APB2Periph_GPIOB

//input
#define JTAG_VCC_PIN                         GPIO_Pin_14
#define JTAG_VCC_GPIO_PORT                   GPIOB
#define JTAG_VCC_GPIO_CLK                    RCC_APB2Periph_GPIOB


#define JTAG_TCK_1                      JTAG_TCK_GPIO_PORT->BSRR = JTAG_TCK_PIN	// =1
#define JTAG_TCK_0                      JTAG_TCK_GPIO_PORT->BRR = JTAG_TCK_PIN	// =0

#define JTAG_TDI_1                      JTAG_TDI_GPIO_PORT->BSRR = JTAG_TDI_PIN	// =1
#define JTAG_TDI_0                      JTAG_TDI_GPIO_PORT->BRR = JTAG_TDI_PIN	// =0

#define JTAG_TMS_1                      JTAG_TMS_GPIO_PORT->BSRR = JTAG_TMS_PIN	// =1
#define JTAG_TMS_0                      JTAG_TMS_GPIO_PORT->BRR = JTAG_TMS_PIN	// =0

#define JTAG_TDO                        JTAG_TDO_GPIO_PORT->IDR & JTAG_TDO_PIN

#define JTAG_VCC                        JTAG_VCC_GPIO_PORT->IDR & JTAG_VCC_PIN



typedef enum {
        COM1 = 0,
        COM2 = 1
 } COM_TypeDef;
#define COMn                             2


/**
 * @brief Definition for COM port1, connected to USART1
 * RING-Consol
 */ 
#define H_COM1                        USART1
#define H_COM1_CLK                    RCC_APB2Periph_USART1
#define H_COM1_TX_PIN                 GPIO_Pin_9
#define H_COM1_TX_GPIO_PORT           GPIOA
#define H_COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define H_COM1_RX_PIN                 GPIO_Pin_10
#define H_COM1_RX_GPIO_PORT           GPIOA
#define H_COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define H_COM1_IRQn                   USART1_IRQn

/**
 * @brief Definition for COM port2, connected to USART2
 */ 
#define H_COM2                        USART3
#define H_COM2_CLK                    RCC_APB1Periph_USART3
#define H_COM2_TX_PIN                 GPIO_Pin_10
#define H_COM2_TX_GPIO_PORT           GPIOB
#define H_COM2_TX_GPIO_CLK            RCC_APB2Periph_GPIOB
#define H_COM2_RX_PIN                 GPIO_Pin_11
#define H_COM2_RX_GPIO_PORT           GPIOB
#define H_COM2_RX_GPIO_CLK            RCC_APB2Periph_GPIOB
#define H_COM2_IRQn                   USART3_IRQn


void init_hardware(void);
void output_gpio_byte(char b);

void com_init(uint8_t port_num, uint32_t baud);
void com_tx_start( uint8_t port_num );
void nvic_config_com1(FunctionalState st);
void nvic_config_com2(FunctionalState st);
void stm32_uart_set(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct, FunctionalState st);
void stm32_uart_init(COM_TypeDef COM);




#endif /* PRVSETUPHARDWARE_H_ */
