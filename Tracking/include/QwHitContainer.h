/*! \file   QwHitContainer.h
 *  \author P. M. King
 *  \date   2008may28
 *  \brief  Agumented vector of QwHits allowing special seach functionality.
 *  
 *
 * 
 *
 */

#ifndef QWHITCONTAINER_H
#define QWHITCONTAINER_H

#include <list>
#include <boost/bind.hpp>

#include "QwTypes.h"
#include "QwHit.h"

static struct logical_not_s{
  bool operator()(bool a){return !a;};
} logical_not;


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

  void Append(const QwHitContainer &list){
    this->insert(this->end(), list.begin(), list.end());
  };

  void Append(std::vector<QwHit>  &list){
    this->insert(this->end(), list.begin(), list.end());
  };
  

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
		    boost::bind<bool>(logical_not,
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
		    boost::bind<bool>(logical_not,
				      boost::bind(&QwHit::PlaneMatches, _1, 
						  boost::ref(region),
						  boost::ref(package),
						  boost::ref(plane))));
  };
  
  
 
 protected:



};

#endif
