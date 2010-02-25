
#include "QwCombinedPMT.h"
#include "QwHistogramHelper.h"
#include <stdexcept>



void QwCombinedPMT::Add(QwIntegrationPMT* pmt, Double_t weight  )
{
//std::cout<<"QwCombinedPMT: Got "<<pmt->GetElementName()<<"  and weight ="<<weight<<"\n";
  fElement.push_back(pmt);
  fWeights.push_back(weight);
}


void  QwCombinedPMT::InitializeChannel(TString name, TString datatosave)
{
  SetElementName(name);
  //SetPedestal(0.);
  //SetCalibrationFactor(1.);

  std::cout<<"initialized combined PMT channel "<< GetElementName()<<std::endl;

  return;
};

void  QwCombinedPMT::LinkChannel(TString name)
{
  SetElementName(name);
  TString sum = name+TString("_sum");
  TString avg = name+TString("_avg");
  fSumADC= new QwIntegrationPMT(sum);
  fAvgADC= new QwIntegrationPMT(avg);
  //std::cout<<"linked combined PMT channel "<< GetElementName()<<std::endl;
}

void QwCombinedPMT::ClearEventData()
{

  fSumADC->ClearEventData();
  fAvgADC->ClearEventData();
}


void QwCombinedPMT::ReportErrorCounters()
{

};


void QwCombinedPMT::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{

};

void QwCombinedPMT::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fSumADC->SetEventData(block, sequencenumber);
  fAvgADC->SetEventData(block, sequencenumber);
};

void QwCombinedPMT::CalculateAverage()
{

  Bool_t ldebug = kFALSE;
  Double_t  total_weights=0.0;

  ClearEventData();
  QwIntegrationPMT *tmpADC;

  for (size_t i=0;i<fElement.size();i++)
    {
      tmpADC = fElement[i];
      tmpADC->Scale(fWeights[i]);
      *fSumADC += *tmpADC;
      total_weights += fWeights[i];
    }

  *fAvgADC = *fSumADC;

  if (total_weights!=0.0)
    fAvgADC->Scale(1/total_weights);

  if (ldebug)
    {
      std::cout<<"QwCombinedPMT::CalculateAverage()"<<std::endl;
      fAvgADC->Print();
      fSumADC->Print();

      std::cout<<"QwCombinedPMT: "<<GetElementName()
      <<"\nweighted average of hardware sums = "<<fAvgADC->GetHardwareSum()<<"\n";
      for (size_t i=0;i<4;i++)
        {
          std::cout<<"weighted average of block["<<i<<"] = "<<fSumADC->GetBlockValue(i)<<"\n";
        }
    }

}


Int_t QwCombinedPMT::SetSingleEventCuts(std::vector<Double_t> & dEventCuts)
{

  //fSumADC->SetSingleEventCuts(dEventCuts);
  //fAvgADC->->SetSingleEventCuts(dEventCuts);

  return 1;
}


void  QwCombinedPMT::ProcessEvent()
{

  CalculateAverage(); //Calculate the weigted averages of the hardware sum and each of the four blocks.

  return;
};


void QwCombinedPMT::SetDefaultSampleSize(Int_t sample_size)
{

}



// report number of events falied due to HW and event cut faliure
Int_t QwCombinedPMT::GetEventcutErrorCounters()
{

  return 1;
}


void QwCombinedPMT::Calculate_Running_Average()
{
  //fIntegrationPMT.Calculate_Running_Average();
};


void QwCombinedPMT::Do_RunningSum()
{
  //fIntegrationPMT.Do_RunningSum();
};


Int_t QwCombinedPMT::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement)
{
  return 0;
};


QwCombinedPMT& QwCombinedPMT::operator= (const QwCombinedPMT &value)
{
  if (GetElementName()!="" && this->fElement.size()==value.fElement.size())
    {
      for (size_t i=0; i<fElement.size(); i++)
        {
          *(this->fElement[i]) += *(value.fElement[i]);
        }
    }
  return *this;
};

QwCombinedPMT& QwCombinedPMT::operator+= (const QwCombinedPMT &value)
{

  if (GetElementName()!="" && this->fElement.size()==value.fElement.size())
    {
      for (size_t i=0; i<fElement.size(); i++)
        {
          *(this->fElement[i]) += *(value.fElement[i]);
        }
    }
  return *this;
};

QwCombinedPMT& QwCombinedPMT::operator-= (const QwCombinedPMT &value)
{
  if (GetElementName()!="" && this->fElement.size()==value.fElement.size())
    {
      for (size_t i=0; i<fElement.size(); i++)
        {
          *(this->fElement[i])-=*(value.fElement[i]);
        }
    }
  return *this;
};


void QwCombinedPMT::Sum(QwCombinedPMT &value1, QwCombinedPMT &value2)
{
  if (GetElementName()!="" && this->fElement.size()==value1.fElement.size()
      && this->fElement.size()==value2.fElement.size() )
    *this =  value1;
  *this += value2;
};

void QwCombinedPMT::Difference(QwCombinedPMT &value1, QwCombinedPMT &value2)
{
  if (GetElementName()!="" && this->fElement.size()==value1.fElement.size()
      && this->fElement.size()==value2.fElement.size() )
    *this =  value1;
  *this -= value2;
};

void QwCombinedPMT::Ratio(QwCombinedPMT &numer, QwCombinedPMT &denom)
{

};

void QwCombinedPMT::Scale(Double_t factor)
{
  fSumADC->Scale(factor);
  fAvgADC->Scale(factor);
  return;
}


void QwCombinedPMT::Print() const
  {

    return;
  }


void  QwCombinedPMT::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      TString sumprefix = prefix+TString("");
      fSumADC->ConstructHistograms(folder, sumprefix);
      TString avgprefix = prefix+TString("");
      fAvgADC->ConstructHistograms(folder, avgprefix);
    }
  return;

};

void  QwCombinedPMT::FillHistograms()
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fSumADC->FillHistograms();
      fAvgADC->FillHistograms();
    }


  return;
};

void  QwCombinedPMT::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip
    }
  else
    {
      TString sumprefix = prefix+"";
      fSumADC->ConstructBranchAndVector(tree, sumprefix,values);
      TString avgprefix = prefix+"";
      fAvgADC->ConstructBranchAndVector(tree, avgprefix,values);
    }
  return;
};

void  QwCombinedPMT::FillTreeVector(std::vector<Double_t> &values)
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fSumADC->FillTreeVector(values);
      fAvgADC->FillTreeVector(values);
    }
  return;
};

void  QwCombinedPMT::DeleteHistograms()
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      //fSumADC->DeleteHistograms();
      //fAvgADC->DeleteHistograms();
    }
  return;
};
/********************************************************/
void  QwCombinedPMT::Copy(VQwDataElement *source)
{
  try
    {
      if (typeid(*source)==typeid(*this))
        {
          QwCombinedPMT* input=((QwCombinedPMT*)source);
          this->fElementName=input->fElementName;
          this->fCalibration=input->fCalibration;
          this->fIntegrationPMT.Copy(&(input->fIntegrationPMT));
        }
      else
        {
          TString loc="Standard exception from QwCombinedPMT::Copy = "
                      +source->GetElementName()+" "
                      +this->GetElementName()+" are not of the same type";
          throw std::invalid_argument(loc.Data());
        }
    }
  catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }

  return;
}


