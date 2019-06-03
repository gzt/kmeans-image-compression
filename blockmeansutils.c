#include<stdio.h>

#include<stdlib.h>
#include<math.h>
#include "array.h"
#include "kmeans.h"
void kmeans_random_best_subset(double **x, unsigned int *l, unsigned int *sub,  unsigned int n, unsigned int p, unsigned int k, unsigned int ksub,
				 double **c, int numb);
int srswor_subset(unsigned int n, unsigned int k, unsigned int *y, unsigned int *z);




int compare(const void *f1, const void *f2);
double vectormedian(double *vec,int n);
double vectormean(double *vec,int n);
double vecpercentile(double *vec, int n, double p);
void triminput(double **outputmatrix, double **inputmatrix, unsigned int height, unsigned int width, int blocksize);
void prepareblockmatrix(double **imagematrix, unsigned int height, unsigned int width, int w, double **blockmatrix, double **medianmatrix);
void restoremeans(double **blockmatrix, double **medianmatrix, double **meanoutput, unsigned int *classoutput, unsigned int numblocks, int w, unsigned int k);
void unrollvectors(unsigned char **x, double **blockmatrix, unsigned int height, unsigned int numblocks, int w);
void bridgeoutput(unsigned char **x, double **blockmatrix, double **medianmatrix, double **meanoutput, unsigned int *classoutput, unsigned int numblocks, unsigned int height, int w, unsigned int k);
unsigned int krza(double *vec, int n, int p);
//unsigned int optimk(unsigned int *label, unsigned int *subset, double **x, unsigned int n, unsigned int p, unsigned int k, double **means, unsigned int *iclass, unsigned int max);
int update_subset( unsigned int *labeled, unsigned int *sample, unsigned int numblocks, unsigned int k);
unsigned char ftc(double x);
double ctf(unsigned char x);
void bridgesplitoutput(unsigned char **x, double **blockmatrix, double **medianmatrix, double **meanoutput, unsigned int *classoutput, unsigned int numblocks, unsigned int height, int w, unsigned int k);
void restoresplitmeans(double **blockmatrix, double **medianmatrix, double **meanoutput, unsigned int *classoutput, unsigned int numblocks, int w, unsigned int k);

int compare(const void *f1, const void *f2){ 
  return ( *(double*)f1 > *(double*)f2 ) ? 1 : -1; 
/* courtesy of http://ndevilla.free.fr/median/median/index.html */
}

double vectormedian(double *vec,int n){  
  double *tempvector;
  int i;
      /* this returns the lower median if n even  */
  MAKE_VECTOR(tempvector,n);
  for(i=0;i<n;++i)
    {
      tempvector[i] = vec[i];
    }
  qsort(tempvector, n, sizeof(double), compare);
  return tempvector[n/2];
  FREE_VECTOR(tempvector);
}

double vectormean(double *vec, int n){
int i;
double temp=0;
for(i =0; i<n;++i){
temp += vec[i] / n;
}
return temp;
}

double vecpercentile(double *vec, int n, double p){
 double *tempvector;
  int i;
  int j = p * n;
  MAKE_VECTOR(tempvector,n);
  for(i=0;i<n;++i)
    {
      tempvector[i] = vec[i];
    }
  qsort(tempvector, n, sizeof(double), compare);
  return tempvector[j];
  FREE_VECTOR(tempvector);
}




/* double **imagematrix; // height*width x 3 */
/* int w = 3; */
/* int k = 100; */
/* int height; */
/* int width; */
/* double **medianmatrix; // numblocks x 3 */
/* double **blockmatrix; // numblocks x 3*w*w */

void triminput(double **outputmatrix, double **inputmatrix, unsigned int height, unsigned int width, int blocksize)
{
  int i,j, component;
  unsigned int  rowtruncation, coltruncation , lastrow;
  

  rowtruncation = (height/blocksize) * blocksize ;
  coltruncation = (width/blocksize) * blocksize ;
  lastrow = rowtruncation * coltruncation;
  j=0;
  i=0;
  while(j<lastrow)
    {
      if ( (i % height) < rowtruncation)
	{
	  for(component=0;component<3;++component)
	    {
	      outputmatrix[j][component] = inputmatrix[i][component];
	    }
	  ++j;
	}
      ++i;
    }
}



