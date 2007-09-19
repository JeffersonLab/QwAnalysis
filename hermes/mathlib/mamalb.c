#include "mathstd.h"

/*--------------------------*
  Funktion : Multiplikation
  	eines Vektors mit einer
  	Matrix.
 *--------------------------*/

vektor 
M_A_mal_b (vektor y, matrix A, int n, int m, vektor b)
{
  int zaehler;
  double Summe;

  while (n--) {
    zaehler = m;
    Summe = 0.0;
    while (zaehler--)
      Summe += *(*(A + n) + zaehler) * *(b + zaehler);
    *(y + n) = Summe;
  }
  return y;
}
