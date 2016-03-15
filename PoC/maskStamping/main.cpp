#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include "png.cpp"

unsigned long int getRandom() {
        timespec tStruct;
        clock_gettime(CLOCK_REALTIME, &tStruct);
        return tStruct.tv_nsec;
}


void UNIVERSE_MASK_1(long double ** matrix, long double scale) {
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

void UNIVERSE_NOISE_1(long double * matrix, long double ** mask, unsigned long int scale, long int offsetX, long int offsetY, unsigned long int realScale) {
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
	char seed = getRandom() & 3;
	char doIt = seed & 1;
	float sign = seed & 2 ? -1.0 : 1.0;
	if (doIt) {
	for(x=0;x<scale;x++) {
		maskY = 0;
			for(y=0;y<scale;y++) {
				// Easy Case
				if ( randX + offsetX + x < realScale && randX + offsetX +x >= 0 && randY + offsetY + y < realScale && randY + offsetY + y >= 0) {
					matrix[ ((x+randX+offsetX) * realScale) + y+randY+offsetY] += sign * mask[maskX][maskY] / (inc);
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

					matrix[ (  wrapX + x + offsetX  + randX) * realScale + (y+randY+offsetY) + wrapY] += sign * mask[maskX][maskY] / (inc);
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

int main(int argc, char ** argv) {
	if (argc == 1) {
		std::cout << "Usage: ./heightMap <power of two>\n";
		return 0;
	}

	// INIATE SHIT N STUFF
	unsigned long int scale = atoi(argv[1]);
	unsigned long int x,y,i,j;
	long double * matrix;
	long double ** mask;
	matrix	= (long double *) malloc( sizeof( long double ) * scale * scale * 12);
	mask 	= (long double **) malloc( sizeof( long double *) * scale );
        PIXEL ** png = (PIXEL **) malloc(sizeof(PIXEL *) * scale * 3);

	for (x=0; x<scale;x++) {
		mask[x] = (long double *) malloc( sizeof(long  double) * scale );
	}
	for (x=0; x<scale;x++) {
		for(y=0;y<scale;y++){
			matrix[x*scale*3+y] = 0;
		}
		png[x] = (PIXEL *) malloc(sizeof(PIXEL) * scale);
	}

	UNIVERSE_MASK_1(mask, (long double ) scale);
	UNIVERSE_NOISE_1(matrix, mask, scale, 0, 0, scale);
        // Adjust color Levels
        long double max=0,min = 65536;
        for (x=0; x<scale;x++) {
                for(y=0;y<scale;y++) {
                        if (matrix[x*scale+y] > max) {
                                max = matrix[x*scale+y];
                        }
                        if (matrix[x*scale+y] < min) {
                                min = matrix[x*scale+y];
                        }
                }
        }

        char color;
        for (x=0; x<scale;x++) {
                for(y=0;y<scale;y++) {
                        color = (((matrix[x*scale+y]) - min ) * 255) / (max - min);
                        png[x][y].Alpha = 0xFF;
                        png[x][y].Red = color;
                        png[x][y].Green = color;
                        png[x][y].Blue = color;
                }
        }
        writePng(png,scale);
	return 0;
}
