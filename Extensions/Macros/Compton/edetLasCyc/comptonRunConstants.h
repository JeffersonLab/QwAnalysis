//Asymmetry constants 
const Double_t PLANCK = 4.135667e-21;//Planck's constant (MeV*s)
const Double_t A0 = 2.817940289;//classical electron radius (m)
const Double_t E0 = 1161;//electron beam energy (MeV)
const Double_t C = 2.997e8;//speed of light (m/s)
const Double_t LAMDA = 532e-9;//wavelength of laser (m)
const Double_t K0 = PLANCK * C/LAMDA;//incident photon energy in lab frame 
const Double_t PI_ = 3.14159265;//value of pi
const Double_t THETA0 = 1.33 * PI_ / 180.0;//incident photon angle
const Double_t ME = 0.5109989;//electron mass (MeV)
const Double_t KMAX = K0 * (E0 + sqrt(E0*E0 - ME*ME)*cos(THETA0)) / //max backscattered
                     (E0+K0-sqrt(E0*E0 - ME*ME)+K0*cos(THETA0));// photon energy (MeV)
const Double_t Z_CROSS = 9;//strip of Compton asymmetry zero crossing
const Double_t C_EDGE = 51;//strip of Compton edge

//Hardware constants
const Int_t nStrips = 96;
const Int_t nPlanes = 4;

//Run constants
const Double_t beamFrac = 0.6;//90% of maximum beam to be considered as beamOn
const Double_t laserFrac = 0.9;//90% of maximum beam to be considered as beamOn
const Int_t WAIT_N_ENTRIES = 10000;//# of mps's to wait after beam trip
const Double_t ignoreBeamAbove = 195.0;
const Double_t ignoreLasPowAbove = 195.0;

const Int_t usedStrips = 50;
const Int_t mystr = 48;
const Int_t startStrip = 1;
  const Int_t endStrip = 55;
const Int_t startPlane = 2;
  const Int_t endPlane = 4;
