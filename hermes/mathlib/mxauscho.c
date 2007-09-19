
#include "mathstd.h"


/*--------------------------*
  Funktion :
	Berechnet Ax=b einer
	Cholesky zerlegten
	Matrix A
 *--------------------------*/


vektor 
M_XausCholesky (matrix L, vektor p, int n, vektor x)
{
  int i, k;
  double sum;
  for( i = 0; i < n; i++ ) {
    sum = x[i];
    for( k = i-1; k >= 0; k-- ) 
      sum -= L[i][k]*x[k];
    x[i] = sum / p[i];
  }
  for( i = n-1; i >= 0; i-- ) {
    sum = x[i];
    for( k = i+1; k < n; k++ ) 
      sum -= L[k][i] * x[k];
    x[i] = sum/p[i];
  }
  return x;
}
