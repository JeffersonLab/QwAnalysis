#include "mathstd.h"

double V_SkalarProdukt (vektor a, vektor b, int dimension);
double fabs (double x);

/*--------------------------*
  Funktion U_Norm:
  		   Berechnet die
  		   Unendlich_Norm eines
  		   Vektors.
 *--------------------------*/

double 
V_UNorm (vektor a, int dimension)
{
  double Max = 0.0, help;

  while (dimension--)
    if (Max < (help = fabs (*(a + dimension))))
      Max = help;

  return Max;
}
