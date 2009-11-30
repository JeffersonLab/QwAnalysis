//
//  date: Sun Nov 29 22:28:23 CST 2009
//
//  jpan: QTOR field map reading class. 
//        Modified from the Qweak geant4 simulation code
//

#ifndef _QwMagneticField_h_
#define _QwMagneticField_h_

#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <fstream>

#include <TVector3.h>
#include <TRotation.h>

class QwMagneticField
{
public:

  QwMagneticField();
  virtual ~QwMagneticField();

  void GetFieldValue( const   double Point[3], double *Bfield, double scalefactor ) const;  
  void GetFieldValueFromGridCell( const  int GridPoint_R, 
				  const  int GridPoint_Phi, 
				  const  int GridPoint_Z, 
				  double *BFieldGridValue ) const;

  void InitializeGrid();
  void ReadFieldMap(std::string filename);


  void SetFieldMap_RMin      ( double Rmin      ) { rMinFromMap    = Rmin;      }
  void SetFieldMap_RMax      ( double Rmax      ) { rMaxFromMap    = Rmax;      }
  void SetFieldMap_RStepsize ( double Rstepsize ) { gridstepsize_r = Rstepsize; }

  void SetFieldMap_ZMin      ( double Zmin      ) { zMinFromMap    = Zmin;      }
  void SetFieldMap_ZMax      ( double Zmax      ) { zMaxFromMap    = Zmax;      }
  void SetFieldMap_ZStepsize ( double Zstepsize ) { gridstepsize_z = Zstepsize; }

  void SetFieldMap_PhiMin      ( double Phimin      ) { phiMinFromMap    = Phimin;      }
  void SetFieldMap_PhiMax      ( double Phimax      ) { phiMaxFromMap    = Phimax;      }
  void SetFieldMap_PhiStepsize ( double Phistepsize ) { gridstepsize_phi = Phistepsize; }

private:

  int nGridPointsInR;
  int nGridPointsInPhi;
  int nGridPointsInZ;  

  double rMinFromMap;
  double rMaxFromMap;
   
  double phiMinFromMap;
  double phiMaxFromMap;
   
  double zMinFromMap;
  double zMaxFromMap;
   
  double gridstepsize_r;
  double gridstepsize_phi;
  double gridstepsize_z;

  double Unit_Bfield; // units of field map 

  int fGridSize;

// Storage space for the table
//   std::vector< std::vector< std::vector< double > > > BFieldGridData_X;
//   std::vector< std::vector< std::vector< double > > > BFieldGridData_Y;
//   std::vector< std::vector< std::vector< double > > > BFieldGridData_Z;
  std::vector< double > BFieldGridData_X;
  std::vector< double > BFieldGridData_Y;
  std::vector< double > BFieldGridData_Z;

  double fZoffset;
  bool   invertX, invertY, invertZ;

  TVector3* BField_ANSYS;
  
  double BFieldScalingFactor;

};

#endif
