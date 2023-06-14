#ifndef I2C_HARD_H_
#define I2C_HARD_H_

#include <stdint.h>

#define I2C_OK			0
#define I2C_ERROR_RW		1
#define I2C_ERROR_ADDR		2

#define U26_ADDR                (0x70)          // LCD
#define U27_ADDR                (0x72)          // Key

void i2c_init(void);
void I2C_EE_ByteWrite(uint16_t WriteAddr, uint8_t val);
uint8_t I2C_EE_ByteRead( uint16_t ReadAddr);
unsigned char I2C_paketRead( uint8_t addr_i2c_device, uint8_t *buf, uint8_t len);
unsigned char I2C_paketWrite( uint8_t addr_i2c_device, uint8_t *buf, uint8_t len);
int pcf8574_read( int file, int devaddr, unsigned char *data);
int pcf8574_write( int file, int devaddr, unsigned char byte_w);


#endif