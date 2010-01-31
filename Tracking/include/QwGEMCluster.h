/*!
 * \file   QwGEMCluster.h
 * \brief  Definition of the clusters of GEM hits
 *
 * \author Wouter Deconinck
 * \date   2010-01-30
 */

#ifndef QWGEMCLUSTER_H
#define QWGEMCLUSTER_H

// System headers
#include <iostream>

// Qweak headers
#include "VQwTrackingElement.h"
#include "QwHitContainer.h"

/**
 *  \class QwGEMCluster
 *  \ingroup QwTracking
 *  \brief Cluster finder for GEM hits
 *
 * Description to follow.
 *
 */
class QwGEMCluster: public VQwTrackingElement {

  public:

    /// \brief Default constructor
    QwGEMCluster(): VQwTrackingElement() { };
    /// \brief Destructor
    virtual ~QwGEMCluster() {
      delete fStripsR;
      delete fStripsPhi;
    };

    double GetR() const;
    double GetPhi() const;
    double GetX() const;
    double GetY() const;

  private:

    double fR;
    double fPhi;
    double fX;
    double fY;
    double fZ;

    QwHitContainer* fStripsR;
    QwHitContainer* fStripsPhi;

}; // class QwGEMCluster

#endif // QWGEMCLUSTER_H
