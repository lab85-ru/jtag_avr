#ifndef __ENV_CONFIG_H_
#define __ENV_CONFIG_H_


// === i2c ====================================================================================
// peremenii lihat posledovatelno !!!

// ohrana off-on
#define MODE_OFF        0xff
#define MODE_ON         0x00

#define KEY_STATE_CLEAR		0xff				// znachenie pri kotorom ne bilo ALARMA


#define TELNUM_QUANTITY		8				// kolichestvo telehonuh nomerov v spiske
#define TELNUM_LEN		16				// char-txt maximum 12345 + \0

#define TELNUMFAT_CLEAR         0xff                            // polnostiu pusto
#define TELNUMFAT_FULL          0x0                             // vse zaneto


//#pragma pack (1)
typedef struct {
	char mode;                                              // mode - ohrana -on -off
        char telnum_balance[TELNUM_LEN];                        // tel num for balance
	char telnum_fat;                                        // bitovoe pole esli bit = 0 snachit telephone propisam s spiske
	char telnum[ TELNUM_QUANTITY ][ TELNUM_LEN ];           // telefonii monera
        char key_state;                                         // sostoyanie konopki (prev)(current)
} env_config_bin;

int env_config_load(env_config_bin *config);
int env_config_load_noos(env_config_bin *config);
int env_config_save(env_config_bin *config);
void print_env(env_config_bin *config);
void env_key_to_flash(env_config_bin *config, char d );
char env_key_from_flash( env_config_bin *config );


#endif /*ENV_CONFIG_H_*/
