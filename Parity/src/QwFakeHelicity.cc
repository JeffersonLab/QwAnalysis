/**********************************************************\
* File: QwFakeHelicity.C                                      *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwFakeHelicity.h"
#include "QwHistogramHelper.h"
#include <stdexcept>

#include "QwLog.h"

extern QwHistogramHelper gQwHists;

Bool_t QwFakeHelicity::IsContinuous()
{ 
  Bool_t results=kFALSE;
  if(IsGoodPatternNumber()&&IsGoodEventNumber()&&IsGoodPhaseNumber())
    results=kTRUE;
  return results;
}


Bool_t QwFakeHelicity::IsGoodPatternNumber()
{
  Bool_t results;

  if((fPatternNumber == fPatternNumberOld) && (fPatternPhaseNumber == fPatternPhaseNumberOld+1))//same pattern new phase
       results = kTRUE; //got same pattern
  else if((fPatternNumber == fPatternNumberOld + 1) && (fPatternPhaseNumber == fMinPatternPhase))  
       results=kTRUE; //new pattern
  else results=kFALSE; //wrong pattern

  if(!results)
    {
      QwWarning << "QwFakeHelicity::IsGoodPatternNumber: \n this is not s good pattern number indeed:"
		<< QwLog::endl;
      Print();
    }

  return results;
};


Bool_t QwFakeHelicity::IsGoodEventNumber()
{
  Bool_t results;
  if(fEventNumber == fEventNumberOld + 1)
    results= kTRUE;
  else
    results= kFALSE;

 if(!results)
    {
      QwWarning << "QwFakeHelicity::IsGoodEventNumber: \n this is not a good event number indeed:"
		<< QwLog::endl;
      Print();
    }
  return results;
};


Bool_t QwFakeHelicity::IsGoodPhaseNumber()
{
  Bool_t results;

  if((fPatternPhaseNumber == fMaxPatternPhase)  && (fPatternNumber == fPatternNumberOld )) //maximum phase of old pattern
     results = kTRUE;
  else if((fPatternPhaseNumber == fPatternPhaseNumberOld+1) && (fPatternNumber == fPatternNumberOld))
    results = kTRUE;
  else if((fPatternPhaseNumber == fMinPatternPhase) && (fPatternNumber == fPatternNumberOld + 1))
    results= kTRUE;
  else
    results = kFALSE;

  if(fPatternPhaseNumber>fMaxPatternPhase)
    results=kFALSE;

 if(!results)
    {
      QwWarning << "QwFakeHelicity::IsGoodPhaseNumber:  not a good phase number \t"
		<< "Phase: " << fPatternPhaseNumber << " out of " 
		<<fMaxPatternPhase
		<< "(was " <<fPatternPhaseNumberOld<<")"
		<< "\tPattern #" << fPatternNumber << "(was " 
		<< fPatternNumberOld <<")" 
		<< QwLog::endl; //Paul's modifications
      Print();
    }

  return results;
};


Bool_t QwFakeHelicity::IsGoodFakeHelicity()
{

  fGoodFakeHelicity = kTRUE;

  return fGoodFakeHelicity;
}


void QwFakeHelicity::ClearEventData()
{

  for (size_t i=0;i<fWord.size();i++)
    fWord[i].ClearEventData();

  //reset data
  fEventNumberOld = fEventNumber;
  fPatternNumberOld = fPatternNumber;
  fPatternPhaseNumberOld = fPatternPhaseNumber;

  //clear data
  fFakeHelicity= kUndefinedFakeHelicity;
  fFakeHelicityBitPlus = kFALSE;
  fFakeHelicityBitMinus = kFALSE;
  return;
}

Int_t QwFakeHelicity::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  //stub function
  std::cerr<<" this function QwFakeHelicity::ProcessConfigurationBuffer does nothing yet \n";
  return 0;
}

 Int_t QwFakeHelicity::LoadInputParameters(TString pedestalfile)
 {
   SetFakeHelicityDelay(8); // that is the number of pattern delayed

   return 0;
 }

Bool_t QwFakeHelicity::ApplySingleEventCuts(){//impose single event cuts //Paul's modifications


  return kTRUE;  
};

Int_t QwFakeHelicity::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure
  
  return 1;  
};

//*****************************************************************
Int_t QwFakeHelicity::GetEventcutErrorFlag(){//return the error flag 

  return 0;

}

void QwFakeHelicity::ProcessEvent()
{

   fEventNumber = fEventNumberOld+1;
   
   if (fPatternPhaseNumber == 4)
     {
       fPatternPhaseNumber = fMinPatternPhase;
       fPatternNumber      = fPatternNumberOld + 1;
     } 
   else  
     {
       fPatternPhaseNumber = fPatternPhaseNumberOld + 1;
       fPatternNumber       = fPatternNumberOld; 
     }
   
   fFakeHelicity=0;
   
   
   fFakeHelicityBitPlus=kTRUE;
   fFakeHelicityBitMinus=kFALSE;
   
   PredictFakeHelicity();

  return;
}

void QwFakeHelicity::EncodeEventData(std::vector<UInt_t> &buffer)
{
 //  std::vector<UInt_t> localbuffer;
//   localbuffer.clear();

//   UInt_t userbit = 0x0;
//   if (fPatternPhaseNumber == fMinPatternPhase) userbit |= 0x80000000;
//   if (fFakeHelicityDelayed == 1)    userbit |= 0x40000000;

//   // Write the words to the buffer
//   localbuffer.push_back(0x1); // cleandata
//   localbuffer.push_back(0xa); // scandata1
//   localbuffer.push_back(0xa); // scandata2
//   localbuffer.push_back(0x0); // scalerheader
//   localbuffer.push_back(0x20); // scalercounter (32)
//   localbuffer.push_back(userbit); // userbit
//   for (int i = 0; i < 64; i++) localbuffer.push_back(0x0); // (not used)

//   // If there is element data, generate the subbank header
//   std::vector<UInt_t> subbankheader;
//   std::vector<UInt_t> rocheader;
//   if (localbuffer.size() > 0) {

//     // Form CODA subbank header
//     subbankheader.clear();
//     subbankheader.push_back(localbuffer.size() + 1);	// subbank size
//     subbankheader.push_back((fCurrentBank_ID << 16) | (0x01 << 8) | (1 & 0xff));
// 		// subbank tag | subbank type | event number

//     // Form CODA bank/roc header
//     rocheader.clear();
//     rocheader.push_back(subbankheader.size() + localbuffer.size() + 1);	// bank/roc size
//     rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
// 		// bank tag == ROC | bank type | event number

//     // Add bank header, subbank header and element data to output buffer
//     buffer.insert(buffer.end(), rocheader.begin(), rocheader.end());
//     buffer.insert(buffer.end(), subbankheader.begin(), subbankheader.end());
//     buffer.insert(buffer.end(), localbuffer.begin(), localbuffer.end());
//   }
}


void QwFakeHelicity::Print()
{

   std::cout<<"this event: Event#, Pattern#, PatternPhase#="
 	   << fEventNumber<<", "
 	   << fPatternNumber<<", "
	    << fPatternPhaseNumber<<"\n";
  std::cout<<"Previous event: Event#, Pattern#, PatternPhase#="
 	   << fEventNumberOld<<", "
 	   << fPatternNumberOld<<", "
 	   << fPatternPhaseNumberOld<<"\n";

  std::cout<<"delta = \n(fEventNumberOld)-(fMaxPatternPhase)x(fPatternNumberOld)-(fPatternPhaseNumberOld)= "
	   <<((fEventNumberOld)-(fMaxPatternPhase)*(fPatternNumberOld)-(fPatternPhaseNumberOld))<<"\n";

  std::cout<<"FakeHelicity Reported, Delayed, Actual ="
	   << fFakeHelicity<<"\n";

  std::cout<<"===\n";
  return;
}


 Int_t QwFakeHelicity::LoadChannelMap(TString mapfile)
 {
   // There is no channel map for load helicity
   return 0;
 };


Int_t QwFakeHelicity::LoadEventCuts(TString filename){

   return 0;
 };



Int_t QwFakeHelicity::ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Bool_t lkDEBUG=kFALSE ;

 //  Int_t index = GetSubbankIndex(roc_id,bank_id);

//   if (index>=0 && num_words>0){
//     //  We want to process this ROC.  Begin loopilooping through the data.
//     if (lkDEBUG)
//       {
// 	std::cout << "QwFakeHelicity::ProcessEvBuffer:  "
// 		  << "Begin processing ROC" << roc_id
// 		  << " and subbank "<<bank_id
// 		  << " number of words="<<num_words<<std::endl;
//       }
    
//     for(Int_t i=fWordsPerSubbank[index].first; i<fWordsPerSubbank[index].second; i++)
//       {
// 	if(fWord[i].fWordInSubbank+1<=num_words)
// 	  {
// 	    fWord[i].fValue=buffer[fWord[i].fWordInSubbank];
// 	  }
// 	else
// 	  {	
// 	    std::cout<<"There is not enough word in the buffer to read data for "
// 		     <<fWord[i].fWordName<<"\n";
// 	    std::cout<<"words in this buffer:"<<num_words<<" tyring to read woord number ="
// 		     <<fWord[i].fWordInSubbank<<"\n";
// 	  }
//       }
//     if(lkDEBUG)
//       {
// 	std::cout<<"Done with Processing this event \n";
// 	for(size_t i=0;i<fWord.size();i++) {
// 	  std::cout<<" word number = "<<i<<" ";
// 	  fWord[i].Print();
// 	}
//       }
//   }
//   lkDEBUG=kFALSE;

  return 0;
}



Int_t QwFakeHelicity::GetHelicityActual()
{
  return fFakeHelicity;
};


Long_t QwFakeHelicity::GetPatternNumber()
{
  return  fPatternNumber;
};

Long_t QwFakeHelicity::GetEventNumber()
{
  return fEventNumber;
};

Int_t QwFakeHelicity::GetPhaseNumber()
{
  return fPatternPhaseNumber;
};

void QwFakeHelicity::SetEventPatternPhase(Int_t event, Int_t pattern, Int_t phase)
{
  fEventNumber = event;
  fPatternNumber = pattern;
  fPatternPhaseNumber = phase;
};

//**************************************************//
void QwFakeHelicity::SetHistoTreeSave(TString &prefix)
{
  if(prefix=="asym_")
    fHistoType=kHelNoSave;
  else if (prefix=="yield_")
    fHistoType=kHelSavePattern;
  else
    fHistoType=kHelSaveMPS;

  return;
}

