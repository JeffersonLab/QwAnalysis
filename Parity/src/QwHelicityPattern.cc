
/**********************************************************\
* File: QwHelicityPattern.cc                              *
*                                                         *
* Author:                                                 *
* Time-stamp:                                              *
\**********************************************************/

#include "QwHelicityPattern.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwLog.h"
#include "QwHistogramHelper.h"
#include "QwHelicity.h"
#include "QwBlinder.h"


/*****************************************************************/
void QwHelicityPattern::ProcessOptions(QwOptions &options)
{
  // Handle command line options
};
/*****************************************************************/
QwHelicityPattern::QwHelicityPattern(QwSubsystemArrayParity &event)
: fBlinder(TString("No seed string defined!"),QwBlinder::kAdditive)
{
  // Retrieve the helicity subsystem to query for
  std::vector<VQwSubsystem*> subsys_helicity = event.GetSubsystemByType("QwHelicity");
  if (subsys_helicity.size() > 0) {
    // Take the first helicity subsystem
    fHelicitySubsystem = dynamic_cast<QwHelicity*>(subsys_helicity.at(0));
    // And use the maximum pattern phase (i.e. pattern size)
    fPatternSize = fHelicitySubsystem->GetMaxPatternPhase();
    if (subsys_helicity.size() > 1) {
      QwWarning << "Multiple helicity subsystems defined! "
                << "Using " << fHelicitySubsystem->GetSubsystemName() << "."
                << QwLog::endl;
    }
  }
  else {
    // Are we using fake helicity? Check.
    std::vector<VQwSubsystem*> subsys_fake_helicity = event.GetSubsystemByType("QwFakeHelicity");
    if (subsys_fake_helicity.size() > 0) {
      fHelicitySubsystem = dynamic_cast<QwHelicity*>(subsys_fake_helicity.at(0));
      fPatternSize = fHelicitySubsystem->GetMaxPatternPhase();

      QwWarning<<""<<QwLog::endl;
      QwWarning<<"USING FAKE HELICITY!"<< QwLog::endl;
      QwWarning<<""<<QwLog::endl;


    } else {
      // We are not usng any helicity subsystem
      QwError << "No helicity subsystem defined!  Brace for impact!" << QwLog::endl;
      fPatternSize = 4; // default to quartets
    }
  }
  QwMessage << "QwHelicity::MaxPatternPhase = " << fPatternSize << QwLog::endl;

  // Enable burst sum and running sum by default
  fEnableBurstSum = kTRUE;
  fEnableRunningSum = kTRUE;

  // Currently the alternate asym works with quartets only

  fEnableAlternateAsym = kFALSE;

  if (fPatternSize != 4)
    fEnableAlternateAsym = kFALSE;

  try
    {
      if(fPatternSize%2 == 0)
        {
          fEvents.resize(fPatternSize);
          for(int i=0;i<fPatternSize;i++)
            {
              fHelicity.push_back(-9999);
              fEventLoaded.push_back(kFALSE);
              fEvents[i].Copy(&event);
              fEventNumber.push_back(-1);
            }

          // Initialize the quartet number
          fQuartetNumber = 0;

          fCurrentPatternNumber = -1;
          fPositiveHelicitySum.Copy(&event);
          fNegativeHelicitySum.Copy(&event);
          fDifference.Copy(&event);

          fYield.Copy(&event);
          fAsymmetry.Copy(&event);
          if (fEnableAlternateAsym) {
            fAsymmetry1.Copy(&event);
            fAsymmetry2.Copy(&event);
          }

          fBurstYield.Copy(&event);
          fBurstDifference.Copy(&event);
          fBurstAsymmetry.Copy(&event);

          fRunningYield.Copy(&event);
          fRunningDifference.Copy(&event);
          fRunningAsymmetry.Copy(&event);
          if (fEnableAlternateAsym) {
            fRunningAsymmetry1.Copy(&event);
            fRunningAsymmetry2.Copy(&event);
          }

          fRunningBurstYield.Copy(&event);
          fRunningBurstDifference.Copy(&event);
          fRunningBurstAsymmetry.Copy(&event);

          ClearEventData();
          ClearBurstSum();
          ClearRunningSum();
        }
      else
        {
          TString loc=
            "Standard exception from QwHelicityPattern : the pattern size has to be even;  right now pattern_size=";
          loc+=Form("%d",fPatternSize);
          throw std::invalid_argument(loc.Data());
        }
    }
  catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }

};


