#include "mathstd.h"
#include "mathdefs.h"

void *calloc (long nitems, long size);
void free (void *ptr);

/*--------------------------*
  Funktion MatrixInverse:
	Bildet die Inverse einer
	nicht singulaeren Matrix.
	
	Algorithmus aus STOER BAND 1
	 (mit kleinen Aenderungen)
 *--------------------------*/

matrix 
M_Inverse (matrix A, int n)
{
  int i, j, k, r, hi;
  double hr, det, h;
  static vektor hv;
  static int *p = 0, maxn = 0;

  /* 1 - 2 dimensions can be done faster */
  if( n == 1 ) {
    if( A[0][0] ) {
      A[0][0] = 1.0/A[0][0];
      return A;
    } else {
      merrno = M_PIVOT;
      return (matrix) 0L;
    }
  }
  if( n == 2 ) {
    det = A[0][0] * A[1][1] - A[1][0] * A[0][1];
    if( !det ) {
      merrno = M_PIVOT;
      return (matrix) 0L;
    }
    h = A[0][0] / det;
    A[0][0] = A[1][1] / det;
    A[1][1] = h;
    A[1][0] /= -det;
    A[0][1] /= -det;
    return A;
  }
  
  if( maxn < n ) {
    if( p )
      free(p);
    if( hv )
      free( hv );
    maxn = n;
    hv = V_Init (n);
    p = (int *) calloc (n, sizeof (int));
  }

  for (j = 0; j < n; j++)
    p[j] = j;

  for (j = 0; j < n; j++) {
    if (-1 == (r = M_Pivot (A, n, n, j))) {
      free (p);
      merrno = M_PIVOT;
      return (matrix) 0L;
    }
    if (j - r) {
      hi = p[j];
      p[j] = p[r];
      p[r] = hi;
    }

    hr = 1.0 / *(*(A + j) + j);

    for (i = 0; i < n; i++)
      *(*(A + i) + j) *= hr;

    *(*(A + j) + j) = hr;

    for (k = 0; k < n; k++) {
      if (k == j)
	continue;
      for (i = 0; i < n; i++) {
	if (i == j)
	  continue;
	*(*(A + i) + k) -= *(*(A + i) + j) * *(*(A + j) + k);
      }
      *(*(A + j) + k) = -hr * *(*(A + j) + k);
    }
  }

  for (i = 0; i < n; i++) {
    for (k = 0; k < n; k++)
      *(hv + *(p + k)) = *(*(A + i) + k);
    for (k = 0; k < n; k++)
      *(*(A + i) + k) = *(hv + k);
  }
  return A;
}
