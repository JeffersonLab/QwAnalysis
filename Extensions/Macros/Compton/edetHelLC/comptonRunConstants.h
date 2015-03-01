#ifndef __COMPTONRUNCONSTANTS_H
#define __COMPTONRUNCONSTANTS_H

#include "/w/hallc/compton/users/narayan/svn/Compton/edetHelLC/rootClass.h"
#include <stdlib.h>

///Boolean constants pertaining to analysis:
    Bool_t kBgdSub = 0; 
const Bool_t kRejectBMod = 1; //1: yes please reject; 0:Don't reject quartets during bMod ramp
const Bool_t kNoiseSub = 0;
const Bool_t kDeadTime = 1, k2parDT = 1;//0: 1-param DT corr; 1: 2-param DT corr
const Bool_t kRadCor=1;
const Bool_t kBeamStable=1;
const Bool_t kOnlyGoodLasCyc = 1;///1: use only laser cycles with no beam trip; 0: use all cycles throwing only quartets with trip
const Bool_t kBPMYield = 1;
const Bool_t kBPMDiff = 1;
const Int_t maxIterations =4;
//Directory paths
const char *pPath = getenv("QWSCRATCH");
const char *www = "www";///to store all image files
const char *txt = "txt";///to store all the text files
//const TString filePrefix = "r%d/edetLCNoDT_%d_";///one spot change of all file names
const TString filePrefix = "r%d/edetLC_%d_";///one spot change of all file names
TString filePre;
//Asymmetry calculation constants 
const Double_t light=0.299792458;///in conjungtion with the 10^9 of GeV, the 10^8 of light in SI units gives this;
const Double_t pi=3.141592;
const Double_t hbarc=0.19732858E-15;/// GeV.m
const Double_t alpha=0.00729927; ///=1.0/137.0;
const Double_t me=0.00051099006;///GeV
const Double_t xmuB=5.788381749E-14; ///  Bohr magnetron GeV/T
const Double_t B_dipoleDefault = 0.544;//0.55186;//0.544;///T
Double_t B_dipole = 0.544;//0.55186;//0.544;///T
const Double_t eEnergyRun2 = 1157.53;///current run2 histogrammed mean
Double_t eEnergy = 1157.53, rms_eEnergy =0.0, eEnergyEr=0.0;///to be read in from infoDAQ

//const Double_t lambda = 248E-9;//uv
const Double_t lambda = 532E-9; //photon wavelength (nm)      
//const Double_t lambda = 1064E-9;//infra red
Double_t xCedge=0.0;
Double_t param[4];//= {0.0};//{-1.1403e-05, 58.9828, -139.527, 291.23};///param values used till Oct 17
Double_t a_const = 0.96033; // eqn.15 of Hall A CDR//!for our setup this will not change
const Int_t nPoints = 10000;///for now arbitrarily chosen the number of points I want to generate the theoretical asymmetry curve
Bool_t noiseRun = 0;//kFALSE;
Double_t qNormBkgdSubSigToBkgdRatioLow = 1.25;//0.80;//!this is arbitrarily chosen by observing how it may vary
/*Ideally this ratio could be safely left >0.0, but due to some form of deadtime, 
 *..I may have to be forgiving 
 *Once I find a mechanism to fit the Cedge, I won't need to be nosy about this choice */

//Hardware related constants
const Int_t nStrips = 64;//96;
const Int_t nPlanes = 4;
const Int_t nModules = 2;//number of slave boards
const Double_t stripWidth = 2.0E-4;//(m) SI unit
const Double_t whereInTheStrip = 0;//1.0E-4;//0;//1.0E-4;
const Double_t lmag = 1.25; ///length of each magnet(m) //1.704;
const Int_t minEntries = 3120; //Laser must be off for at least this many consecutive quartets to be considered (off)
///4000/helRate ~<20 second; we never ran with a laser cycle such that lasOff <20s, so this is good
const Double_t th_det = 11.78;//based on elog495;//detector inclination towards the beam (deg)
///survey provided longitudinal distance of det-bottom from magnetic exit of 3rd dipole
const Double_t ldet[nPlanes] = {1.69159,1.70182,1.71205,1.72886};

