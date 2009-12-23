#ifndef QWHITROOTCONTAINER_H
#define QWHITROOTCONTAINER_H
//nclude <iostream>
//nclude <algorithm>
#include <list>
//nclude <vector>

// ROOT headers
//#include "TOrdCollection.h"
#include "TObject.h"
#include "TClonesArray.h"
#include "TRefArray.h"
#include "TBrowser.h"

#include "QwHit.h"
#include "QwHitContainer.h"

class QwHitContainer;

class QwHitRootContainer : public TObject {

// ROOT container class around the QwHitContainer.  Most helpful discussion
// was found on http://root.cern.ch/root/roottalk/roottalk03/2997.html.  The
// object in the TCollection has to inherit from TObject.

// The following also works, and could be a route towards writing the real
// QwHitContainer objects.  Mostly the problem seems to be that a lot of
// more advanced boost functionality is included in QwHitContainer.h instead
// of in the QwHitContainer.cc.  Moving almost all of it to QwHitContainer.cc
// should fix a lot of rootcint issues.
//
//class QwHitRootContainer: public TOrdCollection, public std::list<QwHit> {

//  private:

 public:

  Int_t               fNQwHits; ///< Number of QwHits in the array
  TClonesArray        *fQwHits; ///< Array of QwHits
  static TClonesArray *gQwHits; //! ///< Static array of QwHits

 public:

  QwHitRootContainer();
  virtual ~QwHitRootContainer();

  // Housekeeping methods
  void Clear(Option_t *option = "");
  void Reset(Option_t *option = "");
  void Delete(Option_t *option = "");

  // Creating and adding hits
  //  QwHit* CreateNewHit();
  void AddHit(QwHit * );//QwHit *hit);
  void AddQwHit(QwHit &in);

  // Get the number of hits
  Int_t GetSize() const { return fNQwHits; };

  // Conversion methods from and to a QwHitContainer
  void Convert(QwHitContainer *hitlist);
  QwHitContainer* Convert();

  void Build(QwHitContainer& hitcontainer);
  
  // Output function
  void Print();

  TClonesArray *GetHits() const { return fQwHits; };
  QwHit *GetHit (Int_t hitID) const ;

  ClassDef(QwHitRootContainer,1);

}; // class QwHitRootContainer

#endif // QWHITROOTCONTAINER_H
