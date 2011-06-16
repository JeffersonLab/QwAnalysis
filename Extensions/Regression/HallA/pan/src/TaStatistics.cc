//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       TaStatistics.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaStatistics.cc,v 1.9 2003/07/31 16:12:00 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    A general purpose statistics class, allowing accumulation of
//    sums for one value or a set of values, from which quantities
//    such as means, widths, errors, etc. can be computed.
//
//    Note that this class permits a single-pass analysis with *no*
//    attempt to reduce errors on the computed quantities.  Such an
//    analysis, using mathematically correct formulas, can still give
//    badly erroneous results for RMS / variance / error due to
//    roundoff problems.  In particular, when N is large and the
//    variance is small, mean(x^2)-(mean(x))^2 is a difference of two
//    large numbers and can have very poor precision.  Results can be
//    improved by making an estimate of mean(x) and subtracting this
//    from each x_i, but automating such an estimate is difficult.
//
//    However, this class also allows 2-pass calculations to be made
//    if desired, with greatly improved precision.  Note that in
//    practice the aforementioned roundoff problems have not been
//    shown to be significant for HAPPEX data.  For more on this
//    subject see for example _The American Statistician_ V. 37 p. 242
//    (1982).
//
////////////////////////////////////////////////////////////////////////

#include "TaStatistics.hh"

#ifndef NODICT
ClassImp(TaStatistics)
#endif

// Constructors/destructors/operators

TaStatistics::TaStatistics (const size_t nquant, const Bool_t goodErrors ):
  fGoodErrors(goodErrors),
  fFirstPass(true),
  fN(0),
  fN2(0),
  fSumWt(nquant,0),
  fSumWt2(nquant,0),
  fSumWtX(nquant,0),
  fSumWtX2(nquant,0),
  fSumWt2Err2(nquant,0),
  fXbar(nquant,0),
  fSumWtXs(nquant,0),
  fSumWtXs2(nquant,0)
{
}


TaStatistics::TaStatistics (const TaStatistics& s):
  fGoodErrors(s.fGoodErrors),
  fFirstPass(s.fFirstPass),
  fN(s.fN),
  fN2(s.fN2),
  fSumWt(s.fSumWt),
  fSumWt2(s.fSumWt2),
  fSumWtX(s.fSumWtX),
  fSumWtX2(s.fSumWtX2),
  fSumWt2Err2(s.fSumWt2Err2),
  fXbar(s.fXbar),
  fSumWtXs(s.fSumWtXs),
  fSumWtXs2(s.fSumWtXs2)
{
}


TaStatistics::~TaStatistics()
{
}

const TaStatistics&
TaStatistics::operator= (const TaStatistics& s)
{
  if (this != &s)
    {
      fGoodErrors = s.fGoodErrors;
      fFirstPass = s.fFirstPass;
      fN = s.fN;
      fN2 = s.fN2;
      fSumWt = s.fSumWt;
      fSumWt2 = s.fSumWt2;
      fSumWtX = s.fSumWtX;
      fSumWtX2 = s.fSumWtX2;
      fSumWt2Err2 = s.fSumWt2Err2;
      fXbar = s.fXbar;
      fSumWtXs = s.fSumWtXs;
      fSumWtXs2 = s.fSumWtXs2;
    }
  return *this;
}


// Major functions

// Functions to update sums
void 
TaStatistics::Zero ()
{
  // Zero all the sums.

  fN = 0;
  fN2 = 0;
  fSumWt = vector<Double_t>(fSumWt.size(),0);
  fSumWt2 = vector<Double_t>(fSumWt2.size(),0);
  fSumWtX = vector<Double_t>(fSumWtX.size(),0);
  fSumWtX2 = vector<Double_t>(fSumWtX2.size(),0);
  fSumWt2Err2 = vector<Double_t>(fSumWt2Err2.size(),0);
  fXbar = vector<Double_t>(fXbar.size(),0);
  fSumWtXs = vector<Double_t>(fSumWtXs.size(),0);
  fSumWtXs2 = vector<Double_t>(fSumWtXs2.size(),0);
}

