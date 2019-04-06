#include <stdio.h>
#include <stdlib.h>

#include "libVf.h"

volatile u8 *sram= (u8*) 0x0E000000;
volatile u8 *rom= (u8*) 0x08000000;
volatile u16 *rom16= (u16*) 0x08000000;
volatile u32 *rom32= (u32*) 0x08000000;

void startModeChange(void)
{
    sram[0xFFF8] = 0x99;
    sram[0xFFF9] = 0x02;
    sram[0xFFFA] = 0x05;
    sram[0xFFFB] = 0x02;
    sram[0xFFFC] = 0x03;
}

void endModeChange(void)
{
    sram[0xFFF8] = 0x99;
    sram[0xFFF9] = 0x03;
    sram[0xFFFA] = 0x62;
    sram[0xFFFB] = 0x02;
    sram[0xFFFC] = 0x56;
}

void DoVfRomInit(void)
{
    startModeChange();
    sram[0xFFFD] = 0x00;
    endModeChange();
}

void DoVfSramInit(u8 mode)
{
    startModeChange();
    sram[0xFFFE] = mode;
    endModeChange();
}

u8 DoSramWriteAndRead(u16 writeLocation,u8 writeValue, u16 readLocation)
{
    sram[writeLocation] = writeValue;
    return sram[readLocation];
}

u8 DoSramRead(u16 readLocation)
{
    return sram[readLocation];
}

void DumpSram(u8* data)
{
    for (u16 x = 0; x < 0xFFFF; ++x){
		data[x] = sram[x];
	}
}

void BlankSram()
{
    for (u16 x = 0; x < 0xFFFF; ++x){
		sram[x] = 0x00;
	}
}

// ONLY WORKS for 0001,0002,0004..powers of 2
u16 FigureOutDestinationLocationForWrite(u8 mode,u16 write)
{
    BlankSram();
    DoVfSramInit(mode);
    sram[write] = 0x20;
    u16 currentReadAddress = 0x0001;
    for(u8 y=0;y<0xf;y++){
        if ( sram[currentReadAddress] != 0 ) {
            return currentReadAddress;
        }
        currentReadAddress = currentReadAddress << 1;
    }
    return 0xffff;
}

void DumpRom(u8* data, u32 startingOffset, u32 memSize)
{
    for (u32 x = 0; x < memSize; ++x){
		data[x] = rom[x+startingOffset];
	}
}

void DumpRom32(u32* data, u32 startingOffset, u32 memSize)
{
    startingOffset /= 4;
    memSize /= 4;
    for (u32 x = 0; x < memSize; ++x){
		data[x] = rom32[x+startingOffset];
	}
}

u32 DumpRom32Yj(u32* data, u32 startingOffset, u32 memSize, const u32 skips[16], int skipBlockStart, int skipBlockEnd)
{
    startingOffset /= 4;
    memSize /= 4;
    for (u32 x = 0; x < memSize; ++x){
        int actualAddress = (x+startingOffset)*4;
        bool skipMatch = false;
        for(u8 y = 0; y < 16; y++) {
            if (actualAddress == skips[y]) {
                skipMatch = true;
                break;
            }
        }
        if (skipMatch || (actualAddress >= skipBlockStart && actualAddress < skipBlockEnd)) {
            data[x] = 0x69696969;
            continue;
        }
        data[x] = rom32[x+startingOffset];
        if (rom32[0] == 0) { // detect if protection was tripped
            return actualAddress;
        }
    }
    return 0;
}

u32 DumpRom32YjForMario(u32 *data, u32 startingOffset, u32 memSize)
{
    // FOR MARIO DX
    u32 skips[16] = {
        0x00000010, 0x00000540, 0x0000067c, 0x000006c0, 0x00000878, 0x000009a0, 0x00000b70, 0x00000d20,
        0x000015c4, 0x002002e0, 0x002003b4, 0x00200614, 0x002008cc, 0x00200a80, 0xffffffff, 0xffffffff
        // Seems like the 0x200??? ones are the same as the other game but just minus 4?
        // The last 3 were just assumed from it
    };
    return DumpRom32Yj(data, startingOffset, memSize, skips, 0x21a100, 0x21c000);
}

u32 DumpRom32YjForRudy(u32* data, u32 startingOffset, u32 memSize)
{
    // for Digimon Rudy & Sapphire
    u32 skips[16] = {
        0x00000540, 0x00000858, 0x00000f2c, 0x00001574, 0x000018c4, 0x00002064, 0x000024bc, 0x002002e4,
        0x002003b8, 0x00200618, 0x002008d0, 0x00200a84, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
    };
    return DumpRom32Yj(data, startingOffset, memSize, skips, 0x21a100, 0x21c000);
}

void DumpRom16(u16* data, u32 startingOffset, u32 memSize)
{
    startingOffset /= 2;
    memSize /= 2;
    for (u32 x = 0; x < memSize; ++x){
		data[x] = rom16[x+startingOffset];
	}
}
