#include "gba_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void DoVfRomInit(void);
void DoVfSramInit(u8 mode);

u8 DoSramWriteAndRead(u16 writeLocation,u8 writeValue, u16 readLocation);
u8 DoSramRead(u16 readLocation);

void DumpRom(u8* data, u32 startingOffset, u32 memSize);
void DumpRom16(u16* data, u32 startingOffset, u32 memSize);
void DumpRom32(u32* data, u32 startingOffset, u32 memSize);
void DumpSram(u8* data);
void BlankSram(void);

u16 FigureOutDestinationLocationForWrite(u8 mode,u16 write);

#ifdef __cplusplus
}
#endif
