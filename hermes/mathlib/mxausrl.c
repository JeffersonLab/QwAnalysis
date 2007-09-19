#include "mathstd.h"

/*--------------------------*
  Funktion : Loesen einer
	in R- und L- Matrix zer-
	legten Gleichung
 *--------------------------*/

vektor 
M_XausRL (matrix A, int dim, vektor x, int ipivot[])
{
  int j, n, l;
  double summe, Hilfe;

  for (n = 0; n < dim; n++) {	/* Berechne c mit Lc = b */
    l = ipivot[n];
    if (n - l) {		/* Falls n ungleich l */
      Hilfe = *(x + n);
      *(x + n) = *(x + l);
      *(x + l) = Hilfe;
    }
    summe = *(x + n);
    /* ^ Bedeutet x[n] */
    for (j = 0; j < n; j++)
      summe -= *(x + j) * *(*(A + n) + j);
    /* ^ Bedeutet : summe = summe - A[n][j] * x[j] */

    *(x + n) = summe;
    /* ^ Bedeutet : x[n] = summe / 1.0; */
  }
  M_Loese (A, dim, x);		/* Loest eine obere Dreiecksmatrix nach dem */
  /* selben Schema auf: Also Rx = c */
  return x;
}
