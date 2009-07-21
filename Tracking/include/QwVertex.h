#ifndef QWVERTEX_H
#define QWVERTEX_H

#include "TObject.h"

/*------------------------------------------------------------------------*//*!

 \brief Contains vertex information

 A QwVertex contains the coordinates and uncertainties of a vertex.  It could
 link back to the tracks and partial tracks that intersect at this vertex.

 \ingroup QwTrackingAnl

*//*-------------------------------------------------------------------------*/
class QwVertex: public TObject {

  public:

    QwVertex();
    ~QwVertex();

    double GetDistance();	/*! return distance between tracks at vertex */

  public:

    double coord[3];		/*!< coordinates of vertex */
    double av[3][3];		/*!< error matrix */
    double maxresidue;		/*!< max residue of tracks */
    int nTracks;		/*!< number of tracks in vertex */

};

#endif // QWVERTEX_H
