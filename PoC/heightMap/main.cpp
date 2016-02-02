#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include "png.cpp"

// g++ demo0x02.cpp -lpng -lm; ./a.out 512

unsigned long int getRandom() {
        timespec tStruct;
        clock_gettime(CLOCK_REALTIME, &tStruct);
        return tStruct.tv_nsec;
}


void UNIVERSE_MASK_1(long double ** matrix, unsigned long int scale) {
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

void UNIVERSE_NOISE_1(long double ** matrix, long double ** mask, unsigned long int scale, unsigned long int offsetX, unsigned long int offsetY, unsigned long int realScale) {
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
					matrix[x+randX+offsetX][y+randY+offsetY] += mask[maskX][maskY] / (inc);
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
	long  double ** matrix;
	long double ** mask;
	matrix	= (long double **) malloc( sizeof( long double *) * scale );
	mask 	= (long double **) malloc( sizeof( long double *) * scale );
        PIXEL ** png = (PIXEL **) malloc(sizeof(PIXEL *) * scale);

	for (x=0; x<scale;x++) {
		matrix[x] = (long double *) malloc( sizeof(long double) * scale );
		for(y=0;y<scale;y++){
			matrix[x][y] = 0;
		}
		mask[x] = (long double *) malloc( sizeof(long  double) * scale );
		png[x] = (PIXEL *) malloc(sizeof(PIXEL) * scale);
	}

	UNIVERSE_MASK_1(mask, scale);
	UNIVERSE_NOISE_1(matrix, mask, scale, 0, 0, scale);
        // Adjust color Levels
        long double max=0,min = 65536;
        for (i=0; i<scale;i++) {
                for(j=0;j<scale;j++) {
                        if (matrix[i][j] > max) {
                                max = matrix[i][j];
                        }
                        if (matrix[i][j] < min) {
                                min = matrix[i][j];
                        }
                }
        }

        char color;
        for (i=0; i<scale;i++) {
                for(j=0;j<scale;j++) {
                        color = ((matrix[i][j]) * 255) / (max);
                        png[i][j].Alpha = 0xFF;
                        png[i][j].Red = color;
                        png[i][j].Green = color;
                        png[i][j].Blue = color;
                }
        }
        writePng(png,scale);
	
	return 0;
}
