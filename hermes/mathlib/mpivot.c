#include <stdlib.h>
#include <float.h>
#include "mathstd.h"

double fabs (double);

/*--------------------------*
  Funktion : Pivotsuche

 *--------------------------*/

int 
M_Pivot (matrix A, int n, int m, int Durchgang)
{
  int zaehler;
  double max = -1.0, help;
  double unorm;
  int maxzeile = -1;

  if (n > Durchgang) {
    zaehler = n;

    while (zaehler-- > Durchgang)
      if (max < (help = fabs (*(*(A + zaehler) + Durchgang)))) {
	maxzeile = zaehler;
	max = help;
      }
    unorm = M_UNorm (A, n, m);
    if (max < n * DBL_EPSILON * unorm)
      maxzeile = -1;
    else if (maxzeile != Durchgang)
      M_SwapZ (A, n, m, Durchgang, maxzeile, 0);
  }
  return maxzeile;
}
