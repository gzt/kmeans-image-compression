# kmeans-image-compression
Image compression using the block k-means algorithm

Run `make all` and make sure that `kmeans-compress.sh` is marked as executable.

This uses the files from [`replaceR`](https://www.github.com/gzt/replaceR), 
though it can be rewritten to use `Rmath.h` easily if you prefer that. 
It depends on having `libtiff` and `zpaq` installed. 

On Fedora, I recommend running `dnf install libtiff libtiff-devel zpaq zpaq-devel`, 
as this depends on having `libtiff` and `libzpaq`. Appropriate packages are available 
in Ubuntu as well.

Basic usage is:

`./kmeans-compress.sh -i inputimage.tiff -o outputimage.tiff -p blocksize -k numberofclusters`

For example, on the included test image, you may try:

`./kmeans-compress.sh -i baboon.tiff -o baboon-test.tiff -p 5 -k 50`

There are additional options controlling other settings (method of initialization of clustering,
assigning seeds, etc). It outputs a TIFF version of the compressed image and and a file
representing the compressed version of the input image. The program also writes some diagnostic
information to `logresults.txt`. This works by passing your arguments to `block-tiff` and then 
compressing the output using the `zpaq` library. The zpaq-compressed output is saved as 
`block-output.kmns`. The uncompressed binary files are saved as `.out` files. A text file,
`block-means-text.out`, of the first-stage means is provided as well for diagnostic purposes.
There is another program called `block-master` which takes in a text file representation of an 
image which was used for some of the images in the corresponding paper. This is included
for the sake of reproducibility. 

```
Usage: ./block-tiff [-v] -i input-TIFF-file 
 -o output-TIFF-file 
 -k number-clusters 
 [-n] number of tries 
 -p block size 
 [-m] initialization method (1 = kmeans||, 2 = kmeans++, 3 = kmeans random starts) 
 [-r] initialization rounds 
 [-l] for kmeans|| centers per round 
 [-L] for kmeans|| multiplication factor (k*L = l), only specify one of L and l 
 [-s] seed file for PRNG (takes two seeds) 
 [-S] seed file for PRNG, plus hash of arguments to seed 
 [-t] uses time(NULL) and getpid() as a seed 
  Note: -v indicates debugging option, default for -n = 1000, default for -m = 1
 default is for L=2 and r=5 for kmeans||
 default for kmeans++ is -r 25, default for kmeans random starts is r 1000 
 default is to hash arguments as a seed 
```
