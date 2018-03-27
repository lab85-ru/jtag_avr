//***************************************************************************//**
//EEPROM LIB v 1.0
//******************************************************************************/
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"
#include "main.h"

#include "i2c.h"

#define EEPROM_HW_ADDRESS       0xA0   /* E0 = E1 = E2 = 0 */
#define I2C_EE                  I2C1  //interface number

/***************************************************************************//**
 *  @brief  I2C Configuration
 ******************************************************************************/
void i2c_init(void)
{

   	   I2C_InitTypeDef  I2C_InitStructure;
	   GPIO_InitTypeDef  GPIO_InitStructure;

	   RCC_APB1PeriphClockCmd( RCC_APB1Periph_I2C1, ENABLE);

	   RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);

	   /* Configure I2C1 pins: PB6->SCL and PB7->SDA */
	   /* Configure I2C2 pins: PB10->SCL and PB11->SDA */
           
	   GPIO_InitStructure.GPIO_Pin =                GPIO_Pin_6 | GPIO_Pin_7;
	   GPIO_InitStructure.GPIO_Speed =              GPIO_Speed_50MHz;
	   GPIO_InitStructure.GPIO_Mode =               GPIO_Mode_AF_OD;
	   GPIO_Init(GPIOB, &GPIO_InitStructure);

	   I2C_DeInit(I2C_EE);
	   I2C_InitStructure.I2C_Mode =                 I2C_Mode_I2C;
	   I2C_InitStructure.I2C_DutyCycle =            I2C_DutyCycle_16_9;
	   I2C_InitStructure.I2C_OwnAddress1 =          1;
	   I2C_InitStructure.I2C_Ack =                  I2C_Ack_Enable;
	   I2C_InitStructure.I2C_AcknowledgedAddress =  I2C_AcknowledgedAddress_7bit;
	   I2C_InitStructure.I2C_ClockSpeed =           100000;  /* 100kHz */

	   I2C_Cmd(I2C_EE, ENABLE);
	   I2C_Init(I2C_EE, &I2C_InitStructure);
	   I2C_AcknowledgeConfig(I2C_EE, ENABLE);

}

//*******************************************************************8
//***************************************************************

void I2C_EE_ByteWrite(uint16_t WriteAddr, uint8_t val)
{


    /* Send START condition */
    I2C_GenerateSTART(I2C_EE, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C_EE, EEPROM_HW_ADDRESS, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));


    /* Send the EEPROM's internal address to write to : MSB of the address first */
//    I2C_SendData(I2C_EE, (uint8_t)((WriteAddr & 0xFF00) >> 8));

    /* Test on EV8 and clear it */
//    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));



    /* Send the EEPROM's internal address to write to : LSB of the address */
    I2C_SendData(I2C_EE, (uint8_t)(WriteAddr & 0x00FF));

    /* Test on EV8 and clear it */
    while(! I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));


     I2C_SendData(I2C_EE, val);

        /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send STOP condition */
    I2C_GenerateSTOP(I2C_EE, ENABLE);

    //delay between write and read...not less 4ms
//    Delay_ms(5);
//	vTaskDelay(10);
}
//*********************************************************************************
uint8_t I2C_EE_ByteRead( uint16_t ReadAddr)
{
    uint8_t tmp;

	/* While the bus is busy */
    while(I2C_GetFlagStatus(I2C_EE, I2C_FLAG_BUSY));

    /* Send START condition */
    I2C_GenerateSTART(I2C_EE, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C_EE, EEPROM_HW_ADDRESS, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));


    /* Send the EEPROM's internal address to read from: MSB of the address first */
//    I2C_SendData(I2C_EE, (uint8_t)((ReadAddr & 0xFF00) >> 8));

    /* Test on EV8 and clear it */
