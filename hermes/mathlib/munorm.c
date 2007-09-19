#include "mathstd.h"

double fabs (double);

/*--------------------------*
  Funktion : Supremumsnorm
  			einer Matrix
 *--------------------------*/

double 
M_UNorm (matrix A, int n, int m)
{
  double Max = 0.0, help;
  int zaehler;

  while (n--) {
    zaehler = m;
    while (zaehler--)
      if (Max < (help = fabs (*(*(A + n) + zaehler))))
	Max = help;
  }
  return Max;
}
