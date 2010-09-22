/**********************************************************\
* File: QwMainCerenkovDetector.cc                          *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#include "QwMainCerenkovDetector.h"

#include <sstream>

#include "QwSubsystemArray.h"
#include "QwLog.h"

// Register this subsystem with the factory
QwSubsystemFactory<QwMainCerenkovDetector>
  theMainCerenkovDetectorFactory("QwMainCerenkovDetector");

/**
 * Defines configuration options for QwEventBuffer class using QwOptions
 * functionality.
 *
 * @param options Options object
 */
void QwMainCerenkovDetector::DefineOptions(QwOptions &options){
  // Define the execution options
  options.AddOptions()
    ("QwMainCerenkovDetector.normalize",
     po::value<bool>()->default_value(false)->zero_tokens(),
     "Normalize the detectors by beam current");
}


/*!
 * Loads the configuration options into this instance of
 * QwMainCerenkovDetector from the QwOptions object.
 *
 * @param options Options object
 */
void QwMainCerenkovDetector::ProcessOptions(QwOptions &options){
  bNormalization = options.GetValue<bool>("QwMainCerenkovDetector.normalize");
  if (! bNormalization){
    QwWarning << "QwMainCerenkovDetector::ProcessOptions:  "
	      << "Detector yields WILL NOT be normalized."
	      << QwLog::endl;
  }
};


//*****************************************************************
/**
 * Publish internal values
 * @return
 */
const Bool_t QwMainCerenkovDetector::PublishInternalValues() const
{
  ///  TODO:  The published variable list should be generated from
  ///         the channel map file.
  // Publish variables
  Bool_t status = kTRUE;
  status = status && PublishInternalValue("md1neg", "MD1-");
  status = status && PublishInternalValue("md1pos", "MD1+");
  status = status && PublishInternalValue("md2neg", "MD2-");
  status = status && PublishInternalValue("md2pos", "MD2+");
  status = status && PublishInternalValue("md3neg", "MD3-");
  status = status && PublishInternalValue("md3pos", "MD3+");
  status = status && PublishInternalValue("md4neg", "MD4-");
  status = status && PublishInternalValue("md4pos", "MD4+");
  status = status && PublishInternalValue("md5neg", "MD5-");
  status = status && PublishInternalValue("md5pos", "MD5+");
  status = status && PublishInternalValue("md6neg", "MD6-");
  status = status && PublishInternalValue("md6pos", "MD6+");
  status = status && PublishInternalValue("md7neg", "MD7-");
  status = status && PublishInternalValue("md7pos", "MD7+");
  status = status && PublishInternalValue("md8neg", "MD8-");
  status = status && PublishInternalValue("md8pos", "MD8+");
  return status;
};


//*****************************************************************
/**
 * Return the pointer to the variable name
 * TODO should incorporate logic below, changes similarly in parent classes
 * @param name Name of the desired variable
 * @return Pointer to the variable name, null if not found
 */
const VQwDataElement* QwMainCerenkovDetector::ReturnInternalValue(const TString& name) const
{
  Bool_t ldebug = kFALSE;
  if (ldebug)
    QwDebug << "QwMainCerenkovDetector::ReturnInternalValue called for value name, "
            << name.Data() << QwLog::endl;

  // search in integration PMTs
  if (GetIntegrationPMT(name) != NULL) {
    if (ldebug)
      QwDebug << "QwMainCerenkovDetector::ReturnInternalValue got element " << name << QwLog::endl;
    return GetIntegrationPMT(name)->GetChannel(name);

  // search in combined PMTs
  } else if (GetCombinedPMT(name) != NULL) {
    if (ldebug)
      QwDebug << "QwMainCerenkovDetector::ReturnInternalValue got element " << name << QwLog::endl;
    return GetCombinedPMT(name)->GetChannel(name);
  }

  // not found
  return 0;
}

/**
 * Return the value of variable name
 * TODO should use function above, not have logic built in
 * @param name Name of the desired variable
 * @param value Pointer to the value to be filled by the call
 * @return True if the variable was found, false if not found
 */
const Bool_t QwMainCerenkovDetector::ReturnInternalValue(const TString& name, VQwDataElement* value) const
{
  Bool_t foundit = kFALSE;
  QwVQWK_Channel* value_ptr = dynamic_cast<QwVQWK_Channel*>(value);
  if (value_ptr == NULL) {
    QwWarning << "QwMainCerenkovDetector::ReturnInternalValue requires that "
              << "'value' be a pointer to QwVQWK_Channel"
              << QwLog::endl;
  } else {
    // Cast into appropriate type
    const QwVQWK_Channel* internal_value_ptr = dynamic_cast<const QwVQWK_Channel*>(ReturnInternalValue(name));
    if (internal_value_ptr) {
      foundit = kTRUE;
      *value_ptr = *internal_value_ptr;
    } else {
      QwWarning << "QwMainCerenkovDetector::ReturnInternalValue did not find "
                << "channel named " << name << QwLog::endl;
    }
  }
  return foundit;
}


