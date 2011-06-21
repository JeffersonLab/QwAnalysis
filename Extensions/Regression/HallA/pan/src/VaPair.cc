//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       VaPair.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: VaPair.cc,v 1.32 2009/11/03 21:16:07 deng Exp $
//
////////////////////////////////////////////////////////////////////////
//
// Base class for pairs of events of opposite helicities.  Contains
// (when full) two events, as well as the results of analysis of that
// pair. Different derived classes correspond to different beam
// helicity structures. Methods are provided to compute differences and
// asymmetries for a given device.
//
////////////////////////////////////////////////////////////////////////


#include "TaCutList.hh"
#include "VaEvent.hh"
#include "TaLabelledQuantity.hh"
#include "VaPair.hh" 
#include "TaRun.hh"
#include "TaString.hh"
#include "TaDataBase.hh"

//#define NOISY

#ifndef NODICT
ClassImp(VaPair)
#endif

// Static members
const ErrCode_t VaPair::fgVAP_OK = 0;
const ErrCode_t VaPair::fgVAP_ERROR = -1;
deque< VaEvent > 
VaPair::fgEventQueue;
Bool_t VaPair::fgSkipping = true;
VaEvent VaPair::fgThisWinEv;
VaEvent VaPair::fgLastWinEv;
UInt_t VaPair::fgShreg = 1;
UInt_t VaPair::fgNShreg = 0;
Bool_t VaPair::fgPairMade = false;
UInt_t VaPair::fgNCuts;
Cut_t VaPair::fgSequenceNo;
UInt_t VaPair::fgOldb = 2;
Bool_t VaPair::fgRandom = 1;
UInt_t VaPair::fgRanType = 1;

VaPair::VaPair() :
  fEvFirst(0),
  fEvSecond(0)
{
}


VaPair::VaPair(const VaPair& copy)
{
  fResults = copy.fResults;
  fEvLeft = copy.fEvLeft;
  fEvRight = copy.fEvRight;
  if (copy.fEvFirst == &(copy.fEvLeft))
    {
      fEvFirst = &fEvLeft;
      fEvSecond = &fEvRight;
    }
  else if (copy.fEvFirst == &(copy.fEvRight))
    {
      fEvFirst = &fEvRight;
      fEvSecond = &fEvLeft;
    }
  else
    {
      fEvFirst = 0;
      fEvSecond = 0;
    }
}


VaPair &
VaPair::operator=(const VaPair &assign)
{
  if (this != &assign)
    { 
      fResults = assign.fResults;
      fEvLeft = assign.fEvLeft;
      fEvRight = assign.fEvRight;
      if (assign.fEvFirst == &(assign.fEvLeft))
	{
	  fEvFirst = &fEvLeft;
	  fEvSecond = &fEvRight;
	}
      else if (assign.fEvFirst == &(assign.fEvRight))
	{
	  fEvFirst = &fEvRight;
	  fEvSecond = &fEvLeft;
	}
      else
	{
	  fEvFirst = 0;
	  fEvSecond = 0;
	}
    } 
  return *this;
}


VaPair::~VaPair()
{
}


ErrCode_t
VaPair::RunInit(const TaRun& run)
{
  // Initialization at start of run -- clear event queue (so we don't
  // pair events from two different runs, for instance)
  fgEventQueue.clear();
  fgSkipping = true;
  fgThisWinEv = VaEvent();
  fgLastWinEv = VaEvent();
  fgShreg = 1;
  fgNShreg = 0;
  fgPairMade = false;

  fgNCuts = (UInt_t) run.GetDataBase().GetNumCuts();
  fgSequenceNo = run.GetDataBase().GetCutNumber("Pair_seq");
  if (fgSequenceNo == fgNCuts)
    fgSequenceNo = run.GetDataBase().GetCutNumber("Sequence"); // backward compat
  if (fgSequenceNo == fgNCuts)
    {
      cerr << "VaPair::RunInit WARNING: Following cut(s) are not defined "
	   << "in database and are required:";
      if (fgSequenceNo == fgNCuts) cerr << " Pair_seq";
      cerr << endl;
      return fgVAP_ERROR;
    }

  // Since there are no non-required pair cuts the following code is
  // commented out (but left for possible future use)

  //   if (fgSequenceNo == fgNCuts)
  //     {
  //       cerr << "VaPair::RunInit WARNING: Following cut(s) are not defined "
  // 	   << "in database and will not be imposed:";
  //       if (fgSequenceNo == fgNCuts) cerr << " Pair_seq";
  //       cerr << endl;
  //     }
  
  // If randomheli is "no" in database set fgRandom to 0, else 1.
  TaString sran = run.GetDataBase().GetRandomHeli();
  fgRandom = sran.CmpNoCase ("no") == 0 ? 0 : 1;
  
  if (fUseRanBitNew30) 
    fgRanType = 3;
  else
    fgRanType = sran.CmpNoCase ("old") == 0 ? 0 
    : sran.CmpNoCase ("30bit") == 0 ? 2 : 1;

  return fgVAP_OK;
}


