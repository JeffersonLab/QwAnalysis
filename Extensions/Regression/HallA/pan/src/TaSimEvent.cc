//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaSimEvent.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaSimEvent.cc,v 1.9 2010/02/12 16:44:30 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    An event of data, probably munged up by some varity of simulation!
//
////////////////////////////////////////////////////////////////////////

//#define NOISY
//#define DEBUG

#include "TaSimEvent.hh"

#include "TaCutList.hh"
#include "TaLabelledQuantity.hh"
#include "TaRun.hh"
#include "TaString.hh"
#include "TaDevice.hh"
#include "TaDataBase.hh"
#include "VaPair.hh"
#include <iostream>
#include <iomanip>
#include <utility>
#include <cmath>

#ifndef NODICT
ClassImp(TaSimEvent)
#endif

// Static members

#ifdef FAKEHELNOFILE
Int_t* TaSimEvent::fgPattern = NULL;
Int_t  TaSimEvent::fgNMultiplet = 2;
Int_t  TaSimEvent::fgOversample = 1;
Bool_t TaSimEvent::fgRandom = 1;
UInt_t TaSimEvent::fgShreg = 1;
UInt_t TaSimEvent::fgRanType = 1;
#endif

#ifdef FAKEHEL
ifstream TaSimEvent::fgHelfile("helicity.data");
#endif

//  TRandom TaSimEvent::fgR;      // Random number object
//  Double_t TaSimEvent::fDetVsBcm[4];
//  Double_t TaSimEvent::fDetNoiseR[4];
//  // modify position by dithering slope for 12x, 4a x,y and 4b x,y given value
//  //  and ident of dithering object
//  //                            coils:  1x     1y     2x     2y     3x     3y     E
//  Double_t TaSimEvent::fBpm4AXvCoil[7];   Double_t TaSimEvent::fBpm4AYvCoil[7];
//  Double_t TaSimEvent::fBpm4BXvCoil[7];   Double_t TaSimEvent::fBpm4BYvCoil[7];
//  Double_t TaSimEvent::fBpm12XvCoil[7];
//  // modify detector by position
//  Double_t TaSimEvent::fDetVsBpm12X[4];  
//  Double_t TaSimEvent::fDetVsBpm4AX[4];  Double_t TaSimEvent::fDetVsBpm4BX[4];
//  Double_t TaSimEvent::fDetVsBpm4AY[4];  Double_t TaSimEvent::fDetVsBpm4BY[4];
//  Double_t TaSimEvent::fDet12Xoff;  
//  Double_t TaSimEvent::fDet4AXoff;  Double_t TaSimEvent::fDet4BXoff;
//  Double_t TaSimEvent::fDet4AYoff;  Double_t TaSimEvent::fDet4BYoff;



TaSimEvent::TaSimEvent(): VaEvent()
{
#ifdef FAKEDITHER
  this->SetSimConstants();
#endif
}

TaSimEvent::~TaSimEvent() 
{
}

TaSimEvent::TaSimEvent(const TaSimEvent& ev): VaEvent(ev)
{
#ifdef FAKEDITHER
  this->SetSimConstants();
#endif
} 

TaSimEvent &TaSimEvent::operator=(const TaSimEvent &ev)
{
  if ( &ev != this )
    {
      VaEvent::Uncreate();
      VaEvent::Create (ev);
    }
  return *this;
}

TaSimEvent& 
TaSimEvent::CopyInPlace (const TaSimEvent& rhs)
{
  // Like operator=, but copy an event into existing memory, not
  // deleting and reallocating.  This should always be safe, but just
  // to minimize possible problems use this instead of operator= only
  // when pointers must be preserved.
  VaEvent::CopyInPlace(rhs);

  // copy over sim specific data arrays

  return *this;
};


// Major functions

