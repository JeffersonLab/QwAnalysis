/**********************************************************\
* File: QwBPMStripline.h                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwBPMStripline.h"
#include "QwHistogramHelper.h"
#include <stdexcept>



const Bool_t QwBPMStripline::kDEBUG = kFALSE;

/*! Position calibration factor, transform ADC counts in mm
value read in G0BeamMonitor on May 2008*/
const Double_t QwBPMStripline::kQwStriplineCalibration = 18.77;
const Double_t QwBPMStripline::kRotationCorrection = 1./1.414;
const TString QwBPMStripline::subelement[4]={"XP","XM","YP","YM"};
const TString QwBPMStripline::axis[3]={"X","Y","Z"};
/* With X being vertical up and Z being the beam direction toward the beamdump */

/********************************************************/
void  QwBPMStripline::InitializeChannel(TString name, Bool_t ROTATED)
{
  bRotated=ROTATED;
  SetOffset(0,0,0);
  for(int i=0;i<4;i++)
    {
      fWire[i].InitializeChannel(name+subelement[i],"raw");
      if(kDEBUG)
	std::cout<<" Wire ["<<i<<"]="<<fWire[i].GetElementName()<<"\n";
    }

  for(int i=0;i<2;i++)
	fRelPos[i].InitializeChannel(name+"Rel"+axis[i],"derived");

  for(int i=0;i<3;i++)
	fAbsPos[i].InitializeChannel(name+axis[i],"derived");

  SetElementName(name);
  bFullSave=kTRUE;

  return;
};
/********************************************************/
void QwBPMStripline::ClearEventData()
{
  for(int i=0;i<4;i++)
	fWire[i].ClearEventData();

  for(int i=0;i<2;i++)
	fRelPos[i].ClearEventData();

  for(int i=0;i<3;i++)
	fAbsPos[i].ClearEventData();

	return;
};
/********************************************************/
void QwBPMStripline::SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY)
{
  // Average values of the signals in the stripline ADCs
  Double_t sumX = 0.5e8; // These are just guesses, but I made X and Y different
  Double_t sumY = 1.5e8; // to make it more interesting for the analyzer...

  // Determine the asymmetry from the position
  Double_t meanXP = (1.0 + meanX / kQwStriplineCalibration) * sumX / 2.0;
  Double_t meanXM = (1.0 - meanX / kQwStriplineCalibration) * sumX / 2.0; // = sumX - meanXP;
  Double_t meanYP = (1.0 + meanY / kQwStriplineCalibration) * sumY / 2.0;
  Double_t meanYM = (1.0 - meanY / kQwStriplineCalibration) * sumY / 2.0; // = sumY - meanYP;

  // Determine the spread of the asymmetry (this is a guess)
  // (negative sigma should work in the QwVQWK_Channel, but still using fabs)
  Double_t sigmaXP = fabs(sumX * sigmaX / meanX);
  Double_t sigmaXM = sigmaXP;
  Double_t sigmaYP = fabs(sumY * sigmaY / meanY);
  Double_t sigmaYM = sigmaYP;

  // TODO No support for rotation here!  When reading generated files, there
  // could be some confusion because the analyzer WILL rotate the striplines.

  // Propagate these parameters to the ADCs
  fWire[0].SetRandomEventParameters(meanXP, sigmaXP);
  fWire[1].SetRandomEventParameters(meanXM, sigmaXM);
  fWire[2].SetRandomEventParameters(meanYP, sigmaYP);
  fWire[3].SetRandomEventParameters(meanYM, sigmaYM);
};
/********************************************************/
void QwBPMStripline::RandomizeEventData(int helicity)
{
  for (int i = 0; i < 4; i++)
    fWire[i].RandomizeEventData(helicity);

  ProcessEvent();
  return;
};
/********************************************************/
void QwBPMStripline::SetEventData(Double_t* relpos, UInt_t sequencenumber)
{
  // This needs to be modified to allow setting the position
  for (int i = 0; i < 2; i++) {
    fRelPos[i].SetHardwareSum(relpos[i], sequencenumber);
  }

  return;
};
/********************************************************/
void QwBPMStripline::EncodeEventData(std::vector<UInt_t> &buffer)
{
  for (int i = 0; i < 4; i++)
    fWire[i].EncodeEventData(buffer);
};
/********************************************************/
void  QwBPMStripline::ProcessEvent()
{
  for(int i=0;i<4;i++)
    fWire[i].ProcessEvent();

  static QwVQWK_Channel numer("numerator"), denom("denominator");

  if(IsGoodEvent())
    {
      for(int i=0;i<2;i++)
	{
	  numer.Difference(fWire[i*2],fWire[i*2+1]);
	  denom.Sum(fWire[i*2],fWire[i*2+1]);
	  fRelPos[i].Ratio(numer,denom);
	  fRelPos[i].Scale(kQwStriplineCalibration);
	  if(kDEBUG)
	    {
	      std::cout<<" stripline name="<<fElementName<<axis[i];
	      std::cout<<" event number="<<fWire[i*2].GetSequenceNumber()<<"\n";
	      std::cout<<" hw  Wire["<<i*2<<"]="<<fWire[i*2].GetHardwareSum()<<"  ";
	      std::cout<<" hw  Wire["<<i*2+1<<"]="<<fWire[i*2+1].GetHardwareSum()<<"\n";
	      std::cout<<" hw numerator="<<numer.GetHardwareSum()<<"  ";
	      std::cout<<" hw denominator="<<denom.GetHardwareSum()<<"\n";
	      std::cout<<" hw  fRelPos["<<i<<"]="<<fRelPos[i].GetHardwareSum()<<"\n \n";
	    }
	}
      if(bRotated)
	{
	  /* for this one I suppose that the direction [0] is vertical and up,
	     direction[3] is the beam line direction toward the beamdump
	     if rotated than the frame  is rotated by 45 deg counter clockwise*/
	  numer=fRelPos[0];
	  denom=fRelPos[1];
	  fRelPos[0].Sum(numer,denom);
	  fRelPos[1].Difference(numer,denom);
	  fRelPos[0].Scale(kRotationCorrection);
	  fRelPos[1].Scale(kRotationCorrection);
	}
      for(int i=0;i<3;i++)
	fAbsPos[i].Offset(fOffset[i]);
    }

  return;
};
/********************************************************/
void QwBPMStripline::Print()
{
  for(int i=0;i<4;i++)
    fWire[i].Print();
  for(int i=0;i<2;i++)
    fRelPos[i].Print();
  return;
}

