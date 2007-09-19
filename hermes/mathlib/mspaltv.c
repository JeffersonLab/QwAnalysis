#include "mathstd.h"

/*--------------------------*
  Funktion : Herausziehen
	eines Splatenvektors aus
	einer Matrix
 *--------------------------*/

vektor 
M_Spaltenvektor (vektor v, matrix A, int n, int m, int spalte)
{
  if (spalte < m)
    while (n--)
      *(v + n) = *(*(A + n) + spalte);
  return v;
}
