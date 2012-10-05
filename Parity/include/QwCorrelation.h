/*
 * QwCorrelation.h
 *
 *  Created on: Oct 3, 2012
 *      Author: wdconinc,smacewan
 */

#ifndef QWCORRELATION_H_
#define QWCORRELATION_H_

// Qweak headers
#include "QwHelicityPattern.h"
#include "QwSubsystemArrayParity.h"
#include "VQwHardwareChannel.h"

/**
 * ... info to follow ... really, I promise ...
 */
class QwCorrelation {

  public:

    /// Type of regression variable
    enum EQwRegType {
      kRegTypeUnknown=0, kRegTypeMps, kRegTypeAsym, kRegTypeDiff
    };

    typedef std::vector< std::pair< VQwHardwareChannel*,VQwHardwareChannel*> >::iterator PairIterator;
    typedef std::vector< std::pair< VQwHardwareChannel*,VQwHardwareChannel*> >::const_iterator PairConstIterator;

  public:

    /// \brief Constructor with helicity pattern
    QwCorrelation(QwOptions &options, QwHelicityPattern& helicitypattern);
    /// Virtual destructor
    virtual ~QwCorrelation();


    /// \brief Define the configuration options
    static void DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);


    /// \brief Load the channels
    Int_t LoadChannelMap(const std::string& mapfile);

    /// \brief Connect to Channels (asymmetry/difference only)
    Int_t ConnectChannels(QwSubsystemArrayParity& asym,
                          QwSubsystemArrayParity& diff);

    /// \brief Accumulate correlations
    void AccumulateCorrelations(EQwRegType type) { };

  private:

    /// Private default constructor
    QwCorrelation();

  protected:

    /// Regression dependent and independent variable map
    std::string fCorrelationMapFile;

    /// Helicity pattern pointer
    QwHelicityPattern* fHelicityPattern;

    UInt_t fErrorFlag;

    /// Parse regression variable in type and name
    std::pair<EQwRegType,std::string> ParseRegressionVariable(const std::string& variable);

    /// Correlation variables
    std::vector< std::string > fCorrelationName; // e.g. diff_md1neg_vs_asym_md1pos
    std::vector< VQwHardwareChannel* > fCorrelation; // e.g. QwVQWK_Channel("diff_md1neg_vs_asym_md1pos")
    std::vector< std::pair< const EQwRegType, const EQwRegType> > fIndependentType;
    std::vector< std::pair< const std::string, const std::string> > fIndependentName;
    std::vector< std::pair< const VQwHardwareChannel*, const VQwHardwareChannel*> > fIndependentVar;

}; // class QwCorrelation

inline std::ostream& operator<< (std::ostream& stream, const QwCorrelation::EQwRegType& i) {
  switch (i){
  case QwCorrelation::kRegTypeMps:  stream << "mps"; break;
  case QwCorrelation::kRegTypeAsym: stream << "asym"; break;
  case QwCorrelation::kRegTypeDiff: stream << "diff"; break;
  default:           stream << "Unknown"; break;
  }
  return stream;
}


#endif // QWCORRELATION_H_
/*
 * QwCorrelation.h
 *
 *  Created on: Oct 3, 2012
 *      Author: wdconinc,smacewan
 */

#ifndef QWCORRELATION_H_
#define QWCORRELATION_H_

// Qweak headers
#include "QwHelicityPattern.h"
#include "QwSubsystemArrayParity.h"
#include "VQwHardwareChannel.h"

/**
 * ... info to follow ... really, I promise ...
 */
class QwCorrelation {

  public:

    /// Type of regression variable
    enum EQwRegType {
      kRegTypeUnknown=0, kRegTypeMps, kRegTypeAsym, kRegTypeDiff
    };

    typedef std::vector< std::pair< VQwHardwareChannel*,VQwHardwareChannel*> >::iterator PairIterator;
    typedef std::vector< std::pair< VQwHardwareChannel*,VQwHardwareChannel*> >::const_iterator PairConstIterator;

  public:

    /// \brief Constructor with helicity pattern
    QwCorrelation(QwOptions &options, QwHelicityPattern& helicitypattern);
    /// Virtual destructor
    virtual ~QwCorrelation();