Bool_t 
VaPair::Fill( VaEvent& ThisEv, TaRun& run )
{
  // If this event makes a pair with a stored one, put the two events
  // into this pair and return true.  Otherwise store this event and
  // return false.

  Bool_t PairMade = false;
  CheckSequence (ThisEv, run);

  // Here we can use conditionals to alter the pairing.
  // Define TOGPS and undefine SUBSAMPLE to pair adjacent events
  // Define TOGPS and define SUBSAMPLE=n to pair events at distance n
  // Undefine TOGPS and undefine SUBSAMPLE to pair adjacent windows (normal)
  // Undefine TOGPS and define SUBSAMPLE=n to pair windows at distance n
  // Note that for non oversampling data, TOGPS has no effect (except that
  // there is a 50-50 chance of mixing up first and second events of pairs
  // if TOGPS is defined)

  //#define TOGPS
  //#define SUBSAMPLE 2

  static EPairSynch thisps0 = SecondPS;  // 'pairsynch' used to make subharmonics
  static EPairSynch thisps = SecondPS;   // subharmonic

#ifdef TOGPS
  // Replace real pairsynch (after sequence checking) with a toggle.
  // Note that it toggles on EVENTS, not WINDOWS.
  thisps0 = thisps0 == FirstPS ? SecondPS : FirstPS;;
#else
  thisps0 = ThisEv.GetPairSynch();
#endif

#ifdef SUBSAMPLE
  // Set thisps to pair events at greater than normal distance by
  // toggling it once for every n changes in thisps0.

  static EPairSynch lastps0 = SecondPS;
  static UInt_t nsub = SUBSAMPLE-1;
//   clog << "lastps0 = " << (lastps0==FirstPS?"F":"S")
//        << " nsub = " << nsub
//        << "thisps0 = " << (thisps0==FirstPS?"F":"S");
  if (thisps0 != lastps0)
    if (++nsub == SUBSAMPLE)
      {
	thisps = thisps == FirstPS ? SecondPS : FirstPS;
	nsub = 0;
      }
//   clog << " thisps = " << (thisps==FirstPS?"F":"S");

  lastps0 = thisps0;
  fgSkipping = false;
#else
  thisps = thisps0;

#ifdef TOGPS
  fgSkipping = false;
#else
  // Skip events until the first event of a new window pair
  if ( thisps == FirstPS &&
       ThisEv.GetTimeSlot() == 1 )
    fgSkipping = false;
#endif

#endif

  if ( !fgSkipping )
    {
#ifdef NOISY
    clog << "Pairing event "  << ThisEv.GetEvNumber() << ": ";
#endif
      // If first of a pair, store it
      if ( thisps == FirstPS )
	{
	  if (fgPairMade && fgEventQueue.size() > 0)
	    {
	      // If event queue isn't empty, something is wrong: we
	      // didn't pair off all first events with second events
	      // before another first event came along.
	      cerr << "VaPair::Fill ERROR: Nothing to pair first event "
		   << fgEventQueue[0].GetEvNumber() << " with\n";
	      fgEventQueue.clear();
	      if (ThisEv.GetTimeSlot() == 1)
		fgEventQueue.push_back(ThisEv);
	      else
		fgSkipping = true;
	    }
	  else
	    {
	      fgEventQueue.push_back(ThisEv);
#ifdef NOISY
	      clog << " storing" << endl;
#endif
	    }
	}
      else
	{
	  // If second of a pair, get its partner and build the pair
	  if (fgEventQueue.size() > 0)
	    {
#ifdef NOISY
	      clog << "pairing with event " << fgEventQueue[0].GetEvNumber() << endl;
#endif
	      if (fgEventQueue[0].GetHelicity() == RightHeli)
		{
		  fEvRight = fgEventQueue[0];
		  fEvLeft = ThisEv;
		  fEvFirst = &fEvRight;
		  fEvSecond = &fEvLeft;
		}
	      else
		{
		  fEvRight = ThisEv;
		  fEvLeft = fgEventQueue[0];
		  fEvFirst = &fEvLeft;
		  fEvSecond = &fEvRight;
		}
	      fgEventQueue.pop_front();
	      PairMade = true;
	    }
	  else
	    {
	      // Something's wrong.  This is a second event but the
	      // queue of first events is empty.
	      cerr << "VaPair::Fill ERROR: Nothing to pair second event "
		   << ThisEv.GetEvNumber() << " with\n";
	      fgSkipping = true;
	    }
	}
    }
#ifdef NOISY
  else
    clog << "Skipping event " << ThisEv.GetEvNumber() << endl;
#endif

  fgPairMade = PairMade;
  return PairMade;
}


