/*! \file   QwParaHit.cc
 *  \author Jie Pan
 *  \date   Tue Nov 24 14:25:14 CST 2009
 *  \brief  Interface class for decoding subsystems except R2 and R3.
 *
 */
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "QwParaHit.h"

ClassImp(QwParaHit);

#include "QwDetectorInfo.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwParaHit::QwParaHit()
{
  fRegion      = 0;   
  fPackage     = 0;  
  fDirection   = 0;
  fPlane       = 0;    
  pDetectorInfo = NULL;
  next = NULL;
  nextdet = NULL;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwParaHit::Print()
{
  if (! this) return;
  std::cout << *this << std::endl;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

ostream& operator<< (ostream& stream, const QwParaHit& hit) {
  stream << "hit: ";
  stream << "package "   << hit.fPackage << ", ";
  stream << "region "    << hit.fRegion << ", ";
  stream << "dir "       << hit.fDirection << ", ";
  stream << "plane "     << hit.fPlane;

  if (hit.pDetectorInfo) stream << " (detector " << hit.pDetectorInfo << "), ";
  else stream << ", ";

  if (hit.fRegion == 1)
  {
      stream << "radius "   << hit.fRPos << " cm, ";
      stream << "phi "  << hit.fPhiPos << " ("<<hit.fPhiPos*180.0/3.1415927<<" deg)";
  }
  else 
    {
    }

  return stream;
};



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwParaHit::~QwParaHit()
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
