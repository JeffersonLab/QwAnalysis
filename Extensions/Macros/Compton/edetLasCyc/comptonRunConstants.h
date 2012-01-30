//Asymmetry constants 
///attempt-1

///attempt-2
const Float_t pi=3.141592;

const Float_t hbarc=0.197E-15;// GeV.m
//const Float_t hbarc=0.197;//old

const Float_t alpha=0.00729927; ///=1.0/137.0;
const Float_t me=0.000511;
const Float_t xmuB=5.788381749E-14; ///  Bohr magnetron GeV/T
const Float_t B_dipole = 0.675;
const Float_t E = 1.159; //Beam energy (GeV)
const Float_t lambda = 532E-9; //photon wavelength (nm)      
//const Float_t lambda = 532.0; //old photon wavelength (nm)      
const Float_t xgap = 0.005; //!min distance (m)
//Hardware constants
const Int_t nStrips = 96;
const Int_t nPlanes = 4;
const Float_t zdrift = 2.275; //drift distance(m) from middle of 2nd dipole to front of 3rd dipole
// const Float_t thetabend = 10*pi/180; //bend angle in Compton chicane (radians)
// const Float_t thetaprime = 10.39*pi/180; //bend angle for the max deflected electrons


//Run constants
const Double_t beamFrac = 0.6;//90% of maximum beam to be considered as beamOn
const Double_t laserFrac = 0.9;//90% of maximum beam to be considered as beamOn
const Int_t WAIT_N_ENTRIES = 10000;//# of mps's to wait after beam trip
const Double_t ignoreBeamAbove = 195.0;
const Double_t ignoreLasPowAbove = 195.0;

/* const Int_t usedStrips = 50; */
/* const Int_t mystr = 48; */
const Int_t startStrip = 0;
  const Int_t endStrip = 55;
const Int_t startPlane = 0;
  const Int_t endPlane = 4;
