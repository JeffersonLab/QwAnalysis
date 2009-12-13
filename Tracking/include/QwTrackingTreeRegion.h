/*!
 * \file   QwTrackingTree.h
 * \brief  Definition of the track search tree
 *
 * \author Wouter Deconinck
 * \date   2009-12-11
 */

#ifndef QWTRACKINGTREEREGION_H
#define QWTRACKINGTREEREGION_H

// System headers
#include <iostream>

// Qweak headers
#include "VQwTrackingElement.h"
#include "shortnode.h"
#include "globals.h"

using QwTracking::shortnode;

/**
 * \class	QwTrackingTreeRegion
 * \ingroup	QwTracking
 *
 * \author	Burnham Stokes <bestokes@jlab.org>
 * \author	Wouter Deconinck <wdconinc@jlab.org>
 * \date	2009-09-04 18:06:23
 *
 * \brief A container for the pattern databases for each detector region.
 *
 * It is useful to separate the pattern database into multiple detector regions
 * to reduce the extent by which a set of hits are compared to a pattern.
 * For instance, there are two databases for the region 3 set of detectors.
 * One for the 'upper' and one for the 'lower' detector sets.  Track segments
 * in the area downstream of the QTOR magnet are approximated by straight lines.
 * Only the VDC is designed to measure track positions, so these two databases
 * designed to contain patterns which ressemble tracks passing through the VDC.
 *
 */
class QwTrackingTreeRegion: public VQwTrackingElement {

  public:

    /// \brief Default constructor
    QwTrackingTreeRegion();
    /// \brief Default destructor
    ~QwTrackingTreeRegion();

    /// \brief Is this region searchable?
    const bool IsSearchable() const { return fSearchable; };
    /// \brief Set this tree region to searchable
    void SetSearchable(bool searchable = true) { fSearchable = searchable; };

    /// \brief Get the node to this tree region
    shortnode* GetNode() { return &fNode; };

    /// \brief Get the width
    const double GetWidth() const { return fWidth; };
    /// \brief Set the width
    void SetWidth(double width) { fWidth = width; };

  private:

    int fDebug;		///< Debug level

    bool fSearchable;	///< Is this tree region searchable?

    shortnode fNode;	///< Top node of this tree region
    double fWidth;	///< Width of the detector plane (i.e. number of wires * wire spacing)

}; // class QwTrackingTreeRegion

#endif // QWTRACKINGTREEREGION_H
