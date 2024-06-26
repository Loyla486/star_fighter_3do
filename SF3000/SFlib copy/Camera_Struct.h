#ifndef __CAMERA_SET_H
#define __CAMERA_SET_H

#define MAX_CAMERAS 3

#define CAMERA_NORMAL	0
#define CAMERA_INTERNAL 1
#define CAMERA_TRACKING 2
#define CAMERA_FLYBY	3



typedef struct camera_data 
					{	struct ship_stack *view_ship ;
						long x_rot ; long y_rot ;
						long zoom ;
						long type ;
						long view ;
						long instance;
						long x_pos ; long y_pos ; long z_pos ;
						long x_pos2 ; long y_pos2 ; long counter2 ;
						long counter ;
					} camera_data
					;

#endif
