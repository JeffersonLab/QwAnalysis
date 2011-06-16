//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaCutList.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaCutList.cc,v 1.29 2010/04/19 02:08:20 silwal Exp $
//
////////////////////////////////////////////////////////////////////////
//
// Container of cut intervals for a given run. The cut list for a
// given run identifies all the intervals during which a cut condition
// existed.  It also contains extensions for each cut type, telling
// how many events to extend each interval before and after the stored
// event numbers; a tally of events failing each cut type; and labels
// for the cut types.  It provides functions to add cut intervals to
// the list and to determine with intervals, if any, a given event is
// in. The cut list is initialized from the database and updated after
// each event is preprocessed.  
//
////////////////////////////////////////////////////////////////////////

//#define NOISY

#include "TaCutList.hh"
#include <iostream>
#include <iomanip>
#include "TaCutInterval.hh"
#include "TaDataBase.hh"
#include "TaOResultsFile.hh"
#include "TaString.hh"

#ifndef NODICT
ClassImp(TaCutList)
#endif

// Constructors/destructors/operators

TaCutList::TaCutList(RunNumber_t run): fRunNumber(run)
{
  fIntervals = new vector<TaCutInterval>;
}

TaCutList::TaCutList()
{
  fIntervals = new vector<TaCutInterval>;
}

TaCutList::TaCutList (const TaCutList& copy)
{
  fRunNumber = copy.fRunNumber;
  if (copy.fIntervals != 0)
    fIntervals = new vector<TaCutInterval> (*copy.fIntervals);
  else
    fIntervals = 0;
  fOpenIntIndices = copy.fOpenIntIndices;
  fLowExtension = copy.fLowExtension;
  fHighExtension = copy.fHighExtension;
  fTally = copy.fTally;
  fCutNames = copy.fCutNames;
  fCutNames.push_back ("undefined");
}

TaCutList::~TaCutList()
{
  delete fIntervals;
}

TaCutList& 
TaCutList::operator= (const TaCutList& assign)
{
  if (this != &assign)
    {
      fRunNumber = assign.fRunNumber;
      if (assign.fIntervals != 0)
	fIntervals = new vector<TaCutInterval> (*assign.fIntervals);
      else
	fIntervals = 0;
      fOpenIntIndices = assign.fOpenIntIndices;
      fLowExtension = assign.fLowExtension;
      fHighExtension = assign.fHighExtension;
      fTally = assign.fTally;
      fCutNames = assign.fCutNames;
    }
  return *this;
}


// Major functions

void
TaCutList::Init(const TaDataBase& db)
{
  // Initialization of a cut list.  The database is queried for
  // a predefined list of cut intervals.

  clog << "TaCutList::Init Cut list Initialization for run " 
       << fRunNumber << endl;
  
  fNumCuts = (UInt_t) db.GetNumCuts();
  fTally.resize (fNumCuts, 0);

  fCutNames = db.GetCutNames();

  // Cut extensions 

  // These are specified in terms of helicity windows in the database
  // but stored in terms of events -- i.e. we multiply by the
  // oversample factor.

  vector<Int_t> temp;
  SlotNumber_t os = db.GetOverSamp();

  fLowExtension.resize (fNumCuts, 0);
  temp = db.GetExtLo();
  for (size_t i = 0; i < temp.size() && i < fNumCuts; ++i)
    fLowExtension[i] = temp[i] * os;

  fHighExtension.resize (fNumCuts, 0);
  temp = db.GetExtHi();
  for (size_t i = 0; i < temp.size() && i < fNumCuts; ++i)
    fHighExtension[i] = temp[i] * os;

  // Cut values
  for (size_t i = 0; i < size_t (db.GetNumBadEv()); ++i)
    {
      temp = db.GetCutInt()[i];
      if (temp[2] >= 0 && (Cut_t) temp[2] < fNumCuts )
	{
	  Cut_t ct = (Cut_t) temp[2];
	  EventNumber_t elo = temp[0];
	  EventNumber_t ehi = temp[1];
	  fIntervals->push_back(TaCutInterval (ct, temp[3], elo, ehi));
	}
      else
	cerr << "TaCutList::Init WARNING: Unknown cut type = " << temp[2]
	     << " found in database -- ignoring" << endl;
    }

  // Store some selected beam cut types
  fLoBeamCNo = db.GetCutNumber (TaString ("Low_beam_c"));
  fLoBeamNo  = db.GetCutNumber (TaString ("Low_beam"));
  fBurpNo    = db.GetCutNumber (TaString ("Beam_burp"));
  fSatNo     = db.GetCutNumber (TaString ("Det_saturate"));
  fMonSatNo  = db.GetCutNumber (TaString ("Mon_saturate"));
  fPosBurpNo = db.GetCutNumber (TaString ("Pos_burp"));
  fPosBurpENo= db.GetCutNumber (TaString ("Pos_burp_E"));
  fCBurpNo   = db.GetCutNumber (TaString ("C_burp"));
  fAdcxDacBurpNo = db.GetCutNumber (TaString ("Adcx_DAC_burp"));
  fAdcxBadNo = db.GetCutNumber (TaString ("Adcx_Bad"));
  fScalerBadNo = db.GetCutNumber (TaString ("Scaler_Bad"));

}


