/*-------------------------------------------------------------------------------

	BUBBENSTEIN/SDL
	File: r_draw.c
	Desc: contains special drawing functions

	Copyright 2013 (c) Sheridan Rathbun, all rights reserved.
	See LICENSE for details.

-------------------------------------------------------------------------------*/

#include "bsdl.h"

Uint32 ReadPixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void DrawPixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

/*-------------------------------------------------------------------------------

	r_DrawWeapons

	Draws the player's first person view weapons.

-------------------------------------------------------------------------------*/

int sway, swingmode;
double swing;

void r_DrawWeapons(void) {
	float screenfactorx, screenfactory;
	long x, y, index;
	long picx, picy, size;
	long x1, y1, x2, y2, x3, y3, x4, y4;
	Uint8 r, g, b;
	Uint8 *p;
	bitmap_t *gun_bmp = NULL;
	
	// move the images differently depending upon the screen size
	screenfactorx = ((float)(xres))/320.0;
	screenfactory = ((float)(yres))/240.0;
	
	// select the bitmap and generate the offset
	switch( selected_weapon ) {
		case 2:
			gun_bmp = &pistol_bmp[weap_anim];
			gunx = xres/2;
			guny = yres-(gun_bmp->height)*screenfactory;
			break;
		case 3:
			gun_bmp = &shotgun_bmp[weap_anim];
			gunx = xres/2;
			guny = yres-(gun_bmp->height)*screenfactory;
			break;
	}
	size = gun_bmp->width*gun_bmp->height;
	//gunx += bob2*screenfactor;
	//guny += (24+bob2+weap_swap[0])*screenfactor+vang*.05;
	
	if( i_GetStatus(IN_FORWARD,0) || i_GetStatus(IN_LEFT,0) || i_GetStatus(IN_BACK,0) || i_GetStatus(IN_RIGHT,0) ) {
		if( swing < 0 ) {
			swingmode = 0;
			swing = 0;
		}
		if( swing > 180 ) {
			swingmode = -2;
			swing = 180;
		}
		swing += (swingmode+1)*timesync*.5;
		sway += timesync;
	}
	sway *= max(1-.01*timesync,0);
	if(!weap_anim) {
		gunx += sway*cos(swing*PI/180)*.25;
		guny += (weap_swap[0]+4)*screenfactory+sway*sin(swing*PI/180)*.25;
	}
	else {
		gunx += sway*cos(swing*PI/180)*.125;
		guny += (weap_swap[0]+4)*screenfactory+sway*sin(swing*PI/180)*.125;
	}
	
	x1 = gunx-(gun_bmp->width*screenfactorx*.5);
	x2 = gunx+(gun_bmp->width*screenfactorx*.5);
	y1 = guny;
	y2 = guny+(gun_bmp->height*screenfactory);
	
	// draw the weapon
	x3=max(x1,0);
	x4=min(x2,xres);
	y3=max(y1,0);
	y4=min(y2,yres);
	for( y=y3; y<y4; y++ ) {
		p = (Uint8 *)screen->pixels + y * screen->pitch; // calculate the column we are drawing in
		picy = (y-y1)/screenfactory;
		for( x=x3; x<x4; x++ ) {
			picx = (x-x1)/screenfactorx;
			index = (picx+picy*gun_bmp->width);
			if( index >= 0 && index < size ) {
				index *= 4;
				r = gun_bmp->data[index];
				g = gun_bmp->data[index+1];
				b = gun_bmp->data[index+2];
				if( r || g!=255 || b!=255 )
					*(Uint32 *)((Uint8 *)p + x * screen->format->BytesPerPixel)=SDL_MapRGB( screen->format, r, g, b ); // draw a pixel
			}
		}
	}
	
	// draw a crosshair
	DrawPixel(screen, xres/2-1, yres/2-1, SDL_MapRGB( screen->format, 255, 255, 255 ));
	DrawPixel(screen, xres/2-1, yres/2, SDL_MapRGB( screen->format, 255, 255, 255 ));
	DrawPixel(screen, xres/2, yres/2-1, SDL_MapRGB( screen->format, 255, 255, 255 ));
	DrawPixel(screen, xres/2, yres/2, SDL_MapRGB( screen->format, 255, 255, 255 ));
}

