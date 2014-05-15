#ifndef __COMPTONRUNCONSTANTS_H
#define __COMPTONRUNCONSTANTS_H

#include <rootClass.h>
#include <stdlib.h>

//Directory paths
const char *pPath = getenv("QWSCRATCH");
const char *webDirectory= "www";
TString filePrefix;
//Asymmetry calculation constants 
const Double_t light=0.2998;///in conjungtion with the 10^9 of GeV, the 10^8 of light in SI units gives this;
const Double_t pi=3.141592;
const Double_t hbarc=0.197E-15;/// GeV.m
const Double_t alpha=0.00729927; ///=1.0/137.0;
const Double_t me=0.000511;///GeV
const Double_t xmuB=5.788381749E-14; ///  Bohr magnetron GeV/T
const Double_t B_dipole = 0.544;///T
const Double_t E = 1.159; //Beam energy (GeV)
const Double_t lambda = 532E-9; //photon wavelength (nm)      
Double_t xCedge;
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
const Int_t minEntries = 4000; //Laser must be off for at least this many consecutive entries to be considered (off)
///4000/helRate ~<20 second; we never ran with a laser cycle such that lasOff <20s, so this is good
const Double_t th_det = 11.78;//based on elog495;//detector inclination towards the beam (deg)
///survey provided longitudinal distance of det-bottom from magnetic exit of 3rd dipole
const Double_t ldet[nPlanes] = {1.69159,1.70182,1.71205,1.72886};

///Run constants
const Bool_t v2processed=0;
const Double_t minLasPow = 2000;///put by eyeballing, needs to be checked !!
const Double_t maxLasPow = 250000;//typical values of sca_laser_PowT ~ 160k when On
const Double_t acceptLasPow = 112000;//typical values of sca_laser_PowT ~ 160k when On
const Double_t laserFrac = 0.5;//this was the limit for full current regluar running during run2.///typical 160E3. 
const Double_t laserFracLo = 0.20;///typical laser off 2E3 ///this is protected explicitly in expAsym.C
const Double_t laserFracHi = 0.90;//90% of maximum beam to be considered as laserOn///typical 150E3.
const Double_t beamFracHi = 0.78;
const Double_t beamFracLo = 0.6;
const Double_t beamFrac = 0.6;
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

Int_t asymflag=0;
Double_t Cedge[nPlanes];
Double_t tempCedge=50;//!should I initiate it like this !
Bool_t paramRead;
Double_t k;
Double_t gamma_my;

Bool_t maskSet=0; //set it on when I call the infoDAQ.C to set the mask
Bool_t mask[nPlanes][nStrips];  
Int_t acTrigSlave[nModules],evTrigSlave[nModules],minWidthSlave[nModules],firmwareRevSlave[nModules],pwtlSlave[nModules],pwtl2Slave[nModules],holdOffSlave[nModules],pipelineDelaySlave[nModules];

std::vector <Int_t> skipStrip; //a list of strips across different planes to be masked (!trying a new idea)
std::vector<Int_t>::iterator itStrip;
/* set<Int_t> skipStrip; */
/* set<Int_t>::iterator itStrip; */

//used in asymFit's fit function evaluation:
Double_t xStrip,rhoStrip,rhoPlus,rhoMinus,dsdrho1,dsdrho;

///Declaration of regular variables to be used in the macro
Double_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips];
Double_t bkgdAsym[nPlanes][nStrips],bkgdAsymEr[nPlanes][nStrips];
Double_t beamMax, laserMax;

Double_t stripAsymDr[nPlanes][nStrips],stripAsymDrEr[nPlanes][nStrips];
Double_t stripAsymNr[nPlanes][nStrips],stripAsymNrEr[nPlanes][nStrips];
Double_t qNormB1L0[nPlanes][nStrips],qNormB1L0Er[nPlanes][nStrips];
Double_t qNormCountsB1L1[nPlanes][nStrips],qNormCountsB1L1Er[nPlanes][nStrips];
Double_t qNormCountsB1L0[nPlanes][nStrips],qNormCountsB1L0Er[nPlanes][nStrips];
Double_t qNormBkgdSubAllB1L1[nPlanes][nStrips],qNormAllB1L0[nPlanes][nStrips],qNormAllB1L0Er[nPlanes][nStrips]; 
Double_t totIAllH1L1=0.0,totIAllH1L0=0.0,totIAllH0L1=0.0,totIAllH0L0=0.0;///total current
Int_t totHelB1L1=0,totHelB1L0=0;///total no.of helicities (hence time)
Int_t totyieldB1L1[nPlanes][nStrips], totyieldB1L0[nPlanes][nStrips];
Int_t totyieldB1H1L1[nPlanes][nStrips],totyieldB1H1L0[nPlanes][nStrips],totyieldB1H0L1[nPlanes][nStrips], totyieldB1H0L0[nPlanes][nStrips];
Double_t tNormYieldB1L1[nPlanes][nStrips],tNormYieldB1L0[nPlanes][nStrips];
Double_t tNormYieldB1L1Er[nPlanes][nStrips],tNormYieldB1L0Er[nPlanes][nStrips];
Double_t beamMaxEver = 200.0, beamOnLimit=20.0;
///skip p1:s02,s06,s20 //as of Feb2,2012
///skip p2:s12
///skip p3:s39,s53,s64
///skip p4:none
#endif
