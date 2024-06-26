#include "ship_struct.h"

// Weapon types
#define AIR_TO_GROUND	((6<<4)+0)
#define AIR_TO_AIR		((6<<4)+1)
#define MINE			((6<<4)+2)
#define MEGA_BOMB		((6<<4)+3)
#define WING_POD		((6<<4)+4)
#define SAM_TO_AIR		((6<<4)+5)

// Wing pod ref
#define POD_1			(1<<0)
#define POD_2			(1<<1)
#define POD_3			(1<<2)
#define POD_4			(1<<3)

extern long which_graphics_set ;

extern long cosine_table [ 2048 ] ;
extern long *sine_table ;

extern long pod_counter ;
extern char height_map [ 256 ] [ 256 ] ;
extern char poly_map [ 128 ] [ 128 ] ;

void weapon_control( ship_stack * );

void mega_bomb_control( ship_stack * );
void air_to_ground_control( ship_stack * );
void air_to_air_control( ship_stack * );
void mine_control( ship_stack * );
void wing_pod_control( ship_stack * );

void add_mega_bomb ( ship_stack * );
void add_air_to_ground ( ship_stack * );
void add_air_to_air ( ship_stack * , ship_stack * );
void add_mine ( ship_stack * );
void add_wing_pod ( ship_stack * );


void add_sam( long , long , long , ship_stack* , long );






























