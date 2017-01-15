#include <stdio.h>
#include <stdlib.h>

#include "libVf.h"

volatile u8 *sram= (u8*) 0x0E000000;
volatile u8 *rom= (u8*) 0x08000000;

void DoVFSRamWritesPart1(void)
{
    sram[0xFFF8] = 0x99;
    sram[0xFFF9] = 0x02;
    sram[0xFFFA] = 0x05;
    sram[0xFFFB] = 0x02;
    sram[0xFFFC] = 0x03;
}


void DoVFSRamWritesPart2(void)
{
    sram[0xFFF8] = 0x99;
    sram[0xFFF9] = 0x03;
    sram[0xFFFA] = 0x62;
    sram[0xFFFB] = 0x02;
    sram[0xFFFC] = 0x56;
}

void DoVFRomInit(void)
{
    DoVFSRamWritesPart1();
    sram[0xFFFD] = 0x00;
    DoVFSRamWritesPart2();
}

void DoVFSRamInit(u8 ffeValue)
{
    DoVFSRamWritesPart1();
    sram[0xFFFE] = ffeValue;
    DoVFSRamWritesPart2();
}

u8 DoVFSRamWriteAndRead(u16 writeLocation,u8 writeValue, u16 readLocation)
{
    sram[writeLocation] = writeValue;
    return sram[readLocation];
}

u8 DoVFSRamRead(u16 readLocation)
{
    return sram[readLocation];
}

void DumpSRam(u8* data)
{
	u16 x;

    for (x = 0; x < 0xFFFF; ++x)
	{
		data[x] = sram[x];
	}
}


void BlankVFSRam()
{
	u16 x;

    for (x = 0; x < 0xFFFF; ++x)
	{
		sram[x] = 0x00;
	}
}


// ONLY WORKS for 0001,0002,0004..powers of 2
u16 FigureOutDestinationLocationForWrite(u8 mode,u16 write)
{
    BlankVFSRam();
    DoVFSRamInit(mode);
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

void FillVFSRam()
{
	u16 x;

    for (x = 0; x < 0xFFFF; ++x)
	{
		sram[x] = 0x69;
	}
}

void DumpRom(u8* data, u32 startingOffset, u32 memSize)
{
	u32 x;

    for (x = 0; x < memSize; ++x)
	{
		data[x] = rom[x+startingOffset];
	}

}


