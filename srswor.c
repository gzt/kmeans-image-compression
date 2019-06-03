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
			j = n * runif(0, 1);
			y[i] = x[j];
			x[j] = x[--n];
		}
		FREE_VECTOR(x);
	}
	return 0;
}

int srswor_subset(unsigned int n, unsigned int k, unsigned int *y, unsigned int *z)
{
		
  int i, j, count=0;
  int *x, *index;
		
	/* Provide k out of n indices sampled at random without replacement */
	/*   z is a vector of 1 and 0 - sample from 1's              */
  MAKE_VECTOR(index,n+1);
  index[0]=0;
	for (i = 0; i < n; ++i){
		  count += z[i];
		  if(z[i]==1) index[count]=i;

		}
	if (k > n || k > count) {
		printf("Error: k greater than n or count in srswor()");
		return 1;
	}
	/* else if ( k == count) { */
	/*   for(i=0;i<k;++i) y[i] = index[i+1]; */
	/* } */
	else {
		
		MAKE_VECTOR(x, count);
		for (i = 0; i < count; i++)	x[i] = i;
		
		for (i = 0; i < k; i++) {
		  j = (int) count * runif(0, 1);
			y[i] = index[x[j]+1];
			x[j] = x[--count];
		}
		FREE_VECTOR(x);
	}
	FREE_VECTOR(index);
	return 0;
}




int update_subset( unsigned int *labeled, unsigned int *sample, unsigned int numblocks, unsigned int k){
  unsigned int *tmpsubset;
  int status, i;
	MAKE_VECTOR(tmpsubset,k);
	status = srswor_subset(numblocks,k,tmpsubset,labeled);
	  for(i = 0; i<k;++i) sample[tmpsubset[i]] = 1;
	  FREE_VECTOR(tmpsubset);
	  return status;
}

void WRSampleUnequalProb(int n, int k, double *prob, int *smpl)
{
  /*
    Given n and k, provide a random sample with replacement of size n from 
    k classes, each occurring with unequal probabilities, or frequencies.
    Input parameters are as follows:

    n = sample size
    k = numer of class ids
    prob = probability of occurrence of class ids (also can handle frequencies)
    smpl = n-variate vector containing ids (0 through k-1) of the sample

    written by Ranjan Maitra, January 21, 2007.  
  */
	return;
}
