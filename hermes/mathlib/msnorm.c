#include "mathstd.h"

double fabs (double);

/*--------------------------*
  Funktion : Spaltensummennorm einer Matrix

 *--------------------------*/

double 
M_SNorm (matrix A, int n, int m)
{
  double Summe, Max = 0.0;
  int zaehler;

  while (m--) {
    zaehler = n;
    Summe = 0.0;
    while (zaehler--)
      Summe += fabs (*(*(A + zaehler) + m));
    if (Summe > Max)
      Max = Summe;
  }
  return Max;
}
