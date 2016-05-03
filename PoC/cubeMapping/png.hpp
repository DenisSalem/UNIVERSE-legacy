#include <png.h>

typedef struct _PIXEL
{
     unsigned char Red;      
     unsigned char Green;    
     unsigned char Blue;
     unsigned char Alpha;
} PIXEL;

int writePng( PIXEL ** matrix, int size); 
