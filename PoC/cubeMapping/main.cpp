#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include "png.cpp"

typedef struct heighmap {
	float * mask;
	int scale;
	float max;
	float * faces[6];
	float * top;
	float * left;
	float * right;
	float * bottom;
} HEIGHTMAP;

unsigned long int getRandom() {
        timespec tStruct;
        clock_gettime(CLOCK_REALTIME, &tStruct);
        return tStruct.tv_nsec;
}


void UNIVERSE_MASK_1( float * mask, int scale) {
	int x,y;
	float halfScale = scale >> 1;
	float radius;
	for(x=0;x<scale;x++) {
		for(y=0;y<scale;y++) {
			radius = sqrtl( ((y-halfScale) * (y-halfScale)) + ((x-halfScale) * (x-halfScale)));
			if ( radius < halfScale ) {
				mask[x + scale * y] = (halfScale - radius) / (halfScale);
			}
			else {
				mask[x + scale * y] = 0;
			}
		}
	}
}

void UNIVERSE_NOISE_1( HEIGHTMAP * heightmap, int scale, int offsetX, int offsetY, int faceId) {

	if (scale == 1) {
		return;
	}

	char doIt = getRandom() & 1;
	int halfScale = scale >> 1;
	int randX = - halfScale + getRandom() % (scale);
	int randY = - halfScale + getRandom() % (scale);
	int inc = heightmap->scale / scale;
	int maskIndex;
	int faceIndex;
	int maskX=0,maskY=0;

	float * dest;

	if (doIt) {
		// When mask fit exactly within face
		if ( randY+offsetY >= 0 && randY+offsetY+scale-1 < heightmap->scale && randX+offsetX >= 0 && randX+offsetX+scale-1< heightmap->scale) {
			for(int y=0; y<scale;y++) {
				maskX = 0;
				for(int x=0; x<scale;x++) {
					maskIndex = maskX + scale * maskY;
					if( heightmap->mask[maskIndex] != 0) {
						faceIndex = (y+randY+offsetY) * heightmap->scale + x+randX+offsetX;
						heightmap->faces[faceId][ faceIndex] += heightmap->mask[maskIndex] / (inc);
						if (heightmap->max < heightmap->faces[faceId][ faceIndex]) {
							heightmap->max = heightmap->faces[faceId][ faceIndex ];
						}
					}
					maskX += inc;
				}
				maskY += (inc * inc);
			}
		}
		else if ( randX+offsetX >= 0 && randX+offsetX+scale-1 < heightmap->scale ) {
			for(int y=0; y<scale;y++) {
				maskX = 0;
				for(int x=0; x<scale;x++) {
					maskIndex = maskX + scale * maskY;
					if( heightmap->mask[maskIndex] != 0) {
						if (randY+offsetY+y < 0) {
							dest = heightmap->top;
							faceIndex = (heightmap->scale + (y+randY+offsetY)) * heightmap->scale + x+randX+offsetX;
						}
						else if (randY+offsetY+y >= heightmap->scale) {
							dest = heightmap->bottom;
							faceIndex = ((y+randY+offsetY) - heightmap->scale) * heightmap->scale + x+randX+offsetX;
						}
						else {
							dest = heightmap->faces[faceId];
							faceIndex = ((y+randY+offsetY)) * heightmap->scale + x+randX+offsetX;
						}
						dest[ faceIndex] += heightmap->mask[maskIndex] / (inc);
						if (heightmap->max < dest[ faceIndex]) {
							heightmap->max = dest[ faceIndex ];
						}
					}
					maskX += inc;
				}
				maskY += (inc * inc);
			}
			
		}
	}

	UNIVERSE_NOISE_1(heightmap, halfScale, offsetX, 		offsetY,		faceId);
	UNIVERSE_NOISE_1(heightmap, halfScale, offsetX, 		offsetY+halfScale,	faceId);
	UNIVERSE_NOISE_1(heightmap, halfScale, offsetX+halfScale, 	offsetY+halfScale,	faceId);
	UNIVERSE_NOISE_1(heightmap, halfScale, offsetX+halfScale, 	offsetY,		faceId);
}

