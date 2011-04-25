
#include "QwCombinedPMT.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwDBInterface.h"

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
  if (datatosave=="raw") fDataToSave=kRaw;
  else
    if (datatosave=="derived") fDataToSave=kDerived;

  fSumADC.InitializeChannel(name, datatosave);
  SetBlindability(kTRUE);
  return;
}

void  QwCombinedPMT::InitializeChannel(TString subsystemname, TString name, TString datatosave)
{
  SetElementName(name);
  //SetPedestal(0.);
  //SetCalibrationFactor(1.);
  if (datatosave=="raw") fDataToSave=kRaw;
  else
    if (datatosave=="derived") fDataToSave=kDerived;

  fSumADC.InitializeChannel(subsystemname, "QwCombinedPMT", name, datatosave);
  SetBlindability(kTRUE);

  return;
}

void  QwCombinedPMT::LinkChannel(TString name)
{
  Bool_t local_debug = false;
  SetElementName(name);
//  TString sumstr = name+TString("_sum");
  TString sumstr = name+TString("");
  fSumADC.SetElementName(sumstr);
//   TString avgstr = name+TString("_avg");
//   fAvgADC.SetElementName(avgstr);
  if(local_debug) std::cout<<"linked combined PMT channel "<< GetElementName()<<std::endl;
}

void QwCombinedPMT::ClearEventData()
{
  fSumADC.ClearEventData();
//  fAvgADC.ClearEventData();
}


void QwCombinedPMT::ReportErrorCounters()
{

}


void QwCombinedPMT::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{

}

void QwCombinedPMT::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fSumADC.SetEventData(block, sequencenumber);
//  fAvgADC.SetEventData(block, sequencenumber);
}

void QwCombinedPMT::CalculateSumAndAverage()
{

  Bool_t ldebug = kFALSE;
  Double_t  total_weights=0.0;

  fSumADC.ClearEventData();
  static QwIntegrationPMT tmpADC("tmpADC");

  for (size_t i=0;i<fElement.size();i++)
    {
      //std::cout<<"=========fElement["<<i<<"]=========="<<std::endl;
      //fElement[i]->Print();
      tmpADC = *(fElement[i]);
      //std::cout<<"=========tmpADC========="<<std::endl;
      //tmpADC->Print();
      tmpADC.Scale(fWeights[i]);
      fSumADC += tmpADC;
      total_weights += fWeights[i];
    }

//   fAvgADC = fSumADC;
//   if (total_weights!=0.0)
//     fAvgADC.Scale(1/total_weights);

   if (total_weights!=0.0)
     fSumADC.Scale(1/total_weights);


  if (ldebug)
    {
      std::cout<<"QwCombinedPMT::CalculateAverage()"<<std::endl;
//      fAvgADC.PrintInfo();
      fSumADC.PrintInfo();

//      std::cout<<"QwCombinedPMT: "<<GetElementName()
//      <<"\nweighted average of hardware sums = "<<fAvgADC.GetHardwareSum()<<"\n";
      std::cout<<"QwCombinedPMT: "<<GetElementName()
      <<"\nweighted average of hardware sums = "<<fSumADC.GetHardwareSum()<<"\n";

      for (size_t i=0;i<4;i++)
        {
          std::cout<<"weighted average of block["<<i<<"] = "<<fSumADC.GetBlockValue(i)<<"\n";
        }
    }

}

/********************************************************/
Bool_t QwCombinedPMT::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;

 
  return fEventIsGood;
}
/********************************************************/

void QwCombinedPMT::SetSingleEventCuts(UInt_t errorflag, Double_t LL=0, Double_t UL=0, Double_t stability=0){
  //set the unique tag to identify device type (Int.PMT & Comb. PMT)
  //errorflag|=kPMTErrorFlag;
  QwMessage<<"QwCombinedPMT Error Code passing to QwIntegrationPMT "<<errorflag<<QwLog::endl;
  fSumADC.SetSingleEventCuts(errorflag,LL,UL,stability);
  
}


void  QwCombinedPMT::ProcessEvent()
{
//Calculate the weigted averages of the hardware sum and each of the four blocks.
  CalculateSumAndAverage();
  //fSumADC.ProcessEvent(); //This is not necessary for combined devices-Rakitha 11-15-2010

  return;
}


