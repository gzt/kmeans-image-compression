#include "kmeans.h"
#include "replaceR.h"
#include "mt19937ar.h"


double kmeans_random(double **x, unsigned int n, unsigned int p, unsigned int k, double **centers)
{
/* 
       x = data matrix (n rows, p columns)
       centers = final randomly allocated centers
       returns the within-sums-of-squares for the initial value
 */

	unsigned int i, j, *iclass, *nclass, *y;
	int ifault;
	double *wss, sum = 0;

	MAKE_VECTOR(y, k);
	MAKE_VECTOR(wss, k);
	MAKE_VECTOR(nclass, k);
	MAKE_VECTOR(iclass, n);

	do {
		srswor(n, k, y);
		
		for (i = 0; i < k; i++) {
			for (j = 0; j < p; j++) centers[i][j] = x[y[i]][j];
		}
		kmeans(x, n, p, centers, k, iclass, nclass, 0, wss, &ifault);
		/* checking if the center is even valid to start with */
	}
	while (ifault == 1);
		
	for (i = 0; i < k; i++)  sum += wss[i];

	
	
	FREE_VECTOR(y);
	FREE_VECTOR(wss);
	FREE_VECTOR(iclass);
	FREE_VECTOR(nclass);

	return sum;
}


void kmeans_random_best(double **x, unsigned int n, unsigned int p, unsigned int k, 
			double **centers, int numbest)
{
	/* input same as above, but chooses the best (in terms of lowest WSS
	   of numbest random initialization points */

	int i, j;
	double **means, w, nuw;

	MAKE_MATRIX(means, k, p);

	w = kmeans_random(x, n, p, k, centers);
	
	for (i = 0; i < numbest; i++) {
		nuw = kmeans_random(x, n, p, k, means);
		if (nuw < w) {
			w = nuw;
			for (i = 0; i < k; i++) {
				for (j = 0; j < p; j++) {
					centers[i][j] = means[i][j];
				}
			}
		}
	}
	printf("Start WSS = %f\n",w);
	FREE_MATRIX(means);
	return;
}
