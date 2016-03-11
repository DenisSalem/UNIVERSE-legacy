#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include "png.cpp"

typedef struct heighmap {
	float * mask;
	int scale;
	float max;
	float min;
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

	char seed = getRandom() & 3;
	char doIt = getRandom() & 1;
	float sign = getRandom() & 2 ? 1.0 : -1.0;
	int halfScale = scale >> 1;
	int randX = - halfScale + getRandom() % (scale);
	int randY = - halfScale + getRandom() % (scale);
	int inc = heightmap->scale / scale;
	int maskIndex;
	int faceIndex;
	int maskX=0,maskY=0;
	

	float * dest;

	// Required to compute the difficult case
	float phiOrigin;
	float phiDest;
	int xDest, yDest;
	int xOrigin, yOrigin;
	if (doIt || 1) {
		// When mask fit exactly within face
		if ((randY+offsetY >= 0 && randY+offsetY+scale-1 < heightmap->scale) && (randX+offsetX >= 0 && randX+offsetX+scale-1< heightmap->scale)) {
			for(int y=0; y<scale;y++) {
				maskX = 0;
				for(int x=0; x<scale;x++) {
					maskIndex = maskX + scale * maskY;
					if( heightmap->mask[maskIndex] != 0) {
						faceIndex = (y+randY+offsetY) * heightmap->scale + x+randX+offsetX;
						heightmap->faces[faceId][ faceIndex] += sign * heightmap->mask[maskIndex] / inc;
						if (heightmap->max < heightmap->faces[faceId][ faceIndex]) {
							heightmap->max = heightmap->faces[faceId][ faceIndex ];
						}
						if (heightmap->min <= 0 && heightmap->min > heightmap->faces[faceId][ faceIndex ]) {
							heightmap->min = heightmap->faces[faceId][ faceIndex ];
						}
					}
					maskX += inc;
				}
				maskY += (inc * inc);
			}
		}
		/*
		// When it both overflow on Y and X axis
		else if (false && !(randX+offsetX >= 0 && randX+offsetX+scale-1 < heightmap->scale) && !( randY+offsetY >= 0 && randY+offsetY+scale-1 < heightmap->scale  )) {
			// On Top Left
			if (randX+offsetX < 0 && randY+offsetY < 0 ) {
				xOrigin = randX+offsetX+halfScale;
				yOrigin = randY+offsetY+halfScale;
				if (xOrigin != 0 || yOrigin != 0) {
					phiOrigin = acos( (float) (xOrigin) / sqrt( pow(xOrigin,2) + pow(yOrigin,2)));
					std::cout << scale << " "<< xOrigin << " " << yOrigin << " " << phiOrigin << "\n";
				}
				else { // easy case where mask is centered on boundary intersection. There is no need to compute relative origin.
					maskY = halfScale * inc * inc;
					for(int y=0; y<halfScale;y++) {
						maskX = halfScale * inc;
						for(int x=0; x<halfScale;x++) {
							maskIndex = maskX + scale * maskY;
							//faceIndex = x + y * heightmap->scale;
							if ( heightmap->mask[maskIndex] != 0){
								heightmap->faces[faceId][faceIndex] += heightmap->mask[maskIndex];
								
								if (heightmap->max < heightmap->faces[faceId][ faceIndex]) {
									heightmap->max = heightmap->faces[faceId][ faceIndex ];
								}
								if (heightmap->min <= 0 && heightmap->min > heightmap->faces[faceId][ faceIndex ]) {
									heightmap->min = heightmap->faces[faceId][ faceIndex ];
								}
							}
							maskX += inc;
						}
						maskY += (inc*inc);
					}
				}
			}		
		}*/
		// When it overflow on the X axis
		else if ( !(randX+offsetX >= 0 && randX+offsetX+scale-1 < heightmap->scale) && ( randY+offsetY >= 0 && randY+offsetY+scale-1 < heightmap->scale  )) {
			for(int y=0; y<scale;y++) {
				maskX = 0;
				for(int x=0; x<scale;x++) {
					maskIndex = maskX + scale * maskY;
					if( heightmap->mask[maskIndex] != 0) {
						if (randX+offsetX+x < 0) {
							dest = heightmap->left;
							if (faceId == 1) {
								faceIndex = ( heightmap->scale - (y+randY+offsetY) ) * heightmap->scale - (x+randX+offsetX)-1;
							}
							else if (faceId == 2) {
								faceIndex = -(x+randX+offsetX + 1) * heightmap->scale +(y+randY+offsetY);
							}
							else if (faceId == 3) {
								faceIndex = ( heightmap->scale + x+randX+offsetX + 1) * heightmap->scale + heightmap->scale - (y+randY+offsetY);
							}
							else if (faceId == 4) {
								faceIndex = (y+randY+offsetY) * heightmap->scale + heightmap->scale + (x+randX+offsetX);
							}
							else if (faceId == 5) {
								faceIndex = ( heightmap->scale - (y+randY+offsetY)) * heightmap->scale - (x+randX+offsetX) - 1;
							}
							else {
								faceIndex = (y+randY+offsetY) * heightmap->scale + heightmap->scale + (x+randX+offsetX);
							}
						}
						else if ( randX+offsetX+x >= heightmap->scale ) {
							dest = heightmap->right;
							if (faceId == 1) {
								faceIndex = (heightmap->scale - (y+randY+offsetY)) * heightmap->scale + heightmap->scale - ((x+randX+offsetX) - heightmap->scale) - 1;
							}
							else if (faceId == 2) {
								faceIndex = ((x+randX+offsetX) - heightmap->scale)* heightmap->scale + heightmap->scale - (y+randY+offsetY);
							}
							else if (faceId == 3) {
								faceIndex = ( heightmap->scale - ((x+randX+offsetX) - heightmap->scale) - 1)* heightmap->scale + (y+randY+offsetY);
							}
							else if (faceId == 4) {
								faceIndex = ( heightmap->scale - (y+randY+offsetY) ) * heightmap->scale + heightmap->scale - ((x+randX+offsetX) - heightmap->scale) - 1;
							}
							else if (faceId == 5) {
								faceIndex = (y+randY+offsetY) * heightmap->scale + (x+randX+offsetX) - heightmap->scale;
							}
							else {
								faceIndex = (y+randY+offsetY) * heightmap->scale + x+randX+offsetX - heightmap->scale;
							}
						}
						else {
							dest = heightmap->faces[faceId];
							faceIndex = ((y+randY+offsetY)) * heightmap->scale + x+randX+offsetX;
						}
						dest[ faceIndex] += sign * heightmap->mask[maskIndex] / inc ;
						if (heightmap->max < dest[ faceIndex]) {
							heightmap->max = dest[ faceIndex ];
						}
						if (heightmap->min <= 0 && heightmap->min > dest[ faceIndex ]) {
							heightmap->min = dest[ faceIndex ];
						}
					}
					maskX += inc;
				}
				maskY += inc * inc;
			}
		}
		// When it overflow on the Y axis
		else if ((randX+offsetX >= 0 && randX+offsetX+scale-1 < heightmap->scale) && !( randY+offsetY >= 0 && randY+offsetY+scale-1 < heightmap->scale  )) {
			for(int y=0; y<scale;y++) {
				maskX = 0;
				for(int x=0; x<scale;x++) {
					maskIndex = maskX + scale * maskY;
					if( heightmap->mask[maskIndex] != 0) {
						if (randY+offsetY+y < 0) {
							dest = heightmap->top;
							if ( faceId == 4) {
								faceIndex = ( heightmap->scale + (y+randY+offsetY)) + heightmap->scale * ( heightmap->scale - (x+randX+offsetX));
							}
							else if ( faceId == 5) {
								faceIndex = (x+randX+offsetX) * heightmap->scale - 1 - y-randY-offsetY;
							}
							else {
								faceIndex = (heightmap->scale + (y+randY+offsetY)) * heightmap->scale + x+randX+offsetX;
							}
						}
						else if (randY+offsetY+y >= heightmap->scale) {
							dest = heightmap->bottom;
							if ( faceId == 4 ) {
								faceIndex = - (y+randY+offsetY) - 1 + (heightmap->scale * ( scale - x+randX+offsetX) );
							}
							else if(faceId == 5) {
								faceIndex = (y+randY+offsetY - heightmap->scale) + heightmap->scale * (heightmap->scale -x-randX-offsetX);
							}
							else {
								faceIndex = (y+randY+offsetY - heightmap->scale) * heightmap->scale + x+randX+offsetX;
							}
						}
						else {
							dest = heightmap->faces[faceId];
							faceIndex = ((y+randY+offsetY)) * heightmap->scale + x+randX+offsetX;
						}
						dest[ faceIndex] += sign * heightmap->mask[maskIndex] / inc ;
						if (heightmap->max < dest[ faceIndex]) {
							heightmap->max = dest[ faceIndex ];
						}
						if (heightmap->min <= 0 && heightmap->min > dest[ faceIndex ]) {
							heightmap->min = dest[ faceIndex ];
						}
					}
					maskX += inc;
				}
				maskY += (inc * inc);
			}
			
		}
		else if( (randX+offsetX < 0 || randX+offsetX+scale >= heightmap->scale) && (randX+offsetX < 0 || randX+offsetX+scale >= heightmap->scale) ) {

		}

	}

	UNIVERSE_NOISE_1(heightmap, halfScale, offsetX, 		offsetY,		faceId);
	UNIVERSE_NOISE_1(heightmap, halfScale, offsetX, 		offsetY+halfScale,	faceId);
	UNIVERSE_NOISE_1(heightmap, halfScale, offsetX+halfScale, 	offsetY+halfScale,	faceId);
	UNIVERSE_NOISE_1(heightmap, halfScale, offsetX+halfScale, 	offsetY,		faceId);
}

