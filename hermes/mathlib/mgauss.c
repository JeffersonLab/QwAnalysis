#include <stdio.h>
#include "mathstd.h"
#include "mathdefs.h"

void free (void *ptr);

/*--------------------------*
  Gauss-Elimination:
  ------------------
  	Zum Berechnen von eindeutig
  	loesbaren LGS.
 *--------------------------*/

vektor 
M_Gauss (vektor Ergebnis, matrix A, int n, vektor b)
{
  matrix G;
  double Aii;
  int d, j;

  G = M_Init (n, n + 1);

  M_Erweitere (G, A, n, n, b);

  for (d = 0; d < n - 1; d++) {
    if (-1 == M_Pivot (G, n, n + 1, d)) {
      merrno = M_PIVOT;
      return NULL;
    }

    Aii = *(*(G + d) + d);

    for (j = d + 1; j < n; j++)
      M_AddZeile (G, n, n + 1, j, d, -*(*(G + j) + d) / Aii, d);
  }
  M_Spaltenvektor (Ergebnis, G, n, n + 1, n);
  M_Loese (G, n, Ergebnis);

  free (G);
  return Ergebnis;
}

