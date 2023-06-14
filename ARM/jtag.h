//#include "common.h"

//unsigned char __flash  * store;
//unsigned long x=0;
//unsigned char sign[3];
//unsigned char i,k,page;
//unsigned int j;


#define FILE_HEADER_LEN         (16)                                    // длинна заголовка файла прошивки (слежебная информация)
#define SIGNATURE_OFFSET        (0)                                     // смешение от начала файла - начало сигнатуры
#define FUSE_OFFSET             (3)                                     // смешение от начала файла - начало fuse



// Walk trough TAP
void TAP(unsigned int state, unsigned char count);
// Shift-IR
unsigned int shift_instruction(unsigned int instruction, unsigned char count);
void InitIO();

void ChipErase(void);

void DeInitIO();

unsigned char CheckSignature(void);      // для Mega128

void Reset(void);

void unReset(void);

void EnterProgramming(void);

void ExitProgramming(void);

void EnterSignByteRead(void);

void LoadSignAddressByte(unsigned char addr);

unsigned int ReadSignByte(void);

void EnterFlashWrite(void);

void LoadAddressHByte(unsigned char Haddr);

void LoadAddressLByte(unsigned char Laddr);

void LoadDataLByte(unsigned char LData);

void LoadDataHByte(unsigned char HData);

void LatchData(void);

void WriteFlashPage(void);

void WriteFlashPageWait(void);

void LoadFuseDataLowByte(unsigned char Data);

void EnterFuseWrite(void);

void WriteFuseExtendedByte(void);

void WriteFuseHighByte(void);

void WriteFuseLowByte(void);

unsigned int ReadFuseAndLock(void);
unsigned char ReadFuseLowByte(void);
unsigned char ReadFuseHighByte(void);
unsigned char ReadFuseExtByte(void);

void EnterLockBitWrite(void);

void LoadLockByte(unsigned char Data);

void WriteLockBits(void);

void EnterFlashRead(void);

void ReadData(void);

unsigned int ReadDataLByte(void);

unsigned int ReadDataHByte(void);

void fuckoff(void);

void prog_jtag(FIL *f_file);



//Code:
// #!/usr/bin/env perl 
// while(<>) { 
//     chop; 
//     if (/device name/) { 
//         s/.*device name="//; 
//         s/".*//; 
//         print "$_ \t"; 
//     } 
//     if (/SIGNATURE[0-9]/) { 
//         s/.*0x(..).*/$1/; 
//         print; 
//     } 
//     print "\n" if eof; 
// } 
// When I run it with ATmega*.xml as argument in the devices directory in Atmel Studion 6 I get: 
/*
Code:
 ATmega1280      1e9703 
 ATmega1281      1e9704 
 ATmega1284P     1e9705 
 ATmega1284      1e9706 
 ATmega128A      1e9702 
 ATmega128RFA1   1ea701 
 ATmega128       1e9702 
 ATmega162       1e9404 
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
 ATmega640       1e9608 
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