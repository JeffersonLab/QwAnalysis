//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaPairFromQuad.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaPairFromQuad.cc,v 1.14 2009/08/11 20:18:30 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Class which makes and analyzes opposite helicity event pairs
//    from a data stream structured as helicity window quadruples.
//    Derived from VaPair.
//
//////////////////////////////////////////////////////////////////////////

//#define NOISY

#include "VaEvent.hh"
#include "TaPairFromQuad.hh"
#include "TaRun.hh"

#ifndef NODICT
ClassImp(TaPairFromQuad)
#endif

// Static members
Int_t TaPairFromQuad::fgQuadCount;

TaPairFromQuad::TaPairFromQuad():VaPair()
{
}

TaPairFromQuad::TaPairFromQuad(const TaPairFromQuad& copy):VaPair(copy)
{
}

TaPairFromQuad &TaPairFromQuad::operator=(const TaPairFromQuad &assign)
{
  VaPair::operator=(assign);
  return *this;
}

TaPairFromQuad::~TaPairFromQuad()
{
}

ErrCode_t
TaPairFromQuad::RunInit(const TaRun& run)
{
  if (VaPair::RunInit(run) == fgVAP_ERROR)
    return fgVAP_ERROR;
  fgQuadCount = 5;
  return fgVAP_OK;
}