/*-------------------------------------------------------------------------------

	r_DrawConsole

	Draws the console background or "wallpaper"

-------------------------------------------------------------------------------*/

void r_DrawConsole(void) {
	float screenfactorx, screenfactory;
	long x, y, index;
	long picx, picy, size;
	Uint8 *p;
	
	screenfactorx = ((float)(xres))/((float)console_bmp.width);
	screenfactory = ((float)(yres))/((float)console_bmp.height);
	size = console_bmp.width*console_bmp.height;
	
	for( y=0; y<yres; y++ ) {
		p = (Uint8 *)screen->pixels + y * screen->pitch; // calculate the column we are drawing in
		picy = y/screenfactory;
		for( x=0; x<xres; x++ ) {
			picx = x/screenfactorx;
			index = (picx+picy*console_bmp.width);
			if( index >= 0 && index < size ) {
				index *= 4;
				*(Uint32 *)((Uint8 *)p + x * screen->format->BytesPerPixel)=SDL_MapRGB( screen->format, console_bmp.data[index], console_bmp.data[index+1], console_bmp.data[index+2] );
			}
		}
	}
}

/*-------------------------------------------------------------------------------

	r_DrawSky

	Draws the sky as an image whose position depends upon the given angles.

-------------------------------------------------------------------------------*/

void r_DrawSky( double angle, double vangle ) {
	float screenfactor;
	int skyx, skyy;
	SDL_Rect dest;
	SDL_Rect src;
	
	if( !drawsky ) // no sky visible
		return;
	
	// move the images differently depending upon the screen size
	screenfactor = ((float)(xres))/320.0;

	// bitmap offsets
	skyx = -angle*((320*screenfactor)/(PI/2.0));
	skyy = (-114*screenfactor+vangle);

	src.x = -skyx;
	src.y = -skyy;
	src.w = (-skyx)+xres; // clip to the screen width
	src.h = (-skyy)+yres; // clip to the screen height
	dest.x = 0;
	dest.y = 0;
	dest.w = xres;
	dest.h = yres;
	
	SDL_BlitSurface(sky_bmp, &src, screen, &dest);
	
	// draw the part of the last part of the sky (only appears when angle > 270 deg.)
	if( skyx < -960*screenfactor ) {
		dest.x = 1280*screenfactor+skyx;
		dest.y = 0;
		dest.w = xres;
		dest.h = yres;
		src.x = 0;
		src.y = -skyy;
		src.w = xres-(-skyx-1280*screenfactor);
		src.h = src.y+yres;
		SDL_BlitSurface(sky_bmp, &src, screen, &dest);
	}
}

/*-------------------------------------------------------------------------------

	r_ScaleSurface

	Scales an SDL_Surface to the given width and height.

-------------------------------------------------------------------------------*/

SDL_Surface* r_ScaleSurface(SDL_Surface *Surface, Uint16 Width, Uint16 Height) {
	Sint32 x,y,o_x,o_y;
	
	if(!Surface || !Width || !Height)
		return NULL;
	
	SDL_Surface *_ret = SDL_CreateRGBSurface(Surface->flags, Width, Height, Surface->format->BitsPerPixel, Surface->format->Rmask, Surface->format->Gmask, Surface->format->Bmask, Surface->format->Amask);
	
	double _stretch_factor_x = (double)Width/(double)Surface->w;
	double _stretch_factor_y = (double)Height/(double)Surface->h;
	
	for(y = 0; y < Surface->h; y++)
		for(x = 0; x < Surface->w; x++)
			for(o_y = 0; o_y < _stretch_factor_y; ++o_y)
				for(o_x = 0; o_x < _stretch_factor_x; ++o_x)
					DrawPixel(_ret, (Sint32)(_stretch_factor_x * x) + o_x, (Sint32)(_stretch_factor_y * y) + o_y, ReadPixel(Surface, x, y));
	
	return _ret;
}