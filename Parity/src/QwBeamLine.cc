/**********************************************************\
* File: QwBeamLine.C                                      *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwBeamLine.h"

// System headers
#include <iostream>
#include <stdexcept>

// Qweak headers
#include "QwLog.h"
#define MYSQLPP_SSQLS_NO_STATICS
#include "QwParitySSQLS.h"
#include "QwParityDB.h"

// Forward declarations
class QwParityDB;
//class QwDBInterface;

// Register this subsystem with the factory
RegisterSubsystemFactory(QwBeamLine);

//*****************************************************************//
void QwBeamLine::ProcessOptions(QwOptions &options){
      //Handle command line options
}

//*****************************************************************//
template <>
Int_t QwBeamLine::AddToElementList<VQwClock_ptr>(std::vector<VQwClock_ptr> &elementlist,
				   QwBeamDetectorID &detector_id)
{
  if (detector_id.fTypeID == kQwClock){
    VQwClock_ptr element( 
		       VQwClock::Create(GetSubsystemName(),
				      detector_id.fdetectorname,
				      detector_id.fmoduletype) );
    elementlist.push_back(element);
  }
  detector_id.fIndex = elementlist.size()-1;
  return detector_id.fIndex;
}
//*****************************************************************//
template <>
Int_t QwBeamLine::AddToElementList<VQwBCM_ptr>(std::vector<VQwBCM_ptr> &elementlist,
				   QwBeamDetectorID &detector_id)
{
  if (detector_id.fTypeID == kQwBCM){
    VQwBCM_ptr element( 
		       VQwBCM::Create(GetSubsystemName(),
				      detector_id.fdetectorname,
				      detector_id.fmoduletype) );
    elementlist.push_back(element);
  }
  detector_id.fIndex = elementlist.size()-1;
  return detector_id.fIndex;
}
//*****************************************************************//
template <>
Int_t QwBeamLine::AddToElementList<VQwBPM_ptr>(std::vector<VQwBPM_ptr> &elementlist,
				   QwBeamDetectorID &detector_id)
{
  if (detector_id.fTypeID == kQwBPMStripline){
    VQwBPM_ptr element( 
	VQwBPM::CreateStripline(GetSubsystemName(),
				detector_id.fdetectorname,
				detector_id.fmoduletype) );
    elementlist.push_back(element);
  }
  detector_id.fIndex = elementlist.size()-1;
  return detector_id.fIndex;
}

//*****************************************************************//
template <typename TT>
Int_t QwBeamLine::AddToElementList(std::vector<TT> &elementlist,
				   QwBeamDetectorID &detector_id)
{
  TT element(GetSubsystemName(), detector_id.fdetectorname);
  elementlist.push_back(element);
  detector_id.fIndex = elementlist.size()-1;
  return detector_id.fIndex;
}



//*****************************************************************//
Int_t QwBeamLine::LoadChannelMap(TString mapfile)
{
  Bool_t ldebug=kFALSE;

  TString varname, varvalue;

  TString combotype, comboname, dev_name;
  TString clock_norm_channel;

  Int_t   index = 0;
  Bool_t  combolistdecoded;
  Bool_t  deviceok;

  std::vector<TString>  fDeviceName;
  std::vector<TString>  fProperty;
  std::vector<TString>  fType;
  std::vector<Double_t> fQWeight;
  std::vector<Double_t> fXWeight;
  std::vector<Double_t> fYWeight;

  std::vector<QwBeamDetectorID> clock_needed_list;

  QwParameterFile mapstr(mapfile.Data());  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  mapstr.EnableGreediness();
  mapstr.SetCommentChars("!");
  mapstr.AddBreakpointKeyword("begin");
  mapstr.AddBreakpointKeyword("end");

  while (mapstr.ReadNextLine()
	 && mapstr.SkipSection("PUBLISH") ){
    UInt_t value;
    if (mapstr.PopValue("roc",value)){
      //      currentrocread=value;
      RegisterROCNumber(value,0);
    }
    if (mapstr.PopValue("bank",value)){
      //      currentbankread=value;
      RegisterSubbank(value);
    }

    // FIXME (wdc) sample_size is unused, needs to be removed or used
    /*
    Int_t sample_size = 0;
    if (mapstr.ReturnValue("sample_size",value)){
      sample_size=value;
    }
    */

    ///TODO:  The clock info ought to just be passed as a keyword in
    ///       the parameter file object, but for now, get it as a
    ///       local variable.
    if (mapstr.ReturnValue("normclock",varvalue)){
      clock_norm_channel = varvalue;
      clock_norm_channel.ToLower();
    }
    


    if (mapstr.HasVariablePair("=",varname,varvalue)){ //  This is a declaration line.  Decode it.
      varname.ToLower();

      if (varname=="begin"){
	
	// Start to decode derived beamline devices combined+energy
	deviceok = kTRUE;
	combotype = varvalue;
	combolistdecoded = kFALSE;

	TString dettype;
	
	while(mapstr.ReadNextLine()&&!combolistdecoded){
	  if (mapstr.HasVariablePair("=",varname,varvalue)){
	    varname.ToLower();
	    if (varname=="end"){
	      // calculate the total weights of the charge
	      for(size_t i=0;i<fDeviceName.size();i++)
		fSumQweights+=fQWeight[i];
	      combolistdecoded = kTRUE;
	      break;
	    }
	  }

	  if (mapstr.PopValue("name",varvalue)){
	    comboname = varvalue;
	  }
	  
	  dev_name = mapstr.GetTypedNextToken<TString>();
	  dev_name.ToLower();
	  dettype  = mapstr.GetTypedNextToken<TString>();
	  dettype.ToLower();

	  // Check to see if the device being read is a valid physical device.
	  // If not, discard the combination.
	  index=GetDetectorIndex(GetQwBeamInstrumentType(dettype),dev_name);

	  if (index == -1) {
	    QwError << "QwBeamLine::LoadChannelMap:  Unknown device: "
		    <<  dev_name <<" used in "<< comboname
		    <<". This combination  will not be decoded!"
		    <<  QwLog::endl;
	    deviceok = kFALSE;
	    combolistdecoded = kTRUE;
	  }else{
	    // Found the device
	    // Add to the array of names
	    fDeviceName.push_back(dev_name);
	    
	    // Read in the weights.
	    // For combined bpms and combined bcms these are charge weights.
	    // For the energy calculator these are the ratios of the transport matrix elements.
	    fQWeight.push_back(mapstr.GetTypedNextToken<Double_t>());
	    
	    // For combined BPMs,in addition, there are weights for the X & Y positions.
	    if(combotype == "combinedbpm"){
	      fXWeight.push_back(mapstr.GetTypedNextToken<Double_t>());
	      fYWeight.push_back(mapstr.GetTypedNextToken<Double_t>());
	    }
	    
	    // For the enrgy calculator there are device type and the specified beam parameters.
	    if(combotype == "energycalculator"){
	      fType.push_back(dettype);
	      fProperty.push_back(mapstr.GetTypedNextToken<TString>());
	    }
	  }
	}
	
	// Now create the combined device
	QwBeamDetectorID localComboID(-1, -1, comboname, combotype,
				      fBeamDetectorID.at(index).fmoduletype );

	localComboID.fdetectorname=comboname(0,comboname.Sizeof()-1);
	localComboID.fIndex = GetDetectorIndex(localComboID.fTypeID,localComboID.fdetectorname);

	if(localComboID.fTypeID==kQwUnknownDeviceType){
	  QwError << "QwBeamLine::LoadChannelMap:  Unknown detector type: "
		  << combotype <<", the detector "<<comboname<<" will not be decoded "
		  << QwLog::endl;
	  deviceok = kFALSE;
	  continue;
	}

	if((localComboID.fIndex==-1) && deviceok) {

	  // Decoding combined BCM array
	  if (localComboID.fTypeID == kQwCombinedBCM){

	    VQwBCM_ptr localbcmcombo(
		VQwBCM::CreateCombo(GetSubsystemName(),
				    localComboID.fdetectorname,localComboID.fmoduletype));
	    fBCMCombo.push_back(localbcmcombo);

	    for(size_t i=0;i<fDeviceName.size();i++){
	      index=GetDetectorIndex(GetQwBeamInstrumentType(dettype),fDeviceName[i]);
	      fBCMCombo[fBCMCombo.size()-1].get()->SetBCMForCombo(fBCM.at(index).get(),
            fQWeight[i],fSumQweights );
	    }
	    fDeviceName.clear();
	    fQWeight.clear();
	    localComboID.fIndex=fBCMCombo.size()-1;
	  }

	  // Decoding combined BPM array.
	  if(localComboID.fTypeID== kQwCombinedBPM){
	    VQwBPM_ptr localbpmcombo(
                VQwBPM::CreateCombo(GetSubsystemName(),
				    localComboID.fdetectorname,localComboID.fmoduletype));
	    fBPMCombo.push_back(localbpmcombo);

	    for(size_t i=0;i<fDeviceName.size();i++){
	      index=GetDetectorIndex(GetQwBeamInstrumentType(dettype),fDeviceName[i]);
	      fBPMCombo[fBPMCombo.size()-1].get()->SetBPMForCombo(
            fStripline.at(index).get(), fQWeight[i],fXWeight[i],
            fYWeight[i],fSumQweights  );

	    }
	    fDeviceName.clear();
	    fQWeight.clear();
	    fXWeight.clear();
	    fYWeight.clear();
	    localComboID.fIndex=fBPMCombo.size()-1;
	  }

	  // Decoding energy calculator.
	  if(localComboID.fTypeID== kQwEnergyCalculator){

	    QwEnergyCalculator localecalculator(GetSubsystemName(), localComboID.fdetectorname);
	    fECalculator.push_back(localecalculator);

	    for(size_t i=0;i<fDeviceName.size();i++){
	      index=GetDetectorIndex(GetQwBeamInstrumentType(fType[i]),fDeviceName[i]);

	      if ( GetQwBeamInstrumentType(fType[i]) == kQwBPMStripline)
		fECalculator[fECalculator.size()-1].Set(fStripline.at(index).get(),fType[i],fProperty[i],fQWeight[i]);

	      if ( GetQwBeamInstrumentType(fType[i]) == kQwCombinedBPM)
		fECalculator[fECalculator.size()-1].Set(fBPMCombo.at(index).get(),fType[i],fProperty[i],fQWeight[i]);

	    }

	    fDeviceName.clear();
	    fQWeight.clear();

	    fProperty.clear();
	    fType.clear();
	    localComboID.fIndex=fECalculator.size()-1;
	  }
	}
	// Use only the combinations that are of known type and has known physical devices.
	if(deviceok)
	  fBeamDetectorID.push_back(localComboID);
      }

      QwError << "At end of processing the combined device " << QwLog::endl;

    } else{
      // Start to decode the physical beamline devices
      QwBeamDetectorID localBeamDetectorID(GetSubbankIndex(), mapstr);
      Bool_t lineok = localBeamDetectorID.ReportInitErrors();
      if (! lineok) continue;

      localBeamDetectorID.fIndex=
	GetDetectorIndex(localBeamDetectorID.fTypeID,
			 localBeamDetectorID.fdetectorname);

      if(localBeamDetectorID.fIndex==-1){
	Int_t index;
	VQwDataElement* local_element;

	if(localBeamDetectorID.fTypeID == kQwHaloMonitor){
	  index = AddToElementList(fHaloMonitor, localBeamDetectorID);
	  local_element = &(fHaloMonitor.at(index));

	  // 	  if(localBeamDetectorID.fdetectorname=="sca_4mhz"){
	  // 	    index_4mhz=localBeamDetectorID.fIndex;
	  // 	  }
	}
	if(localBeamDetectorID.fTypeID==kQwBPMCavity){
	  index = AddToElementList(fCavity, localBeamDetectorID);
	  local_element = &(fCavity.at(index));
	}
	if(localBeamDetectorID.fTypeID== kQwBPMStripline){
	  index = AddToElementList(fStripline, localBeamDetectorID);
	  local_element = fStripline.at(index).get();
	}
	if(localBeamDetectorID.fTypeID== kQwBCM){
	  index = AddToElementList(fBCM, localBeamDetectorID);
	  local_element = fBCM.at(index).get();
	}
	if(localBeamDetectorID.fTypeID== kQwClock ) {
	  index = AddToElementList(fClock, localBeamDetectorID);
	  local_element = fClock.at(index).get();
	}
	if(localBeamDetectorID.fTypeID== kQwQPD){
	  index = AddToElementList(fQPD, localBeamDetectorID);
	  local_element = &(fQPD.at(index));
	}
	if(localBeamDetectorID.fTypeID== kQwLinearArray){
	  index = AddToElementList(fLinearArray, localBeamDetectorID);
	  local_element = &(fLinearArray.at(index));
	}

	local_element->LoadChannelParameters(mapstr);
	if (local_element->NeedsExternalClock()){
	  QwDebug << "Try to push device "
		  << local_element->GetElementName()
		  << " (address=" << std::hex << local_element
		  << ") onto the clock_needed_list"
		  << QwLog::endl;
	  clock_needed_list.push_back(localBeamDetectorID);
	}
      }
      


      fBeamDetectorID.push_back(localBeamDetectorID);
    }
  }

  // Now load the variables to publish
  mapstr.RewindToFileStart();
  QwParameterFile *section;
  std::vector<TString> publishinfo;
  while ((section=mapstr.ReadNextSection(varvalue))) {
    if (varvalue == "PUBLISH") {
      fPublishList.clear();
      while (section->ReadNextLine()) {
        section->TrimComment(); // Remove everything after a comment character
        section->TrimWhitespace(); // Get rid of leading and trailing spaces
        for (int ii = 0; ii < 4; ii++) {
          varvalue = section->GetTypedNextToken<TString>();
          if (varvalue.Length()) {
            publishinfo.push_back(varvalue);
          }
        }
        if (publishinfo.size() == 4)
          fPublishList.push_back(publishinfo);
        publishinfo.clear();
      }
    }
    delete section;
  }
  // Print list of variables to publish
  QwMessage << "Variables to publish:" << QwLog::endl;
  for (size_t jj = 0; jj < fPublishList.size(); jj++)
    QwMessage << fPublishList.at(jj).at(0) << " " << fPublishList.at(jj).at(1) << " "
              << fPublishList.at(jj).at(2) << " " << fPublishList.at(jj).at(3) << QwLog::endl;


  if(ldebug){
    std::cout<<"QwBeamLine::Done with Load map channel \n";
    for(size_t i=0;i<fBeamDetectorID.size();i++)
      fBeamDetectorID[i].Print();
  }

  // Now propagate clock pointers to those channels that need it
  index = 0;
  VQwDataElement* local_element;
  std::string clockname;
  for (size_t i=0; i<clock_needed_list.size();i++ ) {
    local_element = GetElement(clock_needed_list[i]);
    clockname = local_element->GetExternalClockName();
    if (clockname.empty()){
	QwWarning << "QwBeamLine::LoadChannelMap  " 
		  << "Device, " << local_element->GetElementName()
		  << " needs a reference clock, but the reference clock name is empty"
		  << QwLog::endl;
    } else {
      index = GetDetectorIndex(kQwClock,clockname);
      if( index >= 0 ){
	QwMessage << "QwBeamLine::LoadChannelMap  " 
		  << "Setting " << fClock.at(index).get()->GetElementName()
		  << " as the reference clock for channel "
		  << local_element->GetElementName()
		  << QwLog::endl;
	local_element->SetExternalClockPtr(fClock.at(index).get()->GetTime());
      } else {
	QwWarning << "QwBeamLine::LoadChannelMap  " 
		  << "Cannot find clock, " << local_element->GetExternalClockName()
		  << ", needed by device, " << local_element->GetElementName()
		  << QwLog::endl;
      }
    }
  }
  ldebug=kFALSE;

  return 0;
}


