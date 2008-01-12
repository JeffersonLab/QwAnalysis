#ifndef TREE_H
#define TREE_H

#include<iostream>


#define DLAYERS 4		/* max number of detectors of same dir
				   in either the front or the back */
#define TLAYERS 8		/* max number of detectors combined
				   for left-right ambiguity resolution */
#define MAXHITPERLINE 100	/* maximum number of hits in 1 detector plane
				   which will be considered to belong to the
				   track, if several hits are near the line */
#define HSHSIZ 511
#define TREELINABORT 1000 

class treenode;
class shorttree;
class nodenode;
class shortnode;
class treeregion;
class tree;
class Hit;
class Det;
class TreeLine;
class PartTrack;
class Track;
class Bridge;
class Vertex;
class Event;
class Options;

//____________________________________________
enum EUppLow    {		/* upper or lower part of the detector */
  w_nowhere = -1, w_upper, w_lower
};
enum ERegion {		/* detector region */
  r1=1, r2, r3
};
enum Etype      {		/* detector type */
  d_drift, d_gem, d_trigscint, d_cerenkov
};
enum Edir       {		/* chamber wire orientation */
  null_dir = 0,
  u_dir, v_dir, x_dir, y_dir
};

/*
enum Eorientation{		// tracking orientation 
  o_one=1, o_two, o_three, o_four
};
*/





//____________________________________________
/*! \brief A treenode contains the bits which make up a tree pattern.

The treenode also has a pointer to its father and a pointer to its 
son nodenodes.  Each following generation of a treenode will have
a higher bit resolution.
*/
class treenode{

public:
treenode *genlink;
nodenode *son[4];
int maxlevel,minlevel;
int bits;
int bit[TLAYERS];
int xref;
int pattern_offset;
treenode();
~treenode();
void print();


private:

};
//____________________________________________
/*! \brief Creates and manages the treesearch pattern 
database.

The pattern database is used to determine whether subsets
of hits resemble track segments.
*/
class tree{

public:
//objects
int maxlevel;			/* maximum level of the bin division w/in 
                                    the treesearch database, i.e.:
                                    resolution = (detwidth)/(2^maxlevel)   */
int tlayers;
int hshsiz;
int npat;
treenode *generic[HSHSIZ];
double zList[TLAYERS];
double detwidth;
double rcSET_rMaxSlope;//this needs to be defined by rcSET.rMaxSlope
treenode father;
int xref;
int maxref;


//functions
tree();
~tree();
void rcInitTree();
int consistent(treenode *tst, int level,enum Etype type,enum ERegion region);
treenode * existent(treenode *tst, int hash);
treenode *nodeexists(nodenode *nd, treenode *tr);
treenode * treedup(treenode *todup);
void marklin(treenode *Father,int level,enum Etype type,enum ERegion region);

void treeout(treenode *tn, int level, int off);
void printtree(treenode *tn);
void freetree();
treeregion * readtree(char *filename, int levels, int tlayers, double rwidth, int dontread);
long writetree(char *fn, treenode *tn, int levels, int tlayers, double width);
treeregion * inittree(char *filename, int levels, int tlayer, double width,enum Etype type, enum ERegion region);
treenode * _inittree(int tlayer,enum Etype type,enum ERegion region);//should be moved to private once inittree() is written
private:

int _writetree(treenode *tn, FILE *fp, int tlayers);
int _readtree(FILE *f, shorttree *stb, shortnode **fath, int tlayers);
};
//____________________________________________
/*! \brief A nodenode is used as a pointer which links
treenodes to their siblings.

Together with the treenode, any tree pattern can be
related to any of its family members.  This allows the
tree search algorithms to quickly move through the database
to identify matching patterns.

*/
class nodenode{

public:
nodenode();
~nodenode();
nodenode *next;
treenode *tree;

private:

};



