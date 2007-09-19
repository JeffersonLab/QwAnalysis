#include "mathstd.h"

double fabs (double);

/*--------------------------*
  Funktion : Zeilensummennorm einer Matrix

 *--------------------------*/

double 
M_ZNorm (matrix A, int n, int m)
{
  double Summe, Max = 0.0;
  int zaehler;

  while (n--) {
    zaehler = m;
    Summe = 0.0;
    while (zaehler--)
      Summe += fabs (*(*(A + n) + zaehler));
    if (Summe > Max)
      Max = Summe;
  }
  return Max;
}
