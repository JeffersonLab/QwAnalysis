#ifndef __COMPTONRUNCONSTANTS_H
#define __COMPTONRUNCONSTANTS_H

#include <rootClass.h>

//Directory paths
const char *pPath = getenv("QWSCRATCH");
const char *webDirectory= "www";
TString filePrefix;
//Asymmetry calculation constants 
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
//const Double_t zdrift = 2.275;///drift distance(m) from middle of 2nd dipole to front of 3rd dipole
//const Double_t chicaneBend = 10.131; ///(degree)
const Double_t lmag = 1.25; ///length of each magnet(m) //1.704;
const Int_t minEntries = 5000; //Laser must be (off) for at least this many consecutive entries to be considered (off)
const Double_t th_det = 10.08;//based on elog399; 10.3;//angle at which the diamond detector is inclined towards the beam (deg)
const Double_t ldet[nPlanes] = {1.69792,1.70823,1.71855,1.72886};//1.645,; ///survey provided longitudinal distance of det-bottom from edge of 3rd dipole

///Run constants
const Bool_t v2processed=0;
const Double_t minLasPow = 2000;///put by eyeballing, needs to be checked !!
const Double_t acceptLasPow = 120000;//typical values of sca_laser_PowT ~ 160k when On
const Double_t maxLasPow = 250000;//typical values of sca_laser_PowT ~ 160k when On
/* const Double_t beamFracHi = 0.78;//this was for most of the auto-analysis of run2 */
/* const Double_t beamFracLo = 0.2;//90% of maximum beam to be considered as beamOn */
const Double_t beamFrac = 0.6;//!temp

const Double_t laserFrac = 0.9;//this was the limit for full current reguarl running during run2.///typical 160E3. 
/* const Double_t laserFracLo = 0.6;///typical laser off 2E3 ///this is protected explicitly in expAsym.C */
/* const Double_t laserFracHi = 0.8;//90% of maximum beam to be considered as laserOn///typical 150E3.  */
const Int_t WAIT_N_ENTRIES = 10000;//# of mps's to wait after beam trip
const Int_t PREV_N_ENTRIES = 5000;//# of mps's to wait after beam trip
const Double_t ignoreBeamAbove = 195.0;
//const Double_t ignoreLasPowAbove = 195.0;
const Double_t MpsRate = 960.015;
const Double_t lowCurrentLimit = 65.0;

//following numbers are all in C++ counting(start at 0)
const Int_t startStrip = 0;
const Int_t endStrip = 64;
const Int_t startPlane = 0;
  const Int_t endPlane = 1;

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
Double_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips];//,stripAsym_v2[nPlanes][nStrips],stripAsymEr_v2[nPlanes][nStrips];
Double_t bkgdAsym[nPlanes][nStrips],bkgdAsymEr[nPlanes][nStrips];//,bkgdAsym_v2[nPlanes][nStrips],bkgdAsymEr_v2[nPlanes][nStrips];
Double_t stripAsymDr[nPlanes][nStrips],stripAsymDrEr[nPlanes][nStrips];
Double_t stripAsymNr[nPlanes][nStrips],stripAsymNrEr[nPlanes][nStrips];
Double_t qNormB1L0[nPlanes][nStrips],qNormB1L0Er[nPlanes][nStrips];
/* Double_t qNormScalerB1L1[nPlanes][nStrips],qNormScalerB1L0[nPlanes][nStrips]; */
/* Double_t qNormAccumB1L1[nPlanes][nStrips],qNormAccumB1L0[nPlanes][nStrips]; */
Double_t qNormCountsB1L0[nPlanes][nStrips],qNormCountsB1L1[nPlanes][nStrips];

///skip p1:s02,s06,s20 //as of Feb2,2012
///skip p2:s12
///skip p3:s39,s53,s64
///skip p4:none

///flags file: p1b=0xFFFFFFFF,p1m=0xFFFFFFFF,p1t=0xFFF7FFdd,p2b=0xFFFFFFFF,p2m=0xFFFFFFFF,p2t=0xFFFFF7FF,p3b=0xFFFFFFFF,p3m=0x7FEFFFbF,p3t=0xFFFFFFFF,p4b=0xFFFFFFFF,p4m=0xFFFFFFFF,p4t=0xFFFFFFFF

#endif
