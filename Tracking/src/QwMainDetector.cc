/**********************************************************\
* File: QwMainDetector.cc                                   *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#include "QwMainDetector.h"

// Qweak headers
#include "QwParameterFile.h"


const UInt_t QwMainDetector::kMaxNumberOfModulesPerROC     = 21;
const UInt_t QwMainDetector::kMaxNumberOfChannelsPerModule = 32;


// Register this subsystem with the factory
QwSubsystemFactory<QwMainDetector> theMainDetectorFactory("QwMainDetector");

QwMainDetector::QwMainDetector(TString region_tmp):VQwSubsystem(region_tmp),
    VQwSubsystemTracking(region_tmp)
{
    fDEBUG = false;
    fEvtCounter = 0;
};

QwMainDetector::~QwMainDetector()
{
  DeleteHistograms();

  fPMTs.clear();
};


Int_t QwMainDetector::LoadGeometryDefinition ( TString mapfile )
{
  std::cout<<"Main Detector Qweak Geometry Loading..... "<<std::endl;

  TString varname, varvalue,package, direction, dType;
  //  Int_t  chan;
  Int_t  plane, TotalWires, detectorId, region, DIRMODE;
  Double_t Zpos,rot,sp_res, track_res,slope_match,Det_originX,Det_originY,ActiveWidthX,ActiveWidthY,ActiveWidthZ,WireSpace,FirstWire,W_rcos,W_rsin;

  //std::vector< QwDetectorInfo >  fDetectorGeom;

  QwDetectorInfo temp_Detector;

  fDetectorInfo.clear();
  fDetectorInfo.resize ( kNumPackages );
  //  Int_t pkg,pln;

  DIRMODE=0;

  QwParameterFile mapstr ( mapfile.Data() );  //Open the file

  while ( mapstr.ReadNextLine() ) {
    mapstr.TrimComment ( '!' );   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if ( mapstr.LineIsEmpty() )  continue;

    if ( mapstr.HasVariablePair ( "=",varname,varvalue ) ) {
      //  This is a declaration line.  Decode it.
      varname.ToLower();
      //UInt_t value = atol(varvalue.Data());
      if ( varname=="name" ) { //Beginning of detector information
	DIRMODE=1;
      }
    }
    else if ( DIRMODE==1 ) {
      //  Break this line Int_to tokens to process it.
      varvalue = ( mapstr.GetNextToken ( ", " ).c_str() );//this is the sType
      Zpos = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      rot = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      sp_res = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      track_res = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      slope_match = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      package = mapstr.GetNextToken ( ", " ).c_str();
      region  = ( atol ( mapstr.GetNextToken ( ", " ).c_str() ) );
      dType = mapstr.GetNextToken ( ", " ).c_str();
      direction  = mapstr.GetNextToken ( ", " ).c_str();
      Det_originX = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      Det_originY = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      ActiveWidthX = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      ActiveWidthY = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      ActiveWidthZ = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      WireSpace = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      FirstWire = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      W_rcos = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      W_rsin = ( atof ( mapstr.GetNextToken ( ", " ).c_str() ) );
      TotalWires = ( atol ( mapstr.GetNextToken ( ", " ).c_str() ) );
      detectorId = ( atol ( mapstr.GetNextToken ( ", " ).c_str() ) );
      //std::cout<<"Detector ID "<<detectorId<<" "<<varvalue<<" Package "<<package<<" Plane "<<Zpos<<" Region "<<region<<std::endl;

      if ( region==5 ) {
	temp_Detector.SetDetectorInfo ( dType, Zpos, rot, sp_res, track_res, slope_match, package, region, direction, Det_originX, Det_originY, ActiveWidthX, ActiveWidthY, ActiveWidthZ, WireSpace, FirstWire, W_rcos, W_rsin, TotalWires, detectorId );


	if ( package == "u" )
	  fDetectorInfo.at ( kPackageUp ).push_back ( temp_Detector );
	else if ( package == "d" )
	  fDetectorInfo.at ( kPackageDown ).push_back ( temp_Detector );
      }
    }
  }

  std::cout<<"Loaded Qweak Geometry"<<" Total Detectors in pkg_d 1 "<<fDetectorInfo.at ( kPackageUp ).size() << " pkg_d 2 "<<fDetectorInfo.at ( kPackageDown ).size() <<std::endl;

  std::cout << "Sorting detector info..." << std::endl;
  plane = 1;
  std::sort ( fDetectorInfo.at ( kPackageUp ).begin(),
	      fDetectorInfo.at ( kPackageUp ).end() );

  UInt_t i = 0;
  for ( i = 0; i < fDetectorInfo.at ( kPackageUp ).size(); i++ ) {
    fDetectorInfo.at ( kPackageUp ).at ( i ).fPlane = plane++;
    std::cout<<" Region "<<fDetectorInfo.at ( kPackageUp ).at ( i ).fRegion<<" Detector ID "<<fDetectorInfo.at ( kPackageUp ).at ( i ).fDetectorID << std::endl;
  }

  plane = 1;
  std::sort ( fDetectorInfo.at ( kPackageDown ).begin(),
	      fDetectorInfo.at ( kPackageDown ).end() );
  for ( i = 0; i < fDetectorInfo.at ( kPackageDown ).size(); i++ ) {
    fDetectorInfo.at ( kPackageDown ).at ( i ).fPlane = plane++;
    std::cout<<" Region "<<fDetectorInfo.at ( kPackageDown ).at ( i ).fRegion<<" Detector ID " << fDetectorInfo.at ( kPackageDown ).at ( i ).fDetectorID << std::endl;
  }

  std::cout<<"Qweak Geometry Loaded "<<std::endl;

  return 0;
}


Int_t QwMainDetector::LoadChannelMap(TString mapfile)
{
  TString varname, varvalue;
  TString modtype, dettype, name;
  //  Int_t modnum;
  Int_t channum;

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
          //UInt_t value = atol(varvalue.Data());
          UInt_t value = QwParameterFile::GetUInt(varvalue);
          if (varname=="roc")
            {
              RegisterROCNumber(value);
            }
          else if (varname=="qdc_bank")
            {
              fBankID[0] = value;
              RegisterSubbank(value);
            }
          else if (varname=="sca_bank")
            {
              fBankID[1] = value;
              RegisterSubbank(value);
            }
          else if (varname=="tdc_bank")
            {
              fBankID[2] = value;
              RegisterSubbank(value);
            }
          else if (varname=="slot")
            {
              RegisterSlotNumber(value);
            }
          else if (varname=="module")
            {
              std::cout<<"ModuleType="<<varvalue<<"\n";
              RegisterModuleType(varvalue);
            }
        }
      else
        {
          //  Break this line into tokens to process it.
          channum   = (atol(mapstr.GetNextToken(", \t").c_str()));
          name      = mapstr.GetNextToken(", \t").c_str();

          //  Check to see if we've encountered this channel or name yet
          if (fModulePtrs.at(fCurrentIndex).at(channum).first>=0)
            {
              //  We've seen this channel
            }
          else if (FindSignalIndex(fCurrentType, name)>=0)
            {
              //  We've seen this signal
            }
          else
            {
              //  If not, push a new record into the element array
              LinkChannelToSignal(channum, name);
            }
        }
    }
  //
  ReportConfiguration();
  return 0;
};


void  QwMainDetector::ClearEventData()
{
  SetDataLoaded(kFALSE);
  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
          fPMTs.at(i).at(j).SetValue(0);
        }
    }
};

Int_t QwMainDetector::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  return 0;
};



Int_t QwMainDetector::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  //std::cout<<"EvtCont"<<fEvtCounter<<"bank_id="<<bank_id<<"  fBankID[0]="<<fBankID[0]<<"\n";

  //This is a QDC bank
  if (bank_id == fBankID[0]) {

  fEvtCounter++;

  Int_t index = GetSubbankIndex(roc_id,bank_id);

  Bool_t ldebug = false;
    if (ldebug) 
        std::cout << "QwMainDetector::QwMainDetector:  "
                  << "Begin processing ROC" << roc_id <<", Bank "<<bank_id
                  <<"(hex: "<<std::hex<<bank_id<<std::dec<<")"
                  << ", num_words "<<num_words<<", index "<<index<<std::endl;

  if (index>=0 && num_words>0)
    {
      //  We want to process this ROC.  Begin looping through the data.
      SetDataLoaded(kTRUE);
      for (size_t i=0; i<num_words ; i++)
        {
          //  Decode this word as a V775TDC word.
          DecodeTDCWord(buffer[i]);

          if (! IsSlotRegistered(index, GetTDCSlotNumber())) continue;

          if (IsValidDataword())
            {
              // This is a V775 TDC data 
              if (ldebug) {
                std::cout<<"This is a valid QDC/TDC data word. Index="<<index
                         <<" slot="<<GetTDCSlotNumber()<<" Ch="<<GetTDCChannelNumber()
                         <<" Data="<<GetTDCData()<<"\n";
                }
              try
                {
                  FillRawWord(index,GetTDCSlotNumber(),GetTDCChannelNumber(),GetTDCData());
                  //FillRawWord(index,0,GetTDCChannelNumber(),GetTDCData());
                }
              catch (std::exception& e)
                {
                  std::cerr << "Standard exception from QwDriftChamber::FillRawTDCWord: "
                  << e.what() << std::endl;
                  Int_t chan = GetTDCChannelNumber();
                  std::cerr << "   Parameters:  index=="<<index
                  << "; GetV775SlotNumber()=="<<GetTDCSlotNumber()
                  << "; GetV775ChannelNumber()=="<<chan
                  << "; GetV775Data()=="<<GetTDCData()
                  << std::endl;
                  Int_t modindex = GetModuleIndex(index, GetTDCSlotNumber());
                  std::cerr << "   GetModuleIndex()=="<<modindex
                  << "; fModulePtrs.at(modindex).size()=="
                  << fModulePtrs.at(modindex).size()
                  << "; fModulePtrs.at(modindex).at(chan).first {module type}=="
                  << fModulePtrs.at(modindex).at(chan).first
                  << "; fModulePtrs.at(modindex).at(chan).second {signal index}=="
                  << fModulePtrs.at(modindex).at(chan).second
                  << std::endl;
                }
            }
        }
    }
  }
  return 0;
};


void  QwMainDetector::ProcessEvent()
{
  if (! HasDataLoaded()) return;

};


void  QwMainDetector::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
          fPMTs.at(i).at(j).ConstructHistograms(folder, prefix);
        }
    }
};

void  QwMainDetector::FillHistograms()
{
  if (! HasDataLoaded()) return;
  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
          fPMTs.at(i).at(j).FillHistograms();
        }
    }
};


void  QwMainDetector::ConstructBranchAndVector(TTree *tree, TString prefix, std::vector<Double_t> &values)
{
  for (size_t i=0; i<fPMTs.size(); i++){
    for (size_t j=0; j<fPMTs.at(i).size(); j++){
      fPMTs.at(i).at(j).ConstructBranchAndVector(tree, prefix, values);
    }
  }
}

void  QwMainDetector::ConstructBranchAndVector(TTree *tree, TString prefix)
{

  TString basename;
  if (prefix=="") basename = "maindet";
  else basename = prefix;

  fMainDetVector.reserve(6000);
  TString list = "EvtCounter/D";
  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
          if (fPMTs.at(i).at(j).GetElementName()=="")
            {
              //  skip empty channel
            }
          else
            {
              fMainDetVector.push_back(0.0);
              if (i==0 && j==0)
                list = fPMTs.at(i).at(j).GetElementName()+"/D";
              else
                list += ":"+fPMTs.at(i).at(j).GetElementName()+"/D";
            }
        }
    }
  tree->Branch(basename, &fMainDetVector[0], list);
  //std::cout<<list<<"\n";
  return;
};


void  QwMainDetector::FillTreeVector(std::vector<Double_t> &values)
{
  if (! HasDataLoaded()) return;
  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
          fPMTs.at(i).at(j).FillTreeVector(values);
        }
    }
}

void  QwMainDetector::FillTreeVector()
{
  if (! HasDataLoaded()) return;

  Int_t index = 0;
  fMainDetVector[index] = fEvtCounter;
  index++;

  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
          if (fPMTs.at(i).at(j).GetElementName()=="") {}
          else
            {
              fMainDetVector[index] = fPMTs.at(i).at(j).GetValue();
              // std::cout<<"Fill data "<<fMainDetVector[index]<<" to index "
              // <<index<<" ch_name "<<fPMTs.at(i).at(j).GetElementName()<<"\n";
              index++;
            }
        }
    }

    if (fDEBUG) {
    std::cout<<"Main detector tree vector: "<<"\n";
    index = 0;
    for (size_t i=0; i<fPMTs.size(); i++)
      {
        for (size_t j=0; j<fPMTs.at(i).size(); j++)
          {
             std::cout<<  fMainDetVector[index]<<"\t";
             index++;
          }
      }
    std::cout<<"\n";
    }

};


void  QwMainDetector::DeleteHistograms()
{
  for (size_t i=0; i<fPMTs.size(); i++)
    {
      for (size_t j=0; j<fPMTs.at(i).size(); j++)
        {
          fPMTs.at(i).at(j).DeleteHistograms();
        }
    }
};


QwMainDetector& QwMainDetector::operator=  (const QwMainDetector &value)
{
  if (fPMTs.size() == value.fPMTs.size())
    {
      for (size_t i=0; i<fPMTs.size(); i++)
        {
          for (size_t j=0; j<fPMTs.at(i).size(); j++)
            {
              fPMTs.at(i).at(j) = value.fPMTs.at(i).at(j);
            }
        }
    }
  else
    {
      std::cerr << "QwMainDetector::operator=:  Problems!!!"
      << std::endl;
    }
  return *this;
};




void QwMainDetector::ClearAllBankRegistrations()
{
  VQwSubsystemTracking::ClearAllBankRegistrations();
  fModuleIndex.clear();
  fModulePtrs.clear();
  fModuleTypes.clear();
  fNumberOfModules = 0;
}

Int_t QwMainDetector::RegisterROCNumber(const UInt_t roc_id)
{
  VQwSubsystemTracking::RegisterROCNumber(roc_id, 0);
  fCurrentBankIndex = GetSubbankIndex(roc_id, 0);
  std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
  fModuleIndex.push_back(tmpvec);
  return fCurrentBankIndex;
};

Int_t QwMainDetector::RegisterSubbank(const UInt_t bank_id) {
    Int_t stat = VQwSubsystem::RegisterSubbank(bank_id);
    fCurrentBankIndex++;
    std::vector<Int_t> tmpvec(kMaxNumberOfModulesPerROC,-1);
    fModuleIndex.push_back(tmpvec);
    //std::cout<<"Register Subbank "<<bank_id<<" with BankIndex "<<fCurrentBankIndex<<std::endl;
    return stat;
};


Int_t QwMainDetector::RegisterSlotNumber(UInt_t slot_id)
{
  std::pair<Int_t, Int_t> tmppair;
  tmppair.first  = -1;
  tmppair.second = -1;
  if (slot_id<kMaxNumberOfModulesPerROC)
    {
      if (fCurrentBankIndex>=0 && fCurrentBankIndex<=fModuleIndex.size())
        {
          fModuleTypes.resize(fNumberOfModules+1);
          fModulePtrs.resize(fNumberOfModules+1);
          fModulePtrs.at(fNumberOfModules).resize(kMaxNumberOfChannelsPerModule,
                                                  tmppair);
          fNumberOfModules = fModulePtrs.size();
          fModuleIndex.at(fCurrentBankIndex).at(slot_id) = fNumberOfModules-1;
          fCurrentSlot     = slot_id;
          fCurrentIndex = fNumberOfModules-1;
        }
    }
  else
    {
      std::cerr << "QwMainDetector::RegisterSlotNumber:  Slot number "
      << slot_id << " is larger than the number of slots per ROC, "
      << kMaxNumberOfModulesPerROC << std::endl;
    }
  return fCurrentIndex;
};

const QwMainDetector::EModuleType QwMainDetector::RegisterModuleType(TString moduletype)
{
  moduletype.ToUpper();

std::cout<<"moduletype="<<moduletype<<"\n";
  //  Check to see if we've already registered a type for the current slot,
  //  if so, throw an error...

  if (moduletype=="V792")
    {
      fCurrentType = V792_ADC;
    }
  else if (moduletype=="V775")
    {
      fCurrentType = V775_TDC;
    }
  fModuleTypes.at(fCurrentIndex) = fCurrentType;
  if ((Int_t)fPMTs.size()<=fCurrentType)
    {
      fPMTs.resize(fCurrentType+1);
    }
  return fCurrentType;
};


Int_t QwMainDetector::LinkChannelToSignal(const UInt_t chan, const TString &name) {
    size_t index = fCurrentType;
    if (index == 0 || index == 1) {
        fPMTs.at(index).push_back(QwPMT_Channel(name));
        fModulePtrs.at(fCurrentIndex).at(chan).first  = index;
        fModulePtrs.at(fCurrentIndex).at(chan).second = fPMTs.at(index).size() -1;
    } else if (index ==2) {
        std::cout<<"scaler module has not been implemented yet."<<std::endl;
    }
    std::cout<<"Linked channel"<<chan<<" to signal "<<name<<std::endl;
    return 0;
};

void QwMainDetector::FillRawWord(Int_t bank_index,
                                 Int_t slot_num,
                                 Int_t chan, UInt_t data)
{
  Int_t modindex = GetModuleIndex(bank_index,slot_num);

  if (modindex != -1)
    {
      EModuleType modtype = EModuleType(fModulePtrs.at(modindex).at(chan).first);
      Int_t chanindex     = fModulePtrs.at(modindex).at(chan).second;

      if (modtype == EMPTY || chanindex == -1)
        {
          //  This channel is not connected to anything.
          //  Do nothing.
        }
      else
        {
          fPMTs.at(modtype).at(chanindex).SetValue(data);
        }
    };
};


Int_t QwMainDetector::GetModuleIndex(size_t bank_index, size_t slot_num) const
  {
    Int_t modindex = -1;
    //std::cout<<"bank_index="<<bank_index<<" fModuleIndex.size()="<<fModuleIndex.size()<<"\n";

    if (bank_index>=0 && bank_index<fModuleIndex.size())
      {
        if (slot_num>=0 && slot_num<fModuleIndex.at(bank_index).size())
          {
            modindex = fModuleIndex.at(bank_index).at(slot_num);
          }
      }
    return modindex;
  };


Int_t QwMainDetector::FindSignalIndex(const QwMainDetector::EModuleType modtype, const TString &name) const
  {
    size_t index = modtype;
    Int_t chanindex = -1;
    for (size_t chan=0; chan<fPMTs.at(index).size(); chan++)
      {
        if (name == fPMTs.at(index).at(chan).GetElementName())
          {
            chanindex = chan;
            break;
          }
      }
    return chanindex;
  };

void  QwMainDetector::ReportConfiguration() {
    std::cout << "Configuration of main detector:"<< std::endl;
    for (size_t i = 0; i<fROC_IDs.size(); i++) {
        for (size_t j=0; j<fBank_IDs.at(i).size(); j++) {

            Int_t ind = GetSubbankIndex(fROC_IDs.at(i),fBank_IDs.at(i).at(j));
            std::cout << "ROC " << fROC_IDs.at(i)
            << ", subbank " << fBank_IDs.at(i).at(j)
            << ":  subbank index==" << ind
            << std::endl;

            if (fBank_IDs.at(i).at(j)==fBankID[0]) {
                for (size_t k=0; k<kMaxNumberOfModulesPerROC; k++) {
                    Int_t QadcTdcindex = GetModuleIndex(ind,k);
                    if (QadcTdcindex != -1) {
                        std::cout << "    Slot " << k;
                        std::cout << "  Module#" << QadcTdcindex << std::endl;
                    }
                }
            } else if (fBank_IDs.at(i).at(j)==fBankID[2]) {

            } else if (fBank_IDs.at(i).at(j)==fBankID[1]) {
                // std::cout << "    Number of SIS3801 Scaler:  " << fSCAs.size() << std::endl;
            } else if (fBank_IDs.at(i).at(j)==fBankID[3]) {
                // std::cout << "    Number of TRIUMF ADC:  " << fADC_Data.size() << std::endl;
            }
        }
    }

}
; //ReportConfiguration()
