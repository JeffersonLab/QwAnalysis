#include "mathstd.h"

void free (void *ptr);
void *malloc (long size);

/*--------------------------*
  Funktion Determinante einer nxn-
  		   Matrix.
  	Benutzt die RL-Zerlegung
 *--------------------------*/

double 
M_Determinante (matrix A, int n)
{
  double Produkt = 1.0;
  int *ipivot, c;
  matrix B;


  ipivot = (int *) malloc (sizeof (int) * 2);
  B = M_Init (n, n);

  M_Kopiere (B, A, n, n);

  if (!M_RLZerlegung (B, n, ipivot))
    return 0.0;
  c = n;
  while (c--)
    Produkt *= *(*(B + c) + c);
  c = 0;
  while (n--)
    if (ipivot[n] != n)
      c++;
  if (c & 1)			/* c ungerade?  (& heisst bitweise UND) */
    Produkt = -Produkt;

  free (B);
  free (ipivot);
  return Produkt;
}