//    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send the EEPROM's internal address to read from: LSB of the address */
    I2C_SendData(I2C_EE, (uint8_t)(ReadAddr & 0x00FF));

    /* Test on EV8 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));


    /* Send STRAT condition a second time */
    I2C_GenerateSTART(I2C_EE, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for read */
    I2C_Send7bitAddress(I2C_EE, EEPROM_HW_ADDRESS, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(I2C_EE,I2C_EVENT_MASTER_BYTE_RECEIVED));//I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    tmp=I2C_ReceiveData(I2C_EE);


    I2C_AcknowledgeConfig(I2C_EE, DISABLE);

    /* Send STOP Condition */
    I2C_GenerateSTOP(I2C_EE, ENABLE);

    return tmp;
}
//*******************************************************************************
unsigned char I2C_paketRead( uint8_t addr_i2c_device, uint8_t *buf, uint8_t len)
{
    uint32_t status;
    
    /* While the bus is busy */
    while(I2C_GetFlagStatus(I2C_EE, I2C_FLAG_BUSY));

    /* Send START condition */
//    I2C_GenerateSTART(I2C_EE, ENABLE);

    /* Test on EV5 and clear it */
//    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for write */
//    I2C_Send7bitAddress(I2C_EE, addr_i2c_device, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
//    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    /* Send the EEPROM's internal address to read from: LSB of the address */
//    I2C_SendData(I2C_EE, addr_start);

    /* Test on EV8 and clear it */
//    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send STRAT condition a second time */
    I2C_GenerateSTART(I2C_EE, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for read */
    I2C_Send7bitAddress(I2C_EE, addr_i2c_device, I2C_Direction_Receiver);
    
    while(1){
        status = I2C_GetLastEvent(I2C_EE);
        if ((status & I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)
            break;
        if (status & I2C_FLAG_AF){// ERROR: NO ASK.
            I2C_ClearFlag(I2C_EE, I2C_FLAG_AF);
            /* Send STOP Condition */
            I2C_GenerateSTOP(I2C_EE, ENABLE);
            return I2C_ERROR_ADDR;
        }
    }

    while (len>0){
        /* Test on EV6 and clear it */
        while(!I2C_CheckEvent(I2C_EE,I2C_EVENT_MASTER_BYTE_RECEIVED));//I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

        *(buf) = I2C_ReceiveData(I2C_EE);
        buf++;
        len--;
    }

    I2C_AcknowledgeConfig(I2C_EE, DISABLE);

    /* Send STOP Condition */
    I2C_GenerateSTOP(I2C_EE, ENABLE);
    
    return I2C_OK;
}
//*****************************************************************************
unsigned char I2C_paketWrite( uint8_t addr_i2c_device, uint8_t *buf, uint8_t len)
{
    uint32_t status;

    /* Send START condition */
    I2C_GenerateSTART(I2C_EE, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C_EE, addr_i2c_device, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
//    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    while(1){
        status = I2C_GetLastEvent(I2C_EE);
        if ((status & I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED )
            break;
        if (status & I2C_FLAG_AF){// ERROR: NO ASK.
            I2C_ClearFlag(I2C_EE, I2C_FLAG_AF);
            /* Send STOP Condition */
            I2C_GenerateSTOP(I2C_EE, ENABLE);
            return I2C_ERROR_ADDR;
        }
    }


    /* Send the EEPROM's internal address to write to : LSB of the address */
//    I2C_SendData(I2C_EE, addr_start);

    /* Test on EV8 and clear it */
//    while(! I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    while(len>0){
        I2C_SendData(I2C_EE, *buf);
        buf++;
        len--;

        /* Test on EV8 and clear it */
        while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    }
    
    
    /* Send STOP condition */
    I2C_GenerateSTOP(I2C_EE, ENABLE);

    return I2C_OK;
}
//*****************************************************************************
//
//*****************************************************************************
int pcf8574_read( int file, int devaddr, unsigned char *data)
{
    //uint8_t tmp;

	/* While the bus is busy */
    while(I2C_GetFlagStatus(I2C_EE, I2C_FLAG_BUSY));

    /* Send START condition */
    I2C_GenerateSTART(I2C_EE, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for read */
    I2C_Send7bitAddress(I2C_EE, devaddr, I2C_Direction_Receiver);

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(I2C_EE,I2C_EVENT_MASTER_BYTE_RECEIVED));

    *data = I2C_ReceiveData(I2C_EE);

    I2C_AcknowledgeConfig(I2C_EE, DISABLE);

    /* Send STOP Condition */
    I2C_GenerateSTOP(I2C_EE, ENABLE);

    //return tmp;
    return I2C_OK;

    
  
  
//	pthread_mutex_lock( &i2c_mutex );
	
//	if(ioctl(file, I2C_SLAVE, devaddr)<0) {/* Correct address */
//	    log_out("ERROR pcf8574_read i2c ioctl.");
//	    pthread_mutex_unlock( &i2c_mutex );
//	    return I2C_ERROR_ADDR;
//    }

//    if (read(file, data, 1) != 1){
//		log_out("ERROR pcf8574_read i2c read.");
//		pthread_mutex_unlock( &i2c_mutex );
//		return I2C_ERROR_RW;
//    }
	
//	pthread_mutex_unlock( &i2c_mutex );
//	return I2C_OK;	
}

//*****************************************************************************
//
//*****************************************************************************
int pcf8574_write( int file, int devaddr, unsigned char byte_w)
{


    /* Send START condition */
    I2C_GenerateSTART(I2C_EE, ENABLE);

    /* Test on EV5 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_MODE_SELECT));

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C_EE, devaddr, I2C_Direction_Transmitter);

    /* Test on EV6 and clear it */
    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));


    /* Send the EEPROM's internal address to write to : MSB of the address first */
//    I2C_SendData(I2C_EE, (uint8_t)((WriteAddr & 0xFF00) >> 8));

    /* Test on EV8 and clear it */
//    while(!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));



    /* Send the EEPROM's internal address to write to : LSB of the address */
    I2C_SendData(I2C_EE, (uint8_t)(byte_w));

    /* Test on EV8 and clear it */
    while(! I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));


//     I2C_SendData(I2C_EE, val);

        /* Test on EV8 and clear it */
//    while (!I2C_CheckEvent(I2C_EE, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    /* Send STOP condition */
    I2C_GenerateSTOP(I2C_EE, ENABLE);
    //return tmp;
    return I2C_OK;

  
//	pthread_mutex_lock( &i2c_mutex );
//	if(ioctl(file, I2C_SLAVE, devaddr)<0) {/* Correct address */
//	    log_out("ERROR pcf8574_write i2c ioctl.");
//	    pthread_mutex_unlock( &i2c_mutex );
//	    return I2C_ERROR_ADDR;
//    }

//	if (write(file, &byte_w, 1) != 1){
//		log_out("ERROR pcf8574_write i2c sending 1 byte");
//		pthread_mutex_unlock( &i2c_mutex );
//       	return I2C_ERROR_RW;
//   	}

//	pthread_mutex_unlock( &i2c_mutex );
	return I2C_OK;	
}




