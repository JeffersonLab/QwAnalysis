

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
#include <boost/bind.hpp>

#include "QwTypes.h"
#include "QwHit.h"

static struct logical_not_s{
  bool operator()(bool a){return !a;};
} logical_not_;


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
  iterator GetStartOfHits(EQwRegionID region, Int_t package,
			  EQwDirectionID direction){
    return find_if (begin(), end(), 
		    boost::bind(&QwHit::DirMatches, _1, 
				boost::ref(region), boost::ref(package),
				boost::ref(direction)));
  };

  iterator GetEndOfHits(EQwRegionID region, Int_t package,
			EQwDirectionID direction){
    iterator first = GetStartOfHits(region, package, direction);
    return find_if (first, end(), 
		    boost::bind<bool>(logical_not_,
				      boost::bind(&QwHit::DirMatches, _1, 
						  boost::ref(region),
						  boost::ref(package),
						  boost::ref(direction))));
  };
  
  
  iterator GetStartOfHits1(EQwRegionID region, Int_t package,
			  Int_t plane) {
    return find_if (begin(), end(), 
		    boost::bind(&QwHit::PlaneMatches, _1, 
				boost::ref(region), boost::ref(package),
				boost::ref(plane)));
  };
  iterator GetEndOfHits1(EQwRegionID region, Int_t package,
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

  void GetSubList_Dir(EQwRegionID region, Int_t package, EQwDirectionID direction, std::vector<QwHit> &  sublist){
          
    std::list<QwHit>::iterator p;   
    for (p=GetStartOfHits(region,package,direction);p !=GetEndOfHits(region,package,direction);p++){     
      sublist.push_back(*p);
    }  
  }

  void GetSubList_Plane(EQwRegionID region, Int_t package, Int_t plane, std::vector<QwHit> &  sublist){
          
    std::list<QwHit>::iterator p;   
    for (p=GetStartOfHits1(region,package,plane);p !=GetEndOfHits1(region,package,plane);p++){     
      sublist.push_back(*p);
    }  
  }
 
 


};

#endif
