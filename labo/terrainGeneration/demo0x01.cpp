#include <iostream>
#include <cstdlib>
#include <time.h>
#include "png.cpp"

unsigned long int getSeed() {
	timespec tStruct;
	clock_gettime(CLOCK_REALTIME, &tStruct);
	return tStruct.tv_nsec;
}

typedef struct _terrain_ {
	int value;
	int applied;
} TERRAIN;


void UNIVERSE_NOISE_1(TERRAIN ** matrix, unsigned long int scale, unsigned long int offsetX, unsigned long int offsetY) {
	unsigned long int randX,randY;
	// TOP LEFT
	randX = getSeed() % scale;
	randY = getSeed() % scale;
	matrix[randX+offsetX][randY+offsetY].value=scale;

	// TOP RIGHT
	randX = getSeed() % scale;
	randY = getSeed() % scale;
	matrix[randX+offsetX+scale][randY+offsetY].value=scale;

	// BOTTOM RIGHT
	randX = getSeed() % scale;
	randY = getSeed() % scale;
	matrix[randX+offsetX+scale][randY+offsetY+scale].value=scale;

	// BOTTOM LEFT
	randX = getSeed() % scale;
	randY = getSeed() % scale;
	matrix[randX+offsetX][randY+offsetY+scale].value=scale;

	// Apply mask
	int x,y;
	for(x=0;x<scale;x++) {
		for(y=0;y<scale;y++) {
			matrix[x+offsetX][y+offsetY].applied +=matrix[x+offsetX][y+offsetY].value;
			matrix[x+offsetX+scale][y+offsetY].applied +=matrix[x+offsetX+scale][y+offsetY].value;
			matrix[x+offsetX+scale][y+offsetY+scale].applied +=matrix[x+offsetX+scale][y+offsetY+scale].value;
			matrix[x+offsetX][y+offsetY+scale].applied +=matrix[x+offsetX][y+offsetY+scale].value;
		}
	}
}

int main(int argc, char ** argv) {

	// Initiate shit n stuff
	unsigned long int scale = atoi(argv[1]);
	TERRAIN ** matrix = (TERRAIN **) malloc(sizeof(TERRAIN *) * scale);
	PIXEL ** png = (PIXEL **) malloc(sizeof(PIXEL *) * scale);
	int i,j;
	for (i=0; i<scale;i++) {
		matrix[i] = (TERRAIN *) malloc(sizeof(TERRAIN) * scale);
		png[i] = (PIXEL *) malloc(sizeof(PIXEL) * scale);
		for(j=0;j<scale;j++) {
			matrix[i][j].value = 0;
			matrix[i][j].applied = 0;

		}
	}
	UNIVERSE_NOISE_1(matrix, scale/2,0,0);

	// Adjust color Levels
	unsigned int max = 0;
	unsigned int min = 65536;
	for (i=0; i<scale;i++) {
		for(j=0;j<scale;j++) {
			if (matrix[i][j].applied > max) {
				max = matrix[i][j].applied;
			}
			if (matrix[i][j].applied < min) {
				min = matrix[i][j].applied;
			}
		}
	}

	char color;
	for (i=0; i<scale;i++) {
		for(j=0;j<scale;j++) {
			color		= (matrix[i][j].applied / max) * 255;
			png[i][j].Alpha	= 0xFF;
			png[i][j].Red	= color;
			png[i][j].Green	= color;
			png[i][j].Blue	= color;
		}
	}
	writePng(png,scale);
	return 0;
}
