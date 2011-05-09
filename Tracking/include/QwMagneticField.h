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
#include "QwOptions.h"
#include "QwInterpolator.h"

// Forward declarations
template <class value_t, unsigned int value_n> class QwInterpolator;

// Definitions
// - number of field component in the map (x,y,z,r,phi)
#define N_FIELD_COMPONENTS 3

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
class QwMagneticField: public QwInterpolator<field_t,N_FIELD_COMPONENTS> {

  public:

    /// \brief Default constructor
    QwMagneticField();
    /// \brief Virtual destructor
    virtual ~QwMagneticField() { };

    /// \brief Define command line and config file options
    static void DefineOptions(QwOptions& options);
    /// \brief Process command line and config file options
    void ProcessOptions(QwOptions& options);

    /// Set the field rotation around z (with QwUnits)
    void SetRotation(const double rotation) {
      fRotation = rotation;
      fRotationCos = cos(rotation);
      fRotationSin = sin(rotation);
       // Unit conversions
      fRotationDeg = rotation / Qw::deg;
      fRotationRad = rotation / Qw::rad;
    };
    /// Get the field rotation around z (with QwUnits)
    double GetRotation() const
      { return fRotationRad * Qw::rad; };

    /// Set the field translation along z
    void SetTranslation(const double translation)
      { fTranslation = translation; };
    /// Get the field translation along z
    double GetTranslation() const
      { return fTranslation; };

    /// Get the cartesian components of the field value
    void GetCartesianFieldValue(const double point_xyz[3], double field_xyz[3]) const {
      double field[N_FIELD_COMPONENTS];
      GetFieldValue(point_xyz, field);
      #if N_FIELD_COMPONENTS >= 3
        field_xyz[0] = field[0]; // x
        field_xyz[1] = field[1]; // y
        field_xyz[2] = field[2]; // z
      #else
        #warning "N_FIELD_COMPONENTS should be 3 or larger."
      #endif
    };
    /// Get the cylindrical components of the field value
    void GetCylindricalFieldValue(const double point_xyz[3], double field_rfz[3]) const {
      double field[N_FIELD_COMPONENTS];
      GetFieldValue(point_xyz, field);
      #if N_FIELD_COMPONENTS == 3
        double r = sqrt(field[0] * field[0] + field[1] * field[1]) ;
        double phi = atan2(field[1],field[0]);
        if (phi < 0) phi += 2.0 * Qw::pi;
        field_rfz[0] = r; // r
        field_rfz[1] = phi; // phi
        field_rfz[2] = field[2]; // z
      #elif N_FIELD_COMPONENTS == 5
        field_rfz[0] = field[3]; // r
        field_rfz[1] = field[4]; // phi
        field_rfz[2] = field[2]; // z
      #else
        #warning "N_FIELD_COMPONENTS should be 3 or 5."
      #endif
    };

    /// \brief Read a field map input file
    bool ReadFieldMapFile(const std::string filename);
    /// \brief Read a field map input gzip file
    bool ReadFieldMapZip(const std::string filename);
    /// \brief Read the field map input stream
    bool ReadFieldMap(std::istream& input);

  private:

    /// \brief Get the field value
    void GetFieldValue(const double point[3], double field[N_FIELD_COMPONENTS]) const;

    /// Field rotation and translation with respect to the z axis
    // Defined as rotation/translation of the map in the standard coordinate
    // system: rotation of +22.5 deg means that x in the map x is at +22.5 deg,
    // likewise a translation of +5 cm means that the zero in the map is at +5 cm
    // in the standard coordinate system.
    double fRotation, fRotationDeg, fRotationRad, fRotationCos, fRotationSin;
    double fTranslation;

}; // class QwMagneticField

#endif // _QwMagneticField_h_
