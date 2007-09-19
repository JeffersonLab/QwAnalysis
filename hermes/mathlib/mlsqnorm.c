#include "mathstd.h"
void free (void *ptr);

/*--------------------------*
  Funktion :
	Least-Square Optimierung
	ueber Normalengleichung
 *--------------------------*/


vektor 
M_LSqNorm (vektor x, matrix A, int m, int n, vektor b)
{
  int i, j;
  double summe;
  vektor p;
  matrix L;

  L = M_Init (n, n);
  p = V_Init ( n );

  /* Berechne A(T)b */
  for (i = 0; i < n; i++) {
    summe = 0;
    j = m;
    while (j--)
      summe += A[j][i] * b[j];
    x[i] = summe;
  }

  /* Berechne A(T)A */
  M_A_T_A (L, A, m, n);

  /* Cholesky Zerlegung */
  M_Cholesky (L, p, n);

  /* Berechne x */
  M_XausCholesky (L, p, n, x);

  free (L);
  free (p);
  return x;
}