///Run constants
const Bool_t v2processed=0;
const Double_t lasOffCut = 1182;//1261;//1800;//2000;//1261;//1100;///elog 319 
const Float_t maxLasPow = 250000;//typical values of sca_laser_PowT ~ 160k when On
      Double_t acceptLasPow = 112000;//typical values of sca_laser_PowT ~ 160k when On
const Float_t laserFrac = 0.5;//this was the limit for full current regluar running during run2.///typical 160E3. 
const Float_t laserFracLo = 0.20;///typical laser off 2E3 ///this is protected explicitly in expAsym.C
const Float_t laserFracHi = 0.80;//90% of maximum beam to be considered as laserOn///typical 150E3.
const Float_t beamFrac = 0.5;
const Float_t highBeamCut = 90;//uA
const Int_t avoidDAQEr = 10;
const Int_t WAIT_N_ENTRIES = 50;//# of quartets to wait after beam trip
const Int_t PREV_N_ENTRIES = 50;//# of quartets to ignore before a beam trip
const Double_t ignoreBeamAbove = 195.0;

const Double_t MpsRate = 960.015;
const Double_t helRate = 240.00375;///960.015/4.0;
const Double_t lowCurrentLimit = 65.0;

const Int_t startStrip = 0;
const Int_t endStrip = 64;
const Int_t startPlane = 0;
const Int_t endPlane = 1;
const Float_t rmsLimit = 0.01;///if a measured value has RMS higher than this=> it has changed
const Float_t beamMaxEver = 200.0, beamOnLimit=2.0;
Float_t beamTripLimit;
const Float_t effStripWidth = 1.00; 
const Float_t bpmDiff = 0.04;

Int_t plane=1;///the plane that will be analyzed and will be set in the top most hierarchy of the macros
Bool_t polSign=0;
Int_t daqflag=0, retInfoDAQ=0, retNoise=0, retCorrDT=0, retEvalBgdAsym=0;
Int_t asymflag=0;
Bool_t paramRead;
Double_t eLaser;
Double_t gamma_my;
Double_t re,R_bend,kprimemax,asymmax,rho0,k0prime,p_beam,h,kDummy;//r
Double_t betaBar=0.0,betaCM=0.0,gammaCM=0.0;
Double_t delta = 0.0,eGamma;
Double_t sEq211=0.0;
Double_t eLaserCM=0.0,eBeamCM=0.0, eBetaCM;
Double_t costhcm;
Double_t radCor = 1.0;

Int_t skipCyc=0;///counts the no.of times the background corrected counts go <= 0
Int_t acTrigSlave[nModules],evTrigSlave[nModules],minWidthSlave[nModules],fwRev[nModules],pwtlSlave[nModules],pwtl2Slave[nModules],holdOffSlave[nModules],plDelay[nModules];
Int_t acTrig,evTrig,minWidth,firmwareRev,pwtl1,pwtl2,holdOff,pipelineDelay;

std::vector <Int_t> skipStrip; //a list of strips across different planes to be masked (!trying a new idea)
std::vector<Int_t>::iterator itStrip;
/* set<Int_t> skipStrip; */
/* set<Int_t>::iterator itStrip; */

//used in asymFit's fit function evaluation:
Double_t xStrip,rhoStrip,rhoPlus,rhoMinus,dsdrho1,dsdrho;///on purpose left uninitialized, if they are being used without being assigned, it will generate a warning of 'using a variable without initializing'

