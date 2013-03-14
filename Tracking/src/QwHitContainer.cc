#include "QwHitContainer.h"

#if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
ClassImp(QwHitContainer)
#endif

#include <boost/bind.hpp>

static struct logical_not_s{
  bool operator()(bool a){return !a;};
} logical_not_;

// Iterator methods to obtain subsets of QwHitContainer elements - rakitha (08/2008)
QwHitContainer::iterator QwHitContainer::GetStartOfHits (
	EQwRegionID region,
	EQwDetectorPackage package,
	EQwDirectionID direction)
{
  return find_if (begin(), end(),
		  boost::bind(&QwHit::DirMatches, _1,
			boost::ref(region),
			boost::ref(package),
			boost::ref(direction)));
}

QwHitContainer::iterator QwHitContainer::GetEndOfHits (
	EQwRegionID region,
	EQwDetectorPackage package,
	EQwDirectionID direction)
{
  iterator first = GetStartOfHits(region, package, direction);
  return find_if (first, end(),
		  boost::bind<bool>(logical_not_,
			  boost::bind(&QwHit::DirMatches, _1,
				boost::ref(region),
				boost::ref(package),
				boost::ref(direction))));
}


QwHitContainer::iterator QwHitContainer::GetStartOfHits1 (
	EQwRegionID region,
	EQwDetectorPackage package,
	Int_t plane)
{
  return find_if (begin(), end(),
		  boost::bind(&QwHit::PlaneMatches, _1,
			boost::ref(region),
			boost::ref(package),
			boost::ref(plane)));
}

QwHitContainer::iterator QwHitContainer::GetEndOfHits1 (
	EQwRegionID region,
	EQwDetectorPackage package,
	Int_t plane)
{
  iterator first = GetStartOfHits1(region, package, plane);
  return find_if (first, end(),
		  boost::bind<bool>(logical_not_,
			boost::bind(&QwHit::PlaneMatches, _1,
				boost::ref(region),
				boost::ref(package),
				boost::ref(plane))));
}


void QwHitContainer::GetSubList_Dir(EQwRegionID region, EQwDetectorPackage package, EQwDirectionID direction, std::vector<QwHit> &  sublist) {
  iterator p;
  for (p=GetStartOfHits(region,package,direction);p !=GetEndOfHits(region,package,direction);p++){
    sublist.push_back(*p);
  }
}

void QwHitContainer::GetSubList_Plane(EQwRegionID region, EQwDetectorPackage package, Int_t plane, std::vector<QwHit> &  sublist) {
  iterator p;
  for (p=GetStartOfHits1(region,package,plane);p !=GetEndOfHits1(region,package,plane);p++){
    sublist.push_back(*p);
  }
}


// Return the sublist of hits only in specified package, region, and detector plane
QwHitContainer* QwHitContainer::GetSubList_Plane (EQwRegionID region, EQwDetectorPackage package, Int_t plane) {
  QwHitContainer* sublist = new QwHitContainer;
  for (iterator hit = begin(); hit != end(); hit++)
    if (hit->PlaneMatches(region, package, plane))
      sublist->push_back(*hit);
  return sublist;
}

// Return the sublist of hits only in specified package, region, and detector plane
QwHitContainer* QwHitContainer::GetSubList_Dir (EQwRegionID region, EQwDetectorPackage package, EQwDirectionID dir) {
  QwHitContainer* sublist = new QwHitContainer;
  for (iterator hit = begin(); hit != end(); hit++)
    if (hit->DirMatches(region, package, dir))
      sublist->push_back(*hit);
  return sublist;
}


void QwHitContainer::Print (const Option_t* option) const
{
  if (! this) return;
  std::cout << *this << std::endl;
}

std::ostream& operator<< (std::ostream& stream, const QwHitContainer& hitlist)
{
  for (QwHitContainer::const_iterator hit = hitlist.begin(); hit != hitlist.end(); hit++)
    stream << *hit << std::endl;
  return stream;
}
