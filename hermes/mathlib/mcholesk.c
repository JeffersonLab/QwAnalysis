#include <float.h>
#include <math.h>

#include "mathstd.h"
#include "mathdefs.h"


/*--------------------------*
  Funktion :
	Cholesky-Algorithmus zur
	Zerlegung einer positiv
	definiten Matrix A. Das
	Ergebnis steht wieder in
	A. (FORTRAN-Konvention!!)
 *--------------------------*/

matrix 
M_Cholesky (matrix A, vektor p, int n)
{
  register int i, j, k;
  double sum, min;

  min = M_UNorm (A, n, n) * DBL_EPSILON * n;

  for( i = 0; i < n; i++ ) {
    for( j = i; j < n; j++ ) {
      sum = A[i][j];
      for( k = i-1; k >= 0; k-- ) 
	sum -= A[i][k] * A[j][k];
      if( i == j ) {
	if( sum < min ) {
	  Matherror(M_PIVOT);
	  return (matrix)0;
	}
	p[i] = sqrt(sum);
      } else
	A[j][i] = sum/p[i];
    }
  }
  return A;
}
