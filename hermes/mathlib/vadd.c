#include "mathstd.h"

/*--------------------------*
  Funktion Berechnet die
  		   Summe
  		   zweier Vektoren.
 *--------------------------*/
vektor 
V_Add (vektor x, vektor b, vektor c, int n)
{
  while (n--)
    *(x + n) = *(c + n) + *(b + n);
  return x;
}

/*--------------------------*
  Funktion Berechnet die
  		   Differenz
  		   zweier Vektoren.
 *--------------------------*/
vektor 
V_Sub (vektor x, vektor b, vektor c, int n)
{
  while (n--)
    *(x + n) = *(b + n) - *(c + n);
  return x;
}
