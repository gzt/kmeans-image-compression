#include "array.h"
#include "kmeans.h"

void kmeanspp(double **x, unsigned int n, unsigned int p, unsigned int k, double **centers);

double kmeanspp_wss(double **x, unsigned int n, unsigned int p, unsigned int k, double **centers, unsigned int iter)
{
  
	unsigned int  i, *iclass, *nclass;
	int ifault;
	double *wss, sum = 0;
  
	MAKE_VECTOR(wss, k);
	MAKE_VECTOR(nclass, k);
	MAKE_VECTOR(iclass, n);
    
	do{
		kmeanspp(x, n, p, k, centers);
  
		kmeans(x, n, p, centers, k, iclass, nclass, iter, wss, &ifault);
		/* checking if the center is even valid to start with */
	}while(ifault==1);
  
	for (i = 0; i < k; i++)  sum += wss[i];
  
	FREE_VECTOR(wss);
	FREE_VECTOR(iclass);
	FREE_VECTOR(nclass);
  
	return sum;
  
}
void kmeanspp_best(double **x, unsigned int n, unsigned int p, unsigned int k, double **centers,unsigned int numbest, unsigned int iter)
{
	unsigned int i, j, l;
	double **means, w, nuw;
  
	MAKE_MATRIX(means, k, p);
  
	w = kmeanspp_wss(x, n, p, k, centers, iter);
  
	for (l = 1; l < numbest; l++) {

		nuw = kmeanspp_wss(x, n, p, k, means, iter);
		if (nuw < w) {
			w = nuw;
			for (i = 0; i < k; i++) {
				for (j = 0; j < p; j++) {
					centers[i][j] = means[i][j];
				}
			}
		}
	}
	printf("Start wss = %f\n",w);
	FREE_MATRIX(means);
	return;
}


