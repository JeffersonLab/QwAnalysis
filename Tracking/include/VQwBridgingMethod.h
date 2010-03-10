/*!
 * \file   VQwBridgingMethod.h
 * \brief  Definition of the bridging method interface
 *
 * \author Wouter Deconinck
 * \date   2010-01-23
 */

#ifndef VQWBRIDGINGMETHOD_H
#define VQWBRIDGINGMETHOD_H

// System headers
#include <iostream>

// Qweak headers

// Forward declarations
class QwTrack;
class QwPartialTrack;

/**
 *  \class VQwBridgingMethod
 *  \ingroup QwTracking
 *  \brief Interface to the various bridging methods
 */
class VQwBridgingMethod {

  public:

    /// Default constructor
    VQwBridgingMethod() { };
    /// Destructor
    virtual ~VQwBridgingMethod() { };

    /// \brief Bridge from the front to back partial track (pure virtual)
    virtual int Bridge(QwPartialTrack* front, QwPartialTrack* back) = 0;

    /// Clear the list of tracks
    virtual void ClearListOfTracks() { fListOfTracks.clear(); };
    /// Get the list of tracks that was found
    virtual std::vector<QwTrack*> GetListOfTracks() const { return fListOfTracks; };

  private:

    /// List of tracks that were found in this bridging attempt
    std::vector<QwTrack*> fListOfTracks;

}; // class VQwBridgingMethod

#endif // VQWBRIDGINGMETHOD_H
