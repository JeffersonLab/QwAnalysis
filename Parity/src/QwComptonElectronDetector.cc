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
	if (dettype == "eacuum") {
	  if (plane >= (Int_t) fStripsRaw.size())
	    fStripsRaw.push_back(std::vector <Double_t>());
	    if (stripnum >= (Int_t) fStripsRaw[plane-1].size())
             fStripsRaw[plane-1].push_back(0.0);
             // plane goes from 1 - 4 instead of 0 - 3, 

	  if (plane >= (Int_t) fStrips.size())
	   fStrips.push_back(std::vector <Double_t>());
           if (stripnum >= (Int_t) fStrips[plane-1].size())
            fStrips[plane-1].push_back(0.0);
             // plane goes from 1 - 4 instead of 0 - 3, 
        }
	else if (dettype == "esingle") {
	  if (plane >= (Int_t) fStripsRawEv.size())
	    fStripsRawEv.push_back(std::vector <Double_t>());
	    if (stripnum >= (Int_t) fStripsRawEv[plane-1].size())
             fStripsRawEv[plane-1].push_back(0.0);
             // plane goes from 1 - 4 instead of 0 - 3, 

	  if (plane >= (Int_t) fStripsEv.size())
	   fStripsEv.push_back(std::vector <Double_t>());
           if (stripnum >= (Int_t) fStripsEv[plane-1].size())
            fStripsEv[plane-1].push_back(0.0);
             // plane goes from 1 - 4 instead of 0 - 3,    
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
  UInt_t bitwise_mask = 0;

  ports_read = 0;
  // Get the subbank index (or -1 when no match)
  Int_t index = GetSubbankIndex(roc_id, bank_id);
  //    QwOut << "bankindex for"<<bank_id << "is =" << index <<QwLog::endl;
 
  if (index==3) {
    if (num_words > 0) {
    
    //  We want to process this ROC.  Begin looping through the data.
     for (Int_t i = 0; i < NPlanes; i++) { // loop all words in bank
       for (Int_t j = 0; j < StripsPerModule; j++) {
        bitwise_mask = (UInt_t) pow(2,j);
        fStripsRawEv[i][j] = (buffer[(i)] & bitwise_mask)>>j;
       }
       words_read++;
      }
     Int_t ExtraWord = buffer[NPlanes];//diagnostic word for later use, ignore warning
       words_read++;
    }
    if (num_words != words_read) {
      QwError << "QwComptonElectronDetector: There were "
              << num_words - words_read
              << " leftover words after decoding everything we recognize."
              << QwLog::endl;
    }
   
  }  

 
  if (index==4) {
    if (num_words > 0) {
    
    //  We want to process this ROC.  Begin looping through the data.
     for (Int_t i = 0; i < NPlanes; i++) { // loop all words in bank
       for (Int_t j = 0; j < StripsPerModule; j++) {
	Int_t k = StripsPerModule + j;
        bitwise_mask = (UInt_t) pow(2,j);
        fStripsRawEv[i][k] = (buffer[(i)] & bitwise_mask)>>j;
       }
       words_read++;      
      }
     Int_t ExtraWord = buffer[NPlanes];  // diagnostic word to be used later
       words_read++;
    }
    if (num_words != words_read) {
      QwError << "QwComptonElectronDetector: There were "
              << num_words - words_read
              << " leftover words after decoding everything we recognize."
              << QwLog::endl;
    }
   
  }  

 
  if (index==5) {
    if (num_words > 0) {
    
    //  We want to process this ROC.  Begin looping through the data.
     for (Int_t i = 0; i < NPlanes; i++) { // loop all words in bank
       for (Int_t j = 0; j < StripsPerModule; j++) {
	Int_t k = 2*StripsPerModule + j;
        bitwise_mask = (UInt_t) pow(2,j);
        fStripsRawEv[i][k] = (buffer[(i)] & bitwise_mask)>>j;
       }
       words_read++;
      }
     Int_t ExtraWord = buffer[NPlanes];
       words_read++;
    }
    if (num_words != words_read) {
      QwError << "QwComptonElectronDetector: There were "
              << num_words - words_read
              << " leftover words after decoding everything we recognize."
              << QwLog::endl;
    }
   
  }  

   
   if (index==6) {   
// sub-bank 0x0204, accum mode data from strips 0-31 of planes 1 thru 4

    if (num_words > 0) {

    //  We want to process this ROC.  Begin looping through the data.
     for (Int_t i = 0; i < StripsPerModule; i++) { // loop all words in bank
       fStripsRaw[0][i] = (buffer[(i)] &0xff000000)>>24;
       fStripsRaw[1][i] = (buffer[(i)] &0x00ff0000)>>16;
       fStripsRaw[2][i] = (buffer[(i)] &0x0000ff00)>>8;
       fStripsRaw[3][i] = (buffer[(i)] &0x000000ff);
       words_read++;
      }
    }
    if (num_words != words_read) {
      QwError << "QwComptonElectronDetector: There were "
              << num_words - words_read
              << " leftover words after decoding everything we recognize."
              << QwLog::endl;
    }
   
   }  
   if (index==7) {
// sub-bank 0x0205, accum mode data from strips 32-63 of planes 1 thru 4
    if (num_words > 0) {
     for (Int_t i = 0; i < StripsPerModule; i++) { // loop over all words
       Int_t j = StripsPerModule+i;
       fStripsRaw[0][j] = (buffer[(i)] &0xff000000)>>24;
       fStripsRaw[1][j] = (buffer[(i)] &0x00ff0000)>>16;
       fStripsRaw[2][j] = (buffer[(i)] &0x0000ff00)>>8;
       fStripsRaw[3][j] = (buffer[(i)] &0x000000ff);
       words_read++;
     }
    }
    if (num_words != words_read) {
      QwError << "QwComptonElectronDetector: There were "
              << num_words - words_read
              << " leftover words after decoding everything we recognize."
              << QwLog::endl;
    }


   }
   if (index==8) {
// sub-bank 0x0206, accum mode data from strips 64-95 of planes 1 thru 4
    if (num_words > 0) {
     for (Int_t i = 0; i < StripsPerModule; i++) { // loop over all words
       Int_t j = 2*StripsPerModule+i;
       fStripsRaw[0][j] = (buffer[(i)] &0xff000000)>>24;
       fStripsRaw[1][j] = (buffer[(i)] &0x00ff0000)>>16;
       fStripsRaw[2][j] = (buffer[(i)] &0x0000ff00)>>8;
       fStripsRaw[3][j] = (buffer[(i)] &0x000000ff);
       words_read++;
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
  fCalibrationFactor = 1.0;
  fOffset = 0.0;

   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
     fStrips[i][j] = (fStripsRaw[i][j] - fOffset)*fCalibrationFactor;
     fStripsEv[i][j] = (fStripsRawEv[i][j] - fOffset)*fCalibrationFactor;
    }
   }
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
  Int_t nchan=0;
  for (Int_t i=0; i<NPlanes; i++)
    nchan += fStripsRaw[i].size();
  fEventIsGood &= (nchan == 384);
  return fEventIsGood;
};



/**
 * Clear the event data in this subsystem
 */
//*****************************************************************
void QwComptonElectronDetector::ClearEventData()
{
   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
     fStripsRaw[i][j] = 0;
     fStrips[i][j] = 0;
     fStripsRawEv[i][j] = 0;
     fStripsEv[i][j] = 0;
    }
   }
  return;
};