//**************************************************//
void  QwFakeHelicity::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  SetHistoTreeSave(prefix);
  std::cout<<"QwFakeHelicity::ConstructHistograms with prefix "<<prefix<<" and fHistoType="<<fHistoType<<"\n";
  if (folder != NULL) folder->cd();
  TString basename;
  size_t index=0;

  if(fHistoType==kHelNoSave)
    {
      //do nothing
    }
  else if(fHistoType==kHelSavePattern)
    {
      fHistograms.resize(1+fWord.size(), NULL);
      basename="pattern_polarity";
      fHistograms[index]   = gQwHists.Construct1DHist(basename);
      index+=1;
      for (size_t i=0; i<fWord.size(); i++){
	basename="hel_"+fWord[i].fWordName;
	fHistograms[index]   = gQwHists.Construct1DHist(basename);
	index+=1;
      }
    }
  else if(fHistoType==kHelSaveMPS)
    {
      fHistograms.resize(4+fWord.size(), NULL);
      //eventnumber, patternnumber, helicity, patternphase + fWord.size
      basename=prefix+"delta_event_number";
      fHistograms[index]   = gQwHists.Construct1DHist(basename);
      index+=1;
      basename=prefix+"delta_pattern_number";
      fHistograms[index]   = gQwHists.Construct1DHist(basename);
      index+=1;
      basename=prefix+"pattern_phase";
      fHistograms[index]   = gQwHists.Construct1DHist(basename);
      index+=1;
      basename=prefix+"helicity";
      fHistograms[index]   = gQwHists.Construct1DHist(basename);
      index+=1;
      for (size_t i=0; i<fWord.size(); i++){
	basename=prefix+fWord[i].fWordName;
	fHistograms[index]   = gQwHists.Construct1DHist(basename);
	index+=1;
      }
    }
  else
    std::cerr<<"QwFakeHelicity::ConstructHistograms this prefix--"<<prefix<<"-- is not unknown:: no histo created";

  return;
}

