#ifndef UV2XY_H
#define UV2XY_H

// Standard C and C++ headers
#include <iostream>

// Qweak headers
#include "QwTypes.h"


class QwHit;

/*------------------------------------------------------------------------*//*!

 \class Uv2xy
 \ingroup QwTrackingAnl

 \brief Converts between (u,v) and (x,y) coordinates

   This class is to be used to convert the un-orthogonal(word?) u and v
   coordinates of the Region 2 and 3 drift chamber wire planes into
   x and y coordinates.

*//*-------------------------------------------------------------------------*/
class Uv2xy {

  public:

    Uv2xy(EQwRegionID region);
    Uv2xy(double angle, double offsetU = 0.0, double offsetV = 0.0, double wirespacing = 0.0);

    void PrintUV() {
      std::cout << "UV = [" << uv[0][0] << "," << uv[0][1] << ";";
      std::cout << uv[1][0] << "," << uv[1][1] << std::endl;
    };
    void PrintXY() {
      std::cout << "XY = [" << xy[0][0] << "," << xy[0][1] << ";";
      std::cout << xy[1][0] << "," << xy[1][1] << std::endl;
    };
    void Print() {
      PrintUV();
      PrintXY();
    };

    double uv2x(double u, double v);
    double uv2y(double u, double v);
    double xy2u(double x, double y);
    double xy2v(double x, double y);

    double uv[2][2];
    double xy[2][2];

    double fOffset[2];
    double fWirespacing;

  friend class QwTrackingTreeCombine;

  protected:

  private:

    EQwRegionID fRegion;

};


#endif // UV2XY_H
