/**
 * \file	QwComptonElectronDetector.cc
 *
 * \brief	Implementation of the analysis of Compton electron detector data
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The QwComptonElectronDetector class is defined as a parity subsystem that
 * contains all data modules of the electron detector (V1495, ...).
 * It reads in a channel map and pedestal file, and defines the histograms
 * and output trees.
 *
 */

#include "QwComptonElectronDetector.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwLog.h"
#include "QwHistogramHelper.h"

//*****************************************************************
/**
 * Load the channel map
 * @param mapfile Map file
 * @return Zero if successful
 */
Int_t QwComptonElectronDetector::LoadChannelMap(TString mapfile)
{
  TString varname, varvalue;
  TString modtype, dettype, name;
  Int_t modnum, channum, plane, stripnum;

  QwParameterFile mapstr(mapfile.Data());  // Open the file
  while (mapstr.ReadNextLine()) {
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing whitespace (spaces or tabs).
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=", varname, varvalue)) {
      // This is a declaration line.  Decode it.
      varname.ToLower();
      UInt_t value = QwParameterFile::GetUInt(varvalue);
      if (varname == "roc") {
        RegisterROCNumber(value,0);
      } else if (varname == "bank") {
        RegisterSubbank(value);
      }
    } else {
      //  Break this line into tokens to process it.
      modtype   = mapstr.GetNextToken(", \t").c_str();
      modnum    = (atol(mapstr.GetNextToken(", \t").c_str()));
      channum   = (atol(mapstr.GetNextToken(", \t").c_str()));
      dettype   = mapstr.GetNextToken(", \t").c_str();
      name      = mapstr.GetNextToken(", \t").c_str();
      plane     = (atol(mapstr.GetNextToken(", \t").c_str()));
      stripnum  = (atol(mapstr.GetNextToken(", \t").c_str()));
      //  Push a new record into the element array
      if (modtype == "V1495") {
	if (dettype == "eaccum") {
	  if (channum >= (Int_t) fFPGAChannel_ac.size())
           fFPGAChannel_ac.push_back(QwCPV1495_Channel());

           TString chname = modtype + Form("_plane%ld",plane) + Form("_strip%ld",stripnum);
	   fFPGAChannel_ac[channum].SetElementName(chname + "Ch");
	   fFPGAChannel_ac.at(channum).InitializeChannel(channum, name, plane, stripnum);
	} // end of switch (dettype)
      } // end of switch (modtype)
    } // end of if for token line
  } // end of while over parameter file
  return 0;
};

//*****************************************************************
Int_t QwComptonElectronDetector::LoadEventCuts(TString & filename)
{
  return 0;
};

//*****************************************************************
Int_t QwComptonElectronDetector::LoadInputParameters(TString pedestalfile)
{
  TString varname;
  Double_t varcal;
  Bool_t notfound;
  notfound = kTRUE;

  // Open the file
  QwParameterFile mapstr(pedestalfile.Data());
  while (mapstr.ReadNextLine()) {
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;
    else {
      varname = mapstr.GetNextToken(", \t").c_str();  // name of the channel
      varname.ToLower();
      varname.Remove(TString::kBoth,' ');
      varcal = (atof(mapstr.GetNextToken(", \t").c_str())); // value of the calibration factor
    }

    
  } // end of loop reading all lines of the pedestal file

  return 0;
}

//*****************************************************************
void QwComptonElectronDetector::RandomizeEventData(int helicity)
{
}

//*****************************************************************
void QwComptonElectronDetector::EncodeEventData(std::vector<UInt_t> &buffer)
{
}

/**
 * Process the event buffer for this subsystem
 * @param roc_id ROC ID
 * @param bank_id Subbank ID
 * @param buffer Buffer to read from
 * @param num_words Number of words left in buffer
 * @return Number of words read
 */
