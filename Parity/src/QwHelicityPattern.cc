/**********************************************************\
* File: QwHelicityPattern.cc                              *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwHelicityPattern.h"
#include "QwHistogramHelper.h"
#include <stdexcept>


extern QwHistogramHelper gQwHists;

/*****************************************************************/
QwHelicityPattern::QwHelicityPattern(QwSubsystemArray &event, Int_t pattern_size)
{
  try
    {
      if(pattern_size%2 == 0)
	{
	  fEvents.resize(pattern_size);
	  for(int i=0;i<pattern_size;i++)
	    {
	      fFakeHelicity.push_back(2*(i%2)-1);
	      fEventLoaded.push_back(kFALSE);
	      fEvents[i].Copy(&event);
	    }
	  fYield.Copy(&event);
	  fAsymmetry.Copy(&event);
	  pos_sum.Copy(&event);
	  neg_sum.Copy(&event);
	  difference.Copy(&event);
	  ClearEventData();
	  fAsymSoFar=0;
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
    }

  fYield.ClearEventData();
  fAsymmetry.ClearEventData();
  pos_sum.ClearEventData();
  neg_sum.ClearEventData();
  difference.ClearEventData();

  return;
};


void QwHelicityPattern::LoadEventData(QwSubsystemArray &event)
{
  Bool_t localdebug=kFALSE;

  if(localdebug) {
    std::cout<<" in QwHelicityPattern::LoadEventData \n";
    for(size_t i=0; i<fEvents.size(); i++)
      std::cout<<i<<":"<<fEventLoaded[i]<<"  ";
    std::cout<<"\n";
  }
  for(size_t i=0; i<fEvents.size(); i++)
    {
      if(!fEventLoaded[i])
	{
	  if(localdebug) std::cout<<" Loading event "<<i<<"\n";
	  fEvents[i]=event;
	  fEventLoaded[i]=kTRUE;
	  i=fEvents.size()+1;

	}
    }
  if(localdebug) {  
    for(size_t i=0; i<fEvents.size(); i++)
      std::cout<<"event "<<i<<":"<<fEventLoaded[i]<<"  ";
    std::cout<<"\n \n \n";
  }

  return;
};   

Bool_t  QwHelicityPattern::IsCompletePattern()
{
  Bool_t filled=kTRUE;

  for(size_t i=0; i<fEvents.size(); i--)
    {
//       std::cout<<" i="<<i<<" is loaded ?"
// 	       <<fEventLoaded[fEvents.size()-i-1]<<"\n";
      if(!fEventLoaded[fEvents.size()-i-1])
	{
	  i=fEvents.size()+1;
	  filled=kFALSE;
	}
    }
  
  return filled;
}


void  QwHelicityPattern::CalculateAsymmetry()
{
  
  pos_sum.ClearEventData();
  neg_sum.ClearEventData();

// this what it will look like ultimately. For now fake the Helicity
//   for (size_t i=0; i<fEvents.length(); i++){
//     if (fEvents.at(i).IsPositiveHelicity()){
//       pos_sum += fEvents.at(i);
//     } else if (fEvents.at(i).IsNegativeHelicity()){
//       neg_sum += fEvents.at(i);
//     } else {
//       // This is an unknown helicity event.
//     }
//   }

  for (size_t i=0; i<fEvents.size(); i++)
    {
      if (fFakeHelicity[i]==+1)
	{
	  pos_sum += fEvents.at(i);
	} 
      else if (fFakeHelicity[i]==-1)
	{
	  neg_sum += fEvents.at(i);
	} 
      else 
	{
	  std::cerr<<" QwHelicityPattern::CalculateAsymmetry ==";
	  std::cerr<<" Helicity unknown.... Helicity should be +1 or -1 but is"<< fFakeHelicity[i]<<"\n";
	  // This is an unknown helicity event.
	}
    }
  
  fYield.Sum(pos_sum,neg_sum);
  difference.Difference(pos_sum,neg_sum);
  fAsymmetry.Ratio(difference,fYield);
  return;
};


void  QwHelicityPattern::ConstructHistograms(TDirectory *folder)
{
  TString prefix="yield_";
  fYield.ConstructHistograms(folder,prefix);
  prefix="asym_";
  fAsymmetry.ConstructHistograms(folder,prefix);
  return;
}

void  QwHelicityPattern::FillHistograms()
{
  fYield.FillHistograms();
  fAsymmetry.FillHistograms();
  return;
}

void  QwHelicityPattern::DeleteHistograms()
{
  fYield.DeleteHistograms();
  fAsymmetry.DeleteHistograms();
  return;
}