//*****************************************************************
VQwSubsystem&  QwComptonElectronDetector::operator=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
      this->fStripsRaw[i][j] = input->fStripsRaw[i][j];
    }
   }
  }
  return *this;
};

VQwSubsystem&  QwComptonElectronDetector::operator+=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
      this->fStripsRaw[i][j] += input->fStripsRaw[i][j];
    }
   }
  }
  return *this;
};

VQwSubsystem&  QwComptonElectronDetector::operator-=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
      this->fStripsRaw[i][j] -= input->fStripsRaw[i][j];
    }
   }
  }
  return *this;
};

VQwSubsystem&  QwComptonElectronDetector::operator*=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
      this->fStripsRaw[i][j] *= input->fStripsRaw[i][j];
    }
   }
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
  
  //  if (Compare(numer) && Compare(denom)) {
  //  QwComptonElectronDetector* innumer = dynamic_cast<QwComptonElectronDetector*> (numer);
  //  QwComptonElectronDetector* indenom = dynamic_cast<QwComptonElectronDetector*> (denom);
    
  // for (Int_t i = 0; i < NPlanes; i++){
  //  for (Int_t j = 0; j < StripsPerPlane; j++){
      //      this->fStripsRaw.Ratio(innumer->fStripsRaw[i][j], indenom->fStripsRaw[i][j]);
  //  }
  // }
  // }
  
  return; 
};

