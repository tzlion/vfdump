//---------------------------------------------------------------------------------
// V F D U M P
// For dumping GBA games including Vast Fame protected carts
// Vaguely based on SendSave
//---------------------------------------------------------------------------------

#include "gba_interrupt.h"
#include "xcomms.h"
#include "gba_input.h"
#include "gba_systemcalls.h"

#include "gba_types.h"

#include <stdio.h>
#include <stdlib.h>

#include "libVf.h"
#include "libText.h"

u8 save_data[0x20000] __attribute__ ((section (".sbss")));
//u16 save_data16[0x2000] __attribute__ ((section (".sbss"))); // crashes? too much mem usage?
u32 save_data32[0x4000] __attribute__ ((section (".sbss")));
const char file_name[] = {"GBA_Cart.bin"};

#define PRINT(m) { dprintf(m); text_row(m);}
#define PUTCHAR(m) { dputchar(m); text_char(m);};

unsigned int frame = 0;
u8 *pak_ROM= ((u8*)0x08000000);
u16 *pak_SRAM= ((u16*)0x0E000000);
u16 *pak_FLASH= ((u16*)0x09FE0000);
u8 *pak_EEPROM= ((u8*)0x0D000000);
u16 *vid_mem= ((u16*)0x06000000);

struct bothKeys
{
    u16 gbaKeys;
    char keyboardKey;
};

void VblankInterrupt()
{
    frame += 1;
    scanKeys();
}

void findVfAddressReordering()
{
    for (u8 mode=0x00;mode<0x4;mode++)  { // Modes go up to F validly but 0-3 contain all the unique address swaps
        dprintf("\n= MODE %02x/%02x/%02x/%02x =\n",mode,mode+4,mode+8,mode+12);
        text_print("\n= MODE %02x/%02x/%02x/%02x =\n",mode,mode+4,mode+8,mode+12);
        u16 result;
        u16 results[0xFFFF];
        u16 currentWriteAddress = 0x0001;
        for(int z=0;z<0x0F;z++){ // It never gets the F tho is that a problem // I think nah
            BlankSram();
            result = FigureOutDestinationLocationForWrite(mode,currentWriteAddress);
            results[result] = z;
            currentWriteAddress = currentWriteAddress << 1;
        }
        PRINT("0f ");
        for(int ind = 0x4000;ind>0;ind/=2){
          dprintf("%02x ",results[ind]);
          text_print("%02x ",results[ind]);
        }
        PRINT("\n");
    }
}

void findVfValueReordering()
{
    for (u8 mode=0x00;mode<0x10;mode+=4)  { // Modes 0,4,8,C should have the regular addressing
        dprintf("\n= MODE %02x/%02x/%02x/%02x =\n",mode,mode+1,mode+2,mode+3);
        text_print("\n= MODE %02x/%02x/%02x/%02x =\n",mode,mode+1,mode+2,mode+3);
        int result;
        int results[0xFF];
        DoVfSramInit(mode);
        int writeValue=1;
        for(int z=0;z<8;z++){
            BlankSram();
            result=DoSramWriteAndRead(0x6969,writeValue,0x6969);
            results[result] = z;
            writeValue *= 2;
        }
        for(int ind = 0x80;ind>0;ind/=2){
          dprintf("%02x ",results[ind]);
          text_print("%02x ",results[ind]);
        }
        PRINT("\n");
    }
}

void readSramToFile(int handle)
{
    u32 memSize = 0x10000;
    dprintf("Getting chunk of 0x10000 from sram...\n");
    text_print("Read 0x10000 from sram\n");
    DumpSram(save_data);
    dfwrite(save_data,1,memSize,handle);
}

void readRomToFile(int handle,u32 offset,u32 chunkSize)
{
    dprintf("Getting chunk of 0x%08X from offset 0x%08X...\n",chunkSize,offset);
    text_print("Read %08X from %08X\n",chunkSize,offset);
    DumpRom(save_data,offset,chunkSize);
    dfwrite(save_data,1,chunkSize,handle);
}


void readRomToFile32(int handle,u32 offset,u32 chunkSize)
{
    dprintf("Getting chunk of 0x%08X from offset 0x%08X...\n",chunkSize,offset);
    text_print("Read %08X from %08X\n",chunkSize,offset);
    DumpRom32(save_data32,offset,chunkSize);
    dfwrite(save_data32,1,chunkSize,handle);
}

u32 readRomToFile32Yj(int handle, u32 offset, u32 chunkSize, u32* skips, u32 skipBlockStart, u32 skipBlockEnd)
{
    dprintf("Getting chunk of 0x%08X from offset 0x%08X...\n",chunkSize,offset);
    text_print("Read %08X from %08X\n",chunkSize,offset);
    u32 result = DumpRom32Yj(save_data32, offset, chunkSize, skips, skipBlockStart, skipBlockEnd);
    dfwrite(save_data32,1,chunkSize,handle);
    if ( result != 0 ) {
        text_print("Protection trip %08x\n",result);
        dprintf("Protection trip %08x\n",result);
    }
    return result;
}