void QwHelicityPattern::ClearEventData()
{
  for(size_t i=0; i<fEvents.size(); i++)
    {
      fEvents[i].ClearEventData();
      fEventLoaded[i]=kFALSE;
      fHelicity[i]=-999;
    }

  fYield.ClearEventData();
  fAsymmetry.ClearEventData();
  if (fEnableAlternateAsym){
    fAsymmetry1.ClearEventData();
    fAsymmetry2.ClearEventData();
  }

  fPositiveHelicitySum.ClearEventData();
  fNegativeHelicitySum.ClearEventData();
  fDifference.ClearEventData();
  IsGood=kFALSE;
  return;
};

/////////////////////////////////////////////////////////////////////

void QwHelicityPattern::LoadEventData(QwSubsystemArrayParity &event)
{

  Bool_t localdebug = kFALSE;
  IsGood = kFALSE;
  Long_t localPatternNumber  = fHelicitySubsystem->GetPatternNumber();
  Int_t  localPhaseNumber    = fHelicitySubsystem->GetPhaseNumber();
  Int_t  localHelicityActual = fHelicitySubsystem->GetHelicityActual();
  Long_t localEventNumber    = fHelicitySubsystem->GetEventNumber();

  if(localdebug) {
    std::cout<<"\n ###################################\n";
    std::cout<<"QwHelicityPattern::LoadEventData :: ";
    std::cout<<" event, pattern, phase # "<<localEventNumber<<" "<<localPatternNumber<<" "<<localPhaseNumber<<"\n";
    std::cout<<" helicity ="<< localHelicityActual<<"\n";
    for(size_t i=0; i<fEvents.size(); i++)
      std::cout<<i<<":"<<fEventLoaded[i]<<"  ";
    std::cout<<"\n";
  }
  if(fCurrentPatternNumber!=localPatternNumber)
    {
      // new pattern
      ClearEventData();
      fCurrentPatternNumber=localPatternNumber;
    }
  if(localPhaseNumber>fPatternSize)
    {
      std::cerr<<" In QwHelicityPattern::LoadEventData trying upload an event with a phase larger than expected \n";
      std::cerr<<" phase ="<<localPhaseNumber<<" maximum expected phase="<<fPatternSize<<"\n";
      std::cerr<<" operation impossible, pattern reset to 0: no asymmetries will be computed \n";
      ClearEventData();
    }
  else
    {
      Int_t locali=localPhaseNumber-1;

      if(localdebug) std::cout<<"QwHelicityPattern::LoadEventData local i="<<locali<<"\n";
      if (locali < 0) {
        QwError << "Negative array index set to zero!  Check code!" << QwLog::endl;
        locali = 0;
      }
      fEvents[locali] = event;
      fEventLoaded[locali] = kTRUE;
      fHelicity[locali] = localHelicityActual;
      fEventNumber[locali] = localEventNumber;
    }

  if(localdebug)
    Print();


  return;
};

/////////////////////////////////////////////////////////////////////

Bool_t  QwHelicityPattern::IsCompletePattern() const
{
  Bool_t localdebug=kFALSE;
  Bool_t filled=kTRUE;
  Int_t i=fPatternSize-1;
  while(filled && i>-1)
    {
      if (localdebug){
        std::cout<<" i="<<i<<" is loaded ?"
                 <<fEventLoaded[fEvents.size()-i-1]<<"\n";
      }
      if(!fEventLoaded[i])
        filled=kFALSE;
      i--;
    }



  return filled;
}

