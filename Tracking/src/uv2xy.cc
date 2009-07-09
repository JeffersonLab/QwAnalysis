#include <cstdio>
#include "uv2xy.h"
#include <math.h>


extern Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];

/* For Region 2, the x wires actually measure a coordinate in the lab y direction.  So for the reconstruction of tracks in R2, I will use a coordinate system in which x is the lab's y coordinate, y is the lab's x direction, and z is the lab's negative z direction. This is the reason for the negative signs on some of the variables which follow.  This allows me to leave the wire rCos and rSin values as calculated in the lab frame.  Also note that the u and v coordinates had to be shifted a bit due to the origin being placed at one wire spacing below the midpoint of the first x-wire.

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

  R2_offset[0]   = rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->PosOfFirstWire;
  R2_offset[1]   = rcDETRegion[kPackageUp][kRegionID2][kDirectionV]->PosOfFirstWire;
  R2_wirespacing = rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->WireSpacing;

  det = ( uc * vs - vc * us );
  if( det ) {
    R2_uv[0][0] =  vs / det;
    R2_uv[0][1] = -us / det;
    R2_uv[1][0] = -vc / det;
    R2_uv[1][1] =  uc / det;
  }
  R2_xy[0][0] =  uc;
  R2_xy[0][1] =  us;
  R2_xy[1][0] =  vc;
  R2_xy[1][1] =  vs;

  }

  else if(region == kRegionID3){
  // Region 3
  uc = rcDETRegion[kPackageUp][kRegionID3][kDirectionU]->rCos;
  us = rcDETRegion[kPackageUp][kRegionID3][kDirectionU]->rSin;
  vc = rcDETRegion[kPackageUp][kRegionID3][kDirectionV]->rCos;
  vs = rcDETRegion[kPackageUp][kRegionID3][kDirectionV]->rSin;

  det = ( us * vc - vs * uc );
  if( det ) {
    R3_uv[0][0] =  vc / det;
    R3_uv[0][1] = uc / det;
    R3_uv[1][0] = vs / det;
    R3_uv[1][1] =  us / det;
  }

  R3_xy[0][0] =  us;
  R3_xy[0][1] =  -uc;
  R3_xy[1][0] =  -vs;
  R3_xy[1][1] =  vc;

  }

}

//__________________________________________________________________
// The following functions need to have a shift put in to take into
// account any offset positions between the different directions
double Uv2xy::uv2x(double u, double v)
{
  if (fRegion == kRegionID2)
    return (u + R2_offset[0] * R2_xy[0][0]-R2_wirespacing) * R2_uv[0][0]
         + (v + R2_offset[1] * R2_xy[1][0]-R2_wirespacing) * R2_uv[0][1];
  else
    return u * R3_uv[0][0] + v * R3_uv[0][1];
}

//__________________________________________________________________
double Uv2xy::uv2y(double u, double v)
{
  if (fRegion == kRegionID2)
    return (u + R2_offset[0] * R2_xy[0][0] - R2_wirespacing) * R2_uv[1][0]
         + (v + R2_offset[1] * R2_xy[1][0]-R2_wirespacing) * R2_uv[1][1];
  else
    return u * R3_uv[1][0] + v * R3_uv[1][1];
}

//__________________________________________________________________
double Uv2xy::xy2u(double x, double y)
{
  if (fRegion == kRegionID2)
    return R2_xy[0][0] * (x + R2_offset[0]) + R2_xy[0][1] * y + R2_wirespacing;
  else
    return R3_xy[0][0] * x + R3_xy[0][1] * y;
}

//__________________________________________________________________
double Uv2xy::xy2v(double x, double y)
{
  if (fRegion == kRegionID2)
    return R2_xy[1][0] * (x + R2_offset[1]) + R2_xy[1][1] * y + R2_wirespacing;
  else
    return R3_xy[1][0] * x + R3_xy[1][1] * y;
}
//__________________________________________________________________

