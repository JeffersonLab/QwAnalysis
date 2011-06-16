//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       TaMultiplet.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaMultiplet.cc,v 1.6 2010/04/30 14:52:15 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
// Class for groups of two or more pairs of events.
//
////////////////////////////////////////////////////////////////////////


#include "TaCutList.hh"
#include "VaPair.hh"
#include "TaLabelledQuantity.hh"
#include "TaMultiplet.hh" 
#include "TaRun.hh"
#include "TaString.hh"
#include "TaDataBase.hh"

//#define NOISY

#ifndef NODICT
ClassImp(TaMultiplet)
#endif

// Static members
const ErrCode_t TaMultiplet::fgTAM_OK = 0;
const ErrCode_t TaMultiplet::fgTAM_ERROR = -1;
vector < deque< VaPair > > TaMultiplet::fgPairQueue;
Bool_t TaMultiplet::fgSkipping = true;
VaPair TaMultiplet::fgThisWinPair;
VaPair TaMultiplet::fgLastWinPair;
Bool_t TaMultiplet::fgMultipletMade = false;
UInt_t TaMultiplet::fgNCuts;

TaMultiplet::TaMultiplet (const UInt_t n)
  : fN (n),
    fFull (false)
{
}


TaMultiplet::TaMultiplet(const TaMultiplet& copy)
{
  fN = copy.fN;
  fFull = copy.fFull;
  fResults = copy.fResults;
  fPairs = copy.fPairs;
}


TaMultiplet &
TaMultiplet::operator=(const TaMultiplet &assign)
{
  if (this != &assign)
    { 
      fN = assign.fN;
      fFull = assign.fFull;
      fResults = assign.fResults;
      fPairs = assign.fPairs;
    } 
  return *this;
}


TaMultiplet::~TaMultiplet()
{
}


ErrCode_t
TaMultiplet::RunInit(const TaRun& run)
{
  // Initialization at start of run -- clear pair queue (so we don't
  // multiplet pairs from two different runs, for instance)
  fgPairQueue.clear();
  fgPairQueue.resize (fN-1);
  fgSkipping = true;
  fgThisWinPair = VaPair();
  fgLastWinPair = VaPair();
  fgMultipletMade = false;

  fgNCuts = (UInt_t) run.GetDataBase().GetNumCuts();

  return fgTAM_OK;
}


