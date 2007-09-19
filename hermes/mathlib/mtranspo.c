#include <stdio.h>
#include "mathstd.h"

/*--------------------------*
  Funktion Matrix Transponieren:
 *--------------------------*/

matrix 
M_Transponiert (matrix Ergebnis, matrix A, int oldspalte, int oldzeile)
{
  int i,j;
  for( i = 0; i < oldspalte; i++ ) {
    for( j = 0; j < oldzeile; j++ ) {
      Ergebnis[i][j] = A[j][i];
    }
  }
  return Ergebnis;
}