Bool_t 
TaCutList::OK (const VaEvent& ev) const   
{
  // Return true if event not in any cut interval of this list which
  // has nonzero value.  Lobeamc cut is excepted.

  Bool_t oksofar = true;
  for (vector<TaCutInterval>::const_iterator c = fIntervals->begin();
       oksofar && (c != fIntervals->end()); 
       ++c )
    {
      if (c->GetCut() != fLoBeamCNo &&
	  c->Inside(ev, fLowExtension[c->GetCut()], fHighExtension[c->GetCut()]) && 
	  c->GetVal() != 0)
	oksofar = false;
    }
    return oksofar;
}

Bool_t 
TaCutList::OKC (const VaEvent& ev) const   
{
  // Return true if event not in any cut interval of this list which
  // has nonzero value.  This is for hall C only.

  Bool_t oksofar = true;
  for (vector<TaCutInterval>::const_iterator c = fIntervals->begin();
       oksofar && (c != fIntervals->end()); 
       ++c )
    {
      if (c->GetCut() != fLoBeamNo && 
	  c->GetCut() != fBurpNo &&
	  c->GetCut() != fSatNo &&
	  c->GetCut() != fMonSatNo &&
	  c->GetCut() != fPosBurpNo &&
	  c->GetCut() != fPosBurpENo &&
	  c->GetCut() != fAdcxDacBurpNo &&
	  c->GetCut() != fAdcxBadNo &&
	  c->GetCut() != fScalerBadNo &&
	  c->Inside(ev, fLowExtension[c->GetCut()], 
		    fHighExtension[c->GetCut()]) && 
	  c->GetVal() != 0)
	oksofar = false;
    }
    return oksofar;
}

vector<pair<Cut_t,Int_t> >
TaCutList::CutsFailed (const VaEvent& ev) const 
{
  // Return a vector of cuts (pairs of cut type and value) for which
  // the given event is inside an interval with nonzero value.

  vector<pair<Cut_t,Int_t> > cf;
  for (vector<TaCutInterval>::const_iterator c = fIntervals->begin();
       c != fIntervals->end(); 
       ++c )
    {
      if (c->Inside(ev, fLowExtension[c->GetCut()], fHighExtension[c->GetCut()]) && c->GetVal() != 0)
	cf.push_back(make_pair(c->GetCut(),c->GetVal()));
    }
  return cf;
}


void 
TaCutList::UpdateCutInterval (const Cut_t cut, const Int_t val, const EventNumber_t ev)
{
  // Update the cut list.
  // If val is nonzero and an open interval for this cut type exists, but
  // value is different, close that interval and open a new one.
  // If val is nonzero and no open interval for this cut type exists, make one.
  // If val is zero and an open interval for this cut type exists, close it.

  if (cut >= fNumCuts )
    return;

  // Look for an open interval for this cut
  list<size_t>::iterator oiit;
  for ( oiit = fOpenIntIndices.begin();
	oiit != fOpenIntIndices.end() && (*fIntervals)[*oiit].GetCut() != cut;
	++oiit )
    { } // empty loop body
  size_t oi = ( oiit == fOpenIntIndices.end() ) ? fIntervals->size() : *oiit;

  if ( val )
    {
      ++fTally[cut];
      if ( oi != fIntervals->size() )
	{
	  // Found open cut interval for this cut
	  if ( (*fIntervals)[oi].GetVal() != val )
	    {
	      (*fIntervals)[oi].SetEnd(ev);
	      fOpenIntIndices.erase(oiit);
	      fIntervals->push_back(TaCutInterval(cut, val, ev, fgMaxEvent));
	      fOpenIntIndices.push_back(fIntervals->size()-1);
#ifdef NOISY
	      clog << *this;
#endif
	    }
	}
      else
	{
	  // No open cut interval for this cut
	  fIntervals->push_back(TaCutInterval(cut, val, ev, fgMaxEvent));
	  fOpenIntIndices.push_back(fIntervals->size()-1);
#ifdef NOISY
	      clog << *this;
#endif
	}
    }
  else
    {
      if ( oi != fIntervals->size() )
	{
	  // Found open cut interval for this cut
	  (*fIntervals)[oi].SetEnd(ev);
	  fOpenIntIndices.erase(oiit);
#ifdef NOISY
	      clog << *this;
#endif
	}
    }
}