const VaEvent& 
VaPair::GetRight() const
{
  return fEvRight;
}


const VaEvent& 
VaPair::GetLeft() const
{
  return fEvLeft;
}


const VaEvent& 
VaPair::GetFirst() const
{
  return *fEvFirst;
}


const VaEvent& 
VaPair::GetSecond() const
{
  return *fEvSecond;
}


void 
VaPair::QueuePrint() const
{
  // Print stored events, for diagnostic purposes

  if ( fgEventQueue.size() !=0 )
    {
      cout<<" ----- PAIR event queue print out -------- "<<endl;
      for ( deque< VaEvent >::iterator fEvIdx  = fgEventQueue.begin() ; 
            fEvIdx != fgEventQueue.end() ; 
            fEvIdx++ )
        cout<<"event number "<< (*fEvIdx).GetEvNumber() << endl;
      cout<<" ----------------------------------------- " <<endl;
    }
  else cout<< " Pair Event ring empty, no print out "<<endl; 
}


void 
VaPair::AddResult (const TaLabelledQuantity& lq)
{
  // Insert pair analysis results into pair.
  fResults.push_back(lq);
}


Double_t 
VaPair::GetDiff (Int_t key) const
{
  // Get difference in quantity indexed by key for this pair.
  return GetRight().GetData(key) - GetLeft().GetData(key);
}


Double_t 
VaPair::GetDiffSum (vector<Int_t> keys, vector<Double_t> wts) const
{
  // Get difference in weighted sum of quantities indexed by keys for
  // this pair.

  return GetRight().GetDataSum (keys, wts) - GetLeft().GetDataSum (keys, wts);
}


Double_t 
VaPair::GetAvg (Int_t key) const
{
  // Get average quantity indexed by key for this pair.
  return (GetRight().GetData(key) + GetLeft().GetData(key))/2.0;
}


Double_t 
VaPair::GetAvgSum (vector<Int_t> keys, vector<Double_t> wts) const
{
  // Get average weighted sum of quantities indexed by keys for
  // this pair.

  return (GetRight().GetDataSum (keys, wts) + 
	  GetLeft().GetDataSum (keys, wts))/2.0;
}

Double_t 
VaPair::GetAvgN (Int_t key, Int_t curmonkey) const
{
  // Get normalized average in quantity indexed by key for this pair.

  return ((GetRight().GetData(key)/GetRight().GetData(curmonkey))
	  + (GetLeft().GetData(key)/GetLeft().GetData(curmonkey))) / 2.0;
}


