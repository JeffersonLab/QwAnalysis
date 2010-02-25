/*!
 * \file   QwGEMClusterFinder.cc
 * \brief  Implementation of the cluster finder for GEM hits
 *
 * \author Wouter Deconinck
 * \date   2010-01-31
 */

#include "QwGEMClusterFinder.h"

// System headers
#include <cmath>

// Qweak headers
#include "QwDetectorInfo.h"

/**
 * Find the clusters in the hit list
 * @param hits
 * @return
 */
std::vector<QwGEMCluster> QwGEMClusterFinder::FindClusters(const QwHitContainer* strips)
{
  // Create a list to fill with clusters
  std::vector<QwGEMCluster> list;

  // Take the simple average
  double sum = 0;
  double sum2 = 0;
  for (QwHitContainer::const_iterator strip = strips->begin(); strip != strips->end(); strip++) {
    // implicit upconvert from int to double
    sum  += strip->GetElement();
    sum2 += strip->GetElement() * strip->GetElement();;
  }
  double mean = sum / strips->size();
  double variance = sum2 / strips->size() - mean * mean;
  double sigma = sqrt(variance);

  // Create a new cluster
  QwGEMCluster cluster;
  // Copy the geometry info from the first strip
  cluster.SetDetectorInfo(strips->begin()->GetDetectorInfo());
  cluster.SetPackage(strips->begin()->GetPackage());
  cluster.SetRegion(strips->begin()->GetRegion());
  cluster.SetDirection(strips->begin()->GetDirection());
  cluster.SetPlane(strips->begin()->GetPlane());
  // Store the hits
  cluster.SetStrips(strips);
  // Store the results
  cluster.fStripMean = mean;
  cluster.fStripSigma = sigma;
  cluster.fStripVariance = variance;

  // TODO only one cluster for now
  list.push_back(cluster);

  return list;
};

