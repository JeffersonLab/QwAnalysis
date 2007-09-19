#include <stdlib.h>
#include "mathstd.h"
#include "mathdefs.h"

/*--------------------------*
  Funktion Matrixinitialisierung:
		   Stellt Speicherplatz
		   fuer eine nxm-Matrix
		   zur Verfuegung.
 *--------------------------*/

matrix 
M_Init (int Zeilen, int Spalten)
{
  matrix ret;
  int n;

  n = Zeilen;

  ret = (matrix) calloc (1, 0x10 + Zeilen * (sizeof (vektor) + 
					     Spalten * sizeof (double)));

  if (!ret)
    Matherror (M_NOMEM);	/* --> Programmende */

  while (n--)
    *(ret + n) = (vektor) (((char *) ret) +
			   (( (Zeilen * sizeof (vektor)) & ~0xf ) +0x10) +
			   n * Spalten * sizeof (double));

  return ret;
}