void 
TaStatistics::Update (const vector<Double_t>& x,
		      const vector<Double_t>& xerr,
		      const vector<Double_t>& w)
{
  // Update the sums associated with a set of statistics.  Third
  // argument (weight) can be omitted (or passed as zero), in which
  // case weight = 1/xerr^2.  Second argument (xerr) is ignored, and
  // can be omitted, if statistics object was constructed with
  // goodErrors = false.  Different sums are made on first and second
  // passes.

  if ( x.size() != fSumWt.size() || 
       ( fGoodErrors && xerr.size() != fSumWt.size() ) ||
       ( w.size() > 0 && w.size() != fSumWt.size() ) )
    {
      cerr << "TaStatistics::Update ERROR: Vectors wrong size -- Got ";
      if ( x.size() != fSumWt.size() )
	cerr << "data size " << x.size() << ", ";
      if ( fGoodErrors && xerr.size() != fSumWt.size() )
	cerr << "errors size " << xerr.size() << ", ";
      if ( w.size() > 0 && w.size() != fSumWt.size() )
	cerr << "weights size " << w.size() << ", ";
      cerr  << "expected " << fSumWt.size() << endl;
      return;
    }

  if (fFirstPass)
    {
      ++fN;
      if ( w.size() == 0 )
	{
	  if ( fGoodErrors )
	    {
	      // Weights not specified, use wt=1/error^2
	      for ( size_t i = 0; i < x.size(); ++i )
		{
		  Double_t wt = 1 / xerr[i] / xerr[i];
		  fSumWt[i] += wt;
		  fSumWt2[i] += wt * wt;
		  fSumWtX[i] += wt * x[i];
		  fSumWtX2[i] += wt * x[i] * x[i];
		  fSumWt2Err2[i]+= wt;
		}
	    }
	  else
	    {
	      // Weights and errors not specified, use wt=1
	      for ( size_t i = 0; i < x.size(); ++i )
		{
		  ++fSumWt[i];
		  ++fSumWt2[i];
		  fSumWtX[i] += x[i];
		  fSumWtX2[i] += x[i] * x[i];
		}
	    }
	}
      else
	{
	  // Weights specified, use them
	  for ( size_t i = 0; i < x.size(); ++i )
	    {
	      fSumWt[i] += w[i];
	      fSumWt2[i] += w[i] * w[i];
	      fSumWtX[i] += w[i] * x[i];
	      fSumWtX2[i] += w[i] * x[i] * x[i];
	    }
	  if ( fGoodErrors )
	    {
	      for ( size_t i = 0; i < x.size(); ++i )
		{
		  fSumWt2Err2[i] += w[i] * w[i] * xerr[i] * xerr[i];
		}
	    }
	}
    }
  else
    {
      ++fN2;
      if ( w.size() == 0 )
	{
	  if ( fGoodErrors )
	    {
	      // Weights not specified, use wt=1/error^2
	      for ( size_t i = 0; i < x.size(); ++i )
		{
		  Double_t wt = 1 / xerr[i] / xerr[i];
		  fSumWtXs[i] += wt * (x[i]-fXbar[i]);
		  fSumWtXs2[i] += wt * (x[i]-fXbar[i]) * (x[i]-fXbar[i]);
		}
	    }
	  else
	    {
	      // Weights and errors not specified, use wt=1
	      for ( size_t i = 0; i < x.size(); ++i )
		{
		  fSumWtXs[i] += (x[i]-fXbar[i]);
		  fSumWtXs2[i] += (x[i]-fXbar[i]) * (x[i]-fXbar[i]);
		}
	    }
	}
      else
	{
	  // Weights specified, use them
	  for ( size_t i = 0; i < x.size(); ++i )
	    {
	      fSumWtXs[i] += w[i] * (x[i]-fXbar[i]);
	      fSumWtXs2[i] += w[i] * (x[i]-fXbar[i]) * (x[i]-fXbar[i]);
	    }
	}
    }
}


