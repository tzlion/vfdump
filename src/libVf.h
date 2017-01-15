#include "gba_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void DoVFRomInit(void);
void DoVFSRamInit(u8 ffeValue);

u8 DoVFSRamWriteAndRead(u16 writeLocation,u8 writeValue, u16 readLocation);
u8 DoVFSRamRead(u16 readLocation);

void DumpRom(u8* data, u32 startingOffset, u32 memSize);
void DumpSRam(u8* data);
void BlankVFSRam(void);

u16 FigureOutDestinationLocationForWrite(u8 mode,u16 write);

#ifdef __cplusplus
}
#endif
