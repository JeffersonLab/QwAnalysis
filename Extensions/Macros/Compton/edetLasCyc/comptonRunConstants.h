#ifndef __COMPTONRUNCONSTANTS_H
#define __COMPTONRUNCONSTANTS_H

#include <rootClass.h>

//Directory paths
const char *pPath = getenv("QWSCRATCH");
const char *webDirectory= "www";

//Asymmetry calculation constants 
const Float_t pi=3.141592;
const Float_t hbarc=0.197E-15;/// GeV.m
const Float_t alpha=0.00729927; ///=1.0/137.0;
const Float_t me=0.000511;///GeV
const Float_t xmuB=5.788381749E-14; ///  Bohr magnetron GeV/T
const Float_t B_dipole = 0.544;///T
const Float_t E = 1.159; //Beam energy (GeV)
const Float_t lambda = 532E-9; //photon wavelength (nm)      
const Float_t xgap = 0.007; //!min distance (m)

const Double_t stripMin = 30.0;
const Double_t stripMax = 50.0;

//Hardware related constants
const Int_t nStrips = 96;
const Int_t nPlanes = 4;
const Float_t zdrift = 2.275;///drift distance(m) from middle of 2nd dipole to front of 3rd dipole
const Double_t chicaneBend = 10.131; ///(degree)
const Double_t ldet = 1.645; ///(m)
const Double_t lmag = 1.25; ///(m) //1.704;
const Int_t minEntries = 8000; //Laser must be off for at least this many consecutive entries to be considered off.


///Run constants
const Bool_t v2processed=0;//kFALSE;
const Double_t minLasPow = 2000;///put by eyeballing, needs to be checked !!
const Double_t beamFrac = 0.6;//90% of maximum beam to be considered as beamOn
const Double_t laserFrac = 0.9;//90% of maximum beam to be considered as beamOn///typical 160E3. 
const Int_t WAIT_N_ENTRIES = 10000;//# of mps's to wait after beam trip
const Int_t PREV_N_ENTRIES = 5000;//# of mps's to wait after beam trip
const Double_t ignoreBeamAbove = 195.0;
//const Double_t ignoreLasPowAbove = 195.0;
const Int_t IHWP=-1;///-1:in; 1:out

//following numbers are all in C++ counting(start at 0)
const Int_t mystr = 40; //a random strip for testing my script
const Int_t startStrip = 0;
 const Int_t endStrip = 64;
const Int_t startPlane = 0;
  const Int_t endPlane = 3;
const Int_t Cedge = 54;///Compton-edge for the current run(counting from 0)

///skip p1:s02,s06,s20 //as of Feb2,2012
///skip p2:s12
///skip p3:s39,s53,s64
///skip p4:none

///flags file: p1b=0xFFFFFFFF,p1m=0xFFFFFFFF,p1t=0xFFF7FFdd,p2b=0xFFFFFFFF,p2m=0xFFFFFFFF,p2t=0xFFFFF7FF,p3b=0xFFFFFFFF,p3m=0x7FEFFFbF,p3t=0xFFFFFFFF,p4b=0xFFFFFFFF,p4m=0xFFFFFFFF,p4t=0xFFFFFFFF

#endif