    /// \brief Define the configuration options
    static void DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);


    /// \brief Load the channels
    Int_t LoadChannelMap(const std::string& mapfile);

    /// \brief Connect to Channels (asymmetry/difference only)
    Int_t ConnectChannels(QwSubsystemArrayParity& asym,
                          QwSubsystemArrayParity& diff);

    /// \brief Accumulate correlations
    void AccumulateCorrelations(EQwRegType type) { };

  private:

    /// Private default constructor
    QwCorrelation();

  protected:

    /// Regression dependent and independent variable map
    std::string fCorrelationMapFile;

    /// Helicity pattern pointer
    QwHelicityPattern* fHelicityPattern;

    UInt_t fErrorFlag;

    /// Parse regression variable in type and name
    std::pair<EQwRegType,std::string> ParseRegressionVariable(const std::string& variable);

    /// Correlation variables
    std::vector< std::string > fCorrelationName; // e.g. diff_md1neg_vs_asym_md1pos
    std::vector< VQwHardwareChannel* > fCorrelation; // e.g. QwVQWK_Channel("diff_md1neg_vs_asym_md1pos")
    std::vector< std::pair< const EQwRegType, const EQwRegType> > fIndependentType;
    std::vector< std::pair< const std::string, const std::string> > fIndependentName;
    std::vector< std::pair< const VQwHardwareChannel*, const VQwHardwareChannel*> > fIndependentVar;

}; // class QwCorrelation

inline std::ostream& operator<< (std::ostream& stream, const QwCorrelation::EQwRegType& i) {
  switch (i){
  case QwCorrelation::kRegTypeMps:  stream << "mps"; break;
  case QwCorrelation::kRegTypeAsym: stream << "asym"; break;
  case QwCorrelation::kRegTypeDiff: stream << "diff"; break;
  default:           stream << "Unknown"; break;
  }
  return stream;
}


#endif // QWCORRELATION_H_
/*
 * QwCorrelation.h
 *
 *  Created on: Oct 3, 2012
 *      Author: wdconinc,smacewan
 */

#ifndef QWCORRELATION_H_
#define QWCORRELATION_H_

// Qweak headers
#include "QwHelicityPattern.h"
#include "QwSubsystemArrayParity.h"
#include "VQwHardwareChannel.h"

/**
 * ... info to follow ... really, I promise ...
 */
class QwCorrelation {

  public:

    /// Type of regression variable
    enum EQwRegType {
      kRegTypeUnknown=0, kRegTypeMps, kRegTypeAsym, kRegTypeDiff
    };

    typedef std::vector< std::pair< VQwHardwareChannel*,VQwHardwareChannel*> >::iterator PairIterator;
    typedef std::vector< std::pair< VQwHardwareChannel*,VQwHardwareChannel*> >::const_iterator PairConstIterator;

  public:

    /// \brief Constructor with helicity pattern
    QwCorrelation(QwOptions &options, QwHelicityPattern& helicitypattern);
    /// Virtual destructor
    virtual ~QwCorrelation();


    /// \brief Define the configuration options
    static void DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);


    /// \brief Load the channels
    Int_t LoadChannelMap(const std::string& mapfile);

    /// \brief Connect to Channels (asymmetry/difference only)
    Int_t ConnectChannels(QwSubsystemArrayParity& asym,
                          QwSubsystemArrayParity& diff);

    /// \brief Accumulate correlations
    void AccumulateCorrelations(EQwRegType type) { };

  private:

    /// Private default constructor
    QwCorrelation();

  protected:

    /// Regression dependent and independent variable map
    std::string fCorrelationMapFile;

    /// Helicity pattern pointer
    QwHelicityPattern* fHelicityPattern;

    UInt_t fErrorFlag;

    /// Parse regression variable in type and name
    std::pair<EQwRegType,std::string> ParseRegressionVariable(const std::string& variable);

    /// Correlation variables
    std::vector< std::string > fCorrelationName; // e.g. diff_md1neg_vs_asym_md1pos
    std::vector< VQwHardwareChannel* > fCorrelation; // e.g. QwVQWK_Channel("diff_md1neg_vs_asym_md1pos")
    std::vector< std::pair< const EQwRegType, const EQwRegType> > fIndependentType;
    std::vector< std::pair< const std::string, const std::string> > fIndependentName;
    std::vector< std::pair< const VQwHardwareChannel*, const VQwHardwareChannel*> > fIndependentVar;

}; // class QwCorrelation

inline std::ostream& operator<< (std::ostream& stream, const QwCorrelation::EQwRegType& i) {
  switch (i){
  case QwCorrelation::kRegTypeMps:  stream << "mps"; break;
  case QwCorrelation::kRegTypeAsym: stream << "asym"; break;
  case QwCorrelation::kRegTypeDiff: stream << "diff"; break;
  default:           stream << "Unknown"; break;
  }
  return stream;
}


#endif // QWCORRELATION_H_
