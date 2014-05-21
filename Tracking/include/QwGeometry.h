/*
 * QwGeometry.h
 *
 *  Created on: Mar 29, 2011
 *  \author: wdconinc
 */

#ifndef QWGEOMETRY_H_
#define QWGEOMETRY_H_

// System headers
#include <vector>
#include <algorithm>

// Qweak headers
#include "QwDetectorInfo.h"

// Forward declarations
class QwDetectorInfo;

/**
 *  \class QwGeometry
 *  \ingroup QwTracking
 *
 *  \brief Collection of QwDetectorInfo pointers that specifies an experimental geometry
 */
class QwGeometry: public std::vector<QwDetectorInfo*> {

  private:

    /// Functor for sorting QwDetectorInfo pointers
    struct compare {
      bool operator()(const QwDetectorInfo *lhs, const QwDetectorInfo *rhs) { return *lhs < *rhs; }
    };

  public:

    /// Default constructor
    QwGeometry() { clear(); };
    /// Copy constructor
    QwGeometry(const QwGeometry& that): std::vector<QwDetectorInfo*>(that) { };
    /// Virtual destructor
    virtual ~QwGeometry() { };

    /// Assignment operator
    QwGeometry& operator=(const QwGeometry& that) {
      if (this != &that) {
        clear();
        push_back(that);
      }
      return *this;
    }

    /// Add single detector object
    void push_back(QwDetectorInfo& detector) {
      std::vector<QwDetectorInfo*>::push_back(&detector);
      std::sort(begin(),end(),compare());
    }

    /// Add single detector pointer
    void push_back(QwDetectorInfo* detector) {
      std::vector<QwDetectorInfo*>::push_back(detector);
      std::sort(begin(),end(),compare());
    }

    /// Add another geometry
    void push_back(const QwGeometry& detectors) {
      QwGeometry::const_iterator i;
      for (i = detectors.begin(); i != detectors.end(); i++)
        std::vector<QwDetectorInfo*>::push_back(*i);
      std::sort(begin(),end(),compare());
    }

    /// Add vector of detectors
    void push_back(std::vector<QwDetectorInfo>& detectors) {
      std::vector<QwDetectorInfo>::iterator i;
      for (i = detectors.begin(); i != detectors.end(); i++)
        std::vector<QwDetectorInfo*>::push_back(&(*i));
      std::sort(begin(),end(),compare());
    }

    /// Get detectors in given region
    const QwGeometry in(const EQwRegionID& r) const {
      QwGeometry results;
      for (const_iterator i = begin(); i != end(); i++)
        if ((*i)->GetRegion() == r)
          results.push_back(*i);
      return results;
    }

    /// Get detectors in given package
    const QwGeometry in(const EQwDetectorPackage& p) const {
      QwGeometry results;
      for (const_iterator i = begin(); i != end(); i++)
        if ((*i)->GetPackage() == p)
          results.push_back(*i);
      return results;
    }

    /// Get detectors in given direction
    const QwGeometry in(const EQwDirectionID& d) const {
      QwGeometry results;
      for (const_iterator i = begin(); i != end(); i++)
        if ((*i)->GetDirection() == d)
          results.push_back(*i);
      return results;
    }

    /// Get detectors of given type
    const QwGeometry of(const EQwDetectorType& t) const {
      QwGeometry results;
      for (const_iterator i = begin(); i != end(); i++)
        if ((*i)->GetType() == t)
          results.push_back(*i);
      return results;
    }

    /// Get detectors like specified detector (same region, same package, same type)
    const QwGeometry as(const QwDetectorInfo* d) const {
      QwGeometry results;
      for (const_iterator i = begin(); i != end(); i++)
        if ((*i)->GetPackage()   == d->GetPackage()
         && (*i)->GetRegion()    == d->GetRegion()
         && (*i)->GetDirection() == d->GetDirection()
         && (*i)->GetType()      == d->GetType())
          results.push_back(*i);
      return results;
    }

    // Print function
    void Print() const;

    // Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const QwGeometry& detectors);

};

/**
 * Print function
 */
inline void QwGeometry::Print() const
{
  for (QwGeometry::const_iterator i = begin(); i != end(); i++) {
    if (*i)
      (*i)->Print();
    else
      QwOut << "(null)" << QwLog::endl;
    QwOut << QwLog::endl;
  }
}

/**
 * Output stream operator
 * @param stream Output stream
 * @param detectors Detector geometry object
 * @return Output stream
 */
inline std::ostream& operator<< (std::ostream& stream, const QwGeometry& detectors)
{
  for (QwGeometry::const_iterator i = detectors.begin(); i != detectors.end(); i++) {
    if (*i)
      stream << *(*i) << std::endl;
    else
      stream << "(null)" << std::endl;
  }
  return stream;
}

#endif /* QWGEOMETRY_H_ */
