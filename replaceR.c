/* 
   A C program meant to be used with the Mersenne Twister code in order to 
   replace reliance on the R standalone library if it is only being used
   to generate from a uniform distribution. It will not be exactly the same
   for the same seeds as the R library, however, it will be reproducible. 

   Run set_seed() with two integers as before, then use runif() as before.

   The mt19937ar.* files are copyright their original owners subject to their
   terms. See those files for details.

   This file is released under the GPL-3 or any later compatible GPL license.

    Copyright (C) 2019 Geoffrey Thompson

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


   Feedback is quite welcome!

   Geoffrey Thompson, 3/2019
   Iowa State University, Dept of Statistics
   gzthompson@gmail.com

 */
#include <stdio.h>
#include "replaceR.h"

void set_seed(unsigned int x, unsigned int y) {
  unsigned long init_key[2] = {(unsigned long) x, (unsigned long) y};
  init_by_array(init_key, 2);
}

double runif(double lower, double upper) {
  double x = genrand_real2();
  return (lower + x * (upper - lower)) ;  
}

uint32_t randint(unsigned int range){
  // code for this function via M O'Neill, Apache License 2.0
  // algorithm from Lemire, https://arxiv.org/abs/1805.10941
  // adjusted by M O'Neill for speed,
  // http://www.pcg-random.org/posts/bounded-rands.html
  // chooses a random integer in [0, range)
    unsigned long x = genrand_int32();
    uint64_t m = (uint64_t) (x) * (uint64_t) (range);
    uint32_t l = (uint32_t) (m);
    if (l < range) {
        uint32_t t = -range;
        if (t >= range) {
            t -= range;
            if (t >= range) 
                t %= range;
        }
        while (l < t) {
            x = genrand_int32();
            m = (uint64_t) (x) * (uint64_t) (range);
            l = (uint32_t) (m);
        }
    }
    return m >> 32;
}


unsigned long hash(char *str, unsigned long hash) {
  // djb2 hash
  // http://www.cse.yorku.ca/~oz/hash.html
  // they init by 5381, here we make able to keep feeding on itself
        int c;

        while (c = *str++)
	  hash = ((hash << 5) + hash) ^ c; /* hash * 33 + c */

        return hash;
    }

void hash_init_rand(int argc, char **argv) {
  // hash the arguments, use as seed
  int i = 0;
  unsigned long hsh = 5381;
  for(i=0;i<argc;i++) {
    hsh = hash(argv[i], hsh);
  }
  init_genrand(hsh);
}

void set_hash_seed(int argc, char **argv, unsigned int x, unsigned int y) {
  // take the hash and add in two more seeds to init by array
  int i = 0;
  unsigned long hsh = 5381;
  for(i=0;i<argc;i++) {
    hsh = hash(argv[i], hsh);
  }
  unsigned long init_key[3] = {hsh, (unsigned long) x, (unsigned long) y};
  init_by_array(init_key, 3);
}
