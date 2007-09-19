#include "mathstd.h"

/*--------------------------*
  Funktion
  Matrix transponiert mit Matrix
  multiplizieren.

 *--------------------------*/

matrix 
M_A_T_A (matrix B, matrix A, int n, int m)
{
  int i, j, k;
  double summe;

  for (i = 0; i < m; i++)
    for (j = i; j < m; j++) {
      summe = 0;
      k = n;
      while (k--)
	summe += A[k][i] * A[k][j];
      B[i][j] = B[j][i] = summe;
    }
  return B;
}
