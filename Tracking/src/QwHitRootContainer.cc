#include "QwHitRootContainer.h"
#include "QwHitContainer.h"

ClassImp(QwHitRootContainer);

// Convert from a QwHitContainer hitlist to the TOrdCollection
void QwHitRootContainer::Convert(QwHitContainer* hitlist)
{
  Clear();
  for (QwHitContainer::iterator hit = hitlist->begin();
       hit != hitlist->end(); hit++) {
    QwHit* p = &(*hit);
    AddLast((TObject*) p);
//     this->push_back(*hit);
  }
}

// Convert from this TOrdCollection to a QwHitContainer hitlist
QwHitContainer* QwHitRootContainer::Convert()
{
  QwHitContainer* hitlist = new QwHitContainer();
  TIterator* iterator = MakeIterator();
  QwHit* hit = 0;
  while ((hit = (QwHit*) iterator->Next())) {
    hitlist->push_back(*hit);
  }
//   for (std::list<QwHit>::iterator hit = begin(); hit != end(); hit++)
//     hitlist->push_back(*hit);
  return hitlist;
}

// Print the TOrdCollection hitlist
void QwHitRootContainer::Print()
{
  TIterator* iterator = MakeIterator();
  QwHit* hit = 0;
  while ((hit = (QwHit*) iterator->Next()))
    std::cout << *hit << std::endl;
}
