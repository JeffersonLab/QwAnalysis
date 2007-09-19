#include <stdio.h>
#include <float.h>		/* Hier ist DBL_EPSILON mit 5.42...E-20 */
#include "mathstd.h"		/* definiert */
#include "mathdefs.h"

/*--------------------------*
  RL_Zerlegung einer Matrix:
  --------------------------
	Das Ergebnis steht in der
	Matrix selbst.
	(Diagonalelemente von L
	 sind alle 1)
 *--------------------------*
  Warnung: In der Funktion wird
       eine geschwindigkeits-
       steigernde Pointernotation
       verwendet, die jedoch
       dukumentiert ist.
 *--------------------------*/


matrix 
M_RLZerlegung (matrix A, int n, int *ipivot)
{
  double Aii, Lji;
  int d, j;

  for (d = 0; d < n - 1; d++) {
    if (-1 == (j = M_Pivot (A, n, n, d))) {
      merrno = M_PIVOT;
      return (matrix) 0L;
    }
    if (ipivot)
      *(ipivot + d) = j;

    Aii = *(*(A + d) + d);
    /* ^ Bedeutet A[d][d] */

    for (j = d + 1; j < n; j++) {	/* Subtraktion der ersten von al- *//* w
					   eiteren Zeilen                */
      Lji = *(*(A + j) + d) / Aii;
      /* ^ Bedeutet A[j][d] */
      M_AddZeile (A, n, n, j, d, -Lji, d);
      /* Addiert die d-te Zeile mit -Lji */
      /* multipliziert zur j-ten Zeile  */
      /* (aber erst ab der d+1-ten Spalte) */
      *(*(A + j) + d) = Lji;
      /* ^ Bedeutet A[j][d] */
    }
  }
  if (ipivot)
    ipivot[n - 1] = n - 1;
  /* Noetig, da dieses Feld sonst nicht ini-  */
  /* tialisiert ist */
  return A;
}
