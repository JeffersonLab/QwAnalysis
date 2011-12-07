//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaStandardAna.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaStandardAna.cc,v 1.37 2010/04/17 00:48:15 dalton Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Standard analysis.  This class derives from VaAnalysis.  It
//    simply puts differences and asymmetries of beam monitors and
//    detectors into the output root file using the AutoPairAna lists,
//    and prints statistics on these quantities periodically.
//
////////////////////////////////////////////////////////////////////////

#define PVDIS

#include "TaStandardAna.hh"e "VaEvent.hh"
#include "TaRun.hh"
#include "TaLabelledQuantity.hh"
#include "TaDevice.hh"

#ifndef NODICT
ClassImp(TaStandardAna)
#endif

// Constructors/destructors/operators

TaStandardAna::TaStandardAna():VaAnalysis()
{
}

TaStandardAna::~TaStandardAna(){}


// Private member functions

// We should not need to copy or assign an analysis, so copy
// constructor and operator= are private.

TaStandardAna::TaStandardAna (const TaStandardAna& copy) 
{
}


TaStandardAna& TaStandardAna::operator=( const TaStandardAna& assign) 
{ 
  return *this; 
}


void TaStandardAna::EventAnalysis()
{
  // Event analysis.

  fEvt->AddResult ( TaLabelledQuantity ( "bcm1",
					 fEvt->GetData(IBCM1), 
					 "chan" ) );
  if (fRun->GetDevices().IsUsed(IBCM2))
    {
      fEvt->AddResult ( TaLabelledQuantity ( "bcm2",
					     fEvt->GetData(IBCM2), 
					     "chan" ) );
    }
  if (fRun->GetDevices().IsUsed(IDET1R))
    {
      fEvt->AddResult ( TaLabelledQuantity ( "det1",
					     fEvt->GetData(IDET1), 
					     "chan" ) );
    }
  if (fRun->GetDevices().IsUsed(IDET2R))
    {
      fEvt->AddResult ( TaLabelledQuantity ( "det2",
					     fEvt->GetData(IDET2), 
					     "chan" ) );
    }
  if (fRun->GetDevices().IsUsed(IDET3R))
    {
      fEvt->AddResult ( TaLabelledQuantity ( "det3",
					     fEvt->GetData(IDET3), 
					     "chan" ) );
    }
  if (fRun->GetDevices().IsUsed(IDET4R))
    {
      fEvt->AddResult ( TaLabelledQuantity ( "det4",
					     fEvt->GetData(IDET4), 
					     "chan" ) );
    }
}

void TaStandardAna::PairAnalysis()
{ 
  // Pair analysis
  // All we have here is a call to AutoPairAna.

  AutoPairAna();
}

void TaStandardAna::MultipletAnalysis()
{ 
  // Multiplet analysis
  // All we have here is a call to AutoMultipletAna.

  AutoMultipletAna();
}