//*****************************************************************//
Int_t QwBeamLine::LoadEventCuts(TString  filename)
{
  Int_t eventcut_flag = 1;

  // Open the file
  QwParameterFile mapstr(filename.Data());
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  while (mapstr.ReadNextLine()){
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    TString varname, varvalue;
    if (mapstr.HasVariablePair("=",varname,varvalue)){
      if (varname == "EVENTCUTS"){
	eventcut_flag = QwParameterFile::GetUInt(varvalue);
      }
    }
    else{
      TString device_type = mapstr.GetTypedNextToken<TString>();
      device_type.ToLower();
      TString device_name = mapstr.GetTypedNextToken<TString>();
      device_name.ToLower();

      Int_t det_index = GetDetectorIndex(GetQwBeamInstrumentType(device_type),device_name);
      if (det_index == -1) {
	QwWarning << " Device not found " << device_name << " of type " << device_type << QwLog::endl;
	continue;
      }

      if (device_type == GetQwBeamInstrumentTypeName(kQwBCM)){
	Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for BCM value
	Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for BCM value
	varvalue = mapstr.GetTypedNextToken<TString>();//global/loacal
	Double_t stabilitycut = mapstr.GetTypedNextToken<Double_t>();
	varvalue.ToLower();
	QwMessage<<"QwBeamLine Error Code passing to QwBCM "<<GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut)<<QwLog::endl;
	fBCM[det_index].get()->SetSingleEventCuts(GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut),LLX,ULX,stabilitycut);//(fBCMEventCuts);
      }
      else if (device_type == GetQwBeamInstrumentTypeName(kQwHaloMonitor)){
	Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for HaloMonitor value
	Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for HaloMonitor value
	varvalue = mapstr.GetTypedNextToken<TString>();//global/loacal
	Double_t stabilitycut = mapstr.GetTypedNextToken<Double_t>();
	varvalue.ToLower();
	QwMessage<<"QwBeamLine Error Code passing to QwHaloMonitor "<<GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut)<<QwLog::endl;
	fHaloMonitor[det_index].SetSingleEventCuts(GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut),LLX,ULX,stabilitycut);//(fBCMEventCuts);
      }
	else if (device_type ==GetQwBeamInstrumentTypeName(kQwEnergyCalculator)){
	Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for energy
	Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for energy
	varvalue = mapstr.GetTypedNextToken<TString>();//global/loacal
	Double_t stabilitycut = mapstr.GetTypedNextToken<Double_t>();
	varvalue.ToLower();
	QwMessage<<"QwBeamLine Error Code passing to QwEnergyCalculator "<<GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut)<<QwLog::endl;
	fECalculator[det_index].SetSingleEventCuts(GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut),LLX,ULX,stabilitycut);//(fEnergyEventCuts);
      }
	else if (device_type == GetQwBeamInstrumentTypeName(kQwBPMStripline)){
	TString channel_name =  mapstr.GetTypedNextToken<TString>();
	channel_name.ToLower();
	Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for BPMStripline X
	Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for BPMStripline X
	varvalue = mapstr.GetTypedNextToken<TString>();//global/loacal
	Double_t stabilitycut = mapstr.GetTypedNextToken<Double_t>();
	varvalue.ToLower();
	//QwMessage<<"QwBeamLine:QwBPMStripline "<<channel_name<<" "<<varvalue<<" "<<stabilitycut<<QwLog::endl;
	QwMessage<<"QwBeamLine Error Code passing to QwBPMStripline "<<GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut)<<" stability  "<<stabilitycut <<QwLog::endl;
	fStripline[det_index].get()->SetSingleEventCuts(channel_name, GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut), LLX, ULX, stabilitycut);
      }
	else if (device_type == GetQwBeamInstrumentTypeName(kQwQPD)){
	TString channel_name =  mapstr.GetTypedNextToken<TString>();
	channel_name.ToLower();
	Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for QPD X
	Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for QPD X
	varvalue = mapstr.GetTypedNextToken<TString>();//global/loacal
	Double_t stabilitycut = mapstr.GetTypedNextToken<Double_t>();
	varvalue.ToLower();
	QwMessage<<"QwBeamLine Error Code passing to QwQPD "<<GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut)<<QwLog::endl;
	fQPD[det_index].SetSingleEventCuts(channel_name, GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut), LLX, ULX, stabilitycut);
	}
	else if (device_type == GetQwBeamInstrumentTypeName(kQwLinearArray)){
	TString channel_name =  mapstr.GetTypedNextToken<TString>();
	channel_name.ToLower();
	Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for LinearArray X
	Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for LinearArray X
	varvalue = mapstr.GetTypedNextToken<TString>();//global/loacal
	Double_t stabilitycut = mapstr.GetTypedNextToken<Double_t>();
	varvalue.ToLower();
	QwMessage<<"QwBeamLine Error Code passing to QwLinearArray "<<GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut)<<QwLog::endl;
	fLinearArray[det_index].SetSingleEventCuts(channel_name, GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut), LLX, ULX, stabilitycut);
	}
	else if (device_type ==  GetQwBeamInstrumentTypeName(kQwBPMCavity)){
	TString channel_name =  mapstr.GetTypedNextToken<TString>();
	channel_name.ToLower();
	  Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for cavity bpm X
	  Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for cavity bpm X
	  varvalue = mapstr.GetTypedNextToken<TString>();//global/loacal
	  Double_t stabilitycut = mapstr.GetTypedNextToken<Double_t>();
	  varvalue.ToLower();
	  QwMessage<<"QwBeamLine Error Code passing to QwBPMCavity "<<GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut)<<" "<<det_index<<QwLog::endl;	  
	  fCavity[det_index].SetSingleEventCuts(channel_name, GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut), LLX, ULX, stabilitycut);
	}
	else if (device_type == GetQwBeamInstrumentTypeName(kQwCombinedBCM)){
	  Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for BCM value
	  Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for BCM value
	  varvalue = mapstr.GetTypedNextToken<TString>();//global/loacal
	  Double_t stabilitycut = mapstr.GetTypedNextToken<Double_t>();
	  varvalue.ToLower();
	  QwMessage<<"QwBeamLine Error Code passing to QwCombinedBCM "<<GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut)<<QwLog::endl;
	  fBCMCombo[det_index].get()->PrintInfo();
	  fBCMCombo[det_index].get()->SetSingleEventCuts(GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut), LLX, ULX, stabilitycut);

	}
	else if (device_type == GetQwBeamInstrumentTypeName(kQwCombinedBPM)){
	  TString channel_name =  mapstr.GetTypedNextToken<TString>();
	  channel_name.ToLower();
	  
	  Double_t LLX = mapstr.GetTypedNextToken<Double_t>();	//lower limit for combined bpm X
	  Double_t ULX = mapstr.GetTypedNextToken<Double_t>();	//upper limit for combined bpm X
	  varvalue = mapstr.GetTypedNextToken<TString>();//global/loacal
	  Double_t stabilitycut = mapstr.GetTypedNextToken<Double_t>();
	  varvalue.ToLower();
	  QwMessage<<"QwBeamLine Error Code passing to QwCombinedBPM "<<GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut)<<QwLog::endl;
	  fBPMCombo[det_index].get()->SetSingleEventCuts(channel_name, GetGlobalErrorFlag(varvalue,eventcut_flag,stabilitycut), LLX, ULX, stabilitycut);
	}

    }

  }


  //update the event cut ON/OFF for all the devices
  //std::cout<<"EVENT CUT FLAG"<<eventcut_flag<<std::endl;
  for (size_t i=0;i<fStripline.size();i++)
    fStripline[i].get()->SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fQPD.size();i++)
    fQPD[i].SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fLinearArray.size();i++)
    fLinearArray[i].SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fCavity.size();i++)
    fCavity[i].SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fBCM.size();i++)
    fBCM[i].get()->SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fClock.size();i++)
    fClock[i].get()->SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fHaloMonitor.size();i++){
    fHaloMonitor[i].SetEventCutMode(eventcut_flag);
  }

  for (size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].get()->SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].get()->SetEventCutMode(eventcut_flag);

  for (size_t i=0;i<fECalculator.size();i++)
    fECalculator[i].SetEventCutMode(eventcut_flag);

  fQwBeamLineErrorCount=0; //set the error counter to zero

  return 0;
}


