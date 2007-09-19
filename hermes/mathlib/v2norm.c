#include "mathstd.h"

double sqrt (double x);
double V_SkalarProdukt (vektor a, vektor b, int dimension);

/*--------------------------*
  Funktion 2_Norm:
  		   Berechnet die
  		   2_Norm eines
  		   Vektors.
 *--------------------------*/

double 
V_2Norm (vektor a, int dimension)
{
  return sqrt (V_SkalarProdukt (a, a, dimension));
}