//*****************************************************************
Int_t QwComptonElectronDetector::ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  UInt_t words_read = 0;
  Int_t ports_read;
  ports_read = 0;

  // Get the subbank index (or -1 when no match)
  Int_t index = GetSubbankIndex(roc_id, bank_id);

   
   if (bank_id==0x0204) {   
// sub-bank 0x0204, accum mode data from strips 0-31 of planes 1 thru 4

    if (index >= 0 && num_words > 0) {

    //  We want to process this ROC.  Begin looping through the data.
     for (Int_t i = 0; i < NPlanes; i++) { // loop over planes
      for (Int_t j =0; j < WordsPerPlane; j++) {// loop over words per plane 
       Int_t k = ChannelsPerPort*i+ChannelsPerWord*j;
       fFPGAChannel_ac[k+0] = (buffer[(WordsPerPlane*i+j)] &0xff000000)>>24;
       fFPGAChannel_ac[k+1] = (buffer[(WordsPerPlane*i+j)] &0x00ff0000)>>16;
       fFPGAChannel_ac[k+2] = (buffer[(WordsPerPlane*i+j)] &0x0000ff00)>>8;
       fFPGAChannel_ac[k+3] = (buffer[(WordsPerPlane*i+j)] &0x000000ff);
      }
     }
    }
    if (num_words != words_read) {
      QwError << "QwComptonElectronDetector: There were "
              << num_words - words_read
              << " leftover words after decoding everything we recognize."
              << QwLog::endl;
    }
   
   }  
   if (bank_id==0x0205) {
// sub-bank 0x0205, accum mode data from strips 32-63 of planes 1 thru 4
    if (index >= 0 && num_words > 0) {
     for (Int_t i = 0; i < NPlanes; i++) { // loop over planes
      for (Int_t j =0; j < WordsPerPlane; j++) {// loop over words per plane 
       Int_t k = ChannelsPerModule+ChannelsPerPort*i+ChannelsPerWord*j;
       fFPGAChannel_ac[k+0] = (buffer[(WordsPerPlane*i+j)] &0xff000000)>>24;
       fFPGAChannel_ac[k+1] = (buffer[(WordsPerPlane*i+j)] &0x00ff0000)>>16;
       fFPGAChannel_ac[k+2] = (buffer[(WordsPerPlane*i+j)] &0x0000ff00)>>8;
       fFPGAChannel_ac[k+3] = (buffer[(WordsPerPlane*i+j)] &0x000000ff);
      }
     }
    }
    if (num_words != words_read) {
      QwError << "QwComptonElectronDetector: There were "
              << num_words - words_read
              << " leftover words after decoding everything we recognize."
              << QwLog::endl;
    }


   }
   if (bank_id==0x0206) {
// sub-bank 0x0206, accum mode data from strips 64-95 of planes 1 thru 4
    if (index >= 0 && num_words > 0) {
     for (Int_t i = 0; i < NPlanes; i++) { // loop over planes
      for (Int_t j =0; j < WordsPerPlane; j++) {// loop over words per plane 
       Int_t k = 2*ChannelsPerModule+ChannelsPerPort*i+ChannelsPerWord*j;
       fFPGAChannel_ac[k+0] = (buffer[(WordsPerPlane*i+j)] &0xff000000)>>24;
       fFPGAChannel_ac[k+1] = (buffer[(WordsPerPlane*i+j)] &0x00ff0000)>>16;
       fFPGAChannel_ac[k+2] = (buffer[(WordsPerPlane*i+j)] &0x0000ff00)>>8;
       fFPGAChannel_ac[k+3] = (buffer[(WordsPerPlane*i+j)] &0x000000ff);
      }
     }
    }
    if (num_words != words_read) {
      QwError << "QwComptonElectronDetector: There were "
              << num_words - words_read
              << " leftover words after decoding everything we recognize."
              << QwLog::endl;
    }
   }
  return words_read;
};

/**
 * Process the single event cuts
 * @return
 */
//*****************************************************************
Bool_t QwComptonElectronDetector::SingleEventCuts()
{
  QwOut << "QwComptonElectronDetector::SingleEventCuts()" << QwLog::endl;
  return IsGoodEvent();
};

/**
 * Process this event
 */
//*****************************************************************
void  QwComptonElectronDetector::ProcessEvent()
{   
    Int_t i = 0;
    fFPGAChannel_ac[i].ProcessEvent();
   
   return;
};

