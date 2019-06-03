# kmeans-image-compression
Image compression using the block k-means algorithm

This uses the files from [`replaceR`](https://www.github.com/gzt/replaceR), 
though it can be rewritten to use `Rmath.h` easily if you prefer that. 
It depends on having `libtiff` installed. 

On Fedora, I recommend running `dnf install libtiff libtiff-devel zpaq zpaq-devel`, 
as this depends on having `libtiff` and `libzpaq`. Appropriate packages are available 
in Ubuntu as well.

Basic usage is:

`block-tiff -i inputimage.tiff -o outputimage.tiff -p blocksize -k numberofclusters`

There are additional options controlling other settings (method of initialization of clustering,
assigning seeds, etc). It outputs a TIFF version of the compressed image and and a set of files
representing the compressed version of the input image. The program also writes some diagnostic
information to `logresults.txt`. There is another program called `block-master` which takes in 
a text file representation of an image which was used for some of the images in the corresponding
paper. 

