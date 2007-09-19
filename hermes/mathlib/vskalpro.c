#include "mathstd.h"

/*--------------------------*
  Funktion Skalarprodukt:
  		   Berechnet das
  		   Skalarprodukt
  		   zweier Vektoren.
 *--------------------------*/
double 
V_SkalarProdukt (vektor a, vektor b, int dimension)
{
  double Produkt = 0;

  while (dimension--)
    Produkt += *(a + dimension) * *(b + dimension);

  return Produkt;
}
