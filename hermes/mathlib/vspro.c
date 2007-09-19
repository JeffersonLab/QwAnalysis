#include "mathstd.h"

/*--------------------------*
  Funktion S-Produkt
  		   Berechnet das
  		   S-Produkt eines
  		   Vektors mit einem
  		   Skalar.
 *--------------------------*/

vektor 
V_S_Produkt (vektor Ergebnis, double skalar, vektor a, int dimension)
{
  while (dimension--)
    *(Ergebnis + dimension) = skalar * *(a + dimension);

  return Ergebnis;
}
