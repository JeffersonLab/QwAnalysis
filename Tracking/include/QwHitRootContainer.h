#ifndef QWHITROOTCONTAINER_H
#define QWHITROOTCONTAINER_H

#include <list>

#include <TOrdCollection.h>

#include "QwHit.h"

class QwHitContainer;

class QwHitRootContainer: public TObject {

// ROOT container class around the QwHitContainer.  Most helpful discussion
// was found on http://root.cern.ch/root/roottalk/roottalk03/2997.html.  The
// object in the TCollection has to inherit from TObject.

// The following also works, and could be a route towards writing the real
// QwHitContainer objects.  Mostly the problem seems to be that a lot of
// more advanced boost functionality is included in QwHitContainer.h instead
// of in the QwHitContainer.cc.  Moving almost all of it to QwHitContainer.cc
// should fix a lot of rootcint issues.
//
//class QwHitRootContainer: public TObject, public std::list<QwHit> {

  private:

    // Local collection of pointers to TObjects (i.e. QwHits).
    // The name fQwHitContainer was choses so it looks like a QwHitContainer
    // in the ROOT tree browser.
    TOrdCollection* fQwHitContainer;

  public:

    // Constructor
    QwHitRootContainer() {
      fQwHitContainer = new TOrdCollection;
    };
    // Destructor
    virtual ~QwHitRootContainer() {
      delete fQwHitContainer;
    }

    // Append a QwHitContainer to the TOrdCollection
    void Clear();
    void Append(QwHitContainer* hitlist);
    void Replace(QwHitContainer* hitlist);
    Int_t GetSize() { return fQwHitContainer->GetSize(); };

    // Read the hit container from TOrdCollection to QwHitContainer
    QwHitContainer* Read();


  ClassDef(QwHitRootContainer,1)
};

#endif // QWHITROOTCONTAINER_H
