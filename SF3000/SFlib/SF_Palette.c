//	File : SF_Palette
//	Palette changing utilities

#include "SF_Palette.h"

/**************************************/

void palette_set (long p_red, long p_green, long p_blue)

// Purpose : Sets the screens colour palette
// Accepts : Colour component scaling values (0 (off) to 255 (max brightness))
// Returns : nothing

{

long	bank,
		clut_reg;
		
		for (bank = 0; bank < screen->sc_nScreens; bank++)
			for (clut_reg =0; clut_reg < 32; clut_reg ++)
				SetScreenColor(screen->sc_Screens[bank],	MakeCLUTColorEntry(clut_reg,
															((clut_reg<<3) * p_red)>>8,
															((clut_reg<<3) * p_green)>>8,
															((clut_reg<<3) * p_blue)>>8));
}