void  QwFakeHelicity::DeleteHistograms()
{

    std::cout<<"QwFakeHelicity::DeleteHistograms for system="<<fSystemName<<"; fHistograms.size="<<fHistograms.size()<<"\n";
  if((fHistoType==kHelSaveMPS)||(fHistoType==kHelSavePattern))
    {
      for (size_t i=0; i<fHistograms.size(); i++){
	if (fHistograms.at(i) != NULL){
	  fHistograms.at(i)->Delete();
	  fHistograms.at(i) =  NULL;
	}
      }
      fHistograms.clear();
    }
  return;
}


void  QwFakeHelicity::FillHistograms()
{
  Bool_t localdebug=kFALSE;
  size_t index=0;


  if(fHistoType==kHelNoSave)
    {
      //do nothing
    }
  else if(fHistoType==kHelSavePattern)
    {
      if(localdebug) std::cout<<"QwFakeHelicity::FillHistograms helicity info \n";
      if(localdebug) std::cout<<"QwFakeHelicity::FillHistograms  pattern polarity="<<fFakePatternPolarity<<"\n";
      fHistograms[index]->Fill(fFakePatternPolarity);
      index+=1;
      for (size_t i=0; i<fWord.size(); i++){
	fHistograms[index]->Fill(fWord[i].fValue);
	index+=1;
	if(localdebug) std::cout<<"QwFakeHelicity::FillHistograms "<<fWord[i].fWordName<<"="<<fWord[i].fValue<<"\n";
      }
    }
  else if(fHistoType==kHelSaveMPS)
    {
      if(localdebug) std::cout<<"QwFakeHelicity::FillHistograms mps info \n";
      fHistograms[index]->Fill(fEventNumber-fEventNumberOld);
      index+=1;
      fHistograms[index]->Fill(fPatternNumber-fPatternNumberOld);
      index+=1;
      fHistograms[index]->Fill(fPatternPhaseNumber);
      index+=1;
      fHistograms[index]->Fill(fFakeHelicity);
      index+=1;
      for (size_t i=0; i<fWord.size(); i++){
	fHistograms[index]->Fill(fWord[i].fValue);
	index+=1;
	if(localdebug) std::cout<<"QwFakeHelicity::FillHistograms "<<fWord[i].fWordName<<"="<<fWord[i].fValue<<"\n";
      }
    }

  return;
}


