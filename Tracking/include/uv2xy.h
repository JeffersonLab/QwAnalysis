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

   The transformation formulas with identical origins are
   \f[
     x = u \cdot \cos \theta_u + v \cdot \cos \theta_v, \qquad
     y = u \cdot \sin \theta_u + v \cdot \sin \theta_v,
   \f]
   \f[
     u = x \cdot \sin \theta_v - y \cdot \cos \theta_v, \qquad
     v = - x \cdot \sin \theta_u + y \cdot \cos \theta_u,
   \f]
   with \f$ \theta_u \f$ the angle of the u wires and \f$ \theta_v \f$
   the angle of the v wires with respect to the x axis.
   When \f$ \theta_u \f$ and \f$ \theta_v \f$ are not orthogonal, the
   transformation matrices are not normalized.

   The origins are not identical and an offset has to be included.

*//*-------------------------------------------------------------------------*/
class Uv2xy {

  public:

    /// Create a transformation helper by region
    Uv2xy(EQwRegionID region);
    /// Create a transformation helper from one angle
    Uv2xy(const double angleUdeg);
    /// Create a transformation helper from two angles
    Uv2xy(const double angleUdeg, const double angleVdeg);

    /// \brief Set the wire spacing (perpendicular distance between wires)
    void SetWireSpacing (const double spacing) {
      fWireSpacing = spacing;
    };
    /// \brief Set the offset (origin of UV frame in XY coordinates)
    void SetOffset(const double offsetX, const double offsetY) {
      fOffset[0] = offsetX;
      fOffset[1] = offsetY;
    };

    /// \brief Set the origin of the UV frame in the XY frame
    void SetOriginUVinXY(const double originX, const double originY) {
      fOriginUVinXY[0] = originX;
      fOriginUVinXY[1] = originY;
      fOriginXYinUV[0] = xy2u (0.0, 0.0);
      fOriginXYinUV[1] = xy2v (0.0, 0.0);
    };
    /// \brief Set the origin of the XY frame in the UV frame
    void SetOriginXYinUV(const double originU, const double originV) {
      fOriginXYinUV[0] = originU;
      fOriginXYinUV[1] = originV;
      fOriginUVinXY[0] = uv2x (0.0, 0.0);
      fOriginUVinXY[1] = uv2y (0.0, 0.0);
    };
    /// \brief Shift origin of the UV frame by XY coordinates
    void ShiftOriginUVinXY(const double shiftX, const double shiftY) {
      fOriginUVinXY[0] += shiftX;
      fOriginUVinXY[1] += shiftY;
      fOriginXYinUV[0] = xy2u (0.0, 0.0);
      fOriginXYinUV[1] = xy2v (0.0, 0.0);
    };
    /// \brief Shift origin of the XY frame by UV coordinates
    void ShiftOriginXYinUV(const double shiftU, const double shiftV) {
      fOriginXYinUV[0] += shiftU;
      fOriginXYinUV[1] += shiftV;
      fOriginUVinXY[0] = uv2x (0.0, 0.0);
      fOriginUVinXY[1] = uv2y (0.0, 0.0);
    };

    /// \brief Print UV transformation matrix
    void PrintUV() {
      std::cout << "UV = [" << fUV[0][0] << ", " << fUV[0][1] << "; ";
      std::cout << fUV[1][0] << ", " << fUV[1][1] << "]" << std::endl;
    };
    /// \brief Print XY transformation matrix
    void PrintXY() {
      std::cout << "XY = [" << fXY[0][0] << ", " << fXY[0][1] << "; ";
      std::cout << fXY[1][0] << ", " << fXY[1][1] << "]" << std::endl;
    };
    /// \brief Print UV and XY transformation matrices
    void Print() {
      PrintUV();
      PrintXY();
      std::cout << "Offset = [" << fOffset[0] << ", " << fOffset[1] << "]" << std::endl;
    };

    /// \brief Transform from [u,v] to x
    double uv2x(double u, double v);
    /// \brief Transform from [u,v] to y
    double uv2y(double u, double v);
    /// \brief Transform from [u,v] to mx
    double uv2mx(double u, double v);
    /// \brief Transform from [u,v] to my
    double uv2my(double u, double v);

    /// \brief Transform from [x,y] to u
    double xy2u(double x, double y);
    /// \brief Transform from [x,y] to v
    double xy2v(double x, double y);
    /// \brief Transform from [x,y] to mu
    double xy2mu(double x, double y);
    /// \brief Transform from [x,y] to mv
    double xy2mv(double x, double y);


    double fUV[2][2];	///< Transformation matrix UV from [u,v] to [x,y],
                        ///  which satisifies \f$ [x,y] = UV * [u,v] \f$
    double fXY[2][2];	///< Transformation matrix XY from [x,y] to [u,v]
                        ///  which satisifies \f$ [u,v] = XY * [x,y] \f$

    double fOffset[2];		///< Offset in [x,y] coordinates
    double fWireSpacing;	///< Wirespacing in the u/v direction

  friend class QwTrackingTreeCombine;

  protected:

  private:

    EQwRegionID fRegion;

    double fAngleUrad;		///< Angle of the U direction in radians
    double fAngleVrad;		///< Angle of the V direction in radians

    double fOriginXYinUV[2];	///< Origin of the XY system in UV coordinates
    double fOriginUVinXY[2];	///< Origin of the UV system in XY coordinates

    void InitializeRotationMatrices();

}; // class Uv2xy

#endif // UV2XY_H
