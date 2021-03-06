/*-------------------------------------------------------------------------------

	BUBBENSTEIN/SDL
	File: r_data.c
	Desc: contains graphical data structure handling functions

	Copyright 2013 (c) Sheridan Rathbun, all rights reserved.
	See LICENSE for details.

-------------------------------------------------------------------------------*/

#include "bsdl.h"

/*-------------------------------------------------------------------------------

	r_ClearBuffers
	
	Clears the data buffers used to draw one frame

-------------------------------------------------------------------------------*/

void r_ClearBuffers(void) {
	char c;
	float f;
	int i;
	memset( zbuffer, 0, xres*yres*sizeof(f) );
	memcpy( floorbuffer, floorbuffer_s, xres*yres*sizeof(i) );
	memset( rowbuffer, 0, yres*sizeof(c) );
	drawsky=0;
}

/*-------------------------------------------------------------------------------

	r_FreeBmp

	Frees the the given bitmap from memory

-------------------------------------------------------------------------------*/

void r_FreeBmp( bitmap_t *bmp ) {
	free( bmp->data );
}

/*-------------------------------------------------------------------------------

	r_LoadBmp

	Load the given bitmap into memory

-------------------------------------------------------------------------------*/

void r_LoadBmp( char *file, bitmap_t *bmp ) {
	FILE *fp;
	long sx, sy, l;
	char valid_data[3];
	bitmap_t bmp2;

	// open the file
	if((fp = fopen(file,"rb")) == NULL ) {
		g_Close();
		printf( "ERROR: BMP file '%s' does not exist!\n\n", file );
		exit(2);
	}

	// read the data
	valid_data[0] = fgetc(fp);
	valid_data[1] = fgetc(fp);
	if( valid_data[0] != 'B' || valid_data[1] != 'M' ) { // identify the data
		fclose(fp);
		g_Close();
		printf( "ERROR: BMP file '%s' is invalid.\n\n", file );
		exit(3);
	}
	for( sx=0; sx<16; sx++ ) // the next 16 bytes are useless information
		fgetc(fp);

	// get the width/height of the bitmap and allocate memory
	fread(&bmp2.width, 4, 1, fp);
	fread(&bmp2.height, 4, 1, fp);
	bmp2.data = (char *) malloc(bmp2.width*bmp2.height*4);
	
	bmp->width = bmp2.width;
	bmp->height = bmp2.height;
	bmp->data = (char *) malloc(bmp2.width*bmp2.height*4);

	for( sx=0; sx<28; sx++ ) // the next 28 bytes comprise useless information
		fgetc(fp);
	for( sx=0; sx<bmp2.width*bmp2.height; sx++ ) { // grab the data
		bmp2.data[(sx<<2)] = fgetc(fp);
		bmp2.data[(sx<<2)+1] = fgetc(fp);
		bmp2.data[(sx<<2)+2] = fgetc(fp);
		if( sx%bmp2.width >= bmp2.width-1 ) // end of the line
			for( sy=0; sy<bmp2.width%4; sy++ ) {
				fgetc(fp);
			}
	}

	// close the file
	fclose(fp);

	// now the data is upside down, we must reverse it vertically
	l=bmp2.width*bmp2.height-bmp2.width;
	for( sx=0; sx<bmp2.width; sx++ ) {
		for( sy=0; sy<bmp2.height; sy++ ) {
			bmp->data[((sy*bmp2.width+sx)<<2)] = bmp2.data[((sx+(l-sy*bmp2.width))<<2)+2];
			bmp->data[((sy*bmp2.width+sx)<<2)+1] = bmp2.data[((sx+(l-sy*bmp2.width))<<2)+1];
			bmp->data[((sy*bmp2.width+sx)<<2)+2] = bmp2.data[((sx+(l-sy*bmp2.width))<<2)];
		}
	}

	free( bmp2.data );
}