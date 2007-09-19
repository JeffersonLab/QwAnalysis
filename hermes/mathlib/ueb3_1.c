#include <stdio.h>
#include "mathstd.h"
#include "mathdefs.h"
#include <linea.h>
#include <tos.h>

double M1[2][2] =
{
  {1.0, 0.5},
  {0.5, 0.5}
};
double b1[2] =
{1.0, 1.0};


main ()
{
  double i1, i2, i3, i4, i5, i6;
  double feld[2], i;
  int fh;
  PATTERN fill;
  matrix A;
  vektor b;

  linea_init ();
  set_fg_bp (1);
  set_ln_mask (0xFFFF);
  set_clip (0, 0, 639, 399, 0);
  hide_mouse ();
  set_pattern (fill, 1, 0);
  fill[0] = 0x0000;
  set_wrt_mode (REPLACE);
  filled_rect (0, 0, 639, 399);

  draw_line (0, 390, 639, 390);
  draw_line (320, 0, 320, 399);

  for (i = -0.5; i <= 0.5; i += 0.1)
    draw_line (320 + (int) (i * 300.0), 386, 320 + (int) (i * 300.0), 394);
  for (i = 1.5; i <= 2.5; i += 0.1)
    draw_line (316, 750 - (int) (i * 300), 324, 750 - (int) (i * 300));

  A = M_Init (2, 2);
  b = V_Init (2);

  for (i1 = -1; i1 <= 1; i1 += 1.0) {
    for (i2 = -1; i2 <= 1; i2 += 1.0) {
      for (i3 = -1; i3 <= 1; i3 += 1.0) {
	for (i4 = -1; i4 <= 1; i4 += 1.0) {
	  for (i5 = -1; i5 <= 1; i5 += 1.0) {
	    for (i6 = -1; i6 <= 1; i6 += 1.0) {
	      M_Fill (A, *M1, 2, 2);
	      V_Fill (b, b1, 2);
	      A[0][0] *= (1 + i1 * 1e-2);
	      A[1][0] *= (1 + i2 * 1e-2);
	      A[0][1] *= (1 + i3 * 1e-2);
	      A[1][1] *= (1 + i4 * 1e-2);
	      b[0] *= (1 + i5 * 3e-2);
	      b[1] *= (1 + i6 * 3e-2);
	      M_Gauss (feld, A, 2, b);
	      draw_line (318 + (int) (feld[0] * 300), 748 - (int) (feld[1] * 300),
		  322 + (int) (feld[0] * 300), 752 - (int) (feld[1] * 300));
	      draw_line (322 + (int) (feld[0] * 300), 748 - (int) (feld[1] * 300),
		  318 + (int) (feld[0] * 300), 752 - (int) (feld[1] * 300));
	    }
	  }
	}
      }
    }
  }
  fh = Fcreate ("PIC.PIC", 0);
  Fwrite (fh, 32000, Physbase ());
  Fclose (fh);

  {
    int getch (void);
    getch ();
  }

  return 0;
}
