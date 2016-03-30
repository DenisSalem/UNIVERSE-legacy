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


void UNIVERSE_STAMP_1(double * matrix, int scale) {
	int x,y;
	double halfScale = scale / 2 ;
	double radius;
	for(x=0;x<scale;x++) {
		for(y=0;y<scale;y++) {
			radius = sqrtl( ((y-halfScale) * (y-halfScale)) + ((x-halfScale) * (x-halfScale)));
			if ( radius < halfScale ) {
				matrix[x*scale+y] = (halfScale - radius) / (halfScale);
			}
			else {
				matrix[x*scale+y] = 0;
			}
		}
	}
}

void UNIVERSE_STAMP_NOISE(double * matrix, double * stamp, int scale, int offsetX, int offsetY, int realScale) {
	if (scale == 1) {
		return;
	}
	int halfScale = scale >> 1;
	int x, y;
	int randX = - halfScale + getRandom() % scale;
	int randY = - halfScale + getRandom() % scale;
	int inc = realScale / scale;
	int stampX=0, stampY=0;
	int wrapX,wrapY;
	char seed = getRandom() & 3;
	char doIt = seed & 1;
	float sign = seed & 2 ? -1.0 : 1.0;

        int tmpCoordX,tmpCoordY;
        double currentStampValue;

	for(x=0;x<scale;x++) {
	  stampY = 0;
	    for(y=0;y<scale;y++) {
	      // Easy Case
              currentStampValue = stamp[stampX*realScale+stampY];
              if (currentStampValue != 0) {
                tmpCoordX = randX + offsetX + x;
                tmpCoordY = randY + offsetY + y;
	        if ( tmpCoordX < realScale && tmpCoordX >= 0 && tmpCoordY < realScale && tmpCoordY >= 0) {
	          matrix[ (tmpCoordX * realScale) + tmpCoordY] += sign * currentStampValue / (inc);
	        }
	        // Wrap shit n stuff
	        else {
		  wrapX = 0;
	    	  wrapY = 0;
		  if ( tmpCoordX >= realScale && tmpCoordX < realScale*2 ) {
      		    wrapX = - realScale;
		  }
		  else if ( tmpCoordX > -realScale && tmpCoordX < 0) {
		    wrapX = realScale;
	  	  }
		  if ( tmpCoordY > -realScale && tmpCoordY < 0) {
    		    wrapY = realScale;
		  }
		  else if ( tmpCoordY < realScale * 2 && tmpCoordY >= realScale) {
		    wrapY = -realScale;
		  }
		  matrix[ (  wrapX + tmpCoordX) * realScale + tmpCoordY + wrapY] += sign * currentStampValue / (inc);
      	        }
              }
	      stampY+=inc;
	    }
	  stampX+=inc;
	}
	UNIVERSE_STAMP_NOISE(matrix, stamp, scale/2, offsetX+0, offsetY+0, realScale);
	UNIVERSE_STAMP_NOISE(matrix, stamp, scale/2, offsetX+0, offsetY+scale/2, realScale);
	UNIVERSE_STAMP_NOISE(matrix, stamp, scale/2, offsetX+scale/2, offsetY+scale/2, realScale);
	UNIVERSE_STAMP_NOISE(matrix, stamp, scale/2, offsetX+scale/2, offsetY+0, realScale);
}

int main(int argc, char ** argv) {
	if (argc == 1) {
		std::cout << "Usage: ./heightMap <power of two>\n";
		return 0;
	}

	// INIATE SHIT N STUFF
	int scale = atoi(argv[1]);
	int x,y,i,j;
	double * matrix;
	double * stamp;
	matrix	= (double *) malloc( sizeof( double ) * scale * scale);
	stamp 	= (double *) malloc( sizeof( double ) * scale * scale);
        PIXEL ** png = (PIXEL **) malloc(sizeof(PIXEL *) * scale);

	for (x=0; x<scale;x++) {
		for(y=0;y<scale;y++){
			matrix[x*scale+y] = 0;
		}
		png[x] = (PIXEL *) malloc(sizeof(PIXEL) * scale);
	}

	UNIVERSE_STAMP_1(stamp, scale);
	UNIVERSE_STAMP_NOISE(matrix, stamp, scale, 0, 0, scale);
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