Bool_t 
TaMultiplet::Fill( VaPair& ThisPair)
{
  // If this pair completes a multiplet with stored ones, put the pairs
  // into this multiplet and return true.  Otherwise store this pair and
  // return false.

  Bool_t MultipletMade = false;

  EMultipletSynch thisms = ThisPair.GetFirst().GetMultipletSynch();
  static UInt_t multn = 0;  // position within multiplet

  // Skip pairs until the first pair of a new window multiplet
  if ( thisms == FirstMS &&
       ThisPair.GetFirst().GetTimeSlot() == 1 )
    fgSkipping = false;

  if ( !fgSkipping )
    {
#ifdef NOISY
    clog << "Multipleting pair "  << ThisPair.GetFirst().GetEvNumber() << ", " << ThisPair.GetSecond().GetEvNumber() << ": ";
#endif
      // If first of a multiplet, store it
      if (thisms == FirstMS)
	{
	  multn = 0;
	  Bool_t bad = false;
	  for (UInt_t i = 0; i < fN-1; ++i)
	    {
	      if (fgMultipletMade && fgPairQueue[i].size() > 0)
		{
		  // If pair queue isn't empty, something is wrong: we
		  // didn't multiplet off all non-last pairs with last pairs
		  // before another first pair came along.
		  cerr << "TaMultiplet::Fill ERROR: Nothing to multiplet pair "
		       << fgPairQueue[i][0].GetFirst().GetEvNumber() << ", "
		       << fgPairQueue[i][0].GetSecond().GetEvNumber() << " with\n";
		  bad = true;
		}
	    }
	  if (bad)
	    {
	      fgPairQueue.clear();
	      fgPairQueue.resize (fN-1);
	      if (ThisPair.GetFirst().GetTimeSlot() == 1)
		fgPairQueue[0].push_back(ThisPair);
	      else
		fgSkipping = true;
	    }
	  else
	    {
	      fgPairQueue[0].push_back(ThisPair);
#ifdef NOISY
	      clog << " storing" << endl;
#endif
	    }
	}
      else 
	{
	  if (multn < fN-1)
	    {
	      // If non last of a multiplet, store it
	      if (fgPairQueue[multn].size() >= fgPairQueue[0].size())
		++ multn;
	    }
	  if (multn < fN-1)
	    {
	      fgPairQueue[multn].push_back(ThisPair);
#ifdef NOISY
	      clog << " storing" << endl;
#endif
	    }
	  else
	    {
	      // If last of a multiplet, get its partners and build the multiplet
	      Bool_t bad = false;
	      for (UInt_t i = 0; i < fN-1; ++i)
		{
		  if (fgMultipletMade && fgPairQueue[i].size() == 0)
		    bad = true;
		}
	      if (!bad)
		{
		  fPairs.clear();
		  fResults.clear();
		  for (UInt_t i = 0; i < fN-1; ++i)
		    {
#ifdef NOISY
		      clog << "multipleting with pair " << fgPairQueue[i].GetFirst().GetEvNumber()
			   << ", " << fgPairQueue[i].GetSecond().GetEvNumber() << endl;
#endif
		      fPairs.push_back (fgPairQueue[i][0]);
		      fgPairQueue[i].pop_front();
		    }
		  fPairs.push_back (ThisPair);
		  MultipletMade = true;
		}
	      else
		{
		  // Something's wrong.  This is a last pair but one of the
		  // queues of non last pairs is empty.
		  cerr << "TaMultiplet::Fill ERROR: Nothing to multiplet last pair "
		       << ThisPair.GetFirst().GetEvNumber() << ", " << ThisPair.GetSecond().GetEvNumber() << " with\n";
		  fgSkipping = true;
		}
	    }
	}
    }
#ifdef NOISY
  else
    clog << "Skipping pair " << ThisPair.GetFirst().GetEvNumber() << ", " << ThisPair.GetSecond().GetEvNumber() << endl;
#endif

  fgMultipletMade = MultipletMade;
  fFull = MultipletMade;
  return MultipletMade;
}


const VaPair& 
TaMultiplet::GetPair (const UInt_t n) const
{
  return n < fN ? fPairs[n] : fPairs[0];
}

void 
TaMultiplet::AddResult (const TaLabelledQuantity& lq)
{
  // Insert multiplet analysis results into multiplet.
  fResults.push_back (lq);
}

Double_t
TaMultiplet::GetRightSum (Int_t key) const
{
  // Get sum over pairs of quantity indexed by key for right events in
  // this multiplet.
  Double_t sum = 0;
  for (vector<VaPair>::const_iterator pairItr  = fPairs.begin(); 
       pairItr != fPairs.end(); 
       pairItr++)
    sum += pairItr->GetRight().GetData (key);
  return sum;
}

Double_t
TaMultiplet::GetLeftSum (Int_t key) const
{
  // Get sum over pairs of quantity indexed by key for left events in
  // this multiplet.
  Double_t sum = 0;
  for (vector<VaPair>::const_iterator pairItr  = fPairs.begin(); 
       pairItr != fPairs.end(); 
       pairItr++)
    sum += pairItr->GetLeft().GetData (key);
  return sum;
}

Double_t
TaMultiplet::GetRightSumSum (vector<Int_t> keys, vector<Double_t> wts) const
{
  // Get sum over pairs of weighted sum of quantities indexed by keys
  // for right events in this multiplet.
  Double_t sum = 0;
  for (vector<VaPair>::const_iterator pairItr  = fPairs.begin(); 
       pairItr != fPairs.end(); 
       pairItr++)
    sum += pairItr->GetRight().GetDataSum (keys, wts);
  return sum;
}