void
TaStandardAna::InitChanLists ()
{
  // Initialize the lists used by InitTree and AutoPairAna.
  // TaStandardAna's implementation puts channels associated with beam
  // devices and electron detectors, into the lists.

  // Initialize the lists of devices to analyze
  vector<AnaList> f;


  // Channels for which to store left and right values
  fTreeList = ChanListFromName ("helicity", "", fgCOPY);
  f = ChanListFromName ("pairsynch", "", fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanListFromName ("multipletsynch", "", fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanListFromName ("timeslot", "", fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanListFromName ("pitadac", "", fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("batt", "~", "chan", fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("bpm", "~x", "mm", fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("bpm", "~y", "mm", fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("bcm", "~", "chan", fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lumi", "~", "chan", fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanListFromName ("bmw", "", fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanListFromName ("scan", "",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());

  f = ChanListFromName ("bpm12mx", "",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanListFromName ("bpm4amx", "",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanListFromName ("bpm4bmx", "",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());

  // Other Useful Stuff - Paul King and Mark Dalton  2010-04-16
  f = ChanListFromName ("integtime", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());  
  f = ChanListFromName ("v2f_clk0", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());  

  f = ChanListFromName ("bpm12xp", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());  
  f = ChanListFromName ("bpm12xm", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());  
  f = ChanListFromName ("bpm12yp", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());  
  f = ChanListFromName ("bpm12ym", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());  

  f = ChanListFromName ("bpm10xp", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());  
  f = ChanListFromName ("bpm10xm", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());  
  f = ChanListFromName ("bpm10yp", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());  
  f = ChanListFromName ("bpm10ym", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());  

//
  // QWEAK variables
 //  f = ChanList ("vqwk", "~_0_b1_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_0_b2_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_0_b3_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_0_b4_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_1_b1_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_1_b2_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_1_b3_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_1_b4_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_2_b1_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_2_b2_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_2_b3_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_2_b4_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_3_b1_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_3_b2_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_3_b3_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_3_b4_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_4_b1_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_4_b2_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_4_b3_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_4_b4_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_5_b1_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_5_b2_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_5_b3_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_5_b4_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_6_b1_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_6_b2_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_6_b3_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_6_b4_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_7_b1_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_7_b2_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_7_b3_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_7_b4_cal", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  //
  // QPD 
  f = ChanList ("qpd", "~sum", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("qpd", "~x", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("qpd", "~y", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  //
  // Linear Array
  f = ChanList ("lina", "~sum", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~x", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~rms", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~xg", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~rmsg", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_1", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_2", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_3", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_4", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_5", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_6", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_7", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_8", "chan",  fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());

#ifdef PVDIS
  // PVDIS variables
  f = ChanList ("pvdis", "~", "chan", fgCOPY);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
#endif

  // Channels for which to store differences
  // f = ChanList ("vqwk", "~_0_b1_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_0_b2_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_0_b3_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_0_b4_cal","chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_1_b1_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_1_b2_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_1_b3_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_1_b4_cal","chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_2_b1_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_2_b2_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_2_b3_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_2_b4_cal","chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_3_b1_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_3_b2_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_3_b3_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_3_b4_cal","chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_4_b1_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_4_b2_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_4_b3_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_4_b4_cal","chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_5_b1_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_5_b2_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_5_b3_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_5_b4_cal","chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_6_b1_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_6_b2_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_6_b3_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_6_b4_cal","chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_7_b1_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_7_b2_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_7_b3_cal", "chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_7_b4_cal","chan", fgDIFF + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  

  f = ChanList ("batt", "~", "mchan", fgDIFF + fgBLINDSIGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("bpm", "~x", "um", fgDIFF + fgBLINDSIGN);
  for (vector<AnaList>::iterator i_f = f.begin();
       i_f < f.end();
       ++i_f)
    {
      if (i_f->fVarStr == "bpm12x" ||
	  i_f->fVarStr == "bpm4ax" ||
	  i_f->fVarStr == "bpm4bx"
	  )
	i_f->fFlagInt += fgSTATS;
    }
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("bpm", "~y", "um", fgDIFF + fgBLINDSIGN);
  for (vector<AnaList>::iterator i_f = f.begin();
       i_f < f.end();
       ++i_f)
    {
      if (i_f->fVarStr == "bpm12y" ||
	  i_f->fVarStr == "bpm4ay" ||
	  i_f->fVarStr == "bpm4by"
	  )
	i_f->fFlagInt += fgSTATS;
    }
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("qpd", "~x", "um", fgDIFF + fgBLINDSIGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("qpd", "~y", "um", fgDIFF + fgBLINDSIGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  //
  // Linear Array
  f = ChanList ("lina", "~x", "um", fgDIFF);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~rms", "um", fgDIFF);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~xg", "um", fgDIFF);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~rmsg", "um", fgDIFF);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());

  // Channels for which to store averages
  f = ChanList ("bcm", "~", "chan", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("bpm", "~x", "um", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("bpm", "~y", "um", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lumi", "~", "chan", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lumi", "~", "", fgAVGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("det", "~", "chan", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("det", "~", "", fgAVGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanListFromName ("bmw", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());  
  //
  //QPD
  f = ChanList ("qpd", "~sum", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("qpd", "~sum", "", fgAVGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end()); 
  //
  f = ChanList ("lina", "~sum", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~x", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~rms", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~xg", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~rmsg", "", fgAVG);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  //

#ifdef PVDIS
  // PVDIS variables
  // f = ChanList ("pvdis", "~", "chan", fgAVG);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());

// PVDIS avg
  f = ChanList ("pvdis", "~", "chan", fgNONE);
  for (vector<AnaList>::iterator i_f = f.begin();
       i_f < f.end();
       ++i_f)
    {
      if (i_f->fVarStr == "dislen1" || i_f->fVarStr == "dislen2" ||
	  i_f->fVarStr == "dislen3" || i_f->fVarStr == "dislen4" ||
	  i_f->fVarStr == "dislen5" || i_f->fVarStr == "dislen6" ||
	  i_f->fVarStr == "dislen1c" || i_f->fVarStr == "dislen2c" ||
	  i_f->fVarStr == "dislen3c" || i_f->fVarStr == "dislen4c" ||
	  i_f->fVarStr == "dislen5c" || i_f->fVarStr == "dislen6c" ||
	  i_f->fVarStr == "dislew1" || i_f->fVarStr == "dislew2" ||
	  i_f->fVarStr == "dislew3" || i_f->fVarStr == "dislew4" ||
	  i_f->fVarStr == "dislew5" || i_f->fVarStr == "dislew6" ||
	  i_f->fVarStr == "dislew1c" || i_f->fVarStr == "dislew2c" ||
	  i_f->fVarStr == "dislew3c" || i_f->fVarStr == "dislew4c" ||
	  i_f->fVarStr == "dislew5c" || i_f->fVarStr == "dislew6c" ||
	  i_f->fVarStr == "dislenar" || i_f->fVarStr == "dislenarc" ||
	  i_f->fVarStr == "dislewid" || i_f->fVarStr == "dislewidc" ||
	  i_f->fVarStr == "disren1" || i_f->fVarStr == "disren2" ||
	  i_f->fVarStr == "disren3" || i_f->fVarStr == "disren4" ||
	  i_f->fVarStr == "disren5" || i_f->fVarStr == "disren6" ||
	  i_f->fVarStr == "disren7" || i_f->fVarStr == "disren8" ||
	  i_f->fVarStr == "disrew1" || i_f->fVarStr == "disrew2" ||
	  i_f->fVarStr == "disrew3" || i_f->fVarStr == "disrew4" ||
	  i_f->fVarStr == "disrew5" || i_f->fVarStr == "disrew6" ||
	  i_f->fVarStr == "disrew7" || i_f->fVarStr == "disrew8" ||
	  i_f->fVarStr == "disrts1c" || i_f->fVarStr == "disrts2c" ||
	  i_f->fVarStr == "disrts3c" || i_f->fVarStr == "disrts4c" ||
	  i_f->fVarStr == "disrts5c" || i_f->fVarStr == "disrts6c" ||
	  i_f->fVarStr == "disrts7c" || i_f->fVarStr == "disrts8c" ||
	  i_f->fVarStr == "disrps1c" || i_f->fVarStr == "disrps2c" ||
	  i_f->fVarStr == "disrps3c" || i_f->fVarStr == "disrps4c" ||
	  i_f->fVarStr == "disrps5c" || i_f->fVarStr == "disrps6c" ||
	  i_f->fVarStr == "disrps7c" || i_f->fVarStr == "disrps8c" ||
	  i_f->fVarStr == "disrenar" || i_f->fVarStr == "disrenarc" ||
	  i_f->fVarStr == "disrewid" || i_f->fVarStr == "disrewidc" ||
	  i_f->fVarStr == "disrpnar" || i_f->fVarStr == "disrpnarc" ||
	  i_f->fVarStr == "disrpwid" || i_f->fVarStr == "disrpwidc" )

	i_f->fFlagInt += fgAVG + fgBLINDSIGN;
    }
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());

  // PVDIS asym
  f = ChanList ("pvdis", "~", "chan", fgNONE);
  for (vector<AnaList>::iterator i_f = f.begin();
       i_f < f.end();
       ++i_f)
    {
      if (i_f->fVarStr == "dislen1" || i_f->fVarStr == "dislen2" ||
	  i_f->fVarStr == "dislen3" || i_f->fVarStr == "dislen4" ||
	  i_f->fVarStr == "dislen5" || i_f->fVarStr == "dislen6" ||
	  i_f->fVarStr == "dislen1c" || i_f->fVarStr == "dislen2c" ||
	  i_f->fVarStr == "dislen3c" || i_f->fVarStr == "dislen4c" ||
	  i_f->fVarStr == "dislen5c" || i_f->fVarStr == "dislen6c" || 
	  i_f->fVarStr == "dislew1" || i_f->fVarStr == "dislew2" ||
	  i_f->fVarStr == "dislew3" || i_f->fVarStr == "dislew4" ||
	  i_f->fVarStr == "dislew5" || i_f->fVarStr == "dislew6" ||
	  i_f->fVarStr == "dislew1c" || i_f->fVarStr == "dislew2c" ||
	  i_f->fVarStr == "dislew3c" || i_f->fVarStr == "dislew4c" ||
	  i_f->fVarStr == "dislew5c" || i_f->fVarStr == "dislew6c" ||
          i_f->fVarStr == "dislenar" || i_f->fVarStr == "dislenarc" ||
	  i_f->fVarStr == "dislewid" || i_f->fVarStr == "dislewidc" ||
	  i_f->fVarStr == "disren1" || i_f->fVarStr == "disren2" ||
	  i_f->fVarStr == "disren3" || i_f->fVarStr == "disren4" ||
	  i_f->fVarStr == "disren5" || i_f->fVarStr == "disren6" ||
	  i_f->fVarStr == "disren7" || i_f->fVarStr == "disren8" ||
	  i_f->fVarStr == "disrew1" || i_f->fVarStr == "disrew2" ||
	  i_f->fVarStr == "disrew3" || i_f->fVarStr == "disrew4" ||
	  i_f->fVarStr == "disrew5" || i_f->fVarStr == "disrew6" ||
	  i_f->fVarStr == "disrew7" || i_f->fVarStr == "disrew8" ||
	  i_f->fVarStr == "disrts1c" || i_f->fVarStr == "disrts2c" ||
	  i_f->fVarStr == "disrts3c" || i_f->fVarStr == "disrts4c" ||
	  i_f->fVarStr == "disrts5c" || i_f->fVarStr == "disrts6c" ||
	  i_f->fVarStr == "disrts7c" || i_f->fVarStr == "disrts8c" ||
	  i_f->fVarStr == "disrps1c" || i_f->fVarStr == "disrps2c" ||
	  i_f->fVarStr == "disrps3c" || i_f->fVarStr == "disrps4c" ||
	  i_f->fVarStr == "disrps5c" || i_f->fVarStr == "disrps6c" ||
	  i_f->fVarStr == "disrps7c" || i_f->fVarStr == "disrps8c" ||
	  i_f->fVarStr == "disrenar" || i_f->fVarStr == "disrenarc" ||
	  i_f->fVarStr == "disrewid" || i_f->fVarStr == "disrewidc" ||
	  i_f->fVarStr == "disrpnar" || i_f->fVarStr == "disrpnarc" ||
	  i_f->fVarStr == "disrpwid" || i_f->fVarStr == "disrpwidc"
	  )
	i_f->fFlagInt += fgASY + fgBLINDSIGN;
      else
      i_f->fFlagInt += fgASY;
    }
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());

#endif

  // Channels for which to store asymmetries
//    f = ChanList ("bpm", "~xws", "ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
//    fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//    f = ChanList ("bpm", "~yws", "ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
//    fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  // QWEAK
  // f = ChanList ("vqwk", "~_0_b1_cal", "ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_0_b2_cal", "ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_0_b3_cal", "ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_0_b4_cal", "ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_1_b1_cal", "ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_1_b2_cal", "ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_1_b3_cal", "ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("vqwk", "~_1_b4_cal", "ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  //
  //
  f = ChanList ("bpm", "~ws", "ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("bcm", "~", "ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
  for (vector<AnaList>::iterator i_f = f.begin();
       i_f < f.end();
       ++i_f)
    {
      if (i_f->fVarStr == "bcm1" ||
	  i_f->fVarStr == "bcm2"
	  )
	i_f->fFlagInt += fgSTATS;
      if (i_f->fVarStr == "bcm1")
	i_f->fFlagInt += fgORDERED;
      if (i_f->fVarStr == "bcm10")
	i_f->fFlagInt = fgNO_BEAM_C_NO_ASY + fgASY + fgBLINDSIGN + fgSTATS;
    }
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lumi", "~", "ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("qpd", "~sum", "ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());

  //
  f = ChanList ("lina", "~sum", "ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  //

  // Channels for which to store normalized asymmetries
  f = ChanList ("lumi", "~", "ppm", fgNO_BEAM_NO_ASY + fgASYN + fgBLINDSIGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());

  f = ChanList ("det", "~", "ppm", fgNO_BEAM_NO_ASY + fgASYN + fgBLIND + fgSTATS);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  //
  // Linear Array
  f = ChanList ("lina", "~sum", "ppm", fgNO_BEAM_NO_ASY + fgASYN + fgBLINDSIGN+ fgSTATS);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_1","ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN); 
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_2","ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_3","ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_4","ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_5","ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_6","ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_7","ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  f = ChanList ("lina", "~_8","ppm", fgNO_BEAM_NO_ASY + fgASY + fgBLINDSIGN);
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());
  //
  // Below is not the correct way to do this...
//   f = ChanListFromName ("det_", "ppm",  fgNO_BEAM_NO_ASY + fgASYN + fgBLIND);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());

#ifdef PVDIS
  // for pvdis
 //  f = ChanList ("pvdis", "~", "chan", fgNO_BEAM_NO_ASY + fgASYN + fgBLINDSIGN);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
 f = ChanList ("pvdis", "~", "chan", fgNONE);
  for (vector<AnaList>::iterator i_f = f.begin();
       i_f < f.end();
       ++i_f)
    {
      if (i_f->fVarStr == "dislen1" || i_f->fVarStr == "dislen2" ||
	  i_f->fVarStr == "dislen3" || i_f->fVarStr == "dislen4" ||
	  i_f->fVarStr == "dislen5" || i_f->fVarStr == "dislen6" ||
	  i_f->fVarStr == "dislen1c" || i_f->fVarStr == "dislen2c" ||
	  i_f->fVarStr == "dislen3c" || i_f->fVarStr == "dislen4c" ||
	  i_f->fVarStr == "dislen5c" || i_f->fVarStr == "dislen6c" || 
	  i_f->fVarStr == "dislew1" || i_f->fVarStr == "dislew2" ||
	  i_f->fVarStr == "dislew3" || i_f->fVarStr == "dislew4" ||
	  i_f->fVarStr == "dislew5" || i_f->fVarStr == "dislew6" ||
	  i_f->fVarStr == "dislew1c" || i_f->fVarStr == "dislew2c" ||
	  i_f->fVarStr == "dislew3c" || i_f->fVarStr == "dislew4c" ||
	  i_f->fVarStr == "dislew5c" || i_f->fVarStr == "dislew6c" ||
          i_f->fVarStr == "dislenar" || i_f->fVarStr == "dislenarc" ||
	  i_f->fVarStr == "dislewid" || i_f->fVarStr == "dislewidc" ||
	  i_f->fVarStr == "disren1" || i_f->fVarStr == "disren2" ||
	  i_f->fVarStr == "disren3" || i_f->fVarStr == "disren4" ||
	  i_f->fVarStr == "disren5" || i_f->fVarStr == "disren6" ||
	  i_f->fVarStr == "disren7" || i_f->fVarStr == "disren8" ||
	  i_f->fVarStr == "disrew1" || i_f->fVarStr == "disrew2" ||
	  i_f->fVarStr == "disrew3" || i_f->fVarStr == "disrew4" ||
	  i_f->fVarStr == "disrew5" || i_f->fVarStr == "disrew6" ||
	  i_f->fVarStr == "disrew7" || i_f->fVarStr == "disrew8" ||
	  i_f->fVarStr == "disrts1c" || i_f->fVarStr == "disrts2c" ||
	  i_f->fVarStr == "disrts3c" || i_f->fVarStr == "disrts4c" ||
	  i_f->fVarStr == "disrts5c" || i_f->fVarStr == "disrts6c" ||
	  i_f->fVarStr == "disrts7c" || i_f->fVarStr == "disrts8c" ||
	  i_f->fVarStr == "disrps1c" || i_f->fVarStr == "disrps2c" ||
	  i_f->fVarStr == "disrps3c" || i_f->fVarStr == "disrps4c" ||
	  i_f->fVarStr == "disrps5c" || i_f->fVarStr == "disrps6c" ||
	  i_f->fVarStr == "disrps7c" || i_f->fVarStr == "disrps8c" ||
	  i_f->fVarStr == "disrenar" || i_f->fVarStr == "disrenarc" ||
	  i_f->fVarStr == "disrewid" || i_f->fVarStr == "disrewidc" ||
	  i_f->fVarStr == "disrpnar" || i_f->fVarStr == "disrpnarc" ||
	  i_f->fVarStr == "disrpwid" || i_f->fVarStr == "disrpwidc"
	  )
	i_f->fFlagInt += fgASYN + fgBLINDSIGN;
   //    else
//       i_f->fFlagInt += fgASY;
    }
  fTreeList.insert (fTreeList.end(), f.begin(), f.end());

#endif

  // multi-detector asymmetries - 
  // ***  mostly removed from here and generated from raw tree entries  -KDP
  //
  // Here we assume there are four detector signals:
  //   det1 = left spectrometer,  low Q^2
  //   det2 = left spectrometer,  high Q^2
  //   det3 = right spectrometer, low Q^2
  //   det4 = right spectrometer, high Q^2
  // We do sums of left and right, sums of high and low, sum of all,
  // and average of all.
  //
  // (Unfortunate terminology: here "right" and "left" refer to which
  // spectrometer, not which helicity.)
  // Change by LRM for PREX 4/1/10
  //   det1 = right spectrometer,  low Q^2
  //   det2 = right spectrometer,  high Q^2
  //   det3 = left spectrometer, low Q^2
  //   det4 = left spectrometer, high Q^2

  vector<Int_t> keys(0);
  vector<Double_t> wts(0);
  vector<Double_t> wtsa(0);

  if (fRun->GetDevices().IsUsed(IBLUMI1R) &&
      fRun->GetDevices().IsUsed(IBLUMI2R) &&
      fRun->GetDevices().IsUsed(IBLUMI3R) &&
      fRun->GetDevices().IsUsed(IBLUMI4R) &&
      fRun->GetDevices().IsUsed(IBLUMI5R) &&
      fRun->GetDevices().IsUsed(IBLUMI6R) &&
      fRun->GetDevices().IsUsed(IBLUMI7R) &&
      fRun->GetDevices().IsUsed(IBLUMI8R))
    {
      keys.clear(); keys.push_back(IBLUMI1);  keys.push_back(IBLUMI2);
      keys.push_back(IBLUMI3);  keys.push_back(IBLUMI4);
      keys.push_back(IBLUMI5);  keys.push_back(IBLUMI6);
      keys.push_back(IBLUMI7);  keys.push_back(IBLUMI8);
      wts = fRun->GetDataBase().GetBlumiWts();
      if (!WtsOK (wts)) 
	{
	  cerr << "TaStandardAna::InitChanLists WARNING: blumi weights bad, ignored" << endl;
	  wts.clear();
	}
      fTreeList.push_back (AnaList ("blumi_sum", keys, wts, "ppm", 
				    fgNO_BEAM_NO_ASY + fgASYN + fgBLINDSIGN + fgSTATS));
      fTreeList.push_back (AnaList ("blumi_sum", keys, wts, "ppm", 
				    fgAVG));
      fTreeList.push_back (AnaList ("blumi_sum", keys, wts, "ppm", 
				    fgAVGN));
      fTreeList.push_back (AnaList ("blumi_ave", keys, wts, "ppm", 
				    fgNO_BEAM_NO_ASY + fgASYN + fgAVE + fgBLINDSIGN));
    }

  if (fRun->GetDevices().IsUsed(IBLUMI1R) &&
      fRun->GetDevices().IsUsed(IBLUMI5R))
    {
      keys.clear(); keys.push_back(IBLUMI1);  keys.push_back(IBLUMI5);
      wtsa = fRun->GetDataBase().GetBlumiWts();
      wts.clear(); wts.push_back (wtsa[0]); wts.push_back (wtsa[4]); 
      if (!WtsOK (wts)) 
	{
	  cerr << "TaStandardAna::InitChanLists WARNING: blumi 1/5 weights bad, ignored" << endl;
	  wts.clear();
	}
      fTreeList.push_back (AnaList ("blumi_v", keys, wts, "ppm", 
				    fgNO_BEAM_NO_ASY + fgASYN + fgBLINDSIGN));
      fTreeList.push_back (AnaList ("blumi_v", keys, wts, "ppm", 
				    fgAVG));
      fTreeList.push_back (AnaList ("blumi_v", keys, wts, "ppm", 
				    fgAVGN));
    }

  if (fRun->GetDevices().IsUsed(IBLUMI3R) &&
      fRun->GetDevices().IsUsed(IBLUMI7R))
    {
      keys.clear(); keys.push_back(IBLUMI3);  keys.push_back(IBLUMI7);
      wtsa = fRun->GetDataBase().GetBlumiWts();
      wts.clear(); wts.push_back (wtsa[2]); wts.push_back (wtsa[6]); 
      if (!WtsOK (wts)) 
	{
	  cerr << "TaStandardAna::InitChanLists WARNING: blumi 3/7 weights bad, ignored" << endl;
	  wts.clear();
	}
      fTreeList.push_back (AnaList ("blumi_h", keys, wts, "ppm", 
				    fgNO_BEAM_NO_ASY + fgASYN + fgBLINDSIGN));
      fTreeList.push_back (AnaList ("blumi_h", keys, wts, "ppm", 
				    fgAVG));
      fTreeList.push_back (AnaList ("blumi_h", keys, wts, "ppm", 
				    fgAVGN));
    }

  if (fRun->GetDevices().IsUsed(IBLUMI2R) &&
      fRun->GetDevices().IsUsed(IBLUMI6R))
    {
      keys.clear(); keys.push_back(IBLUMI2);  keys.push_back(IBLUMI6);
      wtsa = fRun->GetDataBase().GetBlumiWts();
      wts.clear(); wts.push_back (wtsa[1]); wts.push_back (wtsa[5]); 
      if (!WtsOK (wts)) 
	{
	  cerr << "TaStandardAna::InitChanLists WARNING: blumi 2/6 weights bad, ignored" << endl;
	  wts.clear();
	}
      fTreeList.push_back (AnaList ("blumi_d1", keys, wts, "ppm", 
				    fgNO_BEAM_NO_ASY + fgASYN + fgBLINDSIGN));
      fTreeList.push_back (AnaList ("blumi_d1", keys, wts, "ppm", 
				    fgAVG));
      fTreeList.push_back (AnaList ("blumi_d1", keys, wts, "ppm", 
				    fgAVGN));
    }

  if (fRun->GetDevices().IsUsed(IBLUMI4R) &&
      fRun->GetDevices().IsUsed(IBLUMI8R))
    {
      keys.clear(); keys.push_back(IBLUMI4);  keys.push_back(IBLUMI8);
      wtsa = fRun->GetDataBase().GetBlumiWts();
      wts.clear(); wts.push_back (wtsa[3]); wts.push_back (wtsa[7]); 
      if (!WtsOK (wts)) 
	{
	  cerr << "TaStandardAna::InitChanLists WARNING: blumi 4/8 weights bad, ignored" << endl;
	  wts.clear();
	}
      fTreeList.push_back (AnaList ("blumi_d2", keys, wts, "ppm", 
				    fgNO_BEAM_NO_ASY + fgASYN + fgBLINDSIGN));
      fTreeList.push_back (AnaList ("blumi_d2", keys, wts, "ppm", 
				    fgAVG));
      fTreeList.push_back (AnaList ("blumi_d2", keys, wts, "ppm", 
				    fgAVGN));
    }

  if (fRun->GetDevices().IsUsed(IBLUMI1R) &&
      fRun->GetDevices().IsUsed(IBLUMI5R) && 
      fRun->GetDevices().IsUsed(IBLUMI3R) &&
      fRun->GetDevices().IsUsed(IBLUMI7R))
    {
      keys.clear(); keys.push_back(IBLUMI1);  keys.push_back(IBLUMI3);
      keys.push_back(IBLUMI5);  keys.push_back(IBLUMI7);
      wtsa = fRun->GetDataBase().GetBlumiWts();
      wts.clear(); wts.push_back (wtsa[0]); wts.push_back (wtsa[2]); 
      wts.push_back (wtsa[4]); wts.push_back (wtsa[6]); 
      if (!WtsOK (wts)) 
	{
	  cerr << "TaStandardAna::InitChanLists WARNING: blumi 1/3/5/7 weights bad, ignored" << endl;
	  wts.clear();
	}
      fTreeList.push_back (AnaList ("blumi_c", keys, wts, "ppm", 
				    fgNO_BEAM_NO_ASY + fgASYN + fgBLINDSIGN + fgSTATS));
      fTreeList.push_back (AnaList ("blumi_c", keys, wts, "ppm", 
				    fgAVG));
      fTreeList.push_back (AnaList ("blumi_c", keys, wts, "ppm", 
				    fgAVGN));
    }

  if (fRun->GetDevices().IsUsed(IBLUMI2R) &&
      fRun->GetDevices().IsUsed(IBLUMI4R) && 
      fRun->GetDevices().IsUsed(IBLUMI6R) &&
      fRun->GetDevices().IsUsed(IBLUMI8R))
    {
      keys.clear(); keys.push_back(IBLUMI2);  keys.push_back(IBLUMI4);
      keys.push_back(IBLUMI6);  keys.push_back(IBLUMI8);
      wtsa = fRun->GetDataBase().GetBlumiWts();
      wts.clear(); wts.push_back (wtsa[1]); wts.push_back (wtsa[3]); 
      wts.push_back (wtsa[5]); wts.push_back (wtsa[7]); 
      if (!WtsOK (wts)) 
	{
	  cerr << "TaStandardAna::InitChanLists WARNING: blumi 2/4/6/8 weights bad, ignored" << endl;
	  wts.clear();
	}
      fTreeList.push_back (AnaList ("blumi_x", keys, wts, "ppm", 
				    fgNO_BEAM_NO_ASY + fgASYN + fgBLINDSIGN + fgSTATS));
      fTreeList.push_back (AnaList ("blumi_x", keys, wts, "ppm", 
				    fgAVG));
      fTreeList.push_back (AnaList ("blumi_x", keys, wts, "ppm", 
				    fgAVGN));
    }


  if (fRun->GetDevices().IsUsed(IFLUMI1R) &&
      fRun->GetDevices().IsUsed(IFLUMI2R))
    {
      keys.clear(); keys.push_back(IFLUMI1);  keys.push_back(IFLUMI2);
      wts = fRun->GetDataBase().GetFlumiWts();
      if (!WtsOK (wts)) 
	{
	  cerr << "TaStandardAna::InitChanLists WARNING: flumi weights bad, ignored" << endl;
	  wts.clear();
	}
      fTreeList.push_back (AnaList ("flumi_sum", keys, wts, "ppm", 
				    fgNO_BEAM_NO_ASY + fgASYN + fgBLINDSIGN));
      fTreeList.push_back (AnaList ("flumi_sum", keys, wts, "ppm", 
				    fgAVG));
      fTreeList.push_back (AnaList ("flumi_sum", keys, wts, "ppm", 
				    fgAVGN));
      fTreeList.push_back (AnaList ("flumi_ave", keys, wts, "ppm", 
				    fgNO_BEAM_NO_ASY + fgASYN + fgAVE + fgBLINDSIGN));
    }

  if (fRun->GetDevices().IsUsed(IDET1R) &&
      fRun->GetDevices().IsUsed(IDET2R))
    {
      keys.clear(); keys.push_back(IDET1);  keys.push_back(IDET2);
      wtsa = fRun->GetDataBase().GetDetWts();
      wts.clear(); wts.push_back (wtsa[0]); wts.push_back (wtsa[1]); 
      if (!WtsOK (wts)) 
	{
	  cerr << "TaStandardAna::InitChanLists WARNING: det1/2 weights bad, ignored" << endl;
	  wts.clear();
	}
       fTreeList.push_back (AnaList ("det_r", keys, wts, "ppm", 
 				    fgNO_BEAM_NO_ASY + fgASYN + fgBLIND));
       fTreeList.push_back (AnaList ("det_r", keys, wts, "ppm", 
 				    fgAVG));
       fTreeList.push_back (AnaList ("det_r", keys, wts, "ppm", 
 				    fgAVGN));
     }
   if (fRun->GetDevices().IsUsed(IDET3R) &&
       fRun->GetDevices().IsUsed(IDET4R))
     {
       keys.clear(); keys.push_back(IDET3);  keys.push_back(IDET4);
       wtsa = fRun->GetDataBase().GetDetWts();
       wts.clear(); wts.push_back (wtsa[2]); wts.push_back (wtsa[3]); 
       if (!WtsOK (wts)) 
 	{
 	  cerr << "TaStandardAna::InitChanLists WARNING: det3/4 weights bad, ignored" << endl;
 	  wts.clear();
 	}
       fTreeList.push_back (AnaList ("det_l", keys, wts, "ppm", 
 				    fgNO_BEAM_NO_ASY + fgASYN + fgBLIND + fgSTATS));
       fTreeList.push_back (AnaList ("det_l", keys, wts, "ppm", 
 				    fgAVG));
       fTreeList.push_back (AnaList ("det_l", keys, wts, "ppm", 
 				    fgAVGN));
     }
   if (fRun->GetDevices().IsUsed(IDET1R) &&
       fRun->GetDevices().IsUsed(IDET3R))
     {
       keys.clear(); keys.push_back(IDET1);  keys.push_back(IDET3);
       wtsa = fRun->GetDataBase().GetDetWts();
       wts.clear(); wts.push_back (wtsa[0]); wts.push_back (wtsa[2]); 
       if (!WtsOK (wts)) 
 	{
 	  cerr << "TaStandardAna::InitChanLists WARNING: det1/3 weights bad, ignored" << endl;
 	  wts.clear();
 	}
       fTreeList.push_back (AnaList ("det_lo", keys, wts, "ppm", 
 				    fgNO_BEAM_NO_ASY + fgASYN + fgBLIND + fgSTATS));
       fTreeList.push_back (AnaList ("det_lo", keys, wts, "ppm", 
 				    fgAVG));
       fTreeList.push_back (AnaList ("det_lo", keys, wts, "ppm", 
 				    fgAVGN));
     }
   if (fRun->GetDevices().IsUsed(IDET2R) &&
       fRun->GetDevices().IsUsed(IDET4R))
     {
       keys.clear(); keys.push_back(IDET2);  keys.push_back(IDET4);
       wtsa = fRun->GetDataBase().GetDetWts();
       wts.clear(); wts.push_back (wtsa[1]); wts.push_back (wtsa[3]); 
       if (!WtsOK (wts)) 
 	{
 	  cerr << "TaStandardAna::InitChanLists WARNING: det2/4 weights bad, ignored" << endl;
 	  wts.clear();
 	}
       fTreeList.push_back (AnaList ("det_hi", keys, wts, "ppm", 
 				    fgNO_BEAM_NO_ASY + fgASYN + fgBLIND + fgSTATS));
       fTreeList.push_back (AnaList ("det_hi", keys, wts, "ppm", 
 				    fgAVG));
       fTreeList.push_back (AnaList ("det_hi", keys, wts, "ppm", 
 				    fgAVGN));
     }
   if (fRun->GetDevices().IsUsed(IDET1R) &&
       fRun->GetDevices().IsUsed(IDET2R) &&
       fRun->GetDevices().IsUsed(IDET3R) &&
       fRun->GetDevices().IsUsed(IDET4R))
     {
       keys.clear(); keys.push_back(IDET1);  keys.push_back(IDET2);
       keys.push_back(IDET3);  keys.push_back(IDET4);
       wtsa = fRun->GetDataBase().GetDetWts();
       wts.clear(); wts.push_back (wtsa[0]); wts.push_back (wtsa[1]); 
       wts.push_back (wtsa[2]); wts.push_back (wtsa[3]); 
       if (!WtsOK (wts)) 
 	{
 	  cerr << "TaStandardAna::InitChanLists WARNING: det1-4 weights bad, ignored" << endl;
 	  wts.clear();
 	}
       fTreeList.push_back (AnaList ("det_all", keys, wts, "ppm", 
 				    fgNO_BEAM_NO_ASY + fgASYN + fgBLIND + fgSTATS));
       fTreeList.push_back (AnaList ("det_ave", keys, wts, "ppm", 
 				    fgNO_BEAM_NO_ASY + fgASYN +fgAVE + fgBLIND + fgSTATS));
       fTreeList.push_back (AnaList ("det_all", keys, wts, "ppm", 
 				    fgAVG));
       fTreeList.push_back (AnaList ("det_all", keys, wts, "ppm", 
 				    fgAVGN));
     }

}
