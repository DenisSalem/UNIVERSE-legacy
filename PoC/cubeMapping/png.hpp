#ifndef __PNG__
#define __PNG__

#include <png.h>

typedef struct _PIXEL
{
     unsigned char Red;      
     unsigned char Green;    
     unsigned char Blue;
     unsigned char Alpha;
} PIXEL;

int writePng( PIXEL ** matrix, int width, int height, const char * filename); 

#endif