/////////////////////////////////////////////////////////////////////
void  QwHelicityPattern::CalculateAsymmetry()
{

  Bool_t localdebug=kFALSE;
  if(localdebug)  std::cout<<"Entering QwHelicityPattern::CalculateAsymmetry \n";

  Int_t plushel  = 1;
  Int_t minushel = 0;
  Int_t checkhel = 0;
  Bool_t firstplushel=kTRUE;
  Bool_t firstminushel=kTRUE;

  fPositiveHelicitySum.ClearEventData();
  fNegativeHelicitySum.ClearEventData();

  for (size_t i = 0; i < (size_t) fPatternSize; i++)
    {
      if (fHelicity[i] == plushel)
        {
          if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: here filling fPositiveHelicitySum \n";
          if (firstplushel)
            {
              if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with = \n";
              fPositiveHelicitySum = fEvents.at(i);
              firstplushel = kFALSE;
            }
          else
            {
              if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with += \n";
              fPositiveHelicitySum += fEvents.at(i);
            }
          checkhel += 1;
        }
      else if (fHelicity[i] == minushel)
        {
          if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: here filling fNegativeHelicitySum \n";
          if (firstminushel)
            {
              if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with = \n";
              fNegativeHelicitySum = fEvents.at(i);
              firstminushel = kFALSE;
            }
          else
            {
              if (localdebug) std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with += \n";
              fNegativeHelicitySum += fEvents.at(i);
            }
          checkhel -= 1;
        }
      else
        {
          std::cerr<<" QwHelicityPattern::CalculateAsymmetry ==";
          std::cerr<<" Helicity should be "<<plushel<<" or "<<minushel<<" but is"<< fHelicity[i];
          std::cerr<<" Asymmetry computation aborted \n";
          ClearEventData();
          i = fPatternSize;
          checkhel = -9999;
          // This is an unknown helicity event.
        }
    }
  if (checkhel == -9999)
    {
      //do nothing the asymmetry computation has been aborted earlier in this function
      IsGood = kFALSE;
    }
  else if (checkhel!=0)
    {
      IsGood = kFALSE;
      // there is a different number of plus and minus helicity window.
      std::cerr<<" QwHelicityPattern::CalculateAsymmetry == \n";
      std::cerr<<" you do not have the same number of positive and negative \n";
      std::cerr<<" impossible to compute assymetry \n";
      std::cerr<<" dropping every thing -- pattern number ="<<fCurrentPatternNumber<<"\n";
    }
  else
    {

      IsGood = kTRUE;
      fQuartetNumber++;//Then increment the quartet number
      //std::cout<<" quartet count ="<<fQuartetNumber<<"\n";


      fYield.Sum(fPositiveHelicitySum,fNegativeHelicitySum);
      fDifference.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
      fBlinder.Blind(fDifference,fYield);
      fAsymmetry.Ratio(fDifference,fYield);

      /*
        With additional two asymmetry calculations

        quartet pattern + - - +
                        1 2 3 4
                        fAsymmetry  = (1+4)-(2+3)/(1+2+3+4)
                        fAsymmetry1 = (1+2)-(3+4)/(1+2+3+4)
                        fAsymmetry2 = (1+3)-(2+4)/(1+2+3+4)
      */

      if (fEnableAlternateAsym) {
        fPositiveHelicitySum.ClearEventData();
        fNegativeHelicitySum.ClearEventData();
        fPositiveHelicitySum  = fEvents.at(0);
        fPositiveHelicitySum += fEvents.at(1);
        fNegativeHelicitySum  = fEvents.at(2);
        fNegativeHelicitySum += fEvents.at(3);
        fDifference.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
        fBlinder.Blind(fDifference,fYield);
        fAsymmetry1.Ratio(fDifference,fYield);

        fPositiveHelicitySum.ClearEventData();
        fNegativeHelicitySum.ClearEventData();
        fPositiveHelicitySum  = fEvents.at(0);
        fPositiveHelicitySum += fEvents.at(2);
        fNegativeHelicitySum  = fEvents.at(1);
        fNegativeHelicitySum += fEvents.at(3);
        fDifference.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
        fBlinder.Blind(fDifference,fYield);
        fAsymmetry2.Ratio(fDifference,fYield);
      }

      // Accumulate the burst and running sums
      if (fEnableBurstSum) AccumulateBurstSum();
      if (fEnableRunningSum) AccumulateRunningSum();


      if (localdebug) QwDebug << " pattern number =" << fQuartetNumber << QwLog::endl;
    }

  return;
};

//*****************************************************************
/**
 * Clear the running sums of yield, difference and asymmetry.
 * Also clear the running burst sums if enabled.
 */
