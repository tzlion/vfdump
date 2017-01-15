//---------------------------------------------------------------------------------
// Based on GBA sample code for devkitARM - http://www.devkit.tk
// Transfers GBA backup memory to and from PC
//---------------------------------------------------------------------------------
#include "gba_video.h"
#include "gba_interrupt.h"
#include "xcomms.h"
#include "gba_input.h"

#include <stdio.h>
#include <stdlib.h>

#include "libSave.h"
#include "libText.h"
//---------------------------------------------------------------------------------
// Constants, variables and Addresses
//---------------------------------------------------------------------------------
u8 save_data[0x20000] __attribute__ ((section (".sbss")));
u8 compare_data[0x8000] __attribute__ ((section (".sbss")));
const u8 file_name[] = {"GBA_Cart.sav"};


#define VID_WIDTH  240
#define EEPROM_512  0x00200
#define FLASH_64 0x20000
#define PRINT(m) { dprintf(m); text_row(m);}
#define PUTCHAR(m) { dputchar(m); text_char(m);};

unsigned int frame = 0;
u8 *pak_ROM= ((u8*)0x08000000);
u16 *pak_SRAM= ((u16*)0x0E000000);
u16 *pak_FLASH= ((u16*)0x09FE0000);
u8 *pak_EEPROM= ((u8*)0x0D000000);
u16 *vid_mem= ((u16*)0x06000000);

//---------------------------------------------------------------------------------
void VblankInterrupt()
//---------------------------------------------------------------------------------
{
	frame += 1;
	ScanKeys();
}