//*****************************************************************//
Int_t QwBeamLine::LoadGeometryDefinition(TString mapfile){
  Bool_t ldebug=kFALSE;
  TString varname, varvalue;
  Int_t lineread=1;
  Int_t index = 0;
  TString  devname,devtype;
  TString  melement;
  Double_t devOffsetX = 0,devOffsetY = 0, devOffsetZ = 0;
  Double_t devSENfactor = 0, devAlphaX = 0, devAlphaY = 0;
  TString   rotation_stat;
  VQwBPM * bpm;

  if(ldebug)std::cout<<"QwBeamLine::LoadGeometryParameters("<< mapfile<<")\n";

  QwParameterFile mapstr(mapfile.Data());  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  while (mapstr.ReadNextLine()){
    lineread+=1;
    if(ldebug)std::cout<<" line read so far ="<<lineread<<"\n";
    mapstr.TrimComment('!');
    mapstr.TrimWhitespace();

    if (mapstr.LineIsEmpty())  continue;

    Bool_t notfound=kTRUE;

    devtype = mapstr.GetTypedNextToken<TString>();
    devtype.ToLower();
    devtype.Remove(TString::kBoth,' ');
    devname = mapstr.GetTypedNextToken<TString>();
    devname.ToLower();
    devname.Remove(TString::kBoth,' ');

    index=GetDetectorIndex(GetQwBeamInstrumentType(devtype),devname);
    if( index<0 ) {
      QwError << "Error! Unknown detector type "<<devtype<<" in Geometry file!"<<QwLog::endl;
      /*If the device type is unknown there is no point in going through the rest of the specs for that device*/
      /*Ignore it!*/ 
    } 
    else {
      devOffsetX   = mapstr.GetTypedNextToken<Double_t>(); // X offset
      devOffsetY   = mapstr.GetTypedNextToken<Double_t>(); // Y offset
      devOffsetZ   = mapstr.GetTypedNextToken<Double_t>(); // Z offset
      devSENfactor = mapstr.GetTypedNextToken<Double_t>(); // sensivity scaling factor
      devAlphaX    = mapstr.GetTypedNextToken<Double_t>(); // alpha X
      devAlphaY    = mapstr.GetTypedNextToken<Double_t>(); // alpha Y


      /*If the device is a bpm stripline, assign the rotations and gains*/
      if(GetQwBeamInstrumentType(devtype)==kQwBPMStripline){
	bpm = fStripline.at(index).get();
	AssignGeometry(&mapstr,bpm);
      }


      if(ldebug==1){
	std::cout<<"####################\n";
	std::cout<<"! device type, device_name, Xoffset, Yoffset, Zoffset, BSEN scaling factor, AlpaX, AlpaY\n"<<std::endl;
	std::cout<<GetQwBeamInstrumentType(devtype)<<" / "
		 <<devname    <<" / "
		 <<devOffsetX <<" / "
		 <<devOffsetY <<" / "
		 <<devOffsetZ <<" / "
		 <<devSENfactor <<" / "
		 <<devAlphaX <<" / "
		 <<devAlphaY <<" / "
		 <<std::endl;
      }


      while(notfound){
	if(GetQwBeamInstrumentType(devtype)==kQwBPMStripline){
	  //Load bpm offsets
	  if(index == -1){
	    QwWarning << "QwBeamLine::LoadGeometryDefinition:  Unknown bpm in qweak_beamline_geometry.map: "
		      <<devname
		      <<QwLog::endl;
	    notfound=kFALSE;
	    continue;
	  }

	  TString localname = fStripline.at(index).get()->GetElementName();
	  localname.ToLower();
	  if(ldebug)  std::cout<<"element name =="<<localname
			       <<"== to be compared to =="<<devname<<"== \n";

	  if(localname==devname){
	    if(ldebug) std::cout<<" I found the bpm !\n";
	    bpm->GetSurveyOffsets(devOffsetX,devOffsetY,devOffsetZ);
	    bpm->GetElectronicFactors(devSENfactor,devAlphaX, devAlphaY);

	    // If nothing is specified, a default rotation of 45 degrees is implied.
	    notfound=kFALSE;
	  }
	}
	else if (GetQwBeamInstrumentType(devtype)==kQwCombinedBPM){
	  //Load combined bpm offsets which are, ofcourse, target position in the beamline
	  if(index == -1){
	    QwError << "QwBeamLine::LoadGeometryDefinition:  Unknown combined bpm in qweak_beamline_geometry.map: "
		    <<devname<<" Check the combined bpm names!\n "
		    << QwLog::endl;
	    notfound=kFALSE;
	    continue;
	  }
	  
	  TString localname = fBPMCombo.at(index).get()->GetElementName();
	  localname.ToLower();
	  if(ldebug)
	    std::cout<<"element name =="<<localname<<"== to be compared to =="<<devname<<"== \n";
	  
	  if(localname==devname){
	    if(ldebug) std::cout<<" I found the combinedbpm !\n";
	    fBPMCombo.at(index).get()->GetSurveyOffsets(devOffsetX,devOffsetY,devOffsetZ);
	    notfound=kFALSE;
	  }
	}
	else if(GetQwBeamInstrumentType(devtype)==kQwBPMCavity){
	  //Load cavity bpm offsets
	  if(index == -1){
	    QwError << "QwBeamLine::LoadGeometryDefinition:  Unknown bpm : "
		    <<devname<<" will not be asigned with geometry parameters. \n"
		    <<QwLog::endl;
	    notfound=kFALSE;
	    continue;
	  }
	  TString localname = fCavity.at(index).GetElementName();
	  localname.ToLower();
	  if(ldebug)  std::cout<<"element name =="<<localname
			       <<"== to be compared to =="<<devname<<"== \n";
	  
	  if(localname==devname){
	    if(ldebug) std::cout<<" I found the cavity bpm !\n";
	    fCavity.at(index).GetSurveyOffsets(devOffsetX,devOffsetY,devOffsetZ);
	    notfound=kFALSE;
	  }
	}
	else if(GetQwBeamInstrumentType(devtype)==kQwQPD){
	  //Load QPD calibration factors
	  if(index == -1){
	    QwError << "QwBeamLine::LoadGeometryDefinition:  Unknown QPD : "
		    <<devname<<" will not be asigned with calibration factors. \n"
		    <<QwLog::endl;
	    notfound=kFALSE;
	    continue;
	  }
	  TString localname = fQPD.at(index).GetElementName();
	  localname.ToLower();
	  if(ldebug)  std::cout<<"element name =="<<localname
			       <<"== to be compared to =="<<devname<<"== \n";
	  
	  if(localname==devname){
	    if(ldebug) std::cout<<" I found the QPD !\n";
	    fQPD.at(index).GetCalibrationFactors(devAlphaX, devAlphaY);
	    notfound=kFALSE;
	  }
	}
	else QwError<<" QwBeamLine::LoadGeometryDefinition: Unknown device type :"<<devtype<<". Are you sure we have this in the beamline? I am skipping this."<<QwLog::endl;
      }
    }
  }
  
  if(ldebug) std::cout<<" line read in the geometry file ="<<lineread<<" \n";

  ldebug=kFALSE;
  return 0;

}


void QwBeamLine::AssignGeometry(QwParameterFile* mapstr, VQwBPM * bpm)
{

  Bool_t ldebug = kFALSE;

  TString token = "0";
  TString   angle,xgain,ygain;
  Double_t  rotation_angle = 0;

  while(token!=""){
    token= mapstr->GetTypedNextToken<TString>();
    token.Remove(TString::kBoth,'\0');

    if(token.Contains("unrotated")){
      if(ldebug) std::cout<<" unrotated "<<std::endl;
      bpm->SetRotationOff();	    
    }
    else if(token.Contains("rotation")){
      // If the status is 'rotated'
      
      // If a specific rotation angle is given read that
      if(token.Contains("=")){
	angle = token.Remove(0,9);
	rotation_angle = atof(angle);
	if(ldebug) std::cout<<"Rotation angle = "<<rotation_angle<<std::endl;
	bpm->SetRotation(rotation_angle);	    
      }
    }
    // If nothing is specified for rotation, a default rotation of 45 degrees is implied.

    if(token.Contains("xgain")){
      xgain = token.Remove(0,6);
      if(ldebug) std::cout<<" xgain ="<<xgain<<std::endl;
      bpm->SetGains("X", atof(xgain));
    }

    if(token.Contains("ygain")){
      ygain = token.Remove(0,6);
      if(ldebug) std::cout<<" ygain ="<<ygain<<std::endl;
      bpm->SetGains("Y", atof(ygain));
    }
  }
}

//*****************************************************************//
Int_t QwBeamLine::LoadInputParameters(TString pedestalfile)
{
  Bool_t ldebug=kFALSE;

  Int_t lineread=1;

  if(ldebug)std::cout<<"QwBeamLine::LoadInputParameters("<< pedestalfile<<")\n";

  QwParameterFile mapstr(pedestalfile.Data());  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());

   while (mapstr.ReadNextLine())
    {
      lineread+=1;
      if(ldebug)std::cout<<" line read so far ="<<lineread<<"\n";
      mapstr.TrimComment('!');   // Remove everything after a '!' character.
      mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
      if (mapstr.LineIsEmpty())  continue;
      else
	{
	  TString varname = mapstr.GetTypedNextToken<TString>();	//name of the channel
	  varname.ToLower();
	  varname.Remove(TString::kBoth,' ');
	  Double_t varped = mapstr.GetTypedNextToken<Double_t>(); // value of the pedestal
	  Double_t varcal = mapstr.GetTypedNextToken<Double_t>(); // value of the calibration factor
	  Double_t varweight = mapstr.GetTypedNextToken<Double_t>(); // value of the statistical weight

	  //if(ldebug) std::cout<<"inputs for channel "<<varname
	  //	      <<": ped="<<varped<<": cal="<<varcal<<": weight="<<varweight<<"\n";
	  Bool_t notfound=kTRUE;

	  if(notfound) {

	    for(size_t i=0;i<fStripline.size();i++)
	      {
		for(int j=0;j<4;j++)
		  {
		    TString localname = fStripline[i].get()->GetSubElementName(j);
		    localname.ToLower();
		    if(ldebug)  std::cout<<"Stripline element name =="<<localname
					 <<"== to be compared to =="<<varname<<"== \n";
		    if(localname==varname)
		      {
			if(ldebug) std::cout<<" I found it !\n";
			fStripline[i].get()->SetSubElementPedestal(j,varped);
			fStripline[i].get()->SetSubElementCalibrationFactor(j,varcal);
			notfound=kFALSE;
			j=5;
			i=fStripline.size()+1;
		      }
		  }
	      }

	    for(size_t i=0;i<fQPD.size();i++)
	      {
		for(int j=0;j<4;j++)
		  {
		    TString localname = fQPD[i].GetSubElementName(j);
		    localname.ToLower();
		    if(ldebug)  std::cout<<"QPD element name =="<<localname
					 <<"== to be compared to =="<<varname<<"== \n";
		    if(localname==varname)
		      {
			if(ldebug) std::cout<<" I found it !\n";
			fQPD[i].SetSubElementPedestal(j,varped);
			fQPD[i].SetSubElementCalibrationFactor(j,varcal);
			notfound=kFALSE;
			j=5;
			i=fQPD.size()+1;
		      }
		  }
	      }

	    for(size_t i=0;i<fLinearArray.size();i++)
	      {
		for(int j=0;j<8;j++)
		  {
		    TString localname = fLinearArray[i].GetSubElementName(j);
		    localname.ToLower();
		    if(ldebug)  std::cout<<"LinearArray element name =="<<localname
					 <<"== to be compared to =="<<varname<<"== \n";
		    if(localname==varname)
		      {
			if(ldebug) std::cout<<" I found it !\n";
			fLinearArray[i].SetSubElementPedestal(j,varped);
			fLinearArray[i].SetSubElementCalibrationFactor(j,varcal);
			notfound=kFALSE;
			j=9;
			i=fLinearArray.size()+1;
		      }
		  }
	      }
	    
	    for(size_t i=0;i<fCavity.size();i++)
	      {
		for(int j=0;j<2;j++)
		  {
		    TString localname = fCavity[i].GetSubElementName(j);
		    localname.ToLower();
		    if(ldebug)  std::cout<<"Cavity element name =="<<localname
					 <<"== to be compared to =="<<varname<<"== \n";
		    if(localname==varname)
		      {
			if(ldebug) std::cout<<" I found it !\n";
			fCavity[i].SetSubElementPedestal(j,varped);
			fCavity[i].SetSubElementCalibrationFactor(j,varcal);
			notfound=kFALSE;
			j=3;
			i=fCavity.size()+1;
		      }
		  }
	      }
	    
	    for(size_t i=0;i<fBCM.size();i++) {
	      if(fBCM[i].get()->GetElementName()==varname)
		{
		  fBCM[i].get()->SetPedestal(varped);
		  fBCM[i].get()->SetCalibrationFactor(varcal);
		  i=fBCM.size()+1;
		  notfound=kFALSE;
		  i=fBCM.size()+1;
		}
            }
	    for(size_t i=0;i<fClock.size();i++) {
	      if(fClock[i].get()->GetElementName()==varname)
		{
		  fClock[i].get()->SetPedestal(varped);
		  fClock[i].get()->SetCalibrationFactor(varcal);
		  i=fClock.size()+1;
		  notfound=kFALSE;
		  i=fClock.size()+1;
		}
            }



	    for(size_t i=0;i<fHaloMonitor.size();i++) {
	      if(fHaloMonitor[i].GetElementName()==varname)
		{
		  std::cout<<varname<<" I found it ! "<<varcal<<" ped. "<<varped<<"\n";
		  fHaloMonitor[i].SetPedestal(varped);
		  fHaloMonitor[i].SetCalibrationFactor(varcal);
		  i=fHaloMonitor.size()+1;
		  notfound=kFALSE;
		  i=fHaloMonitor.size()+1;
		}
            }



          }


	}

    }
  if(ldebug) std::cout<<" line read in the pedestal + cal file ="<<lineread<<" \n";

  ldebug=kFALSE;
  return 0;
}


//*****************************************************************//
void QwBeamLine::RandomizeEventData(int helicity, double time)
{
  // Randomize all QwBPMStripline buffers
  for (size_t i = 0; i < fStripline.size(); i++)
    fStripline[i].get()->RandomizeEventData(helicity, time);

  for (size_t i = 0; i < fCavity.size(); i++)
    fCavity[i].RandomizeEventData(helicity, time);

  // Randomize all QwBCM buffers
  for (size_t i = 0; i < fBCM.size(); i++)
    fBCM[i].get()->RandomizeEventData(helicity, time);

  // Randomize all QwHaloMonitor buffers
  //for (size_t i = 0; i < fHaloMonitor.size(); i++)
    //fHaloMonitor[i].RandomizeEventData(helicity, time);
}