void QwCombinedPMT::SetDefaultSampleSize(Int_t sample_size)
{
  fSumADC.SetDefaultSampleSize((size_t)sample_size);
}

// report number of events falied due to HW and event cut faliure
Int_t QwCombinedPMT::GetEventcutErrorCounters()
{
  fSumADC.GetEventcutErrorCounters();
  return 1;
}

/********************************************************/
Bool_t QwCombinedPMT::ApplySingleEventCuts(){
  //  First update the error code based on the codes
  //  of the elements.  This requires that the single
  //  PMTs have had ApplySingleEventCuts run on them already.
  for (size_t i=0;i<fElement.size();i++){
    fSumADC.UpdateErrorCode(fElement.at(i)->GetErrorCode());
  }
  return fSumADC.ApplySingleEventCuts();
}

/********************************************************/

Int_t QwCombinedPMT::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement)
{
  return 0;
}


QwCombinedPMT& QwCombinedPMT::operator= (const QwCombinedPMT &value)
{
  //std::cout<<"Calling QwCombinedPMT::operator="<<std::endl;
  if ( (GetElementName()!="") && (this->fElement.size()==value.fElement.size()) )
    {
      for (size_t i=0; i<value.fElement.size(); i++)
        {
          //std::cout<<"value.fWeight["<<i<<"]="<<value.fWeights[i]<<std::endl;
          //std::cout<<"=========value.fElement["<<i<<"]=========="<<std::endl;
          //value.fElement[i]->Print();
          (this->fElement[i]) = (value.fElement[i]);
          this->fWeights[i] = value.fWeights[i];
          //std::cout<<"this->fWeight["<<i<<"]="<<fWeights[i]<<std::endl;
          //std::cout<<"=========this->fElement["<<i<<"]=========="<<std::endl;
          //fElement[i]->Print();
        }

      this->fSumADC=value.fSumADC;
//      this->fAvgADC=value.fAvgADC;

      //std::cout<<"value.fSumADC"<<std::endl;
      //value.fSumADC.Print();
      //std::cout<<"this->fSumADC"<<std::endl;
      //fSumADC.Print();
    }

  return *this;
}

QwCombinedPMT& QwCombinedPMT::operator+= (const QwCombinedPMT &value)
{
  //std::cout<<"Calling QwCombinedPMT::operator+="<<std::endl;
  if ( (GetElementName()!="") && (this->fElement.size()==value.fElement.size()) )
    {
      for (size_t i=0; i<value.fElement.size(); i++)
        {
          *(this->fElement[i]) += *(value.fElement[i]);
          this->fWeights[i] += value.fWeights[i];
        }

      this->fSumADC+=value.fSumADC;
//      this->fAvgADC+=value.fAvgADC;
    }

  return *this;
}

QwCombinedPMT& QwCombinedPMT::operator-= (const QwCombinedPMT &value)
{
  //std::cout<<"Calling QwCombinedPMT::operator-="<<std::endl;
  if ( (GetElementName()!="") && (this->fElement.size()==value.fElement.size()) )
    {
      for (size_t i=0; i<value.fElement.size(); i++)
        {
          *(this->fElement[i]) -= *(value.fElement[i]);
          this->fWeights[i] -= value.fWeights[i];
        }

      this->fSumADC-=value.fSumADC;
//      this->fAvgADC-=value.fAvgADC;
      //value.fSumADC.Print();
      //fSumADC.Print();
    }

  return *this;
}

void QwCombinedPMT::Sum(QwCombinedPMT &value1, QwCombinedPMT &value2)
{
  //std::cout<<"Calling QwCombinedPMT::Sum"<<std::endl;
  this->fSumADC =  value1.fSumADC;
//  this->fAvgADC =  value1.fAvgADC;
  this->fSumADC += value2.fSumADC;
//  this->fAvgADC += value2.fAvgADC;
}

void QwCombinedPMT::AccumulateRunningSum(const QwCombinedPMT& value)
{
  fSumADC.AccumulateRunningSum(value.fSumADC);
//  fAvgADC.AccumulateRunningSum(value.fAvgADC);
}