//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void)
//---------------------------------------------------------------------------------
{
	u32 x;
	u32 chunkSize, y;
	u32 memSize;
	u32 fileSize;
	u16 keys = 0;
	char keyboard = 0;

	int handle;
	bool error_free = false;
	bool Put = false;		// Change to false to Get saves, true to Put saves

	// Set up the interrupt handlers
	InitInterrupt();

	SetInterrupt( Int_Vblank, VblankInterrupt);

	// Enable Vblank Interrupt to allow VblankIntrWait
	EnableInterrupt(Int_Vblank);

	// Allow Interrupts
	REG_IME = 1;

	xcomms_init();
	text_init();

	PRINT("NAME: ");
	for (x = 0; x < 12; x++){
		PUTCHAR(pak_ROM[160 + x]);
	}
	PRINT("\n");
	
	text_print("Press L+A+B to read a save    from the cart\n");
	text_print("Press R+A+B to write a save tothe cart\n");
	dprintf("Press R to read a save from the cart\n");
	dprintf("Press W to write a save to the cart\n");
	while ((keys != (KEY_A | KEY_B | KEY_L)) && (keys != (KEY_A | KEY_B | KEY_R)) && (keyboard !='R') && (keyboard !='r') && (keyboard !='W') && (keyboard !='w'))
	{
		VBlankIntrWait();
		keys = KeysDown();
		keyboard = dgetch();
	}

	if ((keys == (KEY_A | KEY_B | KEY_L)) || (keyboard == 'R') || (keyboard == 'r'))
	{
		PRINT("Read save selected\n");
		Put = false;
	} else {
		PRINT("Write save selected\n");
		Put = true;
	}

	// Disable interrupts
	DisableInterrupt(Int_Vblank);
	REG_IME = 0;

	PRINT("Detecting Save Size... ");
	memSize = SaveSize();
	PRINT("Done.\n");
	chunkSize = ( (memSize<=0x8000) ? memSize : 0x8000 );

	if (Put == true){
		// Error checking code
		error_free = false;
		while (error_free == false)
		{
			dprintf("Reading save from file...\n");
			handle = dfopen(file_name,"rb");
			dfseek(handle,0,SEEK_END);
			fileSize = dftell(handle);
			dfseek(handle,0,SEEK_SET);
			dfread(save_data,1,memSize,handle);
			dfclose(handle);
			if (memSize != fileSize)
			{
				// Skip error checking, as the file won't be used
				error_free = true;
			}

			PRINT("Checking transfer integrity.\n");
			handle = dfopen(file_name,"rb");
			dfseek(handle,0,SEEK_SET);
			for (y = 0; y < memSize ; y += 0x8000)
			{
				dfread(compare_data,1,chunkSize,handle);
				for(x=0; ((compare_data[x] == save_data[x + y]) && (x < chunkSize)); x++)
				{
				}
				if (x != chunkSize)
				{
					dprintf("Error in byte 0x%06X. Retrying...\n",x + y);
					text_print("Error in byte 0x%06X. \nRetrying...\n",x + y);
					y = memSize + 1;
				} else if (y == (memSize - chunkSize)) {
					error_free = true;
					PRINT("Save transfered okay.\n");
				}
			}
			dfclose(handle);
		}
		// End of error checking

		if (memSize != fileSize){
			PRINT("Save file is wrong size. Aborting...\n");
		} else {
			PRINT("Writing save to GBA cart...\n");
			PRINT("Do not turn off GBA\n");

			switch (memSize){
			case 0x200:
				PRINT("4Kbit EEPROM Save (512B)\n");
				PutSave_EEPROM_512B(save_data);
				break;
			case 0x2000:
				PRINT("64Kbit EEPROM Save (8KB)\n");
				PutSave_EEPROM_8KB(save_data);
				break;
			case 0x8000:
				PRINT("256Kbit SRAM Save (32KB)\n");
				PutSave_SRAM_32KB(save_data);
				break;
			case 0x10000:
				PRINT("512Kbit FLASH Save (64KB)\n");
				PutSave_FLASH_64KB(save_data);
				break;
			case 0x20000:
				PRINT("1Mbit FLASH Save (128KB)\n");
				PutSave_FLASH_128KB(save_data);
				break;
			default:
				PRINT("Unable to put save\n");
			}
		}
	}else{
		PRINT("Reading save from GBA...\n");

		switch (memSize){
		case 0x200:
			PRINT("4Kbit EEPROM Save (512B)\n");
			GetSave_EEPROM_512B(save_data);
			break;
		case 0x2000:
			PRINT("64Kbit EEPROM Save (8KB)\n");
			GetSave_EEPROM_8KB(save_data);
			break;
		case 0x8000:
			PRINT("256Kbit SRAM Save (32KB)\n");
			GetSave_SRAM_32KB(save_data);
			break;
		case 0x10000:
			PRINT("512Kbit FLASH Save (64KB)\n");
			GetSave_FLASH_64KB(save_data);
			break;
		case 0x20000:
			PRINT("1Mbit FLASH Save (128KB)\n");
			GetSave_FLASH_128KB(save_data);
			break;
		default:
			PRINT("Unable to get save\n");
		}

		// Error checking code
		error_free = false;
		while (error_free == false)
		{
			PRINT("Writing save to file...\n");
			PRINT("Do not turn off GBA\n");
			handle = dfopen(file_name,"wb");
			dfseek(handle,0,SEEK_SET);
			dfwrite(save_data,1,memSize,handle);
			dfclose(handle);


			PRINT("Checking transfer integrity.\n");
			handle = dfopen(file_name,"rb");
			dfseek(handle,0,SEEK_SET);
			for (y = 0; y < memSize ; y += 0x8000)
			{
				dfread(compare_data,1,chunkSize,handle);
				for(x=0; ((compare_data[x] == save_data[x + y]) && (x < chunkSize)); x++)
				{
				}
				if (x != chunkSize)
				{
					dprintf("Error in byte 0x%06X. Retrying...\n",x + y);
					text_print("Error in byte 0x%06X. \nRetrying...\n",x + y);
					y = memSize + 1;
				} else if (y == (memSize - chunkSize)) {
					error_free = true;
					PRINT("Save transfered okay.\n");
				}
			}
			dfclose(handle);
		}
		// End of error checking

	}

	PRINT("Done\n");
	PRINT("Okay to turn off GBA\n");

	while (1)
	{
		// Power down GBA
		SystemCall(3);  
	}

	return (0);
}