Int_t QwMainCerenkovDetector::LoadChannelMap(TString mapfile)
{

  Bool_t ldebug=kFALSE;

  TString varname, varvalue;
  TString modtype, dettype, namech, nameofcombinedchan;
  Int_t modnum, channum, combinedchans;
  std::vector<TString> combinedchannelnames;
  std::vector<Double_t> weight;
  Int_t currentrocread=0;
  Int_t currentbankread=0;
  Int_t wordsofar=0;
  Int_t currentsubbankindex=-1;
  Int_t fSample_size=0;

  modnum = 0;
  channum = 0;
  combinedchans = 0;

  QwParameterFile mapstr(mapfile.Data());  //Open the file
  while (mapstr.ReadNextLine())
    {
      mapstr.TrimComment('!');   // Remove everything after a '!' character.
      mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      if (mapstr.LineIsEmpty())  continue;

      if (mapstr.HasVariablePair("=",varname,varvalue))
        {
          //  This is a declaration line.  Decode it.
          varname.ToLower();
          UInt_t value = QwParameterFile::GetUInt(varvalue);

          if (varname=="roc")
            {
              currentrocread=value;
              RegisterROCNumber(value,0);
            }
          else if (varname=="bank")
            {
              currentbankread=value;
              RegisterSubbank(value);
            }
          else if (varname=="sample_size")
            {
              fSample_size=value;
            }
        }
      else
        {
          Bool_t lineok=kTRUE;
          //  Break this line into tokens to process it.
          modtype   = mapstr.GetNextToken(", ").c_str();	// module type
          if (modtype == "VQWK")
            {
              modnum    = (atol(mapstr.GetNextToken(", ").c_str()));	//slot number
              channum   = (atol(mapstr.GetNextToken(", ").c_str()));	//channel number
              dettype   = mapstr.GetNextToken(", ").c_str();	//type-purpose of the detector
              dettype.ToLower();
              namech    = mapstr.GetNextToken(", ").c_str();  //name of the detector
              namech.ToLower();
            }
          else if (modtype == "VPMT")
            {
              channum       = (atol(mapstr.GetNextToken(", ").c_str()));	//channel number
              combinedchans = (atol(mapstr.GetNextToken(", ").c_str()));	//number of combined channels
              dettype   = mapstr.GetNextToken(", ").c_str();	//type-purpose of the detector
              dettype.ToLower();
              namech    = mapstr.GetNextToken(", ").c_str();  //name of the detector
              namech.ToLower();
              //TString nameofchannel;
              combinedchannelnames.clear();
              for (int i=0; i<combinedchans; i++)
                {
                  nameofcombinedchan = mapstr.GetNextToken(", ").c_str();
                  nameofcombinedchan.ToLower();
                  combinedchannelnames.push_back(nameofcombinedchan);
                }
              weight.clear();
              for (int i=0; i<combinedchans; i++)
                {
                  weight.push_back( atof(mapstr.GetNextToken(", ").c_str()));
                }
            }


          if (currentsubbankindex!=GetSubbankIndex(currentrocread,currentbankread))
            {
              currentsubbankindex=GetSubbankIndex(currentrocread,currentbankread);
              wordsofar=0;
            }

          QwMainCerenkovDetectorID localMainDetID;
          localMainDetID.fdetectorname=namech;
          localMainDetID.fmoduletype=modtype;
          localMainDetID.fSubbankIndex=currentsubbankindex;
          localMainDetID.fdetectortype=dettype;

	  //          localMainDetID.fWordInSubbank=wordsofar;
          if (modtype=="VQWK"){
	    Int_t offset = QwVQWK_Channel::GetBufferOffset(modnum, channum);
	    if (offset>0){
	      localMainDetID.fWordInSubbank = wordsofar + offset;
	    }
	  }
          else if (modtype=="VPMT")
            {
              localMainDetID.fCombinedChannelNames = combinedchannelnames;
              localMainDetID.fWeight = weight;
              //std::cout<<"Add in a combined channel"<<std::endl;
            }
          else
            {
              QwError << "QwMainCerenkovDetector::LoadChannelMap:  Unknown module type: "
		      << modtype <<", the detector "<<namech<<" will not be decoded "
		      << QwLog::endl;
              lineok=kFALSE;
              continue;
            }

          localMainDetID.fTypeID=GetDetectorTypeID(dettype);
	  if (localMainDetID.fTypeID==kQwUnknownPMT) {
	    QwError << "QwMainCerenkovDetector::LoadChannelMap:  Unknown detector type: "
		    << dettype <<", the detector "<<namech<<" will not be decoded "
		    << QwLog::endl;
	    lineok=kFALSE;
	    continue;
	  }

          localMainDetID.fIndex= GetDetectorIndex(localMainDetID.fTypeID,
                                                  localMainDetID.fdetectorname);

          if (localMainDetID.fIndex==-1)
            {
              if (localMainDetID.fTypeID==kQwIntegrationPMT)
                {
                  QwIntegrationPMT localIntegrationPMT(GetSubsystemName(),localMainDetID.fdetectorname);
                   fIntegrationPMT.push_back(localIntegrationPMT);
                  fIntegrationPMT[fIntegrationPMT.size()-1].SetDefaultSampleSize(fSample_size);
		  localMainDetID.fIndex=fIntegrationPMT.size()-1;
                }

              else if (localMainDetID.fTypeID==kQwCombinedPMT)
                {
		  QwCombinedPMT localcombinedPMT(GetSubsystemName(),localMainDetID.fdetectorname);
                  fCombinedPMT.push_back(localcombinedPMT);
                  fCombinedPMT[fCombinedPMT.size()-1].SetDefaultSampleSize(fSample_size);
                  localMainDetID.fIndex=fCombinedPMT.size()-1;
                }
            }

          if (ldebug)
            {
              localMainDetID.Print();
              std::cout<<"line ok=";
              if (lineok) std::cout<<"TRUE"<<std::endl;
              else
                std::cout<<"FALSE"<<std::endl;
            }

          if (lineok)
            fMainDetID.push_back(localMainDetID);
        }
    }


  //std::cout<<"linking combined channels"<<std::endl;

  for (size_t i=0; i<fMainDetID.size(); i++)
    {
      if (fMainDetID[i].fTypeID==kQwCombinedPMT)
        {
          Int_t ind = fMainDetID[i].fIndex;

          //check to see if all required channels are available
          if (ldebug)
            {
              std::cout<<"fMainDetID[i].fCombinedChannelNames.size()="
              <<fMainDetID[i].fCombinedChannelNames.size()<<std::endl<<"name list: ";
              for (size_t n=0; n<fMainDetID[i].fCombinedChannelNames.size(); n++)
                std::cout<<"  "<<fMainDetID[i].fCombinedChannelNames[n];
              std::cout<<std::endl;
            }

          Int_t chanmatched=0;
          for (size_t j=0; j<fMainDetID[i].fCombinedChannelNames.size(); j++)
            {

              for (size_t k=0; k<fMainDetID.size(); k++)
                {
                  if (fMainDetID[i].fCombinedChannelNames[j]==fMainDetID[k].fdetectorname)
                    {
                      if (ldebug)
                        std::cout<<"found a to-be-combined channel candidate"<<std::endl;
                      chanmatched ++;
                      break;
                    }
                }
            }

          if ((Int_t) fMainDetID[i].fCombinedChannelNames.size()==chanmatched)
            {
              for (size_t l=0; l<fMainDetID[i].fCombinedChannelNames.size(); l++)
                {
                  Int_t ind_pmt = GetDetectorIndex(GetDetectorTypeID("integrationpmt"),
                                                   fMainDetID[i].fCombinedChannelNames[l]);

                  fCombinedPMT[ind].Add(&fIntegrationPMT[ind_pmt],fMainDetID[i].fWeight[l]);
                }
              fCombinedPMT[ind].LinkChannel(fMainDetID[i].fdetectorname);
              if (ldebug)
                std::cout<<"linked a combined channel"<<std::endl;
            }
          else
            {
              std::cerr<<"cannot combine void channels for "<<fMainDetID[i].fdetectorname<<std::endl;
              fMainDetID[i].fIndex = -1;
              continue;
            }
        }
    }

  if (ldebug)
    {
      std::cout<<"Done with Load channel map\n";
      for (size_t i=0;i<fMainDetID.size();i++)
        if (fMainDetID[i].fIndex>=0)
          fMainDetID[i].Print();
    }
  ldebug=kFALSE;

  return 0;
};


