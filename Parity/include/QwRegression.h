/*
 * QwRegression.h
 *
 *  Created on: Oct 22, 2010
 *      Author: wdconinc
 */

#ifndef QWREGRESSION_H_
#define QWREGRESSION_H_

// Qweak headers
#include "QwHelicityPattern.h"
#include "QwSubsystemArrayParity.h"

/**
 * ... info to follow ...
 */
class QwRegression {

  public:

    /// Type of regression variable
    enum EQwRegType {
      kRegTypeUnknown, kRegTypeMps, kRegTypeAsym, kRegTypeDiff
    };

  public:

    /// \brief Constructor with single event and helicity pattern
    QwRegression(QwOptions &options,
        QwSubsystemArrayParity& event,
        QwHelicityPattern& helicitypattern);
    /// Virtual destructor
    virtual ~QwRegression() { };


    /// \brief Define the configuration options
    static void DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);


    /// \brief Load the channels and sensitivities
    Int_t LoadChannelMap(const std::string& mapfile);
    /// \brief Connect to channels
    Int_t ConnectChannels(QwSubsystemArrayParity& event,
        QwSubsystemArrayParity& asym,
        QwSubsystemArrayParity& diff);


    /// \brief Linear regression
    void LinearRegression(EQwRegType type);


  private:

    /// Private default constructor
    QwRegression() { };

    /// Regression dependent and independent variable map
    std::string fRegressionMapFile;

    /// Single event pointer
    QwSubsystemArrayParity* fSubsystemArray;
    /// Helicity pattern pointer
    QwHelicityPattern* fHelicityPattern;

    /// Parse regression variable in type and name
    std::pair<EQwRegType,std::string> ParseRegressionVariable(const std::string& variable);

    /// List of channels to use in the regression
    std::vector< EQwRegType > fDependentType;
    std::vector< std::string > fDependentName;
    std::vector< VQwDataElement* > fDependentVar;
    std::vector< std::vector< EQwRegType > > fIndependentType;
    std::vector< std::vector< std::string > > fIndependentName;
    std::vector< std::vector< VQwDataElement* > > fIndependentVar;
    std::vector< std::vector< Double_t> > fSensitivity;


}; // class QwRegression

#endif // QWREGRESSION_H_
