//#include "stdio.h"
#include <string.h>
//#include <unistd.h>

#include "main.h"
#include "env_config.h"
#include "i2c.h"


#ifndef DEBUG
#define DEBUG 0
#endif

void print_env(env_config_bin *config)
{
/*	unsigned short size_config;

	size_config = sizeof(env_config_bin)/sizeof(unsigned char);

	printf_d("Print ENV. size=%d\n\r",size_config);

	printf_d("mode            = 0x%x\n\r",config->mode);
	printf_d("telnum_balance  = %s\n\r",config->telnum_balance);
	printf_d("telnum_fat      = 0x%x\n\r",config->telnum_fat);
        
        for (int i=0; i<TELNUM_QUANTITY; i++){
                if ((config->telnum_fat && (1<<i)) == 0)
        	        printf_d("tel numer  = %s\n\r",config->telnum[ i ][ 0 ]);
        }
*/
}

void var_clr(char *buf, unsigned char len)
{
	if ( *buf == 0xff) memset( buf, 0, len );
}



int env_config_load_noos(env_config_bin *config)
{
	unsigned char b;
	unsigned short adr = 0; // start addres for load in eeprom

	for (adr=0; adr!=sizeof(env_config_bin)/sizeof(unsigned char); adr++){
		b = I2C_EE_ByteRead(adr);
		*((unsigned char*)config + adr) = b;
	}

	//var_clr( &config->mode, sizeof(config->mode)/sizeof(char) );
	var_clr( config->telnum_balance, sizeof(config->telnum_balance)/sizeof(char) );
	//var_clr( &config->telnum_fat, sizeof(config->telnum_fat)/sizeof(char) );
        
	var_clr( &config->telnum[0][0], sizeof(config->telnum)/sizeof(char) );

	if (DEBUG) print_env(config);

	return 0;
}

int env_config_save(env_config_bin *config)
{
	unsigned char *buf;
	unsigned short adr;
	unsigned char c;
	
	buf = (unsigned char*)config;

	for (adr=0; adr!=sizeof(env_config_bin)/sizeof(unsigned char); adr++){
		c = *(buf+adr);
//		while(xSemaphoreTake(xI2COSsem, portMAX_DELAY) == pdFALSE);
		I2C_EE_ByteWrite(adr, c);
//		xSemaphoreGive(xI2COSsem);
		os_delay(10);
	}
	
	return 0;
}

//------------------------------------------------------------------------------
// write key_state ->(flash) & (config)
//------------------------------------------------------------------------------
void env_key_to_flash(env_config_bin *config, char d )
{
        uint32_t adr;
        adr = ((uint32_t)&config->key_state - (uint32_t)config) & 0xff;         // vichislaem adress key_state vo flash memory
        config->key_state = d;
        I2C_EE_ByteWrite( (uint16_t)adr, d );
}

//------------------------------------------------------------------------------
// readkey state from flash
//------------------------------------------------------------------------------
char env_key_from_flash( env_config_bin *config )
{
        char c;
        uint32_t adr;
        adr = ((uint32_t)&config->key_state - (uint32_t)config) & 0xff;         // vichislaem adress key_state vo flash memory
        c = I2C_EE_ByteRead( (uint16_t)adr );
        config->key_state = c;
        return c;        
}