Int_t QwMainCerenkovDetector::LoadEventCuts(TString  filename)
{
  Double_t ULX, LLX, ULY, LLY;
  Int_t samplesize;
  Int_t check_flag;
  Int_t eventcut_flag;
  std::vector<Double_t> integrationPMTEventCuts;
  std::vector<Double_t> combinedPMTEventCuts;

  TString varname, varvalue, vartypeID;
  TString device_type,device_name;
  std::cout<<" QwMainCerenkovDetector::LoadEventCuts  "<<filename<<std::endl;
  QwParameterFile mapstr(filename.Data());  //Open the file

  samplesize = 0;
  check_flag = 0;
  eventcut_flag=1;

  while (mapstr.ReadNextLine())
    {
      //std::cout<<"********* In the loop  *************"<<std::endl;
      mapstr.TrimComment('!');   // Remove everything after a '!' character.
      mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      if (mapstr.LineIsEmpty())  continue;
      if (mapstr.HasVariablePair("=",varname,varvalue))
        {
          if (varname=="EVENTCUTS")
            {
              //varname="";
              eventcut_flag= QwParameterFile::GetUInt(varvalue);
              //std::cout<<"EVENT CUT FLAG "<<eventcut_flag<<std::endl;
            }
        }
      else
        {
          device_type= mapstr.GetNextToken(", ").c_str();
          device_type.ToLower();
          device_name= mapstr.GetNextToken(", ").c_str();
          device_name.ToLower();

          //set limits to zero
          ULX=0;
          LLX=0;
          ULY=0;
          LLY=0;

          if (device_type == "integrationpmt")
            {

              //std::cout<<" device name "<<device_name<<" device flag "<<check_flag<<std::endl;

              LLX = (atof(mapstr.GetNextToken(", ").c_str()));	//lower limit for IntegrationPMT value
              ULX = (atof(mapstr.GetNextToken(", ").c_str()));	//upper limit for IntegrationPMT value

              Int_t det_index=GetDetectorIndex(GetDetectorTypeID(device_type),device_name);
              //std::cout<<"*****************************"<<std::endl;
              //std::cout<<" Type "<<device_type<<" Name "<<device_name<<" Index ["<<det_index <<"] "<<" device flag "<<check_flag<<std::endl;
              //fIntegrationPMT[det_index].Print();
              fIntegrationPMT[det_index].SetSingleEventCuts(LLX,ULX);
              //std::cout<<"*****************************"<<std::endl;

            }

        }

    }
  for (size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].SetEventCutMode(eventcut_flag);

  fMainDetErrorCount=0; //set the error counter to zero

  return 0;
};



Int_t QwMainCerenkovDetector::LoadInputParameters(TString pedestalfile)
{
  Bool_t ldebug=kFALSE;
  TString varname;
  Double_t varped;
  Double_t varcal;
  TString localname;

  Int_t lineread=0;

  if (ldebug)std::cout<<"QwMainCerenkovDetector::LoadInputParameters("<< pedestalfile<<")\n";

  QwParameterFile mapstr(pedestalfile.Data());  //Open the file
  while (mapstr.ReadNextLine())
    {
      lineread+=1;
      if (ldebug)std::cout<<" line read so far ="<<lineread<<"\n";
      mapstr.TrimComment('!');   // Remove everything after a '!' character.
      mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      if (mapstr.LineIsEmpty())  continue;
      else
        {
          varname = mapstr.GetNextToken(", \t").c_str();	//name of the channel
          varname.ToLower();
          varname.Remove(TString::kBoth,' ');
          varped= (atof(mapstr.GetNextToken(", \t").c_str())); // value of the pedestal
          varcal= (atof(mapstr.GetNextToken(", \t").c_str())); // value of the calibration factor
          if (ldebug) std::cout<<"inputs for channel "<<varname
            <<": ped="<<varped<<": cal="<<varcal<<"\n";
          Bool_t notfound=kTRUE;

          if (notfound)
            for (size_t i=0;i<fIntegrationPMT.size();i++)
              if (fIntegrationPMT[i].GetElementName()==varname)
                {
                  fIntegrationPMT[i].SetPedestal(varped);
                  fIntegrationPMT[i].SetCalibrationFactor(varcal);
                  i=fIntegrationPMT.size()+1;
                  notfound=kFALSE;
                  i=fIntegrationPMT.size()+1;
                }
        }

    }
  if (ldebug) std::cout<<" line read in the pedestal + cal file ="<<lineread<<" \n";

  ldebug=kFALSE;
  return 0;
}


Bool_t QwMainCerenkovDetector::IsGoodEvent()
{
  Bool_t test=kTRUE;
  return test;
}

void QwMainCerenkovDetector::ClearEventData()
{
  for (size_t i=0;i<fIntegrationPMT.size();i++){
    fIntegrationPMT[i].ClearEventData();
  }
  for (size_t i=0;i<fCombinedPMT.size();i++)
    fCombinedPMT[i].ClearEventData();

  return;
};


/********************************************************/
void QwMainCerenkovDetector::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  for (size_t i = 0; i < fMainDetID.size(); i++)
    {
      // This is a QwIntegrationPMT
      if (fMainDetID.at(i).fTypeID == kQwIntegrationPMT)
        fIntegrationPMT[fMainDetID.at(i).fIndex].SetRandomEventParameters(mean, sigma);
    }

};

