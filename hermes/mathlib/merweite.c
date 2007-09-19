#include "mathstd.h"

/*--------------------------*
  Funktion MatrixErweiterung:
		   Erweitert die Matrix A
		   um den Vektor v.
 *--------------------------*/

matrix 
M_Erweitere (matrix Ergebnis, matrix A, int n, int m, vektor v)
{
  M_Kopiere (Ergebnis, A, n, m);
  while (n--)
    *(*(Ergebnis + n) + m) = *(v + n);

  return Ergebnis;
}


