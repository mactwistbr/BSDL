/*-------------------------------------------------------------------------------

	BUBBENSTEIN/SDL
	File: g_main.c
	Desc: main game source file. initializes engine and starts game.

	Copyright 2013 (c) Sheridan Rathbun, all rights reserved.
	See LICENSE for details.

-------------------------------------------------------------------------------*/

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include "bsdl.h"

/*-------------------------------------------------------------------------------

	main

	Contains the main game loop.

-------------------------------------------------------------------------------*/

int main( int argc, char **argv ) {
	SDL_Rect src, dest;
	src.x=1; dest.x=0;
	src.y=1; dest.y=0;
	src.w=xres*screenscale-screenscale-1; dest.w=0;
	src.h=yres*screenscale-screenscale-1; dest.h=0;
	
	// get a new random seed
	srand(time(0));
	
	// start the engine
	if( argc > 1 && argv[1] != NULL )
		g_Open(argv[1]);
	else
		g_Open("testmap.bsm");
	
	while(gameloop) {
		// run game loop functions
		i_GetFrameRate();
		i_ReceiveInput();
		e_Cycle();
		
		// rendering
		r_ClearBuffers();
		r_DrawSky( camang, vang );
		r_DrawColumns( camx, camy, camz, camang, vang );
		r_DrawFloors( camx, camy, camz, camang, vang );
		r_DrawSprites( camx, camy, camz, camang, vang );
		r_DrawWeapons();
		
		// interface
		if( message_time > 0 ) {
			i_PrintText( font16_bmp, 4, 4, "%s", message_str );
			message_time -= timesync;
		}
		i_PrintText( font8_bmp, 4, yres-20, "FPS: %d", fps );
		
		// paste the drawing surface to the screen surface, resizing it
		//SDL_BlitSurface( SPG_Scale( screen, screenscale, screenscale ), &src, screen2, &dest );
		SDL_BlitSurface( screen, &src, screen2, &dest );
		
		// flip the page
		SDL_Flip( screen2 );
	}
	
	g_Close(); // stop the engine
	return(0);
}