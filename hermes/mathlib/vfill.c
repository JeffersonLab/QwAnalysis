#include "mathstd.h"
#include <string.h>


/*--------------------------*
  Funktion Vektorfuellung:
		   Fuellt die Datenstruktur
		   vektor
 *--------------------------*/

vektor 
V_Fill (vektor v, double *dv, int n)
{
  memcpy (v, dv, n * sizeof (double));
  return v;
}
