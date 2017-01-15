/*
	libText
	Simple text drawing routines
	Data types are from wintermute's gba_types.h libgba library.
	Code and font used from r6502's BGScroller example
*/

#include "libText.h"
#include "gba_video.h"

#include <stdarg.h>
#include <stdio.h>

#include "r6502_portfont.h"

#define MAPADDRESS		0x0600F800	// our base map address


int row = 0;
int col = 0;


const u16 palette[] = {
	RGB8(0x00,0x00,0x00),
	RGB8(0xFF,0xFF,0xFF),
	RGB8(0xF5,0xFF,0xFF),
	RGB8(0xDF,0xFF,0xF2),
	RGB8(0xCA,0xFF,0xE2),
	RGB8(0xB7,0xFD,0xD8),
	RGB8(0x2C,0x4F,0x8B)
};


void text_char(char text)
{
	u16 *temppointer;
	temppointer = (u16 *)MAPADDRESS;

	if (text == '\n' ){
		text_newline();
	} else {
		temppointer[row * 32 + col] = text - 32;
		col++;
	}
	if ((col > 29 && text != ' ') || col > 30){
		text_newline();
	}
}

void text_newline(void) 
{
	u8 i;
	row ++;
	if (row == 32) 
	{
		row = 0;
	}
	col = 0;
	REG_BG0VOFS = (row + 13) * 8;
	for (i = 0;i<30;i++)
	{
		text_char(' ');
	}
	col=0;
}

void text_init(void)
{
	int i;
	u16 *temppointer;

	BGCTRL[0] = BG_SIZE_0 | BG_16_COLOR | CHAR_BASE(0) | SCREEN_BASE(31);
	SetMode( MODE_0 | BG0_ON );
	
	// load the palette for the background, 7 colors
	temppointer = BG_COLORS;
	for(i=0; i<7; i++)
	{
		*temppointer++ = palette[i];
	}
	// load the font into gba video mem (48 characters, 4bit tiles)
	temppointer = (u16 *)VRAM;
	for(i=0; i<(48*32); i++)
	{
		*temppointer++ = *((u16 *)r6502_portfont + i);
	}
	// clear screen map with tile 0 ('space' tile) (256x256 halfwords)
	temppointer = (u16 *)MAPADDRESS;
	for(i=0; i<(256*256)/2; i++)
	{
		*temppointer++ = 0x00;
	}
	REG_BG0VOFS = 0;
	REG_BG0HOFS = 0;
	text_newline();

}

void text_row(char text[256])
{
	u32 i;
	u16 *temppointer;

	temppointer = (u16 *)MAPADDRESS;

	for (i=0;(text[i] != '\0') && i < 256; i++)
	{
		// write a character - we subtract 32, because the font graphics
		// start at tile 0, but our text is in ascii (starting at 32 and up)
		// in other words, tile 0 is a space in our font, but in ascii a
		// space is 32 so we must account for that difference between the two.
		if (text[i] == '\n' ){
			text_newline();
		} else {
			temppointer[row * 32 + col] = text[i] - 32;
			col++;
		}
		if (col > 29){
			text_newline();
		}
	}
}


//---------------------------------------------------------------------------------
void text_print(char *str, ...)
//---------------------------------------------------------------------------------
{
	unsigned char __outstr[256];

	va_list args;
	int len;

	va_start(args, str);
	len=vsnprintf(__outstr,256,str,args);
	va_end(args);

	text_row(__outstr);
}