struct bothKeys readKeys()
{
    struct bothKeys keyInput;
    VBlankIntrWait();
    keyInput.gbaKeys = keysDown();
    keyInput.keyboardKey = dgetch();
    return keyInput;
}

struct bothKeys waitForKey()
{
    irqEnable(IRQ_VBLANK); // Enable Vblank interrupt to allow VblankIntrWait
    REG_IME = 1; // Allow interrupts
    struct bothKeys keyInput;
    keyInput.gbaKeys = 0;
    keyInput.keyboardKey = 0;
    do {
        keyInput = readKeys();
    } while ( keyInput.gbaKeys == 0 && keyInput.keyboardKey == 0 );
    irqDisable(IRQ_VBLANK); // Disable interrupts
    REG_IME = 0;
    return keyInput;
}

void romdump(bool vfame,bool wholeCartArea)
{
    int handle;
    u32 chunkSize;

    chunkSize = 0x8000;

    u32 totalSize = wholeCartArea ? 0x8000000 : 0x2000000;
    u32 offset = 0;

    handle = dfopen(file_name,"wb");
    dfseek(handle,0,SEEK_SET);

    if (vfame) {
        DoVfRomInit();
    }
    while(offset<totalSize) {
        readRomToFile(handle,offset,chunkSize);
        offset+=chunkSize;
    }

    dfclose(handle);
}

void simulateBiosReads()
{
    // Simulate reads done by GBA BIOS.

    int handle;

    const char temp_file_name[] = {"bootreads.bin"};

    handle = dfopen(temp_file_name,"wb");
    dfseek(handle,0,SEEK_SET);

    readRomToFile(handle,0xB7,1);
    readRomToFile(handle,0xB6,1);
    readRomToFile(handle,0xB5,1);
    readRomToFile(handle,0xB4,1);

    int baseOffset = 0xBFFFFE0 - 0x800000;
    for(int x=0;x<=0x12;x+=2) {
        readRomToFile(handle,baseOffset+x+1,1);
        readRomToFile(handle,baseOffset+x,1);
    }

    readRomToFile(handle,0x9D,1);
    readRomToFile(handle,0x9E,1);
    readRomToFile(handle,0x9F,1);
    readRomToFile(handle,0xB0,1);
    readRomToFile(handle,0xB1,1);
    readRomToFile(handle,0xB2,1);
    readRomToFile(handle,0xB3,1);
    readRomToFile(handle,0xB4,1);
    readRomToFile(handle,0xB5,1);
    readRomToFile(handle,0xB6,1);
    readRomToFile(handle,0xB7,1);

    readRomToFile(handle,0x9E,1);

    // Then it tries to load the logo

    readRomToFile(handle,0x04,1);

    dfclose(handle);
}

void testDump32(u32 fromOffset)
{
    int handle;
    u32 chunkSize;

    chunkSize = 0x4000 * 4;

    u32 totalSize = 0x2000000;
    u32 offset = fromOffset;

    handle = dfopen(file_name,"wb");
    dfseek(handle,0,SEEK_SET);

    while(offset<(fromOffset+totalSize)) {
        readRomToFile32(handle,offset,chunkSize);
        offset+=chunkSize;
    }

    dfclose(handle);
}

void readSkipsFromFile(u32* skips)
{
    int handle = dfopen("skips.bin","rb");
    dfseek(handle,0,SEEK_SET);
    dfread(skips,4,18,handle);
    dfclose(handle);
    for(int x=0;x<18;x++) {
        // swap endianness for some reason
        skips[x] = ((skips[x]>>24)&0xff) | ((skips[x]<<8)&0xff0000) | ((skips[x]>>8)&0xff00) | ((skips[x]<<24)&0xff000000);
    }
}

void testDump32Yj(u32 fromOffset)
{
    int handle;
    u32 chunkSize;

    chunkSize = 0x4000 * 4;

    u32 totalSize = 0x2000000;
    u32 offset = fromOffset;

    handle = dfopen(file_name,"wb");
    dfseek(handle,0,SEEK_SET);

    u32 result;

    u32 skips[18];
    readSkipsFromFile(skips);
    // last 2 values in the file are actually the boundaries of the big skippy areas

    while(offset<totalSize) {
        result = readRomToFile32Yj(handle, offset, chunkSize, skips, skips[16], skips[17]);
        if ( result != 0 ) {
            break;
        }
        offset+=chunkSize;
    }

    dfclose(handle);
}

void experimentalDump()
{
    testDump32Yj(0);
}

