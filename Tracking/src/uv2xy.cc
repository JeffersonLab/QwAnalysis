#include <cstdio>
#include <cmath>

#include "uv2xy.h"

#define PI 3.141592653589793

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
  // Store region, TODO this class should at some point become 'one region only'
  fRegion = region;

  SetOriginXYinUV(0.0, 0.0);
  SetOriginUVinXY(0.0, 0.0);

  // Region 2
  if (region == kRegionID2) {  //jpan: determine working on which region
    double cu = rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->rCos*(-1);
    double su = rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->rSin;
    double cv = rcDETRegion[kPackageUp][kRegionID2][kDirectionV]->rCos;
    double sv = rcDETRegion[kPackageUp][kRegionID2][kDirectionV]->rSin*(-1);

    fOffset[0]   = rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->PosOfFirstWire;
    fOffset[1]   = rcDETRegion[kPackageUp][kRegionID2][kDirectionV]->PosOfFirstWire;
    fWireSpacing = rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->WireSpacing;

    double det = (cu * sv - cv * su);
    if (det) {
      fUV[0][0] =  sv / det;
      fUV[0][1] = -su / det;
      fUV[1][0] = -cv / det;
      fUV[1][1] =  cu / det;
    }
    fXY[0][0] =  cu;
    fXY[0][1] =  su;
    fXY[1][0] =  cv;
    fXY[1][1] =  sv;


  } else if (region == kRegionID3) {

    fOffset[0] = 0.0;
    fOffset[1] = 0.0;
    fWireSpacing = 0.0;

    // Region 3
    double cu = rcDETRegion[kPackageUp][kRegionID3][kDirectionU]->rCos;
    double su = rcDETRegion[kPackageUp][kRegionID3][kDirectionU]->rSin;
    double cv = rcDETRegion[kPackageUp][kRegionID3][kDirectionV]->rCos;
    double sv = rcDETRegion[kPackageUp][kRegionID3][kDirectionV]->rSin;

// This is completely wrong: v is the first column of UV, but used in conjunction
// with the u coordinates in uv2x and uv2y!  E.g. fUV[0][0] and fUV[1][0]
    double det = (su * cv - sv * cu);
    if (det) {
      fUV[0][0] = cv / det;
      fUV[0][1] = cu / det;
      fUV[1][0] = sv / det;
      fUV[1][1] = su / det;
    }

    fXY[0][0] =  su;
    fXY[0][1] = -cu;
    fXY[1][0] = -sv;
    fXY[1][1] =  cv;

//     double det = (cu * sv - cv * su);
//     if (det) {
//       fUV[0][0] = cu / det;
//       fUV[0][1] = cv / det;
//       fUV[1][0] = su / det;
//       fUV[1][1] = sv / det;
//     }
//
//     fXY[0][0] =  sv;
//     fXY[0][1] = -cv;
//     fXY[1][0] = -su;
//     fXY[1][1] =  cu;

  }

}


/**
 * Create a coordinate transformation helper object based on a single angle
 *
 * @param angleUdeg Angle (in degrees) of the U axis
 */
Uv2xy::Uv2xy(const double angleUdeg)
{
  // Reset region (TODO references to regions and wirespacing will be removed)
  fRegion = kRegionIDNull;
  fWireSpacing = 0.0;

  // No offset of the origins
  SetOffset(0.0, 0.0); // old
  SetOriginUVinXY(0.0, 0.0);

  // Convert angles to radians and create the transformation matrices
  fAngleUrad = angleUdeg * PI / 180.0;
  fAngleVrad = PI - fAngleUrad;
  InitializeRotationMatrices();
}


/**
 * Create a coordinate transformation helper object based on two angles.
 *
 * @param angleUdeg Angle (in degrees) of the U axis
 * @param angleVdeg Angle (in degrees) of the V axis
 */
Uv2xy::Uv2xy(const double angleUdeg, const double angleVdeg)
{
  // Reset region (TODO references to regions and wirespacing will be removed)
  fRegion = kRegionIDNull;
  fWireSpacing = 0.0;

  // No offset of the origins
  SetOffset(0.0, 0.0); // old
  SetOriginUVinXY(0.0, 0.0);

  // Convert angles to radians and create the transformation matrices
  fAngleUrad = angleUdeg * PI / 180.0;
  fAngleVrad = angleVdeg * PI / 180.0;
  InitializeRotationMatrices();
}


/**
 * Initialize the rotation matrices UV and XY based on the given angles
 * for the U and V axes
 *
 * @param angleUrad Angle (in radians) of the U axis
 * @param angleVrad Angle (in radians) of the V axis
 */
void Uv2xy::InitializeRotationMatrices()
{
  // Angle for u wires (in radians)
  double cu = cos(fAngleUrad);
  double su = sin(fAngleUrad);

  // Angle for v wires (in radians)
  double cv = cos(fAngleVrad);
  double sv = sin(fAngleVrad);

  // [x,y] to [u,v] transformation
  fXY[0][0] =  sv;
  fXY[0][1] = -cv;
  fXY[1][0] = -su;
  fXY[1][1] =  cu;

  // [u,v] to [x,y] transformation (inverse)
  double det = (cu * sv - su * cv);
  if (det) {
    fUV[0][0] = cu / det;
    fUV[0][1] = cv / det;
    fUV[1][0] = su / det;
    fUV[1][1] = sv / det;
  }
}


// The following functions need to have a shift put in to take into
// account any offset positions between the different directions

double Uv2xy::uv2x(double u, double v)
{
  return fUV[0][0] * (u + fOffset[0] * fXY[0][0] - fWireSpacing - fOriginXYinUV[0])
       + fUV[0][1] * (v + fOffset[1] * fXY[1][0] - fWireSpacing - fOriginXYinUV[1]);
}
double Uv2xy::uv2y(double u, double v)
{
  return fUV[1][0] * (u + fOffset[0] * fXY[0][0] - fWireSpacing - fOriginXYinUV[0])
       + fUV[1][1] * (v + fOffset[1] * fXY[1][0] - fWireSpacing - fOriginXYinUV[1]);
}
double Uv2xy::uv2mx(double u, double v)
{
  return fUV[0][0] * u + fUV[0][1] * v;
}
double Uv2xy::uv2my(double u, double v)
{
  return fUV[1][0] * u + fUV[1][1] * v;
}


double Uv2xy::xy2u(double x, double y)
{
  return fXY[0][0] * (x + fOffset[0] - fOriginUVinXY[0])
       + fXY[0][1] * (y - fOriginUVinXY[1])
       + fWireSpacing;
}
double Uv2xy::xy2v(double x, double y)
{
  return fXY[1][0] * (x + fOffset[1] - fOriginUVinXY[0])
       + fXY[1][1] * (y - fOriginUVinXY[1])
       + fWireSpacing;
}
double Uv2xy::xy2mu(double x, double y)
{
  return fXY[0][0] * x + fXY[0][1] * y;
}
double Uv2xy::xy2mv(double x, double y)
{
  return fXY[1][0] * x + fXY[1][1] * y;
}