Double_t
TaMultiplet::GetLeftSumSum (vector<Int_t> keys, vector<Double_t> wts) const
{
  // Get sum over pairs of weighted sum of quantities indexed by keys
  // for left events in this multiplet.
  Double_t sum = 0;
  for (vector<VaPair>::const_iterator pairItr  = fPairs.begin(); 
       pairItr != fPairs.end(); 
       pairItr++)
    sum += pairItr->GetLeft().GetDataSum (keys, wts);
  return sum;
}

Double_t 
TaMultiplet::GetDiff (Int_t key) const
{
  // Get difference in quantity indexed by key for this multiplet.
  return (GetRightSum(key) - GetLeftSum(key)) / fN;
}


Double_t 
TaMultiplet::GetDiffSum (vector<Int_t> keys, vector<Double_t> wts) const
{
  // Get difference in weighted sum of quantities indexed by keys for
  // this multiplet.

  return (GetRightSumSum (keys, wts) - GetLeftSumSum (keys, wts)) / fN;
}


Double_t 
TaMultiplet::GetAvg (Int_t key) const
{
  // Get average quantity indexed by key for this multiplet.
  return (GetRightSum (key) + GetLeftSum (key)) / 2.0 / fN;
}


Double_t 
TaMultiplet::GetAvgSum (vector<Int_t> keys, vector<Double_t> wts) const
{
  // Get average weighted sum of quantities indexed by keys for
  // this multiplet.

  return (GetRightSumSum (keys, wts) + 
	  GetLeftSumSum (keys, wts)) / 2.0 / fN;
}

Double_t 
TaMultiplet::GetAvgN (Int_t key, Int_t curmonkey) const
{
  // Get normalized average in quantity indexed by key for this multiplet.

  return ((GetRightSum (key) / GetRightSum (curmonkey))
	  + (GetLeftSum (key) / GetLeftSum (curmonkey))) / 2.0 / fN;
}


Double_t 
TaMultiplet::GetAvgNSum (vector<Int_t> keys, Int_t curmonkey, vector<Double_t> wts) const
{
  // Get normalized average weighted sum of quantities indexed by keys for
  // this multiplet.

  return (((GetRightSumSum (keys, wts)) / GetRightSum (curmonkey)) 
	  + ((GetLeftSumSum (keys, wts)) / GetLeftSum (curmonkey))) / 2.0 / fN;
}


Double_t 
TaMultiplet::GetAsy (Int_t key) const
{
  // Get asymmetry in quantity indexed by key for this multiplet.

  Double_t denom = GetRightSum (key) + GetLeftSum (key);
  if (denom <= 0)
    return -1;
  return (GetRightSum (key) - GetLeftSum (key)) / denom;
}

Double_t 
TaMultiplet::GetAsyN (Int_t key, Int_t curmonkey) const
{
  // Get normalized asymmetry in quantity indexed by key for this multiplet.

  Double_t denom = GetRightSum (key) / GetRightSum (curmonkey)
    + GetLeftSum (key) / GetLeftSum (curmonkey);
  if (denom <= 0)
    return -1;
  return (GetRightSum (key) / GetRightSum (curmonkey)
	  - GetLeftSum (key) / GetLeftSum (curmonkey)) / denom;
}


Double_t 
TaMultiplet::GetAsySum (vector<Int_t> keys, vector<Double_t> wts) const
{
  // Get asymmetry in weighted sum of quantities indexed by keys for
  // this multiplet.

  Double_t denom = GetRightSumSum (keys, wts) + GetLeftSumSum (keys, wts);
  if (denom <= 0)
    return -1;
  return (GetRightSumSum (keys, wts) - GetLeftSumSum (keys, wts)) / denom;
}


Double_t 
TaMultiplet::GetAsyNSum (vector<Int_t> keys, Int_t curmonkey, vector<Double_t> wts) const
{
  // Get normalized asymmetry in weighted sum of quantities indexed by keys for
  // this multiplet.

  Double_t denom = (GetRightSumSum (keys, wts)) / GetRightSum (curmonkey) + 
    (GetLeftSumSum (keys, wts)) / GetLeftSum (curmonkey);
  if (denom <= 0)
    return -1;
  Double_t numer = ((GetRightSumSum (keys, wts)) / GetRightSum (curmonkey)) - 
		    ((GetLeftSumSum (keys, wts)) / GetLeftSum (curmonkey));

  return  numer/denom;
}


