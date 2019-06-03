#ifndef _UTIL_H
#define _UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "constants.h"
#include "kmeans.h"
#include "replaceR.h"
#include "mt19937ar.h"

/**
 * Simple mathematical functions.
 */
#define SQ(x) ((x) * (x)) 
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SQ(x) ((x) * (x))
#define LTINDEX(k, l) ((MAX(k, l) * (MAX(k, l) + 1))/2 + MIN(k, l))
#define LLTINDEX(k, l) (LTINDEX(k - 1, l))
#define INFINITY 1e40

enum {NO_ERROR,
	MEMORY_ALLOCATION_ERROR,	/*!< failure to allocate memory */
	INVALID_COMMAND_LINE,		/*!< user misuse of command line */
	FILE_OPEN_ERROR,		/*!< failure to open file */
	FILE_FORMAT_ERROR,		/*!< unexpected file format */
	FILE_WRITE_ERROR,		/*!< failure to write to file */
	INTERNAL_ERROR,			/*!< any other error */
	NUMBER_OF_ERRORS
};

#define NO_EXIT 0	/* do not let called function exit */
#define DO_EXIT 1	/* induce a called function to exit */

extern const char *error_string[];


/**
 * Convert upper triangle symmetric matrix indices to vector index.
 *
 * @param n size of matrix
 * @param i row index
 * @param j index, must have j>i
 * @return index of same position in row-order vector
 */
#define VINDEX(n, i, j) (                                     \
	(i) < (j)                                             \
		? (j) + (i) * (n) - ((i) + 1) * ((i) + 2) / 2 \
		: (i) + (j) * (n) - ((j) + 1) * ((j) + 2) / 2 \
)


/**
 * Convert vector index to upper triangle indices.
 *
 * @param n size of matrix
 * @param i row index
 * @param j column index
 * @param ij vector index
 */
#define RINDEX(n, i, j, ij) do {                \
	unsigned int NADD = n - 1, NINDEX = 0;           \
	for (i = 0;                             \
		ij >= NINDEX + NADD;             \
		i++, NINDEX += NADD, NADD--);   \
	j = i + ij - NINDEX + 1;                       \
} while (0)

/**
 * Generic matrix object.  When you need to operate on a matrix using generic
 * functions, like random samplers, that use call-backs to check user-required
 * conditions, can pass this object around as a void pointer.  It currently
 * just provides the data and the number of columns.
 */
struct data_obj {
	double **x;	/*!< data matrix */
	unsigned int p;	/*!< number of columns in matrix */
};

typedef double (*object_distance_function)(unsigned int, unsigned int, void *);
typedef double (*distance_function)(double *, double *, unsigned int);

/* distance calculations */
double estimate_average_pairwise_distance(object_distance_function dist, unsigned int n, unsigned int max_iter, double epsilon, void *obj);
double sqdist(double *x, double *y, unsigned int p);
double dist(double *x, double *y, unsigned int p);
double sq_norm(double *, unsigned int);
double l1norm(double *x, double *y, unsigned int p);
double normalized_dist(double *x, double *y, double *omega, unsigned int p);

/* data transformation */
int unstandardize_data(double **x, unsigned int n, unsigned int p,  double *mu, double *sd);
int standardize_data(double **x, unsigned int n, unsigned int p, double **in_mu, double **in_sd, int transform_data, int scaling);

unsigned int minindex(double *x, unsigned int n);
int unique_points(unsigned int *y, unsigned int i, void *d);
int shuffle(unsigned int *array, unsigned int n);

/* k-means-related */
int check_wss(unsigned int, unsigned int, double *, double *, double *, unsigned int *, unsigned int *, double *, unsigned int *, unsigned int *);
void reset_wss(double *w, unsigned int k);
unsigned int null_cluster_downgrade(double **x, double **centers, unsigned int n, unsigned int p, unsigned int k, double *dis, unsigned int *sd_idx, int free);

double elapsed_seconds(struct timeval *x, struct timeval *y);

int error(int errnum, int do_exit, const char *filename, const char *fxn_name, int line_no, const char *message, ...);

char *string_copy(const char *);

/* io-related */
int check_newline(FILE *finp, const char *filename, const char *file, const char *fxn_name, int line_no);
char *make_full_filename(const char *path, const char *name);
const char *remove_path(const char *fullPath);	/* from optlist version 3 */


#endif
