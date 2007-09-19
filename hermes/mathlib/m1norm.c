#include "mathstd.h"

double fabs (double);

/*--------------------------*
  Funktion : 1-Norm einer Matrix

 *--------------------------*/

double 
M_1Norm (matrix A, int n, int m)
{
  double Summe = 0.0;
  int zaehler;

  while (n--) {
    zaehler = m;
    while (zaehler--)
      Summe += fabs (*(*(A + n) + zaehler));
  }
  return Summe;
}
