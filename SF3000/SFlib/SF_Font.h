/* File : SF_Font header file */

#ifndef __SF_FONT
#define __SF_FONT

// Includes

#include "SF_Message.h"

#include "graphics.h"
#include "Textlib.h"	
#include "Fontlib.h"
#include "Stdio.h"
#include "Utils3DO.h"
#include "String.h"
#include "UMemory.h"

// Definitions

#define	MENU_FONT		0
#define	MESSAGE_FONT	0

// Externals

extern	message			messages [FONT_MAXMESSAGE];
extern	ScreenContext*	screen;
				
// Function Prototypes

long font_loadin (void);										// Load in all fonts & return handles
void font_initialise (TextCel**);								// Initialise text cels
void font_terminate (void);										// Unloads all fonts
void font_paint (long, long, long, long, long, long, char*);	// Paint text to screen

#endif


