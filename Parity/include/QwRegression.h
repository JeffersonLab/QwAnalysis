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
#include "VQwHardwareChannel.h"

/**
 * ... info to follow ... really, I promise ...
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
    /// \brief Constructor with single event
    QwRegression(QwOptions &options, QwSubsystemArrayParity& event);
    /// \brief Constructor with helicity pattern
    QwRegression(QwOptions &options, QwHelicityPattern& helicitypattern);
    /// \brief Constructor with only options
    QwRegression(QwOptions &options);
    /// Virtual destructor
    virtual ~QwRegression();
    QwRegression(const QwRegression &source);
    void Copy (const QwRegression *source);


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
    /// \brief Connect to Channels (event only)
    Int_t ConnectChannels(QwSubsystemArrayParity& event);

    /// \brief Linear regression
    void LinearRegression(EQwRegType type);


    /// \name Tree and vector construction and maintenance
    // @{
    /// Construct the tree and vector for this subsystem
    void ConstructBranchAndVector(TTree *tree, std::vector <Double_t> &values) {
      TString tmpstr("");
      ConstructBranchAndVector(tree,tmpstr,values);
    };
    /// \brief Construct a branch and vector for this subsystem with a prefix
    void ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector <Double_t> &values);
    /// \brief Fill the vector for this subsystem
    void FillTreeVector(std::vector<Double_t> &values) const;
    // @}

 

    void AccumulateRunningSum(QwRegression value);
    void CalculateRunningAverage();
    void PrintValue() const;

  private:

    /// Private default constructor
    QwRegression() { };
  
  protected:
    /// Flag to enable regression
    bool fEnableRegression;

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
    std::vector< std::pair< VQwHardwareChannel*, VQwHardwareChannel*> > fDependentVar;
    std::vector< std::vector< EQwRegType > > fIndependentType;
    std::vector< std::vector< std::string > > fIndependentName;
    std::vector< std::vector< const VQwHardwareChannel* > > fIndependentVar;
    std::vector< std::vector< Double_t> > fSensitivity;


}; // class QwRegression

#endif // QWREGRESSION_H_
