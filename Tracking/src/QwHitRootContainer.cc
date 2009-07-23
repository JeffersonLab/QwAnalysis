#include "QwHitRootContainer.h"
#include "QwHitContainer.h"

ClassImp(QwHitRootContainer);

void QwHitRootContainer::Clear()
{
  fQwHitContainer->Clear();
}

void QwHitRootContainer::Append(QwHitContainer* hitlist)
{
  for (QwHitContainer::iterator hit = hitlist->begin();
       hit != hitlist->end(); hit++) {
    QwHit* p = &(*hit);
    fQwHitContainer->AddLast((TObject*) p);
  }
}

void QwHitRootContainer::Replace(QwHitContainer* hitlist)
{
  Clear();
  Append(hitlist);
}

QwHitContainer* QwHitRootContainer::Read()
{
  QwHitContainer* hitlist = new QwHitContainer();
  TIterator* iterator = fQwHitContainer->MakeIterator();
  QwHit* hit = 0;
  while ((hit = (QwHit*) iterator->Next())) {
    hitlist->push_back(*hit);
  }
  return hitlist;
}
