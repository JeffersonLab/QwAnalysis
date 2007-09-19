
long Super (void *stack);

/*--------------------------*
  Funktion Timer:
		   Berechnet die
		   Zeit zwischen 2
		   Aufrufen in sek.
 *--------------------------*/


double 
X_Timer (void)
{
  static long last;
  long now;
  long ssp;
  double ret;

  ssp = Super ((void *) 0L);

  now = *(long *) 0x4ba;

  (void) Super ((void *) ssp);

  ret = (double) (now - last) / 200.0;

  last = now;

  return ret;
