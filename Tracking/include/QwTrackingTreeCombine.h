//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef QWTRACKINGTREECOMBINE_H
#define QWTRACKINGTREECOMBINE_H

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// Standard C and C++ headers
#include <iostream>

// Qweak headers
#include "QwHit.h"
#include "QwTrackingTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"
#include "QwEvent.h"
#include "uv2xy.h"

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

    chi_hash();

    double x, mx, cov[3], chi;
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

    void SetDebugLevel (int debuglevel) { debug = debuglevel; };

    chi_hash *hasharr[HASHSIZE];

    int chi_hashval (int n, QwHit **hit);
    void chi_hashclear (void);
    void chi_hashinsert (QwHit **hits, int n, double slope, double xshift, double cov[3], double chi);
    int chi_hashfind (QwHit **hits, int n, double *slope, double *xshift, double cov[3], double *chi);

    int bestx (double *xresult, double dist_cut, QwHit *h, QwHit **ha, double Dx = 0);
    int bestx (double *xresult, QwHit *h, QwHit *ha);


    void mul_do (int i, int mul, int l, int *r, QwHit *hx[DLAYERS][MAXHITPERLINE], QwHit **ha);
    void weight_lsq (double *slope, double *xshift, double cov[3],double *chi, QwHit **hits, int n,int tlayers);
    void weight_lsq_r3 (double *slope, double *xshift, double cov[3],double *chi, QwHit **hits, int n,double z1,int offset,int tlayers);

    int selectx(double *xresult,double dist_cut,Det *detec, QwHit *hitarray[], QwHit **ha);
    int contains( double var, QwHit **arr, int len);
    double detZPosition( Det *det, double x, double slope_x, double *xval );

    int TlCheckForX (double x1, double x2, double dx1, double dx2,double Dx,double z1, double dz,QwTrackingTreeLine *treefill,EQwDetectorPackage package, EQwRegionID region,enum EQwDetectorType type,EQwDirectionID wdir,int  dlayer, int tlayer,int  iteration,int  stay_tuned, QwTrackingTreeRegion **myTreeRegion);

    int TlMatchHits (double x1,double x2,double z1, double dz,QwTrackingTreeLine *treefill,EQwDetectorPackage package, EQwRegionID region,enum EQwDetectorType type,EQwDirectionID dir,int tlayers);

    int inAcceptance (EQwDetectorPackage package, EQwRegionID region, double cx, double mx, double cy, double my);
    void TlTreeLineSort (QwTrackingTreeLine *tl, EQwDetectorPackage package, EQwRegionID region, EQwDetectorType type, EQwDirectionID dir/*,enum Eorientation orient*/, unsigned long bins, int tlayer, int dlayer, QwTrackingTreeRegion **myTreeRegion);
    int TcTreeLineCombine (QwTrackingTreeLine *wu, QwTrackingTreeLine *wv, QwTrackingTreeLine *wx, QwPartialTrack *pt, int tlayer);
    int TcTreeLineCombine (QwTrackingTreeLine *wu, QwTrackingTreeLine *wv, QwPartialTrack *pt, int tlayer);
    int TcTreeLineCombine2 (QwTrackingTreeLine *wu, QwTrackingTreeLine *wv, QwPartialTrack *pt, int tlayer);

    QwPartialTrack *TlTreeCombine (QwTrackingTreeLine *uvl[kNumDirections], long bins, EQwDetectorPackage package, EQwRegionID region,enum EQwDetectorType type/*,enum Eorientation orient*/, int tlayer, int dlayer, QwTrackingTreeRegion **myTreeRegion);
    void ResidualWrite (QwEvent *event);

    int r3_TrackFit (int Num, QwHit **Hit, double *fit, double *cov, double *chi, double uv2xy[2][2]);
    int r3_TrackFit2 (int Num, QwHit **Hit, double *fit, double *cov, double *chi);

    int checkR3 (QwPartialTrack *pt, EQwDetectorPackage package);

  private:

    int debug;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif // QWTRACKINGTREECOMBINE_H
