#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include "main.hpp"
unsigned long int getRandom() {
        timespec tStruct;
        clock_gettime(CLOCK_REALTIME, &tStruct);
        return tStruct.tv_nsec;
}


void UNIVERSE_MASK_1(float ** matrix, unsigned long int scale) {
	unsigned long int x,y;
	long double halfScale = scale / 2 ;
	long double radius;
	for(x=0;x<scale;x++) {
		for(y=0;y<scale;y++) {
			radius = sqrtl( ((y-halfScale) * (y-halfScale)) + ((x-halfScale) * (x-halfScale)));
			if ( radius < halfScale ) {
				matrix[x][y] = (halfScale - radius) / (halfScale);
			}
			else {
				matrix[x][y] = 0;
			}
		}
	}
}

void UNIVERSE_NOISE_1(float * matrix, float ** mask, unsigned long int scale, long int offsetX, long int offsetY, unsigned long int realScale) {
	if (scale == 1) {
		return;
	}
	long int halfScale = scale >> 1;
	long int x, y;
	long int randX = - halfScale + getRandom() % scale;
	long int randY = - halfScale + getRandom() % scale;
	int inc = realScale / scale;
	unsigned long int maskX=0, maskY=0;
	long int wrapX,wrapY;
	char doIt = getRandom() & 1;
	if (doIt) {
	for(x=0;x<scale;x++) {
		maskY = 0;
			for(y=0;y<scale;y++) {
				// Easy Case
				if ( randX + offsetX + x < realScale && randX + offsetX +x >= 0 && randY + offsetY + y < realScale && randY + offsetY + y >= 0) {
					matrix[ ((x+randX+offsetX) * realScale) + y+randY+offsetY] += mask[maskX][maskY] / (inc);
				}
				// Wrap shit n stuff
				else {
					wrapX = 0;
					wrapY = 0;

					if ( randX + offsetX + x >= realScale && randX + offsetX + x < realScale*2 ) {
						wrapX = - realScale;
					}
					else if ( randX + offsetX + x > -realScale && randX + offsetX + x < 0) {
						wrapX = realScale;
					}

					if ( randY + offsetY + y > -realScale && randY + offsetY + y < 0) {
						wrapY = realScale;
					}
					else if ( randY + offsetY + y < realScale * 2 && randY + offsetY + y >= realScale) {
						wrapY = -realScale;
					}

					matrix[ (  wrapX + x + offsetX  + randX) * realScale + (y+randY+offsetY) + wrapY] += mask[maskX][maskY] / (inc);
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

