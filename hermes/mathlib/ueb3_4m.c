#include <stdio.h>		/* Include-File fuer Standart Ein-Ausgabe */
#include "mathstd.h"		/* Include-File fuer C-Mathe-Library      */
#include "mathdefs.h"

double M1[4][4] =		/* Hier steht die fest vorge- */
{				/* gebene Matrix fuer Aufga-  */
  {2, 4, 6, 8},			/* benteil a)				  */
  {4, 9, 14, 19},
  {6, 14, 23, 32},
  {8, 19, 32, 50}
};

double b1[4] =
{6, 11, 14, 12};		/* Und das ist der zugehoeh-  */
/* rige Vektor				  */

/*-------------------------------*
  Die Funktion GETPARAMS liefert
  in Abhaengigkeit von n die zu-
  gehoerigen Werte fuer Aufgaben-
  teil a) und b) zurueck.
  n kann die Werte 2 (fuer Teil a)
  und 3 bis 7 (fuer Teil b) anneh-
  men.
 *-------------------------------*/

void 
getparams (int n, int *dim, matrix A, vektor b)
{
  int i, k, j;			/* Laufparameter (wie auf dem Uebungsblatt */

  if (n == 2) {			/* Sonderbehandlung fuer Aufgabenteil a)   */
    M_Fill (A, *M1, 4, 4);	/* M_Fill fuellt die Matrix A mit den */
    /* Daten aus M1 (s.o.)				  */
    V_Fill (b, b1, 4);		/* V_Fill fuellt den Vektor b mit b1  */
    *dim = 4;			/* Gebe als Dimension 4 zurueck       */
  }
  else {			/* Berechne die nxn Hilbertmatrix */
    /* ACHTUNG: Alle Felder fangen in C beim Index 0 */
    /* an. Deshalb auch die leicht geaenderte Formel */
    /* fuer die Matrix A[i][k] und Vektor b. 		  */
    for (i = 0; i < n; i++)
      for (k = 0; k < n; k++)
	A[i][k] = 1.0 / (double) (1 + i + k);	/* Matrix A      */

    for (i = 0; i < n; i++) {
      b[i] = 0.0;
      for (j = 0; j < n; j++)	/* Berechne die Summe fuer    */
	b[i] += (double) (j + 1) * A[i][j];	/* Vektor b    */
    }
    *dim = n;			/* Gebe als Dimension n zurueck       */
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
  int i;			/* Laufvariable fuer den jeweiligen Aufgaben- */
  /* teil (von 2 bis 7)						  */
  int Dimension;		/* Dimension des LGS						  */
  matrix A;
  vektor b;
  double feld[15];		/* Loesungsfeld fuer alternativen Gauss-Algo- */
  /* rithmus (mit Pivotsuche)				   */
  int ipivot[15];		/* Permutationsfeld fuer Pivotsuche */

  A = M_Init (15, 15);		/* Initialisiert A als 7x7 Matrix (maximal) */
  b = V_Init (15);		/* Initialisiert b als 7-dim Vektor (max)   */

  /*------------*/
  /* Berechnung */
  /*------------*/

  puts ("Dieses Programm loest die Aufgabe 4 auf dem Uebungsblatt");
  puts ("Nummer 3 zur Einfuehrung in die Numerik.");
  puts ("Naehere Informationen koennen dort entnommen werden.");
  puts ("----------------------------------------------------");
  puts ("Es wird nach der Loesung des LGS Ax = b gesucht.");
  puts ("Zusaetzlich enthaelt dieses Programm einen alternativen");
  puts ("Gaussalgorithmus mit Pivotsuche.");
  puts ("----------------------------------------------------");


  for (i = 2; i <= 12; i++) {	/* Aufgabenteil a) (i = 2), b) sonst */
    getparams (i, &Dimension, A, b);	/* Holt d. Gleichungssysteme */
    if (i == 2)
      printf ("Aufgabenteil a)\n...................\n");
    else
      printf ("Aufgabenteil b) Hilbert %d:\n............................\n", i);

    M_Ausgabe ("Matrix A:", A, Dimension, Dimension, 6, 3, 4);
    V_Ausgabe ("Vektor b:\nb =", b, Dimension, 2, 5);
    /* Zeigt Matrix A und Vektor b an */
    /*--------------------------------------*/
    X_Timer ();			/* Stellt Stopuhr                 */
    /* S */ if (!M_Gauss (feld, A, Dimension, b)) {	/* Alternativer
							   Gaussalgor-  */
      /* t */
      /* rithmus mit Pivot-Suche.       */
      /* o */
      /* Das Source-Listing ist nicht   */
      /* p */
      /* beigefuegt, kann aber nachge-  */
      /* u */
      /* reicht werden.                 */
      /* h */
      /* A und b bleiben hier unveraen- */
      /* r */
      /* dert.						  */
      Matherror (M_PIVOT);	/*-> Pivotsuche erfolglos */
    }
    printf ("Zeit fuer Gauss mit Pivotsuche: %4.3lf Sec\n", X_Timer ());
    /* Zeigt die Rechenzeit an		  */
    V_Ausgabe ("Ergebnis (Gauss mit Pivotsuche):\nx =", feld, Dimension, 2, 5);
    /* Zeigt das Ergebnis an          */
    /*-------------------------------------*/
    X_Timer ();			/* Stellt Stopuhr                 */
    /* l */ M_RLZerlegung (A, Dimension, ipivot);
    /* a *//* Gaussalgorithmus, der im Auf-  */
    /* e *//* gabenblatt verlangt wurde.     */
    /* u */ M_XausRL (A, Dimension, b, ipivot);
    /* f *//* Berechnet mit Hilfe der L- und */
    /* t *//* der R-Matrix das Ergebnis von  */
    /* . *//* Ax = b und legt dieses in b ab. */
    printf ("Zeit fuer RL-Zerlegung: %4.3lf Sec\n", X_Timer ());
    /* Zeigt die Rechenzeit an		  */
    M_Ausgabe ("Ergebnis der RL-Zerlegung", A, Dimension, Dimension, 6, 3, 4);
    /* Ausgabe der zerlegten Matrix A */
    V_Ausgabe ("Ergebnis fuer x aus RL-Zerlegung:\nx =", b, Dimension, 2, 5);
    /* Ausgabe des Ergebnisses in b   */
    /*-------------------------------------*/
    puts ("\n------------------------------------------------------\n");
  }				/* Ende der Schleife fuer die Aufgabenteile */

  puts ("\nDanke fuer Ihre Aufmerksamkeit.");
  return 0;
}