ErrCode_t TaSimEvent::RunInit(const TaRun& run)
{ 
 // Initialization at start of run.  Get quantities from database
  // which will be needed in event loop, and set other static variables
  // to initial values.

  clog << "!!!!!****************************!!!!!" << endl;
  clog << "!!!!!****************************!!!!!" << endl;
  clog << "TaSimEvent::RunInit WARNING: Data Faking is ON! **************" << endl;
  clog << "!!!!!****************************!!!!!" << endl;
  clog << "!!!!!****************************!!!!!" << endl;

  ErrCode_t stat = VaEvent::RunInit(run);

#ifdef FAKEHELNOFILE
  string type = run.GetDataBase().GetPairType();

  if (type == "octet")
    {
      fgNMultiplet = 8;
    }
  else if (type == "quad")
    {
      fgNMultiplet = 4;
    }
  else if (type == "pair")
    {
      fgNMultiplet = 2;
    }
  else if (type[0] == '+')
    {
      // (assuming if the first character is +, the rest are + or-!)
      fgNMultiplet = type.size();
    }
  else
    {
      cerr << "TaSimEvent::RunInit WARNING: "
	   << "Invalid pair type: " << type << endl;
      cerr << "Pair pairing chosen" << endl;
      fgNMultiplet = 1;
    }
  if (fgPattern != NULL)
    delete[] fgPattern;
  fgPattern = new Int_t[fgNMultiplet];
  if (type[0] == '+')
    {
      for (UInt_t i = 0; i < type.size(); ++i)
	fgPattern[i] = type[i] == '+' ? 0 : 1;
    }  
  else
    {
      fgPattern[0] = 0;
      fgPattern[1] = 1;
      if (type == "quad" || type == "octet")
	{
	  fgPattern[2] = 1;
	  fgPattern[3] = 0;
	  if (type == "octet")
	    {
	      fgPattern[4] = 1;
	      fgPattern[5] = 0;
	      fgPattern[6] = 0;
	      fgPattern[7] = 1;
	    }
	}
    }
  fgOversample = run.GetOversample();
  TaString sran = run.GetDataBase().GetRandomHeli();
  fgRandom = sran.CmpNoCase ("no") == 0 ? 0 : 1;
  fgRanType = sran.CmpNoCase ("old") == 0 ? 0 
    : sran.CmpNoCase ("30bit") == 0 ? 2 : 1;

#endif

  return stat;
}