void prepareblockmatrix(double **imagematrix, unsigned int height, unsigned int width, int w, double **blockmatrix, double **medianmatrix)
{
  // imagematrix is the input received from R, w is the block size, 
  // blockmatrix and medianmatrix are the matrices to hold result
  unsigned int wheight = height/w;
  unsigned int wwidth = width/w;
  unsigned int numblocks = wheight * wwidth;
  int block=0;
  int i,j;
  int rownumber;
  double **tempmatrix; // w*w x 3
  int component;
  double *tempvector; //  w*w
  MAKE_MATRIX(tempmatrix,w*w,3);
  MAKE_VECTOR(tempvector,w*w);
  
  /* convert pixel list to median matrix, block matrix */
  for(block = 0; block < numblocks; ++block)
  {
    for(i=0;i<w;++i)
      {
	for(j=0;j<w;++j)
	  {
	    rownumber= ((block/wheight)*w + i)*height + (block % wheight) * w + j; 
	    // pixel rownumber forrow j, column i in block number "block"
	    for(component=0;component<3;++component)
	      {
		tempmatrix[w*i+j][component] = imagematrix[rownumber][component];
		// making a temporary matrix for our block
	      }     
	  }
      }
    for(component=0;component<3;++component)
      {
	for(i=0;i<w*w;++i)
	  {
	    tempvector[i]=tempmatrix[i][component];
	    
	  }
	medianmatrix[block][component]=vectormean(tempvector, w*w);
	for(i=0;i<w*w;++i)
	  {
	    blockmatrix[block][w*w*component + i] = tempvector[i] - medianmatrix[block][component];
	  }
      }
  }
 FREE_VECTOR(tempvector);
 FREE_MATRIX(tempmatrix);
 
}

/* cluster list + median matrix + cluster IDs -> block matrix + median matrix -> pixel list  */


void restoremeans(double **blockmatrix, double **medianmatrix, double **meanoutput, unsigned int *classoutput, unsigned int numblocks, int w, unsigned int k)
{
  int i,j, tempcounter;
  int tempclass;
  // does mean label go from 0 to k-1 or does it go from 1 to k? i think 0 to k-1
  for(i=0;i<numblocks;++i)
    {
      tempclass=classoutput[i];
      if (tempclass<k)
	{
	  for(j=0;j<(w*w*3);++j)
	    {
	      tempcounter = (int) (j/(w*w));
	      blockmatrix[i][j] = meanoutput[tempclass][j] + medianmatrix[i][tempcounter];
	    }
	} else 
	{
	 for(j=0;j<(w*w*3);++j)
	    {
	      tempcounter = (int) (j/(w*w));
	      blockmatrix[i][j] = meanoutput[0][j] + medianmatrix[i][tempcounter];
	    }
	}
      
    }
}

void unrollvectors(unsigned char **x, double **blockmatrix, unsigned int height, unsigned int numblocks, int w)
{
  int block,i,j,component,componentcheck,rownumber;
  unsigned int wheight = height/w;
  
  // assert x has sufficient number of rows eg rownumber doesn't overflow
  for(block=0;block<numblocks;++block)
    {
      for(component=0;component<3;++component)
	{
	  componentcheck =  w*w*component;
	  for(i=0;i<w;++i)
	    {
	      for(j=0;j<w;++j)
		{
		  rownumber= ((block/wheight)*w +i)*height + (block % wheight) * w + j;
		  		  
		  x[rownumber][component] = ftc(blockmatrix[block][((w*i+j) + componentcheck)]);
		}
	      
	      
	    }
	}
    }
}

void bridgeoutput(unsigned char **x, double **blockmatrix, double **medianmatrix, double **meanoutput, unsigned int *classoutput, unsigned int numblocks, unsigned int height, int w, unsigned int k)
{
  // the idea is to take the stuff spit out by kmeans and turn it into something to spit back into R.
  restoremeans(blockmatrix,  medianmatrix,  meanoutput,  classoutput,  numblocks,  w, k);
  unrollvectors(x,blockmatrix,height,numblocks,w);
  
}