/**
 * Process the configuration buffer for this subsystem
 * @param roc_id ROC ID
 * @param bank_id Subbank ID
 * @param buffer Buffer to read from
 * @param num_words Number of words left in buffer
 * @return Number of words read
 */
//*****************************************************************
Int_t QwComptonElectronDetector::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{
  return 0;
};

/**
 * Check whether this is a good event
 * @return True if the event is good
 */
//*****************************************************************
Bool_t QwComptonElectronDetector::IsGoodEvent()
{
  Bool_t fEventIsGood = kTRUE;
  fEventIsGood &= (fFPGAChannel_ac.size() == 384);
  return fEventIsGood;
};



/**
 * Clear the event data in this subsystem
 */
//*****************************************************************
void QwComptonElectronDetector::ClearEventData()
{
  for (size_t i =0; i < fFPGAChannel_ac.size();i++)
    fFPGAChannel_ac[i] = 0;

  Int_t i=0;
  fFPGAChannel_ac[i].ClearEventData();
  return;
};

//*****************************************************************
VQwSubsystem&  QwComptonElectronDetector::operator=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
    for (size_t i = 0; i < input->fFPGAChannel_ac.size(); i++)
      this->fFPGAChannel_ac[i] = input->fFPGAChannel_ac[i];
  }
  return *this;
};

VQwSubsystem&  QwComptonElectronDetector::operator+=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
    for (size_t i = 0; i < input->fFPGAChannel_ac.size(); i++)
      this->fFPGAChannel_ac[i] += input->fFPGAChannel_ac[i];
  }
  return *this;
};

VQwSubsystem&  QwComptonElectronDetector::operator-=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
    for (size_t i = 0; i < input->fFPGAChannel_ac.size(); i++)
      this->fFPGAChannel_ac[i] -= input->fFPGAChannel_ac[i];
  }
  return *this;
};

VQwSubsystem&  QwComptonElectronDetector::operator*=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
    for (size_t i = 0; i < input->fFPGAChannel_ac.size(); i++)
      this->fFPGAChannel_ac[i] *= input->fFPGAChannel_ac[i];
  }
  return *this;
};


void  QwComptonElectronDetector::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2)) {
    *this  = value1;
    *this += value2;
  }
};

void  QwComptonElectronDetector::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2)) {
    *this  = value1;
    *this -= value2;
  }
};

void QwComptonElectronDetector::Ratio(VQwSubsystem *numer, VQwSubsystem *denom)
{
  if (Compare(numer) && Compare(denom)) {
    QwComptonElectronDetector* innumer = dynamic_cast<QwComptonElectronDetector*> (numer);
    QwComptonElectronDetector* indenom = dynamic_cast<QwComptonElectronDetector*> (denom);
    for (size_t i = 0; i < innumer->fFPGAChannel_ac.size(); i++)
      this->fFPGAChannel_ac[i].Ratio(innumer->fFPGAChannel_ac[i], indenom->fFPGAChannel_ac[i]);
  }
  return; 
};

void QwComptonElectronDetector::Scale(Double_t factor)
{ 
    for (size_t i = 0; i < fFPGAChannel_ac.size(); i++)
     this->fFPGAChannel_ac[i] *= factor;
};

Bool_t QwComptonElectronDetector::Compare(VQwSubsystem *value)
{
  // Immediately fail on null objects
  if (value == 0) return kFALSE;

  // Continue testing on actual object
  Bool_t result = kTRUE;
  if (typeid(*value) != typeid(*this)) {
    result = kFALSE;
  } else {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
    if (input->fFPGAChannel_ac.size() != fFPGAChannel_ac.size()) {
      result = kFALSE;
    }
  }
  return result;
}
/**
 * Construct the histograms
 * @param folder Folder in which the histograms will be created
 * @param prefix Prefix with information about the type of histogram
 */
//*****************************************************************
void  QwComptonElectronDetector::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();
    //  Now create the histograms.
  TString basename = GetSubsystemName();
  //  basename = prefix + GetSubsystemName();

  for (Int_t i=0; i<NPorts; i++){
    V1495[i] = new TH1D(Form("%s_V1495%d",basename.Data(),i),Form("%s_V1495%d",basename.Data(),i),ChannelsPerPort,0,ChannelsPerPort-1);
  }
  Int_t i = 0; 
  fFPGAChannel_ac[i].ConstructHistograms(folder, prefix);
  return;
};

