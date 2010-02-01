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
 *  \brief Cluster of GEM hits
 *
 * Description to follow.
 *
 */
class QwGEMCluster: public VQwTrackingElement {

  public:

    /// \brief Default constructor
    QwGEMCluster() { };
    /// \brief Destructor
    virtual ~QwGEMCluster() { };

    // Set the strips
    void SetStrips(const QwHitContainer* strips) {
      fStrips.Append(strips);
    };
    void SetStrips(const QwHitContainer& strips) {
      fStrips.Append(strips);
    };

    // Accessors for strip mean and sigma
    const double GetStripMean() const { return fStripMean; };
    const double GetStripSigma() const { return fStripSigma; };
    const double GetStripVariance() const { return fStripVariance; };

    // Accessors for cluster position and uncertainty
    const double GetPosition() const { return fPosition; };
    const double GetUncertainty() const { return fUncertainty; };

    //! \brief Output stream operator
    friend ostream& operator<< (ostream& stream, const QwGEMCluster& cluster);

    /// The cluster finder is friend of the clusters
    friend class QwGEMClusterFinder;

  private:

    // Strip mean, standard deviation and variance
    double fStripMean;
    double fStripSigma;
    double fStripVariance;

    // Cluster position and uncertainty
    double fPosition;
    double fUncertainty;

    // Strip list for the cluster
    QwHitContainer fStrips;

}; // class QwGEMCluster

/**
 * Output stream operator
 * @param stream Stream
 * @param cluster Cluster object
 * @return Output stream
 */
inline ostream& operator<< (ostream& stream, const QwGEMCluster& cluster)
{
  stream << "cluster: ";
  stream << "package "   << "?UD"[cluster.GetPackage()] << ", ";
  stream << "region "    << "123"[cluster.GetRegion()] << ", ";
  stream << "dir "       << "?xyuvrf"[cluster.GetDirection()] << ", ";
  stream << "plane "     << cluster.GetPlane();

  if (cluster.GetDetectorInfo()) stream << " (detector " << cluster.GetDetectorInfo() << "), ";
  else                           stream << ", ";

  stream << "strip "  << cluster.fStripMean << " +/- " << cluster.fStripSigma << ", ";
  stream << "position " << cluster.fPosition << " +/- " << cluster.fUncertainty;

  return stream;
};

#endif // QWGEMCLUSTER_H