/********************************************************/
Bool_t QwBPMStripline::IsGoodEvent()
{
  Bool_t fEventIsGood=kTRUE;
  for(int i=0;i<4;i++)
    {
      fEventIsGood &= fWire[i].IsGoodEvent();
      fEventIsGood &= fWire[i].MatchSequenceNumber(fWire[0].GetSequenceNumber());
      fEventIsGood &= fWire[i].MatchNumberOfSamples(fWire[0].GetNumberOfSamples());
    }

  if(!fEventIsGood||kDEBUG)
    {
      std::cerr<<"QwBPMStripline::IsGoodEvent()\n";
      std::cerr<<" test for BPM "<<GetElementName()<<"\n";
      std::cerr<<"Individual wire goodness ";
      for(int i=0;i<4;i++)
	std::cerr<<subelement[i]<<":"<<fWire[i].IsGoodEvent()<<" ;";
      std::cerr<<"\n";
      std::cerr<<" sequence numbers(event number) =";
      for(int i=0;i<4;i++)
	std::cerr<<fWire[i].GetSequenceNumber()<<":";
      std::cerr<<"\n";
      std::cerr<<" number of samples =";
      for(int i=0;i<4;i++)
	std::cerr<<fWire[i].GetNumberOfSamples()<<":";
      std::cerr<<"\n";
      if(!fEventIsGood) std::cerr<<"Unable to construct relative positions \n\n";
    }

  return fEventIsGood;
};
/********************************************************/
Int_t QwBPMStripline::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t index)
{
  if(index<4)
    fWire[index].ProcessEvBuffer(buffer,word_position_in_buffer);
  else
    std::cerr <<
      "QwBPMStripline::ProcessEvBuffer(): attemp to fill in raw date for a wire that doesn't exist \n";

  return word_position_in_buffer;
};
/********************************************************/
void QwBPMStripline::SetOffset(Double_t Xoffset, Double_t Yoffset, Double_t Zoffset)
{
  fOffset[0]=Xoffset;
  fOffset[1]=Yoffset;
  fOffset[2]=Zoffset;

  return;
};