void  QwFakeHelicity::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  SetHistoTreeSave(prefix);
  //  std::cout<<"QwFakeHelicity::ConstructBranchAndVector  prefix="<<prefix<<" fHistoType="<<fHistoType<<"\n";


  fTreeArrayIndex  = values.size();
  size_t localindex=fTreeArrayIndex;
  TString basename;
  if(fHistoType==kHelNoSave)
    {
      //do nothing 
    }
  else if(fHistoType==kHelSaveMPS)
    {
      /*
      basename = "helicity";
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      //
      */
      basename = "faked_helicity";    //predicted actual helicity before being delayed.
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      //
    //   basename = "delayed_helicity";   //predicted delayed helicity
//       values.push_back(0.0);
//       tree->Branch(basename, &(values[localindex]),basename+"/D");
//       //
//       basename = "reported_helicity";  //delayed helicity reported by the input register.
//       values.push_back(0.0);
//       tree->Branch(basename, &(values[localindex]),basename+"/D");
//       localindex  = values.size();
      //
      basename = "pattern_phase";
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
     //
      localindex  = values.size();
      basename = "pattern_number";
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      //
      localindex  = values.size();
      basename = "event_number";
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      //
      for (size_t i=0; i<fWord.size(); i++)
	{
	  localindex  = values.size();
	  basename = fWord[i].fWordName;
	  values.push_back(0.0);
	  tree->Branch(basename, &(values[localindex]),basename+"/D");
	}
    }
  else if(fHistoType==kHelSavePattern)
    {
      /*
      basename = "pattern_polarity";
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      //
      */
      basename = "faked_pattern_polarity";
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
  //     //
//       basename = "delayed_pattern_polarity";
//       values.push_back(0.0);
//       tree->Branch(basename, &(values[localindex]),basename+"/D");
//       //

      localindex  = values.size();
      basename = "pattern_number";
      values.push_back(0.0);
      tree->Branch(basename, &(values[localindex]),basename+"/D");
      //
      for (size_t i=0; i<fWord.size(); i++)
	{
	  localindex  = values.size();
	  basename = fWord[i].fWordName;
	  values.push_back(0.0);
	  tree->Branch(basename, &(values[localindex]),basename+"/D");

	}
    }

  return;
}

