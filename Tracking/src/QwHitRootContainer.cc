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
  //  fNQwHits = 0;
};

// Destructor
QwHitRootContainer::~QwHitRootContainer()
{
  // Delete the static TClonesArray
  Reset();
  // Set local TClonesArray to null
  //  fQwHits = NULL;
};

// Clear the local TClonesArray
void QwHitRootContainer::Clear(Option_t *option)
{
  fQwHits->Clear(option); // Clear the local TClonesArray
  //  fNQwHits = 0; // No hits in local TClonesArray
};

void QwHitRootContainer::Delete(Option_t *option)
{
  fQwHits->Delete(option);
};

// Delete the static TClonesArray
void QwHitRootContainer::Reset(Option_t *option)
{
  delete gQwHits;  
  gQwHits = 0;
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
   printf("\n\nAddHits\n");
   TClonesArray &hits = *fQwHits;
   std::cout << "&hits : " << &hits << " fQwHits " << fQwHits << std::endl;
   QwHit *newhit = new (hits[fNQwHits++]) QwHit();
   *newhit = *hit;
   std::cout << "*newhit " << newhit << " *hit " << hit << std::endl;
 };

void QwHitRootContainer::AddQwHit(QwHit &in)
 {
   TClonesArray &hits = *fQwHits;
   //   printf("fNQwHits %d :", fNQwHits);
   new (hits[fNQwHits++]) QwHit(in);
   return;
 };

QwHit* QwHitRootContainer::AddQwHit()
{
   TClonesArray &hits = *fQwHits;
   QwHit *hit = new (hits[fNQwHits++]) QwHit();
   return hit;
};


QwHit *QwHitRootContainer::AddQwHit2()
 {
   printf("\n\nAddQwHits\n");
   TClonesArray &hits = *fQwHits;
   std::cout << "&hits : " << &hits << " fQwHits " << fQwHits << std::endl;
   QwHit *hit = new ( hits[fNQwHits++] ) QwHit();
   std::cout << "*hit : " << hit << std::endl;
   return hit;
 };




QwHit *QwHitRootContainer::AddQwHit4(QwHit &in)
 {
   printf("\n\nAddQwHits\n");
   TClonesArray &hits = *fQwHits;
   std::cout << "&hits : " << &hits << " fQwHits " << fQwHits << std::endl;
   QwHit *hit = new (hits[fNQwHits++]) QwHit(in);
   return hit;
 };


// Convert from a QwHitContainer hitlist to the TOrdCollection
void QwHitRootContainer::Convert(QwHitContainer *hitlist)
{
  Delete();
  for (QwHitContainer::iterator hit = hitlist->begin(); hit != hitlist->end(); hit++) 
    {
      QwHit* p = &(*hit);
      AddHit(p);
    }
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
  Delete("C");
  fNQwHits = 0;
  QwHit *qwhit;
  for (QwHitContainer::iterator hit = hitlist->begin(); hit != hitlist->end(); hit++)
    {
 //       std::cout << *hit << std::endl;
      qwhit = (QwHit*) AddQwHit2();
      printf("data %p qwhit %p\n", &hit, &qwhit);
      //      std::cout << *qwhit << std::endl;
    }
  return;
}




void QwHitRootContainer::ConvertTest4(QwHitContainer *hitlist)
{
  Delete("C");
  fNQwHits = 0;
  for (QwHitContainer::iterator hit = hitlist->begin(); hit != hitlist->end(); hit++)
    {
      printf("\n\nAddQwHits\n");
      TClonesArray &hits = *fQwHits;
      std::cout << "&hits : " << &hits << " fQwHits " << fQwHits << std::endl;
      new (hits[fNQwHits++]) QwHit((QwHit&)*hit);
      printf("data %p hits %p\n", &hit, &hits);
    }
  return;
}



void QwHitRootContainer::Build(QwHitContainer &hitcontainer)
{
  //Save current Object count
   Int_t ObjectNumber = TProcessID::GetObjectCount();
   Clear();
   //   QwHit *qwhit = 0;
   fNQwHits  = 0;

   QwHitContainer::iterator hititerator;
   for (hititerator = hitcontainer.begin(); hititerator != hitcontainer.end(); hititerator++)
     {
       //       std::cout << *hititerator << std::endl;
       AddQwHit(*hititerator);
     }
   TProcessID::SetObjectCount(ObjectNumber);
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
