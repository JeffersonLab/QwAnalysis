/*!
 * \file   uv2xy.cc
 * \brief  A helper object for transformation between [u,v] and [x,y] frames
 *
 * \author Wouter Deconinck
 * \date   2009-12-05
 */

#include "uv2xy.h"

// System headers
#include <cstdio>
#include <cmath>

// Qweak headers
#include "QwLog.h"

/* For Region 2, the x wires actually measure a coordinate in the lab y direction.
   So for the reconstruction of tracks in R2, I will use a coordinate system
   in which x is the lab's y coordinate, y is the lab's x direction, and z is
   the lab's negative z direction. This is the reason for the negative signs
   on some of the variables which follow.  This allows me to leave the wire
   rCos and rSin values as calculated in the lab frame.  Also note that
   the u and v coordinates had to be shifted a bit due to the origin being
   placed at one wire spacing below the midpoint of the first x-wire.
*/

/**
 * Create a coordinate transformation helper object based on a single angle
 *
 * @param angleU Angle (in radians) of the U axis
 */
Uv2xy::Uv2xy(const double angleU)
{
  // Reset wire spacing
  SetWireSpacing(0.0);

  // No offset of the origins
  SetOffset(0.0, 0.0);
  SetOriginUVinXY(0.0, 0.0);

  // Convert angles to radians and create the transformation matrices
  double angleV = Qw::pi - angleU;
  // Ensure correct handedness
  if (fmod(angleV,2.0*Qw::pi) - fmod(angleU,2.0*Qw::pi) < 0.0) angleV += Qw::pi;
  // Set the angles
  SetAngleUVinXY(angleU, angleV);
  InitializeRotationMatrices();
}


/**
 * Create a coordinate transformation helper object based on two angles.
 *
 * @param angleUrad Angle (in radians) of the U axis
 * @param angleVrad Angle (in radians) of the V axis
 */
Uv2xy::Uv2xy(const double angleUrad, const double angleVrad)
{
  // Reset wire spacing
  SetWireSpacing(0.0);

  // No offset of the origins
  SetOffset(0.0, 0.0);
  SetOriginUVinXY(0.0, 0.0);

  // Convert angles to radians and create the transformation matrices
  SetAngleUVinXY(angleUrad, angleVrad);
  InitializeRotationMatrices();
}



void Uv2xy::SetAngleUVinXY(const double angleUrad, const double angleVrad)
{
  // Check whether U and V angle are sufficiently different
  if (fabs(angleUrad - angleVrad) < 1.0 * Qw::deg) {
    QwWarning << "uv2xy transformation cannot be created with angles "
              << angleUrad/Qw::deg << " deg and "
              << angleVrad/Qw::deg << " deg." << QwLog::endl;
    QwWarning << "uv2xy transformation will not transform anything." << QwLog::endl;
    fAngleUrad = 0.0;
    fAngleVrad = Qw::pi/2;
  } else {
    fAngleUrad = angleUrad;
    fAngleVrad = angleVrad;
  }
}


/**
 * Initialize the rotation matrices UV and XY based on the stored angles
 * for the U and V axes.
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
  fXY[0][0] = cu;
  fXY[0][1] = su;
  fXY[1][0] = cv;
  fXY[1][1] = sv;

  // [u,v] to [x,y] transformation (inverse)
  double det = (fXY[0][0] * fXY[1][1] - fXY[0][1] * fXY[1][0]);
  if (fabs(det) > 0.0) {
    fUV[0][0] =  fXY[1][1] / det;
    fUV[0][1] = -fXY[0][1] / det;
    fUV[1][0] = -fXY[1][0] / det;
    fUV[1][1] =  fXY[0][0] / det;
  }
  // Note: det should be +1 or -1
  if (det < 0.0) {
    QwVerbose << "uv2xy transformation from right- to left-handed system." << QwLog::endl;
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