void TaSimEvent::SetSimConstants()
{
#ifdef FAKEDITHER
  // one day, it would be nice to have a non-hardwire interface for these parameters.

  fDetVsBcm[0] = 1.0;  fDetVsBcm[1] = 1.0;
  fDetVsBcm[2] = 1.0;  fDetVsBcm[3] = 1.0;

  fDetNoiseR[0] = 0.005;  fDetNoiseR[1] = 0.005;
  fDetNoiseR[2] = 0.005;  fDetNoiseR[3] = 0.005;

//   fBpm4AXvCoil[0] =  1.5; //1x 
//   fBpm4AXvCoil[1] = -0.5; //1y 
//   fBpm4AXvCoil[2] = -1.5; //2x 
//   fBpm4AXvCoil[3] =  0.0; //2y 
//   fBpm4AXvCoil[4] = -1.5; //3x 
//   fBpm4AXvCoil[5] = -0.5; //3y 
//   fBpm4AXvCoil[6] =  0.0; //E  

//   fBpm4AYvCoil[0] =  0.5; //1x 
//   fBpm4AYvCoil[1] =  1.5; //1y 
//   fBpm4AYvCoil[2] =  0.5; //2x 
//   fBpm4AYvCoil[3] =  1.5; //2y 
//   fBpm4AYvCoil[4] = -0.5; //3x 
//   fBpm4AYvCoil[5] = -2.0; //3y 
//   fBpm4AYvCoil[6] =  0.0; //E  

//   fBpm4BXvCoil[0] = -1.5; //1x
//   fBpm4BXvCoil[1] =  0.5; //1y
//   fBpm4BXvCoil[2] = -1.5; //2x
//   fBpm4BXvCoil[3] =  0.5; //2y
//   fBpm4BXvCoil[4] =  2.0; //3x
//   fBpm4BXvCoil[5] = -0.5; //3y
//   fBpm4BXvCoil[6] =  0.00; //E  

//   fBpm4BYvCoil[0] =  0.00; //1x
//   fBpm4BYvCoil[1] = -1.50; //1y
//   fBpm4BYvCoil[2] =  0.00; //2x
//   fBpm4BYvCoil[3] =  1.00; //2y
//   fBpm4BYvCoil[4] =  0.50; //3x
//   fBpm4BYvCoil[5] =  1.00; //3y
//   fBpm4BYvCoil[6] =  0.00; //E 

//   fBpm12XvCoil[0] =  0.500; //1x
//   fBpm12XvCoil[1] =  0.500; //1y
//   fBpm12XvCoil[2] =  0.500; //2x
//   fBpm12XvCoil[3] =  0.500; //2y
//   fBpm12XvCoil[4] =  0.500; //3x
//   fBpm12XvCoil[5] =  0.500; //3y
//   fBpm12XvCoil[6] =  2.000; //E 

// Raw BPMs in units off mm.
// Coils readout as uA, with 1E-6 in Decode, so this should be mm/A
// realistically, I think we should expect 100 um / 100 mA, 
// which is on order 1 in these units

  fBpm4AXvCoil[0] =  10.;  //1x 
  fBpm4AXvCoil[1] =   0.0; //1y 
  fBpm4AXvCoil[2] =   0.0; //2x 
  fBpm4AXvCoil[3] =   0.0; //2y 
  fBpm4AXvCoil[4] =   0.0; //3x 
  fBpm4AXvCoil[5] =   0.0; //3y 
  fBpm4AXvCoil[6] =   0.0; //E  

  fBpm4AYvCoil[0] =   0.0; //1x 
  fBpm4AYvCoil[1] =  10.0; //1y 
  fBpm4AYvCoil[2] =   0.0; //2x 
  fBpm4AYvCoil[3] =   0.0; //2y 
  fBpm4AYvCoil[4] =   0.0; //3x 
  fBpm4AYvCoil[5] =   0.0; //3y 
  fBpm4AYvCoil[6] =   0.0; //E  

  fBpm4BXvCoil[0] =   0.0; //1x
  fBpm4BXvCoil[1] =   0.0; //1y
  fBpm4BXvCoil[2] =  10.0; //2x
  fBpm4BXvCoil[3] =   0.0; //2y
  fBpm4BXvCoil[4] =   0.0; //3x
  fBpm4BXvCoil[5] =   0.0; //3y
  fBpm4BXvCoil[6] =   0.0; //E  

  fBpm4BYvCoil[0] =   0.0; //1x
  fBpm4BYvCoil[1] =   0.0; //1y
  fBpm4BYvCoil[2] =   0.0; //2x
  fBpm4BYvCoil[3] =  10.0; //2y
  fBpm4BYvCoil[4] =   0.0; //3x
  fBpm4BYvCoil[5] =   0.0; //3y
  fBpm4BYvCoil[6] =   0.0; //E 

  fBpm12XvCoil[0] =   0.0; //1x
  fBpm12XvCoil[1] =   0.0; //1y
  fBpm12XvCoil[2] =   0.0; //2x
  fBpm12XvCoil[3] =   0.0; //2y
  fBpm12XvCoil[4] =   0.0; //3x
  fBpm12XvCoil[5] =  10.0; //3y
  fBpm12XvCoil[6] =   0.0; //E 

  fDet12Xoff = 0.407;  
  fDet4AXoff = 0.0824;
  fDet4BXoff = -0.166;
  fDet4AYoff = 0.2892;
  fDet4BYoff = -1.084;

  // this is fraction, per mm.
  // we expect on order 40 ppm/um in pair tree, but
  // this is done in raw tree (without extra factor of 2)
  // so 80000 ppm / mm  or about 0.08 in the units seen here.

  fDetVsBpm12X[0] =  0.800;  fDetVsBpm12X[1] =  0.80;
  fDetVsBpm12X[2] =  0.200;  fDetVsBpm12X[3] =  0.20;  
  
  fDetVsBpm4AX[0] =  0.800;  fDetVsBpm4AX[1] = -0.000;
  fDetVsBpm4AX[2] =  0.080;  fDetVsBpm4AX[3] = -0.080;
  
  fDetVsBpm4BX[0] =  0.000;  fDetVsBpm4BX[1] = -0.800;
  fDetVsBpm4BX[2] = -0.080;  fDetVsBpm4BX[3] =  0.080;
  
  fDetVsBpm4AY[0] =  0.000;  fDetVsBpm4AY[1] =  0.200;
  fDetVsBpm4AY[2] =  0.080;  fDetVsBpm4AY[3] =  0.080;
  
  fDetVsBpm4BY[0] =  0.400;  fDetVsBpm4BY[1] =  0.400;
  fDetVsBpm4BY[2] = -0.080;  fDetVsBpm4BY[3] =  0.040;
#endif
}
  

