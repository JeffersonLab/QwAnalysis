#include <stdio.h>
#include "mathstd.h"

/*-------------------------*
  Funktion Matrixausgabe
           Gibt die Matrix
           auf stdout aus.
 *-------------------------*/


void 
M_Ausgabe (char *info, matrix M, int n, int m, int tab, int vorkomma, int nachkomma)
{
  int i, j;

  puts (info);
  for (i = 0; i < n; i++) {
    printf ("%*.*s| ", tab, tab, "");
    for (j = 0; j < m; j++)
      printf ("% *.*f ", vorkomma + 1 + nachkomma, nachkomma, M[i][j]);
    printf ("|\n");
  }
  return;
}

