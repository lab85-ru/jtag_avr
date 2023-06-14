#include "consol.h"

typedef enum{
	GET_CHAR,
	OUT_PROMT,
	OUT_ERROR,
	CMD_ANALYS_LOW,
	CMD_ANALYS_SET,
	WRITE_CFG
}consol_state_e;



const char promt[] = "DEVICE>";
const char error[] = "\n\rERROR\n\r";



const char cmd_help1[] 	        = "help";
const char cmd_help2[] 	        = "?";
const char cmd_view[] 	        = "view";
const char cmd_version[]	= "version";
const char cmd_root[]		= "root";	// print help command for ROOT
const char cmd_reboot[]		= "reboot";	// reboot sigma
const char cmd_xmodem[]         = "xmodem";
const char cmd_mkfs[]           = "mkfs";
const char cmd_free[]           = "free";
const char cmd_ls[]             = "ls";
const char cmd_rm[]             = "rm";
const char cmd_dump[]           = "dump";
const char cmd_jtag[]           = "jtag";

const char STR_CIFRA[] = "0123456789";

const char txt_help[]= {
"\n\r----------------------------------------\n\r"
"------------------ MENU ----------------\n\r"
"help or ?              - This help.\n\r"
"view                   - Views all variables.\n\r"
"version                - View version hard+soft.\n\r"
"xmodem <filename>      - Load binary firmware to Flash.\n\r"
"mkfs                   - Create file system.\r\n"
"free                   - Get volume information and free clusters of drive.\r\n"
"ls                     - List directory files.\r\n"
"rm <filename>          - Delete file.\r\n"
"dump <filename>        - Dump file to consol.\r\n"
"jtag <filename>        - Program file to JTAG.\r\n"
};

const char txt_root_help[]= {
"\n---root command--------------------------\n\r"
"reboot      - reboot device.\n\r"
};

