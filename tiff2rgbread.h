#ifndef TIFF2RGBREAD_H
#define TIFF2RGBREAD_H

#include "tiff2rgb.h"
#include "mt19937ar.h"
#include "replaceR.h"
#include <getopt.h>
#include <ctype.h>

void usage(char *s);
VecImg getTiff(const char *itext);
unsigned char* get_charstar(TiffImg img);
short read_options(int agc, char **agv, char **infil, char **oufil, size_t *k, size_t *niter, size_t *p, unsigned int *rounds, double *lmult, double *Lfactor, int *method, char **seedfil);

#endif /*TIFF2RGBREAD_H*/

