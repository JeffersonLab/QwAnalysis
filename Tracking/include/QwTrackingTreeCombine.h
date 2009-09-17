//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef QWTRACKINGTREECOMBINE_H
#define QWTRACKINGTREECOMBINE_H

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// Standard C and C++ headers
#include <iostream>
using std::cout; using std::cerr; using std::endl;

// Boost uBLAS (linear algebra) headers
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

// Qweak headers
#include "QwHit.h"
#include "QwTrackingTree.h"
#include "QwTrackingTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"
#include "QwEvent.h"

#include "uv2xy.h"
#include "matrix.h"

#define HASHSIZE 1024		/* power of 2 */
#define HASHMASK 1023		/* hashsize-1 */

#define MAGNET_CENTER 275.0 //PLACEHOLDER VALUE

#ifndef DLAYERS
#define DLAYERS 10
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*!
    \todo The chi_hash class is legacy code which I have yet to abolish.  Its
    functions are used by various routines.  The goal is to eventually replace
    the need for this class.
*/
///
/// \ingroup QwTrackingAnl
class chi_hash {

  public:

    chi_hash() {
      hits = 0;
    };
    ~chi_hash() {};

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
class QwTrackingTreeCombine {

  public:

    QwTrackingTreeCombine();
    ~QwTrackingTreeCombine();

    void SetDebugLevel (const int debuglevel) { fDebug = debuglevel; };


    int bestx (double *xresult, double dist_cut,
		QwHitContainer *hitlist, QwHit **ha, double Dx = 0);
    int bestx (double *xresult,
		QwHit *h, QwHit **ha);


    void mul_do (int i, int mul, int l, int *r, QwHit *hx[DLAYERS][MAXHITPERLINE], QwHit **ha);
    void weight_lsq (
		double *slope, double *xshift, double cov[3], double *chi,
		QwHit **hits, int n, int tlayers);
    void weight_lsq_r3 (
		double *slope, double *xshift, double cov[3], double *chi,
		QwHit **hits, int n, double z1, int offset, int tlayers);

    int selectx (double *xresult, double dist_cut, Det *detec, QwHit *hitarray[], QwHit **ha);
    int contains (double var, QwHit **arr, int len);
    double detZPosition (Det *det, double x, double slope_x, double *xval);

    bool TlCheckForX (
		double x1, double x2, double dx1, double dx2, double Dx, double z1, double dz,
		QwTrackingTreeLine *treefill, QwHitContainer *hitlist,
		EQwDetectorPackage package, EQwRegionID region, EQwDetectorType type, EQwDirectionID dir,
		int  dlayer, int tlayer, int iteration, int stay_tuned, double width);

    int TlMatchHits (
		double x1, double x2, double z1, double z2,
		QwTrackingTreeLine *treeline, QwHitContainer *hitlist,
		int tlayers);

    bool InAcceptance (EQwDetectorPackage package, EQwRegionID region, double cx, double mx, double cy, double my);
    void TlTreeLineSort (QwTrackingTreeLine *tl, QwHitContainer *hl, EQwDetectorPackage package, EQwRegionID region, EQwDetectorType type, EQwDirectionID dir, unsigned long bins, int tlayer, int dlayer, double width);

    // Combine the tree lines in partial tracks for region 2 and region 3
    QwPartialTrack* TcTreeLineCombine (
		QwTrackingTreeLine *wu,
		QwTrackingTreeLine *wv,
		QwTrackingTreeLine *wx,
		int tlayer);
    QwPartialTrack* TcTreeLineCombine (
		QwTrackingTreeLine *wu,
		QwTrackingTreeLine *wv,
		int tlayer);
    QwPartialTrack* TcTreeLineCombine2 (
		QwTrackingTreeLine *wu,
		QwTrackingTreeLine *wv,
		int tlayer);

    QwPartialTrack* TlTreeCombine (
		QwTrackingTreeLine *uvl[kNumDirections], long bins, EQwDetectorPackage package,
		EQwRegionID region, EQwDetectorType type, int tlayer, int dlayer,
		QwTrackingTreeRegion **myTreeRegion);

    void ResidualWrite (QwEvent *event);

    int r3_TrackFit (int Num, QwHit **Hit, double *fit, double *cov, double *chi, double uv2xy[2][2]);
    int r3_TrackFit2 (int Num, QwHit **Hit, double *fit, double *cov, double *chi);

    int checkR3 (QwPartialTrack *pt, EQwDetectorPackage package);

  private:

    int fDebug;


    // The following is largely useless (or at least the use is not understood).
    // Only chi_hashinsert is ever called, but never anything is searched in the
    // hash table...
    chi_hash *hasharr[HASHSIZE];

    int chi_hashval (int n, QwHit **hit);
    void chi_hashclear ();
    void chi_hashinsert (QwHit **hits, int n, double slope, double xshift, double cov[3], double chi);
    int chi_hashfind (QwHit **hits, int n, double *slope, double *xshift, double cov[3], double *chi);

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif // QWTRACKINGTREECOMBINE_H
