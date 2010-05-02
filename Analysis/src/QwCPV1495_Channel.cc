/**
 * \file	QwCPV1495_Channel.cc
 *
 * \brief	Implementation of the decoding of V1495 data
 *
 * \author	D. Dutta
 * \date	2010-04-02 17:47:23
 * \ingroup	QwCompton
 *
 * The QwCPV1495 set of classes is defined to read the accumulation mode and 
 * event mode data from the Compton electron detector.  Because the scope of 
 * this module is similar the the MQwSIS3320 module, parts of
 * this class are very similar to MQwSIS3320_Channel.
 *
 * Additional functionality is required to process the sampling data in the
 * V1495 events.
 *
 */

#include "QwCPV1495_Channel.h"

// Qweak headers
#include "QwLog.h"


// Compile-time debug level
const Bool_t QwCPV1495_Channel::kDEBUG = kFALSE;



/**
 * Initialize the QwCPV1495_Channel by assigning it a name
 * @param channum Number of the channel
 * @param name Name for the channel
 * @param plane number of channel
 * @param first strip of module
 */
void  QwCPV1495_Channel::InitializeChannel(UInt_t channel, TString name, UInt_t plane, UInt_t stripnum)
{
  // Inherited from VQwDataElement
  SetElementName(name);
  SetNumberOfDataWords(0);

  // Set plane and strip number
  fChannel = channel;
  fPlane = plane;
  fNStrip = stripnum;

  // Set channel names
    TString chname = GetElementName() + Form("_plane%ld",fPlane) + Form("_strip%ld",fNStrip);
    fStrips[fPlane][fNStrip].SetElementName(chname);
    fStripsRaw[fPlane][fNStrip].SetElementName(chname + "_Raw");

  // Default values when no event read yet
  fCurrentEvent = -1;

  // calibration (mm to GeV), to be added
  fCalibrationFactor = 1.0;
  fOffset = 0.0;

  return;
};


/**
 * Clear the event data in accumulators
 */
void QwCPV1495_Channel::ClearEventData()
{

  // Clear the accumulators
   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
      fStrips[i][j]=0;
      fStripsRaw[i][j]=0;
    }
   }
};


void QwCPV1495_Channel::EncodeEventData(std::vector<UInt_t> &buffer)
{
  return;
};

Int_t QwCPV1495_Channel::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index)
{
  return 0;
};

/**
 * Process the event by removing pedestals and applying calibration
 */
void QwCPV1495_Channel::ProcessEvent()
{
  for (Int_t l = 0; l < NBoards; l++){
   for (Int_t j = 0; j < StripsPerBoard; j++){
    for (Int_t i = 0; i < NPlanes; i++){
     Int_t k = StripsPerBoard*NPlanes*l + NPlanes*j + i;
     Int_t ij =StripsPerBoard*l + j ; 
     fStripsRaw[i][ij] = fFPGAChannel_ac[k];
     fStrips[i][ij] = (fFPGAChannel_ac[k] - fOffset)*fCalibrationFactor;
    }
   }
  }
  return;
};

/**
 * Addition of offset
 * @param value Right-hand side
 * @return Left-hand side
 */
const QwCPV1495_Channel QwCPV1495_Channel::operator+ (const Double_t &value) const
{
  QwCPV1495_Channel result = *this;
  result += value;
  return result;
};

/**
 * Subtraction of offset
 * @param value Right-hand side
 * @return Left-hand side
 */
const QwCPV1495_Channel QwCPV1495_Channel::operator- (const Double_t &value) const
{
  QwCPV1495_Channel result = *this;
  result -= value;
  return result;
};

const QwCPV1495_Channel QwCPV1495_Channel::operator* (const Double_t &value) const
{
  QwCPV1495_Channel result = *this;
  result *= value;
  return result;
};


/**
 * Addition
 * @param value Right-hand side
 * @return Left-hand side
 */
const QwCPV1495_Channel QwCPV1495_Channel::operator+ (const QwCPV1495_Channel &value) const
{
  QwCPV1495_Channel result = *this;
  result += value;
  return result;
};

/**
 * Subtraction
 * @param value Right-hand side
 * @return Left-hand side
 */
const QwCPV1495_Channel QwCPV1495_Channel::operator- (const QwCPV1495_Channel &value) const
{
  QwCPV1495_Channel result = *this;
  result -= value;
  return result;
};

const QwCPV1495_Channel QwCPV1495_Channel::operator* (const QwCPV1495_Channel &value) const
{
  QwCPV1495_Channel result = *this;
  result *= value;
  return result;
};


/**
 * Assignment
 * @param value Right-hand side
 * @return Left-hand side
 */
QwCPV1495_Channel& QwCPV1495_Channel::operator= (const QwCPV1495_Channel &value)
{
  if (!IsNameEmpty()) {
   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
       this->fStrips[i][j] = value.fStrips[i][j];
    }
   }
  }
  return *this;
};

/**
 * Addition assignment of offset
 * @param value Right-hand side
 * @return Left-hand side
 */
QwCPV1495_Channel& QwCPV1495_Channel::operator+= (const Double_t &value)
{
  if (!IsNameEmpty()) {
   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
          this->fStrips[i][j] += value;
    }
   }
  }
  return *this;
};

