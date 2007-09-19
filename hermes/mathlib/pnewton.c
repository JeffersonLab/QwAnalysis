#include "mathstd.h"
#include "mathdefs.h"
#include <float.h>
void free (void *ptr);
double fabs (double x);


/* ---------------------- * 
   Newtonsches Interpola-
   tionsverfahren
   ---------------------- */

vektor 
P_Newton (vektor a,
	  int grad,
	  vektor x, vektor y)
     /* a  sind die Koeffizienten des Polynoms vom Grad grad. x und y sind
        die StÅtzpunkte des Polynoms */
{
  int i, k;
  double *t;

  t = V_Init (grad + 1);

  for (i = 0; i <= grad; i++) {
    t[i] = y[i];
    k = i;
    while (k--) {
      t[k] = (t[k + 1] - t[k]) / (x[i] - x[k]);
    }
    a[i] = t[0];
  }
  free (t);
  return a;
}

/* -------------------- * Hornerschema fÅr Newtoninterpolation
   -------------------- */

double 
P_NHorner (vektor a, vektor x, int grad, double z)
{
  double p;

  p = a[grad];
  while (grad--)
    p = p * (z - x[grad]) + a[grad];
  return p;
}
