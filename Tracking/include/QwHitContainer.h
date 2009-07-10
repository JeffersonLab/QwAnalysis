

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

#include <list>
#include <vector>
#include <algorithm>
#include <boost/bind.hpp>

#include "QwTypes.h"
#include "QwHit.h"

static struct logical_not_s{
  bool operator()(bool a){return !a;};
} logical_not_;


///
/// \ingroup QwTrackingAnl
class QwHitContainer:  public std::list<QwHit>{
 private:
  typedef std::list<QwHit> QwHitArray;
 public:
  using QwHitArray::iterator;
  using QwHitArray::begin;
  using QwHitArray::end;

  void Clear(){
    this->QwHitArray::clear();
  };

  //append from a QwHit vector - rakitha (08/2008)
  void Append(const QwHitContainer &list){
    this->insert(this->end(), list.begin(), list.end());
  };

  void Append(std::vector<QwHit>  &list){
    this->insert(this->end(), list.begin(), list.end());
  };


  //iterator methods to obtain subsets of QwHitContainer elements - rakitha (08/2008)
  iterator GetStartOfHits(EQwRegionID region, EQwDetectorPackage package,
			  EQwDirectionID direction){
    return find_if (begin(), end(),
		    boost::bind(&QwHit::DirMatches, _1,
				boost::ref(region), boost::ref(package),
				boost::ref(direction)));
  };

  iterator GetEndOfHits(EQwRegionID region, EQwDetectorPackage package,
			EQwDirectionID direction){
    iterator first = GetStartOfHits(region, package, direction);
    return find_if (first, end(),
		    boost::bind<bool>(logical_not_,
				      boost::bind(&QwHit::DirMatches, _1,
						  boost::ref(region),
						  boost::ref(package),
						  boost::ref(direction))));
  };


  iterator GetStartOfHits1(EQwRegionID region, EQwDetectorPackage package,
			  Int_t plane) {
    return find_if (begin(), end(),
		    boost::bind(&QwHit::PlaneMatches, _1,
				boost::ref(region), boost::ref(package),
				boost::ref(plane)));
  };
  iterator GetEndOfHits1(EQwRegionID region, EQwDetectorPackage package,
			Int_t plane){
    iterator first = GetStartOfHits1(region, package, plane);
    return find_if (first, end(),
		    boost::bind<bool>(logical_not_,
				      boost::bind(&QwHit::PlaneMatches, _1,
						  boost::ref(region),
						  boost::ref(package),
						  boost::ref(plane))));
  };



  //will return sub set of QwHitContainer list based on wire plane or wire direction -Rakitha (1023/2008)

  void GetSubList_Dir(EQwRegionID region, EQwDetectorPackage package, EQwDirectionID direction, std::vector<QwHit> &  sublist) {
    std::list<QwHit>::iterator p;
    for (p=GetStartOfHits(region,package,direction);p !=GetEndOfHits(region,package,direction);p++){
      sublist.push_back(*p);
    }
  }

  void GetSubList_Plane(EQwRegionID region, EQwDetectorPackage package, Int_t plane, std::vector<QwHit> &  sublist) {
    std::list<QwHit>::iterator p;
    for (p=GetStartOfHits1(region,package,plane);p !=GetEndOfHits1(region,package,plane);p++){
      sublist.push_back(*p);
    }
  }


  // Return the sublist of hits only in specified package, region, and detector plane
  QwHitContainer* GetSubList_Plane (EQwRegionID region, EQwDetectorPackage package, Int_t plane) {
    QwHitContainer* sublist = new QwHitContainer;
    for (QwHitContainer::iterator hit = begin(); hit != end(); hit++)
      if (hit->PlaneMatches(region, package, plane))
        sublist->push_back(*hit);
    return sublist;
  }

  // Return the sublist of hits only in specified package, region, and detector plane
  QwHitContainer* GetSubList_Dir (EQwRegionID region, EQwDetectorPackage package, EQwDirectionID dir) {
    QwHitContainer* sublist = new QwHitContainer;
    for (QwHitContainer::iterator hit = begin(); hit != end(); hit++)
      if (hit->DirMatches(region, package, dir))
        sublist->push_back(*hit);
    return sublist;
  }

  // Return the sublist of hits only in specified detector plane
  void Print () {
    for (QwHitContainer::iterator hit = begin(); hit != end(); hit++)
      hit->Print();
  }



};

#endif
