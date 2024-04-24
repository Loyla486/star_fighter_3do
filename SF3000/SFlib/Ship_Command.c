#include "SF_Mission.h"#include "Smoke_Control.h"#include "Graphic_Struct.h"#include "Plot_Graphic.h"#include "Collision.h"#include "Laser_Control.h"#include "Explosion.h"#include "SF_ArmLink.h"#include "Stdio.h"#include "Weapons.h"#include "Sound_Control.h"#include "Maths_Stuff.h"#include "Misc_Struct.h"#include "SF_Control.h"#include "Ship_Control.h"#include "Ship_Command.h"#include "Plot_Graphic.h"#include "test_prog.h"//###########################################################//#															#//#															#//#		Small ship cammand control							#//#															#//#															#//###########################################################void small_ship_command_update( ship_stack *ship){ship_stack *temp_ship ;long x_rot , y_rot , z_rot ;long x_dist , y_dist , z_dist , dist ;long temp_long , temp_long2 ;long coll_check ;rotate_node node_data ;if (ship->attacker_rating == 197 ) {	new_fighter_control( ship );	return;}//ship->command = COMMAND_AVOID_HILLS ;// Dec the command counter - when 0 then do another commandship->command_counter -= 1 ;switch ( ship->command ){	case COMMAND_TAXI_TURN :		// Turn the ship to face south for takeoff	ship->x_rot = ((ship->x_rot+ ((256/COMMAND_TAXI_TURN_COUNTER)*1024) )&ROT_LIMIT) ;	ship->fire_request = WEAPON_NOTHING ;		// What happens when its finished this command	if (ship->command_counter <= 0)	{		ship->command = COMMAND_TAXI_TAKEOFF ;		ship->command_counter = (COMMAND_TAXI_TAKEOFF_COUNTER>>1) ;	}		break ;	case COMMAND_TAXI_TAKEOFF :	// Put the engines on abit	ship->thrust_control = 512 ;	ship->fire_request = WEAPON_NOTHING ;	ship->y_control = 64 ;	// What happens when its finished this command	if (ship->command_counter <= 0)	{		ship->command = COMMAND_TAKEOFF_CLIMB ;		ship->command_counter = COMMAND_TAKEOFF_CLIMB_COUNTER ;	}	break ;	case COMMAND_TAKEOFF_CLIMB :	// Floor it and climb	ship->thrust_control = 1024 ;		ship->y_control = 160 ;		ship->fire_request = WEAPON_NOTHING ;		// What happens when its finished this command	if (ship->command_counter <= 0)	{		ship->command = COMMAND_ATTACK_MODE_1 ;		// You are free from the hanger you may now crash into it		// and other ships from the same hanger		ship->who_owns_me = (long) ship ;					ship->command = COMMAND_ATTACK ;		ship->command_counter = 512 ;		ship->attacker_rating = 197 ;	}	break ;	case COMMAND_BIGSHIP_LAUNCH :	ship->command = COMMAND_TAKEOFF_CLIMB ;	ship->command_counter = COMMAND_TAKEOFF_CLIMB_COUNTER ;	break ;		default :		ship->attacker_rating = 197 ;	}}void small_ship_damaged( ship_stack* ship ){ship->damage_counter -= 1 ;// Check the low bits only - top bits used to signify hill rebounds etcswitch ( (ship->damage)&127 ){case DAMAGE_SLIGHT :	small_fighter_damage_wobble( ship , 1 + (arm_random()&1) ) ;		if ( ship->damage_counter <= 0)	{		ship->damage = DAMAGE_NOTHING ;		ship->damage_counter = DAMAGE_NOTHING_COUNTER ;	}	// If the counter has reached half value then clear the rebound bit	if ( ((ship->damage_counter)>>1) == (DAMAGE_SLIGHT_COUNTER>>1) )	{		ship->damage = ( ship->damage&127 ) ;	}break ;case DAMAGE_MEDIUM :	small_fighter_damage_wobble( ship , 2 + (arm_random()&1) ) ;	// 50 50 chance of a faster escape	if ( ship->damage_counter <= 0)	{		if ( (arm_random()&1) == 0)		{			ship->damage = DAMAGE_SLIGHT ;			ship->damage_counter = DAMAGE_SLIGHT_COUNTER ;		}		else		{			ship->damage = DAMAGE_NOTHING ;			ship->damage_counter = DAMAGE_NOTHING_COUNTER ;		}				}	// If the counter has reached half value then clear the rebound bit	if ( ((ship->damage_counter)>>1) == (DAMAGE_MEDIUM_COUNTER>>1) )	{		ship->damage = ( ship->damage&127 ) ;	}break ;case DAMAGE_BAD :	small_fighter_damage_wobble( ship , 3 + (arm_random()&3) ) ;	if ( ship->damage_counter <= 0)	{		if ( (arm_random()&1) == 0)		{			ship->damage = DAMAGE_SLIGHT ;			ship->damage_counter = DAMAGE_SLIGHT_COUNTER ;		}		else		{			ship->damage = DAMAGE_MEDIUM ;			ship->damage_counter = DAMAGE_MEDIUM_COUNTER ;		}	}	// If the counter has reached half value then clear the rebound bit	if ( ((ship->damage_counter)>>1) == (DAMAGE_BAD_COUNTER>>1) )	{		ship->damage = ( ship->damage&127 ) ;	}break ;case DAMAGE_VERY_BAD :	small_fighter_damage_wobble( ship , 5 + (arm_random()&3) ) ;	// Make there y direction head down	if ( ship->y_dir < (256*1024) && ship->y_dir > (3*256*1024) )	{		ship->y_dir += (128*(arm_random()&1023)) ;	}	else	{		ship->y_dir -= (128*(arm_random()&1023)) ;		}	if ( ship->damage_counter <= 0)	{		if ( (arm_random()&1) == 0)		{			ship->damage = DAMAGE_BAD ;			ship->damage_counter = DAMAGE_BAD_COUNTER ;		}		else		{			ship->damage = DAMAGE_MEDIUM ;			ship->damage_counter = DAMAGE_MEDIUM_COUNTER ;		}	}	// If the counter has reached half value then clear the rebound bit	if ( ((ship->damage_counter)>>1) == (DAMAGE_VERY_BAD_COUNTER>>1) )	{		ship->damage = ( ship->damage&127 ) ;	}break ;}}void small_fighter_damage_wobble( ship_stack* damaged_ship , long how_bad ){long temp_long ;// how_bad 1 - 8// will make a small fighter less controlable// roll it about abitif (damaged_ship->z_roller >= 0){	damaged_ship->z_roller += ( how_bad * ( (arm_random()&4095)-512 )) ;}else{	damaged_ship->z_roller -= ( how_bad * ( (arm_random()&4095)-512 )) ;}damaged_ship->thrust_control += ( how_bad * ( (arm_random()&63)-32 )) ;damaged_ship->y_rot += (how_bad*((arm_random()&1023)-512)) ;damaged_ship->x_control -= ( ( (how_bad*(arm_random()&127)) * damaged_ship->x_control )>>10 ) ;damaged_ship->y_control -= ( ( (how_bad*(arm_random()&127)) * damaged_ship->y_control )>>10 ) ;if ( (arm_random()&1) == 0){	temp_long = SMALL_DAMAGE_SMOKE ;}else{	temp_long = SMALL_EXPLOSION_SMOKE ;}add_smoke( 	(damaged_ship->x_pos) + ( ( (arm_random()&255) - 128 )<<13 ) + damaged_ship->x_vel ,			(damaged_ship->y_pos) + ( ( (arm_random()&255) - 128 )<<13 ) + damaged_ship->y_vel ,			(damaged_ship->z_pos) + ( ( (arm_random()&255) - 128 )<<13 ) + damaged_ship->z_vel ,			damaged_ship->x_vel ,			damaged_ship->y_vel ,			damaged_ship->z_vel ,			temp_long ,			0 ) ;}