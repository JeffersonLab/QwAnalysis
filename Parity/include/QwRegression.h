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

    /// \brief Constructor with event
    QwRegression(QwSubsystemArrayParity& detectors) {
      QwMessage << "Regression of event yields not implemented" << QwLog::endl;
    };
    /// \brief Constructor with pattern
    QwRegression(QwHelicityPattern& helicitypattern);
    /// Virtual destructor
    virtual ~QwRegression() { };

    /// \brief Linear regression
    void LinearRegression(QwHelicityPattern& helicitypattern);


  private:

    /// Private default constructor
    QwRegression() { };

    /// List of channels to use in the regression
    std::vector< VQwDataElement* > fDependent;
    std::vector< std::vector< VQwDataElement* > > fIndependent;
    std::vector< std::vector< Double_t> > fSensitivity;


}; // class QwRegression

#endif // QWREGRESSION_H_
