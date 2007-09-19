#include <string.h>
#include "mathstd.h"


/*--------------------------*
  Funktion Einheitsvektor:
           Erstellt den k-ten
           Einheitsvektor
 *--------------------------*/

vektor 
V_Einheit (vektor Ergebnis, int k, int dimension)
{
  memset (Ergebnis, 0, sizeof (double) * dimension);

  *(Ergebnis + k) = 1.0;

  return Ergebnis;
}