void 
TaStatistics::Update (const Double_t x, 
		      const Double_t xerr , 
		      const Double_t w ) // Add a single quantity
{
  // Update the sums associated with a single statistic.  Third
  // argument (weight) can be omitted (or passed as zero), in which
  // case weight = 1/xerr^2.  Second argument (xerr) is ignored, and
  // can be omitted, if statistics object was constructed with
  // goodErrors = false.  Different sums are made on first and second
  // passes.

  if ( fSumWt.size() != 1 )
    {
      cerr << "TaStatistics::Update ERROR: Expect vectors" << endl;
      return;
    }

  if (fFirstPass)
    {
      ++fN;
      if ( w == 0 )
	{
	  if ( fGoodErrors )
	    {
	      Double_t wt = 1 / xerr / xerr;
	      fSumWt[0] += wt;
	      fSumWt2[0] += wt * wt;
	      fSumWtX[0] += wt * x;
	      fSumWtX2[0] += wt * x * x;
	      fSumWt2Err2[0] += wt;
	    }
	  else
	    {
	      ++fSumWt[0];
	      ++fSumWt2[0];
	      fSumWtX[0] += x;
	      fSumWtX2[0] += x * x;
	    }
	}
      else
	{
	  fSumWt[0] += w;
	  fSumWt2[0] += w * w;
	  fSumWtX[0] += w * x;
	  fSumWtX2[0] += w * x * x;
	  if ( fGoodErrors )
	    {
	      fSumWt2Err2[0] += w * w * xerr * xerr;
	    }
	}
    }
  else
    {
      ++fN2;
      if ( w == 0 )
	{
	  if ( fGoodErrors )
	    {
	      // Weights not specified, use wt=1/error^2
	      Double_t wt = 1 / xerr / xerr;
	      fSumWtXs[0] += wt * (x-fXbar[0]);
	      fSumWtXs2[0] += wt * (x-fXbar[0]) * (x-fXbar[0]);
	    }
	  else
	    {
	      // Weights and errors not specified, use wt=1
	      fSumWtXs[0] += (x-fXbar[0]);
	      fSumWtXs2[0] += (x-fXbar[0]) * (x-fXbar[0]);
	    }
	}
      else
	{
	  // Weights specified, use them
	  fSumWtXs[0] += w * (x-fXbar[0]);
	  fSumWtXs2[0] += w * (x-fXbar[0]) * (x-fXbar[0]);
	}
    }
}


TaStatistics& 
TaStatistics::operator+= (const TaStatistics& s)
{
  // Sum two statistics objects (of the same size -- otherwise gives
  // error and returns *this).

  if ( fSumWt.size() != s.fSumWt.size() )
    {
      cerr << "TaStatistics::operator+= ERROR: Cannot add stats of unlike size" 
	   << endl;
      return *this;
    }

  fN += s.fN;
  fN2 += s.fN2;
  for ( size_t i = 0; i < fSumWt.size(); ++i )
    {
      fSumWt[i] += s.fSumWt[i];
      fSumWt2[i] += s.fSumWt2[i];
      fSumWtX[i] += s.fSumWtX[i];
      fSumWtX2[i] += s.fSumWtX2[i];
      fSumWt2Err2[i] += s.fSumWt2Err2[i];
      fSumWtXs[i] += s.fSumWtXs[i];
      fSumWtXs2[i] += s.fSumWtXs2[i];
    }
  return *this;
}


// Functions to return results
Int_t     
TaStatistics::N() const                   
{
  // Number of events in stats
  return fN;
}


size_t    
TaStatistics::Size() const                
{
  // Size of statistics vector (nquant)
  return fSumWt.size();
}


vector<Double_t> 
TaStatistics::DataRMS() const      
{
  // Vector of RMS of x

  vector<Double_t> result(fSumWt.size(),0);
  if ( fN > 0 )
    {
      for ( size_t i = 0; i < fSumWt.size(); ++i )
	{
	  result.push_back (sqrt (PDataMS (i)));
	}
    }
  else
    {
      cerr << "TaStatistics::DataRMS ERROR: Statistics empty" << endl;
    }
  return result;
}      


vector<Double_t> 
TaStatistics::Mean() const         
{
  // Vector of means of x
  vector<Double_t> result(fSumWt.size(),0);
  if ( fN > 0 )
    {
      for ( size_t i = 0; i < fSumWt.size(); ++i )
	{
	  result.push_back (PMean (i));
	}
    }
  else
    {
      cerr << "TaStatistics::Mean ERROR: Statistics empty" << endl;
    }
  return result;
}      