//*****************************************************************//
void QwBeamLine::EncodeEventData(std::vector<UInt_t> &buffer)
{
  std::vector<UInt_t> elements;
  elements.clear();

  // Get all buffers in the order they are defined in the map file
  for (size_t i = 0; i < fBeamDetectorID.size(); i++) {
    // This is a QwBCM
    if (fBeamDetectorID.at(i).fTypeID == kQwBCM)
      fBCM[fBeamDetectorID.at(i).fIndex].get()->EncodeEventData(elements);
    // This is a QwBPMStripline (which has 4 entries, only process the first one)
    if (fBeamDetectorID.at(i).fTypeID == kQwBPMStripline
     && fBeamDetectorID.at(i).fSubelement == 0)
      fStripline[fBeamDetectorID.at(i).fIndex].get()->EncodeEventData(elements);
  }

  // If there is element data, generate the subbank header
  std::vector<UInt_t> subbankheader;
  std::vector<UInt_t> rocheader;
  if (elements.size() > 0) {

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


//*****************************************************************//
Int_t QwBeamLine::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  Bool_t lkDEBUG=kFALSE;

  Int_t index = GetSubbankIndex(roc_id,bank_id);
  if (index>=0 && num_words>0){
    //  We want to process this ROC.  Begin looping through the data.
    if (lkDEBUG)
      std::cout << "QwBeamLine::ProcessEvBuffer:  "
		<< "Begin processing ROC" << roc_id
		<< " and subbank "<<bank_id
		<< " number of words="<<num_words<<std::endl;
    if (buffer[0]==0xf0f0f0f0 && num_words%2==1){
      buffer++;
      if (lkDEBUG)
	std::cout << "QwBeamLine::ProcessEvBuffer:  "
		  << "Skipped padding word 0xf0f0f0f0 at beginning of buffer."
		  << std::endl;
    }

    for(size_t i=0;i<fBeamDetectorID.size();i++)
      {
	if(fBeamDetectorID[i].fSubbankIndex==index)
	  {

	    if(fBeamDetectorID[i].fTypeID==kQwBPMStripline)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found stripline data for "<<fBeamDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fBeamDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fStripline[fBeamDetectorID[i].fIndex].get()->
		  ProcessEvBuffer(&(buffer[fBeamDetectorID[i].fWordInSubbank]),
				  num_words-fBeamDetectorID[i].fWordInSubbank,
				  fBeamDetectorID[i].fSubelement);
	      }

	    if(fBeamDetectorID[i].fTypeID==kQwQPD)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found qpd data for "<<fBeamDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fBeamDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fQPD[fBeamDetectorID[i].fIndex].
		  ProcessEvBuffer(&(buffer[fBeamDetectorID[i].fWordInSubbank]),
				  num_words-fBeamDetectorID[i].fWordInSubbank,
				  fBeamDetectorID[i].fSubelement);
	      }

	    if(fBeamDetectorID[i].fTypeID==kQwLinearArray)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found linear array data for "<<fBeamDetectorID[i].fdetectorname<<fBeamDetectorID[i].fIndex<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fBeamDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fLinearArray[fBeamDetectorID[i].fIndex].
		  ProcessEvBuffer(&(buffer[fBeamDetectorID[i].fWordInSubbank]),
				  num_words-fBeamDetectorID[i].fWordInSubbank,
				  fBeamDetectorID[i].fSubelement);

	      }

	    if(fBeamDetectorID[i].fTypeID==kQwBPMCavity)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found stripline data for "<<fBeamDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fBeamDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fCavity[fBeamDetectorID[i].fIndex].
		  ProcessEvBuffer(&(buffer[fBeamDetectorID[i].fWordInSubbank]),
				  num_words-fBeamDetectorID[i].fWordInSubbank,
				  fBeamDetectorID[i].fSubelement);
	      }

	    if(fBeamDetectorID[i].fTypeID==kQwBCM)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found bcm data for "<<fBeamDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fBeamDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fBCM[fBeamDetectorID[i].fIndex].get()->
		  ProcessEvBuffer(&(buffer[fBeamDetectorID[i].fWordInSubbank]),
				  num_words-fBeamDetectorID[i].fWordInSubbank);
	      }

	    if(fBeamDetectorID[i].fTypeID==kQwClock)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found clock data for "<<fBeamDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fBeamDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fClock[fBeamDetectorID[i].fIndex].get()->
		  ProcessEvBuffer(&(buffer[fBeamDetectorID[i].fWordInSubbank]),
				  num_words-fBeamDetectorID[i].fWordInSubbank);
	      }

	    if(fBeamDetectorID[i].fTypeID==kQwHaloMonitor)
	      {
		if (lkDEBUG)
		  {
		    std::cout<<"found halo monitor data for "<<fBeamDetectorID[i].fdetectorname<<std::endl;
		    std::cout<<"word left to read in this buffer:"<<num_words-fBeamDetectorID[i].fWordInSubbank<<std::endl;
		  }
		fHaloMonitor[fBeamDetectorID[i].fIndex].
		  ProcessEvBuffer(&(buffer[fBeamDetectorID[i].fWordInSubbank]),
				  num_words-fBeamDetectorID[i].fWordInSubbank);
	      }

	  }
      }
  }

  return 0;
}