void QwMainCerenkovDetector::SetRandomEventAsymmetry(Double_t asymmetry)
{
  for (size_t i = 0; i < fMainDetID.size(); i++)
    {
      // This is a QwIntegrationPMT
      if (fMainDetID.at(i).fTypeID == kQwIntegrationPMT)
        fIntegrationPMT[fMainDetID.at(i).fIndex].SetRandomEventAsymmetry(asymmetry);
    }

};

void QwMainCerenkovDetector::RandomizeEventData(int helicity, double time)
{
  for (size_t i = 0; i < fMainDetID.size(); i++)
    {
      // This is a QwIntegrationPMT
      if (fMainDetID.at(i).fTypeID == kQwIntegrationPMT)
        fIntegrationPMT[fMainDetID.at(i).fIndex].RandomizeEventData(helicity, time);
    }

}

void QwMainCerenkovDetector::EncodeEventData(std::vector<UInt_t> &buffer)
{
  std::vector<UInt_t> elements;
  elements.clear();

  // Get all buffers in the order they are defined in the map file
  for (size_t i = 0; i < fMainDetID.size(); i++)
    {
      // This is a QwIntegrationPMT
      if (fMainDetID.at(i).fTypeID == kQwIntegrationPMT)
        fIntegrationPMT[fMainDetID.at(i).fIndex].EncodeEventData(elements);
    }

  // If there is element data, generate the subbank header
  std::vector<UInt_t> subbankheader;
  std::vector<UInt_t> rocheader;
  if (elements.size() > 0)
    {

      // Form CODA subbank header
      subbankheader.clear();
      subbankheader.push_back(elements.size() + 1);	// subbank size
      subbankheader.push_back((fCurrentBank_ID << 16) | (0x01 << 8) | (1 & 0xff));
      // subbank tag | subbank type | event number

      // Form CODA bank/roc header
      rocheader.clear();
      rocheader.push_back(subbankheader.size() + elements.size() + 1);	// bank/roc size
      rocheader.push_back((fCurrentROC_ID << 16) | (0x10 << 8) | (1 & 0xff));
      // bank tag == ROC | bank type | event number

      // Add bank header, subbank header and element data to output buffer
      buffer.insert(buffer.end(), rocheader.begin(), rocheader.end());
      buffer.insert(buffer.end(), subbankheader.begin(), subbankheader.end());
      buffer.insert(buffer.end(), elements.begin(), elements.end());
    }
}



Int_t QwMainCerenkovDetector::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  /*  Int_t index = GetSubbankIndex(roc_id,bank_id);
    if (index>=0 && num_words>0){
      //  We want to process the configuration data for this ROC.
      UInt_t words_read = 0;
      for (size_t i = 0; i < fMainDetID.size(); i++) {
        words_read += fIntegrationPMT[i].ProcessConfigurationBuffer(&(buffer[words_read]),
  							   num_words-words_read);
      }
    }*/
  return 0;
};


Int_t QwMainCerenkovDetector::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Bool_t lkDEBUG=kFALSE;

  Int_t index = GetSubbankIndex(roc_id,bank_id);

  if (index>=0 && num_words>0)
    {
      //  We want to process this ROC.  Begin looping through the data.
      if (lkDEBUG)
        std::cout << "QwMainCerenkovDetector::ProcessEvBuffer:  "
        << "Begin processing ROC" << roc_id
        << " and subbank "<<bank_id
        << " number of words="<<num_words<<std::endl;

      for (size_t i=0;i<fMainDetID.size();i++)
        {
          if (fMainDetID[i].fSubbankIndex==index)
            {

              if (fMainDetID[i].fTypeID == kQwIntegrationPMT)
                {
                  if (lkDEBUG)
                    {
                      std::cout<<"found IntegrationPMT data for "<<fMainDetID[i].fdetectorname<<std::endl;
                      std::cout<<"word left to read in this buffer:"<<num_words-fMainDetID[i].fWordInSubbank<<std::endl;
                    }
                  fIntegrationPMT[fMainDetID[i].fIndex].ProcessEvBuffer(&(buffer[fMainDetID[i].fWordInSubbank]),
                      num_words-fMainDetID[i].fWordInSubbank);
                }
            }
        }
    }

  return 0;
};



Bool_t QwMainCerenkovDetector::ApplySingleEventCuts()
{
  //currently this will check the IsGoodEvent() only!
  //std::cout<<" QwMainCerenkovDetector::SingleEventCuts() ";

  Bool_t test_IntegrationPMT=kTRUE;
  Bool_t test_IntegrationPMT1=kTRUE;

  for (size_t i=0;i<fIntegrationPMT.size();i++)
    {
      //std::cout<<"  IntegrationPMT ["<<i<<"] "<<std::endl;
      test_IntegrationPMT1=fIntegrationPMT[i].ApplySingleEventCuts();
      test_IntegrationPMT&=test_IntegrationPMT1;
      if (!test_IntegrationPMT1 && bDEBUG)
        std::cout<<"******* QwMainCerenkovDetector::SingleEventCuts()->IntegrationPMT[ "
        <<i<<" , "<<fIntegrationPMT[i].GetElementName()<<" ] ******\n";
    }


  //if (!test_IntegrationPMT)
  //fNumError_Evt_IntegrationPMT++;//IntegrationPMT falied  event counter for QwMainCerenkovDetector

  if (!test_IntegrationPMT1 || !test_IntegrationPMT)
    fMainDetErrorCount++;

  return test_IntegrationPMT;

};


Int_t QwMainCerenkovDetector::GetEventcutErrorFlag() //return the error flag
{

  return 0;

}

//inherited from the VQwSubsystemParity; this will display the error summary
Int_t QwMainCerenkovDetector::GetEventcutErrorCounters()
{

  std::cout<<"*********QwMainCerenkovDetector Error Summary****************"<<std::endl;
  std::cout<<"Device name ||  Sample || SW_HW || Sequence || SameHW || EventCut\n";
  for (size_t i=0;i<fIntegrationPMT.size();i++)
    {
      //std::cout<<"  IntegrationPMT ["<<i<<"] "<<std::endl;
      fIntegrationPMT[i].ReportErrorCounters();
    }
  //std::cout<<"Total failed events "<<  fMainDetErrorCount<<std::endl;
  //std::cout<<"*********End of error QwMainCerenkovDetector reporting****************"<<std::endl;

  return 1;
}