void QwComptonElectronDetector::Scale(Double_t factor)
{ 

   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
     this->fStripsRaw[i][j] *= factor;
    }
   }
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
    if (input->fStripsRaw.size() != fStripsRaw.size()) {
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

  for (Int_t i=0; i<NPlanes; i++){
    eDetRaw[i] = new TH1D(Form("%s_Accum_Raw_Pl%d",basename.Data(),i),Form("%s_Accum_Raw_Pl%d",basename.Data(),i),StripsPerPlane,0,StripsPerPlane-1);
    eDet[i] = new TH1D(Form("%s_Accum_Pl%d",basename.Data(),i),Form("%s_Accum_Pl%d",basename.Data(),i),StripsPerPlane,0,StripsPerPlane-1);
    eDetRawEv[i] = new TH1D(Form("%s_Evt_Raw_Pl%d",basename.Data(),i),Form("%s_Evt_Raw_Pl%d",basename.Data(),i),StripsPerPlane,0,StripsPerPlane-1);
    eDetEv[i] = new TH1D(Form("%s_Evt_Pl%d",basename.Data(),i),Form("%s_Evt_Pl%d",basename.Data(),i),StripsPerPlane,0,StripsPerPlane-1);
  }

  return;
};

void  QwComptonElectronDetector::DeleteHistograms()
{
  for (Int_t i=0; i<NPlanes; i++){
    if(eDetRaw[i]!=NULL){
      eDetRaw[i]->Delete();
      eDetRaw[i] = NULL;
    }
    if(eDet[i]!=NULL){
      eDet[i]->Delete();
      eDet[i] = NULL;
    }
    if(eDetRawEv[i]!=NULL){
      eDetRawEv[i]->Delete();
      eDetRawEv[i] = NULL;
    }
    if(eDetEv[i]!=NULL){
      eDetEv[i]->Delete();
      eDetEv[i] = NULL;
    }
   }
  return;
};

void  QwComptonElectronDetector::FillHistograms()
{
  Int_t i, j, k;
  for (i=0; i<NPlanes; i++) {
    for (j=0; j<StripsPerPlane; j++) {
     if (eDetRaw[i] != NULL)
      for (k=0; k<fStripsRaw[i][j]; k++)
       eDetRaw[i]->Fill(j);
     if (eDet[i] != NULL)
      for (k=0; k<fStrips[i][j]; k++)
       eDet[i]->Fill(j);
     if (eDetRawEv[i] != NULL)
      for (k=0; k<fStripsRawEv[i][j]; k++)
       eDetRawEv[i]->Fill(j);
     if (eDetEv[i] != NULL)
      for (k=0; k<fStripsEv[i][j]; k++)
       eDetEv[i]->Fill(j);
    }
  }
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
  for (Int_t i=0; i<NPlanes; i++) {
    for (Int_t j=0; j<StripsPerPlane; j++) {

    }
  }

  return;
};


void QwComptonElectronDetector::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  return;
};

void QwComptonElectronDetector::FillTreeVector(std::vector<Double_t> &values)
{
  
  return;
};


//*****************************************************************
void  QwComptonElectronDetector::Print()
{
  //  VQwSubsystemParity::Print();
  Int_t nchan =0;
  for (Int_t i=0; i<NPlanes; i++)
    nchan += fStripsRaw[i].size();
  QwOut << " there were " << nchan << " strips registered" << QwLog::endl;
  //  for (Int_t i=0; i<NPlanes; i++) {
  // for (Int_t j=0; j<StripsPerPlane; j++) {

  //   QwOut << " plane #," << i << "strip #, " << j << "has " << fStrips[i][j] << QwLog::endl;
  // }
  //}
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


      