//*****************************************************************//
Bool_t QwBeamLine::ApplySingleEventCuts(){

  Bool_t status=kTRUE;

  for(size_t i=0;i<fBCM.size();i++){
    status &= fBCM[i].get()->ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******* QwBeamLine::SingleEventCuts()->BCM[ "<<i
				   <<" , "<<fBCM[i].get()->GetElementName()<<" ] ******\n";
  }

  for(size_t i=0;i<fClock.size();i++){
    status &= fClock[i].get()->ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******* QwBeamLine::SingleEventCuts()->Clock[ "<<i
				   <<" , "<<fClock[i].get()->GetElementName()<<" ] ******\n";
  }

  for(size_t i=0;i<fHaloMonitor.size();i++){
    status &= fHaloMonitor[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******* QwBeamLine::SingleEventCuts()->HaloMonitor[ "<<i
				   <<" , "<<fHaloMonitor[i].GetElementName()<<" ] ******\n";
  }

  for(size_t i=0;i<fStripline.size();i++){
    status &= fStripline[i].get()->ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******** QwBeamLine::SingleEventCuts()->BPMStripline[ "<<i
				   <<" , "<<fStripline[i].get()->GetElementName()<<" ] *****\n";

    }

  for(size_t i=0;i<fQPD.size();i++){
    status &= fQPD[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******** QwBeamLine::SingleEventCuts()->QPD[ "<<i
				   <<" , "<<fQPD[i].GetElementName()<<" ] *****\n";
  }
  for(size_t i=0;i<fLinearArray.size();i++){
    status &= fLinearArray[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******** QwBeamLine::SingleEventCuts()->LinearArray[ "<<i
				   <<" , "<<fLinearArray[i].GetElementName()<<" ] *****\n";
  }

  for(size_t i=0;i<fCavity.size();i++){
    status &= fCavity[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******** QwBeamLine::SingleEventCuts()->BPMCavity[ "<<i
				   <<" , "<<fCavity[i].GetElementName()<<" ] *****\n";
  }

  for(size_t i=0;i<fBCMCombo.size();i++){
    status &= fBCMCombo[i].get()->ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******* QwBeamLine::SingleEventCuts()->CombinedBCM[ "<<i
				   <<" , "<<fBCMCombo[i].get()->GetElementName()<<" ] ******\n";
  }

  for(size_t i=0;i<fBPMCombo.size();i++){
    status &= fBPMCombo[i].get()->ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******* QwBeamLine::SingleEventCuts()->CombinedBPM[ "<<i
				   <<" , "<<fBPMCombo[i].get()->GetElementName()<<" ] ******\n";

  }
  for(size_t i=0;i<fECalculator.size();i++){
    status &= fECalculator[i].ApplySingleEventCuts();
    if(!status && bDEBUG) std::cout<<"******* QwBeamLine::SingleEventCuts()->EnergyCalculator[ "<<i
				   <<" , "<<fECalculator[i].GetElementName()<<" ] ******\n";

  }


  //If at least one of the devices falied  event cuts, increment error counter for QwBeamLine
  if (!status)
    fQwBeamLineErrorCount++;


  return status;

}


//*****************************************************************//
Int_t QwBeamLine::GetEventcutErrorCounters(){//inherited from the VQwSubsystemParity; this will display the error summary

  QwMessage<<"*********QwBeamLine Error Summary****************"<<QwLog::endl;
  QwVQWK_Channel::PrintErrorCounterHead();

  for(size_t i=0;i<fClock.size();i++){
    fClock[i].get()->GetEventcutErrorCounters();
  }

  for(size_t i=0;i<fBCM.size();i++){
    fBCM[i].get()->GetEventcutErrorCounters();
  }

  for(size_t i=0;i<fHaloMonitor.size();i++){
    fHaloMonitor[i].GetEventcutErrorCounters();
  }

  for(size_t i=0;i<fStripline.size();i++){
    fStripline[i].get()->GetEventcutErrorCounters();
  }

 for(size_t i=0;i<fQPD.size();i++){
    fQPD[i].GetEventcutErrorCounters();
  }

 for(size_t i=0;i<fLinearArray.size();i++){
    fLinearArray[i].GetEventcutErrorCounters();
  }

 for(size_t i=0;i<fCavity.size();i++){
    fCavity[i].GetEventcutErrorCounters();
  }

  for(size_t i=0;i<fBCMCombo.size();i++){
    fBCMCombo[i].get()->GetEventcutErrorCounters();
  }

  for(size_t i=0;i<fBPMCombo.size();i++){
    fBPMCombo[i].get()->GetEventcutErrorCounters();
  }

  for(size_t i=0;i<fECalculator.size();i++){
    fECalculator[i].GetEventcutErrorCounters();
  }
  QwVQWK_Channel::PrintErrorCounterTail();
  return 1;
}

//*****************************************************************//
UInt_t QwBeamLine::GetEventcutErrorFlag(){//return the error flag
  UInt_t ErrorFlag;
  UInt_t ErrorFlagtmp;
  ErrorFlag=0;
  for(size_t i=0;i<fBCM.size();i++){
    ErrorFlagtmp = fBCM[i].get()->GetEventcutErrorFlag();
    ErrorFlag |=ErrorFlagtmp;    
  }
  for(size_t i=0;i<fStripline.size();i++){
    ErrorFlag |= fStripline[i].get()->GetEventcutErrorFlag();        
  }
  for(size_t i=0;i<fQPD.size();i++){
    ErrorFlag |= fQPD[i].GetEventcutErrorFlag();
  }  
  for(size_t i=0;i<fLinearArray.size();i++){
    ErrorFlag |= fLinearArray[i].GetEventcutErrorFlag();
  }
  for(size_t i=0;i<fCavity.size();i++){
    ErrorFlag |= fCavity[i].GetEventcutErrorFlag();
  }  
  for(size_t i=0;i<fBCMCombo.size();i++){
    ErrorFlag |= fBCMCombo[i].get()->GetEventcutErrorFlag();
  }  
  for(size_t i=0;i<fBPMCombo.size();i++){
    ErrorFlag |= fBPMCombo[i].get()->GetEventcutErrorFlag();
  }  
  for(size_t i=0;i<fECalculator.size();i++){
    ErrorFlag |= fECalculator[i].GetEventcutErrorFlag();
  }

  return ErrorFlag;

}

//*****************************************************************//
void QwBeamLine::UpdateEventcutErrorFlag(UInt_t errorflag){
       /*
      for(size_t i=0;i<input->fClock.size();i++)
	*(this->fClock[i].get())=*(input->fClock[i].get());
	*/
      for(size_t i=0;i<fStripline.size();i++)
	(this->fStripline[i].get())->UpdateEventcutErrorFlag(errorflag);      
      for(size_t i=0;i<fQPD.size();i++)
	(this->fQPD[i]).UpdateEventcutErrorFlag(errorflag);
      for(size_t i=0;i<fLinearArray.size();i++)
	(this->fLinearArray[i]).UpdateEventcutErrorFlag(errorflag);
      for(size_t i=0;i<fCavity.size();i++)
	(this->fCavity[i]).UpdateEventcutErrorFlag(errorflag);      
      for(size_t i=0;i<fBCM.size();i++)
	(this->fBCM[i].get())->UpdateEventcutErrorFlag(errorflag);
      for(size_t i=0;i<fBCMCombo.size();i++)
	(this->fBCMCombo[i].get())->UpdateEventcutErrorFlag(errorflag);
      for(size_t i=0;i<fBPMCombo.size();i++)
	(this->fBPMCombo[i].get())->UpdateEventcutErrorFlag(errorflag);
      for(size_t i=0;i<fECalculator.size();i++)
	(this->fECalculator[i]).UpdateEventcutErrorFlag(errorflag);
      for(size_t i=0;i<fHaloMonitor.size();i++)
	(this->fHaloMonitor[i]).UpdateEventcutErrorFlag(errorflag);

}

//*****************************************************************//
void QwBeamLine::UpdateEventcutErrorFlag(VQwSubsystem *ev_error){
  if(Compare(ev_error))
    {

      QwBeamLine* input = dynamic_cast<QwBeamLine*>(ev_error);

       /*
      for(size_t i=0;i<input->fClock.size();i++)
	*(this->fClock[i].get())=*(input->fClock[i].get());
	*/
      for(size_t i=0;i<input->fStripline.size();i++)
	(this->fStripline[i].get())->UpdateEventcutErrorFlag(input->fStripline[i].get());      
      for(size_t i=0;i<input->fQPD.size();i++)
	(this->fQPD[i]).UpdateEventcutErrorFlag(&(input->fQPD[i]));
      for(size_t i=0;i<input->fLinearArray.size();i++)
	(this->fLinearArray[i]).UpdateEventcutErrorFlag(&(input->fLinearArray[i]));
      for(size_t i=0;i<input->fCavity.size();i++)
	(this->fCavity[i]).UpdateEventcutErrorFlag(&(input->fCavity[i]));      
      for(size_t i=0;i<input->fBCM.size();i++){
	(this->fBCM[i].get())->UpdateEventcutErrorFlag(input->fBCM[i].get());
      } 	
      for(size_t i=0;i<input->fBCMCombo.size();i++)
	(this->fBCMCombo[i].get())->UpdateEventcutErrorFlag(input->fBCMCombo[i].get()); //*(this->fBCMCombo[i].get())=*(input->fBCMCombo[i].get());     
      for(size_t i=0;i<input->fBPMCombo.size();i++)
	(this->fBPMCombo[i].get())->UpdateEventcutErrorFlag(input->fBPMCombo[i].get()); //=*(input->fBPMCombo[i].get());      
      for(size_t i=0;i<input->fECalculator.size();i++)
	(this->fECalculator[i]).UpdateEventcutErrorFlag(&(input->fECalculator[i]));
      for(size_t i=0;i<input->fHaloMonitor.size();i++)
	(this->fHaloMonitor[i]).UpdateEventcutErrorFlag(&(input->fHaloMonitor[i]));
      
      

    }
}


//*****************************************************************//
void  QwBeamLine::ProcessEvent()
{
  // Make sure this one comes first! The clocks are needed by
  // other elements.
  for(size_t i=0;i<fClock.size();i++)
    fClock[i].get()->ProcessEvent();

  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].get()->ProcessEvent();

  for(size_t i=0;i<fCavity.size();i++)
    fCavity[i].ProcessEvent();

  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].get()->ProcessEvent();

  for(size_t i=0;i<fQPD.size();i++)
    fQPD[i].ProcessEvent();

  for(size_t i=0;i<fLinearArray.size();i++)
    fLinearArray[i].ProcessEvent();

//   Double_t clock_counts = 0.0;
//   if (index_4mhz != -1){
//     fHaloMonitor.at(index_4mhz).ProcessEvent();//call the ProcessEvent() for the 4MHz scaler
//     clock_counts = fHaloMonitor.at(index_4mhz).GetValue();
//     //QwError << "QwBeamLine::ProcessEvent() "<<fHaloMonitor[index_4mhz].GetElementName()<<" "<<fHaloMonitor[index_4mhz].GetValue()<<QwLog::endl;
//   }
  for(size_t i=0;i<fHaloMonitor.size();i++){
//     if (index_4mhz != -1 && clock_counts>0.0 ){
//       fHaloMonitor[i].ScaleRawRate(4.0e6/clock_counts);//convert raw rates to Hz
//     }
    fHaloMonitor[i].ProcessEvent();
 }

  for(size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].get()->ProcessEvent();

  for(size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].get()->ProcessEvent();

  for(size_t i=0;i<fECalculator.size();i++)
    fECalculator[i].ProcessEvent();

  return;
}


//*****************************************************************//
Int_t QwBeamLine::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  return 0;
}

//*****************************************************************//
Bool_t QwBeamLine::PublishInternalValues() const
{
  // Publish variables
  Bool_t status = kTRUE;
  
  // Publish variables through map file
  // This should work with bcm, bpmstripline, bpmcavity, combo bpm and combo bcm
  for (size_t pp = 0; pp < fPublishList.size(); pp++) {
    TString publish_name = fPublishList.at(pp).at(0);
    TString device_type = fPublishList.at(pp).at(1);
    TString device_name = fPublishList.at(pp).at(2);
    TString device_prop = fPublishList.at(pp).at(3);
    device_type.ToLower();
    device_prop.ToLower();

    const VQwDataElement* tmp_channel = 0;

    if (device_type == "bcm") {
      tmp_channel = GetBCM(device_name)->GetCharge();
    } else if (device_type == "clock") {
      tmp_channel = GetClock(device_name)->GetTime();
    } else if (device_type == "bpmstripline") {
      if (device_prop == "x")
	tmp_channel = GetBPMStripline(device_name)->GetPosition(VQwBPM::kXAxis);
      else if (device_prop == "y")
	tmp_channel = GetBPMStripline(device_name)->GetPosition(VQwBPM::kYAxis);
      else if (device_prop == "ef")
	tmp_channel = GetBPMStripline(device_name)->GetEffectiveCharge();
    } else if (device_type == "bpmcavity") {
      if (device_prop == "x")
	tmp_channel = GetBPMCavity(device_name)->GetPosition(VQwBPM::kXAxis);
      else if (device_prop == "y")
	tmp_channel = GetBPMCavity(device_name)->GetPosition(VQwBPM::kYAxis);
      else if (device_prop == "ef")
	tmp_channel = GetBPMCavity(device_name)->GetEffectiveCharge();
    } else if (device_type == "combobpm") {
      if (device_prop == "x")
	tmp_channel = GetCombinedBPM(device_name)->GetPosition(VQwBPM::kXAxis);
      else if (device_prop == "y")
	tmp_channel = GetCombinedBPM(device_name)->GetPosition(VQwBPM::kYAxis);
      else if (device_prop == "xp")
	tmp_channel = GetCombinedBPM(device_name)->GetAngleX();
      else if (device_prop == "yp")
	tmp_channel = GetCombinedBPM(device_name)->GetAngleY();
    } else if (device_type == "combobcm") {
      tmp_channel = GetCombinedBCM(device_name)->GetCharge();
    } else if (device_type == "comboenergy") {
      tmp_channel = GetEnergyCalculator(device_name)->GetEnergy();
    }  else if (device_type == "scaler") {
      tmp_channel = GetScalerChannel(device_name)->GetScaler();  
    }else
      QwError << "QwBeamLine::PublishInternalValues() error "<< QwLog::endl;
    
     if (tmp_channel == NULL) {
       QwError << "QwBeamLine::PublishInternalValues(): " << publish_name << " not found" << QwLog::endl;
       status |= kFALSE;
     } else {
       QwDebug << "QwBeamLine::PublishInternalValues(): " << publish_name << " found" << QwLog::endl;
     }
     status = status && PublishInternalValue(publish_name, "published-value", tmp_channel);
  }
  
  return status;
}

//*****************************************************************//
void QwBeamLine::ClearEventData()
{
  for(size_t i=0;i<fClock.size();i++)
    fClock[i].get()->ClearEventData();
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].get()->ClearEventData();
  for(size_t i=0;i<fCavity.size();i++)
    fCavity[i].ClearEventData();
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].get()->ClearEventData();
  for(size_t i=0;i<fQPD.size();i++)
    fQPD[i].ClearEventData();
  for(size_t i=0;i<fLinearArray.size();i++)
    fLinearArray[i].ClearEventData();
  for(size_t i=0;i<fHaloMonitor.size();i++)
    fHaloMonitor[i].ClearEventData();

  for(size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].get()->ClearEventData();
  for(size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].get()->ClearEventData();
  for(size_t i=0;i<fECalculator.size();i++)
    fECalculator[i].ClearEventData();
  return;
}

//*****************************************************************//
Int_t QwBeamLine::GetDetectorIndex( EQwBeamInstrumentType type_id, TString name)
{
  Bool_t ldebug=kFALSE;
  if(ldebug)
    {
      std::cout<<"QwBeamLine::GetDetectorIndex\n";
      std::cout<<"type_id=="<<type_id<<" name="<<name<<"\n";
      std::cout<<fBeamDetectorID.size()<<" already registered detector\n";
    }

  Int_t result=-1;
  for(size_t i=0;i<fBeamDetectorID.size();i++)
    {
      if(fBeamDetectorID[i].fTypeID==type_id)
	if(fBeamDetectorID[i].fdetectorname==name)
	  result=fBeamDetectorID[i].fIndex;
      if(ldebug)
	{
	  std::cout<<"testing against ("<<fBeamDetectorID[i].fTypeID
		   <<","<<fBeamDetectorID[i].fdetectorname<<")=>"<<result<<"\n";
	}
    }

  return result;
}

VQwDataElement* QwBeamLine::GetElement(QwBeamDetectorID det_id)
{
  return GetElement(det_id.fTypeID, det_id.fIndex);
};

VQwDataElement* QwBeamLine::GetElement(EQwBeamInstrumentType TypeID, TString name)
{
  Int_t index = GetDetectorIndex(TypeID,name);
  return GetElement(TypeID,index);
};

VQwDataElement* QwBeamLine::GetElement(EQwBeamInstrumentType TypeID, Int_t index)
{
  VQwDataElement* tmp_ptr;
  switch (TypeID){
  case kQwBPMStripline:
    tmp_ptr = fStripline.at(index).get();
    break;
  case kQwQPD:
    tmp_ptr = &(fQPD.at(index));
    break;
  case kQwLinearArray:
    tmp_ptr = &(fLinearArray.at(index));
    break;
  case kQwBCM:
    tmp_ptr = fBCM.at(index).get();
    break;
  case kQwCombinedBCM:
    tmp_ptr = fBCMCombo.at(index).get();
    break;
  case kQwCombinedBPM:
    tmp_ptr = fBPMCombo.at(index).get();
    break;
  case kQwEnergyCalculator:
    tmp_ptr = &(fECalculator.at(index));
    break;
  case kQwHaloMonitor:
    tmp_ptr = &(fHaloMonitor.at(index));
    break;
  case kQwBPMCavity:
    tmp_ptr = &(fCavity.at(index));
    break;
  case kQwClock:
    tmp_ptr = fClock.at(index).get();
    break;
  default:
    TString loc="QwBeamLine::GetElement called by "
      +this->GetSubsystemName()+" with invalid arguements: "
      +GetQwBeamInstrumentTypeName(TypeID)+" "
      +Form("%d",index);
    throw std::invalid_argument(loc.Data());
  }
  return tmp_ptr;
};


//*****************************************************************//
VQwBPM* QwBeamLine::GetBPMStripline(const TString name)
{
  if (! fStripline.empty()) {
    for (std::vector<VQwBPM_ptr >::iterator stripline = fStripline.begin(); stripline != fStripline.end(); ++stripline) {
      if ((*stripline).get()->GetElementName() == name) {
	return (*stripline).get();
      }
    }
  }
  return 0;
}

//*****************************************************************//

QwBPMCavity* QwBeamLine::GetBPMCavity(const TString name)
{
  if (! fCavity.empty()) {
    for (std::vector<QwBPMCavity>::iterator cavity = fCavity.begin(); cavity != fCavity.end(); ++cavity) {
      if (cavity->GetElementName() == name) {
	return &(*cavity);
      }
    }
  }
  return 0;
}


//*****************************************************************//
VQwBCM* QwBeamLine::GetBCM(const TString name)
{
  //QwWarning << "QwBeamLine::GetBCM" << QwLog::endl;
  if (! fBCM.empty()) {
    for (std::vector<VQwBCM_ptr >::iterator bcm = fBCM.begin(); bcm != fBCM.end(); ++bcm) {
      if ((*bcm).get()->GetElementName() == name) {
	return (*bcm).get();
      }
    }
    
    //QwWarning << "BCM Found" << QwLog::endl;
    return 0;
  }
  return 0;
}


//*****************************************************************//
VQwClock* QwBeamLine::GetClock(const TString name)
{
  //QwWarning << "QwBeamLine::GetClock" << QwLog::endl;
  if (! fClock.empty()) {
    for (std::vector<VQwClock_ptr >::iterator clock = fClock.begin(); clock != fClock.end(); ++clock) {
      if ((*clock).get()->GetElementName() == name) {
	return (*clock).get();
      }
    }
    
    //QwWarning << "Clock Found" << QwLog::endl;
    return 0;
  }
  return 0;
}

//*****************************************************************//
VQwBCM* QwBeamLine::GetCombinedBCM(const TString name)
{
  //QwWarning << "QwBeamLine::GetCombinedBCM" << QwLog::endl;
  if (! fBCMCombo.empty()) {
    
    for (std::vector<VQwBCM_ptr>::iterator cbcm = fBCMCombo.begin(); cbcm != fBCMCombo.end(); ++cbcm) {
      if ((*cbcm).get()->GetElementName() == name) {
	return (*cbcm).get();
      }
    }
    

  }
  return 0;
}

//*****************************************************************//
VQwBPM* QwBeamLine::GetCombinedBPM(const TString name)
{
  //QwWarning << "QwBeamLine::GetCombinedBPM" << QwLog::endl;
  if (! fBPMCombo.empty()) {
    
    for (std::vector<VQwBPM_ptr>::iterator cbpm = fBPMCombo.begin(); cbpm != fBPMCombo.end(); ++cbpm) {
      if ((*cbpm).get()->GetElementName() == name) {
	return (*cbpm).get();
      }
    }
    

  }
  return 0;
}

//*****************************************************************//
QwEnergyCalculator* QwBeamLine::GetEnergyCalculator(const TString name){
   if (! fECalculator.empty()) {
    
    for (std::vector<QwEnergyCalculator>::iterator ecal = fECalculator.begin(); ecal != fECalculator.end(); ++ecal) {
      if (ecal->GetElementName() == name) {
	return &(*ecal);
      }
    }
    

  }
  return 0;
}

//*****************************************************************//
const VQwBPM* QwBeamLine::GetBPMStripline(const TString name) const
{
  return const_cast<QwBeamLine*>(this)->GetBPMStripline(name);
}

QwHaloMonitor* QwBeamLine::GetScalerChannel(const TString name){
   if (! fHaloMonitor.empty()) {
    
    for (std::vector<QwHaloMonitor>::iterator halo = fHaloMonitor.begin(); halo != fHaloMonitor.end(); ++halo) {
      if (halo->GetElementName() == name) {
       return &(*halo);
      }
    }
    

  }
  return 0;
};

//*****************************************************************//
const QwBPMCavity* QwBeamLine::GetBPMCavity(const TString name) const
{
  return const_cast<QwBeamLine*>(this)->GetBPMCavity(name);
}

//*****************************************************************//
const VQwBCM* QwBeamLine::GetBCM(const TString name) const
{
  return const_cast<QwBeamLine*>(this)->GetBCM(name);
}

//*****************************************************************//
const VQwClock* QwBeamLine::GetClock(const TString name) const
{
  return const_cast<QwBeamLine*>(this)->GetClock(name);
}

//*****************************************************************//
const VQwBCM* QwBeamLine::GetCombinedBCM(const TString name) const{
  return const_cast<QwBeamLine*>(this)->GetCombinedBCM(name);
}

//*****************************************************************//
const VQwBPM* QwBeamLine::GetCombinedBPM(const TString name) const{
  return const_cast<QwBeamLine*>(this)->GetCombinedBPM(name);
}

//*****************************************************************//
const QwEnergyCalculator* QwBeamLine::GetEnergyCalculator(const TString name) const{
  return const_cast<QwBeamLine*>(this)->GetEnergyCalculator(name);
}

 //*****************************************************************//
const QwHaloMonitor* QwBeamLine::GetScalerChannel(const TString name)const {
  return const_cast<QwBeamLine*>(this)->GetScalerChannel(name);
};


//*****************************************************************//
VQwSubsystem&  QwBeamLine::operator=  (VQwSubsystem *value)
{
  //  std::cout<<" here in QwBeamLine::operator= \n";
  if(Compare(value))
    {

      QwBeamLine* input = dynamic_cast<QwBeamLine*>(value);

      //      index_4mhz = input->index_4mhz;

      for(size_t i=0;i<input->fClock.size();i++)
	*(this->fClock[i].get())=*(input->fClock[i].get());
      for(size_t i=0;i<input->fStripline.size();i++)
	*(this->fStripline[i].get())=*(input->fStripline[i].get());
      for(size_t i=0;i<input->fQPD.size();i++)
	this->fQPD[i]=input->fQPD[i];
      for(size_t i=0;i<input->fLinearArray.size();i++)
	this->fLinearArray[i]=input->fLinearArray[i];
      for(size_t i=0;i<input->fCavity.size();i++)
	this->fCavity[i]=input->fCavity[i];
      for(size_t i=0;i<input->fBCM.size();i++)
	*(this->fBCM[i].get())=*(input->fBCM[i].get());
      for(size_t i=0;i<input->fHaloMonitor.size();i++)
	this->fHaloMonitor[i]=input->fHaloMonitor[i];
      for(size_t i=0;i<input->fBCMCombo.size();i++)
	*(this->fBCMCombo[i].get())=*(input->fBCMCombo[i].get());
      for(size_t i=0;i<input->fBPMCombo.size();i++)
	*(this->fBPMCombo[i].get())=*(input->fBPMCombo[i].get());
      for(size_t i=0;i<input->fECalculator.size();i++)
	this->fECalculator[i]=input->fECalculator[i];

      if (input->fPublishList.size()>0){
	this->fPublishList.resize(input->fPublishList.size());
	for(size_t i=0;i<input->fPublishList.size();i++){
	  this->fPublishList.at(i).resize(input->fPublishList.at(i).size());
	  for(size_t j=0;j<input->fPublishList.at(i).size();j++){
	    this->fPublishList.at(i).at(j)=input->fPublishList.at(i).at(j);
	  }
	}
      }

    }
  return *this;
}


//*****************************************************************//
VQwSubsystem&  QwBeamLine::operator+=  (VQwSubsystem *value)
{
  if(Compare(value))
    {
      //QwBeamLine* input= (QwBeamLine*)value ;
      QwBeamLine* input = dynamic_cast<QwBeamLine*>(value);

      for(size_t i=0;i<input->fClock.size();i++)
	*(this->fClock[i].get())+=*(input->fClock[i].get());
      for(size_t i=0;i<input->fStripline.size();i++)
	*(this->fStripline[i].get())+=*(input->fStripline[i].get());
      for(size_t i=0;i<input->fCavity.size();i++)
	this->fCavity[i]+=input->fCavity[i];
      for(size_t i=0;i<input->fQPD.size();i++)
	this->fQPD[i]+=input->fQPD[i];
      for(size_t i=0;i<input->fLinearArray.size();i++)
	this->fLinearArray[i]+=input->fLinearArray[i];
      for(size_t i=0;i<input->fBCM.size();i++)
	*(this->fBCM[i].get())+=*(input->fBCM[i].get());
      for(size_t i=0;i<input->fHaloMonitor.size();i++)
	this->fHaloMonitor[i]+=input->fHaloMonitor[i];
      for(size_t i=0;i<input->fBCMCombo.size();i++)
	*(this->fBCMCombo[i].get())+=*(input->fBCMCombo[i].get());
      for(size_t i=0;i<input->fBPMCombo.size();i++)
	*(this->fBPMCombo[i].get())+=*(input->fBPMCombo[i].get());
      for(size_t i=0;i<input->fECalculator.size();i++)
	this->fECalculator[i]+=input->fECalculator[i];

      if (input->fPublishList.size()>0){
	this->fPublishList.resize(input->fPublishList.size());
	for(size_t i=0;i<input->fPublishList.size();i++){
	  this->fPublishList.at(i).resize(input->fPublishList.at(i).size());
	  for(size_t j=0;j<input->fPublishList.at(i).size();j++){
	    this->fPublishList.at(i).at(j)=input->fPublishList.at(i).at(j);
	  }
	}
      }

    }
  return *this;
}

//*****************************************************************//
VQwSubsystem&  QwBeamLine::operator-=  (VQwSubsystem *value)
{

  if(Compare(value))
    {
      QwBeamLine* input = dynamic_cast<QwBeamLine*>(value);

      for(size_t i=0;i<input->fClock.size();i++)
	*(this->fClock[i].get())-=*(input->fClock[i].get());
      for(size_t i=0;i<input->fStripline.size();i++)
	*(this->fStripline[i].get())-=*(input->fStripline[i].get());
      for(size_t i=0;i<input->fCavity.size();i++)
	this->fCavity[i]-=input->fCavity[i];
      for(size_t i=0;i<input->fQPD.size();i++)
	this->fQPD[i]-=input->fQPD[i];
      for(size_t i=0;i<input->fLinearArray.size();i++)
	this->fLinearArray[i]-=input->fLinearArray[i];
      for(size_t i=0;i<input->fBCM.size();i++)
	*(this->fBCM[i].get())-=*(input->fBCM[i].get());
      for(size_t i=0;i<input->fHaloMonitor.size();i++)
	this->fHaloMonitor[i]-=input->fHaloMonitor[i];
      for(size_t i=0;i<input->fBCMCombo.size();i++)
	*(this->fBCMCombo[i].get())-=*(input->fBCMCombo[i].get());
      for(size_t i=0;i<input->fBPMCombo.size();i++)
	*(this->fBPMCombo[i].get())-=*(input->fBPMCombo[i].get());
      for(size_t i=0;i<input->fECalculator.size();i++)
	this->fECalculator[i]-=input->fECalculator[i];

      if (input->fPublishList.size()>0){
	this->fPublishList.resize(input->fPublishList.size());
	for(size_t i=0;i<input->fPublishList.size();i++){
	  this->fPublishList.at(i).resize(input->fPublishList.at(i).size());
	  for(size_t j=0;j<input->fPublishList.at(i).size();j++){
	    this->fPublishList.at(i).at(j)=input->fPublishList.at(i).at(j);
	  }
	}
      } 

    }
  return *this;
}


//*****************************************************************//
void  QwBeamLine::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this += value2;
    }
}


//*****************************************************************//
void  QwBeamLine::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if(Compare(value1)&&Compare(value2))
    {
      *this =  value1;
      *this -= value2;
    }
}


//*****************************************************************//
void QwBeamLine::Ratio(VQwSubsystem  *numer, VQwSubsystem  *denom)
{
  if(Compare(numer)&&Compare(denom))
    {
      QwBeamLine* innumer = dynamic_cast<QwBeamLine*>(numer);
      QwBeamLine* indenom = dynamic_cast<QwBeamLine*>(denom);

      for(size_t i=0;i<innumer->fClock.size();i++)
	this->fClock[i].get()->Ratio(*(innumer->fClock[i].get()),
      *(indenom->fClock[i].get()));
      for(size_t i=0;i<innumer->fStripline.size();i++)
	this->fStripline[i].get()->Ratio(*(innumer->fStripline[i].get()),
      *(indenom->fStripline[i].get()));
      for(size_t i=0;i<innumer->fCavity.size();i++)
	this->fCavity[i].Ratio(innumer->fCavity[i],indenom->fCavity[i]);
      for(size_t i=0;i<innumer->fQPD.size();i++)
	this->fQPD[i].Ratio(innumer->fQPD[i],indenom->fQPD[i]);
      for(size_t i=0;i<innumer->fLinearArray.size();i++)
	this->fLinearArray[i].Ratio(innumer->fLinearArray[i],indenom->fLinearArray[i]);
      for(size_t i=0;i<innumer->fBCM.size();i++)
	this->fBCM[i].get()->Ratio(*(innumer->fBCM[i].get()),
      *(indenom->fBCM[i].get()));
     for(size_t i=0;i<innumer->fHaloMonitor.size();i++)
	this->fHaloMonitor[i].Ratio(innumer->fHaloMonitor[i],indenom->fHaloMonitor[i]);
      for(size_t i=0;i<innumer->fBCMCombo.size();i++)
	this->fBCMCombo[i].get()->Ratio(*(innumer->fBCMCombo[i].get()),
      *(indenom->fBCMCombo[i]));
      for(size_t i=0;i<innumer->fBPMCombo.size();i++)
	this->fBPMCombo[i].get()->Ratio(*(innumer->fBPMCombo[i].get()),
      *(indenom->fBPMCombo[i].get()));
     for(size_t i=0;i<innumer->fECalculator.size();i++)
       this->fECalculator[i].Ratio(innumer->fECalculator[i],indenom->fECalculator[i]);

      // For the combined bcm, maybe we might want to think about getting
      // the asymmetry using the asymmetries of the individual bcms with a
      // weight. But right now it is unclear if really need to have that
      // option.
    }
  return;
}

//*****************************************************************//
void QwBeamLine::Scale(Double_t factor)
{
  for(size_t i=0;i<fClock.size();i++)         fClock[i].get()->Scale(factor);
  for(size_t i=0;i<fStripline.size();i++)   fStripline[i].get()->Scale(factor);
  for(size_t i=0;i<fCavity.size();i++)      fCavity[i].Scale(factor);
  for(size_t i=0;i<fQPD.size();i++)         fQPD[i].Scale(factor);
  for(size_t i=0;i<fLinearArray.size();i++) fLinearArray[i].Scale(factor);
  for(size_t i=0;i<fBCM.size();i++)         fBCM[i].get()->Scale(factor);
  for(size_t i=0;i<fHaloMonitor.size();i++) fHaloMonitor[i].Scale(factor);
  for(size_t i=0;i<fBCMCombo.size();i++)    fBCMCombo[i].get()->Scale(factor);
  for(size_t i=0;i<fBPMCombo.size();i++)    fBPMCombo[i].get()->Scale(factor);
  for(size_t i=0;i<fECalculator.size();i++) fECalculator[i].Scale(factor);
  return;
}

//*****************************************************************//
void QwBeamLine::CalculateRunningAverage()
{
  for (size_t i = 0; i < fClock.size();          i++) fClock[i].get()->CalculateRunningAverage();
  for (size_t i = 0; i < fStripline.size();    i++) fStripline[i].get()->CalculateRunningAverage();
  for (size_t i = 0; i < fCavity.size();       i++) fCavity[i].CalculateRunningAverage();
  for (size_t i = 0; i < fQPD.size();          i++) fQPD[i].CalculateRunningAverage();
  for (size_t i = 0; i < fLinearArray.size();  i++) fLinearArray[i].CalculateRunningAverage();
  for (size_t i = 0; i < fBCM.size();          i++) fBCM[i].get()->CalculateRunningAverage();
  for (size_t i = 0; i < fHaloMonitor.size();  i++) fHaloMonitor[i].CalculateRunningAverage();
  for (size_t i = 0; i < fBCMCombo.size();     i++) fBCMCombo[i].get()->CalculateRunningAverage();
  for (size_t i = 0; i < fBPMCombo.size();     i++) fBPMCombo[i].get()->CalculateRunningAverage();
  for (size_t i = 0; i < fECalculator.size();  i++) fECalculator[i].CalculateRunningAverage();
}

//*****************************************************************//
void QwBeamLine::PrintValue() const
{
  QwMessage << "=== QwBeamLine: " << GetSubsystemName() << " ===" << QwLog::endl;
  QwMessage << "Clock" << QwLog::endl;
  for (size_t i = 0; i < fClock.size();       i++) fClock[i].get()->PrintValue();
  QwMessage << "BPM stripline" << QwLog::endl;
  for (size_t i = 0; i < fStripline.size(); i++) fStripline[i].get()->PrintValue();
  QwMessage << "QPD" << QwLog::endl;
  for (size_t i = 0; i < fQPD.size(); i++) fQPD[i].PrintValue();
  QwMessage << "LinearArray" << QwLog::endl;
  for (size_t i = 0; i < fLinearArray.size(); i++) fLinearArray[i].PrintValue();
  QwMessage << "BPM cavity" << QwLog::endl;
  for (size_t i = 0; i < fCavity.size(); i++) fCavity[i].PrintValue();
  QwMessage << "BCM" << QwLog::endl;
  for (size_t i = 0; i < fBCM.size();       i++) fBCM[i].get()->PrintValue();
  QwMessage << "HaloMonitor" << QwLog::endl;
  for (size_t i = 0; i < fHaloMonitor.size();       i++) fHaloMonitor[i].PrintValue();
  QwMessage << "BPM combo" << QwLog::endl;
  for (size_t i = 0; i < fBCMCombo.size();  i++) fBCMCombo[i].get()->PrintValue();
  QwMessage << "BPM combo" << QwLog::endl;
  for (size_t i = 0; i < fBPMCombo.size();  i++) fBPMCombo[i].get()->PrintValue();
  QwMessage << "Energy " << QwLog::endl;
  for (size_t i = 0; i < fECalculator.size();  i++) fECalculator[i].PrintValue();

}

//*****************************************************************//
void QwBeamLine::AccumulateRunningSum(VQwSubsystem* value1)
{
  if (Compare(value1)) {
    QwBeamLine* value = dynamic_cast<QwBeamLine*>(value1);

    for (size_t i = 0; i < fClock.size();       i++)
      fClock[i].get()->AccumulateRunningSum(*(value->fClock[i].get()));
    for (size_t i = 0; i < fStripline.size(); i++)
      fStripline[i].get()->AccumulateRunningSum(*(value->fStripline[i].get()));
    for (size_t i = 0; i < fCavity.size(); i++)
      fCavity[i].AccumulateRunningSum(value->fCavity[i]);
    for (size_t i = 0; i < fBCM.size();       i++)
      fBCM[i].get()->AccumulateRunningSum(*(value->fBCM[i].get()));    
    for (size_t i = 0; i < fBCMCombo.size();  i++)
      fBCMCombo[i].get()->AccumulateRunningSum(*(value->fBCMCombo[i].get()));
    for (size_t i = 0; i < fBPMCombo.size();  i++)
      fBPMCombo[i].get()->AccumulateRunningSum(*(value->fBPMCombo[i].get()));
    for (size_t i = 0; i < fECalculator.size();  i++)
      fECalculator[i].AccumulateRunningSum(value->fECalculator[i]);
    for (size_t i = 0; i < fQPD.size();  i++)
      fQPD[i].AccumulateRunningSum(value->fQPD[i]);
    for (size_t i = 0; i < fLinearArray.size();  i++)
      fLinearArray[i].AccumulateRunningSum(value->fLinearArray[i]);
    for (size_t i = 0; i <fHaloMonitor.size();  i++)
      fHaloMonitor[i].AccumulateRunningSum(value->fHaloMonitor[i]);
    
  }
}

//*****************************************************************//
void QwBeamLine::DeaccumulateRunningSum(VQwSubsystem* value1){
    if (Compare(value1)) {
    QwBeamLine* value = dynamic_cast<QwBeamLine*>(value1);
    /*
    for (size_t i = 0; i < fClock.size();       i++)
      fClock[i].get()->DeaccumulateRunningSum(*(value->fClock[i].get()));
    */
    for (size_t i = 0; i < fStripline.size(); i++)
      fStripline[i].get()->DeaccumulateRunningSum(*(value->fStripline[i].get()));    
    for (size_t i = 0; i < fCavity.size(); i++)
      fCavity[i].DeaccumulateRunningSum(value->fCavity[i]);    
    for (size_t i = 0; i < fBCM.size();       i++)
      fBCM[i].get()->DeaccumulateRunningSum(*(value->fBCM[i].get()));
    for (size_t i = 0; i < fBCMCombo.size();  i++)
      fBCMCombo[i].get()->DeaccumulateRunningSum(*(value->fBCMCombo[i].get()));    
    for (size_t i = 0; i < fBPMCombo.size();  i++)
      fBPMCombo[i].get()->DeaccumulateRunningSum(*(value->fBPMCombo[i].get()));
    for (size_t i = 0; i < fQPD.size();  i++)
      fQPD[i].DeaccumulateRunningSum(value->fQPD[i]);
    for (size_t i = 0; i < fLinearArray.size();  i++)
      fLinearArray[i].DeaccumulateRunningSum(value->fLinearArray[i]);    
    for (size_t i = 0; i < fECalculator.size();  i++)
      fECalculator[i].DeaccumulateRunningSum(value->fECalculator[i]);
    for (size_t i = 0; i <fHaloMonitor.size();  i++)
      fHaloMonitor[i].DeaccumulateRunningSum(value->fHaloMonitor[i]);
    
  }
};

//*****************************************************************//
Bool_t QwBeamLine::Compare(VQwSubsystem *value)
{
  //  std::cout<<" Here in QwBeamLine::Compare \n";

  Bool_t res=kTRUE;
  if(typeid(*value)!=typeid(*this))
    {
      res=kFALSE;
      //      std::cout<<" types are not ok \n";
      //      std::cout<<" this is bypassed just for now but should be fixed eventually \n";
    }
  else
    {
      QwBeamLine* input = dynamic_cast<QwBeamLine*>(value);
      if(input->fStripline.size()!=fStripline.size())
	{
	  //	  std::cout<<" not the same number of striplines \n";
	  res=kFALSE;
	}
      else if(input->fBCM.size()!=fBCM.size())
	  {
	    res=kFALSE;
	  //	  std::cout<<" not the same number of bcms \n";
	  }
      else if(input->fHaloMonitor.size()!=fHaloMonitor.size())
	  {
	    res=kFALSE;
	  //	  std::cout<<" not the same number of halomonitors \n";
	  }
      else if(input->fClock.size()!=fClock.size()){
	    res=kFALSE;
	  //	  std::cout<<" not the same number of halomonitors \n";
      }else if(input->fBCMCombo.size()!=fBCMCombo.size()){
	res=kFALSE;
      }else if(input->fBPMCombo.size()!=fBPMCombo.size()){
	res=kFALSE;
      }else if(input->fLinearArray.size()!=fLinearArray.size()){
	res=kFALSE;
      }else if(input->fECalculator.size()!=fECalculator.size()){
	res=kFALSE;
      }else if(input->fCavity.size()!=fCavity.size()){
	res=kFALSE;
      }else if(input->fQPD.size()!=fQPD.size()){
	res=kFALSE;
      }
 
    }
  return res;
}


//*****************************************************************//
void  QwBeamLine::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  //  std::cout<<" here is QwBeamLine::ConstructHistogram with prefix ="<<prefix<<"\n";
  for(size_t i=0;i<fClock.size();i++)
      fClock[i].get()->ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fStripline.size();i++)
      fStripline[i].get()->ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fQPD.size();i++)
      fQPD[i].ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fLinearArray.size();i++)
      fLinearArray[i].ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fCavity.size();i++)
      fCavity[i].ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fBCM.size();i++)
      fBCM[i].get()->ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fHaloMonitor.size();i++)
      fHaloMonitor[i].ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fBCMCombo.size();i++)
      fBCMCombo[i].get()->ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fBPMCombo.size();i++)
      fBPMCombo[i].get()->ConstructHistograms(folder,prefix);

  for(size_t i=0;i<fECalculator.size();i++)
      fECalculator[i].ConstructHistograms(folder,prefix);
  return;
}

