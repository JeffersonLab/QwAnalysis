/**********************************************************\
* File: QwHelicityCorrelatedFeedback.cc                   *
*                                                         *
* Author:Author: Rakitha Beminiwattha                     *
* Time-stamp:Time-stamp: <2010-09-28>                     *
\**********************************************************/

#include "QwHelicityCorrelatedFeedback.h"

/*****************************************************************/


/**
 * Defines configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwHelicityCorrelatedFeedback::DefineOptions(QwOptions &options)
{
  QwHelicityPattern::DefineOptions(options);
}

/*****************************************************************/
void QwHelicityCorrelatedFeedback::ProcessOptions(QwOptions &options)
{
  QwHelicityPattern::ProcessOptions(options);
}

/*****************************************************************/
void QwHelicityCorrelatedFeedback::LoadParameterFile(TString filename){


};


/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedIASetPoint(){
};
/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedPCPos(){
};
/*****************************************************************/
void QwHelicityCorrelatedFeedback::FeedPCNeg(){
};
/*****************************************************************/
void QwHelicityCorrelatedFeedback::LogParameters(){
};
/*****************************************************************/
void QwHelicityCorrelatedFeedback::UpdateGMClean(Int_t state){
};
/*****************************************************************/
void QwHelicityCorrelatedFeedback::UpdateGMScanParameters(){
};
/*****************************************************************/
Bool_t QwHelicityCorrelatedFeedback::IsAqPrecisionGood(){
  return kFALSE;
};
/*****************************************************************/
Bool_t QwHelicityCorrelatedFeedback::IsPatternsAccumulated(){
  return kFALSE;
};
/*****************************************************************/
Int_t QwHelicityCorrelatedFeedback::GetLastGoodHelicityPatternType(){
  return 0;
};
/*****************************************************************/
