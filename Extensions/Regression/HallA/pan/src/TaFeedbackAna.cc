//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaFeedbackAna.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaFeedbackAna.cc,v 1.4 2009/08/06 18:36:03 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Feedback analysis. Exclusive task is to compute and send feedback 
//    parameters to the source.   
//
////////////////////////////////////////////////////////////////////////

#include "TaFeedbackAna.hh"
#include "TaEvent.hh"
#include "TaRun.hh"
#include "TaLabelledQuantity.hh"

#ifndef NODICT
ClassImp(TaFeedbackAna)
#endif

// Constructors/destructors/operators

TaFeedbackAna::TaFeedbackAna():VaAnalysis()
{
  fDoSlice = kTRUE;
  fDoRun = kTRUE;
  fDoRoot =kFALSE;
}

TaFeedbackAna::~TaFeedbackAna(){}

TaFeedbackAna::TaFeedbackAna (const TaFeedbackAna& copy) 
{
}


TaFeedbackAna& TaFeedbackAna::operator=( const TaFeedbackAna& assign) 
{ 
  return *this; 
}


void TaFeedbackAna::EventAnalysis()
{
  fEvt->AddResult ( TaLabelledQuantity ( "bcm1 raw",
					 fEvt->GetData(IBCM1R), 
					 "chan" ) );
  fEvt->AddResult ( TaLabelledQuantity ( "bcm1",
					 fEvt->GetData(IBCM1), 
					 "chan" ) );
  fEvt->AddResult ( TaLabelledQuantity ( "bcm2 raw",
					 fEvt->GetData(IBCM2R), 
					 "chan" ) );
  fEvt->AddResult ( TaLabelledQuantity ( "bcm2",
					 fEvt->GetData(IBCM2), 
					 "chan" ) );
}

void TaFeedbackAna::PairAnalysis()
{ 
  AutoPairAna();
}


void
TaFeedbackAna::InitChanLists ()
{
  // no tree leaf defined, we won't write anything in the root file.
  vector<AnaList> f;

  // Channels for which to store left and right values
  fTreeList = ChanListFromName ("helicity", "", fgCOPY);
//   f = ChanListFromName ("multipletsynch", "", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanListFromName ("pairsynch", "", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanListFromName ("timeslot", "", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanListFromName ("pitadac", "", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("batt", "~", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("bpm", "~x", "mm", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("bpm", "~y", "mm", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("bcm", "~", "chan", fgCOPY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//  // Channels for which to store differences
//   f = ChanList ("batt", "~", "mchan", fgDIFF);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("bpm", "~x", "um", fgDIFF);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("bpm", "~y", "um", fgDIFF);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());

//   // Channels for which to store asymmetries
//   f = ChanList ("batt", "~", "ppm", fgASY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
//   f = ChanList ("bcm", "~", "ppm", fgNO_BEAM_NO_ASY + fgASY);
//   fTreeList.insert (fTreeList.end(), f.begin(), f.end());
}
