#ifndef UV2XY_H
#define UV2XY_H

// Standard C and C++ headers
#include <iostream>
#include "tree.h"

// Qweak headers
#include "QwTypes.h"
#include "Hit.h"
#include "Det.h"



/*------------------------------------------------------------------------*//*!

 \class Uv2xy

 \brief Converts between (u,v) and (x,y) coordinates

   This class is to be used to convert the un-orthogonal(word?) u and v
   coordinates of the Region 2 and 3 drift chamber wire planes into
   x and y coordinates.

*//*-------------------------------------------------------------------------*/

class Uv2xy {

   friend class treecombine;
   friend int r2_TrackFit(int Num, Hit **Hit, double *fit, double *cov, double *chi);

  public:

    Uv2xy();
    ~Uv2xy();

    double uv2x(double u, double v, EQwRegionID region);
    double uv2y(double u, double v, EQwRegionID region);
    double xy2u(double x, double y, EQwRegionID region);
    double xy2v(double x, double y, EQwRegionID region);

  private:

    double R2_uv[2][2];
    double R2_xy[2][2];
    double R3_uv[2][2];
    double R3_xy[2][2];

    double R2_offset[2];
    double R2_wirespacing;

};


#endif // UV2XY_H
