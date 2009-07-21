#include <cstdio>
#include <cmath>

#include "uv2xy.h"


#include "Det.h"
extern Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];

/* For Region 2, the x wires actually measure a coordinate in the lab y direction.
   So for the reconstruction of tracks in R2, I will use a coordinate system
   in which x is the lab's y coordinate, y is the lab's x direction, and z is
   the lab's negative z direction. This is the reason for the negative signs
   on some of the variables which follow.  This allows me to leave the wire
   rCos and rSin values as calculated in the lab frame.  Also note that
   the u and v coordinates had to be shifted a bit due to the origin being
   placed at one wire spacing below the midpoint of the first x-wire.
*/
//__________________________________________________________________
Uv2xy::Uv2xy(EQwRegionID region)
{
  double uc,us,vc,vs;
  double det;

  // Store region, TODO this class should at some point become 'one region only'
  fRegion = region;

  // Region 2
  if(region == kRegionID2){  //jpan: determine working on which region
  uc = rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->rCos*(-1);
  us = rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->rSin;
  vc = rcDETRegion[kPackageUp][kRegionID2][kDirectionV]->rCos;
  vs = rcDETRegion[kPackageUp][kRegionID2][kDirectionV]->rSin*(-1);

  fOffset[0]   = rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->PosOfFirstWire;
  fOffset[1]   = rcDETRegion[kPackageUp][kRegionID2][kDirectionV]->PosOfFirstWire;
  fWirespacing = rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->WireSpacing;

  det = ( uc * vs - vc * us );
  if( det ) {
    uv[0][0] =  vs / det;
    uv[0][1] = -us / det;
    uv[1][0] = -vc / det;
    uv[1][1] =  uc / det;
  }
  xy[0][0] =  uc;
  xy[0][1] =  us;
  xy[1][0] =  vc;
  xy[1][1] =  vs;

  }

  else if(region == kRegionID3){
  // Region 3
  uc = rcDETRegion[kPackageUp][kRegionID3][kDirectionU]->rCos;
  us = rcDETRegion[kPackageUp][kRegionID3][kDirectionU]->rSin;
  vc = rcDETRegion[kPackageUp][kRegionID3][kDirectionV]->rCos;
  vs = rcDETRegion[kPackageUp][kRegionID3][kDirectionV]->rSin;

  det = ( us * vc - vs * uc );
  if( det ) {
    uv[0][0] = vc / det;
    uv[0][1] = uc / det;
    uv[1][0] = vs / det;
    uv[1][1] = us / det;
  }

  xy[0][0] =  us;
  xy[0][1] = -uc;
  xy[1][0] = -vs;
  xy[1][1] =  vc;

  }

}

Uv2xy::Uv2xy(double angleU, double offsetU, double offsetV, double wirespacing)
{
  // Wire offsets and wire spacing
  fOffset[0] = offsetU;
  fOffset[1] = offsetV;
  fWirespacing = wirespacing;

  // Angle for U wires
  double uc = cos(angleU);
  double us = sin(angleU);

  // Angle for V wires
  double angleV = 3.141592653589793 / 2.0 - angleU;
  double vc = cos(angleV);
  double vs = sin(angleV);

  // (x,y) to (u,v) transformation
  xy[0][0] =  us;
  xy[0][1] = -uc;
  xy[1][0] = -vs;
  xy[1][1] =  vc;

  // Invert: (u,v) to (x,y) transformation
  double det = (us * vc - vs * uc);
  if (det) {
    uv[0][0] = vc / det;
    uv[0][1] = uc / det;
    uv[1][0] = vs / det;
    uv[1][1] = us / det;
  }


}


//__________________________________________________________________
// The following functions need to have a shift put in to take into
// account any offset positions between the different directions
double Uv2xy::uv2x(double u, double v)
{
  if (fRegion == kRegionID2)
    return (u + fOffset[0] * xy[0][0] - fWirespacing) * uv[0][0]
         + (v + fOffset[1] * xy[1][0] - fWirespacing) * uv[0][1];
  else
    return u * uv[0][0] + v * uv[0][1];
}

//__________________________________________________________________
double Uv2xy::uv2y(double u, double v)
{
  if (fRegion == kRegionID2)
    return (u + fOffset[0] * xy[0][0] - fWirespacing) * uv[1][0]
         + (v + fOffset[1] * xy[1][0] - fWirespacing) * uv[1][1];
  else
    return u * uv[1][0] + v * uv[1][1];
}

//__________________________________________________________________
double Uv2xy::xy2u(double x, double y)
{
  if (fRegion == kRegionID2)
    return xy[0][0] * (x + fOffset[0]) + xy[0][1] * y + fWirespacing;
  else
    return xy[0][0] * x + xy[0][1] * y;
}

//__________________________________________________________________
double Uv2xy::xy2v(double x, double y)
{
  if (fRegion == kRegionID2)
    return xy[1][0] * (x + fOffset[1]) + xy[1][1] * y + fWirespacing;
  else
    return xy[1][0] * x + xy[1][1] * y;
}
//__________________________________________________________________

