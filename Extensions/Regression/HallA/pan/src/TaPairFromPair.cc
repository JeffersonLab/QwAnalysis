//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaPairFromPair.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaPairFromPair.cc,v 1.19 2009/08/11 20:18:29 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Class which makes and analyzes opposite helicity event pairs
//    from a data stream structured as helicity window pairs.  Derived
//    from VaPair.
//
//////////////////////////////////////////////////////////////////////////

//#define NOISY

#include "VaEvent.hh"
#include "TaPairFromPair.hh"
#include "TaRun.hh"

#ifndef NODICT
ClassImp(TaPairFromPair)
#endif

TaPairFromPair::TaPairFromPair():VaPair()
{
}

TaPairFromPair::TaPairFromPair(const TaPairFromPair& copy):VaPair(copy)
{
}

TaPairFromPair &TaPairFromPair::operator=(const TaPairFromPair &assign)
{
  VaPair::operator=(assign);
  return *this;
}

TaPairFromPair::~TaPairFromPair()
{
}

void TaPairFromPair::CheckSequence( VaEvent& ThisEv, TaRun& run )
{
  // Look for sequence errors in the beam's window pair structure.
  // Errors include:
  //
  // Pairsynch unchanged from previous window
  // In first window of pair, helicity does not match expected value
  // In second window of pair, helicity unchanged from first window
  // In second and later events of window, pairsynch changed from first event
  // In second and later events of window, helicity changed from first event

  const Int_t EPSCHANGE  = 0x1;
  const Int_t WPSSAME    = 0x2;
  const Int_t EHELCHANGE = 0x3;
  const Int_t WHELSAME   = 0x4;
  const Int_t WHELWRONG  = 0x5;

  Int_t val = 0;

  static UInt_t gLastTimeSlot;
  Bool_t newWin = ThisEv.GetTimeSlot() == 1 ||
    ThisEv.GetTimeSlot() <= gLastTimeSlot;
  gLastTimeSlot = ThisEv.GetTimeSlot();
  
  EPairSynch lps = ThisEv.GetPairSynch();
  
  //    clog << "TaPairFromPair::CheckSequence hel/ps/ts="
  //         << " " << (ThisEv.GetHelicity() == RightHeli ? "R" : "L")
  //         << " " << (ThisEv.GetPairSynch() == FirstPS ? "F" : "S")
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
	  
	  if ( fgLastWinEv.GetEvNumber() > 0 )
	    {
	      // Comparisons to last window
	      // See if pairsynch changed since last window
	      if ( lps == fgLastWinEv.GetPairSynch() )
		{
		  cout << "TaPairFromPair::CheckSequence ERROR: Event " 
		       << ThisEv.GetEvNumber() 
		       << " pair synch unchanged" << endl;
		  val = WPSSAME;
		}
	      
	      if (lps == FirstPS)
		// See if helicity is right
		{
		  if (!HelSeqOK (ThisEv.GetHelicity()))
		    {
		      cout << "TaPairFromPair::CheckEvent ERROR: Event " 
			   << ThisEv.GetEvNumber() 
			   << " helicity sequence error" << endl;
		      val = WHELWRONG;
		    }	      
		} 
	      else 
		// See if helicity changed
		{
		  if ( ThisEv.GetHelicity() == fgLastWinEv.GetHelicity() )
		    {
		      cout << "TaPairFromPair::CheckSequence ERROR: Event " 
			   << ThisEv.GetEvNumber() 
			   << " helicity unchanged from previous window" << endl;
		      val = WHELSAME;
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
	      cout << "TaPairFromPair::CheckSequence ERROR: Event " 
		   << ThisEv.GetEvNumber()
		   << " pairsynch change in mid window\n";
	      val = EPSCHANGE;
	    }
	  // See if helicity stayed the same
	  if ( ThisEv.GetHelicity() != fgThisWinEv.GetHelicity() )
	    {
	      cout << "TaPairFromPair::CheckSequence ERROR: Event " 
		   << ThisEv.GetEvNumber()
		   << " helicity change in mid window\n";
	      val = EHELCHANGE;
	    }
	}
    }

#ifdef NOISY
  clog << "This Event  " << ThisEv.GetEvNumber()
       << " hel/ps/ts " << (UInt_t)ThisEv.GetHelicity() 
       << " " << (UInt_t)ThisEv.GetPairSynch()
       << " " << ThisEv.GetTimeSlot() << endl;
  clog << "This Window " << fgThisWinEv.GetEvNumber()
       << " hel/ps/ts " << (UInt_t)fgThisWinEv.GetHelicity() 
       << " " << (UInt_t)fgThisWinEv.GetPairSynch()
       << " " << fgThisWinEv.GetTimeSlot() << endl;
  if(fgLastWinEv.GetEvNumber() != 0){
     clog << "Last Window " << fgLastWinEv.GetEvNumber()
          << " hel/ps/ts " << (UInt_t)fgLastWinEv.GetHelicity() 
          << " " << (UInt_t)fgLastWinEv.GetPairSynch()
          << " " << fgLastWinEv.GetTimeSlot() << endl;
  }
#endif

  if (fgSequenceNo < fgNCuts)
    {
      ThisEv.AddCut (fgSequenceNo, val);
      run.UpdateCutList (fgSequenceNo, val, ThisEv.GetEvNumber());
    }
}