void QwCombinedPMT::Difference(QwCombinedPMT &value1, QwCombinedPMT &value2)
{
  //std::cout<<"Calling QwCombinedPMT::Difference="<<std::endl;
  this->fSumADC =  value1.fSumADC;
//  this->fAvgADC =  value1.fAvgADC;
  this->fSumADC -= value2.fSumADC;
//  this->fAvgADC -= value2.fAvgADC;
}

void QwCombinedPMT::Ratio(QwCombinedPMT &numer, QwCombinedPMT &denom)
{
  //std::cout<<"Calling QwCombinedPMT::Ratio"<<std::endl;
  fSumADC.Ratio(numer.fSumADC,denom.fSumADC);
//  fAvgADC.Ratio(numer.fAvgADC,denom.fAvgADC);
  return;
}

void QwCombinedPMT::Scale(Double_t factor)
{
  fSumADC.Scale(factor);
//  fAvgADC.Scale(factor);
  return;
}
void QwCombinedPMT::Normalize(VQwDataElement* denom)
{
  fSumADC.Normalize(denom);

  return;
}


void QwCombinedPMT::CalculateRunningAverage()
{
  fSumADC.CalculateRunningAverage();
//  fAvgADC.CalculateRunningAverage();
}

void QwCombinedPMT::Blind(const QwBlinder *blinder)
{
  fSumADC.Blind(blinder);
//  fAvgADC.Blind(blinder);
}

void QwCombinedPMT::Blind(const QwBlinder *blinder, const QwCombinedPMT& yield)
{
  fSumADC.Blind(blinder, yield.fSumADC);
//  fAvgADC.Blind(blinder, yield.fAvgADC);
}

void QwCombinedPMT::PrintValue() const
{
  fSumADC.PrintValue();
//  fAvgADC.PrintValue();
}

void QwCombinedPMT::PrintInfo() const
{
  fSumADC.PrintInfo();
//  fAvgADC.PrintInfo();
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
      fSumADC.ConstructHistograms(folder, sumprefix);
//      TString avgprefix = prefix+TString("");
//      fAvgADC.ConstructHistograms(folder, avgprefix);
    }
  return;

}

void  QwCombinedPMT::FillHistograms()
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fSumADC.FillHistograms();
//      fAvgADC.FillHistograms();
    }


  return;
}

void  QwCombinedPMT::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip
    }
  else
    {
      TString sumprefix =  prefix+"";
      fSumADC.ConstructBranchAndVector(tree, sumprefix,values);

    }
  return;
}

void  QwCombinedPMT::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip
    }
  else
    {
      TString sumprefix =  prefix+"";
      fSumADC.ConstructBranch(tree, sumprefix);
    }
  return;
}

void  QwCombinedPMT::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile &modulelist)
{
  TString devicename;
  devicename=GetElementName();
  devicename.ToLower();
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip
    }
  else
    {
      if (modulelist.HasValue(devicename)){
	TString sumprefix =  prefix+"";
	fSumADC.ConstructBranch(tree, sumprefix);
	QwMessage <<" Tree leave added to "<<devicename<<QwLog::endl;
      }
    }
  return;
}


void  QwCombinedPMT::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  } else {
    fSumADC.FillTreeVector(values);
//  fAvgADC.FillTreeVector(values);
  }
}

void  QwCombinedPMT::DeleteHistograms()
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fSumADC.DeleteHistograms();
//      fAvgADC.DeleteHistograms();
    }
  return;
}
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

          size_t element_size = input->fElement.size();
          this->fElement.resize(element_size);
          this->fWeights.resize(element_size);
          for (size_t i=0; i<element_size; i++)
            {
              fElement[i] = input->fElement[i];
              this->fWeights[i] = input->fWeights[i];

              //std::cout<<"ElementName="<<fElement[i]->GetElementName()
              //         <<" weight["<<i<<"]="<<fWeights[i]<<std::endl;
            }

          fSumADC.Copy(&(input->fSumADC));
//          fAvgADC.Copy(&(input->fAvgADC));
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

/********************************************************/
std::vector<QwDBInterface>  QwCombinedPMT::GetDBEntry()
{
  return fSumADC.GetDBEntry();
}
