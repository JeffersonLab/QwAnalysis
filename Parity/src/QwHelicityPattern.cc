
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
: fBlinder(TString("No seed string defined!"),kTRUE)
{
  bAlternateAsym = kFALSE;
  QwHelicity* input = ((QwHelicity*)event.GetSubsystem("Helicity info"));
  fPatternSize = input->GetMaxPatternPhase();

  std::cout<<"QwHelicity::MaxPatternPhase = "<<fPatternSize<<std::endl;

  if (fPatternSize != 4)//currently the alternate asym works with quartets only
    bAlternateAsym = kFALSE;

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
          fDiff.Copy(&event);

          fYield.Copy(&event);
          fAsymmetry.Copy(&event);
          if (bAlternateAsym) {
            fAsymmetry1.Copy(&event);
            fAsymmetry2.Copy(&event);
          }

          fRunningSumYield.Copy(&event);
          fRunningSumAsymmetry.Copy(&event);

          fRunningSumYield.ClearEventData();
          fRunningSumAsymmetry.ClearEventData();

          ClearEventData();
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
  if (bAlternateAsym){
    fAsymmetry1.ClearEventData();
    fAsymmetry2.ClearEventData();
  }

  //fRunningSumYield.ClearEventData();
  //fRunningSumAsymmetry.ClearEventData();

  fPositiveHelicitySum.ClearEventData();
  fNegativeHelicitySum.ClearEventData();
  fDiff.ClearEventData();
  IsGood=kFALSE;
  return;
};

/////////////////////////////////////////////////////////////////////

void QwHelicityPattern::LoadEventData(QwSubsystemArrayParity &event)
{

  Bool_t localdebug=kFALSE;
  QwHelicity* input=((QwHelicity*)event.GetSubsystem("Helicity info"));
  IsGood=kFALSE;
  Long_t localPatternNumber=input->GetPatternNumber();
  Int_t localPhaseNumber=input->GetPhaseNumber();
  Int_t localHelicityActual=input->GetHelicityActual();
  Long_t localEventNumber=input->GetEventNumber();
  /*
  Long_t localPatternNumber=1;//input->GetPatternNumber();
  Int_t localPhaseNumber=1;//input->GetPhaseNumber();
  Int_t localHelicityActual=1;//input->GetHelicityActual();
  Long_t localEventNumber=1;//input->GetEventNumber();
  */
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

Bool_t  QwHelicityPattern::IsCompletePattern()
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
      fDiff.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
      fBlinder.Blind(fDiff,fYield);
      fAsymmetry.Ratio(fDiff,fYield);


      /*
        With additional two asymmetry calculations

        quartet pattern + - - +
                        1 2 3 4
                        fAsymmetry  = (1+4)-(2+3)/(1+2+3+4)
                        fAsymmetry1 = (1+2)-(3+4)/(1+2+3+4)
                        fAsymmetry2 = (1+3)-(2+4)/(1+2+3+4)

      */

      if (bAlternateAsym){
        fPositiveHelicitySum.ClearEventData();
        fNegativeHelicitySum.ClearEventData();
        fPositiveHelicitySum  = fEvents.at(0);
        fPositiveHelicitySum += fEvents.at(1);
        fNegativeHelicitySum  = fEvents.at(2);
        fNegativeHelicitySum += fEvents.at(3);
        fDiff.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
        fBlinder.Blind(fDiff,fYield);
        fAsymmetry1.Ratio(fDiff,fYield);

        fPositiveHelicitySum.ClearEventData();
        fNegativeHelicitySum.ClearEventData();
        fPositiveHelicitySum  = fEvents.at(0);
        fPositiveHelicitySum += fEvents.at(2);
        fNegativeHelicitySum  = fEvents.at(1);
        fNegativeHelicitySum += fEvents.at(3);
        fDiff.Difference(fPositiveHelicitySum,fNegativeHelicitySum);
        fBlinder.Blind(fDiff,fYield);
        fAsymmetry2.Ratio(fDiff,fYield);

      }

      fRunningSumYield.AccumulateRunningSum(fYield);
      fRunningSumAsymmetry.AccumulateRunningSum(fAsymmetry);
      //      fRunningSumAsymmetry1.AccumulateRunningSum(fAsymmetry1);
      //      fRunningSumAsymmetry2.AccumulateRunningSum(fAsymmetry2);


      if (localdebug) std::cout<<" pattern number ="<<fQuartetNumber<<"\n";
    }

  return;
};

