/*!
 * \file   QwGEMClusterFinder.h
 * \brief  Definition of the cluster finder for GEM hits
 *
 * \author Wouter Deconinck
 * \date   2010-01-30
 */

#ifndef QWGEMCLUSTERFINDER_H
#define QWGEMCLUSTERFINDER_H

// System headers
#include <iostream>

// Qweak headers
#include "QwGEMCluster.h"

/**
 *  \class QwGEMClusterFinder
 *  \ingroup QwTracking
 *  \brief Cluster finder for GEM hits
 *
 * Description to follow.
 *
 */
class QwGEMClusterFinder {

  public:

    /// \brief Default constructor
    QwGEMClusterFinder() { };
    /// \brief Destructor
    virtual ~QwGEMClusterFinder() { };

    /// \brief Set the detector info pointers for the r and phi detectors
    void SetDetectorInfo(const QwDetectorInfo* info_r, const QwDetectorInfo* info_phi);


    /// \brief Find the cluster corresponding to two sets of hits
    std::vector<QwGEMCluster> FindClusters(QwHitContainer* hits_r, QwHitContainer* hits_phi);

  private:


}; // class QwGEMClusterFinder

#endif // QWGEMCLUSTERFINDER_H
