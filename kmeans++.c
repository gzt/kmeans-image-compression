/**
   @file: kmeans++.c
   
   Perform kmeans++ algorithm.

   Reference: Arthur, David, and Sergi Vassilvitskii. "K-means++: The Advantages of Careful Seeding." SODA ‘07: Proceedings of the Eighteenth Annual ACM-SIAM Symposium on Discrete Algorithms. 2007, pp. 1027–1035.

   Author:

   Israel Almodovar-Rivera and Ranjan Maitra
   Iowa State University
   Department of Statistics
   almodova@iastate.edu

   Copyright 2015-2016

   edited by Geoffrey Thompson to reduce memory and number of loops 12/30/16
   Iowa State University
   Department of Statistics
   gzt@iastate.edu

   edited by Geoffrey Thompson to switch to stochastic acceptance sampling
   and to use an unbiased randint method 03/13/19

**/

#include "array.h"
#include "util.h"

// weighted sampling by stochastic acceptance
unsigned int WSample(unsigned int m, double *prob){
  double max = 0.0;
  for (int i = 0; i < m; ++i){
    if (prob[i] > max) max = prob[i];
  }
  // could do below but faster "inline"
  // return WMaxSample(m. prob, max);
  while(1){
    unsigned int result = (unsigned int) randint(m);
    if (runif(0.,1.) < prob[result]/max) return result;
  }
}

unsigned int WMaxSample(unsigned int m, double *prob, double max){
  // if you specify the max in advance - DOES NOT CHECK MAX IS BIGGEST
  // can use weights instead of probabilities
    while(1){
    unsigned int result = (unsigned int) randint(m);
    if (runif(0.,1.) < prob[result]/max) return result;
  }
}


// outdated
unsigned int PPSsample(unsigned int m, double *prob)
{
  double uni = runif(0., 1.);
  // double *cumprob;
  double cumprob;
  unsigned int i;
  
  cumprob = 0.;
  for ( i = 0; ( (i < m) && ((cumprob += prob[i]) <= uni) ) ; i++);
  return i;
}

void kmeanspp(double **x, unsigned int n, unsigned int p, unsigned int k, double **centers){
  unsigned int i, j, l, y, *z,  ans , chosen = 0;
  double distMin, *D, Dmax = 0.0 ;
  
  if(k == 1){
    /* If k = 1, just choose any center with Probability =1/n */
    for(i = 0; i < k; i++)
      for(j = 0; j < p; j++)
	centers[i][j] = 0.;
    y = (unsigned int) randint(n);
    for (i = 0;i < p; i++)
      centers[0][i] = x[y][i];
  } else {
    for(i = 0; i < k; i++)
      for(j = 0; j < p; j++)
	centers[i][j] = 0.;
    MAKE_VECTOR(z, k);
    MAKE_VECTOR(D, n);
    
    /* choose first center at random with Prob = 1/n */
    y = (unsigned int) randint(n);
    for(i = 0; i < p; i++)
      centers[0][i] = x[y][i];
    
    z[0] = y; /* store index of observation chosen */
    /* Set distance to INFINITY to find minimum */
    for(i = 0; i < n; i++)
      D[i] = INFINITY;  
    
    for(l = 0; l < (k-1); l++) {
      Dmax = 0.;
      chosen = l + 1;
      
      for(i = 0; i < n; i++){
	/* collapsing a few loops into one loop! */
	distMin = 0.;
	for(j = 0; j < p; j++){
	  distMin += SQ(x[i][j]-centers[l][j]);
	  if (distMin > D[i]) j = p; /*go to the end of the loop*/
	  
	}
	if(distMin < D[i])
	  D[i] = distMin;
	if (Dmax < D[i])
	  Dmax = D[i];
	/* Compute probability note is define as, Prob = ||x_i - bmu||^2/\sum ||x_i -\bmu ||^2 */
      }
      /* Set those already chosen Distance = 0. since we don't want to repeat centers */
      for(i = 0; i < l; i++)
	D[z[i]] = 0.0;
      /* This *should* guarantee Prob = 0 for those points - they should already be zero */
      
      /* Choose one center with probability defined from above */
      /* this may be inefficient as the Dmax may be a selected point but it shouldn't be */
      ans = WMaxSample(n, D, Dmax);
      z[chosen] = ans;
      D[ans] = 0.;
      for(j = 0; j < p; j++)
	centers[chosen][j] = x[ans][j];
      
    }
    FREE_VECTOR(z);
    FREE_VECTOR(D);
  }
}