vector<Double_t> 
TaStatistics::MeanVar() const      
{
  // Vector of variances of means of x
  vector<Double_t> result(fSumWt.size(),0);
  if ( fN > 0 )
    {
      for ( size_t i = 0; i < fSumWt.size(); ++i )
	{
	  result.push_back (PMeanVar (i));
	}
    }
  else
    {
      cerr << "TaStatistics::MeanVar ERROR: Statistics empty" << endl;
    }
  return result;
}      


vector<Double_t> 
TaStatistics::MeanErr() const      
{
  // Vector of errors of means of x
  vector<Double_t> result(fSumWt.size(),0);
  if ( fN > 0 )
    {
      for ( size_t i = 0; i < fSumWt.size(); ++i )
	{
	  result.push_back ( sqrt (PMeanVar (i)));
	}
    }
  else
    {
      cerr << "TaStatistics::MeanErr ERROR: Statistics empty" << endl;
    }
  return result;
}      


vector<pair<Double_t,Double_t> > 
TaStatistics::MeanAndErr() const   
{
  // Vector of means of x and their errors
  vector<pair<Double_t,Double_t> > result(fSumWt.size(),pair<Double_t,Double_t>(0,0));
  if ( fN > 0 )
    {
      for ( size_t i = 0; i < fSumWt.size(); ++i )
	{
	  result.push_back (make_pair (PMean (i),
				       sqrt (PMeanVar (i))));
	}
    }
  else
    {
      cerr << "TaStatistics::MeanAndErr ERROR: Statistics empty" << endl;
    }
  return result;
}      


vector<Double_t>
TaStatistics::Neff() const 
{
  // Effective N for all x
  vector<Double_t> result(fSumWt.size(),0);
  if ( fN > 0 )
    {
      for ( size_t i = 0; i < fSumWt.size(); ++i )
	{
	  result.push_back (PNeff (i));
	}
    }
  else
    {
      for ( size_t i = 0; i < fSumWt.size(); ++i )
	{
	  result.push_back (0);
	}
    }
  return result;
}      


Double_t 
TaStatistics::DataRMS (const size_t i) const      
{
  // RMS of one x (with error checking)
  if ( fSumWt.size() > i )
    {
      if ( fN > 0 )
	{
	  return sqrt (PDataMS (i));
	}
      else
	{
	  cerr << "TaStatistics::DataRMS ERROR: Statistics empty" << endl;
	  return 0;
	}
    }
  else
    {
      cerr << "TaStatistics::DataRMS ERROR: Index out of range" << endl;
      return 0;
    }
}      


Double_t 
TaStatistics::Mean (const size_t i) const         
{
  // Mean of one x (with error checking)
  if ( fSumWt.size() > i )
    {
      if ( fN > 0 )
	{
	  return PMean (i);
	}
      else
	{
	  cerr << "TaStatistics::Mean ERROR: Statistics empty" << endl;
	  return 0;
	}
    }
  else
    {
      cerr << "TaStatistics::Mean ERROR: Index out of range" << endl;
      return 0;
    }
}      


Double_t 
TaStatistics::MeanVar (const size_t i) const      
{
  // Variance of mean of one x (with error checking)
  if ( fSumWt.size() > i )
    {
      if ( fN > 0 )
	{
	  return PMeanVar (i);
	}
      else
	{
	  cerr << "TaStatistics::MeanVar ERROR: Statistics empty" << endl;
	  return 0;
	}
    }
  else
    {
      cerr << "TaStatistics::MeanVar ERROR: Index out of range" << endl;
      return 0;
    }
}      


Double_t 
TaStatistics::MeanErr (const size_t i) const      
{
  // Error of mean of one x
  if ( fSumWt.size() > i )
    {
      if ( fN > 0 )
	{
	  return sqrt (PMeanVar (i));
	}
      else
	{
	  cerr << "TaStatistics::MeanErr ERROR: Statistics empty" << endl;
	  return 0;
	}
    }
  else
    {
      cerr << "TaStatistics::MeanErr ERROR: Index out of range" << endl;
      return 0;
    }
}      