//____________________________________________
/*! \brief Similar to a nodenode */
class shortnode{

public:
shortnode();
~shortnode();
shortnode *next;
shorttree *tree;

private:

};
//____________________________________________
/*! \brief Similar to a treenode.
*/
class shorttree{

public:
shorttree();
~shorttree();
int maxlevel;
int minlevel,bits;
int xref;
int bit[TLAYERS];
shortnode *son[4];
void print();

private:

};
//____________________________________________
/*! \brief A container for the pattern databases for each detector region.

It is useful to separate the pattern database into multiple detector regions
to reduce the extent by which a set of hits are compared to a pattern.  For instance,
there are two databases for the region 3 set of detectors.  One for the 'upper' and one
for the 'lower' detector sets.  Track segments in the area downstream of the QTOR magnet
are approximated by straight lines.  Only the VDC is designed to measure track positions,
so these two databases are designed to contain patterns which resemble tracks passing
through the VDC.

*/
class treeregion{

public:
treeregion();
~treeregion();
int searchable;
shortnode node;
double rWidth;
int nLayers;
int dLayers;
double rZPos[TLAYERS];

private:

};
//____________________________________________
/*! \brief Contains hit data and information regarding the hit detector

Hits for individual detector elements are strung together.  They also
have a pointer back to the detector in case more information about the
hit is needed.  The various position values are used in multiple ways,
and therefore are not strictly defined.
*/
class Hit{

public:
int wire;//wire ID
double Zpos;//Z position of hit
double rPos1;			/* rPos1 and                         */
double rPos2;			/* rPos2 from level II decoding      */
double Resolution;//resolution of this specific hit
double rTrackResolution;//tracking road width around hit
Det *detec;//which detector
Hit *next, *nextdet;//next hit and next hit in same detector
int ID;//event ID
bool  used;		/* hit is used by a track            */
double rResultPos;/* Resulting hit posisition          */
double rPos;			/* Position of from track finding    */

private:


};
/*! \brief Describes a detector element

The Det class hold information about a detector,
such as its position, orientation, and size.  In the case of
drift chambers, it has information about the wires.
*/
class Det{

public:
char cName[8];//Name of the chamber
char cType[8];//Detector Type
double Zpos;//Z position
double Rot;//rotation angle
double rRotCos,rRotSin;	/* cos and sin of the rotation angle */
double resolution;//resolution of the chamber
double TrackResolution;//tracking resolution
double SlopeMatching;//front/back track segment slope matching

enum EUppLow upplow;
enum ERegion region;
enum Etype type;
enum Edir dir;


double center[2];//x and y position of detector center
double width[3];//width in x and y
double WireSpacing;
double PosOfFirstWire;
double rCos;
double rSin;
int NumOfWires;
Hit *hitbydet;//hitlist
Det *nextsame;//same wire orientation
int samesearched;

int  ID;			/*Id representing a detector ID number.  This is needed to separate detectors when comparing hits.  */

private:

};
//____________________________________________
/*! \brief A container for track information

The TreeLine has a pointer to a set of hits.  It is passed 
to various track fitting procedures and carries around 
the fit results.
*/
class TreeLine {		/* used for found tracks */

public:
  TreeLine();
  ~TreeLine();

  bool isvoid;		/* has been found void */
  TreeLine *next;	/* link to next list element */
  double cx, mx, chi;		/* line parameters... */
  double cov_cxmx[3];		/* errors in these */
  int a_beg, a_end;		/* bin in tlayer 0 */
  int b_beg, b_end;		/* bin in tlayer tlaym1 */
  int   numhits;		/* number of hits */
  int   numvcmiss;		/* missing hits in vc */
  int   nummiss;		/* number of planes without hit */
  //enum  Emethod method;		/* treeline generation method  */
  Hit   *hits[2*TLAYERS];	/* hitarray */
  Hit   thehits[2*TLAYERS];
  int   hasharray[2*TLAYERS];
  bool Used;			/* used (part of parttrack) */
  int   ID;			/* adamo ID */
  int   r3offset,firstwire,lastwire;
};
//____________________________________________
/*! \brief Similar to a TreeLine.  It contains tracking information.

A PartTrack contains tracking information and is the result of
multiple track segments fit together.
*/
class PartTrack {

public:
  double x,y;			/* position at magnet_center */
  double mx,my;			/* slope in x and y       */
  double Cov_Xv[4][4];		/* covariance matrix      */
  double chi;			/* combined chi square    */
  TreeLine *tline[3];		/* tree line in u v and x */
  //Cluster  *cluster;		/* cluster pointed to by track */
  double  ECalor;		/* energy in this cluster */
  double  clProb;		/* prob. that this cluster belongs to track */
  double  pathlenoff;		/* pathlength offset */
  double  pathlenslo;		/* pathlength slope  */
  PartTrack *next;	/* linked list */
  Bridge *bridge;       /* magnetic field bridging */
  //enum Emethod method;		/* reconstruction method used */
  //struct _trdcollect *trdcol;	/* trd information */
  int    Used;			/* used (part of a Track)  */
  int    nummiss;		/* missing hits */
  int    numhits;		/* used hits */
  int    isvoid;		/* marked as being void    */
  int    ID;			/* adamo Id */
  
