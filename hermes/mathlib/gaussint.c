/* ---------------------------------------- *\ Turbo-C		V 1.1						*
   Versuch der Reihenentwicklung von		* I(exp(-x^2/2))							*
   = x - x^3/3 + x^5/10 - x^7/42 + ...	* Untersuchung des
   Konvergenzverhaltens.	* *

*				\ *----------------\ Bericht:
   G U T 		  * --------
   ---------------/ Die Reihe kovergiert fr kleine x	*
   -wie zu	erwarten- gut. Fr x > 6	* d.h. mehr als 90
   Folgeglieder wird	* das Ergebnis ungenau (Ausl”schung).	* Die
   Rechenzeiten liegen zwischen	* 0.005(|x| < .5) und 0.06 (|x| > 5) 	*
   Sekunden.							* 6.11.1989	*
   \* ---------------------------------------- */

#include <math.h>
#include <stdio.h>
#include "mathstd.h"

#define Genau 1e-18

#define abs(x) (((x)<0)?-(x):x)

double 
Gauss_Integral (double x)
{
  double s, g;
  int i;

  x /= sqrt (2.0);
  s = x;
  g = x;

  for (i = 0; abs (g) > Genau && i < 90; i++, s += g)
    g *= -(double) (i + i + 1) / (double) (i * i * 2 + 5 * i + 3) * x * x;
  s /= sqrt (M_PI);
  if (i == 90) {
    if (x > 0.0)
      s = .5;
    else
      s = -.5;
  }
  s += 0.5;
  return s;
}