//*****************************************************************//
void  QwBeamLine::FillHistograms()
{
  for(size_t i=0;i<fClock.size();i++)
    fClock[i].get()->FillHistograms();
  for(size_t i=0;i<fStripline.size();i++)
    fStripline[i].get()->FillHistograms();
  for(size_t i=0;i<fQPD.size();i++)
    fQPD[i].FillHistograms();
  for(size_t i=0;i<fLinearArray.size();i++)
    fLinearArray[i].FillHistograms();
  for(size_t i=0;i<fCavity.size();i++)
    fCavity[i].FillHistograms();
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].get()->FillHistograms();
  for(size_t i=0;i<fHaloMonitor.size();i++)
    fHaloMonitor[i].FillHistograms();
  for(size_t i=0;i<fBCMCombo.size();i++)
    fBCMCombo[i].get()->FillHistograms();
  for(size_t i=0;i<fBPMCombo.size();i++)
    fBPMCombo[i].get()->FillHistograms();
  for(size_t i=0;i<fECalculator.size();i++)
    fECalculator[i].FillHistograms();

  return;
}


//*****************************************************************//
void QwBeamLine::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{

  for(size_t i = 0; i < fClock.size(); i++)
    fClock[i].get()->ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i < fStripline.size(); i++)
    fStripline[i].get()->ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i < fQPD.size(); i++)
    fQPD[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i < fLinearArray.size(); i++)
    fLinearArray[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i < fCavity.size(); i++)
    fCavity[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i < fBCM.size(); i++)
    fBCM[i].get()->ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i < fHaloMonitor.size(); i++)
    fHaloMonitor[i].ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i <fBCMCombo.size();i++)
    fBCMCombo[i].get()->ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i <fBPMCombo.size();i++)
    fBPMCombo[i].get()->ConstructBranchAndVector(tree, prefix, values);
  for(size_t i = 0; i <fECalculator.size();i++)
    fECalculator[i].ConstructBranchAndVector(tree, prefix, values);

  return;
}