int main(int argc, char ** argv) {
        if (argc == 1) {
          std::cout << "Usage: ./heighMap <power of two>\n";
          return 0;
        }
	HEIGHTMAP heightmap;
	heightmap.max = 0;
	heightmap.min = 0;
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

	heightmap.top = heightmap.faces[2];
	heightmap.bottom = heightmap.faces[3];
	heightmap.left = heightmap.faces[0];
	heightmap.right = heightmap.faces[1];
	UNIVERSE_NOISE_1(&heightmap, heightmap.scale, 0, 0, 4);

	heightmap.top = heightmap.faces[2];
	heightmap.bottom = heightmap.faces[3];
	heightmap.left = heightmap.faces[1];
	heightmap.right = heightmap.faces[0];
	UNIVERSE_NOISE_1(&heightmap, heightmap.scale, 0, 0, 5);

	int scale4 = heightmap.scale * 4;
	int scale3 = heightmap.scale * 3;
	int scale2 = heightmap.scale * 2;
	unsigned char color;

	// Beyond this remains the assembly of each faces in the png
	for(int y = 0; y<scale4; y++) {
		for(int x = 0; x<scale3; x++) {
			if (x >= heightmap.scale && x < scale2 && y >= heightmap.scale && y < scale2) { // plusZ
				color = (unsigned char) (255 * (heightmap.faces[0][ (x-heightmap.scale) + heightmap.scale*(y-heightmap.scale)] - heightmap.min) / (heightmap.max - heightmap.min));
				png[x][y].Red	= color;
				png[x][y].Green = color;
				png[x][y].Blue = color;
			}
			else if (x >= heightmap.scale && x < scale2 && y >= 0 && y < heightmap.scale) { // plusY
				color = (unsigned char) (255 * (heightmap.faces[2][ (x-heightmap.scale) + heightmap.scale*y] - heightmap.min) / (heightmap.max - heightmap.min));
				png[x][y].Red	= color;
				png[x][y].Green = color;
				png[x][y].Blue = color;
			}
			else if (x >= heightmap.scale && x < scale2 && y >= scale2 && y < scale3) { // minusY
				color = (unsigned char) (255 * (heightmap.faces[3][ (x-heightmap.scale) + heightmap.scale*(y-scale2)] - heightmap.min) / (heightmap.max- heightmap.min));
				png[x][y].Red	= color;
				png[x][y].Green = color;
				png[x][y].Blue = color;
			}
			else if (x >= heightmap.scale && x < scale2 && y >= scale3 && y < scale4) { // minusZ
				color = (unsigned char) (255 * (heightmap.faces[1][ (x-heightmap.scale) + heightmap.scale*(y-scale3)] - heightmap.min) / (heightmap.max- heightmap.min));
				png[x][y].Red = color; 
				png[x][y].Green	= color;
				png[x][y].Blue = color;
			}
			else if (x >= 0 && x < heightmap.scale && y >= heightmap.scale && y < scale2) { // minuxX
				color = (unsigned char) (255 * (heightmap.faces[5][ x + heightmap.scale*(y-heightmap.scale)] - heightmap.min) / (heightmap.max- heightmap.min));
				png[x][y].Red = color; 
				png[x][y].Green	= color;
				png[x][y].Blue = color;
			}
			else if (x >= scale2 && x < scale3 && y >= heightmap.scale && y < scale2) { // plusX
				color =  (unsigned char) (255 * (heightmap.faces[4][ (x-scale2) + heightmap.scale*(y-heightmap.scale)] - heightmap.min) / (heightmap.max- heightmap.min));
				png[x][y].Red = color; 
				png[x][y].Green	= color;
				png[x][y].Blue = color;
			}
			else {
				png[x][y].Red = 0; 
				png[x][y].Green	= 255;
				png[x][y].Blue = 0;
			}
			png[x][y].Alpha	= 255;
		}

	}

        writePng(png,heightmap.scale);
	
	return 0;
}