//*****************************************************************
void  QwHelicityPattern::CalculateRunningAverage()
{
  std::cout<<" Running average of asymmetry "<<std::endl;
  std::cout<<" =============================="<<std::endl;
  fRunningSumAsymmetry.CalculateRunningAverage();

  if (bAlternateAsym) {
    std::cout<<" Running average of asymmetry1 "<<std::endl;
    std::cout<<" =============================="<<std::endl;
    fAsymmetry1.CalculateRunningAverage();

    std::cout<<" Running average of asymmetry2 "<<std::endl;
    std::cout<<" =============================="<<std::endl;
    fAsymmetry2.CalculateRunningAverage();
  }

  std::cout<<" Running average of Yields "<<std::endl;
  std::cout<<" =============================="<<std::endl;
  fRunningSumYield.CalculateRunningAverage();

  std::cout<<" Blinder info "<<std::endl;
  std::cout<<" ============ "<<std::endl;
  fBlinder.PrintFinalValues();
};

//*****************************************************************
void  QwHelicityPattern::ConstructHistograms(TDirectory *folder)
{
  //  std::cout<<"QwHelicityPattern::ConstructHistograms \n";
  TString prefix="yield_";
  fYield.ConstructHistograms(folder,prefix);
  prefix="asym_";
  fAsymmetry.ConstructHistograms(folder,prefix);

  if (bAlternateAsym){
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
      if (bAlternateAsym){
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
  if (bAlternateAsym){
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

  if (bAlternateAsym){
    asymprefix = "asym1_" + prefix;
    fAsymmetry1.ConstructBranchAndVector(tree, asymprefix, values);
    asymprefix = "asym2_" + prefix;
    fAsymmetry2.ConstructBranchAndVector(tree, asymprefix, values);
  }

//   //  std::cout<<"QwHelicityPattern::ConstructBranchAndVector\n";
//   ((QwBeamLine*)fYield.GetSubsystem("Injector Beamline Copy"))->ConstructBranchAndVector(tree,thisprefix,values);
//   ((QwHelicity*)fYield.GetSubsystem("Helicity Copy"))->ConstructBranchAndVector(tree,thisprefix,values);
//   thisprefix="yield";
//   ((QwMainCerenkovDetector*)fYield.GetSubsystem("Quartz bar Copy"))->ConstructBranchAndVector(tree,thisprefix,values);
//
//   thisprefix="asym_";
//   ((QwBeamLine*)fAsymmetry.GetSubsystem("Injector Beamline Copy"))->ConstructBranchAndVector(tree,thisprefix,values);
//   ((QwHelicity*)fAsymmetry.GetSubsystem("Helicity Copy"))->ConstructBranchAndVector(tree,thisprefix,values);
//   thisprefix="asym";
//   ((QwMainCerenkovDetector*)fAsymmetry.GetSubsystem("Quartz bar Copy"))->ConstructBranchAndVector(tree,thisprefix,values);

  //  the following lines are the syntax we want at the end :
  //  fYield.ConstructBranchAndVector(tree, prefix,values);
  //  fAsymmetry.ConstructBranchAndVector(tree, prefix,values);

  return;
}

void QwHelicityPattern::FillTreeVector(std::vector<Double_t> &values)
{
  if (IsGood) {
    fYield.FillTreeVector(values);
    fAsymmetry.FillTreeVector(values);

    if (bAlternateAsym){
      fAsymmetry1.FillTreeVector(values);
      fAsymmetry2.FillTreeVector(values);
    }
  }
  return;
};


void QwHelicityPattern::FillDB(QwDatabase *db)
{
  //  if (IsGood) {
    fYield.FillDB(db, "yield");
    fAsymmetry.FillDB(db, "asymmetry");

    if (bAlternateAsym){
      // not yet determine to use
      //      fAsymmetry1.FillDB(db, "asymmetry1");
      //      fAsymmetry2.FillDB(db, "asymmetry2").;

    }

    //  }

  return;
}
//*****************************************************************

void QwHelicityPattern::Print()
{
   std::cout<<"\n Pattern number ="<<fCurrentPatternNumber<<"\n";
  for(size_t i=0; i< (size_t) fPatternSize; i++)
    std::cout<<"event "<<fEventNumber[i]<<":"<<fEventLoaded[i]<<", "<<fHelicity[i]<<"\n";
  std::cout<<"Is a complete pattern ?(n/y:0/1) "<<IsCompletePattern()<<"\n";

}