void  QwHelicityPattern::ClearRunningSum()
{
  if (fEnableRunningSum) {
    fRunningYield.ClearEventData();
    fRunningDifference.ClearEventData();
    fRunningAsymmetry.ClearEventData();
    if (fEnableAlternateAsym) {
      fRunningAsymmetry1.ClearEventData();
      fRunningAsymmetry2.ClearEventData();
    }
 }
  if (fEnableBurstSum) {
    fRunningBurstYield.ClearEventData();
    fRunningBurstDifference.ClearEventData();
    fRunningBurstAsymmetry.ClearEventData();
  }
}

//*****************************************************************
/**
 * Clear the burst sums of yield and difference.  No asymmetry
 * burst sum is used.
 */
void  QwHelicityPattern::ClearBurstSum()
{
  if (fEnableBurstSum) {
    fBurstYield.ClearEventData();
    fBurstDifference.ClearEventData();
  }
}

//*****************************************************************
/**
 * Accumulate the burst sum by adding this helicity pattern to the
 * burst sums of yield and difference.  There is no burst sum of
 * asymmetry, because that can only be calculated with meaningful
 * moments at the end of a burst.
 */
void  QwHelicityPattern::AccumulateBurstSum()
{
  fBurstYield.AccumulateRunningSum(fYield);
  fBurstDifference.AccumulateRunningSum(fDifference);
  // The difference is blinded, so the burst difference is also blinded.
}

//*****************************************************************
/**
 * Accumulate the running sum by adding this helicity pattern to the
 * running sums of yield, difference and asymmetry.
 */
void  QwHelicityPattern::AccumulateRunningSum()
{
  fRunningYield.AccumulateRunningSum(fYield);
  fRunningDifference.AccumulateRunningSum(fDifference);
  // The difference is blinded, so the running difference is also blinded.
  fRunningAsymmetry.AccumulateRunningSum(fAsymmetry);
  if (fEnableAlternateAsym) {
    fRunningAsymmetry1.AccumulateRunningSum(fAsymmetry1);
    fRunningAsymmetry2.AccumulateRunningSum(fAsymmetry2);
  }
}

//*****************************************************************
/**
 * Accumulate the running burst sum by adding the current burst sum
 * to the running sums of burst yield, difference and asymmetry.
 */
void  QwHelicityPattern::AccumulateRunningBurstSum()
{
  // Accumulate the burst yield and difference
  fRunningBurstYield.AccumulateRunningSum(fBurstYield);
  fRunningBurstDifference.AccumulateRunningSum(fBurstDifference);
  // The burst difference is blinded, so the running burst difference is also blinded.

  // Calculate asymmetry over this entire burst
  fBurstAsymmetry.Ratio(fBurstDifference, fBurstYield);
  // Accumulate this burst asymmetry
  fRunningBurstAsymmetry.AccumulateRunningSum(fBurstAsymmetry);

  // Be sure to clear the burst sums after this function!
}

//*****************************************************************
/**
 * Calculate the average burst yield, difference and asymmetry.
 */
void  QwHelicityPattern::CalculateBurstAverage()
{
  fBurstAsymmetry.CalculateRunningAverage();
  fBurstDifference.CalculateRunningAverage();
  fBurstYield.CalculateRunningAverage();
}

//*****************************************************************
/**
 * Calculate the average running burst yield, difference and asymmetry.
 */
void  QwHelicityPattern::CalculateRunningBurstAverage()
{
  fRunningBurstAsymmetry.CalculateRunningAverage();
  fRunningBurstDifference.CalculateRunningAverage();
  fRunningBurstYield.CalculateRunningAverage();
}

//*****************************************************************
void  QwHelicityPattern::CalculateRunningAverage()
{
  fRunningAsymmetry.CalculateRunningAverage();
  fRunningDifference.CalculateRunningAverage();
  fRunningYield.CalculateRunningAverage();
}

//*****************************************************************
void  QwHelicityPattern::PrintRunningBurstAverage() const
{
  QwMessage << " Running burst average of asymmetry" << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fRunningBurstAsymmetry.PrintValue();

  QwMessage << " Running burst average of difference" << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fRunningBurstDifference.PrintValue();

  QwMessage << " Running burst average of yields" << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fRunningBurstYield.PrintValue();
}