  int    triggerhit;		/* Did this track pass through the trigger?*/
  double trig[2];		/* x-y position at trigger face */
  int cerenkovhit;		/* Did this track pass through the cerenkov bar? */
  double cerenkov[2];		/* x-y position at Cerenkov bar face */
};
//____________________________________________
/*! \brief This class has not yet been implemented
*/
class Track
{
public:
  double ZVx,TVx;		/* Vertex posistion in Z and transverse */
  double The, Phi;		/* theta and phi of track */
  int    AngleCorr;             /* true if theta and phi are corrected for
				   the holding field */
  double rDXSl;			/* bending in the magnet (x direction) */
  double chi;			/* combined chi square */

  double P, ECalor;		/* spect. and calorimeter Energy */

  double XBj, Y, Q2, W2, Nu;	/* kinematix */

  int    RecoEvent;		/* track from mcHits instead of digits */

  Bridge *bridge;		/* magnet matching information */
  int    iMagnetMatched;	/* number of magnet hits along track */
  int    yTracks;		/* number of y tracks */
  
  Track *next;		/* next track */
  Track *ynext;		/* link for y tracks */
  Track *usednext;	/* link of used tracks */

  PartTrack *front;		/* front partial track */
  PartTrack *back;		/* back partial track */

  
  Vertex    *vertex;		/* vertex of this track */

  //HodoscopeInfo H0;            /* H0        data associated with this track*/
  //HodoscopeInfo Hodoscope;     /* Hodoscope data associated with this track*/
  //HodoscopeInfo Preshower;     /* Preshower data associated with this track*/
  //CerenkovInfo  Cerenkov;      /* Cerenkov data associated with this track*/

  bool   Used;		/* used (part of usedTrack list) */
  bool   inBounds;		/* lookup table was usable */
  //enum Emethod method;		/* treesearch method in front track */
  
  //particleID PId;               /* PID info */
  int        iCharge;		/* charge of particle */
  
  int       ID;			/* adamo ID */

private:
};
//____________________________________________
/*! \brief This class has not yet been implemented*/
class Bridge
{
public:
  double xOff, yOff, ySOff;	/* Offset expected from lookup */
  double ySlopeMatch;		/* BENDING: slope match in Y   */
  double yMatch;		/* match in y */
  double ySMatch;		/* matching in Yslope */
  double xMatch;		/* match in x */
  double Momentum;		/* momentum  */
  Hit    *hits[9];		/* matching points in magnet */
  //pointList *point;
  int ID;
private:
};
//____________________________________________
/*! \brief This class has not yet been implemented*/
class Vertex {
  double coord[3];		/* coordinates of vertex */
  double av[3][3];              /* error matrix */  
  double maxresidue;		/* max residue of tracks */
  int nTracks;			/* number of tracks in vertex */
  //enum Emethod method;		/* Front Track Treesearch technique */
  struct Vertex *next;		/* chained list */
  int ID;			/* adamo ID */
};
//____________________________________________
/*! \brief Contains all the track reconstruction for a single event.
*/
class Event {
public:
  int    ID;
  TreeLine  *treeline[2][3][4][4];/* [up/low][region][type][u/v/x/y] */
// Cluster   *cluster[2];	/* [upper/lower] */
//  Cluster   *lumicluster[2];	/* [right/left] */
  PartTrack *parttrack[2][3][4];/* [meth][upper/lower][forw/back] */
  Track     *track[2];	        /* [upper/lower] */
  Track     *usedtrack[2];      /* applying chi^2 cuts to the above list */
  Vertex    *vertex;		/* all vertices */
  int       goodguess;		/* guessed right in electron PID */
  struct    _event *next;	/* next event */
private:
};
//____________________________________________
/*! \brief Used to carry option information
throughout the code.
*/
class Options {

public:
int levels[2][3][4];//pattern resolution
int showEventPattern;
int showMatchingPatterns;
int showMatchedPattern;
private:


};

#endif
