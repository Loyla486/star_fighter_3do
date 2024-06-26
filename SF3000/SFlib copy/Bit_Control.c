#include "Bit_Control.h"
#include "Collision.h"
#include "SF_Armburn.h"
#include "Explosion.h"
#include "Smoke_Control.h"
#include "Stdio.h"
#include "SF_ArmLink.h"
#include "Sound_Control.h"
#include "SF_ArmUtils.h"
#include "Graphics_Set.h"

void bit_update(void)

{

bit_stack *bit ;
bit_stack *bit_temp ;

long loop ;
long temp_long ;
long coll_check , coll_check_2 ;

bit=(bits.info).start_address ;

while ((bit->header).status==1)
{

//Bit types 0 - 11 0 = smallest 11 = biggest

// add on bit position velocitys
bit->x_pos += bit->x_vel ; 
bit->y_pos += bit->y_vel ;
bit->z_pos += bit->z_vel ;

// add on bit rotation velocitys
bit->x_rot = (bit->x_rot + bit->x_r_vel)&ROT_LIMIT ; 
bit->y_rot = (bit->y_rot + bit->y_r_vel)&ROT_LIMIT ;
bit->z_rot = (bit->z_rot + bit->z_r_vel)&ROT_LIMIT ;

// Slow down position and rotation velocitys
bit->x_vel -= ((bit->x_vel)>>6) ;
bit->y_vel -= ((bit->y_vel)>>6) ;
bit->z_vel -= ((bit->z_vel)>>6) ;

bit->x_r_vel -= ((bit->x_r_vel)>>6) ;
bit->y_r_vel -= ((bit->y_r_vel)>>6) ;
bit->z_r_vel -= ((bit->z_r_vel)>>6) ;

// No gravity if in space
if ( which_graphics_set != SPACE_GRAPHICS )
{
	// Gravity on bits - the bigger the bit the bigger the gravity
	bit->z_vel -= ( (1<<19) + ( ((bit->type)>>2)<<18 ) ) ;
}

// Smoke from explosion bits
if ( ((bit->type)&3)==1 )
{
	add_smoke (	bit->x_pos , bit->y_pos , bit->z_pos ,
				((bit->x_vel)>>3) , ((bit->y_vel)>>3) , ((bit->z_vel)>>3) ,
				EXPLOSION_SMOKE ,
				0 ) ;
}

	

// Check collision with ground / static objects	

coll_check = check_collision ( bit->x_pos , bit->y_pos , bit->z_pos ) ;

if ( coll_check != 0 )
{
	//make_sound( bit->x_pos , bit->y_pos , bit->z_pos , LASER_HIT_SOUND ) ;

	// Has the bit hit an object or the ground
	if ( coll_check > 0 )
	{
		if ( bit->who_owns_me >= 0 )
		{
			if ( bit->who_owns_me != coll_check )
			{
				// Its hit a static graphic
				static_explode ( coll_check , (32+((bit->type)<<2)) ) ;
			}
		}
		
		bit->counter = 0 ;
		
	}
	else
	{
		// Its hit the ground
		armburn_addexplosion ( (bit->x_pos)>>24 , (bit->y_pos)>>24 , ((bit->x_pos)>>14)&3 );
	
		dent_ground( bit->x_pos , bit->y_pos , 1 + ((bit->type)>>1) ) ;
	
		add_smoke( 	bit->x_pos , bit->y_pos , bit->z_pos ,
					0 , 0 , 0 ,
					EXPLOSION_SMOKE , 0 );
		bit->z_vel = -bit->z_vel ;
		bit->counter -= 16 ;

		// New bit rotation velocitys
		bit->x_r_vel = ((arm_random())&131071)-65536 ;
		bit->y_r_vel = ((arm_random())&131071)-65536 ;
		bit->z_r_vel = ((arm_random())&131071)-65536 ;

		// Find which way to bounce based on change of hill height
		coll_check = -coll_check ;
		coll_check_2 = find_ground_height( (bit->x_pos)+(1<<24) , bit->y_pos );
		
		bit->x_vel +=(	((coll_check-coll_check_2)>>14) *
							((bit->z_vel)>>14) ) ;
	
		coll_check_2 = find_ground_height( bit->x_pos , (bit->y_pos)+(1<<24) );
		
		bit->y_vel +=(	((coll_check-coll_check_2)>>14) *
							((bit->z_vel)>>14) ) ;
	}
}


// update the counter for the bit when <0 then delete
bit->counter -= 3 ;

// Get the adr of the next before any deleting is done
bit_temp=(bit->header).next_address ;


if ( (bit->counter) < 0 )
{
	// delete the bit
	armlink_deleteitem(bit,&bits);
}

bit=bit_temp ;
}

}



