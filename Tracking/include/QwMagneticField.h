//
//  date: Sun Nov 29 22:28:23 CST 2009
//
//  jpan: QTOR field map reading class.
//        Modified from the Qweak geant4 simulation code
//

#ifndef _QwMagneticField_h_
#define _QwMagneticField_h_

// System headers
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>

// ROOT headers
#include <TVector3.h>
#include <TRotation.h>

// Qweak headers
#include "QwUnits.h"

// Forward declarations
template <class value_t, unsigned int value_n> class QwInterpolator;

// Field map storage data type
typedef float field_t;

/**
 *  \class QwMagneticField
 *  \ingroup QwTracking
 *  \brief Magnetic field map object
 *
 * This class implements a magnetic field object using the QwInterpolator class.
 *
 * Five magnetic field components are stored (larger memory usage, computationally
 * more intensive on creation, but faster access by avoiding trigoniometric
 * operations).
 *
 * Access to the field components is possible using two functions:
 * \code
 * field->GetCartesianFieldValue(double point_xyz[3], double field_xyz[3]);
 * field->GetCylindricalFieldValue(doubl point_xyz[3], double field_rfz[3]);
 * \endcode
 *
 * Currently no function exists that takes cylindrical coordinates.
 */
class QwMagneticField {

  public:

    /// \brief Default constructor
    QwMagneticField();
    /// \brief Virtual destructor
    virtual ~QwMagneticField();

    /// Set the field scaling factor
    void SetFieldScalingFactor(const double fieldscalingfactor)
      { fFieldScalingFactor = fieldscalingfactor; };
    /// Get the field scaling factor
    const double GetFieldScalingFactor() const
      { return fFieldScalingFactor; };

    /// Set the field rotation around z (with QwUnits)
    void SetRotation(const double rotation) {
      fRotationDeg = rotation / Qw::deg; // unit conversion
      fRotationRad = rotation / Qw::rad;
      fRotationCos = cos(rotation);
      fRotationSin = sin(rotation);
    };
    /// Get the field rotation around z (with QwUnits)
    const double GetRotation() const
      { return fRotationRad * Qw::rad; };

    /// Set the field translation along z
    void SetTranslation(const double translation)
      { fTranslation = translation; };
    /// Get the field translation along z
    const double GetTranslation() const
      { return fTranslation; };

    /// Get the cartesian components of the field value
    void GetCartesianFieldValue(const double point[3], double field[3]) const {
      double field_xyzrf[5];
      GetFieldValue(point, field_xyzrf);
      field[0] = field_xyzrf[0]; // x
      field[1] = field_xyzrf[1]; // y
      field[2] = field_xyzrf[2]; // z
    };
    /// Get the cylindrical components of the field value
    void GetCylindricalFieldValue(const double point[3], double field[3]) const {
      double field_xyzrf[5];
      GetFieldValue(point, field_xyzrf);
      field[0] = field_xyzrf[3]; // r
      field[1] = field_xyzrf[4]; // phi
      field[2] = field_xyzrf[2]; // z
    };

    /// \brief Read a field map input file
    const bool ReadFieldMapFile(const std::string filename);
    /// \brief Read a field map input gzip file
    const bool ReadFieldMapZip(const std::string filename);
    /// \brief Read the field map input stream
    const bool ReadFieldMap(std::istream& input);

  private:

    /// \brief Get the field value
    void GetFieldValue(const double point[3], double field[5]) const;


    /// Field interpolator object
    QwInterpolator<field_t,5> *fField;

    /// Units of field map
    double fUnitBfield;

    /// Field scaling factor (BFIL)
    double fFieldScalingFactor;

    /// Field rotation and translation with respect to the z axis
    // Defined as rotation/translation of the map in the standard coordinate
    // system: rotation of +22.5 deg means that x in the map x is at +22.5 deg,
    // likewise a translation of +5 cm means that the zero in the map is at +5 cm
    // in the standard coordinate system.
    double fRotationDeg, fRotationRad, fRotationCos, fRotationSin;
    double fTranslation;

}; // class QwMagneticField

#endif // _QwMagneticField_h_
