//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef QWTRACKINGTREECOMBINE_H
#define QWTRACKINGTREECOMBINE_H

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// Standard C and C++ headers
#include <iostream>
#include <utility>
using std::cout; using std::cerr; using std::endl;

#include "QwTypes.h"
#include "QwDetectorInfo.h"
#include "QwGeometry.h"

#include "globals.h"
#include "matrix.h"

#define HASHSIZE 1024		/* power of 2 */
#define HASHMASK 1023		/* hashsize-1 */

#define MAGNET_CENTER 275.0 //PLACEHOLDER VALUE

#ifndef DLAYERS
#warning Defining DLAYERS outside of the globals.h header file!
#define DLAYERS 10
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*!
    \todo The chi_hash class is legacy code which I have yet to abolish.  Its
    functions are used by various routines.  The goal is to eventually replace
    the need for this class.
*/
///
/// \ingroup QwTracking
class chi_hash {

  public:

    chi_hash() {
      hits = 0;
    };
    virtual ~chi_hash() {};

    double cx, mx, cov[3], chi;
    double hit[DLAYERS];
    int    hits;
    chi_hash *next;

  private:

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! \brief QwTrackingTreeCombine combines track segments and performs line fitting

QwTrackingTreeCombine performs many of the tasks involved with matching hits to track segments
and combining track segments into full tracks with lab coordinates.
*/

// Forward declarations
class QwHit;
class QwHitContainer;
class QwTrackingTreeRegion;
class QwTreeLine;
class QwPartialTrack;
class QwEvent;

class QwTrackingTreeCombine {

  public:

    QwTrackingTreeCombine();
    virtual ~QwTrackingTreeCombine();

    /// Set the debug level
    void SetDebugLevel (const int debuglevel) { fDebug = debuglevel; };
    /// Set the maximum road width
    void SetMaxRoad (const double maxroad) { fMaxRoad = maxroad; };
    /// Set the maximum X road width (?)
    void SetMaxXRoad (const double maxxroad) { fMaxXRoad = maxxroad; };

    /// Set the geometry
    void SetGeometry(const QwGeometry& geometry) { fGeometry = geometry; };

    /// \brief Select the left or right hit assignment for HDC hits
    int SelectLeftRightHit (double *xresult, double dist_cut,
		QwHitContainer *hitlist, QwHit **ha, double Dx = 0);

    int SelectLeftRightHit (QwHitContainer *hitlist, QwHit **ha, int bin, double width,double Dx = 0);
    /// \brief Select the left or right hit assignment for VDC hits
    QwHit* SelectLeftRightHit (double track_position, QwHit* hit);


    void mul_do (int i, int mul, int l, int *r, QwHit *hx[DLAYERS][MAXHITPERLINE], QwHit **ha);
    void weight_lsq (
		double& slope, double& offset, double cov[3], double& chi,
		QwHit **hits, int n);
    void weight_lsq_r3 (
		double& slope, double& offset, double cov[3], double& chi,
		QwHit **hits, int n, double z1, int wire_offset );


    int selectx (double *xresult, double dist_cut, QwHit *hitarray[], QwHit **ha);
    int contains (double var, QwHit **arr, int len);

    bool TlCheckForX (
		double x1, double x2, double dx1, double dx2, double Dx, double z1, double dz,
		QwTreeLine *treefill, QwHitContainer *hitlist,
		int  dlayer, int tlayer, int iteration, int stay_tuned, double width);

    int TlMatchHits (
		double x1, double x2, double z1, double z2,
		QwTreeLine *treeline, QwHitContainer *hitlist,
		int tlayers);

    bool InAcceptance (EQwDetectorPackage package, EQwRegionID region, double cx, double mx, double cy, double my);
    void TlTreeLineSort (QwTreeLine *tl, QwHitContainer *hl, EQwDetectorPackage package, EQwRegionID region, EQwDirectionID dir, unsigned long bins, int tlayer, int dlayer, double width);

    // Combine the tree lines in partial tracks for region 2 and region 3
    QwPartialTrack* TcTreeLineCombine (
		QwTreeLine *wu,
		QwTreeLine *wv,
		QwTreeLine *wx,
		int tlayer);
     QwPartialTrack* TcTreeLineCombine (
		QwTreeLine *wu,
		QwTreeLine *wv,
		QwTreeLine *wx,
		int tlayer,bool opt);

    QwPartialTrack* TcTreeLineCombine (
		QwTreeLine *wu,
		QwTreeLine *wv,
		int tlayer);
    QwPartialTrack* TcTreeLineCombine (
		QwTreeLine *wu,
		QwTreeLine *wv);

    QwPartialTrack* TlTreeCombine (
		QwTreeLine *uvl[kNumDirections], EQwDetectorPackage package,
		EQwRegionID region, int tlayer, int dlayer);

    void ResidualWrite (QwEvent *event);

    int r2_TrackFit  (const int num, QwHit **hits, double *fit, double *cov, double &chi,double * signedresidual);
    int r2_TrackFit  (const int num, QwHit **hits, double *fit, double *cov, double &chi,double * signedresidual, bool opt);
    int r3_TrackFit_deprecated  (const int num, QwHit **hits, double *fit, double *cov, double &chi, double uv2xy[2][2]);
    QwPartialTrack* r3_TrackFit (const int num, QwHit **hits,
        QwTreeLine *wu, QwTreeLine *wv);

  private:

    int fDebug;		///< Debug level

    double fMaxRoad;
    double fMaxXRoad;

    QwGeometry fGeometry;

    /// Maximum number of missed planes in region 2
    int fMaxMissedPlanes;
    /// Maximum number of missed wires in region 3
    int fMaxMissedWires;

}; // class QwTrackingTreeCombine

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif // QWTRACKINGTREECOMBINE_H
