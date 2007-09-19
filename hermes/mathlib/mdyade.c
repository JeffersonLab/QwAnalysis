#include "mathstd.h"

/*--------------------------*
  Funktion Matrixdyade:
		   Berechnet eine Dyade
		   aus zwei Vektoren
 *--------------------------*/

matrix 
M_Dyade (matrix Ergebnis, vektor u, int dim_u, vektor v, int dim_v)
{
  int zaehler;

  while (dim_u--) {
    zaehler = dim_v;
    while (zaehler--)
      *(*(Ergebnis + dim_u) + zaehler) =
	*(u + dim_u) * *(v + zaehler);
  }
  return Ergebnis;
}