//*****************************************************************//
void QwBeamLine::ConstructBranch(TTree *tree, TString & prefix)
{
  for(size_t i = 0; i < fClock.size(); i++)
    fClock[i].get()->ConstructBranch(tree, prefix);
  for(size_t i = 0; i < fStripline.size(); i++)
    fStripline[i].get()->ConstructBranch(tree, prefix);
  for(size_t i = 0; i < fQPD.size(); i++)
    fQPD[i].ConstructBranch(tree, prefix);
  for(size_t i = 0; i < fLinearArray.size(); i++)
    fLinearArray[i].ConstructBranch(tree, prefix);
  for(size_t i = 0; i < fBCM.size(); i++)
    fBCM[i].get()->ConstructBranch(tree, prefix);
  for(size_t i = 0; i <fCavity.size(); i++)
    fStripline[i].get()->ConstructBranch(tree, prefix);
  for(size_t i = 0; i < fHaloMonitor.size(); i++)
    fHaloMonitor[i].ConstructBranch(tree, prefix);
  for(size_t i = 0; i <fBCMCombo.size();i++)
    fBCMCombo[i].get()->ConstructBranch(tree, prefix);
  for(size_t i = 0; i <fBPMCombo.size();i++)
    fBPMCombo[i].get()->ConstructBranch(tree, prefix);
  for(size_t i = 0; i <fECalculator.size();i++)
    fECalculator[i].ConstructBranch(tree, prefix);


  return;
}

//*****************************************************************//
void QwBeamLine::ConstructBranch(TTree *tree, TString & prefix, QwParameterFile& trim_file)
{
  TString tmp,varname,varvalue;
  tmp="QwBCM";
  QwParameterFile* nextmodule;
  trim_file.RewindToFileStart();


  tmp="QwBPMStripline";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i < fStripline.size(); i++)
      fStripline[i].get()->ConstructBranch(tree, prefix,*nextmodule);

  }

  tmp="QwQPD";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i < fQPD.size(); i++)
      fQPD[i].ConstructBranch(tree, prefix,*nextmodule);
  }

  tmp="QwLinearDiodeArray";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i < fLinearArray.size(); i++)
      fLinearArray[i].ConstructBranch(tree, prefix,*nextmodule);
  }

  tmp="QwBPMCavity";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i < fCavity.size(); i++)
      fCavity[i].ConstructBranch(tree, prefix,*nextmodule);

  }

  tmp="QwBCM";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i < fBCM.size(); i++)
      fBCM[i].get()->ConstructBranch(tree, prefix,*nextmodule);
  }

  tmp="QwClock";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i < fClock.size(); i++)
      fClock[i].get()->ConstructBranch(tree, prefix,*nextmodule);
  }

  tmp="QwHaloMonitor";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i < fHaloMonitor.size(); i++)
      fHaloMonitor[i].ConstructBranch(tree, prefix,*nextmodule);
  }


  tmp="QwCombinedBCM";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i <fBCMCombo.size();i++)
      fBCMCombo[i].get()->ConstructBranch(tree, prefix,*nextmodule);
  }


  tmp="QwCombinedBPM";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i <fBPMCombo.size();i++)
      fBPMCombo[i].get()->ConstructBranch(tree, prefix,*nextmodule);
  }

  tmp="QwEnergyCalculator";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for(size_t i = 0; i <fECalculator.size();i++)
      fECalculator[i].ConstructBranch(tree, prefix,*nextmodule);
  }

  return;
}

//*****************************************************************//
void QwBeamLine::FillTreeVector(std::vector<Double_t> &values) const
{
  for(size_t i = 0; i < fClock.size(); i++)
    fClock[i].get()->FillTreeVector(values);
  for(size_t i = 0; i < fStripline.size(); i++)
    fStripline[i].get()->FillTreeVector(values);
  for(size_t i = 0; i < fQPD.size(); i++)
    fQPD[i].FillTreeVector(values);
  for(size_t i = 0; i < fLinearArray.size(); i++)
    fLinearArray[i].FillTreeVector(values);
  for(size_t i = 0; i < fCavity.size(); i++)
    fCavity[i].FillTreeVector(values);
  for(size_t i = 0; i < fBCM.size(); i++)
    fBCM[i].get()->FillTreeVector(values);
  for(size_t i = 0; i < fHaloMonitor.size(); i++)
    fHaloMonitor[i].FillTreeVector(values);
  for(size_t i = 0; i < fBCMCombo.size(); i++)
    fBCMCombo[i].get()->FillTreeVector(values);
  for(size_t i = 0; i < fBPMCombo.size(); i++)
    fBPMCombo[i].get()->FillTreeVector(values);
  for(size_t i = 0; i < fECalculator.size(); i++){
    fECalculator[i].FillTreeVector(values);}
  return;
}


//*****************************************************************//
void  QwBeamLine::PrintInfo() const
{
  std::cout<<"Name of the subsystem ="<<fSystemName<<"\n";
  std::cout<<"there are "<<fClock.size()<<" clock \n";
  std::cout<<"there are "<<fStripline.size()<<" striplines \n";
  std::cout<<"there are "<<fQPD.size()<<" QPDs \n";
  std::cout<<"there are "<<fLinearArray.size()<<" LinearArrays \n";
  std::cout<<"there are "<<fCavity.size()<<" cavities \n";
  std::cout<<"there are "<<fBCM.size()<<" bcm \n";
  std::cout<<"there are "<<fHaloMonitor.size()<<" halomonitors \n";
  std::cout<<"there are "<<fBCMCombo.size()<<" combined bcms \n";
  std::cout<<"there are "<<fBPMCombo.size()<<" combined bpms \n";
  std::cout<<"there are "<<fECalculator.size()<<" energy calculators \n";
  std::cout<<" Printing Running AVG and other channel info for BCMs"<<std::endl;
  for(size_t i=0;i<fBCM.size();i++)
    fBCM[i].get()->PrintInfo();
  for(size_t i=0;i<fHaloMonitor.size();i++)
    fHaloMonitor[i].PrintInfo();
  return;
}