int main(int argc, char ** argv) {
	HEIGHTMAP heightmap;
	heightmap.max = 0;
	heightmap.scale = atoi(argv[1]);
	PIXEL ** png = (PIXEL **) malloc(sizeof(PIXEL *) * heightmap.scale * 3);
	heightmap.mask = (float *) malloc(sizeof(float) * heightmap.scale * heightmap.scale);

	// Cube faces
	heightmap.faces[0]	= (float *) malloc(sizeof(float) * heightmap.scale * heightmap.scale);
	heightmap.faces[1]	= (float *) malloc(sizeof(float) * heightmap.scale * heightmap.scale);
	heightmap.faces[2]	= (float *) malloc(sizeof(float) * heightmap.scale * heightmap.scale);
	heightmap.faces[3]	= (float *) malloc(sizeof(float) * heightmap.scale * heightmap.scale);
	heightmap.faces[4]	= (float *) malloc(sizeof(float) * heightmap.scale * heightmap.scale);
	heightmap.faces[5]	= (float *) malloc(sizeof(float) * heightmap.scale * heightmap.scale);

	for (int x =0; x < heightmap.scale * 3 ; x++) {
		png[x] = (PIXEL *) malloc(sizeof(PIXEL) * heightmap.scale * 4);
	}

	for(int y = 0; y<heightmap.scale; y++) {
		for(int x = 0; x<heightmap.scale; x++) {
			heightmap.faces[0][x + heightmap.scale*y]	= 0;
			heightmap.faces[1][x + heightmap.scale*y]	= 0;
			heightmap.faces[2][x + heightmap.scale*y]	= 0;
			heightmap.faces[3][x + heightmap.scale*y]	= 0;
			heightmap.faces[4][x + heightmap.scale*y]	= 0;
			heightmap.faces[5][x + heightmap.scale*y]	= 0;
		}
	}
	int X=0,Y=0;
	float test;
	float hyp = (heightmap.scale * 0.5) / 0.7071 ;
	
	UNIVERSE_MASK_1(heightmap.mask, heightmap.scale);

	heightmap.top = heightmap.faces[2];
	heightmap.bottom = heightmap.faces[3];
	heightmap.left = heightmap.faces[5];
	heightmap.right = heightmap.faces[4];
	UNIVERSE_NOISE_1(&heightmap, heightmap.scale, 0, 0, 0);

	heightmap.top = heightmap.faces[3];
	heightmap.bottom = heightmap.faces[2];
	heightmap.left = heightmap.faces[5];
	heightmap.right = heightmap.faces[4];
	UNIVERSE_NOISE_1(&heightmap, heightmap.scale, 0, 0, 1);

	heightmap.top = heightmap.faces[1];
	heightmap.bottom = heightmap.faces[0];
	heightmap.left = heightmap.faces[5];
	heightmap.right = heightmap.faces[4];
	UNIVERSE_NOISE_1(&heightmap, heightmap.scale, 0, 0, 2);
	
	heightmap.top = heightmap.faces[0];
	heightmap.bottom = heightmap.faces[1];
	heightmap.left = heightmap.faces[5];
	heightmap.right = heightmap.faces[4];
	UNIVERSE_NOISE_1(&heightmap, heightmap.scale, 0, 0, 3);

	int scale4 = heightmap.scale * 4;
	int scale3 = heightmap.scale * 3;
	int scale2 = heightmap.scale * 2;
	unsigned char color;
	// Beyond this remains the assembly of each faces in the png
	for(int y = 0; y<scale4; y++) {
		for(int x = 0; x<scale3; x++) {
			if (x >= heightmap.scale && x < scale2 && y >= heightmap.scale && y < scale2) { // plusZ
				color = (unsigned char) (255 * heightmap.faces[0][ (x-heightmap.scale) + heightmap.scale*(y-heightmap.scale)] / heightmap.max);
				png[x][y].Red	= color;
				png[x][y].Green = color;
				png[x][y].Blue = color;
			}
			else if (x >= heightmap.scale && x < scale2 && y >= 0 && y < heightmap.scale) { // plusY
				color = (unsigned char) (255 * heightmap.faces[2][ (x-heightmap.scale) + heightmap.scale*y]/ heightmap.max);
				png[x][y].Red	= color;
				png[x][y].Green = color;
				png[x][y].Blue = color;
			}
			else if (x >= heightmap.scale && x < scale2 && y >= scale2 && y < scale3) { // minusY
				color = (unsigned char) (255 * heightmap.faces[3][ (x-heightmap.scale) + heightmap.scale*(y-scale2)]/ heightmap.max);
				png[x][y].Red	= color;
				png[x][y].Green = color;
				png[x][y].Blue = color;
			}
			else if (x >= heightmap.scale && x < scale2 && y >= scale3 && y < scale4) { // minusZ
				color = (unsigned char) (255 * heightmap.faces[1][ (x-heightmap.scale) + heightmap.scale*(y-scale3)] / heightmap.max);
				png[x][y].Red = color; 
				png[x][y].Green	= color;
				png[x][y].Blue = color;
			}
			else if (x >= 0 && x < heightmap.scale && y >= heightmap.scale && y < scale2) { // minuxX
				color = (unsigned char) (255 * heightmap.faces[5][ x + heightmap.scale*(y-heightmap.scale)] / heightmap.max);
				png[x][y].Red = color; 
				png[x][y].Green	= color;
				png[x][y].Blue = color;
			}
			else if (x >= scale2 && x < scale3 && y >= heightmap.scale && y < scale2) { // plusX
				color =  (unsigned char) (255 * heightmap.faces[4][ (x-scale2) + heightmap.scale*(y-heightmap.scale)] / heightmap.max);
				png[x][y].Red = color; 
				png[x][y].Green	= color;
				png[x][y].Blue = color;
			}
			else {
				png[x][y].Red = 128; 
				png[x][y].Green	= 128;
				png[x][y].Blue = 128;
			}
			png[x][y].Alpha	= 255;
		}

	}

        writePng(png,heightmap.scale);
	
	return 0;
}
