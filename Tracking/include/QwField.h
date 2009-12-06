#ifndef QWFIELD_H
#define QWFIELD_H

#include <cmath>
#include <string>
#include <vector>
#include <fstream>

#include <TVector3.h>
#include <TRotation.h>

/*------------------------------------------------------------------------*//*!

 \brief ...

 A QwField ...

 \ingroup QwTracking

*//*-------------------------------------------------------------------------*/
class QwField {

  public: // methods

    // Constructor
    QwField(std::string fieldmap, double scalingfactor = 1.0);

    // Return the field
    const TVector3 GetField(TVector3 position) const;
    const TVector3 GetField(const double r, const double phi, const double z) const {
      TVector3 position(fabs(r) * cos(phi), fabs(r) * sin(phi), z);
      return GetField(position);
    };
    const double GetFieldX(const TVector3 position) const {
      return GetField(position).X();
    };
    const double GetFieldY(const TVector3 position) const {
      return GetField(position).Y();
    };
    const double GetFieldZ(const TVector3 position) const {
      return GetField(position).Z();
    };

  private:

    // Initialize the grid and read the field map
    void InitializeGrid();
    void ReadFieldMap(std::string fieldmap);
    const unsigned int GetIndex(const int* index) const;
    const unsigned int GetIndex(const int iR, const int iPhi, const int iZ) const;
    const unsigned int GetIndex(const double pos, const int coord) const;
    const unsigned int GetIndex(const TVector3 pos) const;

    // Query the field by getting fractional indices
    const bool IsValidPosition(const TVector3 pos) const;
    const std::vector<unsigned int> GetLocalCorners(const TVector3 pos) const;
    const double* GetLocalCoordinates(const TVector3 pos) const;

    // Field map
    std::string fFieldMap;

    // Grid description
    unsigned int fGridSize;
    double fGridMin[3];
    double fGridMax[3];
    double fGridStep[3];
    double fGridNSteps[3];

    // Field values
    double fBFieldScalingFactor;
    std::vector<float> fGridValueX;
    std::vector<float> fGridValueY;
    std::vector<float> fGridValueZ;

    // Mis-alignment of the field
    TRotation fFieldRotation;
    TVector3  fFieldOffset;

    // Debug level
    bool fDebug;

    // Coordinate constants
    static const int kR   = 0;
    static const int kPhi = 1;
    static const int kX = 0;
    static const int kY = 1;
    static const int kZ = 2;

}; // class QwField

#endif // QWFIELD_H
