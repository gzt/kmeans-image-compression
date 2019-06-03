#include<stdio.h>

#include<stdlib.h>
#include <string.h>     /* strtok() */
#include <unistd.h>     /* getopt(), getpid() */
#include <getopt.h>
#include "array.h"
#include "kmeans.h"
#include "util.h"
#include "tiff2rgb.h"
#include "tiff2rgbread.h"
#include "write_tiff.h"

void kmeans_random_best(double **x, unsigned int n, unsigned int p, unsigned int k, 
			double **centers, unsigned int numbest);
int compare(const void *f1, const void *f2);
double vectormedian(double *vec,int n);
void triminput(double **outputmatrix, double **inputmatrix, unsigned int height, unsigned int width, int blocksize);
void prepareblockmatrix(double **imagematrix, unsigned int height, unsigned int width, int w, double **blockmatrix, double **medianmatrix);
void restoremeans(double **blockmatrix, double **medianmatrix, 
		  double **meanoutput, unsigned int *classoutput, unsigned int numblocks, int w, unsigned int k);
void unrollvectors(unsigned char **x, double **blockmatrix, unsigned int height, unsigned int numblocks, int w);
void bridgeoutput(unsigned char **x, double **blockmatrix, double **medianmatrix, 
		  double **meanoutput, unsigned int *classoutput, unsigned int numblocks, unsigned int height, int w, unsigned int k);
unsigned char ftc(double x);
double ctf(unsigned char x);
void scalablekmeanspp(double **x, unsigned int n, unsigned int p, unsigned int k, unsigned int rounds, double L, double **centers);
void kmeanspp(double **x, unsigned int n, unsigned int p, unsigned int k, double **centers);

double kmeanspp_wss(double **x, unsigned int n, unsigned int p, unsigned int k, double **centers, unsigned int iter);
void kmeanspp_best(double **x, unsigned int n, unsigned int p, unsigned int k, double **centers,unsigned int numbest, unsigned int iter);



