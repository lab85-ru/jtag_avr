#include "hardware.h"
#include "main.h"
#include "ff.h"
#include "jtag.h"
#include "task_consol.h"
#include "task_menu.h"
#include "lcd.h"

extern char fbuff[ ];
extern int fbuff_n;
extern volatile FRESULT f_result;
extern char string_for_sprint[];

unsigned char *store;
unsigned long x=0;
unsigned char sign[3];
unsigned char i,k,page;
unsigned int j;

unsigned int signature_from_file;               // сигнатура прочитаная из файла
unsigned int flash_addr_page;                   // адресс страници старший байт
unsigned int fuse_from_file;                    // fuse прочитание из файла

#define TXT_DEVICE_NAME_SIZE    (20)
typedef struct {
        const char txt_device_name[ TXT_DEVICE_NAME_SIZE ];     // txt
        const unsigned int signature;                           // 3 byte
        const unsigned int sector_size;                         // size WORD
}avr_signature_struct;

const avr_signature_struct avr_signature_st[] = {
  {"ATmega162", 0x1e9404, 64},
  {"ATmega128", 0x1e9702, 128},
  {"ATmega640", 0x1e9608, 64} 

};

/*
ATmega1280      1e9703 
 ATmega1281      1e9704 
 ATmega1284P     1e9705 
 ATmega1284      1e9706 
 ATmega128A      1e9702 
 ATmega128RFA1   1ea701 
 ATmega164A      1e940f 
 ATmega164PA     1e940a 
 ATmega164P      1e940a 
 ATmega165A      1e9410 
 ATmega165PA     1e9407 
 ATmega165P      1e9407 
 ATmega168A      1e9406 
 ATmega168PA     1e940b 
 ATmega168P      1e940b 
 ATmega168       1e9406 
 ATmega169A      1e9411 
 ATmega169PA     1e9405 
 ATmega169P      1e9405 
 ATmega16A       1e9403 
 ATmega16HVB     1e940d 
 ATmega16M1      1e9484 
 ATmega16U2      1e9489 
 ATmega16U4      1e9488 
 ATmega16        1e9403 
 ATmega2560      1e9801 
 ATmega2561      1e9802 
 ATmega324A      1e9515 
 ATmega324PA     1e9511 
 ATmega324P      1e9508 
 ATmega3250A     1e9506 
 ATmega3250PA    1e950e 
 ATmega3250P     1e950e 
 ATmega3250      1e9506 
 ATmega325A      1e9505 
 ATmega325PA     1e950d 
 ATmega325P      1e950d 
 ATmega325       1e9505 
 ATmega328P      1e950f 
 ATmega328       1e9514 
 ATmega3290A     1e9504 
 ATmega3290PA    1e950c 
 ATmega3290P     1e950c 
 ATmega3290      1e9504 
 ATmega329A      1e9503 
 ATmega329PA     1e950b 
 ATmega329P      1e950b 
 ATmega329       1e9503 
 ATmega32A       1e9502 
 ATmega32C1      1e9586 
 ATmega32HVB     1e9510 
 ATmega32M1      1e9584 
 ATmega32U2      1e958a 
 ATmega32U4      1e9587 
 ATmega32        1e9502 
 ATmega48A       1e9205 
 ATmega48PA      1e920a 
 ATmega48P       1e920a 
 ATmega48        1e9205 
 ATmega644A      1e9609 
 ATmega644PA     1e960a 
 ATmega644P      1e960a 
 ATmega644       1e9609 
 ATmega6450A     1e9606 
 ATmega6450P     1e960e 
 ATmega6450      1e9606 
 ATmega645A      1e9605 
 ATmega645P      1e960D 
 ATmega645       1e9605 
 ATmega6490A     1e9604 
 ATmega6490P     1e960C 
 ATmega6490      1e9604 
 ATmega649A      1e9603 
 ATmega649P      1e960b 
 ATmega649       1e9603 
 ATmega64A       1e9602 
 ATmega64C1      1e9686 
 ATmega64M1      1e9684 
 ATmega64        1e9602 
 ATmega8515      1e9306 
 ATmega8535      1e9308 
 ATmega88A       1e930a 
 ATmega88PA      1e930f 
 ATmega88P       1e930f 
 ATmega88        1e930a 
 ATmega8A        1e9307 
 ATmega8U2       1e9389 
 ATmega8         1e9307
*/


