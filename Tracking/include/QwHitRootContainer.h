#ifndef QWHITROOTCONTAINER_H
#define QWHITROOTCONTAINER_H
#include <list>

#include "TObject.h"
#include "TClonesArray.h"
#include "TRefArray.h"
#include "TBrowser.h"

#include "QwHit.h"
#include "QwHitContainer.h"

class QwHitContainer;

class QwHitRootContainer : public TObject {

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
  QwHitContainer* Convert() const;

  void Build(QwHitContainer& hitcontainer);

  // Output function
  void Print(Option_t* option = "") const;

  TClonesArray *GetHits() const { return fQwHits; };
  QwHit *GetHit (Int_t hitID) const ;

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwHitRootContainer,1);
  #endif

}; // class QwHitRootContainer

#endif // QWHITROOTCONTAINER_H