void add_bit(	long x_pos , long y_pos , long z_pos ,
				long x_vel , long y_vel , long z_vel ,
				long colour1 , long colour2 ,
				long type , long explosion_velocity ,
				long owner , long style
				)
				
{

// explosion_velocity = 0 - 32 

// This is the general purpose explosion bit adding routine which will
// add a bit of a certain type starting from the given position
// going in the direction specified by the x y z velocity and a
// addition arm_randomom velocity based on the bit type.

long velocity ;
long coll_check ;
long pointer ;
long x_dir , y_dir , z_dir ;
long x_rot , y_rot ;
bit_stack *bit ;

bit_stack* temp_bit = NULL ;
bit_stack* new_bit = NULL ;
long new_bit_val = (1<<20) ;

bit = armlink_addtolist( &bits ) ;


if (bit == NULL)
{

temp_bit=(bits.info).start_address ;

while ((temp_bit->header).status==1)
{

if ( (temp_bit->counter+(temp_bit->type<<1)) < new_bit_val )
{
	new_bit_val = temp_bit->counter+(temp_bit->type<<1) ;
	new_bit = temp_bit ;
}

temp_bit=(temp_bit->header).next_address ;
}

bit = new_bit ;

}


//if (bit != NULL)
//{

	// Psuedo random overall velocity
	velocity = (explosion_velocity>>1) + ( (explosion_velocity * ((arm_random())&15))>>4 );

	// arm_randomom rotation to base arm_randomom velocity adder on
	x_rot = ((arm_random())&1023) ;
	y_rot = ((arm_random())&1023) ;

	// arm_randomom velocity adder based on arm_randomom x,y rot
	// and base arm_randomom velocity 
	// Max value = 1 sprite map unit (1<<24)
	x_dir = (	cosine_table [ x_rot ] * cosine_table [ y_rot ]
				* velocity )>>5 ;
	y_dir = (	sine_table [ x_rot ] * cosine_table [ y_rot ]
				* velocity )>>5 ;
	z_dir = ( sine_table [ y_rot ] *velocity )<< 7 ;

	pointer = ((arm_random())&(MAX_BITS-1)) ;

	// Set start point of bit + arm_randomom bit
	bit->x_pos = x_pos + (x_dir>>1) ;
	bit->y_pos = y_pos + (y_dir>>1) ;
	bit->z_pos = z_pos + (z_dir>>1) ;

	// Check to see if the bit is below ground
	coll_check = check_collision( 	bit->x_pos ,
									bit->y_pos ,
									bit->z_pos ) ;

	// If the bit is below ground a negative value of
	// the ground height is set - Set height to this
	// value and make sure the z velocity is up
	if (coll_check<0)
	{
		bit->z_pos = -coll_check ;
		if (z_dir<0) z_dir = -z_dir ;
	}

	// Set velocity
	bit->x_vel = x_vel + x_dir ;
	bit->y_vel = y_vel + y_dir ;
	bit->z_vel = z_vel + z_dir ;

	// Set rotation and rotation speed arm_randomomly
	bit->x_rot = ((arm_random())&ROT_LIMIT) ;
	bit->y_rot = ((arm_random())&ROT_LIMIT) ;
	bit->z_rot = ((arm_random())&ROT_LIMIT) ;

	bit->x_r_vel = ((arm_random())&131071)-65536 ;
	bit->y_r_vel = ((arm_random())&131071)-65536 ;
	bit->z_r_vel = ((arm_random())&131071)-65536 ;

	// Set misc details
	bit->type = type ;
	bit->colour1 = colour1 ;
	bit->who_owns_me = owner ;
	
	switch( style )
	{
	case EXPLOSION_STYLE_NORMAL :

		bit->counter = 31+((arm_random())&31) ;	

	break ;
	
	case EXPLOSION_STYLE_MEGA_BOMB :

		bit->counter = 64+((arm_random())&63) ;
	
	break ;
	
	case EXPLOSION_STYLE_COMET :

		bit->counter = 128+((arm_random())&63) ;
	
	break ;
	
	case EXPLOSION_STYLE_LOADS_OF_BITS :
	
		bit->counter = 40+((arm_random())&31) ;
		
	break ;
	
	}
//}

}
