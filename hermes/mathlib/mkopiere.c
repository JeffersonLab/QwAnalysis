#include "mathstd.h"

/*--------------------------*
  Funktion Matrixdyade:
		   Berechnet eine Dyade
		   aus zwei Vektoren
 *--------------------------*/

#include <string.h>

matrix 
M_Kopiere (matrix Ergebnis, matrix A, int n, int m)
{

  while (n--)
    memcpy (*(Ergebnis + n), *(A + n), sizeof (double) * m);
  return Ergebnis;
}

