#include <iostream>
#include <cstdlib>
#include <time.h>
#include "png.cpp"

unsigned long int getRandom() {
        timespec tStruct;
	clock_gettime(CLOCK_REALTIME, &tStruct);
	return tStruct.tv_nsec % 255;
}

void UNIVERSE_NOISE_1(unsigned long int ** matrix, unsigned long int scale) {
	unsigned long int x,y; // x,y :p
	int j=1; // right shit on scale (divide by two)
	unsigned long int i; (getRandom() % (scale >> j)); // gradient with phase
	unsigned long int peak; //gradient max
	char increase;
	while (scale >> j >= 1) {
		peak = (scale >> j) + (getRandom() % (scale >> j));
		increase=1;
		i= (getRandom() % (scale >> j));
		for(y=0;y<scale;y++) {
			if(peak == i) {
				increase = 0;
			}
			if (y==0 || increase == 0 && i == 0) {
				peak = (scale >> j) + (getRandom() % (scale >> j));
				increase = 1;
			}
			for(x=0;x<scale;x++) {
				matrix[x][y] += i;
			}
			if (increase == 1) {
				i++;
			}
			else {
				i--;
			}
		}
		j++;
	}
	j = 1;
	while (scale >> j >= 1) {
		peak = (scale >> j) + (getRandom() % (scale >> j));
		increase=1;
		i= (getRandom() % (scale >> j));
		for(x=0;x<scale;x++) {
			if(peak == i) {
				increase = 0;
			}
			if (x==0 || increase == 0 && i == 0) {
				peak = (scale >> j) + (getRandom() % (scale >> j));
				increase = 1;
			}
			for(y=0;y<scale;y++) {
				matrix[x][y] += i;
			}
			if (increase == 1) {
				i++;
			}
			else {
				i--;
			}
		}
		j++;
	}
}


int main(int argc, char ** argv) {
        unsigned long int scale = atoi(argv[1]);
        unsigned long int ** matrix = (unsigned long int **) malloc(sizeof(unsigned long int *) * scale);
        PIXEL ** png = (PIXEL **) malloc(sizeof(PIXEL *) * scale);
        unsigned long int i,j;
        for (i=0; i<scale;i++) {
                matrix[i] = (unsigned long int *) malloc(sizeof(unsigned long int) * scale);
                png[i] = (PIXEL *) malloc(sizeof(PIXEL) * scale);
                for(j=0;j<scale;j++) {
                        matrix[i][j] = 0;

                }
        }

	UNIVERSE_NOISE_1(matrix, scale);
        // Adjust color Levels
        int max,min = 65536;
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
			color = ((matrix[i][j] - min) * 255) / (max-min);
                        png[i][j].Alpha = 0xFF;
                        png[i][j].Red = color;
                        png[i][j].Green = color;
                        png[i][j].Blue = color;
                }
        }
        writePng(png,scale);

	return 0;
}
