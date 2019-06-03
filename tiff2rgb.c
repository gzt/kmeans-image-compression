/* 
   tiff2rgb - Convert TIFF images to raw RGB format

   This function converts 8 bit/pixel grayscale and 24 bit/pixel TIFF images to     
   raw/RGB format. It should properly convert the TIFF images in the database 
   back to the grayscale/RGB format originally used in the database.  Image 
   files in raw/RGB format contains only image data.  There
   are no headers, trailers, etc.  Pixels are in the file in a left-right,
   top-bottom sequence.

   The program requires the "libtiff" library from http://www.libtiff.org

   It is based off a program written  by Allan G. Weber, weber@sipi.usc.edu,
   June 9, 2004 which does things a little differently. It is actually quite 
   different function, but I thought I would leave in the acknowledgement. 

   Written Ranjan Maitra, April 26, 2016.

   The function just takes in the character string of the filename and 
   returns a struct of type TiffImg. 
   The struct has several components:

   1. depth (depth of file being read -- currently only 1 (grayscale) or 3 (RGB) image. 
   2. height -- the height of the image
   3. width -- the width of the image
   4. rgb -- pointer (of size n = height x width) to 3-component array of type 
      unsigned char which stores the 3-vector RGB values (only returned nonnull 
      for depth 3 RGB images).
      Thus, we get rgb[j][i] for i = 1,2,...,n, and j = 1,2,3 for the RGB value
      at the ith pixel.
   5. gray -- pointer to type unsigned char stores the grayscale values (0 to
      255) at each pixel  (only returned nonnull for depth 1 images). 

*/

#include "tiff2rgb.h"

TiffImg get_tiff2rgb(const char *infile) {
	TIFF *tif;
	TiffImg img;
	size_t npixels;
	uint32 x, y, z, *raster;
	
	/*open the file and check for whether it exists*/
	if ((tif = TIFFOpen(infile, "r")) == NULL) {
		fprintf(stderr, "Can't open input file \"%s\". Is the file pathname correct? Exiting....\n", infile);
		exit(1);
	}
	
	/* See if this is a color or gray image */
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &img.depth);
	/*Get size of the image */
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &img.height);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &img.width);
	npixels = img.width * img.height;

	if (img.depth == 3) 
		for (x = 0; x < 3; x++) {
			if ((img.rgb[x] = (unsigned char *) malloc(npixels)) == NULL) {
				fprintf(stderr, "Can't malloc rgb memory\n");
				TIFFClose(tif);
				exit(1);
			}
		}
	else {
		if (img.depth == 1) {
			if ((img.gray = (unsigned char *) malloc(npixels)) == NULL) {
				fprintf(stderr, "Can't malloc grayscale memory\n");
				TIFFClose(tif);
				exit(1);
			}
		}
		else {
			fprintf(stderr, "Unrecognized image img.depth = %d\n", img.depth);
			TIFFClose(tif);
			exit(1);
		}
	}
	if ((raster = (uint32*) _TIFFmalloc(npixels * sizeof(*raster))) == NULL) {
		fprintf(stderr, "Unable to allocate memory...Quitting\n");
		TIFFClose(tif);
		exit(1);
	}
	else {	
		if (TIFFReadRGBAImageOriented(tif, img.width, img.height, raster, ORIENTATION_TOPLEFT, 0)) {
			if (img.depth == 3) {
				unsigned char *r = img.rgb[0], *g = img.rgb[1], *b = img.rgb[2];
				uint32 *xx = raster;
				for (y = 0; y < img.height; y++) {
					for (x = 0; x < img.width; x++) {
						z = *xx++;
						*r++ = TIFFGetR(z);
						*g++ = TIFFGetG(z);
						*b++ = TIFFGetB(z);
					}
				}
			}
			else {		/* img.depth must be 1 */
				unsigned char *g = img.gray;
				uint32 *xx = raster;
				for (y = 0; y < img.height; y++) {
					for (x = 0; x < img.width; x++) {
						z = *xx++;
						*g++ = TIFFGetR(z);
					}
				}
			}
		}
		_TIFFfree(raster);
	}
	TIFFClose(tif);
	return img;
}