//*****************************************************************//
void  QwBeamLine::PrintDetectorID() const
{
  for (size_t i=0;i<fBeamDetectorID.size();i++)
    {
      std::cout<<"============================="<<std::endl;
      std::cout<<" Detector ID="<<i<<std::endl;
      fBeamDetectorID[i].Print();
    }
  return;
}



//*****************************************************************//
void  QwBeamLine::Copy(const VQwSubsystem *source)
{

  try
    {
     if(typeid(*source)==typeid(*this))
	{
	  VQwSubsystem::Copy(source);

          const QwBeamLine* input = dynamic_cast<const QwBeamLine*>(source);

	  //  index_4mhz = input->index_4mhz;

	  this->fClock.reserve(input->fClock.size());
	  for(size_t i=0;i<input->fClock.size();i++) {
	    this->fClock.push_back(VQwClock_ptr(
        VQwClock::Create((input->fClock[i].get())->GetModuleType())));
      if((this->fClock[i].get()) )
        (this->fClock[i].get())->Copy((input->fClock[i].get()));
    }

	  this->fStripline.reserve(input->fStripline.size());
	  for(size_t i=0;i<input->fStripline.size();i++) {
      this->fStripline.push_back(VQwBPM_ptr(
            VQwBPM::CreateStripline(input->fStripline[i].get()->GetModuleType())));
      (this->fStripline[i].get())->Copy((input->fStripline[i].get()));
    }

	  this->fQPD.resize(input->fQPD.size());
	  for(size_t i=0;i<this->fQPD.size();i++)
	    this->fQPD[i].Copy(&(input->fQPD[i]));

	  this->fLinearArray.resize(input->fLinearArray.size());
	  for(size_t i=0;i<this->fLinearArray.size();i++)
	    this->fLinearArray[i].Copy(&(input->fLinearArray[i]));

	  this->fCavity.resize(input->fCavity.size());
	  for(size_t i=0;i<this->fCavity.size();i++)
	    this->fCavity[i].Copy(&(input->fCavity[i]));

	  this->fBCM.reserve(input->fBCM.size());
	  for(size_t i=0;i<input->fBCM.size();i++) {
	    this->fBCM.push_back(VQwBCM_ptr(
        VQwBCM::Create((input->fBCM[i].get())->GetModuleType())));
      if((this->fBCM[i].get()) )
        (this->fBCM[i].get())->Copy((input->fBCM[i].get()));
    }

	  this->fHaloMonitor.resize(input->fHaloMonitor.size());
	  for(size_t i=0;i<this->fHaloMonitor.size();i++)
	    this->fHaloMonitor[i].Copy(&(input->fHaloMonitor[i]));

	  this->fBCMCombo.reserve(input->fBCMCombo.size());
	  for(size_t i=0;i<input->fBCMCombo.size();i++) {
	    this->fBCMCombo.push_back(VQwBCM_ptr(
        VQwBCM::CreateCombo((
            input->fBCMCombo[i].get())->GetModuleType())));
      if((this->fBCMCombo[i].get()) )
        (this->fBCMCombo[i].get())->Copy((input->fBCMCombo[i].get()));
    }

	  this->fBPMCombo.reserve(input->fBPMCombo.size());
	  for(size_t i=0;i<input->fBPMCombo.size();i++){
	    this->fBPMCombo.push_back(VQwBPM_ptr(
        VQwBPM::CreateCombo(input->fBPMCombo[i].get()->GetModuleType())));
      this->fBPMCombo[i].get()->Copy(input->fBPMCombo[i].get());
	  }

	  this->fECalculator.resize(input->fECalculator.size());
	  for(size_t i=0;i<this->fECalculator.size();i++){
	    this->fECalculator[i].Copy(&(input->fECalculator[i]));
	  }

	  if (input->fPublishList.size()>0){
	    this->fPublishList.resize(input->fPublishList.size());
	    for(size_t i=0;i<input->fPublishList.size();i++){
	      this->fPublishList.at(i).resize(input->fPublishList.at(i).size());
	      for(size_t j=0;j<input->fPublishList.at(i).size();j++){
		this->fPublishList.at(i).at(j)=input->fPublishList.at(i).at(j);
	      }
	    }
	  }
	    

	}
     else
       {
	  TString loc="Standard exception from QwBeamLine::Copy = "
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

//*****************************************************************//
void QwBeamLine::FillDB(QwParityDB *db, TString datatype)
{

  Bool_t local_print_flag = false;

  if(local_print_flag) {
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
    QwMessage << "                         QwBeamLine::FillDB                      " << QwLog::endl;
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  }

  std::vector<QwDBInterface> interface;
  std::vector<QwParitySSQLS::beam> entrylist;

  UInt_t analysis_id = db->GetAnalysisID();

  TString measurement_type_bcm;
  TString measurement_type_bpm;
  TString measurement_type_halo;

  measurement_type_bcm = 
    QwDBInterface::DetermineMeasurementTypeID(datatype,"q");
  measurement_type_bpm = 
    QwDBInterface::DetermineMeasurementTypeID(datatype,"p",kTRUE);
  measurement_type_halo = 
    QwDBInterface::DetermineMeasurementTypeID(datatype);

  UInt_t i,j;
  i = j = 0;
  // try to access BCM mean and its error
  // there are 2 different types BCM data we have at the moment
  // Yield and Asymmetry
  if(local_print_flag)  QwMessage <<  QwColor(Qw::kGreen) << "Beam Current Monitors" <<QwLog::endl;

  for(i=0; i< fBCM.size(); i++) {
    interface.clear();
    interface = fBCM[i].get()->GetDBEntry();
    for (j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id );
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bcm );
      interface.at(j).PrintStatus( local_print_flag );
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }

  ///   try to access BPM mean and its error
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "Beam Position Monitors" <<QwLog::endl;
  for(i=0; i< fStripline.size(); i++) {
    //    fStripline[i].MakeBPMList();
    interface.clear();
    interface = fStripline[i].get()->GetDBEntry();
    for (j=0; j<interface.size()-5; j++){
      interface.at(j).SetAnalysisID( analysis_id ) ;
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bpm );
      interface.at(j).PrintStatus( local_print_flag);
      interface.at(j).AddThisEntryToList( entrylist );
    }
    // effective charge (last 4 elements)  need to be saved as measurement_type_bcm
    for (j=interface.size()-5; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id ) ;
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bcm );
      interface.at(j).PrintStatus( local_print_flag);
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }


  ///   try to access CombinedBPM means and errors
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "Combined Beam Position Monitors" <<QwLog::endl;
  for(i=0; i< fBPMCombo.size(); i++) {
    //    fBPMCombo[i].MakeBPMComboList();
    interface.clear();
    interface = fBPMCombo[i].get()->GetDBEntry();
    for (j=0; j<interface.size()-5; j++){
      interface.at(j).SetAnalysisID( analysis_id ) ;
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bpm );
      interface.at(j).PrintStatus( local_print_flag);
      interface.at(j).AddThisEntryToList( entrylist );
    }
    // effective charge (last element) need to be saved as measurement_type_bcm
    for (j=interface.size()-5; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id ) ;
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bcm );
      interface.at(j).PrintStatus( local_print_flag);
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }

  ///   try to access CombinedBCM means and errors
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "Combined Beam Current Monitors" <<QwLog::endl;

  for(i=0; i< fBCMCombo.size(); i++) {
    interface.clear();
    interface = fBCMCombo[i].get()->GetDBEntry();
    for (j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id );
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bcm );
      interface.at(j).PrintStatus( local_print_flag );
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }

  ///   try to access Energy Calculator mean and its error
  if(local_print_flag)  QwMessage <<  QwColor(Qw::kGreen) << "Energy Calculator" <<QwLog::endl;

  for(i=0; i< fECalculator.size(); i++) {
    interface.clear();
    interface = fECalculator[i].GetDBEntry();
    for (j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id );
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bpm );
      interface.at(j).PrintStatus( local_print_flag );
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }


  ///   try to access QPD mean and its error
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "Quadrant PhotoDiodes" <<QwLog::endl;
  for(i=0; i< fQPD.size(); i++) {
    //    fQPD[i].MakeQPDList();
    interface.clear();
    interface = fQPD[i].GetDBEntry();
    for (j=0; j<interface.size()-5; j++){
      interface.at(j).SetAnalysisID( analysis_id ) ;
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bpm );
      interface.at(j).PrintStatus( local_print_flag);
      interface.at(j).AddThisEntryToList( entrylist );
    }
    // effective charge need (last element) to be saved as measurement_type_bcm
    for (j=interface.size()-5; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id ) ;
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bcm );
      interface.at(j).PrintStatus( local_print_flag);
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }

  ///   try to access LinearArray mean and its error
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "Linear PhotoDiode Array" <<QwLog::endl;
  for(i=0; i< fLinearArray.size(); i++) {
    //    fLinearArray[i].MakeLinearArrayList();
    interface.clear();
    interface = fLinearArray[i].GetDBEntry();
    for (j=0; j<interface.size()-5; j++){
      interface.at(j).SetAnalysisID( analysis_id ) ;
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bpm );
      interface.at(j).PrintStatus( local_print_flag);
      interface.at(j).AddThisEntryToList( entrylist );
    }
    for (j=interface.size()-5; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id ) ;
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bcm );
      interface.at(j).PrintStatus( local_print_flag);
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }

  ///   try to access cavity bpm mean and its error
  if(local_print_flag) QwMessage <<  QwColor(Qw::kGreen) << "Cavity Monitors" <<QwLog::endl;
  for(i=0; i< fCavity.size(); i++) {
    //    fCavity[i].MakeBPMCavityList();
    interface.clear();
    interface = fCavity[i].GetDBEntry();
    for (j=0; j<interface.size()-5; j++){
      interface.at(j).SetAnalysisID( analysis_id ) ;
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bpm );
      interface.at(j).PrintStatus( local_print_flag);
      interface.at(j).AddThisEntryToList( entrylist );
    }
    for (j=interface.size()-5; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id ) ;
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_bcm );
      interface.at(j).PrintStatus( local_print_flag);
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }
  
  // try to access halo mean and its error
  if(local_print_flag)  QwMessage <<  QwColor(Qw::kGreen) << "Halo Monitors" <<QwLog::endl;

  for(i=0; i< fHaloMonitor.size(); i++) {
    interface.clear();
    interface = fHaloMonitor[i].GetDBEntry();
    for (j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id );
      interface.at(j).SetMonitorID( db );
      interface.at(j).SetMeasurementTypeID( measurement_type_halo );
      interface.at(j).PrintStatus( local_print_flag );
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }

  if(local_print_flag){
    QwMessage << QwColor(Qw::kGreen)   << "Entrylist Size : "
	      << QwColor(Qw::kBoldRed) << entrylist.size()
              << QwColor(Qw::kNormal)  << QwLog::endl;
  }

  db->Connect();
  // Check the entrylist size, if it isn't zero, start to query..
  if( entrylist.size() ) {
    mysqlpp::Query query= db->Query();
    query.insert(entrylist.begin(), entrylist.end());
    query.execute();
  }
  else {
    QwMessage << "QwBeamLine::FillDB :: This is the case when the entrlylist contains nothing in "<< datatype.Data() << QwLog::endl;
  }
  db->Disconnect();

  return;
}


void QwBeamLine::FillErrDB(QwParityDB *db, TString datatype)
{

  Bool_t local_print_flag = true;

  if(local_print_flag) {
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
    QwMessage << "                      QwBeamLine::FillErrDB                      " << QwLog::endl;
    QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  }

  std::vector<QwErrDBInterface> interface;
  std::vector<QwParitySSQLS::beam_errors> entrylist;

  UInt_t analysis_id = db->GetAnalysisID();

  UInt_t i,j;
  i = j = 0;
  
  if(local_print_flag)  QwMessage <<  QwColor(Qw::kGreen) << "Beam Current Monitors" <<QwLog::endl;

  for(i=0; i< fBCM.size(); i++) {
    interface.clear();
    interface = fBCM[i].get()->GetErrDBEntry();
    for (j=0; j<interface.size(); j++){
      interface.at(j).SetAnalysisID( analysis_id );
      interface.at(j).SetMonitorID( db );
    //   interface.at(j).SetMeasurementTypeID( measurement_type_bcm );
      interface.at(j).PrintStatus( local_print_flag );
      interface.at(j).AddThisEntryToList( entrylist );
    }
  }



  if(local_print_flag){
    QwMessage << QwColor(Qw::kGreen)   << "Entrylist Size : "
	      << QwColor(Qw::kBoldRed) << entrylist.size()
              << QwColor(Qw::kNormal)  << QwLog::endl;
  }

  db->Connect();
  // Check the entrylist size, if it isn't zero, start to query..
  if( entrylist.size() ) {
    mysqlpp::Query query= db->Query();
    query.insert(entrylist.begin(), entrylist.end());
    query.execute();
  }
  else {
    QwMessage << "QwBeamLine::FillErrDB :: This is the case when the entrlylist contains nothing in "<< datatype.Data() << QwLog::endl;
  }
  db->Disconnect();
  return;
}


void QwBeamLine::WritePromptSummary() const
{

  QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  QwMessage << "                  QwBeamLine::WritePromptSummary()               " << QwLog::endl;
  QwMessage << " --------------------------------------------------------------- " << QwLog::endl;
  return;
};