void QwBPMStripline::SetSubElementPedestal(Int_t j, Double_t value)
{
  fWire[j].SetPedestal(value);
  return;
}

void QwBPMStripline::SetSubElementCalibrationFactor(Int_t j, Double_t value)
{
  fWire[j].SetCalibrationFactor(value);
  return;
}
/********************************************************/
TString QwBPMStripline::GetSubElementName(Int_t subindex)
{
  TString thisname;
  if(subindex<4&&subindex>-1)
    thisname=fWire[subindex].GetElementName();
  else
    std::cerr<<"QwBPMStripline::GetSubElementName for "<<
      GetElementName()<<" this subindex doesn't exists \n";

  return thisname;
}

UInt_t QwBPMStripline::GetSubElementIndex(TString subname)
{
  subname.ToUpper();
  UInt_t localindex=999999;
  for(int i=0;i<4;i++)
    if(subname==subelement[i])localindex=i;

  if(localindex>3)
    std::cerr << "QwBPMStripline::GetSubElementIndex is unable to associate the string -"
	      <<subname<<"- to any index"<<std::endl;

  return localindex;
};
/********************************************************/
QwBPMStripline& QwBPMStripline::operator= (const QwBPMStripline &value)
{
  if (GetElementName()!="")
  {
    this->bRotated=value.bRotated;
	for(int i=0;i<4;i++)
		this->fWire[i]=value.fWire[i];
	for(int i=0;i<2;i++)
		this->fRelPos[i]=value.fRelPos[i];
	for(int i=0;i<3;i++)
		{
		this->fAbsPos[i]=value.fAbsPos[i];
		this->fOffset[i]=value.fOffset[i];
		}
	}
  return *this;
};

QwBPMStripline& QwBPMStripline::operator+= (const QwBPMStripline &value)
{
  if (GetElementName()!="")
    {
      for(int i=0;i<4;i++)
	this->fWire[i]+=value.fWire[i];
      for(int i=0;i<2;i++)
	this->fRelPos[i]+=value.fRelPos[i];
      for(int i=0;i<3;i++)
	this->fAbsPos[i]+=value.fAbsPos[i];
    }
  return *this;
};

QwBPMStripline& QwBPMStripline::operator-= (const QwBPMStripline &value)
{
  if (GetElementName()!="")
    {
      for(int i=0;i<4;i++)
	this->fWire[i]-=value.fWire[i];
      for(int i=0;i<2;i++)
	this->fRelPos[i]-=value.fRelPos[i];
      for(int i=0;i<3;i++)
	this->fAbsPos[i]-=value.fAbsPos[i];
    }
  return *this;
};


void QwBPMStripline::Sum(QwBPMStripline &value1, QwBPMStripline &value2){
  *this =  value1;
  *this += value2;
};

void QwBPMStripline::Difference(QwBPMStripline &value1, QwBPMStripline &value2){
  *this =  value1;
  *this -= value2;
};