void  QwFakeHelicity::FillTreeVector(std::vector<Double_t> &values)
{
  //std::cout<<"QwFakeHelicity::FillTreeVector fPatternNumber="<<fPatternNumber<<"\n";
  //if(ldebug)
  
  size_t index=fTreeArrayIndex;
  if(fHistoType==kHelSaveMPS)
    {
      values[index++] = fFakeHelicity;
      values[index++] = fPatternPhaseNumber;
      values[index++] = fPatternNumber;
      values[index++] = fEventNumber;
      for (size_t i=0; i<fWord.size(); i++)
	values[index++] = fWord[i].fValue;
    }
  else if(fHistoType==kHelSavePattern)
    {
      values[index++] = fFakePatternPolarity;
      values[index++] = fPatternNumber;
      for (size_t i=0; i<fWord.size(); i++)
	values[index++] = fWord[i].fValue;
    }

  return;
}


void QwFakeHelicity::SetFirst24Bits(UInt_t seed)
{

  // This gives the predictor a quick start
  n_ranbits = 24;
  UShort_t first24bits[n_ranbits];
  for (unsigned int i = 0; i < n_ranbits; i++) first24bits[i] = (seed >> i) & 0x1;
   iseed_Fake = iseed_Delayed;
}

UInt_t QwFakeHelicity::GetRandbit(UInt_t& ranseed){
  Bool_t status = false;

  if (BIT24)
    status=GetRandbit24(ranseed);
  if (BIT30)
    status=GetRandbit30(ranseed);

  return status;
}

UInt_t QwFakeHelicity::GetRandbit24(UInt_t& ranseed){
  /*This is a 24 bit random bit generator according to the "new" algorithm described in "G0 FakeHelicity Digital Controls
    described in G0 FakeHelicity Digial Controls by E. Stangland, R. Flood, H. Dong.*/

  const UInt_t IB1 = 1;                     //Bit 1 of shift register 000000000000000000000001
  const UInt_t IB3 = 4;                     //Bit 3 of shift register 000000000000000000000100
  const UInt_t IB4 = 8;                     //Bit 4 of shift register 000000000000000000001000
  const UInt_t IB24 = 8388608;               //Bit 24 of shift register 100000000000000000000000
  const UInt_t MASK = IB1+IB3+IB4+IB24;     //Sum of the four feedback bits is 100000000000000000001101

  UInt_t result; //The generated pattern


  if(ranseed<=0)
    {
      std::cerr<<"ranseed must be greater than zero!"<<ranseed<<"\n";
      result = 0;
    }

  if(ranseed & IB24) // if bit 24 of ranseed = 1, then output 1
    {
      ranseed = ((ranseed^MASK)<<1|IB1);
      result = 1;
    }
  else
    {
      ranseed<<=1;
      result = 0;
    }
  return(result);
  
};

UInt_t QwFakeHelicity::GetRandbit30(UInt_t & ranseed){
  UInt_t bit7    = (ranseed & 0x00000040) != 0;
  UInt_t bit28   = (ranseed & 0x08000000) != 0;
  UInt_t bit29   = (ranseed & 0x10000000) != 0;
  UInt_t bit30   = (ranseed & 0x20000000) != 0;

  UInt_t result = (bit30 ^ bit29 ^ bit28 ^ bit7) & 0x1;

  if(ranseed<=0) { 
    std::cerr<<"ranseed must be greater than zero!"<<"\n";
    result = 0;
  }
  ranseed =  ( (ranseed<<1) | result ) & 0x3FFFFFFF;

  return(result);
};



// UInt_t QwFakeHelicity::GetRandomSeed(UShort_t* first24randbits)
// {
UInt_t QwFakeHelicity::GetRandomSeed()
{
  Bool_t ldebug=0;
  if(ldebug)std::cout<<" Entering QwFakeHelicity::GwetRandomSeed \n";

  /*  This the random seed generator used in G0 (L.Jianglai)
      Here we get the 24 random bits and derive the randome seed from that.
      randome seed                      : b24 b23 b22.....b2 b1
      first 24 random bit from this seed: h1 h2 h3 ....h23 h24
      we have,
      b23 = h1, b22 = h2,... b5 = h20,
      h21^b24 = b4 , h3^b24^b23 = b3 ,h23^b23^b22 = b2, h24^b22^b24 = b1.
      Thus by using h1,...h24, we can derive the b24,..b1 of the randseed.
  */

  UShort_t b[25];
  UInt_t ranseed = 0;

  ranseed = 0x0001D2&0xFFFFFF; //put a mask. 
  //The value of the radseed was picked up from the studies 
  //I did on the helicity bit generater back in 2008.

  if(ldebug)std::cout<<" seed ="<<ranseed<<"\n";
  if(ldebug)std::cout<<" Exiting QwFakeHelicity::GetRandomSeed \n";


  return ranseed;

}

