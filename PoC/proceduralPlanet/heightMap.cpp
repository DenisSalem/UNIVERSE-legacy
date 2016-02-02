#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <math.h>

unsigned long int getRandom() {
        timespec tStruct;
        clock_gettime(CLOCK_REALTIME, &tStruct);
        return tStruct.tv_nsec;
}

void UNIVERSE_MASK_1(float ** matrix, unsigned long int scale) {
	unsigned long int x,y;
	unsigned long int halfScale = scale >> 1;
	long  double radius;
	for(x=0;x<scale;x++) {
		for(y=0;y<scale;y++) {
			radius = sqrtl( ((y-halfScale) * (y-halfScale)) + ((x-halfScale) * (x-halfScale)));
			if ( radius < halfScale ) {
				matrix[x][y] = halfScale - radius;
			}
			else {
				matrix[x][y] = 0;
			}
		}
	}
}

void UNIVERSE_NOISE_1( float * matrix, float ** mask, unsigned long int scale, unsigned long int offsetX, unsigned long int offsetY, unsigned long int realScale) {
	if (scale == 1) {
		return;
	}
	unsigned long int halfScale = scale >> 1;
	unsigned long int x, y;
	long int randX = - halfScale + getRandom() % scale;
	long int randY = - halfScale + getRandom() % scale;
	int inc = realScale / scale;
	unsigned long int maskX=0, maskY=0;
	char doIt = getRandom() & 1;
	if (doIt) {
	for(x=0;x<scale;x++) {
		maskY = 0;
			for(y=0;y<scale;y++) {
				if ( randX + offsetX + x < realScale && randX + offsetX +x >= 0 && randY + offsetY + y < realScale && randY + offsetY + y >= 0) {
					matrix[ (x+randX+offsetX ) * realScale + y + randY+offsetY ] += mask[maskX][maskY] / (inc);
				}
				maskY+=inc;
			}
			maskX+=inc;
		}
	}
	UNIVERSE_NOISE_1(matrix, mask, scale/2, offsetX+0, offsetY+0, realScale);
	UNIVERSE_NOISE_1(matrix, mask, scale/2, offsetX+0, offsetY+scale/2, realScale);
	UNIVERSE_NOISE_1(matrix, mask, scale/2, offsetX+scale/2, offsetY+scale/2, realScale);
	UNIVERSE_NOISE_1(matrix, mask, scale/2, offsetX+scale/2, offsetY+0, realScale);
}
