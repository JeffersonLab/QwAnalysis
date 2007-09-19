#include "mathstd.h"
#include "mathdefs.h"

int Matherror (int Fehlernummer);

/*--------------------------*
  Funktion Matrixmultiplikation:
		   Berechnet das Produkt
		   zweier Matrizen
 *--------------------------*/


matrix 
M_Multiplikation (matrix Ergebnis,
		  matrix A, int A_Zeilen, int A_Spalten,
		  matrix B, int B_Zeilen, int B_Spalten)
{
  int i, zaehler;
  double Summe;

  if (B_Zeilen != A_Spalten)
    Matherror (M_NOMUL);	/* --> Programmende */

  while (A_Zeilen--) {
    zaehler = B_Spalten;
    while (zaehler--) {
      Summe = 0;
      i = A_Spalten;
      while (i--)
	Summe += A[A_Zeilen][i] * B[i][zaehler];
      Ergebnis[A_Zeilen][zaehler] = Summe;
    }
  }

  return Ergebnis;
}