void  QwMainCerenkovDetector::ProcessEvent()
{
  for (size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].ProcessEvent();
  //   fIntegrationPMT[0].Print();

  for (size_t i=0;i<fCombinedPMT.size();i++)
    {
      //std::cout<<"Process combination "<<i<<std::endl;
      fCombinedPMT[i].ProcessEvent();

    }

  return;
};

/**
 * Exchange data between subsystems
 */
void  QwMainCerenkovDetector::ExchangeProcessedData()
{
  //QwWarning << "QwMainCerenkovDetector::ExchangeProcessedData "<< QwLog::endl;
  bIsExchangedDataValid = kTRUE;
  if (bNormalization){
    // Create a list of all variables that we need
    // TODO This could be a static list to avoid repeated vector initializiations
    /*
    std::vector<VQwDataElement*> variable_list;
    variable_list.push_back(&fTargetCharge);
    variable_list.push_back(&fTargetX);
    variable_list.push_back(&fTargetY);
    variable_list.push_back(&fTargetXprime);
    variable_list.push_back(&fTargetYprime);
    variable_list.push_back(&fTargetEnergy);
    */

    // Loop over all variables in the list
    /*
      //IMPORTANT NOTE ABOUT THE COMMENTED LOOP
      //NAMES OF THE variable_list DEVICES WILL BE OVER WRITTEN BY ACTUAL NAMES OF DATA ELEMENTS THAT WE ARE READING FROM THE
      //ROUTINE RequestExternalValue(variable->GetElementName(), variable). DO TRY THIS AT HOME!
      

    std::vector<VQwDataElement*>::iterator variable_iter;
    for (variable_iter  = variable_list.begin(); variable_iter != variable_list.end(); variable_iter++){
	VQwDataElement* variable = *variable_iter;
	//QwWarning << "QwMainCerenkovDetector::ExchangeProcessedData for loop "<<variable->GetElementName()<< QwLog::endl;
	if (RequestExternalValue(variable->GetElementName(), variable))
	  {
	    if (bDEBUG)
	      dynamic_cast<QwVQWK_Channel*>(variable)->PrintInfo();
	    //QwWarning << "QwMainCerenkovDetector::ExchangeProcessedData Found "<<variable->GetElementName()<< QwLog::endl;
	  }
	else
	  {
	    bIsExchangedDataValid = kFALSE;
	    QwError << GetSubsystemName() << " could not get external value for "
		    << variable->GetElementName() << QwLog::endl;
	  }
      } // end of loop over variables

    */

    if(RequestExternalValue("q_targ", &fTargetCharge)){
      if (bDEBUG){
	QwWarning << "QwMainCerenkovDetector::ExchangeProcessedData Found "<<fTargetCharge.GetElementName()<< QwLog::endl;
	//QwWarning <<"****QwMainCerenkovDetector****"<< QwLog::endl;
	(dynamic_cast<QwVQWK_Channel*>(&fTargetCharge))->PrintInfo();
      }
    }else{
      bIsExchangedDataValid = kFALSE;
      QwError << GetSubsystemName() << " could not get external value for "
	      << fTargetCharge.GetElementName() << QwLog::endl;
    }

    if(RequestExternalValue("x_targ", &fTargetX)){
      if (bDEBUG){
	dynamic_cast<QwVQWK_Channel*>(&fTargetX)->PrintInfo();
	QwWarning << "QwMainCerenkovDetector::ExchangeProcessedData Found "<<fTargetX.GetElementName()<< QwLog::endl;  
      }    
    }else{
      bIsExchangedDataValid = kFALSE;
      QwError << GetSubsystemName() << " could not get external value for "
	      << fTargetX.GetElementName() << QwLog::endl;
    }

    if(RequestExternalValue("y_targ", &fTargetY)){
      if (bDEBUG){
	dynamic_cast<QwVQWK_Channel*>(&fTargetY)->PrintInfo();
	QwWarning << "QwMainCerenkovDetector::ExchangeProcessedData Found "<<fTargetY.GetElementName()<< QwLog::endl;
      }
    }else{
      bIsExchangedDataValid = kFALSE;
      QwError << GetSubsystemName() << " could not get external value for "
	      << fTargetY.GetElementName() << QwLog::endl;
    }

    if(RequestExternalValue("xp_targ", &fTargetXprime)){
      if (bDEBUG){
	dynamic_cast<QwVQWK_Channel*>(&fTargetXprime)->PrintInfo();
	QwWarning << "QwMainCerenkovDetector::ExchangeProcessedData Found "<<fTargetXprime.GetElementName()<< QwLog::endl;
      }
    }else{
      bIsExchangedDataValid = kFALSE;
      QwError << GetSubsystemName() << " could not get external value for "
	      << fTargetXprime.GetElementName() << QwLog::endl;
    }

    if(RequestExternalValue("yp_targ", &fTargetYprime)){
      if (bDEBUG){
	dynamic_cast<QwVQWK_Channel*>(&fTargetYprime)->PrintInfo();
	QwWarning << "QwMainCerenkovDetector::ExchangeProcessedData Found "<<fTargetYprime.GetElementName()<< QwLog::endl;
      }
    }else{
      bIsExchangedDataValid = kFALSE;
      QwError << GetSubsystemName() << " could not get external value for "
	      << fTargetYprime.GetElementName() << QwLog::endl;
    }

    if(RequestExternalValue("e_targ", &fTargetEnergy)){
      if (bDEBUG){
	dynamic_cast<QwVQWK_Channel*>(&fTargetEnergy)->PrintInfo();
	QwWarning << "QwMainCerenkovDetector::ExchangeProcessedData Found "<<fTargetEnergy.GetElementName()<< QwLog::endl;
      }
    }else{
      bIsExchangedDataValid = kFALSE;
      QwError << GetSubsystemName() << " could not get external value for "
	      << fTargetEnergy.GetElementName() << QwLog::endl;
    }

    
  }
};


