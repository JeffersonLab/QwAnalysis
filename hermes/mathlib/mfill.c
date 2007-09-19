#include "mathstd.h"


/*--------------------------*
  Funktion Matrixfuellung:
		   Fuellt die Datenstruktur
		   matrix
 *--------------------------*/

matrix 
M_Fill (matrix A, double *M, int Zeilen, int Spalten)
{
  int zaehler;

  while (Zeilen--) {
    zaehler = Spalten;
    while (zaehler--)
      *(*(A + Zeilen) + zaehler) = *(M + Zeilen * Spalten + zaehler);
  }
  return A;
}
