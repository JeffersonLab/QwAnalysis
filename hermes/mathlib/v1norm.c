#include "mathstd.h"

double V_SkalarProdukt (vektor a, vektor b, int dimension);
double fabs (double x);

/*--------------------------*
  Funktion 1_Norm:
  		   Berechnet die
  		   1_Norm eines
  		   Vektors.
 *--------------------------*/

double 
V_1Norm (vektor a, int dimension)
{
  double Summe = 0.0;

  while (dimension--)
    Summe += fabs (*(a + dimension));

  return Summe;
}
