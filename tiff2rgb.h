#ifndef TIFF2RGB_H
#define TIFF2RGB_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "tiffio.h"
#include "array.h"

typedef struct tiffimg {
	uint16 depth;
	uint32 height;
	uint32 width;
	unsigned char *rgb[3];
	unsigned char *gray;
} TiffImg;

TiffImg get_tiff2rgb(const char *infile);

typedef struct vecimg {
	size_t n;
	size_t height; /* height and width only used for ready reference to image*/
	size_t width;
	double **x;
} VecImg;

#endif /* TIFF2RGB_H */
