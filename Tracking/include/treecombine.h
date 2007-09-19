#ifndef TREECOMBINE_H
#define TREECOMBINE_H

#include <iostream>
#include "tree.h"
#include "treesort.h"

#define HASHSIZE 1024		/* power of 2 */
#define HASHMASK 1023		/* hashsize-1 */

#define magnet_center 275.0 //PLACEHOLDER VALUE

#ifndef DLAYERS
#define DLAYERS 10
#endif

class chi_hash {
public:
  chi_hash();
  double x, mx, cov[3], chi;
  double hit[DLAYERS];
  int    hits;
  chi_hash *next;
private:
};




class treecombine{

public:
treecombine();
~treecombine();

chi_hash *hasharr[HASHSIZE];

int chi_hashval( int n, Hit **hit );
void chi_hashclear(void);
void chi_hashinsert(Hit **hits, int n, double slope, double xshift, double cov[3],double chi);
int chi_hashfind( Hit **hits, int n, double *slope, double *xshift, double cov[3],double *chi);

int bestx(double *xresult, double dist_cut, Hit *h, Hit **ha);
int bestx(double *xresult,Hit *h,Hit **ha,int i);
void mul_do(int i,int mul,int l,int *r,Hit   *hx[DLAYERS][MAXHITPERLINE],  Hit **ha);
void weight_lsq(double *slope, double *xshift, double cov[3],double *chi,Hit **hits, int n);
void weight_lsq_r3(double *slope, double *xshift, double cov[3],double *chi,Hit **hits, int n,double z1,int offset,int tlayers);

int selectx(double *xresult,double dist_cut,Det *detec, Hit *hitarray[], Hit **ha);
int contains( double var, Hit **arr, int len);
double detZPosition( Det *det, double x, double slope_x, double *xval );

int TlCheckForX(double x1, double x2, double dx1, double dx2,double z1, double dz,TreeLine *treefill,enum EUppLow up_low,enum ERegion region,enum Etype type,enum Edir wdir/*,enum Eorientation orient*/,int  dlayer, int tlayer,int  iteration,int  stay_tuned);

int TlMatchHits(double x1,double x2,double z1, double dz,TreeLine *treefill,enum EUppLow up_low,enum ERegion region,enum Etype type,enum Edir dir,int bins,int tlayers);

int inAcceptance( enum EUppLow up_low,enum ERegion region,double cx, double mx,double cy, double my);
void TlTreeLineSort(TreeLine *tl,enum EUppLow up_low,enum ERegion region,enum Etype type,enum Edir dir/*,enum Eorientation orient*/,unsigned long bins,int tlayer,int dlayer);
int TcTreeLineCombine( TreeLine *wu, TreeLine *wv, TreeLine *wx,PartTrack *pt, int tlayer );
PartTrack *TlTreeCombine(TreeLine *uvl[2],long bins,enum EUppLow up_low,enum ERegion region,enum Etype type/*,enum Eorientation orient*/,int tlayer,int dlayer);
void ResidualWrite( Event *event);
/*


*/


private:


};


#endif