unsigned int krza(double *vec, int n, int p){
  double *KL, temp, counter;
  unsigned int i, k, stopk=2;
  unsigned int result;
  /* eventually, SS bottoms out and starts fluctuating - want to stop before there */
  
  do {
    stopk++;
  } while (vec[stopk-1] >= 0.98 * vec[stopk] && stopk < n);
  MAKE_VECTOR(KL,n);
  KL[0]=0;KL[n-1]=0;
  for(i=1;i<n-1;++i){
    k = i+1;
    temp = pow(k,(2/p)) * vec[k] - pow(k+1, (2/p)) * vec[k+1];
    if(temp==0) KL[i] = 0;
    else{
      KL[i] = abs((pow(k-1,(2/p)) * vec[k-1] - pow(k,(2/p)) * vec[k])/temp     );
    }
  }
    result = 0;counter = KL[0];
    for(i =1; i<stopk;++i){
      if (KL[i]>counter){counter = KL[i]; result = i;}
    }
  
  result = result+1;
  FREE_VECTOR(KL);
  return result;
}

/* unsigned int optimk(unsigned int *label, unsigned int *subset, double **x, unsigned int n, unsigned int p, unsigned int k, double **means, unsigned int *iclass, unsigned int max){ */
/*   /\* idea: know k means already, want to sample, find optimal K* for new set*\/ */
/*   double *wss, *w, **meansub; */
/*   unsigned int i,j,l; */
/*   int ifault; */
/*   unsigned int result; */
/*   unsigned int *nc; */
/*   MAKE_VECTOR(wss, max+1); */
/*   MAKE_MATRIX(meansub, k+max+1,p); */
/*   for(i=1;i<max;++i){ */
/*     for(j = 0; j<k;++j){ */
/*       for(l = 0;l<p;++l) meansub[j][l] = means[j][l];       */
/*     } */
/*     printf("optim: %d ",i); */
/*     j = i+1; */
/*     MAKE_VECTOR(nc,k+j); */
/*     MAKE_VECTOR(w,k+j); */
/*     do{ */
/*     kmeans_random_best_subset(x,label,subset, n, p,  j,k, meansub,500); //not much */
/*     samplekmeans(label, subset,x, n, p, meansub, k+j, iclass, nc, 500, w, &ifault);} while (ifault == 1); */

/*     printf(" wss calculated "); */
/* 	wss[i]=0; */
/* 	for (l = 0; l < j+k;++l){ */
/* 	wss[i] += w[l]; */
/* 	} */
/* 	printf(" wss = %f ifault = %d \n",wss[i], ifault); */
/* 	FREE_VECTOR(nc); */
/* 	FREE_VECTOR(w); */
/*   } */
/*   /\* for(i = 0; i < 3; ++i) wss[i] = 0; *\/ // constraining to add 4+ clusters */
/*   result = krza(wss,max,p) + 2; */
/*   FREE_VECTOR(wss); */
/*   FREE_MATRIX(meansub); */
/*   return result; */
/* } */


unsigned char ftc(double q){
  unsigned char tmp;
  if (q > 1.0) q = 1.0;
  if (q < 0.0) q = 0.0;
  tmp = (unsigned char) (255 * q + 0.5) ;
  return tmp;

}

double ctf(unsigned char x){
  double tmp;
  tmp = x/255.0;
  return tmp;
}


void bridgesplitoutput(unsigned char **x, double **blockmatrix, double **medianmatrix, double **meanoutput, unsigned int *classoutput, unsigned int numblocks, unsigned int height, int w, unsigned int k)
{
  // the idea is to take the stuff spit out by kmeans and turn it into something to spit back into R.
  
  restoresplitmeans(blockmatrix,  medianmatrix,  meanoutput,  classoutput,  numblocks,  w, k);
  unrollvectors(x,blockmatrix,height,numblocks,w);
  
}

void restoresplitmeans(double **blockmatrix, double **medianmatrix, double **meanoutput, unsigned int *classoutput, unsigned int numblocks, int w, unsigned int k)
{
  int i,j, tempcounter;
  int tempclass=0;
  // does mean label go from 0 to k-1 or does it go from 1 to k? i think 0 to k-1
  for(i=0;i<numblocks;++i)
    {
     	  for(j=0;j<(w*w*3);++j)
	    {
	      tempcounter = (int) (j/(w*w)); 
	      tempclass = classoutput[i + tempcounter * numblocks];
	      blockmatrix[i][j] = meanoutput[tempclass][j] + medianmatrix[i][tempcounter];
	    }
    }
}