void QwFakeHelicity::RunPredictor()
{
  Int_t ldebug = 0;

  //if(ldebug)  std::cerr<<"Entering QwFakeHelicity::RunPredictor \n";
  if(ldebug)  std::cerr<<"Entering QwFakeHelicity::RunPredictor for fEventNumber, "<<fEventNumber
		       <<", fPatternNumber, "<<fPatternNumber
		       << ", and fPatternPhaseNumber, "<<fPatternPhaseNumber<<std::endl;//Paul's modifications

  //Update the random seed if the new event is from a different pattern.
  //Have to write the for loop!
  for( size_t i=0;i<size_t(fPatternNumber- fPatternNumberOld);i++) //got a new pattern
    {
      fFakePatternPolarity = GetRandbit(iseed_Fake);
      if(ldebug)
	std::cout<<"Predicting : seed actual, delayed: "<< iseed_Fake<<std::endl;
    }

  //Predict the helicity according to pattern

  //  Defined patterns:
  //    Pair:    +-       or -+
  //    Quartet: +--+     or -++-
  //    Octet:   +--+-++- or -++-+--+
  //    Symmetric octet:  +-+--+-+ or -+-++-+-
  Int_t localphase = fPatternPhaseNumber-fMinPatternPhase;//Paul's modifications
   if ((fMaxPatternPhase<=4) || (1==1)){//Paul's modifications
    switch(localphase){
    case 0: //phase 0 -> new pattern, fFakeHelicityFake has been predicteed in the for loop.
    case 3:
    case 5:
    case 6:
      fFakeHelicity  = fFakePatternPolarity;  //DON'T flip helicity 
      break;
    case 1:
    case 2: //phase 2 helicity  = phase 1 helicity
    case 4: //phase 4 helicity  = phase 1 helicity
    case 7: //phase 7 helicity  = phase 4 helicity
      fFakeHelicity  = fFakePatternPolarity  ^ 0x1; //flip helicity 
      break; 
      
    default: //for values other than 1,2,3,4. //This would be discovered in error checking.
      ResetPredictor();
      break;
    }
  } else {
    //  This is the Symmetric octet:  +-+--+-+ or -+-++-+-
    switch(localphase){
    case 0: //phase 1 -> new pattern, fFakeHelicityFake has been predicteed in the for loop.
    case 2:
    case 5:
    case 7:
      fFakeHelicity  = fFakePatternPolarity;  //DON'T flip helicity
      break;
    case 1:
    case 3: 
    case 4: 
    case 6: 
      fFakeHelicity  = fFakePatternPolarity  ^ 0x1; //flip helicity
      break;
    default: //for values other than 1,2,3,4. //This would be discovered in error checking.
      ResetPredictor();
      break;
    }
   }
  if(ldebug)
    {
      std::cout<<"Predicted Polarity ::: Faked ="
	       <<fFakePatternPolarity<<"\n";
      std::cout<<"Predicted FakeHelicity "<<fFakeHelicity<<std::endl;
    }


  return;
}


