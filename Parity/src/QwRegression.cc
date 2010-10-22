/*
 * QwRegression.cc
 *
 *  Created on: Oct 22, 2010
 *      Author: wdconinc
 */

#include "QwRegression.h"

QwRegression::QwRegression(QwHelicityPattern& helicitypattern)
{
  /// Get the asymmetry and difference for the pattern
  QwSubsystemArrayParity& asym = helicitypattern.fAsymmetry;
  QwSubsystemArrayParity& diff = helicitypattern.fDifference;

  // Load dv's from parameter file
  std::vector<std::string> dv_names;
  dv_names.push_back("md1neg");
  dv_names.push_back("md2neg");
  dv_names.push_back("md3neg");
  dv_names.push_back("md4neg");
  dv_names.push_back("md5neg");
  dv_names.push_back("md6neg");
  dv_names.push_back("md7neg");
  dv_names.push_back("md8neg");

  // Load iv's and sensitivities from parameter file
  std::vector<std::string> iv_names;
  std::vector<double> iv_sens;
  iv_names.push_back("x_targ");  iv_sens.push_back(0.10);
  iv_names.push_back("y_targ");  iv_sens.push_back(0.12);
  iv_names.push_back("xp_targ"); iv_sens.push_back(0.08);
  iv_names.push_back("yp_targ"); iv_sens.push_back(0.11);


  /// Fill structures
  for (size_t dv = 0; dv < dv_names.size(); dv++) {
    VQwDataElement* dv_ptr = asym.ReturnInternalValueForFriends(dv_names[dv]);
    if (dv_ptr) {
      QwMessage << "dv: " << dv_names[dv] << QwLog::endl;
      fDependent.push_back(dv_ptr);
      fIndependent.resize(fDependent.size());
      fSensitivity.resize(fDependent.size());
      for (size_t iv = 0; iv < iv_names.size(); iv++) {
        VQwDataElement* iv_ptr = diff.ReturnInternalValueForFriends(iv_names[iv]);
        if (iv_ptr) {
          QwMessage << " iv: " << iv_names[iv] << QwLog::endl;
          fIndependent.back().push_back(iv_ptr);
          fSensitivity.back().push_back(iv_sens[iv]);
        }
      }
    }
  }
}

void QwRegression::LinearRegression(QwHelicityPattern& helicitypattern)
{
  /// Linear regression
  for (size_t i = 0; i < fDependent.size(); i++) {
    for (size_t j = 0; j < fIndependent.at(i).size(); j++) {
      QwVQWK_Channel correction("correction");
      correction.Copy(fIndependent.at(i).at(j));
      correction.Scale(fSensitivity.at(i).at(j));
      *(fDependent.at(i)) += correction;
    }
  }
}