void TaSimEvent::Decode(TaDevice& devices) {
// Decodes all the raw data and applies all the calibrations and BPM
// rotations.. THEN it adds in a simulation, modifying the 
// beam monitors to simulate dithering (depending on paramters)
// and then modifying detectors
// to account for beam position, beam current, and some noise
// This will also allow for faking the helicity signals

  VaEvent::Decode(devices);  // first, handle the real data

#ifdef FAKEDITHER
  Int_t key;

   for (Int_t i = 0; i < 4; ++i)
     {
       // set detector = cal * bcm
       key = DETOFF + 3*i +1;
       fData[key] = fData[IBCM1]*(fDetVsBcm[i] + fgR.Gaus(0,fDetNoiseR[i]));
       devices.SetUsed(key);
       devices.SetUsed(key-1);
     }

  // modify position by dithering slope for 12x, 4a x,y and 4b x,y given value
  //  and ident of dithering object
   static Int_t fakeit;
   static Int_t fakecyc;
   fakeit++;
   if (fakeit == 2000)
     {
       fakecyc++;
       fakeit = 0;
     }
   if (fakeit < 100)
     {
       fData[IBMWOBJ] = fakeit / 15;
       fData[IBMWCLN] = 1;
       fData[IBMWCYC] = fakecyc;
     }
   else
     {
       fData[IBMWOBJ] = -1;
       fData[IBMWCLN] = 0;
       fData[IBMWCYC] = 0;
     }

  if (fData[IBMWOBJ]<7 && fData[IBMWOBJ]>-1 && 
      fData[IBMWCYC]>0 && fData[IBMWCLN]>0    ) {
    Int_t icoil = (int) fData[IBMWOBJ];
    if (icoil>=0) {
      fData[IBPM4AX] += fBpm4AXvCoil[icoil]*fData[IBMWVAL]/1.E6;
      fData[IBPM4BX] += fBpm4BXvCoil[icoil]*fData[IBMWVAL]/1.E6;
      fData[IBPM4AY] += fBpm4AYvCoil[icoil]*fData[IBMWVAL]/1.E6;
      fData[IBPM4BY] += fBpm4BYvCoil[icoil]*fData[IBMWVAL]/1.E6;
      fData[IBPM12X] += fBpm12XvCoil[icoil]*fData[IBMWVAL]/1.E6;
    }
  }

  // modify detector by position
  for (Int_t i = 0; i < DETNUM; i++) {
    key = DETOFF + 3*i +1;
    fData[key] *= (1.0+fDetVsBpm4AX[i]*(fData[IBPM4AX]-fDet4AXoff));
    fData[key] *= (1.0+fDetVsBpm4BX[i]*(fData[IBPM4BX]-fDet4BXoff));
    fData[key] *= (1.0+fDetVsBpm4AY[i]*(fData[IBPM4AY]-fDet4AYoff));
    fData[key] *= (1.0+fDetVsBpm4BY[i]*(fData[IBPM4BY]-fDet4BYoff));
    fData[key] *= (1.0+fDetVsBpm12X[i]*(fData[IBPM12X]-fDet12Xoff));
  }
#endif

#ifdef FAKEHEL
  // overwrite recorded control signals
  while (1)
    {
      if (!fgHelfile.is_open())
	clog << "TaSimEvent::Decode ERROR helicity.data file not open" << endl;

      fgHelfile >> fData[IHELICITY] >> fData[IPAIRSYNCH]
		>> fData[IMULTIPLETSYNCH] >> fData[ITIMESLOT];
      if (fgHelfile.eof())
	{
	  fgHelfile.close();
	  fgHelfile.open ("helicity.data");
	}
      else
	break;
    }
//   clog << "TaSimEvent::Load hel/ps/ms/ts: " 
//        << " " << fData[IHELICITY]
//        << " " << fData[IPAIRSYNCH]
//        << " " << fData[IMULTIPLETSYNCH]
//        << " " << fData[ITIMESLOT] << endl;
#endif

#ifdef FAKEHELNOFILE
  static Int_t tsCount = 0; // count of timeslots
  static Int_t mCount = fgNMultiplet+1;  // count of multiplets
  static UInt_t lms;  // multiplet synch
  static UInt_t lps;  // pair synch
  static UInt_t eb0 = 0;   // helicity (0 or 1) of first window
  static UInt_t eb;   // helicity of current window

  if (tsCount == 0)
    {
      // First event of a window
      if (mCount == fgNMultiplet+1)
	mCount = 0;
      else
	mCount = (mCount + 1) % fgNMultiplet;
      lms = mCount == 0 ? FirstMS : OtherMS;
      lps = mCount % 2 == 0 ? FirstPS : SecondPS;
      if (mCount == 0)
	{
	  if (fgRandom)
	    eb0 = RanBit ();
	  eb = eb0;
	}
	else
	  eb = eb0 ^ fgPattern[mCount];
    }
  fData[ITIMESLOT] = tsCount + 1;
  fData[IPAIRSYNCH] = lps == FirstPS ? 1 : 0;
  fData[IMULTIPLETSYNCH] = lms == FirstMS ? 1 : 0;
  fData[IHELICITY] = eb;

//   clog << "TaSimEvent::Load hel/ps/ms/ts: " 
//        << " " << (GetROHelicity() == RightHeli ? "R" : "L")
//        << " " << (GetPairSynch() == FirstPS ? "F" : "S")
//        << " " << (GetMultipletSynch() == FirstMS ? "F" : "O")
//        << " " << GetTimeSlot()
//        << endl;
  
  tsCount = (tsCount + 1) % fgOversample;
#endif

};

