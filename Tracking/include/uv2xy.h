#ifndef UV2XY_H
#define UV2XY_H

// Standard C and C++ headers
#include <iostream>
#include "QwTrackingTree.h"

// Qweak headers
#include "QwTypes.h"
#include "QwHit.h"
#include "Det.h"



/*------------------------------------------------------------------------*//*!

 \class Uv2xy

 \brief Converts between (u,v) and (x,y) coordinates

   This class is to be used to convert the un-orthogonal(word?) u and v
   coordinates of the Region 2 and 3 drift chamber wire planes into
   x and y coordinates.

*//*-------------------------------------------------------------------------*/

///
/// \ingroup QwTrackingAnl
class Uv2xy {

   friend class QwTrackingTreeCombine;
   friend int r2_TrackFit(int Num, QwHit **Hit, double *fit, double *cov, double *chi);

  public:

    Uv2xy(EQwRegionID region);

    double uv2x(double u, double v);
    double uv2y(double u, double v);
    double xy2u(double x, double y);
    double xy2v(double x, double y);

  private:

    EQwRegionID fRegion;

    double R2_uv[2][2];
    double R2_xy[2][2];
    double R3_uv[2][2];
    double R3_xy[2][2];

    double R2_offset[2];
    double R2_wirespacing;

};


#endif // UV2XY_H
