/*! \file   QwMatrixLookup.h
 *
 *  \author Wouter Deconinck <wdconinc@mit.edu>
 *
 *  \date   Mon Mar  1 11:48:48 EST 2010
 *  \brief  Definition of the matrix lookup bridging method
 *
 *  \ingroup QwTracking
 *
 */

// System headers
#include <vector>

// Qweak headers
#include "QwOptions.h"
#include "VQwBridgingMethod.h"

// Forward declarations
template <class value_t, unsigned int value_n> class QwInterpolator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef __QWMATRIXLOOKUP_H__
#define __QWMATRIXLOOKUP_H__

class QwMatrixLookup: public VQwBridgingMethod {

  public:

    /// \brief Default constructor
    QwMatrixLookup(QwOptions& options);
    /// \brief Destructor
    virtual ~QwMatrixLookup();

    /// \brief Load the trajectory matrix from disk
    bool LoadTrajMatrix(const std::string filename);
    /// \brief Write the trajectory matrix to disk
    bool WriteTrajMatrix(const std::string filename);

    /// \brief Bridge from the front to back partial track
    const QwTrack* Bridge(const QwPartialTrack* front, const QwPartialTrack* back);

  private:

    /// Front and back reference planes
    double fFrontRefPlane;
    double fBackRefPlane;

    /// Look-up table minimum, maximum and step size
    std::vector<double> fMin;
    std::vector<double> fMax;
    std::vector<double> fStep;
    /// Look-up table
    QwInterpolator<float,4> *fMatrix;

}; // class QwMatrixLookup

#endif // __QWMATRIXLOOKUP_H__
