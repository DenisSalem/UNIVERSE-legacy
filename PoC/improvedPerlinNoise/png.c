#include <png.h>
#include <stdlib.h>
#include "png.h"

int writePng( PIXEL ** matrix, int size) { 
        png_structp     png_ptr;
        png_infop       info_ptr;
        png_bytep * row_pointers;
        int x,y;


        /* create file */
        FILE *fp = fopen("terrain.png", "wb");
        if (!fp)
                //abort_("[write_png_file] File %s could not be opened for writing", "decoded.png");
		return -1;


        /* initialize stuff */
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr)
                //abort_("[write_png_file] png_create_write_struct failed");
		return -1;

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
                //abort_("[write_png_file] png_create_info_struct failed");
		return -1;

        if (setjmp(png_jmpbuf(png_ptr)))
                //abort_("[write_png_file] Error during init_io");
		return -1;

        png_init_io(png_ptr, fp);

        /* write header */
        if (setjmp(png_jmpbuf(png_ptr)))
                //abort_("[write_png_file] Error during writing header");
		return -1;

        png_set_IHDR(png_ptr, info_ptr, size, size, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(png_ptr, info_ptr);

    
        /* init buffer */
        row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * size);
        for (y=0; y<size; y++)
                row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
        /* COPY BUFFER */
        if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
                //abort_("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
                  //     "(lacks the alpha channel)");
		return -1;

        if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
                //abort_("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
                  //     PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));
		return -1;

        for (y=0; y<size; y++) {
                png_byte* row = row_pointers[y];
                for (x=0; x<size; x++) {
                        png_byte* ptr = &(row[x*4]);

                        ptr[0] = matrix[x][y].Red;
                        ptr[1] = matrix[x][y].Green;
                        ptr[2] = matrix[x][y].Blue;
                        ptr[3] = 255;
                }
        }

        /* write bytes */
        if (setjmp(png_jmpbuf(png_ptr)))
                //abort_("[write_png_file] Error during writing bytes");
		return -1;

        png_write_image(png_ptr, row_pointers);


        /* end write */
        if (setjmp(png_jmpbuf(png_ptr)))
                //abort_("[write_png_file] Error during end of write");
		return -1;

        png_write_end(png_ptr, NULL);

        /* cleanup heap allocation */
        for (y=0; y<size; y++)
                free(row_pointers[y]);
        free(row_pointers);

        fclose(fp);

}