Bool_t QwFakeHelicity::CollectRandBits()
{
    //routine to collect 24 random bits before getting the randseed for prediction
    Bool_t  ldebug = kFALSE;
    const UInt_t ranbit_goal = 24;

  if(ldebug) std::cout<<"QwFakeHelicity::Entering CollectRandBits24...."<<"\n";

  fGoodFakeHelicity = kFALSE; //reset before prediction begins


  if(IsContinuous())
    {
      if((fPatternPhaseNumber==fMinPatternPhase)&& (fPatternNumber>=0))
	{
// 	  if(ldebug)std::cout<<"Collected 24 random bits. Get the random seed for the predictor."<<"\n";
// 	  iseed_Delayed = GetRandomSeed();
// 	  //This random seed will predict the helicity of the event (24+fFakeHelicityDelay) patterns  before;
// 	  // run GetRandBit 24 times to get the delayed helicity for this event
// 	  if(ldebug)std::cout<<"The reported seed 24 patterns ago = "<<iseed_Delayed<<"\n";
	
// 	  fFakeHelicity = fDelayedPatternPolarity;
// 	  //The helicity of the first phase in a pattern is
// 	  //equal to the polarity of the pattern
	      
	      
// 	  if(fFakeHelicityDelay >=0){
// 	    iseed_Fake = iseed_Delayed;
// 	    for(Int_t i=0; i<fFakeHelicityDelay; i++)
// 	      {
// 		if(ldebug) std::cout<<"Delaying helicity \n";
// 		fFakePatternPolarity = GetRandbit(iseed_Fake);
// 	      }
// 	  }
	  
	  fFakeHelicity =  fFakePatternPolarity;
	}
      
    } 
  if(ldebug) std::cout<<"QwFakeHelicity::CollectRandBits24 => Done collecting ...\n";
  
  return kTRUE;
  
  
};



void QwFakeHelicity::PredictFakeHelicity()
{
   Bool_t ldebug=kFALSE;

  if(ldebug)  std::cout<<"Entering QwFakeHelicity::PredictFakeHelicity \n";
  //Routine to predict the true helicity from the delayed helicity.
  //Helicities are delayed by 8 events or 2 quartets
  if(CollectRandBits()) {
    //After accumulating 24/30 helicity bits, iseed id up-to-date
    //If nothing goes wrong, n-ranbits will stay as 24/30
    //Reset it to zero if something goes wrong
    if(ldebug)  std::cout<<"QwFakeHelicity::PredictFakeHelicity=>Predicting the  helicity \n";
    RunPredictor();
    if(!IsGoodFakeHelicity())
      ResetPredictor();
  }
 
  if(ldebug)  std::cout<<"n_ranbit exiting the function = "<<n_ranbits<<"\n";


  return;
}

void QwFakeHelicity::SetFakeHelicityDelay(Int_t delay)
{
  //  Int_t locali=kUndefinedFakeHelicity;
  //Sets the number of bits the helicity reported gets delayed with
  if(delay>=0)
    {
      fFakeHelicityDelay = delay;

      //       for(int i=0;i<delay+1;i++)
      // 	fCheckFakeHelicityDelay.push_back(locali);
      // this is obsolete
      //basically we are creating a ring to store the FakeFakeHelicity.
      //The Fakehelcity for this event goes in position 0, the actualhelicity for the previous event
      //is in position 1, and the actual helicity from fFakeHelicityDelay events is in event delay and
      //it should match the reported helicity
    }
  else
    std::cerr<<"QwFakeHelicity::SetFakeHelicityDelay We cannot handle negative delay in the prediction of delayed helicity. Exiting.. \n";

  return;
}

  //start a new helicity prediction sequence

void QwFakeHelicity::ResetPredictor()
{
  n_ranbits = 0;
  fGoodFakeHelicity = kFALSE;
  fGoodPattern = kFALSE;
  return;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
void QwFakeHelicity::Copy(VQwSubsystem *source)
{

 try
    {
     if(typeid(*source)==typeid(*this))
	{
	  VQwSubsystem::Copy(source);
	  //QwFakeHelicity* input=((QwFakeHelicity*)source);
	  VQwSubsystem::Copy(source);
          QwFakeHelicity* input = dynamic_cast<QwFakeHelicity*>(source);
	  this->fWord.resize(input->fWord.size());
	  for(size_t i=0;i<this->fWord.size();i++)
	    {
	      this->fWord[i].fWordName=input->fWord[i].fWordName;
	      this->fWord[i].fModuleType=input->fWord[i].fModuleType;
	      this->fWord[i].fWordType=input->fWord[i].fWordType;
	    }
	}
      else
	{
	  TString loc="Standard exception from QwFakeHelicity::Copy = "
	    +source->GetSubsystemName()+" "
	    +this->GetSubsystemName()+" are not of the same type";
	  throw std::invalid_argument(loc.Data());
	}
    }
  catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }

  return;
}

