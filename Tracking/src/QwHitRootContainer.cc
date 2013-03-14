#include "QwHitRootContainer.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
ClassImp(QwHitRootContainer);
#endif

// Initialize the static list of QwHits
TClonesArray *QwHitRootContainer::gQwHits = 0;

// Constructor
QwHitRootContainer::QwHitRootContainer()
{
  // Create the static TClonesArray if not existing yet
  if (! gQwHits) gQwHits = new TClonesArray("QwHit", 100000);
  // Set local TClonesArray to static TClonesArray and zero hits
  fQwHits = gQwHits;
  fQwHits->Clear();
}

// Destructor
QwHitRootContainer::~QwHitRootContainer()
{
  // Delete the static TClonesArray
  Reset();
  // Set local TClonesArray to null
  fQwHits = 0;
}

// Clear the local TClonesArray
void QwHitRootContainer::Clear(Option_t *option)
{
  fQwHits->Clear(option);
  fNQwHits = 0;
}

void QwHitRootContainer::Delete(Option_t *option)
{
  fQwHits->Delete(option);
  fNQwHits = 0;
}

// Delete the static TClonesArray
void QwHitRootContainer::Reset(Option_t *option)
{
  delete gQwHits;
  gQwHits = 0;
}

// Add an existing QwHit
void QwHitRootContainer::AddHit(QwHit *hit)
{
  TClonesArray &hits = *fQwHits;
  QwHit *newhit = new (hits[fNQwHits++]) QwHit();
  *newhit = *hit;
}

void QwHitRootContainer::AddQwHit(QwHit &in)
{
  TClonesArray &hits = *fQwHits;
  new (hits[fNQwHits++]) QwHit(in);
  return;
}

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

void QwHitRootContainer::Build(QwHitContainer &hitcontainer)
{
  // Save current Object count
  // Int_t ObjectNumber = TProcessID::GetObjectCount();
  Clear();
  QwHitContainer::iterator hititerator;
  for (hititerator = hitcontainer.begin(); hititerator != hitcontainer.end(); hititerator++)
    {
      AddQwHit(*hititerator);
    }
  //   TProcessID::SetObjectCount(ObjectNumber);
  return;
}

// Convert from this TOrdCollection to a QwHitContainer hitlist
QwHitContainer* QwHitRootContainer::Convert() const
{
  QwHitContainer* hitlist = new QwHitContainer();
  TIterator* iterator = fQwHits->MakeIterator();
  QwHit* hit = 0;
  while ((hit = dynamic_cast<QwHit*>(iterator->Next())))
    hitlist->push_back(*hit);
  return hitlist;
}

// Print the TOrdCollection hitlist
void QwHitRootContainer::Print(Option_t* option) const
{
  TIterator* iterator = fQwHits->MakeIterator();
  QwHit* hit = 0;
  while ((hit = dynamic_cast<QwHit*>(iterator->Next())))
    std::cout << *hit << std::endl;
}

QwHit* QwHitRootContainer::GetHit (Int_t hit) const
{
  return dynamic_cast<QwHit*>(fQwHits->At(hit));
}
