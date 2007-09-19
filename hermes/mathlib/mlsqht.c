#include "mathstd.h"
#include "mathdefs.h"
#include <float.h>

double sqrt (double x);

void free (void *ptr);

/*--------------------------*
  Funktion :
	Least-Square Optimierung
	Householder-Transformation
 *--------------------------*/

vektor 
M_LSqHouseholder (matrix A, int m, int n, vektor D)
{
  int i, j, k;
  double alpha, uTu, uTy, Quotient, min;
  double ynorm, y2;


  min = M_UNorm (A, m, n) * DBL_EPSILON * m;

  for (i = 0; i < n; i++) {
    y2 = 0.0;			/* Berechnung der 2-Norm der */
    for (j = i; j < m; j++)	/* i-ten Spalte von A ( erst */
      y2 += A[j][i] * A[j][i];	/* ab der i-ten Zeile )      */
    ynorm = sqrt (y2);

    if (abs (A[i][i]) < min) {
      Matherror (M_PIVOT);	/* Rang A < n --> Abbruch	 */
      return (vektor) 0L;
    }

    alpha = (A[i][i] > 0.0) ?	/* (exp)? a:b bedeutet:      */
      -ynorm : ynorm;		/* Triff exp zu, dann nehme a */
    /* sonst b. Einsparung einer */
    /* Multiplkation mit 1 bzw -1 */

    uTu = 1 / (y2 - alpha * A[i][i]);	/* Faktor 2 kuerzt sich      */
    /* spaeter wieder raus		 */

    for (j = i + 1; j < n; j++) {	/* Wende P auf Restmatrix an */

      uTy = -alpha * A[i][j];	/* Berechne uTy 			 */
      for (k = i; k < m; k++)
	uTy += A[k][i] * A[k][j];

      Quotient = uTy * uTu;	/* Fester Quotient nur 1mal  */
      /* berechnen				 */

      A[i][j] += alpha * Quotient;
      for (k = i; k < m; k++)
	A[k][j] -= A[k][i] * Quotient;
    }

    D[i] = A[i][i] - alpha;	/* Setze Transformations-    */
    /* informationen.			 */
    A[i][i] = alpha;		/* Setze Aii				 */

  }
  return D;			/* Und verabschiede Dich!    */
}

/*--------------------------*
  Funktion :
	Least-Square Optimierung
	Anwendung der Matrix Q auf b
 *--------------------------*/

vektor 
M_XausHouseholder (matrix Q, vektor D, int m, int n, vektor b)
{
  double Quotient, uTu, uTy, summe;
  int i, k;

  for (i = 0; i < n; i++) {
    uTy = D[i] * b[i];		/* Wende P auf b an.		 */
    uTu = D[i] * D[i];		/* sonst das gleiche wie oben */
    for (k = i + 1; k < m; k++) {
      uTy += Q[k][i] * b[k];
      uTu += Q[k][i] * Q[k][i];
    }
    Quotient = uTy / uTu;
    Quotient += Quotient;

    b[i] -= D[i] * Quotient;
    for (k = i + 1; k < m; k++)
      b[k] -= Q[k][i] * Quotient;
  }

  i = n;			/* Loese Gleichungssystem    */
  while (i--) {			/* Schema Rueckwaertssubsti- */
    summe = b[i];		/* tution.					 */
    for (k = i + 1; k < n; k++)
      summe -= Q[i][k] * b[k];
    b[i] = summe / Q[i][i];
  }
  return b;
}
