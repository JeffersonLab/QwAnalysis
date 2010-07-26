#ifndef QWCORRELATOR_H
#define QWCORRELATOR_H

// System headers
#include <vector>
#include <iostream>

// for UBLAS
// code at: /usr/include/boost/numeric/ublas/matrix.hpp 

#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

// ROOT headers
#include "TString.h"

// Qweak headers
#include "QwSubsystemArrayParity.h"


// Forward declarations

/**
 * \class QwCorrelator
 * \ingroup Qw??
 *
 * \brief Computes & applies linear regression corrections
 *
 */

class QwCorrelatorVariable {
 public:
  TString name;
  const QwVQWK_Channel* channel;
  QwCorrelatorVariable( TString x="fixMe"){ name=x; channel=0;}
};


//-----------------------------------------
class QwCorrelator {
 private:
  //
  Long64_t fGoodEventNumber;	///< accumulated so far 
  Int_t   fNumVar;  ///< number of monitored variables
  TString fAllNames;
  std::vector< QwCorrelatorVariable> fVariables; ///< list of  monitored variables
  boost::numeric::ublas::vector<double> fM1; ///< mean 
  boost::numeric::ublas::matrix<double> fC2; ///< correlation matrix, includes diagonal

  TH1 *** hC; ///< hC[i][j]  monitoring 2D Correlation histograms
  TH1 **  hM; ///<   monitoring 1D  histograms of the means
  void  recomputHistoAxis();

  double par_highCorr; // for tagging
 public:  
  /// Default constructor
  QwCorrelator();
  /// Destructor
  virtual ~QwCorrelator();
  
  /// initialization of histos
  void SetHistos();
  /// initialization of monitred variables

  /// orders variables by name, to lower case,  removes duplicates
  void AccessInputVector( QwSubsystemArrayParity &detectors);

  /// appends current list of variables, must be done before  AccessInputVector()
  void AddInputVariables(TString x){ fAllNames+=" "+x; assert(fVariables.size()==0);}

  /// processing single events
  void Accumulate();
  // after last event
  void PrintSummary();
  // add decorative information to histograms before saving
  void NiceOutput();

  /// Get mean value of a variable, returns error code
  Int_t GetElementMean(const int i, Double_t &mean );

  /// Get mean value of a variable, returns error code
  Int_t GetElementSigma(const int i, Double_t &sigma );

  /// Get mean value of a variable, returns error code
  Int_t GetCovariance( int i, int j, Double_t &covar );
  
  /// \brief Output stream operator
  friend std::ostream& operator<< (std::ostream& stream, const QwCorrelator& h);
    
};

/// Output stream operator
inline std::ostream& operator<< (std::ostream& stream, const QwCorrelator& h) {
  stream << "Correlator::print() ";
  stream << "num. of variables " << h.fNumVar << ", ";
  stream << "good events " << h.fGoodEventNumber << ",";
  return stream;
}



#endif // QWCORRELATOR_H
