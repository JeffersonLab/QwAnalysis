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

    /// \brief Find the cluster in the hit list
    std::vector<QwGEMCluster> FindClusters(const QwHitContainer* hits);

  private:

}; // class QwGEMClusterFinder

#endif // QWGEMCLUSTERFINDER_H