void  QwComptonElectronDetector::DeleteHistograms()
{
  for (Int_t i=0; i<NPorts; i++){
    if(V1495[i]!=NULL){
      V1495[i]->Delete();
      V1495[i] = NULL;
    }
   }
  size_t i = 0;
  fFPGAChannel_ac[i].DeleteHistograms();
  return;
};

void  QwComptonElectronDetector::FillHistograms()
{
  Int_t i, j, k;
  i = j = k = 0;
  for (i=0; i<NPorts; i++) {
    for (j=0; j<ChannelsPerPort; j++) {
      k = ChannelsPerPort*i + j;
        if (V1495[i] != NULL)
	  V1495[i]->Fill(this->V1495ChannelValue[k]);
    }
  }
  
  i = 0; 
  fFPGAChannel_ac[i].FillHistograms();
  return;
};

/**
 * Construct the tree
 * @param folder Folder in which the tree will be created
 * @param prefix Prefix with information about the type of histogram
 */
void  QwComptonElectronDetector::ConstructTree(TDirectory *folder, TString &prefix)
{
  folder->cd();
  fTree = new TTree("ComptonElectron", "Compton Electron Detector");
  fTree->Branch("nevents",&fTree_fNEvents,"nevents/I");
  return;
};

/**
 * Delete the tree
 */
void  QwComptonElectronDetector::DeleteTree()
{
  delete fTree;
  return;
};

/**
 * Fill the tree with data
 */
void  QwComptonElectronDetector::FillTree()
{
  for(size_t i = 0; i < fFPGAChannel_ac.size(); i++) {
    //  fTree_fNEvents = fFPGAChannel_ac[i].GetNumberOfEvents();
    // fTree->Fill();  To be fixed
  }

  return;
};


void QwComptonElectronDetector::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
    if (prefix=="") prefix = "V1495_";
    TString basename = prefix + "event";
    fFPGAChannelVector.reserve(6000);    

    fFPGAChannelVector.push_back(0.0);
    TString list = "EvtCounter/D";
    fFPGAChannelVector.push_back(0.0);    
    list += ":TrigEvtCounter/D";
    fFPGAChannelVector.push_back(0.0);
    list += ":SumEvtCounter/D";


    tree->Branch(basename, &fFPGAChannelVector[0], list);

    Int_t i = 0;
    fFPGAChannel_ac[i].ConstructBranchAndVector(tree, prefix, values);
  return;
};

void QwComptonElectronDetector::FillTreeVector(std::vector<Double_t> &values)
{
     Int_t index = 0;

     fFPGAChannelVector[index++] = 0; // to be done 
    fFPGAChannelVector[index++] = 0;
    fFPGAChannelVector[index++] = 0;
  Int_t i = 0; 
  fFPGAChannel_ac[i].FillTreeVector(values);
  return;
};


//*****************************************************************
void  QwComptonElectronDetector::Print()
{
  VQwSubsystemParity::Print();
  QwOut << " there are " << fFPGAChannel_ac.size() << " V1495 modules" << QwLog::endl;

  return;
}

//*****************************************************************
 void  QwComptonElectronDetector::Copy(VQwSubsystem *source)
{
  return;

}

VQwSubsystem*  QwComptonElectronDetector::Copy()
{

  //  QwComptonElectronDetector* TheCopy = new QwComptonElectronDetector(this->GetSubsystemName() + " Copy");
  // copy->Copy(this);
  // return copy;
  return NULL;
}
/**
 * Get the V1495 Channels for this electron detector
 * @param name Name of the V1495 Channels
 * @return Pointer to the V1495 Channels
 */
QwCPV1495_Channel* QwComptonElectronDetector::GetCPV1495Channel(const TString name)
{
  if (! fFPGAChannel_ac.empty()) {
    for (std::vector<QwCPV1495_Channel>::iterator accum = fFPGAChannel_ac.begin(); accum != fFPGAChannel_ac.end(); ++accum) {
      if (accum->GetElementName() == name) {
        return &(*accum);
      }
    }
  }
  return 0;

};
