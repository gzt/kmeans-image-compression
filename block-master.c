#include<stdio.h>

#include<stdlib.h>
#include <string.h>     /* strtok() */
#include <unistd.h>     /* getopt() */
#include <getopt.h>
#include "array.h"
#include "kmeans.h"
#include "util.h"

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
  double **x, **trimx, **medianmatrix,**blockmatrix, **means, *w, wss = 0;
  unsigned char **y;
  	unsigned int n; //512*512 for baboon, for stars 6703*7795; now calc from height,width
	unsigned int height, width; //512 512 for baboon, now read from file
	unsigned int p, i, j, *iclass, *nc, k;
	unsigned int rowtruncation, coltruncation, nlength,numblocks;
	int method = 1;
	char *filename;
	int ifault;
	extern char   *optarg;
        extern int optind;
	int filenameset = 0;
	int block;
       	int input;
	int opt;
	int is_print_usage = 0;
	int readinfile = 0;
	unsigned int filenumber = 0;
	unsigned int seed1, seed2;
	//	unsigned char *binmedianmatrix;
	float *binmedianmatrix;

	unsigned char *newclass;
	unsigned short *shortclass;
	float *binmeans;
	FILE *finp, *fout;
	double lmult = 0.,Lfactor = 0.0;
	unsigned int rounds = 5;

	finp = fopen("random.seed", "r");
	fscanf(finp, "%u %u", &seed1, &seed2);
	fclose(finp);
		
	set_seed(seed1, seed2);


    while ( (opt=getopt(argc,argv,"f:n:K:H:B:W:R:l:L:si:h"))!= EOF) {
        switch (opt) {
	    case 'f': {
	              filename=optarg;
	              filenameset = 1;
	              break;
	    }
	    case 'n': filenumber = atoi(optarg);
	              break;
	    case 's': readinfile = 1; /* this will read in set.kp and override anything */
                      break;			      
            case 'K': k = atoi(optarg);
                      break;
            case 'B': block = atoi(optarg);
                      break;
            case 'H': height = atoi(optarg);
                      break;
            case 'W': width = atoi(optarg);
                      break;
	    case 'i': method = atoi(optarg);
                      break;		      
            /* case 'h': is_print_usage = 1; */
            /*           break; */
	    case 'l': lmult=atof(optarg);
	              break;
	    case 'L': Lfactor=atof(optarg);
	              break;
	    case 'R': rounds = (unsigned int) atoi(optarg); 
	               break; 
	    default:  is_print_usage = 1;
                      break;
        }
    }

    if(filenameset == 0) filename = "rtiffoutput.dat";

	
	// reading in the parameters
	if( argc < 2 || readinfile == 1){
	  finp = fopen("set.kp", "r");
	  fscanf(finp, "%u %d %u %u %u", &k, &block, &height, &width,&filenumber);
	  
	  fclose(finp);
	}
	
	  if(filenumber==1){
	    n = height * width;


	    MAKE_MATRIX(x, n, 3);

	    finp = fopen(filename, "r");
	    // use when output is in float format
	    for (i = 0; i < n; i++) {
	      for (j = 0; j < 3; j++) fscanf(finp, "%lf ", &x[i][j]);
	    }
	    fclose(finp);
	  } else if (filenumber==2){
	    width = 6703;
	    height = 7795;
	    n = height * width;
	    MAKE_MATRIX(x, n, 3);
	    finp = fopen("sh2-174.dat", "r");
	    for (i = 0; i < n; i++) {
	      for (j = 0; j < 3; j++) fscanf(finp, "%lf ", &x[i][j]);
	    }
	    fclose(finp);

	  } else if (filenumber == 3){
	    width = 4795;
	    height = 4941;
	    n = height * width;
	    MAKE_MATRIX(x, n, 3);

	    finp = fopen("sh2-290.dat", "r");
	    for (i = 0; i < n; i++) {
	      //for (j = 0; j < 3; j++) fscanf(finp, "%lf ", &x[i][j]);
	      for (j = 0; j < 3; j++) {
		fscanf(finp, "%d ", &input);
		x[i][j] = input/255.0;
	      }
	    }
	    fclose(finp);

	    
	  } else if (filenumber == 4) {
	    width = 6000;
	    height = 5325;
	    n = height * width;
	    MAKE_MATRIX(x, n, 3);
	    finp = fopen("veil.dat", "r");
	    for (i = 0; i < n; i++) {
	      //for (j = 0; j < 3; j++) fscanf(finp, "%lf ", &x[i][j]);
	      for (j = 0; j < 3; j++) {
		fscanf(finp, "%d ", &input);
		x[i][j] = input/255.0;
	      }
	    }
	    fclose(finp);
	    
	  }	else  if (filenumber == 5) {
	    n = height * width;
	    
	    MAKE_MATRIX(x, n, 3);

	    finp = fopen(filename, "r");
	    // use when output is in integer format
	    for (i = 0; i < n; i++) {
	      for (j = 0; j < 3; j++) {
		fscanf(finp, "%d ", &input);
		x[i][j] = input/255.0;
	      }		
	    }
	    fclose(finp);
	  }	else {
	    n = height * width;
	    if(filenameset == 0) filename = "rtiffoutput.dat";
	    
	    MAKE_MATRIX(x, n, 3);
	    finp = fopen(filename, "r");
	    for (i = 0; i < n; i++) {
	      for (j = 0; j < 3; j++) fscanf(finp, "%lf ", &x[i][j]);
	    }
	    fclose(finp);
	  }

	printf("Done reading the data\n");

	// calculations needed for later

	p=3*block*block;
	rowtruncation = (height/block) * block;
	coltruncation = (width/block) * block;
	nlength = rowtruncation * coltruncation;
	numblocks = (height/block)*(width/block);

	// trimming the image based on the block size

	MAKE_MATRIX(trimx,nlength ,3);
	
	triminput(trimx,x,height,width,block);

	FREE_MATRIX(x);
	MAKE_MATRIX(blockmatrix,numblocks,p);
	MAKE_MATRIX(medianmatrix,numblocks,3);

	// this makes the block matrix, which breaks the image up into the blocks
	// and subtracts out the medians, which are stored in medianmatrix

	prepareblockmatrix(trimx,rowtruncation,coltruncation,block,blockmatrix,medianmatrix);

	// move the following to the bottom unless you're going to directly calculate SSE
	FREE_MATRIX(trimx);
	// debugging output:

	/* printf("first entries of blockmatrix:\n\n"); */
	/* printf("%f %f \n %f %f\n\n",blockmatrix[0][0],blockmatrix[0][1],blockmatrix[0][block],blockmatrix[0][block+1]); */

	



	MAKE_MATRIX(means, k, p);


	if ( method == 1) {
	  if(Lfactor < .1 && lmult < 1 ) Lfactor = 2.0;
	  if(rounds < 1) rounds = 5;
	  if(lmult < 1 || lmult < Lfactor * k) lmult = Lfactor*k;
	  if(lmult*rounds < 1.25*k) lmult = 1.25*k/rounds;
	  scalablekmeanspp(blockmatrix,numblocks, p, k, rounds, lmult, means);
	} else if (method == 2)   {
	  kmeanspp_best(blockmatrix, numblocks, p, k, means, rounds, 0);
	} else {
	  if(rounds < 6) rounds = 200;
	  kmeans_random_best(blockmatrix, numblocks, p, k, means, rounds);
	}
	printf("Done with the first random start\n");

	MAKE_VECTOR(iclass, numblocks);
	MAKE_VECTOR(nc, k);
	MAKE_VECTOR(w, k);

	kmeans(blockmatrix, numblocks, p, means, k, iclass, nc, 1000, w, &ifault); 

	printf("Done with k-means\n");
	
	printf("ifault = %d\n", ifault);
	


	for (i = 0; i < k; i++) wss += w[i];

	printf("within sums of squares = %f \n", wss);
	printf("numblocks = %u block = %d  k = %u \n", numblocks, block, k);


	// writing to the log the results
	fout = fopen("logresults.txt","a+");
	fprintf(fout, "%u %d %f %u\n", k, block, wss, filenumber);
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
	
	kmeanspp_best(medianmatrix, numblocks, 3, 256, newmeans, 1, 0);

	kmeans(medianmatrix, numblocks, 3, newmeans, 256, newiclass, newnc, 100, neww, &ifault); 

	printf("means clusters \n");
	wss = 0;
	for (i = 0; i < 256; i++) wss += neww[i];
	printf("Means wss: %f\n", wss);
	double **predmeans;
	MAKE_MATRIX(predmeans,numblocks,3);
	for(i = 0; i < numblocks; ++i)
	  for(j = 0; j < 3; ++j)
	    predmeans[i][j] = newmeans[newiclass[i]][j];

 	
	/* MAKE_VECTOR(binmedianmatrix,numblocks*3); */
	/* for(i = 0; i < numblocks ; i++){ */
	/*   for(j=0; j <3 ; j++){ */
	/*     // perhaps this should be done column-major order... */
	/*     binmedianmatrix[ 3*i + j ] = ftc(medianmatrix[i][j]); */
	/*   } */
	/* } */
	/* // outputting the results if they are wanted */

	/* fout = fopen("block-median.out", "wb"); */
	/* /\* for (i = 0; i < numblocks; i++) { *\/ */
	/* /\* 	for (j = 0; j < 3; j++) fprintf(fout, "%f ", medianmatrix[i][j]); *\/ */
	/* /\* 	fprintf(fout, "\n"); *\/ */
	/* /\* } *\/ */
	/* fwrite(binmedianmatrix,sizeof(binmedianmatrix[0]),(numblocks * 3),fout); */
	/* fclose(fout); */

 	
	MAKE_VECTOR(binmedianmatrix,256*3);
	for(i = 0; i < 256 ; i++){
	  for(j=0; j <3 ; j++){
	    // perhaps this should be done column-major order...
	    binmedianmatrix[ 3*i + j ] = (float) (newmeans[i][j]);
	  }
	}
	//outputting the results if they are wanted

	fout = fopen("block-mean-means.out", "wb");
	/* for (i = 0; i < numblocks; i++) { */
	/* 	for (j = 0; j < 3; j++) fprintf(fout, "%f ", medianmatrix[i][j]); */
	/* 	fprintf(fout, "\n"); */
	/* } */
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
	/* for (i = 0; i < numblocks; i++) { */
	/*   fprintf(fout, "%u ", iclass[i]); */
	/* 	fprintf(fout, "\n"); */
	/* } */

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

	fout = fopen("block-pixel.out", "w");
	for (i = 0; i < nlength; i++) {
		for (j = 0; j < 3; j++) fprintf(fout, "%u ", y[i][j]);
		fprintf(fout, "\n");
	}
	fclose(fout);

	FREE_VECTOR(binmedianmatrix);

	FREE_MATRIX(newmeans);
	FREE_VECTOR(neww);
	FREE_VECTOR(newnc);
        FREE_VECTOR(newiclass);
	FREE_MATRIX(predmeans);

	// freeing all the memory
	//	FREE_MATRIX(trimx);
	FREE_MATRIX(blockmatrix);
	FREE_MATRIX(y);
	FREE_VECTOR(w);
	FREE_VECTOR(nc);
	FREE_VECTOR(iclass);
	FREE_MATRIX(means);
	FREE_MATRIX(medianmatrix);



/*
	get_seed(&seed1,&seed2);

	fout=fopen("random.seed","w");
	fprintf(fout,"%d %d\n",seed1,seed2);
	fclose(fout); 
*/
	return EXIT_SUCCESS;
}