VQwSubsystemParity*  QwFakeHelicity::Copy()
{

  QwFakeHelicity* TheCopy=new QwFakeHelicity("FakeHelicity Copy");
  TheCopy->Copy(this);
  return TheCopy;
}

VQwSubsystem&  QwFakeHelicity::operator=  (VQwSubsystem *value)
{

  if(Compare(value))
    {
      //QwFakeHelicity* input= (QwFakeHelicity*)value;
      VQwSubsystem::operator=(value);
      QwFakeHelicity* input= dynamic_cast<QwFakeHelicity*>(value);
      for(size_t i=0;i<input->fWord.size();i++)
	this->fWord[i].fValue=input->fWord[i].fValue;
      this->fFakeHelicity=input->fFakeHelicity;
      this->fPatternNumber =input->fPatternNumber ;
      this->fPatternPhaseNumber=input->fPatternPhaseNumber;
      this->fEventNumber=input->fEventNumber;
      this->fFakePatternPolarity=input->fFakePatternPolarity;
      this->fFakeHelicityBitPlus=input->fFakeHelicityBitPlus;
      this->fFakeHelicityBitMinus=input->fFakeHelicityBitMinus;
      this->fGoodFakeHelicity=input->fGoodFakeHelicity;
      this->fGoodPattern=input->fGoodPattern;
      /*
      std::cerr << "QwFakeHelicity::operator=:  "
		<< " input->fEventNumber=="<< input->fEventNumber
		<< " input->fPatternNumber=="<< input->fPatternNumber
		<< " input->fPatternPhaseNumber=="<< input->fPatternPhaseNumber
		<< " this->fEventNumber=="<< this->fEventNumber
		<< " this->fPatternNumber=="<< this->fPatternNumber
		<< " this->fPatternPhaseNumber=="<< this->fPatternPhaseNumber
		<< std::endl;
      */ 
    }      
  return *this;
};

VQwSubsystem&  QwFakeHelicity::operator+=  (VQwSubsystem *value)
{
  Bool_t localdebug=kFALSE;
  if(localdebug) std::cout<<"Entering QwFakeHelicity::operator+= adding "<<value->GetSubsystemName()<<" to "<<this->GetSubsystemName()<<" \n";

  //this routine is most likely to be called during the computatin of assymetry
  //this call doesn't make too much sense for this class so the following lines
  //are only use to put safe gards testing for example if the two instantiation indeed
  // refers to elements in the same pattern
  if(Compare(value))
    {
      //QwFakeHelicity* input= (QwFakeHelicity*)value;
      QwFakeHelicity* input= dynamic_cast<QwFakeHelicity*>(value);
      for(size_t i=0;i<input->fWord.size();i++)
	{
	  if(this->fWord[i].fValue!=input->fWord[i].fValue)
	    this->fWord[i].fValue=-999999;
	}
      if(localdebug) std::cout<<"QwFakeHelicity::operator+= this->fPatternNumber="<<this->fPatternNumber;
      if(localdebug) std::cout<<"input->fPatternNumber="<<input->fPatternNumber<<"\n";

      if(this->fPatternNumber!=input->fPatternNumber)
	this->fPatternNumber=-999999;
      if(this->fFakePatternPolarity!=input->fFakePatternPolarity)
	this->fPatternNumber=-999999;
    }
  return *this;
};

void QwFakeHelicity::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  //this routine is most likely to be called during the computatin of assymetry
  //this call doesn't make too much sense for this class so the followign lines
  //are only use to put safe gards testing for example if the two instantiation indeed
  // refers to elements in the same pattern
  if(Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this += value2;
    }
};

void QwFakeHelicity::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  // this is stub function defined here out of completion and uniformity between each subsystem
      *this =  value1;
};

void QwFakeHelicity::Ratio(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  // this is stub function defined here out of completion and uniformity between each subsystem
      *this =  value1;
};

void QwFakeHelicity::Calculate_Running_Average(){
};

void QwFakeHelicity::Do_RunningSum(){

};
 

Bool_t QwFakeHelicity::Compare(VQwSubsystem *value)
{
  Bool_t res=kTRUE;
  if(typeid(*value)!=typeid(*this))
    {
      res=kFALSE;
    }
  else
    {
      QwFakeHelicity* input= dynamic_cast<QwFakeHelicity*>(value);
      if(input->fWord.size()!=fWord.size())
	{
	res=kFALSE;
	}
    }
  return res;
}