void  QwMainCerenkovDetector::ProcessEvent_2()
{
  if (bIsExchangedDataValid)
    {
      //data is valid, process it
      if (bDEBUG)
        {
          Double_t  pedestal = fTargetCharge.GetPedestal();
          Double_t  calfactor = fTargetCharge.GetCalibrationFactor();
          Double_t  volts = fTargetCharge.GetAverageVolts();
          std::cout<<"QwMainCerenkovDetector::ProcessEvent_2(): processing with exchanged data"<<std::endl;
          std::cout<<"pedestal, calfactor, average volts = "<<pedestal<<", "<<calfactor<<", "<<volts<<std::endl;
        }

      // assume fTargetCharge.fHardwareSum is a calibrated value,
      // detector signals will be normalized to it
      if (bNormalization) this->DoNormalization();
    }
  else
    {
      QwError<<"QwMainCerenkovDetector::ProcessEvent_2(): could not get all external values."<<QwLog::endl;
    }
};




void  QwMainCerenkovDetector::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  for (size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].ConstructHistograms(folder,prefix);

  for (size_t i=0;i<fCombinedPMT.size();i++)
    fCombinedPMT[i].ConstructHistograms(folder,prefix);
  return;
};


void  QwMainCerenkovDetector::FillHistograms()
{
  for (size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].FillHistograms();

  for (size_t i=0;i<fCombinedPMT.size();i++)
    fCombinedPMT[i].FillHistograms();

  return;
};


void  QwMainCerenkovDetector::DeleteHistograms()
{
  for (size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].DeleteHistograms();

  for (size_t i=0;i<fCombinedPMT.size();i++)
    fCombinedPMT[i].DeleteHistograms();

  return;
};


void QwMainCerenkovDetector::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  for (size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].ConstructBranchAndVector(tree, prefix, values);

  for (size_t i=0;i<fCombinedPMT.size();i++)
    fCombinedPMT[i].ConstructBranchAndVector(tree, prefix, values);

  return;
};

void QwMainCerenkovDetector::ConstructBranch(TTree *tree, TString & prefix)
{
  for (size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].ConstructBranch(tree, prefix);

  for (size_t i=0;i<fCombinedPMT.size();i++)
    fCombinedPMT[i].ConstructBranch(tree, prefix);

  return;
};

void QwMainCerenkovDetector::ConstructBranch(TTree *tree, TString & prefix, QwParameterFile& trim_file)
{
  TString tmp;
  QwParameterFile* nextmodule;
  trim_file.RewindToFileStart();
  tmp="QwIntegrationPMT";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for (size_t i=0;i<fIntegrationPMT.size();i++)

      fIntegrationPMT[i].ConstructBranch(tree, prefix, *nextmodule);
  }

  tmp="QwCombinedPMT";
  trim_file.RewindToFileStart();
   if (trim_file.FileHasModuleHeader(tmp)){
     nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
     for (size_t i=0;i<fCombinedPMT.size();i++)
       fCombinedPMT[i].ConstructBranch(tree, prefix, *nextmodule );
   }

  return;
};

void QwMainCerenkovDetector::FillTreeVector(std::vector<Double_t> &values) const
{
  for (size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].FillTreeVector(values);

  for (size_t i=0;i<fCombinedPMT.size();i++)
    fCombinedPMT[i].FillTreeVector(values);

  return;
};


const QwIntegrationPMT* QwMainCerenkovDetector::GetChannel(const TString name) const
{
  return GetIntegrationPMT(name);
};


void  QwMainCerenkovDetector::Copy(VQwSubsystem *source)
{

  try
    {
      if (typeid(*source)==typeid(*this))
        {
          VQwSubsystem::Copy(source);
          QwMainCerenkovDetector* input= dynamic_cast<QwMainCerenkovDetector*>(source);

          this->fIntegrationPMT.resize(input->fIntegrationPMT.size());
          for (size_t i=0;i<this->fIntegrationPMT.size();i++)
            this->fIntegrationPMT[i].Copy(&(input->fIntegrationPMT[i]));

          this->fCombinedPMT.resize(input->fCombinedPMT.size());
          for (size_t i=0;i<this->fCombinedPMT.size();i++)
            this->fCombinedPMT[i].Copy(&(input->fCombinedPMT[i]));
        }
      else
        {
          TString loc="Standard exception from QwMainCerenkovDetector::Copy = "
                      +source->GetSubsystemName()+" "
                      +this->GetSubsystemName()+" are not of the same type";
          throw std::invalid_argument(loc.Data());
        }
    }
  catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }
  // this->Print();

  return;
}


VQwSubsystem*  QwMainCerenkovDetector::Copy()
{
  QwMainCerenkovDetector* TheCopy = new QwMainCerenkovDetector("MainDetector Copy");
  TheCopy->Copy(this);
  return TheCopy;
}


Bool_t QwMainCerenkovDetector::Compare(VQwSubsystem *value)
{
  //  std::cout<<" Here in QwLumi::Compare \n";

  Bool_t res=kTRUE;
  if (typeid(*value)!=typeid(*this))
    {
      res=kFALSE;
      //      std::cout<<" types are not ok \n";
      //      std::cout<<" this is bypassed just for now but should be fixed eventually \n";
    }
  else
    {
      QwMainCerenkovDetector* input = dynamic_cast<QwMainCerenkovDetector*>(value);
      if (input->fIntegrationPMT.size()!=fIntegrationPMT.size() ||
          input->fCombinedPMT.size()!=fCombinedPMT.size() )
        {
          res=kFALSE;
          //	  std::cout<<" not the same number of channels \n";
        }
    }
  return res;
}


VQwSubsystem&  QwMainCerenkovDetector::operator=  (VQwSubsystem *value)
{
  //  std::cout<<" here in QwMainCerenkovDetector::operator= \n";
  if (Compare(value))
    {
//      VQwSubsystem::operator=(value);
      QwMainCerenkovDetector* input = dynamic_cast<QwMainCerenkovDetector*> (value);

      for (size_t i=0;i<input->fIntegrationPMT.size();i++)
        this->fIntegrationPMT[i]=input->fIntegrationPMT[i];

      for (size_t i=0;i<input->fCombinedPMT.size();i++)
        (this->fCombinedPMT[i])=(input->fCombinedPMT[i]);
    }
  return *this;
};