// Walk trough TAP
void TAP(unsigned int state, unsigned char count)
{
        unsigned char i;
        JTAG_TCK_0;
        JTAG_TMS_0;
        for (i=0;i<count;i++){
                if ((state&0x01)!=0x00)
                        JTAG_TMS_1;
                else
                        JTAG_TMS_0;
                state>>=1;
                delay_5us();
                JTAG_TCK_1;
                delay_5us();
                JTAG_TCK_0;
                delay_5us();
        }
}

// Shift-IR
unsigned int shift_instruction(unsigned int instruction, unsigned char count)
{
        unsigned int in;
        unsigned char i;
        unsigned int x=0;
        JTAG_TMS_0;
        JTAG_TCK_0;
        for (i=0;i<count;i++){
                if ((instruction&0x01)!=0x00)
                        JTAG_TDI_1;
                else
                        JTAG_TDI_0;
                instruction>>=1;
                if (i==(count-1))
                        JTAG_TMS_1;
                delay_5us();
                JTAG_TCK_1;
                delay_5us();
                
                in = JTAG_TDO;
                if (in != 0x00) x |= 0x80000000;
                
                x = (x >> 1);
                JTAG_TCK_0;
                delay_5us();
        }
        return x;
}

void InitIO()
{
        JTAG_TCK_1;
        JTAG_TMS_1;
        JTAG_TDI_1;
}

void ChipErase(void)
{
        unsigned int res;
          
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x2380,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3180,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3380,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3380,15);

        do{
                TAP(0x0D,6);
                shift_instruction(0x05,4);
                TAP(0x3,4);
                res = shift_instruction(0x3380,15);
        } while ( res & 0x0200 );
}

void DeInitIO()
{
}

unsigned char CheckSignature(void)
{
        return 1;
}


void Reset(void)
{
        TAP(0xFFFF,16);
        TAP(0x0000,16);
        TAP(0x3,4);
        shift_instruction(0xC,4);
        TAP(0x3,4);
        shift_instruction(0x01,1);
}

void unReset(void)
{
        TAP(0x0D,6);
        shift_instruction(0xC,4);
        TAP(0x3,4);
        shift_instruction(0x00,1);
}

void EnterProgramming(void)
{
        TAP(0x0D,6);
        shift_instruction(0x4,4);
        TAP(0x3,4);
        shift_instruction(0xA370,16);
}

void ExitProgramming(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x2300,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3300,15);

        TAP(0x0D,6);
        shift_instruction(0x04,4);
        TAP(0x3,4);
        shift_instruction(0x0000,15);
}

void EnterSignByteRead(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x2308,15);
}

void LoadSignAddressByte(unsigned char addr)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction((0x0300|addr),15);
}

unsigned int ReadSignByte(void)
{
        unsigned int aa;
        
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3200,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        aa = shift_instruction(0x3300,15);
        return aa;
}

void EnterFlashWrite(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x2310,15);
}

void LoadAddressHByte(unsigned char Haddr)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction((0x0700|Haddr),15);
}

void LoadAddressLByte(unsigned char Laddr)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction((0x0300|Laddr),15);
}

void LoadDataLByte(unsigned char LData)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction((0x1300|LData),15);
}

void LoadDataHByte(unsigned char HData)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction((0x1700|HData),15);
}

void LatchData(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3700,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x7700,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3700,15);
}

void WriteFlashPage(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3700,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3500,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3700,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3700,15);
}

void WriteFlashPageWait(void)
{
        unsigned int res;
        do{
                TAP(0x0D,6);
                shift_instruction(0x05,4);
                TAP(0x3,4);
                res = shift_instruction(0x3700,15);
        }while(res & 0x200 );
}

void LoadFuseDataLowByte(unsigned char Data)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction((0x1300|Data),15);
}

void EnterFuseWrite(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x2340,15);
}

void WriteFuseExtendedByte(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3B00,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3900,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3B00,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3B00,15);
}

void WriteFuseHighByte(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3700,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3500,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3700,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3700,15);
}

void WriteFuseLowByte(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3300,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3100,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3300,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3300,15);
}

