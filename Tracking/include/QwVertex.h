/**
 * \class	QwVertex	QwVertex.h
 *
 * \brief Contains vertex information
 *
 * A QwVertex contains the coordinates and uncertainties of a vertex.  It could
 * link back to the tracks and partial tracks that intersect at this vertex.
 *
 * \ingroup QwTracking
 *
 */

#ifndef QWVERTEX_H
#define QWVERTEX_H

// ROOT headers
#include "TObject.h"
#include "TVector3.h"

// Forward declarations
class QwTrack;

class QwVertex: public TObject {

  public:

    QwVertex();
    QwVertex(const TVector3& position);
    QwVertex(const QwTrack& track1, const QwTrack& track2);
    QwVertex(double x, double y, double z);
    virtual ~QwVertex() { };

    /// Return the distance between the tracks at the vertex
    double GetDistance();

  public:

    double fCoord[3];		///< coordinates of vertex
    double fCov[3][3];		///< error matrix
    double maxresidue;		///< max residue of tracks
    int fNTracks;		///< number of tracks in vertex

};

#endif // QWVERTEX_H
