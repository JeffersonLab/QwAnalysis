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
  std::vector< QwCorrelatorVariable> fVariables; ///< list of  monitored variables
  boost::numeric::ublas::vector<double> fM1,fM2; ///< mean & variance
  boost::numeric::ublas::matrix<double> fC2; ///< correlation matrix

  TH1 *** hM; ///< hM[i][j] holds pointers to monitoring histograms
  void  recomputHistoAxis();


 public:  
  /// Default constructor
  QwCorrelator();
  /// Destructor
  virtual ~QwCorrelator();
  
  /// initialization of histos
  void SetHistos();
  /// initialization of monitred variables
  void SetInputVector( QwSubsystemArrayParity &detectors);
  /// processing single events
  void AddEvent();
  // after last event
  void PrintSummary();

  /// Get mean value of a variable, returns error code
  Int_t GetElementMean(const int i, Double_t &mean ){
    mean=-1e50;
    if(i<0 || i >= (int)fVariables.size() ) return -1; 
    if( fVariables[i].channel==0) return -2;
    if( fGoodEventNumber<1) return -3;
    mean=fM1[i];    return 0;
  }

  /// Get mean value of a variable, returns error code
  Int_t GetElementSigma(const int i, Double_t &sigma ){
    sigma=-1e50;
    if(i<0 || i >= (int)fVariables.size() ) return -1; 
    if( fVariables[i].channel==0) return -2;
    if( fGoodEventNumber<2) return -3;
    sigma=sqrt(fM2[i]/(fGoodEventNumber-1.));
    return 0;
  }

  /// Get mean value of a variable, returns error code
  Int_t GetCovariance( int i, int j, Double_t &covar ){
    covar=-1e50;
    if(i==j)  return  GetElementSigma(i,covar); // diagonal 

    if( i>j) { int k=i; i=j; j=k; }//swap
    //... now we need only upper right triangle 
    if(i<0 || j >= (int)fVariables.size() ) return -11; 
    if( fVariables[i].channel==0) return -12;
    if( fVariables[j].channel==0) return -13;
    if( fGoodEventNumber<2) return -14;
    covar=fC2(i,j)/(fGoodEventNumber-1.);
    return 0;
  }

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
