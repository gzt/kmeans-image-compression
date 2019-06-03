CC =	gcc

CFLAGS = -std=c99 -Wall -pedantic -Ofast

OBJ =	replaceR.o mt19937ar.o kmeans.o srswor.o kmeans_randoms.o blockmeansutils.o

OBJ4 =	replaceR.o mt19937ar.o kmeans.o blockmeansutils.o kmeans++.o kmeanspp_best.o

OBJMASTER = replaceR.o mt19937ar.o kmeans.o srswor.o kmeans_randoms.o blockmeansutils.o kmeans++.o kmeanspp_best.o wkmeans.o scalablekmeans++.o

OBJTIFF  = replaceR.o mt19937ar.o kmeans.o srswor.o kmeans_randoms.o blockmeansutils.o kmeans++.o kmeanspp_best.o tiff2rgb.o tiff2rgbread.o write_tiff.o kmeanspp_best.o wkmeans.o scalablekmeans++.o


LIBS =  -lm 

all:	 block-master block-tiff

block-master: block-master.c $(OBJMASTER)
	$(CC) -o $@ $^ $(CFLAGS) -lm 
block-tiff: block-tiff.c $(OBJTIFF)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -ltiff
kmeans.o:	kmeans.c
	$(CC) -c $(CFLAGS) $^
srswor.o:	srswor.c
	$(CC) -c $(CFLAGS) $^
kmeans_randoms.o:	kmeans_randoms.c
	$(CC) -c $(CFLAGS) $^
blockmeansutils.o:	blockmeansutils.c
	$(CC) -c $(CFLAGS) $^
kmeans++.o:	kmeans++.c
	$(CC) -c $(CFLAGS) $^
kmeanspp_best.o:	kmeanspp_best.c
	$(CC) -c $(CFLAGS) $^
wkmeans.o:		wkmeans.c
	$(CC) -c $(CFLAGS) $^ 
tiff2rgb.o:	tiff2rgb.c
	$(CC) -c $(CFLAGS) $^
tiff2rgbread.o:	tiff2rgbread.c
	$(CC) -c $(CFLAGS) $^
write_tiff.o:	write_tiff.c
	$(CC) -c $(CFLAGS) $^
replaceR.o: 	replaceR.c
	$(CC) -c $(CFLAGS) $^
mt19937ar.o:	mt19937ar.c
	$(CC) -c $(CFLAGS) $^



clean:	
	rm *~   block-master block-tiff  *.o