void 
TaCutList::AddExtension (const Cut_t cut, const UInt_t lex, const UInt_t hex)
{
  // For the given cut type, add a pair of extensions to the extensions lists.

  if (cut >= fNumCuts )
    return;

  fLowExtension[cut] = lex;
  fHighExtension[cut] = hex;
}

void 
TaCutList::AddName (const Cut_t cut, const string& s)
{
  // For the given cut type, add a label to the list of cut type names.

  if (cut >= fNumCuts )
    return;

  fCutNames[cut] = s;
}

const string& 
TaCutList::GetName (const Cut_t cut) const
{
  // Get name from list

  if (cut >= fNumCuts )
    return fCutNames[fNumCuts];

  return fCutNames[(unsigned int) cut];
}

void
TaCutList::PrintInt (ostream& s) const
{
  // Print the intervals in the list to the given output stream.
  // Open intervals are tagged with an asterisk.

  for (size_t i = 0;
       i < fIntervals->size();
       ++i)
    {
      s << (*fIntervals)[i];
      list<size_t>::const_iterator j;
      for (j = fOpenIntIndices.begin();
	   (j != fOpenIntIndices.end()) && (*j != i);
	   ++j)
	{}
      if (j != fOpenIntIndices.end())
	s << "*";
      s << endl;
    }
}

void
TaCutList::PrintExt (ostream& s) const
{
  // Print the lists of extensions.

  s << "Low extensions:" << endl;
  for (vector<UInt_t>::const_iterator k = fLowExtension.begin();
       k != fLowExtension.end();
       ++k)
    s << " " << *k;
  s << endl;
  s << "High extensions:" << endl;
  for (vector<UInt_t>::const_iterator k = fHighExtension.begin();
       k != fHighExtension.end();
       ++k)
    s << " " << *k;
  s << endl;
  s << "N.B. extensions are in events, not windows as in database" << endl;
}

void
TaCutList::PrintTally (ostream& s) const
{
  // Print tally of number of events failing each cut type.
  // (Note that this is not the same as the number of events actually
  // cut, since cut intervals may be extended.)

  s << "Events failing cut conditions:" << endl;
  for (size_t k = 0;
       k != fTally.size();
       ++k)
    {
      s << k << "  ";
      clog.setf(ios::left,ios::adjustfield);
      // N.B. seem to need to convert to C-style string for setw to work.
      s << setw(15) << fCutNames[k].c_str() << " " ;
      clog.setf(ios::right,ios::adjustfield);
      s << setw(6) << fTally[k] << endl;
    }
}

void 
TaCutList::WriteTally (TaOResultsFile& resFile,
		       const EventNumber_t ev0,
		       const EventNumber_t ev1
		       ) const
{
  // Write tally to results file
  
  for (size_t k = 0;
       k != fTally.size();
       ++k)
    {
      string nk = fCutNames[k];
      // Eliminate invalid characters

      for (string::iterator i = (nk).begin(); i != (nk).end(); ++i)
	if (!((*i >= 'a' && *i <= 'z') || 
	      (*i >= 'A' && *i <= 'Z') || 
	      (*i >= '0' && *i <= '9') || 
	      *i == '_'))
	  *i = '_';

      resFile.WriteNextLine (nk + "_condTally", Double_t (fTally[k]), 0, ev0, ev1, "", "");
    }
}

// Access functions

// Private member functions

// Friend functions

ostream& 
operator<< (ostream& s, const TaCutList q)
{
  // Print full information on this cut list -- intervals, extensions,
  // and tallies.

  s << "Run " << q.fRunNumber << " cuts:" << endl;
  q.PrintInt(s);
  q.PrintExt(s);
  q.PrintTally(s);
  return s;
}