Double_t 
VaPair::GetAvgNSum (vector<Int_t> keys, Int_t curmonkey, vector<Double_t> wts) const
{
  // Get normalized average weighted sum of quantities indexed by keys for
  // this pair.

  return (((GetRight().GetDataSum (keys, wts))/GetRight().GetData(curmonkey)) 
	  + ((GetLeft().GetDataSum (keys, wts))/GetLeft().GetData(curmonkey)))
    /2.0;

}


Double_t 
VaPair::GetAsy (Int_t key) const
{
  // Get asymmetry in quantity indexed by key for this pair.

  Double_t denom = GetRight().GetData(key) + GetLeft().GetData(key);
  if ( denom <= 0 )
    {
//        cerr << "VaPair::GetAsy ERROR: Denominator is <= zero, key = " 
//  	   << key << endl;
      return -1;
    }
  return (GetRight().GetData(key) - GetLeft().GetData(key)) / denom;
}

Double_t 
VaPair::GetAsyN (Int_t key, Int_t curmonkey) const
{
  // Get normalized asymmetry in quantity indexed by key for this pair.

  Double_t denom = GetRight().GetData(key)/GetRight().GetData(curmonkey)
    + GetLeft().GetData(key)/GetLeft().GetData(curmonkey);
  if ( denom <= 0 )
    {
//        cerr << "VaPair::GetAsyN ERROR: Denominator is <= zero, key = " 
//  	   << key << endl;
      return -1;
    }
  return (GetRight().GetData(key)/GetRight().GetData(curmonkey)
	  - GetLeft().GetData(key)/GetLeft().GetData(curmonkey)) / denom;
}


Double_t 
VaPair::GetAsySum (vector<Int_t> keys, vector<Double_t> wts) const
{
  // Get asymmetry in weighted sum of quantities indexed by keys for
  // this pair.

  Double_t denom = GetRight().GetDataSum (keys, wts) + 
    GetLeft().GetDataSum (keys, wts);
  if ( denom <= 0 )
    {
//       cerr << "VaPair::GetAsySum ERROR: Denominator is <= zero" << endl;
      return -1;
    }
  return (GetRight().GetDataSum (keys, wts) - 
	  GetLeft().GetDataSum (keys, wts)) / denom;
}


Double_t 
VaPair::GetAsyNSum (vector<Int_t> keys, Int_t curmonkey, vector<Double_t> wts) const
{
  // Get normalized asymmetry in weighted sum of quantities indexed by keys for
  // this pair.

  Double_t denom = (GetRight().GetDataSum (keys, wts))/GetRight().GetData(curmonkey) + 
    (GetLeft().GetDataSum (keys, wts))/GetLeft().GetData(curmonkey);
  if ( denom <= 0 )
    {
      //      cerr << "VaPair::GetAsyNSum ERROR: Denominator is <= zero" << endl;
      return -1;
    }
  Double_t numer = ((GetRight().GetDataSum (keys, wts))/GetRight().GetData(curmonkey)) - 
		    ((GetLeft().GetDataSum (keys, wts))/GetLeft().GetData(curmonkey));

  return  numer/denom;
}


Double_t 
VaPair::GetAsyAve (vector<Int_t> keys, vector<Double_t> wts) const
{
  // Get weighted average of asymmetries of quantities indexed by keys
  // for this pair.

  Double_t sumxw = 0;
  Double_t sumw = 0;

  if (wts.size() == 0)
    for (vector<Int_t>::const_iterator p = keys.begin();
	 p != keys.end();
	 ++p)
      {
	sumxw += GetAsy (*p);
	sumw++;
      }
  else if (wts.size() != keys.size())
    cerr << "VaPair::GetAsyAve ERROR: Weight and key vector sizes differ" << endl;
  else
    for (size_t i = 0; i < keys.size(); ++i)
      {
	sumxw += wts[i] * GetAsy (keys[i]);
	sumw += wts[i];
      }

  if (sumw <= 0)
    {
      cerr << "VaPair::GetAsyAve ERROR: Weight sum non-positive" << endl;
      return 0;
    }

  return sumxw / sumw;
}