Double_t 
TaMultiplet::GetAsyAve (vector<Int_t> keys, vector<Double_t> wts) const
{
  // Get weighted average of asymmetries of quantities indexed by keys
  // for this multiplet.

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
    cerr << "TaMultiplet::GetAsyAve ERROR: Weight and key vector sizes differ" << endl;
  else
    for (size_t i = 0; i < keys.size(); ++i)
      {
	sumxw += wts[i] * GetAsy (keys[i]);
	sumw += wts[i];
      }

  if (sumw <= 0)
    {
      cerr << "TaMultiplet::GetAsyAve ERROR: Weight sum non-positive" << endl;
      return 0;
    }

  return sumxw / sumw;
}

Double_t 
TaMultiplet::GetAsyNAve (vector<Int_t> keys, Int_t curmonkey, vector<Double_t> wts) const
{
  // Get weighted average of normalized asymmetries of quantities
  // indexed by keys for this multiplet.

  Double_t sumxw = 0;
  Double_t sumw = 0;

  if (wts.size() == 0)
    for (vector<Int_t>::const_iterator p = keys.begin();
	 p != keys.end();
	 ++p)
      {
	sumxw += GetAsyN (*p, curmonkey);
	sumw++;
      }
  else if (wts.size() != keys.size())
    cerr << "TaMultiplet::GetAsyNAve ERROR: Weight and key vector sizes differ" << endl;
  else
    for (size_t i = 0; i < keys.size(); ++i)
      {
	sumxw += wts[i] * GetAsyN (keys[i], curmonkey);
	sumw += wts[i];
      }

  if (sumw <= 0)
    {
      cerr << "TaMultiplet::GetAsyNAve ERROR: Weight sum non-positive" << endl;
      return 0;
    }
  return sumxw / sumw;
}


Bool_t 
TaMultiplet::PassedCuts() const
{
  // True if no event has cut condition

  vector<VaPair>::const_iterator pairItr;
  for (pairItr  = fPairs.begin(); 
       pairItr != fPairs.end() && pairItr->PassedCuts(); 
       pairItr++)
    {} // empty loop body

  return pairItr == fPairs.end();
}

Bool_t 
TaMultiplet::PassedCutsInt (const TaCutList& cl) const
{
  // True if no event is in cut interval

  vector<VaPair>::const_iterator pairItr;
  for (pairItr  = fPairs.begin(); 
       pairItr != fPairs.end() && pairItr->PassedCutsInt (cl); 
       pairItr++)
    {} // empty loop body

  return pairItr == fPairs.end();
}

Bool_t 
TaMultiplet::PassedCCutsInt (const TaCutList& cl) const
{
  // True if no event is in cut interval (for hall C)

  vector<VaPair>::const_iterator pairItr;
  for (pairItr  = fPairs.begin(); 
       pairItr != fPairs.end() && pairItr->PassedCCutsInt (cl); 
       pairItr++)
    {} // empty loop body

  return pairItr == fPairs.end();
}

Bool_t 
TaMultiplet::BeamCut () const
{
  // True if any event has beam cut
  vector<VaPair>::const_iterator pairItr;
  for (pairItr  = fPairs.begin(); 
       pairItr != fPairs.end() && !(pairItr->BeamCut());
       pairItr++)
    {} // empty loop body

  return pairItr != fPairs.end();
}

Bool_t 
TaMultiplet::BeamCCut () const
{
  // True if any event has Hall C beam cut
  vector<VaPair>::const_iterator pairItr;
  for (pairItr  = fPairs.begin(); 
       pairItr != fPairs.end() && !(pairItr->BeamCCut());
       pairItr++)
    {} // empty loop body

  return pairItr != fPairs.end();
}



const vector<TaLabelledQuantity>& 
TaMultiplet::GetResults() const
{
  return fResults;
}


// Private member functions


