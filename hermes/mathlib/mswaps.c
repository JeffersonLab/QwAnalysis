#include "mathstd.h"

/*--------------------------*
  Funktion : Spaltenvertauschen
  			Die spalte1 und
  			spalte2 werden vertauscht
 *--------------------------*/

void 
M_SwapS (matrix A, int n, int m, int spalte1, int spalte2, int start)
{
  double help;

  if (spalte1 != spalte2 && spalte1 < m && spalte2 < m) {
    while (n-- > start) {
      help = *(*(A + n) + spalte1);
      *(*(A + n) + spalte1) = *(*(A + n) + spalte2);
      *(*(A + n) + spalte2) = help;
    }
  }
  return;
}

