#include "mathstd.h"

double fabs (double);
double sqrt (double);

/*--------------------------*
  Funktion : Frobeniusnorm
  			einer Matrix
 *--------------------------*/

double 
M_FNorm (matrix A, int n, int m)
{
  double Summe = 0.0;
  int zaehler;

  while (n--) {
    zaehler = m;
    while (zaehler--)
      Summe += *(*(A + n) + zaehler) * *(*(A + n) + zaehler);
  }
  return sqrt (Summe);
}