Double_t 
VaPair::GetAsyNAve (vector<Int_t> keys, Int_t curmonkey, vector<Double_t> wts) const
{
  // Get weighted average of normalized asymmetries of quantities
  // indexed by keys for this pair.

  Double_t sumxw = 0;
  Double_t sumw = 0;

  if (wts.size() == 0)
    for (vector<Int_t>::const_iterator p = keys.begin();
	 p != keys.end();
	 ++p)
      {
	sumxw += GetAsyN (*p,curmonkey);
	sumw++;
      }
  else if (wts.size() != keys.size())
    cerr << "VaPair::GetAsyNAve ERROR: Weight and key vector sizes differ" << endl;
  else
    for (size_t i = 0; i < keys.size(); ++i)
      {
	sumxw += wts[i] * GetAsyN (keys[i],curmonkey);
	sumw += wts[i];
      }

  if (sumw <= 0)
    {
      cerr << "VaPair::GetAsyNAve ERROR: Weight sum non-positive" << endl;
      return 0;
    }
  return sumxw / sumw;
}


Bool_t 
VaPair::PassedCuts() const
{
// True if neither event has cut condition

  return ! ( GetLeft().CutStatus() || GetRight().CutStatus() );
}

Bool_t 
VaPair::PassedCutsInt(const TaCutList& cl) const
{
// True if neither event is in cut interval

  return ( cl.OK(GetLeft()) && cl.OK(GetRight()) );
}

Bool_t 
VaPair::PassedCCutsInt(const TaCutList& cl) const
{
// True if neither event is in cut interval (for hall C)

  return ( cl.OKC(GetLeft()) && cl.OKC(GetRight()) );
}

Bool_t 
VaPair::BeamCut () const
{
  // True if either event has beam cut
  return GetRight().BeamCut() || GetLeft().BeamCut();
}

Bool_t 
VaPair::BeamCCut () const
{
  // True if either event has Hall C beam cut
  return GetRight().BeamCCut() || GetLeft().BeamCCut();
}



const vector<TaLabelledQuantity>& 
VaPair::GetResults() const
{
  return fResults;
}


// Private member functions

Bool_t
VaPair::HelSeqOK (EHelicity h)
{
  // Return true iff helicity h matches what we expect to find next.  
  
  // Get this helicity bit (or 2 if unknown)
  UInt_t hb = ( h == UnkHeli ? 2 :
		( h == RightHeli ? 0 : 1 ) );

  // Get expected helicity bit (or 2 if unknown)
  UInt_t eb;
  if (fgRandom)
    eb = RanBit (hb);
  else
    {
      eb = fgOldb;
      fgOldb = hb;
    }

  Bool_t expectOK = (fgNShreg++ > 30);

  if ( expectOK && hb != 2 && hb != eb )
    {
      // Not the expected value, reset count
	fgNShreg = 0;
    }

#ifdef NOISY
  clog << "Helicity expected/got = " << eb << " " << hb;
  if (fgRandom)
    clog << " | " << fgShreg;
  clog << " fgNShreg = " << fgNShreg;
  if ( eb == 2 || eb != hb )
    clog << " (MISMATCH)";
  clog  << endl;
#endif

  // Generate error if expected is known and does not match found

  return ( !expectOK || (eb == 2 || eb == hb ));
}

UInt_t 
VaPair::RanBit (UInt_t hRead)
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
    case 3:
      ret = RanBitNew30 (hRead);
      break;
    }
  return ret;
}

UInt_t 
VaPair::RanBitOld (UInt_t hRead)
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
VaPair::RanBit24 (UInt_t hRead)
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
VaPair::RanBit30 (UInt_t hRead)
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


UInt_t
VaPair::RanBitNew30 (UInt_t hRead)
{
  
  UInt_t bit7    = (fgShreg & 0x00000040) != 0;
  UInt_t bit28   = (fgShreg & 0x08000000) != 0;
  UInt_t bit29   = (fgShreg & 0x10000000) != 0;
  UInt_t bit30   = (fgShreg & 0x20000000) != 0;

  UInt_t newbit = (bit30 ^ bit29 ^ bit28 ^ bit7) & 0x1;
  fgShreg = ( (hRead == 2 ? newbit : hRead) | (fgShreg << 1 )) & 0x3FFFFFFF;
  return newbit; 

}
