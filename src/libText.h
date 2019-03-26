/*
	libText
	Simple text drawing routines
	Original by chishm, minimal modifications by taizou
	Data types are from wintermute's gba_types.h libgba library.
	Code used from r6502's BGScroller example
    Font: PCPaint Bold via BBD
*/


//---------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//---------------------------------------------------------------------------------

void text_init(void);
void text_char(char text);
void text_row(char text[256]);
void text_print(char *str, ...);
void text_newline(void);

//---------------------------------------------------------------------------------
#ifdef __cplusplus
}	   // extern "C"
#endif
//---------------------------------------------------------------------------------
