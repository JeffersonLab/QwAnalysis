#ifndef QWHITROOTCONTAINER_H
#define QWHITROOTCONTAINER_H

#include <list>

#include <TOrdCollection.h>

#include "QwHit.h"

class QwHitContainer;

class QwHitRootContainer: public TOrdCollection {

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

  public:

    // Conversion methods from and to a QwHitContainer
    void Convert(QwHitContainer* hitlist);
    QwHitContainer* Convert();

    // Make this browseable
    Bool_t IsFolder() { return kTRUE; };

    // Output function
    void Print();

  ClassDef(QwHitRootContainer,1);
};

#endif // QWHITROOTCONTAINER_H