void printRomName()
{
    u32 x;
    PRINT("\n");
    PRINT("NAME: ");
    for (x = 0; x < 12; x++){
        PUTCHAR(pak_ROM[160 + x]);
    }

}

int main(void)
{
    struct bothKeys keyInput;

    // Set up the interrupt handlers
    irqInit();

    irqSet( IRQ_VBLANK, VblankInterrupt);

    xcomms_init();
    text_init();

    text_print("Press A to read header\n");
    text_print("Press B to skip\n");
    dprintf("Press Y to read header\n");
    dprintf("Press N to skip\n");

    do {
        keyInput = waitForKey();
    } while ( (keyInput.gbaKeys != (KEY_A)) && (keyInput.keyboardKey !='Y') && (keyInput.keyboardKey !='y') &&
              (keyInput.gbaKeys != (KEY_B)) && (keyInput.keyboardKey !='N') && (keyInput.keyboardKey !='n'));

    if (keyInput.gbaKeys == KEY_A || keyInput.keyboardKey == 'Y' || keyInput.keyboardKey == 'y')  {
        //simulateBiosReads();
        printRomName();
    }
    PRINT("\n\n");

    text_print("Press A to dump normal ROM\n");
    text_print("Press B to dump VF ROM\n");
    text_print("DOWN to dump whole cart area\n");
    text_print("RIGHT for EXPERIMENT\n");
    text_print("UP to reset GBA\n");
    text_print("START to get value reordering\n");
    text_print("SELECT to get addr reordering\n");
    text_print("\n*SEL/START will erase save!*\n");
    dprintf("Press D to dump normal ROM\n");
    dprintf("Press V to dump VF ROM\n");
    dprintf("Press M to dump whole cart area\n");
    dprintf("Press X for EXPERIMENT\n");
    dprintf("Press R to reset GBA\n");
    dprintf("Press L to get value reordering\n");
    dprintf("Press S to get address reordering\n");
    dprintf("\n* R/S will erase your save data!\n");
    PRINT("\n");

    do {
        keyInput = waitForKey();
    } while ( (keyInput.gbaKeys != (KEY_A)) && (keyInput.keyboardKey !='D') && (keyInput.keyboardKey !='d') &&
              (keyInput.gbaKeys != (KEY_B)) && (keyInput.keyboardKey !='V') && (keyInput.keyboardKey !='v')  &&
              (keyInput.gbaKeys != (KEY_START)) && (keyInput.keyboardKey !='L') && (keyInput.keyboardKey !='l') &&
              (keyInput.gbaKeys != (KEY_SELECT)) && (keyInput.keyboardKey !='S') && (keyInput.keyboardKey !='s') &&
              (keyInput.gbaKeys != (KEY_DOWN)) && (keyInput.keyboardKey !='M') && (keyInput.keyboardKey !='m')  &&
              (keyInput.gbaKeys != (KEY_UP)) && (keyInput.keyboardKey !='R') && (keyInput.keyboardKey !='r') &&
              (keyInput.gbaKeys != (KEY_RIGHT)) && (keyInput.keyboardKey !='X') && (keyInput.keyboardKey !='x') );

    if ( keyInput.gbaKeys == KEY_B || keyInput.keyboardKey == 'V' || keyInput.keyboardKey == 'v' ) {
        PRINT("Let's VF DUMP\n");
        romdump(true,false);
    } else if (keyInput.gbaKeys == KEY_START || keyInput.keyboardKey == 'L' || keyInput.keyboardKey == 'l') {
        PRINT("Let's GET VALUE REORDERING\n");
        findVfValueReordering();
    } else if (keyInput.gbaKeys == KEY_SELECT || keyInput.keyboardKey == 'S' || keyInput.keyboardKey == 's') {
        PRINT("Let's GET ADDRESS REORDERING\n");
        findVfAddressReordering();
    } else if (keyInput.gbaKeys == KEY_DOWN || keyInput.keyboardKey == 'M' || keyInput.keyboardKey == 'm') {
        PRINT("MEGA DUMP EVERYTHING\n");
        romdump(false,true);
    } else if (keyInput.gbaKeys == KEY_A || keyInput.keyboardKey == 'D' || keyInput.keyboardKey == 'd')  {
        PRINT("Let's NORMAL DUMP\n");
        romdump(false,false);
    } else if (keyInput.gbaKeys == KEY_RIGHT || keyInput.keyboardKey == 'X' || keyInput.keyboardKey == 'x')  {
        PRINT("Let's EXPERIMENT SON\n");
        experimentalDump();
    }

    if (!(keyInput.gbaKeys == KEY_UP || keyInput.keyboardKey == 'R' || keyInput.keyboardKey == 'r')) {
        PRINT("\nDone\n");

        PRINT("\nPress any key to reset GBA\n");
        waitForKey();
    }

    SystemCall(0x26); // reset!

    return (0);
}
