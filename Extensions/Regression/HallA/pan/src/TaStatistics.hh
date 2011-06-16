//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       TaStatistics.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaStatistics.hh,v 1.9 2003/07/31 16:12:00 rsholmes Exp $
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

#ifndef PAN_TaStatistics
#define PAN_TaStatistics

#include <math.h>
#include "Rtypes.h"
#include <vector>
#include <utility>
#include <iostream>
using namespace std;

class TaStatistics
{

public:

  // Constructors/destructors/operators

  TaStatistics (const size_t nquant, const Bool_t goodErrors = true);  

  TaStatistics (const TaStatistics& s);
  virtual ~TaStatistics();
  const TaStatistics& operator= (const TaStatistics& s);

  // Major functions

  // Functions to update sums
  void Zero ();                 // Set sums to zero
  
  // The Update functions update the sums associated with a
  // statistic or set of statistics.  Third argument (weight) can be
  // omitted (or passed as zero), in which case weight = 1/xerr^2.
  // Second argument (xerr) is ignored, and can be omitted, if
  // statistics constructed with goodErrors = false.
  
  void Update ( const vector<Double_t>& x,
		const vector<Double_t>& xerr = vector<Double_t>(0),
		const vector<Double_t>& w = vector<Double_t>(0) );       // Add a vector
  void Update ( const Double_t x, 
		const Double_t xerr = 0, 
		const Double_t w = 0); // Add a single quantity
  TaStatistics& operator+= ( const TaStatistics& s );           // Combine statistics             
  
  // Functions to return results
  Int_t     N() const;                   // number of events in stats
  size_t    Size() const;                // size of statistics vector (nquant)
  vector<Double_t> DataRMS() const;      // vector of RMS of x
  vector<Double_t> Mean() const;         // vector of means of x
  vector<Double_t> MeanVar() const;      // vector of variances of means of x
  vector<Double_t> MeanErr() const;      // vector of errors of means of x
  vector<pair<Double_t,Double_t> > 
  MeanAndErr() const;   // vector of means of x and their errors
  vector<Double_t>  Neff() const;        // effective N for all x
  Double_t  DataRMS(const size_t i) const;     // RMS of one x
  Double_t  Mean(const size_t i) const;        // mean of one x
  Double_t  MeanVar(const size_t i) const;     // variance of mean of one x
  Double_t  MeanErr(const size_t i) const;     // error of mean of one x
  pair<Double_t,Double_t> 
  MeanAndErr(const size_t i) const;  // mean of one x and its error
  Double_t  Neff(const size_t i) const;        // effective N for one x
  
  // Other functions
  void      SetFirstPass(Bool_t);        // for multipass analysis
  void      DumpSums (const size_t i) const;         // For debugging, print raw sums for one quantity.

private:
  
// Private member functions

  Double_t PDataMS (const size_t i) const;
  Double_t PMean (const size_t i) const;
  Double_t PMeanVar (const size_t i) const;
  Double_t PNeff (const size_t i) const;

  // Data members

  Bool_t             fGoodErrors;   // update functions will pass good errors
  Bool_t             fFirstPass;    // First or second pass?
  Int_t              fN;            // sum of 1 (= N)
  Int_t              fN2;           // sum of 1 (= N) for second pass
  vector<Double_t>   fSumWt;        // sum of weights
  vector<Double_t>   fSumWt2;       // sum of weights^2
  vector<Double_t>   fSumWtX;       // sum of weights * x
  vector<Double_t>   fSumWtX2;      // sum of weights * x^2
  vector<Double_t>   fSumWt2Err2;   // sum of weights^2 * error^2
  vector<Double_t>   fXbar;         // means of x from first pass
  vector<Double_t>   fSumWtXs;      // sum of weights * (x-<x>)
  vector<Double_t>   fSumWtXs2;     // sum of weights * (x-<x>)^2

#ifndef NODICT
  ClassDef(TaStatistics, 0)  //   Statistics -- means, widths, etc
#endif

};

// Non member functions

#endif