//*****************************************************************
void  QwHelicityPattern::PrintRunningAverage() const
{
  QwMessage << " Running average of asymmetry  " << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fRunningAsymmetry.PrintValue();

  QwMessage << " Running average of difference " << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fRunningDifference.PrintValue();

  QwMessage << " Running average of yields     " << QwLog::endl;
  QwMessage << " ==============================" << QwLog::endl;
  fRunningYield.PrintValue();
}

//*****************************************************************
void  QwHelicityPattern::ConstructHistograms(TDirectory *folder)
{
  //  std::cout<<"QwHelicityPattern::ConstructHistograms \n";
  TString prefix="yield_";
  fYield.ConstructHistograms(folder,prefix);
  prefix="asym_";
  fAsymmetry.ConstructHistograms(folder,prefix);

  if (fEnableAlternateAsym) {
    prefix="asym1_";
    fAsymmetry1.ConstructHistograms(folder,prefix);
    prefix="asym2_";
    fAsymmetry2.ConstructHistograms(folder,prefix);
  }
  //prefix="HelPLUS_";
  //fPositiveHelicitySum.ConstructHistograms(folder,prefix);
  //prefix="HelNEG_";
  //fNegativeHelicitySum.ConstructHistograms(folder,prefix);
  return;
}

void  QwHelicityPattern::FillHistograms()
{
  //std::cout<<"QwHelicityPattern::FillHistograms ";
  if(IsGood)
    {
      //std::cout<<"************ YIELD ************\n";
      fYield.FillHistograms();
      //  std::cout<<"************ ASYMMETRY ************\n";
      fAsymmetry.FillHistograms();
      if (fEnableAlternateAsym){
        fAsymmetry1.FillHistograms();
        fAsymmetry2.FillHistograms();
      }

      //fPositiveHelicitySum.FillHistograms();
      //fNegativeHelicitySum.FillHistograms();
    }
  //std::cout<<"\n";
  return;
}

void  QwHelicityPattern::DeleteHistograms()
{
  fYield.DeleteHistograms();
  fAsymmetry.DeleteHistograms();
  if (fEnableAlternateAsym){
    fAsymmetry1.DeleteHistograms();
    fAsymmetry2.DeleteHistograms();
  }
  return;
}

void QwHelicityPattern::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  TString yieldprefix = "yield_" + prefix;
  fYield.ConstructBranchAndVector(tree, yieldprefix, values);

  TString asymprefix = "asym_" + prefix;
  fAsymmetry.ConstructBranchAndVector(tree, asymprefix, values);

  if (fEnableAlternateAsym){
    asymprefix = "asym1_" + prefix;
    fAsymmetry1.ConstructBranchAndVector(tree, asymprefix, values);
    asymprefix = "asym2_" + prefix;
    fAsymmetry2.ConstructBranchAndVector(tree, asymprefix, values);
  }
  return;
}

void QwHelicityPattern::FillTreeVector(std::vector<Double_t> &values)
{
  if (IsGood) {
    fYield.FillTreeVector(values);
    fAsymmetry.FillTreeVector(values);

    if (fEnableAlternateAsym){
      fAsymmetry1.FillTreeVector(values);
      fAsymmetry2.FillTreeVector(values);
    }
  }
  return;
};


void QwHelicityPattern::FillDB(QwDatabase *db)
{
  //if (IsGood()) {

  fYield.FillDB(db, "yield");
  fAsymmetry.FillDB(db, "asymmetry");

  //fYield.FillDB(db, "yield");
  //fAsymmetry.FillDB(db, "asymmetry");
  fRunningYield.FillDB(db, "yield");
  fRunningAsymmetry.FillDB(db, "asymmetry");
  if (fEnableAlternateAsym) {
    fAsymmetry1.FillDB(db, "asymmetry1");
    fAsymmetry2.FillDB(db, "asymmetry2");
  }

  //} else {
  //  QwWarning << "Not good!" << QwLog::endl;
  //}

  return;
};

//*****************************************************************

void QwHelicityPattern::Print() const
{
   std::cout<<"\n Pattern number ="<<fCurrentPatternNumber<<"\n";
  for(size_t i=0; i< (size_t) fPatternSize; i++)
    std::cout<<"event "<<fEventNumber[i]<<":"<<fEventLoaded[i]<<", "<<fHelicity[i]<<"\n";
  std::cout<<"Is a complete pattern ?(n/y:0/1) "<<IsCompletePattern()<<"\n";

}
