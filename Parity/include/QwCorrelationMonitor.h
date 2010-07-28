#ifndef QWCORRELMONITOR_H
#define QWCORRELMONITOR_H

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
class VQwSubsystemParity;

/**
 * \class QwCorrelationMonitor
 * \ingroup QwParity
 *
 * \brief Computes correlation between arbitrary channels
 *
 * multiple instances work  independently of different lists. 
 * Same channel may be used on different lists
 * Full correlation matrix is printed out
 * Histos are saved in the root file, large correlation tagged,  and displayable by the plotting macro: plCorrMon.C 
 *  Writeup is posted in the ELOG
 *  https://qweak.jlab.org/elog/Analysis+%26+Simulation/22
 */

class QwCorrelationMonitorVariable {
 public:
  TString name;
  const QwVQWK_Channel* channel;
  QwCorrelationMonitorVariable( TString x="fixMe"){ name=x; channel=0;}
};


//-----------------------------------------
class QwCorrelationMonitor {
 private:
  /// skip events at aall and for histogramming
  int par_nSkipEveHist;

  Long64_t fGoodEventNumber;	///< accumulated so far 
  TString fAllNames;
  TString fCore;
  std::vector< QwCorrelationMonitorVariable> fVariables; ///< list of  monitored variables
  boost::numeric::ublas::vector<double> fM1; ///< mean 
  boost::numeric::ublas::matrix<double> fC2; ///< correlation matrix, includes diagonal

  TH1 *** hC; ///< hC[i][j]  monitoring 2D Correlation histograms
  TH1 **  hM; ///<   monitoring 1D  histograms of the means
  TH1 *   hA; ///< keeps just list of variables
  void  recomputHistoAxis();

  double par_highCorr; // for tagging

 public:  
  /// Default constructor
  QwCorrelationMonitor();
  void AddVariableList(TString mapName);
  /// Destructor
  virtual ~QwCorrelationMonitor();
   
  /// initialization of histos
  void ConstructHistograms();
  void DeleteHistograms();
  /// initialization of monitred variables
  void AccessChannels( QwSubsystemArrayParity &detectors);
  void AccessChannels( VQwSubsystemParity *detector);
  /// appends current list of variables, must be done before  AccessInputVector()
  void AddVariables(TString x){ fAllNames+=" "+x; assert(fVariables.size()==0);}
  void SetParams(TString core, int n2,float x1) { fCore=core;  par_nSkipEveHist=n2; par_highCorr=x1;}

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
  friend std::ostream& operator<< (std::ostream& stream, const QwCorrelationMonitor& h);
    
};

/// Output stream operator
inline std::ostream& operator<< (std::ostream& stream, const QwCorrelationMonitor& h) {
  stream << Form("CorrelationMonitor_%s::print() ",h.fCore.Data());
  stream << Form("  event cuts:  nFirstHist=%d ,", h.par_nSkipEveHist);
  stream << Form("  tag correlation>%.3f ", h.par_highCorr);
  stream << "good events " << h.fGoodEventNumber << ","<<h.par_nSkipEveHist;
  return stream;
}



#endif 
