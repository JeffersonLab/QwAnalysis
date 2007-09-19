#include <stdio.h>
#include <math.h>


#define Normierung sqrt(2.0 * M_PI)
#define MaxV 2.0E-3
#define Gauss(t) exp(-0.5*(t)*(t))

double 
Gauss_Verteilung (double Mittelwert,
		  double Standardabweichung,
		  double Value)
{
  int large = 0;
  double t, dt, dV = 0.0, V, y, ly, six = 1.0 / 6.0;

  if (Value < 0.5) {
    Value = 1.0 - Value;
    large = 1;
  }
  if (Value > 1.0)
    return 0;

  Value *= Normierung;

  for (V = 0.5 * Normierung, t = 0.0, dt = MaxV, y = 1.0;
       V < Value; t += dt, V += dV) {
    ly = y;
    do {			/* Schrittweitensteuerung */
      dV = six * ((y = Gauss (t + dt)) + 4.0 * Gauss (t + 0.5 * dt) + ly) * dt;
      /* Auch als Simpsonregel bekannt */
      if (dV < MaxV) {
	dt *= 1.1 * MaxV / dV;
	continue;
      }
    }
    while (0);

  }
  t -= dt * (V - Value) / dV;

  if (large)
    t = -t;

  return t * Standardabweichung + Mittelwert;
}