/**
 * Subtraction assignment of offset
 * @param value Right-hand side
 * @return Left-hand side
 */
QwCPV1495_Channel& QwCPV1495_Channel::operator-= (const Double_t &value)
{
  if (!IsNameEmpty()) {
   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
      this->fStrips[i][j] -= value;
    }
   }
  }
  return *this;
};

QwCPV1495_Channel& QwCPV1495_Channel::operator*= (const Double_t &value)
{
  if (!IsNameEmpty()) {
   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
      this->fStrips[i][j] *= value;
    }
   }
  }
  return *this;
};

/**
 * Addition assignment
 * @param value Right-hand side
 * @return Left-hand side
 */
QwCPV1495_Channel& QwCPV1495_Channel::operator+= (const QwCPV1495_Channel &value)
{
  if (!IsNameEmpty()) {
   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
      this->fStrips[i][j] += value.fStrips[i][j];
    }
   }
  }
  return *this;
};

/**
 * Subtraction assignment
 * @param value Right-hand side
 * @return Left-hand side
 */
QwCPV1495_Channel& QwCPV1495_Channel::operator-= (const QwCPV1495_Channel &value)
{
  if (!IsNameEmpty()) {
   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
      this->fStrips[i][j] -= value.fStrips[i][j];
    }
   }
  }
  return *this;
};

QwCPV1495_Channel& QwCPV1495_Channel::operator*= (const QwCPV1495_Channel &value)
{
  if (!IsNameEmpty()) {
   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
      this->fStrips[i][j] *= value.fStrips[i][j];
    }
   }
  }
  return *this;
};

/**
 * Sum of two channels
 * @param value1
 * @param value2
 */
void QwCPV1495_Channel::Sum(QwCPV1495_Channel &value1, QwCPV1495_Channel &value2)
{
  *this =  value1;
  *this += value2;
};

/**
 * Difference of two channels
 * @param value1
 * @param value2
 */
void QwCPV1495_Channel::Difference(QwCPV1495_Channel &value1, QwCPV1495_Channel &value2)
{
  *this =  value1;
  *this -= value2;
};

/**
 * Addition of a offset
 * @param offset
 */
void QwCPV1495_Channel::Offset(Double_t offset)
{
  if (!IsNameEmpty()) {
   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
      this->fStrips[i][j] += offset;
    }
   }
  }
};

/**
 * Scaling by a scale factor
 * @param scale
 */
void QwCPV1495_Channel::Scale(Double_t scale)
{
  if (!IsNameEmpty()) {
   for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
            this->fStrips[i][j] *= scale;
    }
   }
  }
};

/**
 * Construct the histograms
 * @param folder Folder in which the histograms will be created
 * @param prefix Prefix with information about the type of histogram
 */
//*****************************************************************
void  QwCPV1495_Channel::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();
    //  Now create the histograms.
  TString basename, fullname;
  basename = prefix + GetElementName();
  for (Int_t i=0; i<NPlanes; i++){
    ePlane[i] = new TH1D(Form("%s_ePlane%d",basename.Data(),i),Form("%s_ePlane%d",basename.Data(),i),StripsPerPlane,0,StripsPerPlane-1);
    ePlaneRaw[i] = new TH1D(Form("%s_ePlaneRaw%d",basename.Data(),i),Form("%s_ePlane%d",basename.Data(),i),StripsPerPlane,0,StripsPerPlane-1);
  }
};


void  QwCPV1495_Channel::DeleteHistograms()
{
  for (Int_t i=0; i<NPlanes; i++){
    if(ePlane[i]!=NULL){
      ePlane[i]->Delete();
      ePlane[i] = NULL;
    }
    if(ePlaneRaw[i]!=NULL){
      ePlaneRaw[i]->Delete();
      ePlaneRaw[i] = NULL;
    }
  }
};

void  QwCPV1495_Channel::FillHistograms(){
  for (Int_t i=0; i<NPlanes; i++) {
   for (Int_t j=0; j<StripsPerPlane; j++) {
      //  if (ePlane[i] != NULL)
	//       ePlane[i]->Fill(this->fStrips[i][j]);
      //  if (ePlaneRaw[i] != NULL)
	//       ePlaneRaw[i]->Fill(this->fStripsRaw[i][j]);

	// to be fixed

   }
  }
};    

void  QwCPV1495_Channel::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  // Strips

    if (prefix=="") prefix = "ePlane_";
    TString basename = prefix + "event";
    fPlaneVector.reserve(6000);    

    fPlaneVector.push_back(0.0);
    TString list = "EvtCounter/D";
    fPlaneVector.push_back(0.0);    
    list += ":TrigEvtCounter/D";
    fPlaneVector.push_back(0.0);
    list += ":SumEvtCounter/D";


    tree->Branch(basename, &fPlaneVector[0], list);

};

void  QwCPV1495_Channel::FillTreeVector(std::vector<Double_t> &values)
{
  // Strips
     Int_t index = 0;

    fPlaneVector[index++] = 0; // to be done 
    fPlaneVector[index++] = 0;
    fPlaneVector[index++] = 0;
};

/**
 * Print some debugging information about the QwCPV1495_Channel
 */
void QwCPV1495_Channel::Print() const
{

  return;
}

