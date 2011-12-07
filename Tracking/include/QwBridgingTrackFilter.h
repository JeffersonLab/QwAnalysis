/*! \file   QwBridgingTrackFilter.h
 *
 *  \author Jie Pan <jpan@jlab.org>
 *  \author Wouter Deconinck <wdconinc@mit.edu>
 *
 *  \date   Sun May 23 16:43:37 EDT 2010
 *  \brief  Definition of the track filter for the bridging methods
 *
 *  \ingroup QwTracking
 */

#ifndef __QWBRIDGINGTRACKFILTER_H__
#define __QWBRIDGINGTRACKFILTER_H__

// System headers
#include <iostream>
#include <vector>

// Forward declarations
class QwPartialTrack;


/**
 *  \class QwBridgingTrackFilter
 *  \ingroup QwTracking
 *  \brief Track filter for the bridging methods
 *
 * Not all combinations of front and back tracks lead to a valid bridged
 * track.  This object filters combinations of front and back tracks and
 * only accepts combinations that satisfy certain criteria.
 */
class QwBridgingTrackFilter {

  public:

    /// \brief Default constructor
    QwBridgingTrackFilter();
    /// \brief Destructor
    virtual ~QwBridgingTrackFilter() { };

    /// List of possible failure modes for the filter
    enum EStatus {
      kPass = 0,
      kFailThetaFront,
      kFailThetaBack,
      kFailPhiFront,
      kFailPhiBack,
      kFailDiffTheta,
      kFailDiffPhi,
      kFailVertexZ,
      kFailUnknown
    };

    /// \brief Filter front and back track combinations
    EStatus Filter(const QwPartialTrack* front,
                   const QwPartialTrack* back) const;

    /// \todo Setters for the filter angle boundaries (wdc)

  private:

    /// Angle and position boundaries for the filter
    double fMinTheta;
    double fMaxTheta;
    double fMinPhi;
    double fMaxPhi;
    double fMinDiffTheta;
    double fMaxDiffTheta;
    double fMinDiffPhi;
    double fMaxDiffPhi;
    double fMinVertexZ;
    double fMaxVertexZ;

}; // class QwBridgingTrackFilter

#endif // __QWBRIDGINGTRACKFILTER_H__