VQwSubsystem&  QwMainCerenkovDetector::operator+=  (VQwSubsystem *value)
{
  if (Compare(value))
    {
      QwMainCerenkovDetector* input= dynamic_cast<QwMainCerenkovDetector*>(value) ;

      for (size_t i=0;i<input->fIntegrationPMT.size();i++)
        this->fIntegrationPMT[i]+=input->fIntegrationPMT[i];

      for (size_t i=0;i<input->fCombinedPMT.size();i++)
        this->fCombinedPMT[i]+=input->fCombinedPMT[i];

    }
  return *this;
};


VQwSubsystem&  QwMainCerenkovDetector::operator-=  (VQwSubsystem *value)
{

  if (Compare(value))
    {
      QwMainCerenkovDetector* input= dynamic_cast<QwMainCerenkovDetector*>(value);

      for (size_t i=0;i<input->fIntegrationPMT.size();i++)
        this->fIntegrationPMT[i]-=input->fIntegrationPMT[i];

      for (size_t i=0;i<input->fCombinedPMT.size();i++)
        this->fCombinedPMT[i]-=input->fCombinedPMT[i];

    }
  return *this;
};



void QwMainCerenkovDetector::Sum(VQwSubsystem *value1,VQwSubsystem *value2)
{
  if (Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this += value2;
    }
};

void QwMainCerenkovDetector::Difference(VQwSubsystem *value1,VQwSubsystem *value2)
{
  if (Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this -= value2;
    }
};

void QwMainCerenkovDetector::Ratio(VQwSubsystem  *numer, VQwSubsystem  *denom)
{
  if (Compare(numer)&&Compare(denom))
    {
      QwMainCerenkovDetector* innumer= dynamic_cast<QwMainCerenkovDetector*>(numer) ;
      QwMainCerenkovDetector* indenom= dynamic_cast<QwMainCerenkovDetector*>(denom) ;

      for (size_t i=0;i<innumer->fIntegrationPMT.size();i++)
        this->fIntegrationPMT[i].Ratio(innumer->fIntegrationPMT[i],indenom->fIntegrationPMT[i]);

      for (size_t i=0;i<innumer->fCombinedPMT.size();i++)
        this->fCombinedPMT[i].Ratio(innumer->fCombinedPMT[i],indenom->fCombinedPMT[i]);

    }
  return;
};


void QwMainCerenkovDetector::Scale(Double_t factor)
{
  for (size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].Scale(factor);

  for (size_t i=0;i<fCombinedPMT.size();i++)
    fCombinedPMT[i].Scale(factor);

  return;
};


void QwMainCerenkovDetector::CalculateRunningAverage()
{
  for (size_t i=0;i<fIntegrationPMT.size();i++)
    fIntegrationPMT[i].CalculateRunningAverage();

  for (size_t i=0;i<fCombinedPMT.size();i++)
    fCombinedPMT[i].CalculateRunningAverage();

  return;
};

void QwMainCerenkovDetector::AccumulateRunningSum(VQwSubsystem* value1)
{
  if (Compare(value1)) {
    QwMainCerenkovDetector* value = dynamic_cast<QwMainCerenkovDetector*>(value1);

    for (size_t i = 0; i < fIntegrationPMT.size(); i++)
      fIntegrationPMT[i].AccumulateRunningSum(value->fIntegrationPMT[i]);
    for (size_t i = 0; i < fCombinedPMT.size(); i++)
      fCombinedPMT[i].AccumulateRunningSum(value->fCombinedPMT[i]);
  }
};


/**
 * Blind the asymmetry
 * @param blinder Blinder
 */
void QwMainCerenkovDetector::Blind(const QwBlinder *blinder)
{
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].Blind(blinder);
  for (size_t i = 0; i < fCombinedPMT.size(); i++)
    fCombinedPMT[i].Blind(blinder);
}

/**
 * Blind the difference using the yield
 * @param blinder Blinder
 * @param yield Corresponding yield
 */
void QwMainCerenkovDetector::Blind(const QwBlinder *blinder, const VQwSubsystemParity* subsys)
{
  /// \todo TODO (wdc) At some point we should introduce const-correctness in
  /// the Compare() routine to ensure nothing funny happens.  This const_casting
  /// is just an ugly stop-gap measure.
  if (Compare(const_cast<VQwSubsystemParity*>(subsys))) {

    const QwMainCerenkovDetector* yield = dynamic_cast<const QwMainCerenkovDetector*>(subsys);
    if (yield == 0) return;

    for (size_t i = 0; i < fIntegrationPMT.size(); i++)
      fIntegrationPMT[i].Blind(blinder, yield->fIntegrationPMT[i]);
    for (size_t i = 0; i < fCombinedPMT.size(); i++)
      fCombinedPMT[i].Blind(blinder, yield->fCombinedPMT[i]);
  }
}

EQwPMTInstrumentType QwMainCerenkovDetector::GetDetectorTypeID(TString name)
{
  return GetQwPMTInstrumentType(name);
};

//*****************************************************************
Int_t QwMainCerenkovDetector::GetDetectorIndex(EQwPMTInstrumentType type_id, TString name)
{
  Bool_t ldebug=kFALSE;
  if (ldebug)
    {
      std::cout<<"QwMainCerenkovDetector::GetDetectorIndex\n";
      std::cout<<"type_id=="<<type_id<<" name="<<name<<"\n";
      std::cout<<fMainDetID.size()<<" already registered detector\n";
    }

  Int_t result=-1;
  for (size_t i=0;i<fMainDetID.size();i++)
    {
      if (fMainDetID[i].fTypeID==type_id)
        if (fMainDetID[i].fdetectorname==name)
          {
            result=fMainDetID[i].fIndex;
            if (ldebug)
              std::cout<<"testing against ("<<fMainDetID[i].fTypeID
              <<","<<fMainDetID[i].fdetectorname<<")=>"<<result<<"\n";
          }
    }

  return result;
};

const QwIntegrationPMT* QwMainCerenkovDetector::GetIntegrationPMT(const TString name) const
{
  TString tmpname = name;
  tmpname.ToLower();
  if (! fIntegrationPMT.empty())
    {
      for (size_t i=0;i<fIntegrationPMT.size();i++)
        {
          if (fIntegrationPMT.at(i).GetElementName() == tmpname)
            {
              //std::cout<<"Get IntegrationPMT "<<tmpname<<std::endl;
              return &(fIntegrationPMT.at(i));
            }
        }
    }
  QwMessage << "QwMainCerenkovDetector::GetIntegrationPMT: cannot find channel " << tmpname << QwLog::endl;
  return NULL;
};

