#include "tiff2rgb.h"
#include "util.h"
#include <getopt.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>

void usage(char *s)
{
	fprintf(stderr, "Usage: %s [-v] -i input-TIFF-file \n -o output-TIFF-file \n -k number-clusters \n [-n] number of tries \n", s);
	fprintf(stderr, " -p block size \n [-m] initialization method (1 = kmeans||, 2 = kmeans++, 3 = kmeans random starts) \n [-r] initialization rounds \n [-l] for kmeans|| centers per round \n");
	fprintf(stderr, " [-L] for kmeans|| multiplication factor (k*L = l), only specify one of L and l \n [-s] seed file for PRNG (takes two seeds) \n [-S] seed file for PRNG, plus hash of arguments to seed \n [-t] uses time(NULL) and getpid() as a seed \n ");
	fprintf(stderr, " Note: -v indicates debugging option, default for -n = 1000, default for -m = 1\n default is for L=2 and r=5 for kmeans||\n");
	fprintf(stderr, " default for kmeans++ is -r 25, default for kmeans random starts is r 1000 \n default is to hash arguments as a seed \n");
}


VecImg getTiff(const char *itext)
{
	VecImg v;
	size_t i, j;
	TiffImg tifrgb;
	
	printf ("input file = %s", itext);
	tifrgb = get_tiff2rgb(itext);

	if (tifrgb.depth != 3) {
		fprintf(stderr, "Not a RGB image\n");
		exit(1);
	}
	
	v.n = tifrgb.width * tifrgb.height;
	v.width = tifrgb.width;
	v.height = tifrgb.height;
	
	MAKE_MATRIX(v.x, v.n, 3);
	for (i = 0; i < v.n; i++)
		for (j = 0; j < 3; j++)
			v.x[i][j] = tifrgb.rgb[j][i];
	for (i = 0; i < 3; i++)
		_TIFFfree(tifrgb.rgb[i]);
	return v;
}


unsigned char* get_charstar(TiffImg tifrgb)
{
        unsigned char *v;
	size_t i, j;

	if (tifrgb.depth != 3) {
		fprintf(stderr, "Not a RGB image\n");
		exit(1);
	}
	
        int sliceBytes8 = tifrgb.width * tifrgb.height;
	if ((v = (unsigned char *)malloc( sliceBytes8*3 )) == NULL) {
	  fprintf(stderr, "Can't malloc rgb memory\n");
	  for (i = 0; i < 3; i++)
	    _TIFFfree(tifrgb.rgb[i]);
	  exit(1);
	}
		
	for (i = 0; i < sliceBytes8; i++)
		for (j = 0; j < 3; j++)
			v[i*3 + j] = tifrgb.rgb[j][i];

	return v;
}
					     


short read_options(int agc, char **agv, char **infil, char **oufil, size_t *k, size_t *niter, size_t *p, unsigned int *rounds, double *lmult, double *Lfactor, int *method, char **seedfil) {
        char c;
        short verbose = 0;
	unsigned int seed1, seed2;
        *niter = 1000;
        *lmult = 0.,*Lfactor = 0.0;
        *rounds = 0;
        *method = 2;
	FILE *finp;

        while ((c = getopt(agc, agv, "vi:o:k:p:n:r:l:L:m:s:S:t")) != EOF) {
                switch (c) {
                case 'v':
                        verbose = 1;
                        break;
                case 'i':
                        *infil = optarg;
                        break;
                case 'o':
                        *oufil = optarg;
                        break;
                case 'k':
                        *k = atoi(optarg);
                        break;
                case 'p':
                        *p = atoi(optarg);
                        break;
                case 'n':
                        *niter = atoi(optarg);
                        break;
                case 'r':
                        *rounds = (unsigned int) atoi(optarg);
                        break;
                case 'l':
                        *lmult=atof(optarg);
                        break;
                case 'L':
                        *Lfactor=atof(optarg);
                        break;
                case 'm':
                        *method = atoi(optarg);
                        break;
		case 's':
		        *seedfil = optarg;
			finp = fopen(*seedfil, "r");
			fscanf(finp, "%u %u", &seed1, &seed2);
			fclose(finp);
			set_seed(seed1, seed2);
			break;
		case 'S':
		        *seedfil = optarg;
			finp = fopen(*seedfil, "r");
			fscanf(finp, "%u %u", &seed1, &seed2);
			fclose(finp);
			set_hash_seed(agc, agv, seed1, seed2);
			break;
		case 't':
		        set_seed(time(NULL), getpid());
			break;
                case '?':
                        if (optopt == 'i' || optopt == 'o' || optopt == 'p' || optopt == 'k' ||
                            optopt == 'n' || optopt == 'r' || optopt == 'l' || optopt == 'L' ||
                            optopt == 'm' || optopt == 's' || optopt == 'S')
                                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                        else
                                if (isprint (optopt))
                                        fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                                else
                                        fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                        usage(agv[0]);
                        exit(1);
                default:
                        fprintf(stderr, "-i -o -k -p needed, -v -n -r -l -L -m -s -S optional\n");
                        usage(agv[0]);
                        exit(1);
                }
        }
        if (!*infil || !*oufil || !*k || !*p) {
                printf("Missing mandatory arguments\n");
                usage(agv[0]);
                exit(1);
        }
        if (verbose) {
                printf ("input file = %s, output file = %s\n", *infil, *oufil);
                printf ("number of blocks = %zu, k = %zu, max iterations = %zu\n", *p, *k, *niter);
                printf ("method = %d, rounds = %u, L = %f, l = %f\n", *method, *rounds, *Lfactor, *lmult);
                
        }
        return verbose;
}
