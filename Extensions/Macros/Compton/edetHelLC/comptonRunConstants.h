#ifndef COMPTONRUNCONSTANTS_H
#define COMPTONRUNCONSTANTS_H

//#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
#include "rootbasic.h"
#include "rootClass.h"
#include <stdlib.h>

//Directory paths
static const char *pPath = getenv("QWSCRATCH");
static const char *webDirectory= "www";
static const TString filePrefix = "run_%d/edetLC_%d_";///one spot change of all file names
static TString filePre;
//Asymmetry calculation constants 
static const Double_t light=0.2998;///in conjungtion with the 10^9 of GeV, the 10^8 of light in SI units gives this;
static const Double_t pi=3.141592;
static const Double_t hbarc=0.19732858E-15;/// GeV.m
static const Double_t alpha=0.00729927; ///=1.0/137.0;
static const Double_t me=0.00051099006;///GeV
static const Double_t xmuB=5.788381749E-14; ///  Bohr magnetron GeV/T
static const Double_t B_dipole = 0.544;///T
static Double_t eEnergy = 1.159, rms_eEnergy =0.0;///to be read in from infoDAQ

static const Double_t lambda = 532E-9; //photon wavelength (nm)      
static Double_t xCedge=0.0;
static Double_t param[6];//= {0.0};//{-1.1403e-05, 58.9828, -139.527, 291.23};///param values used till Oct 17
static Double_t a_const = 0.96033; // eqn.15 of Hall A CDR//!for our setup this will not change
static const Int_t nPoints = 10000;///for now arbitrarily chosen the number of points I want to generate the theoretical asymmetry curve
static Bool_t noiseRun = 0;//kFALSE;
static Double_t qNormBkgdSubSigToBkgdRatioLow = 1.25;//0.80;//!this is arbitrarily chosen by observing how it may vary
/*Ideally this ratio could be safely left >0.0, but due to some form of deadtime, 
 *..I may have to be forgiving 
 *Once I find a mechanism to fit the Cedge, I won't need to be nosy about this choice */

//Hardware related constants
static const Int_t nStrips = 64;//96;
static const Int_t nPlanes = 4;
static const Int_t nModules = 2;//number of slave boards
static const Double_t stripWidth = 2.0E-4;//(m) SI unit
static const Double_t whereInTheStrip = 0;//1.0E-4;//0;//1.0E-4;
static const Double_t lmag = 1.25; ///length of each magnet(m) //1.704;
static const Int_t minEntries = 4000; //Laser must be off for at least this many consecutive entries to be considered (off)
///4000/helRate ~<20 second; we never ran with a laser cycle such that lasOff <20s, so this is good
static const Double_t th_det = 11.78;//based on elog495;//detector inclination towards the beam (deg)
///survey provided longitudinal distance of det-bottom from magnetic exit of 3rd dipole
static const Double_t ldet[nPlanes] = {1.69159,1.70182,1.71205,1.72886};

///Run constants
static const Bool_t v2processed=0;
static const Double_t minLasPow = 2000;///put by eyeballing, needs to be checked !!
static const Double_t maxLasPow = 250000;//typical values of sca_laser_PowT ~ 160k when On
static const Double_t acceptLasPow = 112000;//typical values of sca_laser_PowT ~ 160k when On
static const Double_t laserFrac = 0.5;//this was the limit for full current regluar running during run2.///typical 160E3. 
static const Double_t laserFracLo = 0.20;///typical laser off 2E3 ///this is protected explicitly in expAsym.C
static const Double_t laserFracHi = 0.90;//90% of maximum beam to be considered as laserOn///typical 150E3.
static const Double_t beamFracHi = 0.78;
static const Double_t beamFracLo = 0.6;
static const Double_t beamFrac = 0.6;
static const Int_t avoidDAQEr = 10;
static const Int_t WAIT_N_ENTRIES = 50;//# of quartets to wait after beam trip
static const Int_t PREV_N_ENTRIES = 50;//# of quartets to ignore before a beam trip
static const Double_t ignoreBeamAbove = 195.0;

static const Double_t MpsRate = 960.015;
static const Double_t helRate = 240.00375;///960.015/4.0;
static const Double_t lowCurrentLimit = 65.0;

static const Int_t startStrip = 0;
static const Int_t endStrip = 64;
static const Int_t startPlane = 0;
static const Int_t endPlane = 1;
static const Float_t rmsLimit = 0.01;///if a measured value has RMS higher than this=> it has changed

///Boolean constants pertaining to analysis:
static const Bool_t kRejectBMod = 1; //1: yes please reject; 0:Don't reject quartets during bMod ramp
static const Bool_t kNoiseSub = 1;
static const Bool_t kDeadTime = 1, k2parDT = 1;//0: 1-param DT corr; 1: 2-param DT corr
static const Bool_t kRadCor=1;
static const Int_t maxIterations =10;

static Int_t plane=1;///the plane that will be analyzed and will be set in the top most hierarchy of the macros
static Bool_t polSign=0;
static Int_t daqflag=0;
static Int_t asymflag=0;
static Double_t Cedge;
static Double_t tempCedge=50;//!should I initiate it like this !
static Bool_t paramRead;
static Double_t eLaser;
static Double_t gamma_my;
static Double_t re,R_bend,kprimemax,asymmax,rho0,k0prime,p_beam,h,kDummy;//r
static Double_t betaBar=0.0,betaCM=0.0,gammaCM=0.0;
static Double_t delta = 0.0,eGamma;
static Double_t sEq211=0.0;
static Double_t eLaserCM=0.0,eBeamCM=0.0, eBetaCM;
static Double_t costhcm;
static Double_t radCor = 1.0;

