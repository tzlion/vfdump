#include <stdio.h>
#include <stdlib.h>

#include "libVf.h"

volatile u8 *sram= (u8*) 0x0E000000;
volatile u8 *rom= (u8*) 0x08000000;

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
