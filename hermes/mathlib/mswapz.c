#include <stdlib.h>
#include "mathstd.h"
#include "mathdefs.h"
#include <string.h>


/*--------------------------*
  Funktion : Zeilenvertauschen
  			Die zeile1 und
  			zeile2 werden vertauscht
 *--------------------------*/

void 
M_SwapZ (matrix A, int n, int m, int zeile1, int zeile2, int start)
{
  static double helpbuf[20];
  double help;
  vektor zeile;

  if (zeile1 != zeile2 && zeile1 < n && zeile2 < n) {
    if (start) {		/* Start > 0 ? D.h. Soll nicht ab Anfang
				   vertauscht */
      /* werden ? */
      if (m <= 20) {
	memcpy (helpbuf, *(A + zeile1) + start, sizeof (double) * (m - start));
	memcpy (*(A + zeile1) + start, *(A + zeile2) + start, sizeof (double) * (m - start));
	memcpy (*(A + zeile2) + start, helpbuf, sizeof (double) * (m - start));
      }
      else {
	while (m-- > start) {
	  help = *(*(A + zeile1) + m);
	  *(*(A + zeile1) + m) = *(*(A + zeile2) + m);
	  *(*(A + zeile2) + m) = help;
	}
      }
    }
    else {			/* Vertausche nur Pointer */
      zeile = *(A + zeile1);
      *(A + zeile1) = *(A + zeile2);
      *(A + zeile2) = zeile;
    }
  }
  return;
}

