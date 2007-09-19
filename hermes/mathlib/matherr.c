#include <stdio.h>
#include <stdlib.h>
#include "mathstd.h"
#include "mathdefs.h"

/*--------------------------*
  Funktion Mathe-Fehler:
		   Gibt eine Fehlermeldung
		   aus.
 *--------------------------*/


int 
Matherror (int errnr)
{
  switch (errnr) {
    case M_NOMEM:puts ("Der Speicherplatz ist nicht ausreichend");
    break;
  case M_NOMUL:
    puts ("Die Matrixmultiplikation ist ungueltig");
    break;
  case M_PIVOT:
    puts ("Pivotsuche ist fehlgeschlagen");
    break;
  case M_STUETZ:
    puts ("Im Interpolationsverfahren treten 2 gleiche Stuetzstellen auf!");
    break;
  }
  return 0;
}
