#include "mathdefs.h"
#include "mathstd.h"
#include <stdio.h>
#include <math.h>

void free (void *);


double f1 (double x);
double f2 (double x);
double f3 (double x);
double f4 (double x);

struct funk {
  char *Name;
  double a;
  double b;
  double (*f) (double x);
};

struct funk Funktion[]=
{
  {"(1-x)^6", 0.0, 2.0, f1},
  {"exp(-2*cos(x))", 0.0, 2 * M_PI, f2},
  {"sqrt(x)*sin(x)", 0.0, 1.0, f3},
  {"2*x*x*sin(x*x)", 0.0, 1.0, f4}
};
static int Nr;


double 
f1 (double x)
{
  return pow (1 - x, 6);
}

double 
f2 (double x)
{
  return exp (-2 * cos (x));
}

double 
f3 (double x)
{
  return sqrt (x) * sin (x);
}

double 
f4 (double x)
{
  return 2 * x * x * sin (x * x);
}


void 
ROMBERG (double a, double b, double (*f) (double x), int n)
{
  matrix A;
  int i, k;
  double h, summe, vier, j;


  X_Timer ();			/* Stelle Stoppuhr		 */
  h = b - a;
  A = M_Init (n + 1, n + 1);	/* Speicherplatz fÅr A 	 */
  /* Reservieren			 */
  A[0][0] = h * ((*f) (a) + (*f) (b)) / 2.0;	/* Berechne die erste   */
  /* Trapezsumme			 */
  for (i = 1; i <= n; i++) {
    summe = 0.0;

    for (j = a + h / 2.0; j < b; j += h)	/* Berechne die ungera-	 */
      summe += (*f) (j);	/* den Zwischenstellen  */

    h /= 2.0;
    A[i][0] = h * summe + A[i - 1][0] / 2.0;

    vier = 1.0;
    for (k = 1; k <= i; k++) {
      vier *= 4.0;
      A[i][k] = A[i][k - 1] +	/* Und hier die Extra-  */
	(A[i][k - 1] - A[i - 1][k - 1]) /	/* polationsformel		 */
	(vier - 1);
    }
  }

  /* Und nun noch die erwartete Ausgabe */
  h = X_Timer ();

  printf ("Programm ROMBERG\n");
  printf ("----------------\n");
  printf ("                 Rechenzeit : %9.3lf s\n", h);

  printf (" Integrierte Funktion f(x) = %s\n", Funktion[Nr].Name);
  printf (" Auf dem Interval [%lf,%lf]\n", a, b);
  printf (" Ergebnis:\n\n");

  for (i = 0; i <= n; i++) {
    for (k = 0; k <= i; k++)
      printf ("%19.16lf  ", A[i][k]);
    printf ("\n");
  }
  puts ("----------------------------------------------------------\n\n");
  free (A);
}

main ()
{
  for (Nr = 0; Nr < 4; Nr++)
    ROMBERG (Funktion[Nr].a, Funktion[Nr].b,
	     Funktion[Nr].f, 6);
  return 0;
}
