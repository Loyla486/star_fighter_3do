#include "Bit_Struct.h"

extern long cosine_table [ 2048 ] ;
extern long *sine_table ;

extern bit_list bits ;
#define ROT_LIMIT	((1024*1024)-1)

extern long which_graphics_set ;

void bit_update(void);

void add_bit( 	long , long , long ,
				long , long , long ,
				long , long ,
				long , long ,
				long , long );