const QwCombinedPMT* QwMainCerenkovDetector::GetCombinedPMT(const TString name) const
{
  TString tmpname = name;
  tmpname.ToLower();
  if (! fCombinedPMT.empty())
    {
      for (size_t i=0;i<fCombinedPMT.size();i++)
        {
          if (fCombinedPMT.at(i).GetElementName() == tmpname)
            {
              //std::cout<<"Get CombinedPMT "<<tmpname<<std::endl;
              return &(fCombinedPMT.at(i));
            }
        }
    }
  QwMessage << "QwMainCerenkovDetector::GetCombinedPMT: cannot find channel " << tmpname << QwLog::endl;
  return NULL;
};

void QwMainCerenkovDetector::DoNormalization(Double_t factor)
{
  static Bool_t notwarned = kTRUE;

  if (bIsExchangedDataValid)
    {
      try
        {
          Double_t  norm = fTargetCharge.GetHardwareSum()*factor;
	  if (norm >1e-9){
	    this->Scale(1.0/norm);
	    notwarned = kTRUE;
	  } else if (notwarned) {
	    notwarned = kFALSE;
	    QwError << "QwMainCerenkovDetector::DoNormalization:  Charge is too small to do the "
		    << "normalization (fTargetCharge==" << fTargetCharge.GetHardwareSum()
		    << ")" << QwLog::endl;
	  }
        }
      catch (std::exception& e)
        {
          std::cerr << e.what() << std::endl;
        }
    }
}

void  QwMainCerenkovDetector::FillDB(QwDatabase *db, TString datatype)
{
  Bool_t local_print_flag = true;
  if(local_print_flag) {
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
    QwMessage << "            QwMainCerenkovDetector::FillDB                       " << QwLog::endl;
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  }

  std::vector<QwDBInterface> interface;
  std::vector<QwParityDB::md_data> entrylist;

  UInt_t analysis_id = db->GetAnalysisID();

  Char_t measurement_type[4];

  if(datatype.Contains("yield")) {
    sprintf(measurement_type, "%s", "y");
  }
  else if (datatype.Contains("asymmetry")) {
    sprintf(measurement_type, "%s", "a");
  }
  else {
    sprintf(measurement_type, "%s", "");
  }

  UInt_t i,j;
  i = j = 0;
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "IntegrationPMT" <<QwLog::endl;

  for(i=0; i<fIntegrationPMT.size(); i++) {
    interface.clear();
    interface = fIntegrationPMT[i].GetDBEntry();
    for(j=0; j<interface.size(); j++) {
      interface.at(j).SetAnalysisID( analysis_id );
      interface.at(j).SetMainDetectorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type );
      interface.at(j).PrintStatus( local_print_flag );
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }

  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "Combined PMT" <<QwLog::endl;

  for(i=0; i< fCombinedPMT.size(); i++)
    {
      interface.clear();
      interface = fCombinedPMT[i].GetDBEntry();
      for(j=0; j<interface.size(); j++) {
	interface.at(j).SetAnalysisID( analysis_id );
	interface.at(j).SetMainDetectorID( db );
	interface.at(j).SetMeasurementTypeID( measurement_type );
	interface.at(j).PrintStatus( local_print_flag );
	interface.at(j).AddThisEntryToList( entrylist );
      }
    }
  if(local_print_flag) {
    QwMessage << QwColor(Qw::kGreen) << "Entrylist Size : "
	      << QwColor(Qw::kBoldRed) << entrylist.size() << QwLog::endl;
  }

  db->Connect();
  // Check the entrylist size, if it isn't zero, start to query..
  if( entrylist.size() ) {
    mysqlpp::Query query= db->Query();
    query.insert(entrylist.begin(), entrylist.end());
    query.execute();
  }
  else {
    QwMessage << "QwMainCerenkovDetector::FillDB :: This is the case when the entrlylist contains nothing in "<< datatype.Data() << QwLog::endl;
  }
  db->Disconnect();
  return;
};

void  QwMainCerenkovDetector::PrintValue() const
{
  QwMessage << "=== QwMainCerenkovDetector: " << GetSubsystemName() << " ===" << QwLog::endl;
  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].PrintValue();
  for (size_t i = 0; i < fCombinedPMT.size(); i++)
    fCombinedPMT[i].PrintValue();
}

void  QwMainCerenkovDetector::PrintInfo() const
{
  std::cout<<"Name of the subsystem ="<<fSystemName<<"\n";

  std::cout<<"there are "<<fIntegrationPMT.size()<<" IntegrationPMT \n";
  std::cout<<"          "<<fCombinedPMT.size()<<" CombinedPMT \n";

  std::cout<<" Printing Running AVG and other channel info"<<std::endl;

  for (size_t i = 0; i < fIntegrationPMT.size(); i++)
    fIntegrationPMT[i].PrintInfo();
  for (size_t i = 0; i < fCombinedPMT.size(); i++)
    fCombinedPMT[i].PrintInfo();
}

void  QwMainCerenkovDetector::PrintDetectorID() const
{
  for (size_t i=0;i<fMainDetID.size();i++)
    {
      std::cout<<"============================="<<std::endl;
      std::cout<<" Detector ID="<<i<<std::endl;
      fMainDetID[i].Print();
    }
  return;
}

void  QwMainCerenkovDetectorID::Print() const
{

  std::cout<<std::endl<<"Detector name= "<<fdetectorname<<std::endl;
  std::cout<<"SubbankkIndex= "<<fSubbankIndex<<std::endl;
  std::cout<<"word index in subbank= "<<fWordInSubbank<<std::endl;
  std::cout<<"module type= "<<fmoduletype<<std::endl;
  std::cout<<"detector type= "<<fdetectortype<<"    index= "<<fTypeID<<std::endl;
  std::cout<<"Index of this detector in the vector of similar detector= "<<fIndex<<std::endl;
  std::cout<<"Subelement index= "<<fSubelement<<std::endl;
  std::cout<<"==========================================\n";

  return;
}


