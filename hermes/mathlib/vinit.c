#include <stdlib.h>
#include "mathstd.h"
#include "mathdefs.h"

int Matherror (int Fehlernummer);

/*--------------------------*
  Funktion Vektorinitialisierung:
		   Stellt Speicherplatz
		   fuer einen n-dim
		   Vektor zur Verfuegung.
 *--------------------------*/

vektor 
V_Init (int Dimension)
{
  vektor ret;

  ret = (vektor) malloc (sizeof (double) * Dimension);

  if (!ret)
    Matherror (M_NOMEM);	/* --> Programmende */

  return ret;
}

