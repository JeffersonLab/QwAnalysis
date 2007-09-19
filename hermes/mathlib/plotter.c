/* ------------------------ * Modul Plotter:

FÅr die Mathe-Library im Rahmen der Vorlesung zur EinfÅhrung in die Numerik
   von PD. Dr. Bader

------------------------ */

#include <tos.h>
#include <math.h>
#include "mathstd.h"
void draw (int, int, int, int, int);
int printf (const char *format,...);
unsigned *screenbase;

int initialized = 0;
int lx, ly;
double xa, xe, ya, ye;
int Sx, Sw, Sy, Sh;

#define _X_(x) (Sx + (int)((double)Sw * ((x)-xa) / (xe-xa)))
#define _Y_(y) (Sy+Sh - (int)((double)Sh * ((y)-ya) / (ye-ya)))

void 
_Pl_Line (int x1, int y1, int x2, int y2)
{
  if (x1 < Sx || x2 < Sx || x1 > Sx + Sw || x2 > Sx + Sw ||
      y1 < Sy || y2 < Sy || y1 > Sy + Sh || y2 > Sy + Sh)
    return;
  else
    draw (x1, y1, x2, y2, 0);
}

void 
Pl_Line (double x1, double y1, double x2, double y2)
{
  _Pl_Line (_X_ (x1), _Y_ (y1), _X_ (x2), _Y_ (y2));
}

void 
Pl_draw (int mode, double x, double y)
{
  int ax, ay;
  if (mode) {			/* Startwerte setzen */
    lx = _X_ (x);
    ly = _Y_ (y);
    if (mode == 2)		/* Und ersten Punkt zeichnen */
      _Pl_Line (lx, ly, lx, ly);
  }
  else {
    ax = _X_ (x);
    ay = _Y_ (y);
    _Pl_Line (ax, ay, lx, ly);
    lx = ax;
    ly = ay;
  }
}

int 
Pl_System (double Xa, double Xe, double Ya, double Ye,
	   double mx, double my,
	   int sx, int sw, int sy, int sh,
	   int clear)
{
  double x, h;

  if (Xa >= Xe || Ya >= Ye)
    return 1;

  Sx = sx;
  Sy = sy;
  Sw = sw;
  Sh = sh;

  if (!initialized) {
    initialized = 1;
    screenbase = Physbase ();
  }
  xa = Xa;
  xe = Xe;
  ya = Ya;
  ye = Ye;

  if (clear)
    Cconws ("\033E");
  Pl_Line (0.0, ya, 0.0, ye);
  Pl_Line (xa, 0.0, xe, 0.0);

  for (x = xa; x <= xe; x += mx) {
    h = x - fmod (x, mx);
    Pl_Line (h, (ye - ya) / 80, h, (ya - ye) / 80);
  }
  for (x = ya; x <= ye; x += my) {
    h = x - fmod (x, my);
    Pl_Line ((xe - xa) / 80, h, (xa - xe) / 80, h);
  }
  return 0;
}

void 
Pl_Plot (double a, double b, double c, double (*f) (double x))
{				/* start    ende      schrittweite  */
  double x;
  Pl_draw (1, a, (*f) (a));
  for (x = a + c; x <= b; x += c)
    Pl_draw (0, x, (*f) (x));
}