int main(int argc, char **argv)
{
  //  double **x;
  double **trimx, **medianmatrix,**blockmatrix, **means, *w, wss = 0;
  unsigned char **y;
  	unsigned int n; //512*512 for baboon, for stars 6703*7795; now calc from height,width
	unsigned int height, width, p; //512 512 for baboon, now read from file
	unsigned int i, j, *iclass, *nc;
	unsigned int rowtruncation, coltruncation, nlength,numblocks;
	unsigned int rounds = 0;
	int method = 2;
	double lmult = 0.,Lfactor = 0.0;
	//char *filename;
	int ifault;
	extern char   *optarg;
        extern int optind;
	//int filenameset = 0;
	int block;

	//unsigned int seed1, seed2;
	//	unsigned char *binmedianmatrix;
	float *binmedianmatrix;

	unsigned char *newclass;
	unsigned short *shortclass;
	float *binmeans;
	FILE *fout;
	hash_init_rand(argc, argv);
	//finp = fopen("random.seed", "r");
	//fscanf(finp, "%u %u", &seed1, &seed2);
	//fclose(finp);
		
	//set_seed(seed1, seed2);
	char *itext, *otxt, *seedfil;
	VecImg tif;
	size_t kinput, niter = 25, blockinput;

	short verbose;
/*	double *d;*/

	verbose = read_options(argc, argv, &itext, &otxt, &kinput, &niter, &blockinput, &rounds, &lmult, &Lfactor, &method, &seedfil);
	block = (int) blockinput;
	unsigned int k = (unsigned int) kinput;
	if (verbose)
		printf (" input file = %s, output file = %s \n", itext, otxt);

	tif = getTiff(itext);
	// switched because of row major vs column major in tiff vs this
	width = (unsigned int) tif.height;
	height = (unsigned int) tif.width;
	n = (unsigned int) tif.n;;
	
	if (verbose)
	  printf ("height: %u \t width: %u n: %u \n", height, width, n);

	printf("Done reading the data \n");

	// calculations needed for later

	p=3*block*block;
	rowtruncation = (height/block) * block;
	coltruncation = (width/block) * block;
	nlength = rowtruncation * coltruncation;
	numblocks = (height/block)*(width/block);

	if (verbose)
	  printf("rows: %u \t cols: %u \t numblocks: %u \n", rowtruncation, coltruncation, numblocks);
	// trimming the image based on the block size

	MAKE_MATRIX(trimx, nlength, 3);
	
	triminput(trimx,tif.x,height,width,block);
	for(i = 0; i < nlength; ++i)
	  for(j = 0; j < 3; ++j)
	    trimx[i][j] = trimx[i][j]/255.0;

	FREE_MATRIX(tif.x);
	MAKE_MATRIX(blockmatrix,numblocks,p);
	MAKE_MATRIX(medianmatrix,numblocks,3);

	// this makes the block matrix, which breaks the image up into the blocks
	// and subtracts out the medians, which are stored in medianmatrix

	prepareblockmatrix(trimx,rowtruncation,coltruncation,block,blockmatrix,medianmatrix);

	// move the following to the bottom unless you're going to directly calculate SSE
	FREE_MATRIX(trimx);
	// debugging output:
	if (verbose) {
	  printf("first entries of blockmatrix:\n\n"); 
	    printf("%f %f \n %f %f\n\n",blockmatrix[0][0],blockmatrix[0][1],blockmatrix[0][block],blockmatrix[0][block+1]); 
	}
	



	MAKE_MATRIX(means, k, p);


	// choose initialization method
	if ( method == 1) {
	  if (verbose) printf("Beginning scalable k-means++ \n");
	  if(Lfactor < .1 && lmult < 1 ) Lfactor = 2.0;
	  if(rounds < 1) rounds = 5;
	  if(lmult < 1 || lmult < Lfactor * k) lmult = Lfactor*k;
	  if(lmult*rounds < 1.25*k) lmult = 1.25*k/rounds;
	  if (verbose) printf("L: %f \t rounds: %u \n", lmult, rounds);
	  scalablekmeanspp(blockmatrix,numblocks, p, k, rounds, lmult, means);
	} else if (method == 2)   {
	  if (verbose) printf("Beginning k-means++ \n");
	  if (rounds < 1) rounds = 5;
	  if (verbose) printf("Rounds: %u \n", rounds);
	  kmeanspp_best(blockmatrix, numblocks, p, k, means, rounds, 0);
	} else {
	  if (verbose) printf("Beginning random starts \n");
	  if (rounds < 1) rounds = 200;
	  if (verbose) printf("Rounds: %u \n", rounds);
	  kmeans_random_best(blockmatrix, numblocks, p, k, means, rounds);
	}
	
	printf("Done with the first random start\n");

	MAKE_VECTOR(iclass, numblocks);
	MAKE_VECTOR(nc, k);
	MAKE_VECTOR(w, k);

	kmeans(blockmatrix, numblocks, p, means, k, iclass, nc, niter, w, &ifault); 

	printf("Done with k-means\n");
	
	printf("ifault = %d\n", ifault);
	


	for (i = 0; i < k; i++) wss += w[i];

	printf("within sums of squares = %f \n", wss);
	printf("numblocks = %u block = %d  k = %u \n", numblocks, block, k);


	// writing to the log the results
	fout = fopen("logresults.txt","a+");
	fprintf(fout, "%u %d %f %s\n", k, block, wss, otxt);
	fclose(fout);

	// second level:

	double **newmeans;
	double *neww;
	unsigned int *newnc;
	MAKE_MATRIX(newmeans,256,3);
	MAKE_VECTOR(neww,256);
	MAKE_VECTOR(newnc,256);
	unsigned int *newiclass;
	MAKE_VECTOR(newiclass,numblocks);
	
	printf("starting means clustering \n");

	
	kmeanspp_best(medianmatrix, numblocks, 3, 256, newmeans, rounds, 1);
	if(verbose)
	  printf("Done with intialization of means step. \n");
	
	kmeans(medianmatrix, numblocks, 3, newmeans, 256, newiclass, newnc, niter, neww, &ifault); 

	printf("means clusters done \n");
	wss = 0;
	printf("ifault = %d\n", ifault);
	for (i = 0; i < 256; i++) wss += neww[i];
	printf("Means wss: %f\n", wss);
	double **predmeans;
	MAKE_MATRIX(predmeans,numblocks,3);
	for(i = 0; i < numblocks; ++i)
	  for(j = 0; j < 3; ++j)
	    predmeans[i][j] = newmeans[newiclass[i]][j];



 	
	MAKE_VECTOR(binmedianmatrix,256*3);
	for(i = 0; i < 256 ; i++){
	  for(j=0; j <3 ; j++){
	    // perhaps this should be done column-major order...
	    binmedianmatrix[ 3*i + j ] = (float) (newmeans[i][j]);
	  }
	}
	//outputting the results if they are wanted

	fout = fopen("block-mean-means.out", "wb");

	fwrite(binmedianmatrix,sizeof(binmedianmatrix[0]),(256 * 3),fout);
	fclose(fout);


	fout = fopen("block-mean-id.out", "wb");
	unsigned char *meanclass;
	MAKE_VECTOR(meanclass, numblocks);
	for(i = 0; i < numblocks ; ++i) meanclass[i] = (unsigned char) newiclass[i];
	  
	fwrite(meanclass,sizeof(meanclass[0]),numblocks,fout);
	fclose(fout);
	FREE_VECTOR(meanclass);




	

	fout = fopen("block-class.out", "wb");


	if ( k <=256 ){
	  MAKE_VECTOR(newclass,numblocks);
	  for(i = 0; i < numblocks ; ++i) newclass[i] = (unsigned char) iclass[i];
	  
	  	fwrite(newclass,sizeof(newclass[0]),numblocks,fout);
		fclose(fout);
		FREE_VECTOR(newclass);
	} else {
	  MAKE_VECTOR(shortclass,numblocks);
	  for(i = 0; i < numblocks ; ++i) shortclass[i] = (unsigned short) iclass[i];
	fwrite(shortclass,sizeof(shortclass[0]),numblocks,fout);
	FREE_VECTOR(shortclass);
	fclose(fout);
	}
	fout = fopen("block-means.out", "wb");
	/* for (i = 0; i < k; i++) { */
	/* 	for (j = 0; j < p; j++) fprintf(fout, "%f ", means[i][j]); */
	/* 	fprintf(fout, "\n"); */
	/* } */
	MAKE_VECTOR(binmeans,(p*k));

	for(i = 0; i < k; ++i){
	  for(j = 0; j<p;++j)  binmeans[p*i + j] = (float) means[i][j]; 
	    }
	fwrite(binmeans,sizeof(binmeans[0]),(k * p),fout);
	fclose(fout);
	FREE_VECTOR(binmeans);  

	fout = fopen("block-means-text.out", "w");
	for (i = 0; i < k; i++) {
		for (j = 0; j < p; j++) fprintf(fout, "%f ", means[i][j]);
		fprintf(fout, "\n");
	}
	fclose(fout);


	
	  
		MAKE_MATRIX(y,nlength ,3);
		// this converts the output of the kmeans into the image again - this could be done in R
		bridgeoutput(y,blockmatrix,predmeans,means,iclass,numblocks,rowtruncation,block,k);
		// free a lot of memory before writing tiff as I need to recopy image again
	FREE_VECTOR(binmedianmatrix);

	FREE_MATRIX(newmeans);
	FREE_VECTOR(neww);
	FREE_VECTOR(newnc);
        FREE_VECTOR(newiclass);
	FREE_MATRIX(predmeans);
	FREE_MATRIX(blockmatrix);
	FREE_VECTOR(iclass);
	FREE_MATRIX(means);
	FREE_MATRIX(medianmatrix);
	FREE_VECTOR(w);
	FREE_VECTOR(nc);

	// prepare to write tiff - this should be a function but whatever
	unsigned char *v;
	if ((v = (unsigned char *)malloc( nlength*3 )) == NULL) {
	  fprintf(stderr, "Can't malloc rgb memory\n");
	  
	FREE_MATRIX(y);
	}
		
	for (i = 0; i < nlength; i++)
		for (j = 0; j < 3; j++)
			v[i*3 + j] = y[i][j];

	write_tiff_img(otxt, v, rowtruncation, coltruncation, 8, 3, 0);

	free(v);

	FREE_MATRIX(y);





/*
	get_seed(&seed1,&seed2);

	fout=fopen("random.seed","w");
	fprintf(fout,"%d %d\n",seed1,seed2);
	fclose(fout); 
*/
	return EXIT_SUCCESS;
}









