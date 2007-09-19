#include <stdio.h>
#include <math.h>
#include "mathdefs.h"
#include "mathstd.h"


double 
Simpson (double a, double b, double (*f) (double x))
{
  return (b - a) * ((*f) (a) + 4. * (*f) ((a + b) / 2.) + (*f) (b)) / 6.;
}

double 
Pulcherima (double a, double b, double (*f) (double x))
{
  double h;

  h = (b - a) / 3.;

  return (b - a) * ((*f) (a) + 3. * (*f) (a + h) +
		    (*f) (b) + 3. * (*f) (b - h)) / 8.;
}

double 
f (double x)
{
  return 1 / (1 + x);
}

main ()
{
  double h, a, pulch, simps, ex;

  ex = log (2);

  for (h = 1; h >= 0.1; h /= 2.) {
    simps = pulch = 0.0;

    for (a = 0; a < 1; a += h) {
      simps += Simpson (a, a + h, f);
      pulch += Pulcherima (a, a + h, f);
    }
    printf ("h = %4.2lf: Exakter Wert :     %10.8lf\n", h, ex);
    printf ("            nach Simpson :     %10.8lf  rel. Fehler %6.4lf%%\n", simps, 100. * fabs (ex - simps) / ex);
    printf ("            Pulcherima   :     %10.8lf  rel. Fehler %6.4lf%%\n\n", pulch, 100. * fabs (ex - pulch) / ex);
  }
  return 0;
}