unsigned int ReadFuseAndLock(void)
{
        unsigned int d=0;
        
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x2304,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3a00,15);
        
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        d |= ((shift_instruction(0x3e00,15)>>16)&0xff)<<16;   //ext
        
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        d |= ((shift_instruction(0x3200,15)>>16)&0xff)<<8;    //high
        
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        d |= ((shift_instruction(0x3600,15)>>16)&0xff);       //low
        
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        d |= ((shift_instruction(0x3700,15)>>16)&0xff)<<24;   //lock
        
        return d;
}

unsigned char ReadFuseHighByte(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x2304,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3e00,15);
        
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        return shift_instruction(0x3f00,15);
}

unsigned char ReadFuseExtByte(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x2304,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3a00,15);
        
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        return shift_instruction(0x3f00,15);
}

void EnterLockBitWrite(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x2320,15);
}

void LoadLockByte(unsigned char Data)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction((0x13C0|Data),15);
}

void WriteLockBits(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3300,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3100,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3300,15);

        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3300,15);
}

void EnterFlashRead(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x2302,15);
}


void ReadData(void)
{ 
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        shift_instruction(0x3200,15);
}

unsigned int ReadDataLByte(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        return (shift_instruction(0x3600,15));
}

unsigned int ReadDataHByte(void)
{
        TAP(0x0D,6);
        shift_instruction(0x05,4);
        TAP(0x3,4);
        return (shift_instruction(0x3700,15));
}

void fuckoff(void)
{
        ExitProgramming();
        unReset();
        DeInitIO();
        printf_d("\r\nERROR: jtag programming...\r\n");
}

