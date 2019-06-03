/* 

   A C program meant to be used with the Mersenne Twister code in order to 
   replace reliance on the R standalone library if it is only being used
   to generate from a uniform distribution. It will not be exactly the same
   for the same seeds as the R library, however, it will be reproducible. 

   Run set_seed() with two integers as before, then use runif() as before.

   The mt19937ar.* files are copyright their original owners subject to their
   terms. See those files for details.

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
#ifndef REPLACE_R
#define REPLACE_R
#include <stdint.h>
#include "mt19937ar.h"

void set_seed(unsigned int x, unsigned int y);

double runif(double lower, double upper);

unsigned long hash(char *str, unsigned long hash);

void hash_init_rand(int argc, char **argv);

void set_hash_seed(int argc, char **argv, unsigned int x, unsigned int y);

uint32_t randint(unsigned int range);
#endif
