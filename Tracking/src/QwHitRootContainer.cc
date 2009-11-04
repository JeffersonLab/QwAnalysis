#include "QwHitRootContainer.h"

ClassImp(QwHitRootContainer);

// Initialize the static list of QwHits
TClonesArray *QwHitRootContainer::gQwHits = 0;

// Constructor
QwHitRootContainer::QwHitRootContainer()
{
  // Create the static TClonesArray if not existing yet
  if (! gQwHits) gQwHits = new TClonesArray("QwHit", 100000);
  // Set local TClonesArray to static TClonesArray and zero hits
  fQwHits = gQwHits;
  fNQwHits = 0;
};

// Destructor
QwHitRootContainer::~QwHitRootContainer()
{
  // Delete the static TClonesArray
  Reset();
  // Set local TClonesArray to null
  fQwHits = NULL;
};

// Clear the local TClonesArray
void QwHitRootContainer::Clear(Option_t *option)
{
  fQwHits->Clear(option); // Clear the local TClonesArray
  fNQwHits = 0; // No hits in local TClonesArray
};

// Delete the static TClonesArray
void QwHitRootContainer::Reset(Option_t *option)
{
  delete gQwHits;  
  gQwHits = NULL;
};

// // Create a new QwHit
// //QwHit* QwHitRootContainer::CreateNewHit()
// //{
// //    TClonesArray &hits = *fQwHits;
// //    QwHit *hit = new (hits[fNQwHits++]) QwHit();
// //    return hit;
// //};

// Add an existing QwHit
void QwHitRootContainer::AddHit(QwHit *hit )
 {
   TClonesArray &hits = *fQwHits;
   QwHit *newhit = new (hits[fNQwHits++]) QwHit();
   //  QwHit* newhit = CreateNewHit();
   *newhit = *hit;
 };

// Convert from a QwHitContainer hitlist to the TOrdCollection
void QwHitRootContainer::Convert(QwHitContainer *hitlist)
{
  Clear();
  for (QwHitContainer::iterator hit = hitlist->begin(); hit != hitlist->end(); hit++) 
    {
      QwHit* p = &(*hit);
      AddHit(p);
    }
}

// Convert from this TOrdCollection to a QwHitContainer hitlist
QwHitContainer* QwHitRootContainer::Convert()
{
  QwHitContainer* hitlist = new QwHitContainer();
  TIterator* iterator = fQwHits->MakeIterator();
  QwHit* hit = NULL;
  while ((hit = (QwHit*) iterator->Next())) {
    hitlist->push_back(*hit);
  }
  return hitlist;
}

// Print the TOrdCollection hitlist
void QwHitRootContainer::Print()
{
  TIterator* iterator = fQwHits->MakeIterator();
  QwHit* hit = 0;
  while ((hit = (QwHit*) iterator->Next()))
    std::cout << *hit << std::endl;
}
