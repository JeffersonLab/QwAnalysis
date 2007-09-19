#include <stdio.h>		/* Include-File fuer Standart Ein-Ausgabe */
#include "mathstd.h"		/* Include-File fuer C-Mathe-Library      */
#include "mathdefs.h"

void free (void *ptr);

double A1[6][5] =		/* Hier steht die fest vorge- */
{				/* gebene Matrix fuer Aufga-  */
  {-74, 80, 18, -11, -4},	/* benteil 1(ii)			  */
  {14, -69, 21, 28, 0},
  {66, -72, -5, 7, 1},
  {-12, 66, -30, -23, 3},
  {3, 8, -7, -4, 1},
  {4, -12, 4, 4, 0}
};

double b1[6] =			/* Und das sind die zugehoeh- */
/* rigen Vektoren			  */
{
  51,
  -61,
  -56,
  69,
  10,
  -12
};
double b2[6] =
{
  -5,
  -9,
  708,
  4165,
  -13266,
  8409
};

/*-------------------------------*
  Die Funktion GETPARAMS liefert
  in Abhaengigkeit von par die zu-
  gehoerigen Werte fuer Aufgaben-
  teil a) und b) zurueck.
  par kann die Werte 1 und 2 (fuer
  Teil (ii)) und 3 bis 7 (fuer
  Teil (i) annehmen.)
 *-------------------------------*/

void 
getparams (int par, int *m, int *n, matrix A, vektor b)
{
  int i, k, j;			/* Laufparameter (wie auf dem Uebungsblatt */

  if (par < 3) {		/* Sonderbehandlung fuer Aufgabenteil (ii)   */
    if (par == 1)		/* Falls par = 2, dann ist QR bereits */
      /* berechnet 						  */
      M_Fill (A, *A1, 6, 5);
    /* M_Fill fuellt die Matrix A mit den */
    /* Daten aus M1 (s.o.)				  */
    if (par == 1)
      V_Fill (b, b1, 6);	/* V_Fill fuellt den Vektor b mit b1  */
    else
      V_Fill (b, b2, 6);	/* V_Fill fuellt den Vektor b mit b2  */
    *m = 6;
    *n = 5;
  }
  else {			/* Berechne die nxn Hilbertmatrix */
    /* ACHTUNG: Alle Felder fangen in C beim Index 0 */
    /* an. Deshalb auch die leicht geaenderte Formel */
    /* fuer die Matrix A[i][k] und Vektor b. 		  */
    for (i = 0; i < par; i++)
      for (k = 0; k < par; k++)
	A[i][k] = 1.0 / (double) (1 + i + k);	/* Matrix A      */

    for (i = 0; i < par; i++) {
      b[i] = 0.0;
      for (j = 0; j < par; j++)	/* Berechne die Summe fuer    */
	b[i] += (double) (j + 1) * A[i][j];	/* Vektor b    */
    }
    *n = *m = par;		/* Gebe als Dimension par zurueck       */
  }
}


/****************************/
/* Start des Hauptprogramms */
/****************************/

main ()
{
  /*------------------------
	  Variablendeklaration
    ------------------------*/
  int par;			/* Laufvariable fuer den jeweiligen Aufgaben- */
  /* teil (von 2 bis 7)						  */
  int m, n;			/* Dimension des LGS						  */
  matrix A;
  vektor b, D;

  A = M_Init (15, 15);		/* Initialisiert A als 15x15 Matrix (max) */
  b = V_Init (15);		/* Initialisiert b als 15-dim Vektor (max) */
  D = V_Init (15);		/* Initialisiert D als 15-dim Vektor (max) */

  /*------------*/
  /* Berechnung */
  /*------------*/

  puts ("Dieses Programm loest die Aufgabe 1b) auf dem Uebungsblatt");
  puts ("Nummer 6 zur Einfuehrung in die Numerik.");
  puts ("Naehere Informationen koennen dort entnommen werden.");
  puts ("----------------------------------------------------");
  puts ("Es wird nach der Loesung des LSQ-Problems |Ax - b| = min gesucht.");
  puts ("----------------------------------------------------");


  for (par = 1; par <= 7; par++) {	/* Aufgabenteil a) (i <= 2), b) sonst */
    getparams (par, &m, &n, A, b);	/* Holt d. Gleichungssysteme */
    if (par <= 2)
      printf ("Aufgabenteil (ii)\n...................\n");
    else
      printf ("Aufgabenteil (i) Hilbert %d:\n............................\n", par);

    M_Ausgabe ("Matrix A:", A, m, n, 6, 3, 4);
    V_Ausgabe ("Vektor b:\nb =", b, m, 2, 5);
    /* Zeigt Matrix A und Vektor b an */
    /*--------------------------------------*/
    X_Timer ();			/* Stellt Stopuhr                 */
    if (par != 2)		/* Wurde schon bei par= 1 bestimmt */
      M_LSqHouseholder (A, m, n, D);
    M_XausHouseholder (A, D, m, n, b);
    printf ("Rechenzeit fuer die Loesung: %lfSekunden\n", X_Timer ());
    M_Ausgabe ("Matrix A nach Transformation:", A, m, n, 6, 3, 4);
    V_Ausgabe ("Loesung fuer x und Residuum:", b, m, 2, 5);
  }				/* Ende der Schleife fuer die Aufgabenteile */

  puts ("\nDanke fuer Ihre Aufmerksamkeit.");
  free (A);
  free (b);
  free (D);
  return 0;
}