pair<Double_t,Double_t> 
TaStatistics::MeanAndErr (const size_t i) const   
{
  // Mean of one x and its error
  if ( fSumWt.size() > i )
    {
      if ( fN > 0 )
	{
	  return make_pair (PMean (i),
			    sqrt (PMeanVar (i)));
	}
      else
	{
	  cerr << "TaStatistics::MeanAndErr ERROR: Statistics empty" << endl;
	  return pair<Double_t,Double_t>(0,0);
	}
    }
  else
    {
      cerr << "TaStatistics::MeanAndErr ERROR: Index out of range" << endl;
      return pair<Double_t,Double_t>(0,0);
    }
}      


Double_t
TaStatistics::Neff (const size_t i) const 
{
  // Effective N for one x (with error checking)
  if ( fSumWt.size() > i )
    {
      if ( fN > 0 )
	{
	  return PNeff (i);
	}
      else
	{
	  return 0;
	}
    }
  else
    {
      cerr << "TaStatistics::Neff ERROR: Index out of range" << endl;
      return 0;
    }
}      

void
TaStatistics::SetFirstPass (Bool_t fp)
{
  // Mark this as first or second pass depending on whether argument
  // is true or not.

  fFirstPass = fp;
  if (!fp)
    {
      for (size_t i = 0; i < fXbar.size(); ++i)
	fXbar[i] = PMean(i);
    }
}


void
TaStatistics::DumpSums (const size_t i) const
{
  // For debugging, print raw sums for one quantity.

  cout << " fN: " << fN
       << " fN2: " << fN2
       << " fSumWt: " << fSumWt[i]
       << " fSumWt2: " << fSumWt2[i]
       << " fSumWtX: " << fSumWtX[i]
       << " fSumWtX2: " << fSumWtX2[i]
       << " fSumWt2Err2: " << fSumWt2Err2[i]
       << " fXbar: " << fXbar[i]
       << " fSumWtXs: " << fSumWtXs[i]
       << " fSumWtXs2: " << fSumWtXs2[i]
       << " fFirstPass: " << (fFirstPass?"T":"F")
       << endl;
}

// Private member functions

Double_t
TaStatistics::PDataMS (const size_t i) const
{
  // RMS of one x.  Computed differently depending on whether first or
  // second pass.

  if ( fSumWt[i] > 0 )
    {
      Double_t ms;
      if (fFirstPass)
	{
	  ms = (fSumWtX2[i] - 
	    fSumWtX[i] / fSumWt[i] * fSumWtX[i]) / fSumWt[i];
	}
      else
	{
	  if (fN == fN2)
	    ms = (fSumWtXs2[i] - 
	      fSumWtXs[i] / fSumWt[i] * fSumWtXs[i]) / fSumWt[i];
	  else
	    {
	      cerr << "TaStatistics::PDataMS ERROR: First and second passes unequal length (" 
		   << fN << " and " << fN2 << ")" << endl;
	      return 0;
	    }	    
	}
      if ( ms > 0 )
	return ms;
      else
	{
	  cerr << "TaStatistics::PDataMS ERROR: Mean square deviation non-positive" << endl;
	  return 0;
	}
    }
  else
    {
      cerr << "TaStatistics::PDataMS ERROR: Sum of weights non-positive" << endl;
      return 0;
    }
}


Double_t
TaStatistics::PMean (const size_t i) const
{
  // Mean of one x
  if ( fSumWt[i] > 0 )
    {
      return ( fSumWtX[i] / fSumWt[i] );
    }
  else
    {
      cerr << "TaStatistics::PMean ERROR: Sum of weights non-positive" << endl;
      return 0;
    }
}


Double_t
TaStatistics::PMeanVar (const size_t i) const
{
  // Variance of mean of one x
  if ( fSumWt[i] > 0 )
    {
      if ( fGoodErrors )
	{
	  return fSumWt2Err2[i] / fSumWt[i] / fSumWt[i];
	}
      else
	{
	  return (PDataMS (i)) / (PNeff (i) - 1);
	}
    }
  else
    {
      cerr << "TaStatistics::PMeanVar ERROR: Sum of weights non-positive" << endl;
      return 0;
    }
}


Double_t
TaStatistics::PNeff (const size_t i) const
{
  // Effective N for one x
  if ( fSumWt2[i] > 0 )
    {
      return ( fSumWt[i] / fSumWt2[i] * fSumWt[i] );
    }
  else
    {
      cerr << "TaStatistics::PNeff ERROR: Sum of weights squared non-positive" << endl;
      return 0.0;
    }
}


// Non member functions

