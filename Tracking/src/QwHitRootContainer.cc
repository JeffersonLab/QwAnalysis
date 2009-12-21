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

void QwHitRootContainer::Delete(Option_t *option)
{
  fQwHits->Delete(option);
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
   *newhit = *hit;
 };

QwHit *QwHitRootContainer::AddQwHit(QwHit &in)
 {
   TClonesArray &hits = *fQwHits;
   printf("fNQwHits %d :", fNQwHits);
   QwHit *hit = new (hits[fNQwHits++]) QwHit();

   hit = &in;
   printf("&hits %p &in %p hit %p\n", &hits, &in, hit);
   
   return hit;
 };


QwHit *QwHitRootContainer::AddQwHit2()
 {
   TClonesArray &hits = *fQwHits;
   QwHit *hit = new (hits[fNQwHits++]) QwHit();
   return hit;
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


void QwHitRootContainer::ConvertTest(QwHitContainer *hitlist)
{
  Clear();
  QwHit *qwhit; 
  for (QwHitContainer::iterator hit = hitlist->begin(); hit != hitlist->end(); hit++)
    {
      std::cout << *hit << std::endl;
      qwhit = (QwHit*) AddQwHit((QwHit &) *hit);
      printf("qwhit %p\n", qwhit);
      std::cout <<*qwhit << std::endl;
  //     printf("qwhit %p, *hit %p\n", &qwhit, *hit);
    }
  return;
}


void QwHitRootContainer::ConvertTest2(QwHitContainer *hitlist)
{
  Clear();
  
  for (QwHitContainer::iterator hit = hitlist->begin(); hit != hitlist->end(); hit++)
    {
      TClonesArray &hits = *fQwHits;
      new (hits[fNQwHits++]) QwHit((QwHit&)*hit);
    }
  return;
}


void QwHitRootContainer::ConvertTest3(QwHitContainer *hitlist)
{
  Clear("C");
  fNQwHits = 0;
  
  for (QwHitContainer::iterator hit = hitlist->begin(); hit != hitlist->end(); hit++)
    {
      printf("===================== hit list %p\n", &hit);
      std::cout << *hit << std::endl;
      QwHit *qwhit = AddQwHit2();
      qwhit = &(*hit);
      printf("===================== root hit list %p\n", &qwhit);
      std::cout << *qwhit << std::endl;
    }
  return;
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

QwHit *QwHitRootContainer::GetHit (Int_t hitID) const
{
  return (QwHit*) fQwHits->At(hitID);
}