void QwBPMStripline::Ratio(QwBPMStripline &numer, QwBPMStripline &denom)
{
  // this function is called when forming asymmetries. In this case waht we actually want for the
  // stripline is the difference only not the asymmetries
  *this=numer;

//   if (GetElementName()!="")
//     {
//       for(int i=0;i<4;i++)
// 	this->fWire[i].Ratio(numer.fWire[i], denom.fWire[i]);
//       for(int i=0;i<2;i++)
// 	this->fRelPos[i].Ratio(numer.fRelPos[i], denom.fRelPos[i]);
//       for(int i=0;i<3;i++)
// 	this->fAbsPos[i].Ratio(numer.fAbsPos[i], denom.fAbsPos[i]);
//     }

  return;
};


void QwBPMStripline::Scale(Double_t factor)
{
  for(int i=0;i<2;i++)
    {
      fRelPos[i].Scale(factor);
      fAbsPos[i].Scale(factor);
    }
}



/********************************************************/
void QwBPMStripline::SetRootSaveStatus(TString &prefix)
{
  if(prefix=="diff_"||prefix=="yield_"|| prefix=="asym_")
    bFullSave=kFALSE;

  return;
}


void  QwBPMStripline::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      TString thisprefix=prefix;
      if(prefix=="asym_")
	thisprefix="diff_";
      SetRootSaveStatus(prefix);

      if(bFullSave)
	for(int i=0;i<4;i++)
	  fWire[i].ConstructHistograms(folder, thisprefix);
      for(int i=0;i<2;i++)
 	fRelPos[i].ConstructHistograms(folder, thisprefix);
      // for(int i=0;i<3;i++)
      //  fAbsPos[i].ConstructHistograms(folder, prefix);
    }
  return;
};

void  QwBPMStripline::FillHistograms()
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      if(bFullSave)
	for(int i=0;i<4;i++)
	  fWire[i].FillHistograms();
      for(int i=0;i<2;i++)
	fRelPos[i].FillHistograms();
      // for(int i=0;i<3;i++)
      //  fAbsPos[i].FillHistograms();
    }
  return;
};

void  QwBPMStripline::DeleteHistograms()
{
  if (GetElementName()=="")
    {
    }
  else
    {
      if(bFullSave)
	for(int i=0;i<4;i++)
	  fWire[i].DeleteHistograms();
      for(int i=0;i<2;i++)
	fRelPos[i].DeleteHistograms();
    }
  return;
};


void  QwBPMStripline::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  } else
    {
      TString thisprefix=prefix;
      if(prefix=="asym_")
	thisprefix="diff_";

      SetRootSaveStatus(prefix);
      if(bFullSave)
	for(int i=0;i<4;i++)
	  fWire[i].ConstructBranchAndVector(tree,thisprefix,values);
      for(int i=0;i<2;i++)
	fRelPos[i].ConstructBranchAndVector(tree,thisprefix,values);
    }
  return;
};

void  QwBPMStripline::FillTreeVector(std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the tree.
  } else
    {
      if(bFullSave)
	for(int i=0;i<4;i++)
	  fWire[i].FillTreeVector(values);
      for(int i=0;i<2;i++)
	fRelPos[i].FillTreeVector(values);

    }
  return;
};

//******************************************************************
void QwBPMStripline::Copy(VQwDataElement *source)
{
  try
    {
     if(typeid(*source)==typeid(*this))
       {
	 QwBPMStripline* input=((QwBPMStripline*)source);
	 this->fElementName=input->fElementName;
	 this->bRotated=input->bRotated;
	 for(int i=0;i<3;i++)
	   this->fOffset[i]=input->fOffset[i];
	 for(int i=0;i<4;i++)
	   this->fWire[i].Copy(&(input->fWire[i]));
	 for(int i=0;i<2;i++)
	   this->fRelPos[i].Copy(&(input->fRelPos[i]));
	 for(int i=0;i<3;i++)
	   this->fAbsPos[i].Copy(&(input->fAbsPos[i]));
       }
     else
	{
	  TString loc="Standard exception from QwBPMStripline::Copy = "
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