void TaPairFromQuad::CheckSequence( VaEvent& ThisEv, TaRun& run )
{
  // Look for sequence errors in the beam's window pair structure.
  // Errors include:
  //
  // Pairsynch unchanged from previous window
  // multipletsync == FirstMS when OtherMS expected
  // multipletsync == OtherMS when FirstMS expected
  // In first window of quad, helicity does not match expected value
  // In second window of quad, helicity unchanged from first window
  // In third window of quad, helicity changed from second window
  // In fourth window of quad, helicity unchanged from third window
  // In first or third window of quad, pairsynch is SecondPS
  // In second or fourth window of quad, pairsynch is FirstPS
  // In second and later events of window, pairsynch changed from first event
  // In second and later events of window, multipletsynch changed from first event
  // In second and later events of window, helicity changed from first event

  const Int_t EPSCHANGE  = 0x1;
  const Int_t WPSSAME    = 0x2;
  const Int_t EHELCHANGE = 0x3;
  const Int_t WHELSAME   = 0x4;
  const Int_t WHELWRONG  = 0x5;
  const Int_t WHELCHANGE = 0x6;
  const Int_t WMSFIRST   = 0x7;
  const Int_t WMSOTHER   = 0x8;
  const Int_t EMSCHANGE  = 0x9;
  const Int_t WMSPSWRONG = 0x10;

  Int_t val = 0;

  static UInt_t gLastTimeSlot;
  Bool_t newWin = ThisEv.GetTimeSlot() == 1 ||
    ThisEv.GetTimeSlot() <= gLastTimeSlot;
  gLastTimeSlot = ThisEv.GetTimeSlot();
  
  EPairSynch lps = ThisEv.GetPairSynch();
  EMultipletSynch lms = ThisEv.GetMultipletSynch();
  
  //    clog << "TaPairFromQuad::CheckSequence hel/ps/ms/ts="
  //         << " " << (ThisEv.GetHelicity() == RightHeli ? "R" : "L")
  //         << " " << (ThisEv.GetPairSynch() == FirstPS ? "F" : "S")
  //         << " " << (ThisEv.GetMultipletSynch() == FirstMS ? "F" : "O")
  //         << " " << ThisEv.GetTimeSlot()
  //         << endl;
  
  if (fgSequenceNo < fgNCuts)
    {
      if (newWin)
	{ 
	  // New window.
	  // Store event for comparison to later ones
	  fgLastWinEv = fgThisWinEv;
	  fgThisWinEv = ThisEv;
	  if (lms == FirstMS && fgQuadCount == 5)
	    fgQuadCount = 0;
	  else if (fgQuadCount < 5)
	    {
	      fgQuadCount = (fgQuadCount + 1) % 4;
	      if (lms == FirstMS && fgQuadCount != 0)
		{
		  cout << "TaPairFromQuad::CheckSequence ERROR: Event " 
		       << ThisEv.GetEvNumber() 
		       << " unexpected first window of quad" << endl;
		  val = WMSFIRST;
		  fgQuadCount = 0;
		}
	      else if (lms == OtherMS && fgQuadCount == 0)
		{
		  cout << "TaPairFromQuad::CheckSequence ERROR: Event " 
		       << ThisEv.GetEvNumber() 
		       << " unexpected non-first window of quad" << endl;
		  val = WMSOTHER;
		}
	    }
	  
	  if (fgQuadCount == 0)
	    // See if helicity is right
	    {
	      if (!HelSeqOK (ThisEv.GetHelicity()))
		{
		  cout << "TaPairFromQuad::CheckEvent ERROR: Event " 
		       << ThisEv.GetEvNumber() 
		       << " helicity sequence error" << endl;
		  val = WHELWRONG;
		}	      
	    } 
	  
	  if (((fgQuadCount == 0 || fgQuadCount == 2) && lps == SecondPS)
	      ||
	      ((fgQuadCount == 1 || fgQuadCount == 3) && lps == FirstPS))
	    // See if pairsynch is right
	    {
	      cout << "TaPairFromQuad::CheckEvent ERROR: Event " 
		   << ThisEv.GetEvNumber() 
		   << " pairsynch/multipletsynch mismatch" << endl;
	      val = WMSPSWRONG;
	    } 
	  
	  if ( fgLastWinEv.GetEvNumber() > 0 )
	    {
	      // Comparisons to last window
	      // See if pairsynch changed since last window
	      if ( lps == fgLastWinEv.GetPairSynch() )
		{
		  cout << "TaPairFromQuad::CheckSequence ERROR: Event " 
		       << ThisEv.GetEvNumber() 
		       << " pair synch unchanged" << endl;
		  val = WPSSAME;
		}
	      
	      if (fgQuadCount == 1 || fgQuadCount == 3)
		// See if helicity changed
		{
		  if ( ThisEv.GetHelicity() == fgLastWinEv.GetHelicity() )
		    {
		      cout << "TaPairFromQuad::CheckSequence ERROR: Event " 
			   << ThisEv.GetEvNumber() 
			   << " helicity unchanged from previous window" << endl;
		      val = WHELSAME;
		    }
		}
	      else if (fgQuadCount == 2)
		// See if helicity unchanged
		{
		  if ( ThisEv.GetHelicity() != fgLastWinEv.GetHelicity() )
		    {
		      cout << "TaPairFromQuad::CheckSequence ERROR: Event " 
			   << ThisEv.GetEvNumber() 
			   << " helicity changed from previous window" << endl;
		      val = WHELCHANGE;
		    }
		}
	    }
	}
      
      if ( !newWin && fgThisWinEv.GetEvNumber() != 0 )
	{
	  // Comparisons to last event
	  // See if pairsynch stayed the same
	  if ( lps != fgThisWinEv.GetPairSynch() )
	    {
	      cout << "TaPairFromQuad::CheckSequence ERROR: Event " 
		   << ThisEv.GetEvNumber()
		   << " pairsynch change in mid window\n";
	      val = EPSCHANGE;
	    }
	  // See if multipletsynch stayed the same
	  if ( lms != fgThisWinEv.GetMultipletSynch() )
	    {
	      cout << "TaPairFromQuad::CheckSequence ERROR: Event " 
		   << ThisEv.GetEvNumber()
		   << " multipletsynch change in mid window\n";
	      val = EMSCHANGE;
	    }
	  // See if helicity stayed the same
	  if ( ThisEv.GetHelicity() != fgThisWinEv.GetHelicity() )
	    {
	      cout << "TaPairFromQuad::CheckSequence ERROR: Event " 
		   << ThisEv.GetEvNumber()
		   << " helicity change in mid window\n";
	      val = EHELCHANGE;
	    }
	}
    }

#ifdef NOISY
  clog << "This Event  " << ThisEv.GetEvNumber()
       << " hel/ps/ms/ts " << (UInt_t)ThisEv.GetHelicity() 
       << " " << (UInt_t)ThisEv.GetPairSynch()
       << " " << (UInt_t)ThisEv.GetMultipletSynch()
       << " " << ThisEv.GetTimeSlot() << endl;
  clog << "This Window " << fgThisWinEv.GetEvNumber()
       << " hel/ps/ms/ts " << (UInt_t)fgThisWinEv.GetHelicity() 
       << " " << (UInt_t)fgThisWinEv.GetPairSynch()
       << " " << (UInt_t)fgThisWinEv.GetMultipletSynch()
       << " " << fgThisWinEv.GetTimeSlot() << endl;
  if(fgLastWinEv.GetEvNumber() != 0){
     clog << "Last Window " << fgLastWinEv.GetEvNumber()
          << " hel/ps/ms/ts " << (UInt_t)fgLastWinEv.GetHelicity() 
          << " " << (UInt_t)fgLastWinEv.GetPairSynch()
          << " " << (UInt_t)fgLastWinEv.GetMultipletSynch()
          << " " << fgLastWinEv.GetTimeSlot() << endl;
  }
#endif

  if (fgSequenceNo < fgNCuts)
    {
      ThisEv.AddCut (fgSequenceNo, val);
      run.UpdateCutList (fgSequenceNo, val, ThisEv.GetEvNumber());
    }
}


