#include <stdio.h>
#include "mathstd.h"

/*-------------------------*
  Funktion Vektorausgabe
           Gibt einen Vektor
           auf stdout aus.
 *-------------------------*/


void 
V_Ausgabe (char *info, vektor v, int n, int vorkomma, int nachkomma)
{
  int j;

  printf ("%s ( ", info);
  for (j = 0; j < n; j++)
    printf ("% *.*f ", vorkomma + 1 + nachkomma, nachkomma, v[j]);
  printf (")\n");
  return;
}

