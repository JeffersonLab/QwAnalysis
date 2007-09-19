#include "mathstd.h"

double sqrt (double x);
double V_SkalarProdukt (vektor a, vektor b, int dimension);

/*--------------------------*
  Funktion Betrag:
  		   Berechnet den
  		   Betrag eines
  		   Vektors.
 *--------------------------*/

double 
V_Betrag (vektor a, int dimension)
{
  return sqrt (V_SkalarProdukt (a, a, dimension));
}
