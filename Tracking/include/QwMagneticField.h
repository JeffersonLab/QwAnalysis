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
#include "QwParameterFile.h"

// Forward declarations
template <class value_t, unsigned int value_n> class QwInterpolator;

/**
 *  \class QwMagneticField
 *  \ingroup QwTracking
 *  \brief Magnetic field map object
 *
 * This class implements a magnetic field object using the QwInterpolator class.
 *
 * Five magnetic field components are stored (larger memory usage, computationally
 * more intensive on creation, but faster access by avoiding trigonometric
 * operations).
 *
 * Access to the field components is possible using two functions:
 * \code
 * field->GetCartesianFieldValue(double point_xyz[3], double field_xyz[3]);
 * field->GetCylindricalFieldValue(double point_xyz[3], double field_rfz[3]);
 * \endcode
 *
 * Currently no function exists that takes cylindrical coordinates.
 */
class QwMagneticField  {

  private:

    // Number of field component in the map (x,y,z,r,phi)
    static const unsigned int value_n = 3;

    // Field map storage data type
    typedef float field_t;

  public:

    /// \brief Default constructor
    QwMagneticField(QwOptions& options, const bool suppress_read_field_map = false);
    /// \brief Virtual destructor
    virtual ~QwMagneticField();


    /// \brief Define command line and config file options
    static void DefineOptions(QwOptions& options);
    /// \brief Process command line and config file options
    void ProcessOptions(QwOptions& options);

    /// \brief Load beam property
    void LoadBeamProperty(const TString& map);

    /// Set the filename
    void SetFilename(const std::string& filename)
      { fFilename = filename; };
    /// Get the filename
    const std::string GetFilename() const
      { return fFilename; };

    /// Set the actual current
    void SetActualCurrent(const double current) {
      fActualCurrent = current;
      if (fReferenceCurrent > 0.0)
        fScaleFactor = fActualCurrent/fReferenceCurrent;
    }
    /// Get the actual current
    double GetActualCurrent() const
      { return fActualCurrent; }

    /// Set the reference current
    void SetReferenceCurrent(const double current) {
      fReferenceCurrent = current;
      if (fReferenceCurrent > 0.0)
        fScaleFactor = fActualCurrent/fReferenceCurrent;
    }
    /// Get the reference current
    double GetReferenceCurrent() const
    { return fReferenceCurrent; }

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
      double field[value_n];
      GetFieldValue(point_xyz, field);
      if (value_n >= 3) {
        field_xyz[0] = field[0]; // x
        field_xyz[1] = field[1]; // y
        field_xyz[2] = field[2]; // z
      }
    };
    /// Get the cylindrical components of the field value
    void GetCylindricalFieldValue(const double point_xyz[3], double field_rfz[3]) const {
      double field[value_n];
      GetFieldValue(point_xyz, field);
      if (value_n == 3) {
        double r = sqrt(field[0] * field[0] + field[1] * field[1]) ;
        double phi = atan2(field[1],field[0]);
        if (phi < 0) phi += 2.0 * Qw::pi;
        field_rfz[0] = r; // r
        field_rfz[1] = phi; // phi
        field_rfz[2] = field[2]; // z
      } else if (value_n == 5) {
        field_rfz[0] = field[3]; // r
        field_rfz[1] = field[4]; // phi
        field_rfz[2] = field[2]; // z
      } else {
        QwWarning << "Number of field components should be 3 or 5." << QwLog::endl;
      }
    };

    /// Get the cartesian components of the field value
    void GetCartesianFieldValue(const TVector3& point, TVector3& field) const {
      double point_xyz[3], field_xyz[3];
      point.GetXYZ(point_xyz);
      GetCartesianFieldValue(point_xyz, field_xyz);
      field = TVector3(field_xyz);
    }

    /// \brief Read a field map
    bool ReadFieldMap();
    /// \brief Read a field map input file
    bool ReadFieldMapFile(const std::string& filename);
    /// \brief Read a field map input gzip file
    bool ReadFieldMapZip(const std::string& filename);
    /// \brief Read the field map input stream
    bool ReadFieldMapStream(std::istream& input);

    /// \brief Test the field map
    bool TestFieldMap();

    /// \name Expose some functionality of underlying field map
    // @{
    /// Write a binary field map
    bool WriteBinaryFile(const std::string& fieldmap) const {
      std::string filename = getenv_safe_string("QW_FIELDMAP") + "/" + fieldmap;
      if (fField) return fField->WriteBinaryFile(filename);
      else return false;
    }
    /// Write a text field map
    bool WriteTextFile(const std::string& fieldmap) const {
      std::string filename = getenv_safe_string("QW_FIELDMAP") + "/" + fieldmap;
      if (fField) return fField->WriteTextFile(filename);
      else return false;
    }
    /// Set interpolation method
    void SetInterpolationMethod(const EQwInterpolationMethod method) {
      if (fField) fField->SetInterpolationMethod(method);
    }
    /// Get interpolation method
    EQwInterpolationMethod GetInterpolationMethod() const {
      if (fField) return fField->GetInterpolationMethod();
      else return kInterpolationMethodUnknown;
    }
    // @}

  private:

    /// File name
    std::string fFilename;

    /// Field map
    QwInterpolator<field_t,value_n>* fField;

    /// Field map grid min, max, step, wrap
    std::vector<double> fMin, fMax, fStep;
    std::vector<size_t> fWrap;

    /// Field scale factor
    double fReferenceCurrent;
    double fActualCurrent;
    double fScaleFactor;

    /// \brief Get the field value
    void GetFieldValue(const double point[3], double field[value_n]) const;

    /// Field rotation and translation with respect to the z axis
    // Defined as rotation/translation of the map in the standard coordinate
    // system: rotation of +22.5 deg means that x in the map x is at +22.5 deg,
    // likewise a translation of +5 cm means that the zero in the map is at +5 cm
    // in the standard coordinate system.
    // Note: This rotation and translation are only applied when reading from
    // an ASCII field map generated by ANSYS.
    double fRotation, fRotationDeg, fRotationRad, fRotationCos, fRotationSin;
    double fTranslation;

}; // class QwMagneticField

#endif // _QwMagneticField_h_