///Declaration of regular variables to be used in the macro
Float_t c2B1H1L1[nStrips]={1.0},c2B1H1L0[nStrips]={1.0},c2B1H0L1[nStrips]={1.0},c2B1H0L0[nStrips]={1.0};
Double_t stripAsym[nStrips]={0.0},stripAsymEr[nStrips]={0.0};
Double_t bkgdAsym[nStrips]={0.0},bkgdAsymEr[nStrips]={0.0};
Double_t beamMax=0.0, laserMax=0.0;
Double_t beamMean = -1.0, beamRMS = -1.0, beamMeanEr = -1.0;
Double_t pol=0.0,polEr=0.0,chiSq=0.0;
Int_t NDF=0,resFitNDF=0, bgdAsymFitNDF=0;
Double_t resFit=0.0,resFitEr=0.0, chiSqResidue=0.0;
Double_t bgdAsymFit =0.0, bgdAsymFitEr = 0.0, chiSqBgdAsym=0.0;

Double_t asymErSqrLC[nStrips]={0.0},qNormAsymLC[nStrips]={0.0};
Double_t wmNrAsym[nStrips]={0.0},wmDrAsym[nStrips]={0.0};
Double_t wmNrBkgdAsym[nStrips]={0.0},wmDrBkgdAsym[nStrips]={0.0};
Double_t wmNrBCqNormSum[nStrips]={0.0},wmDrBCqNormSum[nStrips]={0.0};
Double_t wmNrBCqNormDiff[nStrips]={0.0};
Double_t wmNrqNormB1L0[nStrips]={0.0},wmDrqNormB1L0[nStrips]={0.0};
Double_t stripAsymDr[nStrips]={0.0},stripAsymDrEr[nStrips]={0.0};
Double_t stripAsymNr[nStrips]={0.0},stripAsymNrEr[nStrips]={0.0};
Double_t qNormB1L0[nStrips]={0.0},qNormB1L0Er[nStrips]={0.0};
Double_t qNormCntsB1H1L1[nStrips]={0.0},qNormCntsB1H1L1Er[nStrips]={0.0};
Double_t qNormCntsB1H1L0[nStrips]={0.0},qNormCntsB1H1L0Er[nStrips]={0.0};
Double_t qNormCntsB1H0L1[nStrips]={0.0},qNormCntsB1H0L1Er[nStrips]={0.0};
Double_t qNormCntsB1H0L0[nStrips]={0.0},qNormCntsB1H0L0Er[nStrips]={0.0};
Double_t qNormBkgdSubAllB1L1[nStrips]={0.0},qNormAllB1L0[nStrips]={0.0},qNormAllB1L0Er[nStrips]={0.0}; 
Double_t qAllH1L1=0.0,qAllH1L0=0.0,qAllH0L1=0.0,qAllH0L0=0.0;///total current
Double_t totHelB1H1L1=0,totHelB1H1L0=0;///total no.of helicities (hence time)
Double_t totHelB1H0L1=0,totHelB1H0L0=0;///total no.of helicities (hence time)
Double_t totyieldB1L1[nStrips]={0.0}, totyieldB1L0[nStrips]={0.0};
Double_t totyieldB1H1L1[nStrips]={0.0},totyieldB1H1L0[nStrips]={0.0},totyieldB1H0L1[nStrips]={0.0}, totyieldB1H0L0[nStrips]={0.0};
Double_t tNormYieldB1H1L1[nStrips]={0.0},tNormYieldB1H1L1Er[nStrips]={0.0};
Double_t tNormYieldB1H1L0[nStrips]={0.0},tNormYieldB1H1L0Er[nStrips]={0.0};
Double_t tNormYieldB1H0L1[nStrips]={0.0},tNormYieldB1H0L1Er[nStrips]={0.0};
Double_t tNormYieldB1H0L0[nStrips]={0.0},tNormYieldB1H0L0Er[nStrips]={0.0};
Double_t timeB0, rateB0[nStrips], rateB0Er[nStrips];
Double_t checkAsym=0.0;///use to check if sign of asymmetry changed during run
const Int_t myStr = 35;///a sample strip number to check for various quantities;
///skip p1:s02,s06,s20 //as of Feb2,2012
///skip p2:s12
///skip p3:s39,s53,s64
///skip p4:none
#endif
