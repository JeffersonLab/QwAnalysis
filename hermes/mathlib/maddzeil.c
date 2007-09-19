#include "mathstd.h"

/*--------------------------*
  Funktion : Addition einer
  	Zeile z2 multipliziert
  	mit faktor zu z1
 *--------------------------*/

void 
M_AddZeile (matrix A, int n, int m, int z1, int z2, double faktor, int start)
{
  if (z1 < n && z2 < n)		/* && bedeutet UND */
    while (m-- > start)		/* Solange m > start decremen- */
      /* tiere m aund fuehre aus:    */
      *(*(A + z1) + m) += faktor * *(*(A + z2) + m);
  /* A[z1,m] = A[z1,m] + faktor * A[z2,m] */
}
