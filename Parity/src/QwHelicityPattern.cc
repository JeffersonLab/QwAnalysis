
/**********************************************************\
* File: QwHelicityPattern.cc                              *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwHelicityPattern.h"
#include "QwHistogramHelper.h"
#include "QwHelicity.h"


#include <stdexcept>



/*****************************************************************/
QwHelicityPattern::QwHelicityPattern(QwSubsystemArrayParity &event, Int_t pattern_size)
{
  try
    {
      if(pattern_size%2 == 0)
	{
	  fEvents.resize(pattern_size);
	  for(int i=0;i<pattern_size;i++)
	    {
	      fHelicity.push_back(-9999);
	      fEventLoaded.push_back(kFALSE);
	      fEvents[i].Copy(&event);
	      fEventNumber.push_back(-1);
	    }
	  fQuartetNumber=0;//initialize the quartet number
	  fYield.Copy(&event);
	  fAsymmetry.Copy(&event);
	  fAverage.Copy(&event);
	  pos_sum.Copy(&event);
	  neg_sum.Copy(&event);
	  difference.Copy(&event);

	  fCurrentPatternNumber=-1;
	  fPatternSize=pattern_size;
	  ClearEventData();
	}
      else
	{
	  TString loc=
	    "Standard exception from QwHelicityPattern : the pattern size has to be even;  rigth now pattern_size=";
	  loc+=Form("%d",pattern_size);
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
  pos_sum.ClearEventData();
  neg_sum.ClearEventData();
  difference.ClearEventData();
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
      if (locali == -1) {
        std::cerr << "Negative array index set to zero!  Check code!" << std::endl;
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

  pos_sum.ClearEventData();
  neg_sum.ClearEventData();

  for (size_t i=0; i< (size_t) fPatternSize; i++)
    {
	if (fHelicity[i]==plushel)
	{
	  if(localdebug)  std::cout<<"QwHelicityPattern::CalculateAsymmetry:: here filling pos_sum \n";
	  if(firstplushel)
	    {
	      if(localdebug)  std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with = \n";
	      pos_sum=fEvents.at(i);
	      firstplushel=kFALSE;
	    }
	  else
	    {
	      if(localdebug)  std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with += \n";
	      pos_sum += fEvents.at(i);
	    }
	  checkhel+=1;
	}
      else if (fHelicity[i]==minushel)
	{
	  if(localdebug)  std::cout<<"QwHelicityPattern::CalculateAsymmetry:: here filling neg_sum \n";
	  if(firstminushel)
	    {
	      if(localdebug)  std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with = \n";
	      neg_sum = fEvents.at(i);
	      firstminushel=kFALSE;
	    }
	  else
	    {
	      if(localdebug)  std::cout<<"QwHelicityPattern::CalculateAsymmetry:: with += \n";
	      neg_sum += fEvents.at(i);
	    }
	  checkhel-=1;
	}
      else
	{
	  std::cerr<<" QwHelicityPattern::CalculateAsymmetry ==";
	  std::cerr<<" Helicity should be "<<plushel<<" or "<<minushel<<" but is"<< fHelicity[i];
	  std::cerr<<" Asymmetry computation aborted \n";
	  ClearEventData();
	  i=fPatternSize;
	  checkhel=-9999;
	  // This is an unknown helicity event.
	}
    }
  if(checkhel==-9999)
    {
      //do nothing the asymmetry computation has been aborted earlier in this function
      IsGood=kFALSE;
    }
  else if(checkhel!=0)
    {
      IsGood=kFALSE;
      // there is a different number of plus and minus helicity window.
      std::cerr<<" QwHelicityPattern::CalculateAsymmetry == \n";
      std::cerr<<" you do not have the same number of positive and negative \n";
      std::cerr<<" impossible to compute assymetry \n";
      std::cerr<<" dropping every thing -- pattern number ="<<fCurrentPatternNumber<<"\n";
    }
  else
    {

      IsGood=kTRUE;
      fQuartetNumber++;//Then increment the quartet number
      //std::cout<<" quartet count ="<<fQuartetNumber<<"\n";
	
      
      fYield.Sum(pos_sum,neg_sum);
      fYield.Do_RunningSum();
      difference.Difference(pos_sum,neg_sum);
      fAsymmetry.Ratio(difference,fYield);
      fAsymmetry.Do_RunningSum();
      if (localdebug) std::cout<<" pattern number ="<<fQuartetNumber<<"\n";
    }

  return;
};
//*****************************************************************
void  QwHelicityPattern::CalculateRunningAverage(){
  std::cout<<" Running average of asymmetry "<<std::endl;
  std::cout<<" =============================="<<std::endl;
  fAsymmetry.Calculate_Running_Average();
  std::cout<<" Running average of Yields "<<std::endl;
  std::cout<<" =============================="<<std::endl;
  fYield.Calculate_Running_Average();
};

//*****************************************************************
void  QwHelicityPattern::ConstructHistograms(TDirectory *folder)
{
  //  std::cout<<"QwHelicityPattern::ConstructHistograms \n";
  TString prefix="yield_";
  fYield.ConstructHistograms(folder,prefix);
  prefix="asym_";
  fAsymmetry.ConstructHistograms(folder,prefix);
  //prefix="HelPLUS_";
  //pos_sum.ConstructHistograms(folder,prefix);
  //prefix="HelNEG_";
  //neg_sum.ConstructHistograms(folder,prefix);
  return;
}

void  QwHelicityPattern::FillHistograms()
{
  //  std::cout<<"QwHelicityPattern::FillHistograms \n";
  if(IsGood)
    {
      //  std::cout<<"************ YIELD ************\n";
      fYield.FillHistograms();
      //  std::cout<<"************ ASYMMETRY ************\n";
      fAsymmetry.FillHistograms();
      //pos_sum.FillHistograms();
      //neg_sum.FillHistograms();
    }
  return;
}

void  QwHelicityPattern::DeleteHistograms()
{
  fYield.DeleteHistograms();
  fAsymmetry.DeleteHistograms();
  return;
}

void QwHelicityPattern::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  TString yieldprefix = "yield_" + prefix;
  fYield.ConstructBranchAndVector(tree, yieldprefix, values);

  TString asymprefix = "asym_" + prefix;
  fAsymmetry.ConstructBranchAndVector(tree, asymprefix, values);

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

//      ((QwBeamLine*)fYield.GetSubsystem("Injector Beamline Copy"))->FillTreeVector(values);
//      ((QwHelicity*)fYield.GetSubsystem("Helicity Copy"))->FillTreeVector(values);
//      ((QwMainCerenkovDetector*)fYield.GetSubsystem("Quartz bar Copy"))->FillTreeVector(values);
//
//      ((QwBeamLine*)fAsymmetry.GetSubsystem("Injector Beamline Copy"))->FillTreeVector(values);
//      ((QwHelicity*)fAsymmetry.GetSubsystem("Helicity Copy"))->FillTreeVector(values);
//      ((QwMainCerenkovDetector*)fAsymmetry.GetSubsystem("Quartz bar Copy"))->FillTreeVector(values);
  }

  return;
};

//*****************************************************************

void QwHelicityPattern::Print()
{
   std::cout<<"\n Pattern number ="<<fCurrentPatternNumber<<"\n";
  for(size_t i=0; i< (size_t) fPatternSize; i++)
    std::cout<<"event "<<fEventNumber[i]<<":"<<fEventLoaded[i]<<", "<<fHelicity[i]<<"\n";
  std::cout<<"Is a complete pattern ?(n/y:0/1) "<<IsCompletePattern()<<"\n";

}