void prog_jtag(FIL *f_file)
{
        int i,b;
        int page_size;                                                          // размер страници в словах 16 бит
        int device_num_of_list;
        int count;
        int page_in_block;                                                      // Количество страниц(отдельно взятого контролера) в блоке(размер считываемх данных из файла)
        unsigned int fuse_read;                                                 // прочитаные значения fUSE из CPU (для проверки)
        unsigned int file_size;                                                 // размер файла в байтах
        unsigned int file_size_in_block;                                        // размер файла в блоках(один блок = длинна блока чтения файла)
        unsigned int calcul_procent;                                            // подстчет процентов для текущей операции

        
        printf_d("JTAG Programm Start.-----------------------------------------\r\n");
        lcd_set_cursor_position(0,1,4);
        lcd_write_string(0, "                    ");

        file_size = f_size(f_file);
        printf_d("File Firmware size = %d bytes.\r\n", file_size);
        
        file_size_in_block = (file_size - FILE_HEADER_LEN) / FBUFF_SIZE;        // размер файла в блоках
        
        if (((file_size - FILE_HEADER_LEN) % FBUFF_SIZE) > 0)                   // если есть не полный блок то округляем в большую сторону т.е. +1
                file_size_in_block++;

        i = JTAG_VCC;
        
        if (i == 0){
                printf_d("ERROR JTAG: no power input...\r\n");
                lcd_clear_display(0);
                lcd_set_cursor_position(0,1,1);
                lcd_write_string(0, "Error: JTAG power in");
                wait_key_press();
                return;
        }
        
        printf_d("JTAG: Power input OK.\r\n");
        
//*************************** Сброс
        InitIO();
        Reset();
        EnterProgramming();
//*************************** Сигнатура
        EnterSignByteRead();
        for (i=0;i<3;i++){
                LoadSignAddressByte(i);
                x=ReadSignByte();
                sign[i]=(x>>16);
        }
        printf_d("Signature Byte: %X %X %X\r\n",sign[0],sign[1],sign[2]);

        xsprintf(string_for_sprint, "%002x%002x%002x", sign[0],sign[1],sign[2]);

        if (sign[0]==0 && sign[1]==0 && sign[2]==0){
                printf_d("ERROR: Signature.\r\n");
                lcd_clear_display(0);
                lcd_set_cursor_position(0,1,1);
                lcd_write_string(0, "Error: Signature CPU");
                lcd_set_cursor_position(0,1,2);
                lcd_write_string(0, "read = 0x");
                lcd_write_string(0,string_for_sprint);
                wait_key_press();
                return;
        }
        if (sign[0]==0xff && sign[1]==0xff && sign[2]==0xff){
                printf_d("ERROR: Signature.\r\n");
                lcd_clear_display(0);
                lcd_set_cursor_position(0,1,1);
                lcd_write_string(0, "Error: Signature CPU");
                lcd_set_cursor_position(0,1,2);
                lcd_write_string(0, "read = 0x");
                lcd_write_string(0,string_for_sprint);
                wait_key_press();
                return;
        }
        
        // поиск устройства по сигнатуре в списке устройств
        device_num_of_list = 0;
        page_size = 0;
        for (i=0;i<sizeof(avr_signature_st);i++){
                if (avr_signature_st[i].signature == (sign[0]<<16 | sign[1]<<8 | sign[2])){
                        device_num_of_list = i;
                        page_size = avr_signature_st[i].sector_size;
                        break;
                }
        }

        if (page_size == 0){
                printf_d("ERROR: Device not found from list.\r\n");
                lcd_clear_display(0);
                lcd_set_cursor_position(0,1,1);
                lcd_write_string(0, "Err:CPUnoFountToList");
                lcd_set_cursor_position(0,1,2);
                lcd_write_string(0,string_for_sprint);
                wait_key_press();
                return;
        }

        printf_d("Device name = %s\r\n",avr_signature_st[ device_num_of_list ].txt_device_name);
        lcd_set_cursor_position(0,1,1);
        lcd_write_string(0, "                    ");
        lcd_set_cursor_position(0,1,1);
        lcd_write_string(0, avr_signature_st[ device_num_of_list ].txt_device_name);

        // читаем заголовок файла
        if ((f_result = f_read(f_file, fbuff, FILE_HEADER_LEN, (UINT*)&fbuff_n )) != FR_OK && (fbuff_n != FILE_HEADER_LEN) ){
                printf_d("ERROR: read header firmware file.\r\n");
                lcd_clear_display(0);
                lcd_set_cursor_position(0,1,1);
                lcd_write_string(0, "Err:HeaderToFirmware");
                wait_key_press();
                return;
        }
        
        signature_from_file = fbuff[ SIGNATURE_OFFSET+0 ]<<16 | fbuff[ SIGNATURE_OFFSET+1 ]<<8 | fbuff[ SIGNATURE_OFFSET+2 ];
        
        if (signature_from_file != avr_signature_st[ device_num_of_list ].signature){
                printf_d("ERROR: signature CPU != signature FROM FILE.\r\n");
                lcd_clear_display(0);
                lcd_set_cursor_position(0,1,1);
                lcd_write_string(0, "Error: Signature CPU");
                lcd_set_cursor_position(0,1,2);
                lcd_write_string(0, "ReadCPU != FromFile ");
                wait_key_press();
                return;
        }

        fuse_from_file = fbuff[ FUSE_OFFSET+0 ]<<8 | fbuff[ FUSE_OFFSET+1 ] | fbuff[ FUSE_OFFSET+2 ]<<16;
        printf_d("Fuse set= %x\r\n",fuse_from_file);
        
        // запись во флеш ------------------------------------------------------
        lcd_set_cursor_position(0,1,3);
        lcd_write_string(0, "Erase flash.........");

        ChipErase();
        delay_ms(1000);
        EnterFlashWrite();
        
        flash_addr_page = 0;
        page_in_block = FBUFF_SIZE / (page_size * 2);

        lcd_set_cursor_position(0,1,3);
        lcd_write_string(0, "                    ");
        lcd_set_cursor_position(0,1,3);
        lcd_write_string(0, "Write flash:");
        
        while((f_result = f_read(f_file, fbuff, FBUFF_SIZE, (UINT*)&fbuff_n )) == FR_OK && (fbuff_n>0) ){
                LoadAddressHByte(flash_addr_page);
                for (b=0; b<page_in_block; b++){
                        // проверяем если секторо пустой т.е. = FF то пропускаем
                        count = 0;
                        for (j=b*page_size*2; j<(b+1)*page_size*2; j++){
                                if (fbuff[j] == 0xff)
                                        count++;
                        }
                
                        if (count == page_size*2){
                                continue;
                        }
                        for (j=b*page_size; j<(b+1)*page_size; j++){
                                LoadAddressLByte(j);
                                k=fbuff[j*2];
                                LoadDataLByte(k);
                                k=fbuff[j*2+1];
                                LoadDataHByte(k);
                                LatchData();
                        }
                        WriteFlashPage();
                        delay_ms(25);
                        WriteFlashPageWait();
                }
                printf_d("W");

                flash_addr_page++;
                
                calcul_procent = (flash_addr_page*100) / file_size_in_block;

                lcd_set_cursor_position(0,13,3);
                xsprintf(string_for_sprint, "%d %%", calcul_procent);
                lcd_write_string(0, string_for_sprint);
        }
        

        // переносим позицию в файле в начало минуя заголовок
        f_result = f_lseek(f_file, FILE_HEADER_LEN);

        if (f_result != FR_OK){
                printf_d("ERROR: lseek. NO VERIFY FIRMWARE.\r\n");
                lcd_clear_display(0);
                lcd_set_cursor_position(0,1,2);
                lcd_write_string(0, "Err: lseek No verify");
                wait_key_press();
                return;
        }

//       Проверка Flash --------------------------------------------------------
        EnterFlashRead();
        flash_addr_page = 0;
        
        lcd_set_cursor_position(0,1,3);
        lcd_write_string(0, "                    ");
        lcd_set_cursor_position(0,1,3);
        lcd_write_string(0, "Verify flash:");

        while((f_result = f_read(f_file, fbuff, FBUFF_SIZE, (UINT*)&fbuff_n )) == FR_OK && (fbuff_n>0) ){
                LoadAddressHByte(flash_addr_page);
                for (b=0; b<page_in_block; b++){
                        // проверяем если секторо пустой т.е. = FF то пропускаем
                        count = 0;
                        for (j=b*page_size*2; j<(b+1)*page_size*2; j++){
                                if (fbuff[j] == 0xff)
                                        count++;
                        }
                
                        if (count == page_size*2){
                                //printf_d("skip page.\r\n");
                                continue;
                        }
                        //printf_d("verify page.\r\n");
                        for (j=b*page_size; j<(b+1)*page_size; j++){
                                LoadAddressLByte(j);
                                ReadData();
                                k=(ReadDataLByte()>>16);
                                if (k != fbuff[j*2]){
                                        printf_d("ERROR: Verify adr %d %X != %X\r\n",j,k,fbuff[j*2]);
                                        
                                        lcd_set_cursor_position(0,1,3);
                                        lcd_write_string(0, "Error: Verify flash.");
                                        wait_key_press();
                                        return;
                                }
                                k=(ReadDataHByte()>>16);
                                if (k != fbuff[j*2+1]){
                                        printf_d("ERROR: Verify adr %d %X != %X\r\n",j,k,fbuff[j*2+1]);
                                        
                                        lcd_set_cursor_position(0,1,3);
                                        lcd_write_string(0, "Error: Verify flash.");
                                        wait_key_press();
                                        return;
                                }
                        }
                }
                //printf_d("read page num = %d.\r\n", flash_addr_page);
                printf_d("V");
                flash_addr_page++;

                calcul_procent = (flash_addr_page*100) / file_size_in_block;
                
                lcd_set_cursor_position(0,14,3);
                xsprintf(string_for_sprint, "%d %%", calcul_procent);
                lcd_write_string(0, string_for_sprint);

        }
       
// Fuses write + verify --------------------------------------------------------

        printf_d("Write FUSE.\r\n");

        lcd_set_cursor_position(0,1,2);
        lcd_write_string(0, "Write FUSE:");
        
        EnterFuseWrite();
        
        LoadFuseDataLowByte((fuse_from_file>>16) & 0xff);
        WriteFuseExtendedByte();
        delay_ms(20);
        
        LoadFuseDataLowByte((fuse_from_file>>8) & 0xff);
        WriteFuseHighByte();
        delay_ms(20);
        
        LoadFuseDataLowByte(fuse_from_file & 0xff);
        WriteFuseLowByte();
        delay_ms(20);
        
        fuse_read = ReadFuseAndLock();
        printf_d("Read fuse all = %X\r\n", fuse_read);  
        
        if (fuse_read&0x00ffffff != fuse_from_file){
                printf_d("Error: fuse write. write(%X) != read(%X)\r\n", fuse_from_file, fuse_read); 
                lcd_write_string(0, "Error.");        
        }else{
                printf_d("FUSE write OK.\r\n");
                lcd_write_string(0, "OK.");        
        }
        
// exit --------------------------------
        ExitProgramming();
        printf_d("Reset CPU.\r\n");
        Reset();
        unReset();
        DeInitIO();

        return;
}
