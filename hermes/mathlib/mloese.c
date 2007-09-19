#include "mathstd.h"

/*--------------------------*
  Funktion : Loesen einer
  	oberen Dreieckmatrixgleichung
 *--------------------------*/

vektor 
M_Loese (matrix A, int nm, vektor v)
{
  int j, n;
  double summe;

  n = nm;
  while (n--) {			/* Solange n ungleich 0 decrementiere n und
				   f�hre aus */
    summe = *(v + n);		/* summe = v[n] */
    for (j = n + 1; j < nm; j++)
      summe -= *(v + j) * *(*(A + n) + j);	/* summe = summe - v[j] *
						   A[n,j] */
    *(v + n) = summe / *(*(A + n) + n);	/* v[n] = summe / A[n,n] */
  }
  return v;
}
