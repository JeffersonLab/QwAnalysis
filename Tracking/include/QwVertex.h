/**
 * \class	QwVertex	QwVertex.h
 *
 * \brief Contains vertex information
 *
 * A QwVertex contains the coordinates and uncertainties of a vertex.  It could
 * link back to the tracks and partial tracks that intersect at this vertex.
 *
 * \ingroup QwTrackingAnl
 *
 */

#ifndef QWVERTEX_H
#define QWVERTEX_H

#include "TObject.h"

// Forward declarations
class QwTrack;

class QwVertex: public TObject {

  public:

    QwVertex();
    QwVertex(QwTrack track1, QwTrack track2);
    ~QwVertex();

    /// Return the distance between the tracks at the vertex
    double GetDistance();

  public:

    double fCoord[3];		///< coordinates of vertex
    double av[3][3];		///< error matrix
    double maxresidue;		///< max residue of tracks
    int fNTracks;		///< number of tracks in vertex

};

#endif // QWVERTEX_H