void TaSimEvent::Create(const TaSimEvent& rhs)
{
  // Utility routine used by copy constructor and assignment.
  VaEvent::Create(rhs);

  // also copy simulation specific arrays... err, reset, but it's ok, 'cause
  //  they are hardwired in anyway...
#ifdef FAKEDITHER
  this->SetSimConstants();
#endif
};




#ifdef FAKEHELNOFILE
UInt_t 
TaSimEvent::RanBit (UInt_t hRead)
{
  // Pseudorandom bit generator.  New bit is returned.  This algorithm
  // mimics the one implemented in hardware in the helicity box and is
  // used for random helicity mode to set the helicity bit for the
  // first window of each window pair/quad/octet.

  // Except: if the helicity bit actually read is passed as argument,
  // it is used to update the shift register, not the generated bit.
  UInt_t ret = 0;
  switch (fgRanType)
  {
    case 0: 
      ret = RanBitOld (hRead);
      break;
    case 1: 
      ret = RanBit24 (hRead);
      break;
    case 2: 
      ret = RanBit30 (hRead);
      break;
    }
  return ret;
}

UInt_t 
TaSimEvent::RanBitOld (UInt_t hRead)
{
  // Pseudorandom bit generator, used for old data, based on 24 bit
  // shift register.  New bit is XOR of bits 17, 22, 23, 24 of 24 bit
  // shift register fgShreg.  New fgShreg is old one shifted one bit
  // left, with new bit injected at bit 1. (bit numbered 1 on right to
  // 24 on left.)  

  UInt_t bit24  = (fgShreg & 0x800000) != 0;
  UInt_t bit23  = (fgShreg & 0x400000) != 0;
  UInt_t bit22  = (fgShreg & 0x200000) != 0;
  UInt_t bit17  = (fgShreg & 0x010000) != 0;
  UInt_t newbit = ( bit24 ^ bit23 ^ bit22 ^ bit17 ) & 0x1;
  fgShreg = ( (hRead == 2 ? newbit : hRead) | (fgShreg << 1 )) & 0xFFFFFF;
  return newbit; 
}

UInt_t 
TaSimEvent::RanBit24 (UInt_t hRead)
{
  // Pseudorandom bit generator, used for newer data, based on 24 bit
  // shift register.  New bit is bit 24 of shift register fgShreg.
  // New fgShreg is old one XOR bits 1, 3, 4, 24, shifted one bit
  // left, with new bit injected at bit 1. (bit numbered 1 on right to
  // 24 on left.)

  const UInt_t IB1 = 0x1;	        // Bit 1 mask
  const UInt_t IB3 = 0x4;	        // Bit 3 mask
  const UInt_t IB4 = 0x8;	        // Bit 4 mask
  const UInt_t IB24 = 0x800000;         // Bit 24 mask
  const UInt_t MASK = IB1+IB3+IB4+IB24;	// 100000000000000000001101
 
  int hPred = (fgShreg & IB24) ? 1 : 0;

  if ((hRead == 2 ? hPred : hRead) == 1)
    fgShreg = ((fgShreg ^ MASK) << 1) | IB1;
  else
    fgShreg <<= 1;

  return hPred;
}

UInt_t 
TaSimEvent::RanBit30 (UInt_t hRead)
{
  // Pseudorandom bit generator, used for even newer data, based on 30 bit
  // shift register.  New bit is bit 30 of shift register fgShreg.
  // New fgShreg is old one XOR bits 1, 4, 6, 30, shifted one bit
  // left, with new bit injected at bit 1. (bit numbered 1 on right to
  // 30 on left.)

  const UInt_t IB1 = 0x1;	        // Bit 1 mask
  const UInt_t IB4 = 0x8;	        // Bit 4 mask
  const UInt_t IB6 = 0x20;	        // Bit 6 mask
  const UInt_t IB30 = 0x20000000;       // Bit 30 mask
  const UInt_t MASK = IB1+IB4+IB6+IB30;	// 100000000000000000000000101001
 
  int hPred = (fgShreg & IB30) ? 1 : 0;

  if ((hRead == 2 ? hPred : hRead) == 1)
    fgShreg = ((fgShreg ^ MASK) << 1) | IB1;
  else
    fgShreg <<= 1;

  return hPred;
}
#endif
