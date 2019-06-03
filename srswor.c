#include "array.h"
#include "replaceR.h"
#include "mt19937ar.h"


/* Note that use of this function involves a prior call to the Rmath library to
   get the seeds in place. It is assumed that this is done in the calling 
   function. */

/* Equal probability sampling; without-replacement case */
/* Adapted from the R functions called SampleNoReplace, ProbSampleReplace,
   ProbSampleReplace*/
/* SRS subset program written by Geoffrey Thompson 11/14/13
 gzt@iastate.edu*/


int srswor(unsigned int n, unsigned int k, unsigned int *y)
{
	/* Provide k out of n indices sampled at random without replacement */
	
	if (k > n) {
		printf("Error: k greater than n in srswor()");
		return 1;
	}
	else {
		
		int i, j;
		int *x;
		
		MAKE_VECTOR(x, n);
		for (i = 0; i < n; i++)	x[i] = i;
		
		for (i = 0; i < k; i++) {
		        j = randint(n);
			y[i] = x[j];
			x[j] = x[--n];
		}
		FREE_VECTOR(x);
	}
	return 0;
}
