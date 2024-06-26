
/* Include Headers */

#include "draw_frame.h"
#include "SF_Io.h"
#include "Smoke_Control.h"
#include "Rotate_Land.h"
#include "Draw_Land.h"
#include "Plot_Graphic.h"
#include "Laser_Control.h"
#include "Bit_Control.h"
#include "SF_ArmLink.h"
#include "Explosion.h"
#include "Weapons.h"
#include "Collision.h"
#include "Maths_Stuff.h"
#include "Misc_Struct.h"
#include "Ground_Control.h"
#include "Bonus_Control.h"
#include "SF_Control.h"
#include "Stdio.h"
#include "SF_War.h"
#include "SF_ArmUtils.h"
#include "Sound_Control.h"
#include "Graphic_Struct.h"

long test_cam = 0 ;

void draw_frame( long camera_number )
{

long coll_check ;
static long engine_zoom = 0 ;
static long last_camera = 0 ;
ship_stack *view_ship = camera[camera_number].view_ship ;
long x_dist , y_dist , z_dist , x_y_dist ;
target_struct tracking_camera ;
rotate_node node_data ;
graphics_details *details = (graphics_details*) ships_adr ;

static long dead_cam_rot = 0 ;

long ship_size = ( (1<<22) + ((details+view_ship->type)->clip_size) )>>12 ;

long old_camera_x_position = camera_x_position ;
long old_camera_y_position = camera_y_position ;
long old_camera_z_position = camera_z_position ;

long docked_camera = 0 ;

//camera[camera_number].counter2 = 0 ;

if (	((view_ship->type)>>4) == PLAYERS_SHIP &&
		docked.status != DOCKING_OUT )
{
	docked_camera = 1 ;
	view_ship = docked.ship ;
}

// Test disable the camera
if (test_mode == 1)
{
	docked_camera = 0 ;
	camera[camera_number].type = -1 ;
}

// Is it camera type 0 - normal spin round ship

if ( (camera[camera_number].type) == CAMERA_NORMAL && docked_camera == 0 && camera[camera_number].counter2 == 0 )
{

	// Get the ships y rotation between positive and negative 512*1024 (+/- 180 degrees)
	// and multiply the camera x rotation cosine to make side view level


	//add on the camera y rot and limit the y rotation
	camera_y_rotation = ( (view_ship->y_rot) + (camera[camera_number].y_rot) )&ROT_LIMIT ;

	if ( camera_y_rotation >512*1024 ) camera_y_rotation -= 1024*1024 ;

	camera_y_rotation = ((camera_y_rotation * 
						cosine_table [ ( (camera[camera_number].x_rot)&ROT_LIMIT )>>10 ])>>12)
						&ROT_LIMIT ;

	//Setup the x,y,z distances from the ship being viewed		

	//Move the camera to the left or right based on climb / dive when banked over
	node_data.x_pos = ( (view_ship->y_control)*( sine_table [ (view_ship->z_rot)>>10 ] ) )>>1;//>>5 ;

	//Move the camera back from the ship based on the zoom and duration thrust is used
	//node_data.y_pos =  -((camera[camera_number].zoom)+engine_zoom)<<12 ;
	node_data.y_pos =  -( ( (camera[camera_number].zoom * ship_size )>>10 )+engine_zoom)<<12 ;
	
				
	//Move the camera up/down based on climb / dive when banked flat
	//Plus move the camera above the ship based on zoom and cosine camera y
	node_data.z_pos =  ( (( (camera[camera_number].zoom * ship_size)>>10 )>>2) * ( cosine_table [ ((camera[camera_number].y_rot)&ROT_LIMIT)>>10 ] )) + 
						(( (view_ship->y_control)*( cosine_table [ (view_ship->z_rot)>>10 ] ) )>>3) ; // >>7) ;

	//Setup the x,y rot around the ship being viewed
	node_data.x_rot = ((view_ship->x_rot)+(camera[camera_number].x_rot))&ROT_LIMIT ;
	node_data.y_rot = camera_y_rotation ;
	node_data.z_rot = 0 ;

	rotate_node_from_c ( &node_data ) ;

//	if ( (view_ship->type)>>4 == BIG_SHIP)
//	{
		camera_x_position = (view_ship->x_pos) + ((node_data.x_pos)) ;
		camera_y_position = (view_ship->y_pos) + ((node_data.y_pos)) ;
		camera_z_position = (view_ship->z_pos) + ((node_data.z_pos)) ;// <<4
//	}
//	else
//	{
//		camera_x_position = (view_ship->x_pos) + ((node_data.x_pos)<<1) ;
//		camera_y_position = (view_ship->y_pos) + ((node_data.y_pos)<<1) ;
//		camera_z_position = (view_ship->z_pos) + ((node_data.z_pos)<<1) ;
//	}

	camera_x_rotation = ((512+((camera[camera_number].x_rot)>>10)+((view_ship->x_rot)>>10))&1023) ;
	camera_y_rotation = camera_y_rotation>>10 ;
	
	// Nice rolly the horizon effect for these types of ship - else keep it flat
	if (	(view_ship->type>>4) == BIG_SHIP ||
			(view_ship->type>>4) == SMALL_SHIP ||
			(view_ship->type>>4) == PLAYERS_SHIP )
	{
		camera_z_rotation = ( sine_table [ (view_ship->z_rot)>>10 ] 
							* cosine_table [ ( (camera[camera_number].x_rot)&ROT_LIMIT )>>10 ]
							>>19 )&1023 ;
	}
	else
	{
		camera_z_rotation = 0 ;
	}
}


// Is it camera type 1+ - tracking camera / fly by
if ( 	(camera[camera_number].type)==CAMERA_TRACKING || 
		(camera[camera_number].type)==CAMERA_FLYBY ||
		docked_camera != 0 ||
		camera[camera_number].counter2 > 0 )
{


	// Is it a fixed tracking camera
	if ( (camera[camera_number].type)==CAMERA_TRACKING || camera[camera_number].counter2 > 0 )
	{
		// Is it a big ship
		if ( (view_ship->type)>>4 == BIG_SHIP && camera[camera_number].counter2 == 0 )
		{
			// Base camera position on ship pos with bottom bits cleared
			camera_x_position = (((view_ship->x_pos)>>30)<<30)+(1<<29) ;
			camera_y_position = (((view_ship->y_pos)>>30)<<30)+(1<<29) ;
			camera_z_position = (((view_ship->z_pos)>>30)<<30)+(1<<29) ;
		}
		else
		{
			// Base camera position on ship pos with bottom bits cleared
			camera_x_position = (((view_ship->x_pos)>>28)<<28)+(1<<27) ;
			camera_y_position = (((view_ship->y_pos)>>28)<<28)+(1<<27) ;
			camera_z_position = (((view_ship->z_pos)>>28)<<28)+(1<<27) ;
		}
	}


	// Has the camera just been switched to fly by - if so then define position 
	// Or is the camera counter less than 0
	if ( (camera[camera_number].type)==CAMERA_FLYBY && camera[camera_number].counter2 == 0 )
	{
		if ( 	(camera[camera_number].type)!=last_camera ||
				camera[camera_number].counter<0 )
		{
			if ( ((view_ship->type)>>4) == BIG_SHIP )
			{
				// Flyby cam for big ships
				camera_x_position = (view_ship->x_pos)+((view_ship->x_vel)<<6)+(( ( (arm_random()) &255) +128)<<18) ;
				camera_y_position = (view_ship->y_pos)+((view_ship->y_vel)<<6)+(( ( (arm_random()) &255) +128)<<18) ;
				camera_z_position = (view_ship->z_pos)+((view_ship->z_vel)<<6)+(( ( (arm_random()) &255) -128)<<18) ;
				camera[camera_number].counter = 128 ;
			}
			else
			{
				// Flyby cam for small ships
				camera_x_position = (view_ship->x_pos)+((view_ship->x_vel)<<4)+(( ( (arm_random()) &255) +128)<<16) ;
				camera_y_position = (view_ship->y_pos)+((view_ship->y_vel)<<4)+(( ( (arm_random()) &255) +128)<<16) ;
				camera_z_position = (view_ship->z_pos)+((view_ship->z_vel)<<4)+(( ( (arm_random()) &255) -128)<<16) ;
				camera[camera_number].counter = 32 ;
			}			
		}
	}

	// Is it a internal docking bay view or wot ?
	if (	(camera[camera_number].type) != CAMERA_TRACKING &&
			(camera[camera_number].type) != CAMERA_FLYBY &&
			camera[camera_number].counter2 == 0 )
	{
		node_data.x_pos = 0 ;
		node_data.y_pos = ( (1<<23) + (1<<22) + (1<<21) ) ;// - (docked.counter<<21) ;
		node_data.z_pos = ((1<<22)+(1<<20)) ;
		node_data.x_rot = view_ship->x_rot ;
		node_data.y_rot = view_ship->y_rot ;
		node_data.z_rot = view_ship->z_rot ;
		rotate_node_from_c ( &node_data ) ;
		camera_x_position = view_ship->x_pos + node_data.x_pos ;
		camera_y_position = view_ship->y_pos + node_data.y_pos ;
		camera_z_position = view_ship->z_pos + node_data.z_pos ;
		view_ship = camera[camera_number].view_ship ;
	}

	if ( camera[camera_number].counter2 > 0 )
	{
		dead_cam_rot += 16 ;
		if (dead_cam_rot >= 1024) dead_cam_rot -= 1024 ;

		camera_x_position =  camera[camera_number].x_pos + (cosine_table [ dead_cam_rot ]<<15) ;
		camera_y_position =  camera[camera_number].y_pos + (sine_table [ dead_cam_rot ]<<15) ;
		camera_z_position =  camera[camera_number].z_pos + (8<<24) ;
	}

	// Are we in space
	if (planet_info.space_mission != 1)
	{
		// Check that the camera is above the hill level if less than set to min height

		coll_check = find_ground_height( camera_x_position , camera_y_position ) ;
		if (camera_z_position < (coll_check+(1<<24)) ) camera_z_position = coll_check + (1<<24) ;
	}
	

	
	// Use the target finder to calculate the cameras x,y rotation
	// Note the x and y positions are flipped over compared to
	// normal as used in targeting missiles etc.
	tracking_camera.x_aim = camera_x_position ;
	tracking_camera.y_aim = camera_y_position ;

	tracking_camera.z_pos = camera_z_position ;

	if ( camera[camera_number].counter2 > 0 )
	{
		tracking_camera.x_pos = camera[camera_number].x_pos ;
		tracking_camera.y_pos = camera[camera_number].y_pos ;
		tracking_camera.z_aim = camera[camera_number].z_pos ;	
	}
	else
	{
		tracking_camera.x_pos = view_ship->x_pos ;
		tracking_camera.y_pos = view_ship->y_pos ;
		tracking_camera.z_aim = view_ship->z_pos ;
	}

	target_finder( &tracking_camera );

	camera_x_rotation = (tracking_camera.x_rot)>>10 ;
	camera_y_rotation = (tracking_camera.y_rot)>>10 ;
	camera_z_rotation = 0 ;

}

if ( (camera[camera_number].type) == CAMERA_INTERNAL && docked_camera == 0 && camera[camera_number].counter2 == 0 )
{
	node_data.x_pos = 0 ;
	node_data.y_pos = 0 ;
	node_data.z_pos = (1<<19) ;
	node_data.x_rot = view_ship->x_rot ;
	node_data.y_rot = view_ship->y_rot ;
	node_data.z_rot = view_ship->z_rot ;
	rotate_node_from_c ( &node_data ) ;
	camera_x_position = view_ship->x_pos + node_data.x_pos ;
	camera_y_position = view_ship->y_pos + node_data.y_pos ;
	camera_z_position = view_ship->z_pos + node_data.z_pos ;

	camera_x_rotation = (((view_ship->x_rot)+512*1024)&ROT_LIMIT)>>10 ;
	camera_y_rotation = (view_ship->y_rot)>>10 ;
	camera_z_rotation = (view_ship->z_rot)>>10 ;

	ship_being_viewed = view_ship ;
}
else
{
	ship_being_viewed = 0 ;
}

engine_zoom += ((view_ship->thrust_control)>>6) ;

engine_zoom -= (engine_zoom>>4) ;

if (planet_info.space_mission != 1)
{
	// Prepare the rotations and nodes for the landscape
	rotate_land () ;	
}

	// Plot all landscape sections and all graphics
	draw_land () ;


wave_counter = (wave_counter+16)&1023 ;
wave_counter2 = (wave_counter2+16)&1023 ;
wave_counter3 = (wave_counter3+16)&1023 ;

// Cycle the collision box atg/ata highlight thingy colour
collision_box_colour_adder -= 1;
if (collision_box_colour_adder<0) collision_box_colour_adder=7;

//keep a record of the last camera used to check for camera changes
last_camera = camera[camera_number].type ;
//update camera counter
camera[camera_number].counter -= 1 ;

camera_x_velocity = old_camera_x_position - camera_x_position ;
camera_y_velocity = old_camera_y_position - camera_y_position ;
camera_z_velocity = old_camera_z_position - camera_z_position ;

// Update the engine sounds for the 2 near by ships
update_engine_sounds() ;

}

