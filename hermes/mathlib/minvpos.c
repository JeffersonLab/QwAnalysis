/* ----------------------------------------------------------------------
 * invert a positive definite matrix A
 * Wolfgang Wander 10.4.1996
 */

#include <stdlib.h>		/* for free ,-) */
#include "mathstd.h"
#include "mathdefs.h"

matrix 
M_InvertPos (matrix A, int n)
{
  int i,j, k;
  double sum;
  vektor p = V_Init(n);
  vektor q = V_Init(n);
  matrix inv = M_Init(n,n);

  if( !p || !q || ! inv ) {
    Matherror(M_NOMEM);
    return (matrix)0;
  }
  
  /* first we need the cholesky decomposition */

  if( M_Cholesky( A, p, n) ) {

    for( i = 0; i < n; i++) {
      A[i][i] = 1.0/p[i];
      for( j = i+1; j < n; j++ ) {
	sum = 0;
	for( k = i; k < j; k++ )
	  sum -= A[j][k] * A[k][i];
	A[j][i] = sum / p[j];
      }
    }
    
    for( i = 0; i < n; i++ ) {
      for( j = i; j < n; j++ ) {
	sum = 0;
	for( k = j; k < n; k++ ) 
	  sum += A[k][i] * A[k][j];
	inv[i][j] = inv[j][i] = sum;
      }
    }
    
    M_Kopiere( A, inv, n,n);
  } else 
    A = 0;
  
  free(p);
  free(q);
  free(inv);
  
  return A;
}

#ifdef TEST
main()
{
#define n 3
  matrix A = M_Init(n,n);
  matrix B = M_Init(n,n);
  A[0][0] = 1.0;
  A[0][1] = 0.5;
  A[0][2] = 0.25;
  A[1][0] = 0.5;
  A[1][1] = 1.0;
  A[1][2] = 0.5;
  A[2][2] = 1.0;
  A[2][1] = 0.5;
  A[2][0] = 0.25;
#define m n
  M_Ausgabe("A",A,m,m,0,2,3);
  M_Kopiere( B, A, m,m);
  
  M_InvertPos( A, m);
  M_Inverse(B,m);

  M_Ausgabe("A^-1",A,m,m,0,2,3);
  M_Ausgabe("B^-1",B,m,m,0,2,3);
  M_InvertPos( A, m);
  M_Inverse( B, m);

  M_Ausgabe("A^-1-1",A,m,m,0,2,3);
  M_Ausgabe("B^-1-1",B,m,m,0,2,3);
  return 0;
}
#endif
	  
