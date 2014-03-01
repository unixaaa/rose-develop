#   define NI ARRAYSIZE
#   define NJ ARRAYSIZE
# define _PB_NI ARRAYSIZE
# define _PB_NJ ARRAYSIZE
/**
 * gramschmidt.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

int main(int argc,char **argv)
{
/* Retrieve problem size. */
  int ni = 16;
  int nj = 16;
/* Variable declaration/allocation. */
  double A[16][16];
  double R[16][16];
  double Q[16][16];
  int i;
  int j;
  int k;
  double nrm;
  
#pragma scop
  for (k = 0; k < 16; k++) {{
      int c1;
      nrm = 0;
      for (c1 = 0; c1 <= 15; c1++) {
        nrm += A[c1][k] * A[c1][k];
      }
    }
    R[k][k] = sqrt(nrm);
{
      int c1;
      int c3;
      for (c1 = 0; c1 <= 15; c1++) {
        Q[c1][k] = A[c1][k] / R[k][k];
      }
      for (c1 = k + 1; c1 <= 15; c1++) {
        R[k][c1] = 0;
        for (c3 = 0; c3 <= 15; c3++) {
          R[k][c1] += Q[c3][k] * A[c3][c1];
        }
        for (c3 = 0; c3 <= 15; c3++) {
          A[c3][c1] = A[c3][c1] - Q[c3][k] * R[k][c1];
        }
      }
    }
  }
  
#pragma endscop
  return 0;
}