static Int_t skipCyc=0;///counts the no.of times the background corrected counts go <= 0
static Bool_t maskSet=0; //set it on when I call the infoDAQ.C to set the mask
static Int_t acTrigSlave[nModules],evTrigSlave[nModules],minWidthSlave[nModules],firmwareRevSlave[nModules],pwtlSlave[nModules],pwtl2Slave[nModules],holdOffSlave[nModules],pipelineDelaySlave[nModules];
static Int_t acTrig,evTrig,minWidth,firmwareRev,pwtl1,pwtl2,holdOff,pipelineDelay;

static std::vector <Int_t> skipStrip; //a list of strips across different planes to be masked (!trying a new idea)
static std::vector<Int_t>::iterator itStrip;
/* set<Int_t> skipStrip; */
/* set<Int_t>::iterator itStrip; */

//used in asymFit's fit function evaluation:
static Double_t xStrip,rhoStrip,rhoPlus,rhoMinus,dsdrho1,dsdrho;///on purpose left uninitialized, if they are being used without being assigned, it will generate a warning of 'using a variable without initializing'

///Declaration of regular variables to be used in the macro
static Float_t corrB1H1L1_0=0.0,corrB1H1L0_0=0.0,corrB1H0L1_0=0.0,corrB1H0L0_0=0.0;//deadtime correction for given run
static Float_t corrB1H1L1_1=0.0,corrB1H1L0_1=0.0,corrB1H0L1_1=0.0,corrB1H0L0_1=0.0;//deadtime correction for given run
static Float_t c2B1H1L1[nStrips]={0.0},c2B1H1L0[nStrips]={0.0},c2B1H0L1[nStrips]={0.0},c2B1H0L0[nStrips]={0.0};
static Double_t stripAsym[nStrips]={0.0},stripAsymEr[nStrips]={0.0};
static Double_t bkgdAsym[nStrips]={0.0},bkgdAsymEr[nStrips]={0.0};
static Double_t beamMax=0.0, laserMax=0.0;
static Double_t pol=0.0,polEr=0.0,chiSq=0.0;
static Double_t cEdge=0.0,cEdgeEr=0.0;
static const Float_t effStripWidth = 1.0033; ///set to the value used by Vladas !! 
static Int_t NDF=0,resFitNDF=0, bgdAsymFitNDF=0;
static Double_t resFit=0.0,resFitEr=0.0, chiSqResidue=0.0;
static Double_t bgdAsymFit =0.0, bgdAsymFitEr = 0.0, chiSqBgdAsym=0.0;

static Double_t asymErSqrLC[nStrips]={0.0},qNormAsymLC[nStrips]={0.0};
static Double_t wmNrAsym[nStrips]={0.0},wmDrAsym[nStrips]={0.0};
static Double_t wmNrBkgdAsym[nStrips]={0.0},wmDrBkgdAsym[nStrips]={0.0};
static Double_t wmNrBCqNormSum[nStrips]={0.0},wmDrBCqNormSum[nStrips]={0.0};
static Double_t wmNrBCqNormDiff[nStrips]={0.0};
static Double_t wmNrqNormB1L0[nStrips]={0.0},wmDrqNormB1L0[nStrips]={0.0};
static Double_t stripAsymDr[nStrips]={0.0},stripAsymDrEr[nStrips]={0.0};
static Double_t stripAsymNr[nStrips]={0.0},stripAsymNrEr[nStrips]={0.0};
static Double_t qNormB1L0[nStrips]={0.0},qNormB1L0Er[nStrips]={0.0};
static Double_t qNormCountsB1L1[nStrips]={0.0},qNormCountsB1L1Er[nStrips]={0.0};
static Double_t qNormCountsB1L0[nStrips]={0.0},qNormCountsB1L0Er[nStrips]={0.0};
static Double_t qNormBkgdSubAllB1L1[nStrips]={0.0},qNormAllB1L0[nStrips]={0.0},qNormAllB1L0Er[nStrips]={0.0}; 
static Double_t totIAllH1L1=0.0,totIAllH1L0=0.0,totIAllH0L1=0.0,totIAllH0L0=0.0;///total current
static Int_t totHelB1L1=0,totHelB1L0=0;///total no.of helicities (hence time)
static Int_t totyieldB1L1[nStrips]={0.0}, totyieldB1L0[nStrips]={0.0};
static Int_t totyieldB1H1L1[nStrips]={0.0},totyieldB1H1L0[nStrips]={0.0},totyieldB1H0L1[nStrips]={0.0}, totyieldB1H0L0[nStrips]={0.0};
static Double_t tNormYieldB1L1[nStrips]={0.0},tNormYieldB1L0[nStrips]={0.0};
static Double_t tNormYieldB1L1Er[nStrips]={0.0},tNormYieldB1L0Er[nStrips]={0.0};
static Double_t beamMaxEver = 200.0, beamOnLimit=20.0;
static Double_t timeB0,rateB0[nStrips]={0.0};
///skip p1:s02,s06,s20 //as of Feb2,2012
///skip p2:s12
///skip p3:s39,s53,s64
///skip p4:none
#endif
