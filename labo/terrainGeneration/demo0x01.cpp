#include <iostream>
#include <cstdlib>
#include <time.h>
#include "png.cpp"
#include <math.h>

unsigned long int getSeed() {
	timespec tStruct;
	clock_gettime(CLOCK_REALTIME, &tStruct);
	return tStruct.tv_nsec;
}

void UNIVERSE_NOISE_1(unsigned long int ** matrix, long int scale, int offsetX, long int offsetY, unsigned long int * max) {
	long int halfScale= scale >> 1;
	long int x,y;
	long int randX0, randY0, randX1, randY1, randX2, randY2,randX3,randY3;
	randX0 = (getSeed() % halfScale) - halfScale;
	randY0 = (getSeed() % halfScale) - halfScale;
	randX1 = (getSeed() % halfScale);
	randY1 = (getSeed() % halfScale) - halfScale;
	randX2 = (getSeed() % halfScale);
	randY2 = (getSeed() % halfScale);
	randX3 = (getSeed() % halfScale) - halfScale;
	randY3 = (getSeed() % halfScale);
	char rand = getSeed() & 15;
	for(y=0;y<scale;y++) {
		for(x=0;x<scale;x++) {
			//TOP RIGHT
			if((rand & 1 ) && offsetX+x+randX0 >= 0 && offsetX+x+randX0 < scale && y+randY0 >= 0 && y+randY0 < scale) {
				if (x < y && x < scale-y ) {
					matrix[offsetX+x+randX0][y+randY0] += x;
				}
				else if (x >= scale-y && scale-x <= scale-y) {
					matrix[offsetX+x+randX0][y+randY0] += scale-x;
				}
				else if (y <= x && y < scale-x) {
					matrix[offsetX+x+randX0][y+randY0] += y;
				}
				else if (y >= scale-x && scale-y <= scale-y) {
					matrix[offsetX+x+randX0][y+randY0] += scale-y;
				}
				if (*max < matrix[offsetX+x+randX0][y+randY0]) {
					*max = matrix[offsetX+x+randX0][y+randY0];
				}
			}/*
			//TOP RIGHT
			if((rand & 2) && offsetX+x+randX1 >= 0 && offsetX+x+randX1 < scale && y+randY1 >= 0 && y+randY1 < scale) {
				if (x < y && x < scale-y ) {
					matrix[offsetX+x+randX1][y+randY1] += x;
				}
				else if (x >= scale-y && scale-x <= scale-y) {
					matrix[offsetX+x+randX1][y+randY1] += scale-x;
				}
				else if (y <= x && y < scale-x) {
					matrix[offsetX+x+randX1][y+randY1] += y;
				}
				else if (y >= scale-x && scale-y <= scale-y) {
					matrix[offsetX+x+randX1][y+randY1] += scale-y;
				}
				if (*max < matrix[x+randX1][y+randY1]) {
					*max = matrix[offsetX+x+randX1][y+randY1];
				}
			}
			//BOTTOM RIGHT
			if((rand & 4) && offsetX+x+randX2 >= 0 && offsetX+x+randX2 < scale && y+randY2 >= 0 && y+randY2 < scale) {
				if (x < y && x < scale-y ) {
					matrix[offsetX+x+randX2][y+randY2] += x;
				}
				else if (x >= scale-y && scale-x <= scale-y) {
					matrix[offsetX+x+randX2][y+randY2] += scale-x;
				}
				else if (y <= x && y < scale-x) {
					matrix[offsetX+x+randX2][y+randY2] += y;
				}
				else if (y >= scale-x && scale-y <= scale-y) {
					matrix[offsetX+x+randX2][y+randY2] += scale-y;
				}
				if (*max < matrix[x+randX2][y+randY2]) {
					*max = matrix[offsetX+x+randX2][y+randY2];
				}
			}
			//BOTTOM LEFT
			if((rand & 8) && offsetX+x+randX3 >= 0 && offsetX+x+randX3 < scale && y+randY3 >= 0 && y+randY3 < scale) {
				if (x < y && x < scale-y ) {
					matrix[offsetX+x+randX3][y+randY3] += x;
				}
				else if (x >= scale-y && scale-x <= scale-y) {
					matrix[offsetX+x+randX3][y+randY3] += scale-x;
				}
				else if (y <= x && y < scale-x) {
					matrix[offsetX+x+randX3][y+randY3] += y;
				}
				else if (y >= scale-x && scale-y <= scale-y) {
					matrix[offsetX+x+randX3][y+randY3] += scale-y;
				}
				if (*max < matrix[x+randX3][y+randY3]) {
					*max = matrix[offsetX+x+randX3][y+randY3];
				}
			}*/
		}
	}
}

int main(int argc, char ** argv) {

	// Initiate shit n stuff
	long int scale = atoi(argv[1]);
	unsigned long int ** matrix = (unsigned long int **) malloc(sizeof(unsigned long int *) * scale);
	PIXEL ** png = (PIXEL **) malloc(sizeof(PIXEL *) * scale);
	long unsigned int i,j,max=0;
	char color;
	for (i=0; i<scale;i++) {
		matrix[i] = (unsigned long int *) malloc(sizeof(unsigned long int) * scale);
		png[i] = (PIXEL *) malloc(sizeof(PIXEL) * scale);
		for(j=0;j<scale;j++) {
			matrix[i][j] = 0;

		}
	}
	// DRAW MAP RECURSIVELY
	UNIVERSE_NOISE_1(matrix, scale,256,0, &max);
	// COLOR LEVEL ADJUSTEMENT
	for (i=0; i<scale;i++) {
		for(j=0;j<scale;j++) {
			color		= ((double) (matrix[i][j]) / max) * 255;
			png[i][j].Alpha	= 0xFF;
			png[i][j].Red	= color;
			png[i][j].Green	= color;
			png[i][j].Blue	= color;
		}
	}
	writePng(png,scale);
	return 0;
}
