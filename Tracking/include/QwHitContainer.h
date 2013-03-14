

/**********************************************************\
* File: QwHitContainer.h                                   *
*                                                          *
* Author: P. M. King ,Rakitha Beminiwattha                 *
* brief  Agumented vector of QwHits allowing special       *
* seach functionality                                      *
* Time-stamp: <2008-07-08 15:40>                           *
\**********************************************************/


#ifndef QWHITCONTAINER_H
#define QWHITCONTAINER_H

#include <iostream>
#include <list>
#include <vector>
#include <algorithm>

#include <TObject.h>

#include "QwTypes.h"
#include "QwHit.h"


///
/// \ingroup QwTracking
class QwHitContainer: public std::list<QwHit>, public TObject {
 private:
 /*  typedef std::list<QwHit> QwHitArray; */
 public:
  typedef std::list<QwHit> QwHitArray;

  using QwHitArray::iterator;
  using QwHitArray::begin;
  using QwHitArray::end;


  void Clear(const Option_t* /*option*/ = 0) {
    this->QwHitArray::clear();
  };

  //append from a QwHit vector - rakitha (08/2008)
  void Append(const QwHitContainer &mylist) {
    this->insert(this->end(), mylist.begin(), mylist.end());
  };

  void Append(const QwHitContainer* mylist) {
    this->insert(this->end(), mylist->begin(), mylist->end());
  };

  void Append(std::vector<QwHit> &mylist) {
    this->insert(this->end(), mylist.begin(), mylist.end());
  };


  //iterator methods to obtain subsets of QwHitContainer elements - rakitha (08/2008)
  iterator GetStartOfHits(EQwRegionID region, EQwDetectorPackage package, EQwDirectionID direction);
  iterator GetEndOfHits(EQwRegionID region, EQwDetectorPackage package, EQwDirectionID direction);
  iterator GetStartOfHits1(EQwRegionID region, EQwDetectorPackage package, Int_t plane);
  iterator GetEndOfHits1(EQwRegionID region, EQwDetectorPackage package, Int_t plane);

  // Return sub set of QwHitContainer list based on wire plane or wire direction -Rakitha (1023/2008)
  void GetSubList_Dir(EQwRegionID region, EQwDetectorPackage package, EQwDirectionID direction, std::vector<QwHit> &  sublist);
  void GetSubList_Plane(EQwRegionID region, EQwDetectorPackage package, Int_t plane, std::vector<QwHit> &  sublist);

  // Return the sublist of hits only in specified package, region, and detector plane or direction
  QwHitContainer* GetSubList_Plane (EQwRegionID region, EQwDetectorPackage package, Int_t plane);
  QwHitContainer* GetSubList_Dir (EQwRegionID region, EQwDetectorPackage package, EQwDirectionID dir);


  void Print (const Option_t* option = 0) const;
  friend std::ostream& operator<< (std::ostream& stream, const QwHitContainer& hitlist);


  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwHitContainer,1);
  #endif
};

#endif
