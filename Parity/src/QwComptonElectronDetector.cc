/**************************************************
 * \file	QwComptonElectronDetector.cc
 * \brief	Implementation of the analysis of Compton electron detector data
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * \D. Dutta
 *
 * \V. Tvaskis
 * I have addet hits selections procedure, to select best hits, which corresponds
 * to the best track (track with the best Chi2). Tracking procedure is also included here.
 * Also, Angular distribution of the tracks is added.
 *
 * \A. Narayan 
****************************************************/

/*****************************************************
 * The QwComptonElectronDetector class is defined as a parity subsystem that
 * contains all data modules of the electron detector (V1495, ...).
 * It reads in a channel map and pedestal file, and defines the histograms
 * and output trees.
 *****************/

#include "QwComptonElectronDetector.h"

#include "QwSubsystemArrayParity.h"
#include "MQwCodaControlEvent.h"

// System headers
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <sstream>

using namespace std;

// Qweak headers
#include "QwLog.h"
#include "QwHistogramHelper.h"


// Register this subsystem with the factory
RegisterSubsystemFactory(QwComptonElectronDetector);
/* (in header file)
// Assign static const member fields
const Int_t QwComptonElectronDetector::NModules = 3;///number of slave modules
const Int_t QwComptonElectronDetector::NPlanes = 4;///number of diamond detector planes
const Int_t QwComptonElectronDetector::StripsPerModule = 32;///number of strips in each v1495 slave module
const Int_t QwComptonElectronDetector::StripsPerPlane = 96;///total number of strips in each detecor
*/

const Int_t numberofWordPerEv = 5;

// Ugly, should go inside functions, seems undefined for running sum
Int_t myrun;

/*****************************************************************
 * Load the channel map
 * @param mapfile Map file
 * @return Zero if successful
 *****************************************************************/
Int_t QwComptonElectronDetector::LoadChannelMap(TString mapfile)
{
  TString varname, varvalue;
  TString modtype, dettype, name;
  UInt_t modnum, channum;
  Int_t plane, stripnum, fdettype;
  Int_t currentrocread=0;
  Int_t currentbankread=0;
  Int_t currentsubbankindex=-1;


  QwParameterFile mapstr(mapfile.Data());  // Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());
  while (mapstr.ReadNextLine()) {
    mapstr.TrimComment();    // Remove everything after a comment character.
    mapstr.TrimWhitespace(); // Get rid of leading and trailing whitespace (spaces or tabs).
    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=", varname, varvalue)) {
      // This is a declaration line.  Decode it.
      varname.ToLower();
      UInt_t value = QwParameterFile::GetUInt(varvalue);
      if (varname == "roc") {
	currentrocread=value;
        RegisterROCNumber(value,0);
      } else if (varname == "bank") {
        currentbankread=value;
        RegisterSubbank(value);
	if(currentsubbankindex!=GetSubbankIndex(currentrocread,currentbankread))
	  currentsubbankindex=GetSubbankIndex(currentrocread,currentbankread);
      }
    } else {
      //  Break this line into tokens to process it.
      modtype   = mapstr.GetNextToken(", \t").c_str();
      modnum    = QwParameterFile::GetUInt(mapstr.GetNextToken(", \t"));
      channum   = QwParameterFile::GetUInt(mapstr.GetNextToken(", \t"));
      dettype   = mapstr.GetNextToken(", \t").c_str();
      name      = mapstr.GetNextToken(", \t").c_str();
      plane     = (atol(mapstr.GetNextToken(", \t").c_str()));
      stripnum  = (atol(mapstr.GetNextToken(", \t").c_str()));
      //  Push a new record into the element array
      if (modtype == "V1495") {
	if (dettype == "eaccum") {
          // Register data channel type
          fMapping[currentsubbankindex] = kV1495Accum;
          fdettype = 1;
	  ///the assignment of fdettype is arbitrarily made but is honoured subsequently
	  ///due to the numbers assigned for fdettype, the first index of vector 'fSubbankIndex' holds data
	  ///in the order:: 0:esingle; 1:eaccum; 2:escalers
 	  if (fdettype >= (Int_t) fSubbankIndex.size())
	    fSubbankIndex.resize(fdettype+1);
          if (modnum >= fSubbankIndex[fdettype].size()) {
            fSubbankIndex[fdettype].push_back(modnum);
            fSubbankIndex[fdettype][modnum] = currentsubbankindex;
          }
	  if (plane >= (Int_t) fStripsRaw.size())
	    fStripsRaw.resize(plane);
	  if (stripnum >= (Int_t) fStripsRaw[plane-1].size())
	    fStripsRaw[plane-1].push_back(0);
	  // plane goes from 1 - 4 instead of 0 - 3,

	  if (plane >= (Int_t) fStrips.size())
	    fStrips.resize(plane);
	  if (stripnum >= (Int_t) fStrips[plane-1].size())
            fStrips[plane-1].push_back(0);
        }
	
	else if (dettype == "escalers") {
          // Register data channel type
          fMapping[currentsubbankindex] = kV1495Scaler;
          fdettype = 2;
 	  if (fdettype >= (Int_t) fSubbankIndex.size())
	    fSubbankIndex.resize(fdettype+1);
          if (modnum >= fSubbankIndex[fdettype].size()) {
            fSubbankIndex[fdettype].push_back(modnum);
            fSubbankIndex[fdettype][modnum] = currentsubbankindex;
          }
	  if (plane >= (Int_t) fStripsRawScal.size())
	    fStripsRawScal.resize(plane);
	  if (stripnum >= (Int_t) fStripsRawScal[plane-1].size())
	    fStripsRawScal[plane-1].push_back(0);
	}

	else if (dettype == "esingle") {
          // Register data channel type
          fMapping[currentsubbankindex] = kV1495Single;
          fdettype = 0;
	  if (fdettype >= (Int_t) fSubbankIndex.size())
	    fSubbankIndex.resize(fdettype+1);
          if (modnum >= fSubbankIndex[fdettype].size()) {
            fSubbankIndex[fdettype].push_back(modnum);
            fSubbankIndex[fdettype][modnum] = currentsubbankindex;
          }
	  if (plane >= (Int_t) fStripsRawEv.size())
	    fStripsRawEv.resize(plane);
	  if (stripnum >= (Int_t) fStripsRawEv[plane-1].size())
	    fStripsRawEv[plane-1].push_back(0);

	  if (plane >= (Int_t) fStripsEv.size())
	    fStripsEv.resize(plane);
	  if (stripnum >= (Int_t) fStripsEv[plane-1].size())
            fStripsEv[plane-1].push_back(0);
	} // end of switch (dettype)

      } else if (modtype == "SIS3801D24") {
        // Register data channel type
        fMapping[currentsubbankindex] = kScaler;
        // Add to mapping
        if (modnum >= fScaler_Mapping[currentsubbankindex].size())
          fScaler_Mapping[currentsubbankindex].resize(modnum+1);
        if (channum >= fScaler_Mapping[currentsubbankindex].at(modnum).size())
          fScaler_Mapping[currentsubbankindex].at(modnum).resize(channum+1,-1);
        // Add scaler channel
        if (fScaler_Mapping[currentsubbankindex].at(modnum).at(channum) < 0) {
          QwMessage << "Registering SIS3801D24 " << name
                    << std::hex
                    << " in ROC 0x" << currentrocread << ", bank 0x" << currentbankread
                    << std::dec
                    << " at mod " << modnum << ", chan " << channum
                    << QwLog::endl;
          UInt_t index = fScaler.size();
          fScaler_Mapping[currentsubbankindex].at(modnum).at(channum) = index;
          fScaler.push_back(QwSIS3801D24_Channel(name));
        }
      } // end of switch (modtype)
    } // end of if for token line
  } // end of while over parameter file
  
  return 0;
}

//*****************************************************************
Int_t QwComptonElectronDetector::LoadEventCuts(TString & filename)
{
  return 0;
}

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

//Boot CheckForEndOfBurst()
//{
//  Test the value of the laser power thing and retun true if it has changed
//  return false otherwise.
//};


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
  div_t div_output;
  //  UInt_t accum_count = 0;
  ports_read = 0;
  // Get the subbank index (or -1 when no match)
  Int_t subbank = GetSubbankIndex(roc_id, bank_id);

  if (subbank >= 0 && num_words > 0) {

    //  We want to process this ROC.  Begin looping through the data.
    switch (fMapping[subbank]) {
    case kScaler:     // Scalers
      {
        // Read header word
        words_read++;
        // TODO Multiscaler functionality
        // Read scalers
        for (size_t modnum = 0; modnum < fScaler_Mapping[subbank].size(); modnum++) {
          for (size_t channum = 0; channum < fScaler_Mapping[subbank].at(modnum).size(); channum++) {
            Int_t index = fScaler_Mapping[subbank].at(modnum).at(channum);
            if (index >= 0) {
              words_read += fScaler[index].ProcessEvBuffer(&(buffer[words_read]), num_words - words_read);
            }
          }
        }
        words_read = num_words;
        break;
      }

      // V1495
    case kV1495Accum:
      {
	for (Int_t k = 0; k < NModules; k++) {
	  if (fSubbankIndex[1][k]==subbank) {
	    // sub-bank 0x0204, accum mode data from strips 0-31 of planes 1 thru 4
	    if (num_words > 0) { //!?this has already been checked in this function. remove this if?

	      for (Int_t i = 0; i < StripsPerModule; i++) { // loop all words in bank
		Int_t j = k*StripsPerModule+i;
		/*		accum_count = (buffer[i] & 0xff000000) >> 24;
		  if (accum_count != 255)fStripsRaw[0][j] = accum_count;
		  accum_count = (buffer[i] & 0x00ff0000) >> 16;
		  if (accum_count != 255)fStripsRaw[1][j] = accum_count;
		  accum_count = (buffer[i] & 0x0000ff00) >> 8;
		  if (accum_count != 255)fStripsRaw[2][j] = accum_count;
		  accum_count = (buffer[i] & 0x000000ff);
		  if (accum_count != 255)fStripsRaw[3][j] = accum_count;
		*/
		fStripsRaw[0][j] = (buffer[i] & 0xff000000) >> 24;
		fStripsRaw[1][j] = (buffer[i] & 0x00ff0000) >> 16;
		fStripsRaw[2][j] = (buffer[i] & 0x0000ff00) >> 8;
		fStripsRaw[3][j] = (buffer[i] & 0x000000ff);
		words_read++;
	      }
	    }
	    if (num_words != words_read) {
	      QwError << "QwComptonElectronDetector: There were "
		      << num_words - words_read
		      << " leftover words after decoding everything we recognize in accum data"
		      << std::hex
		      << " in ROC " << roc_id << ", bank " << bank_id << "."
		      << std::dec
		      << QwLog::endl;
	    }
	  }
	} // for (Int_t k = 0; k < NModules; k++) 
	break;
      }
      
    case kV1495Single:
      {
	//	if (fScaler[13].GetValue()>50) {
	for (Int_t jj = 0; jj < NModules; jj++) {
	  if (fSubbankIndex[0][jj]==subbank) {
	    if (num_words > 0) {
	      div_output = div(num_words,numberofWordPerEv);
	      Int_t numbOfEvDataSet = div_output.quot;

	      Int_t evHitMap[numbOfEvDataSet][NPlanes][StripsPerPlane];

	      for (Int_t m = 0; m < numbOfEvDataSet; m++) {
		for (Int_t i = 0; i <NPlanes; i++) {
		  for (Int_t j = 0; j < StripsPerPlane; j++) {
		    evHitMap[m][i][j] = 0;
		  }
		}
	      }

	      if (div_output.rem) { 
		printf("\n\n***Error***:Event Data suspicious. Report to an expert:\n");
		printf(",found %d number of words which is not a multiple of 5!\n\n",num_words);
	      }///This should not happen until the event word readout format is changed in DAQ

	      for (Int_t m = 0; m < numbOfEvDataSet; m++) {
		for (Int_t i = 0; i < NPlanes; i++) {
		  Int_t bufferIndex = m*(NPlanes+1) + i;
		  for (Int_t j = 0; j < StripsPerModule; j++) {
		    Int_t k = jj*StripsPerModule + j; 
		    bitwise_mask = (0x1 << j);
		    evHitMap[m][i][k] = (buffer[bufferIndex] & bitwise_mask) >> j;
		  }
		  words_read++;
		}
		//Int_t ExtraWord = buffer[NPlanes*(m+1)];//diagnostic word, ignore warning, unused currently
		words_read++;
	      }
	      for (Int_t k = 0; k < StripsPerPlane; k++) {
		for (Int_t i = 0; i < NPlanes; i++) {
		  for (Int_t m = 0; m < numbOfEvDataSet; m++) {
		    fStripsRawEv[i][k] += evHitMap[m][i][k]; 
		  }
		}
	      }
	      
	      if (num_words != words_read) {
		QwError << "QwComptonElectronDetector: There were "
			<< num_words - words_read
			<< " leftover words after decoding everything recognizable in event data."
			<< QwLog::endl;
	      }
	    }
	  }
	}
	//	}
	break;
      }///end of case V1495Singles
      
    case kV1495Scaler:
      {
	for (Int_t k = 0; k < NModules; k++) {
	  if (fSubbankIndex[2][k]==subbank) {
	    // sub-bank 0x020A, V1495 SCALER data from strips 0-31 of planes 1 thru 4
	    if (num_words > 0) {		
	      //  We want to process this ROC.  Begin looping through the data.
	      for (Int_t i = 0; i < StripsPerModule; i++) { // loop all words in bank
		Int_t j = k*StripsPerModule + i;
		fStripsRawScal[0][j] = (buffer[i] & 0xff000000) >> 24;
		fStripsRawScal[1][j] = (buffer[i] & 0x00ff0000) >> 16;
		fStripsRawScal[2][j] = (buffer[i] & 0x0000ff00) >> 8;
		fStripsRawScal[3][j] = (buffer[i] & 0x000000ff);
		words_read++;
		//if (fStripsRawScal[0][j] > 0) printf("\nfStripsRawScal[%d][%d]:%f",i,j,fStripsRawScal[i][j]);
	      }
	    }
	    if (num_words != words_read) {
	      QwError << "QwComptonElectronDetector: There were "
		      << num_words - words_read
		      << " leftover words after decoding everything we recognize in v1495 scaler data"
		      << std::hex
		      << " in ROC " << roc_id << ", bank " << bank_id << "."
		      << std::dec
		      << QwLog::endl;
	    }	      
	  } // if (fSubbankIndex[1][k]==subbank) 
	} // for (Int_t k = 0; k < NModules; k++)  
	break;
      }
     
    case kUnknown: // Unknown data channel type
    default:
      {
	QwError << "QwComptonElectronDetector: Unknown data channel type for ROC " 
		<< roc_id << ", bank " << bank_id << QwLog::endl;
	break;
      }
    }
  }
  return words_read;
}

/*** Process the single event cuts * @return */
//*****************************************************************
Bool_t QwComptonElectronDetector::SingleEventCuts()
{
  return IsGoodEvent();
}

/*** Process this event */
//*****************************************************************
void  QwComptonElectronDetector::ProcessEvent()
{
  
  Int_t fCalibrationFactor = 1;
  Int_t fOffset = 0;
  int pas1 =0;
  int pas2 =0;
  int pas3 =0;
  int trig=0;
  int eve;
  int track=0;
  ofstream gui;
  vector<int> det0, det1, det2, det3;
  vector<int> det0eff, det1eff, det2eff, det3eff;
  double bestfita=1000000;
  int besttrack;
  int bestplane1=1000000;
  int bestplane2=1000000;
  int bestplane3=1000000;
  int besty1=1000000;
  int besty2=1000000 ;
  int besty3=1000000; 
  double edet_tr_angle=1000000; 
  
  int use_gui=0;
  int test_print=0;
  int print_fit=0;

  // EVENT MODE = 0
  // ACUMM MODE = 1
  int runmode = 1;
   
  //    QwOut << "Scalers = " << fScaler[3] << QwLog::endl;
    
  //    QwOut << "Scalers = " << QwSIS3801D24_Channel() << QwLog::endl; 
    
  myrun = this->GetParent()->GetCodaRunNumber();
  eve=this->GetParent()->GetCodaEventNumber();
  //   QwOut << "Run End Time: = " << GetEndTime() << QwLog::endl;

 
  if(use_gui==1) { 
    gui.open ("data.detector");    
    gui << eve << endl;
    gui << "1" << endl;
  }
   
  if(test_print==1) QwOut << "------------------------- " << QwLog::endl;

  for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
      fStrips[i][j] = (fStripsRaw[i][j] - fOffset)*fCalibrationFactor;
      fStripsEv[i][j] = (fStripsRawEv[i][j] - fOffset)*fCalibrationFactor;
  
      if(test_print==1) {
	if(fStripsEv[i][j]>0) {
	  QwOut << i << " " << j << QwLog::endl;
	}
      }

      //no 0 Detector , YET


      // for efficiency study only start
      //      if(i==1 && fStripsEv[1][j] > 0) det1eff.push_back(j); 
      //      if(i==2 && fStripsEv[2][j] > 0) det2eff.push_back(j); 
      //      if(i==3 && fStripsEv[3][j] > 0) det3eff.push_back(j); 
      // for efficiency study only stop


      //////////////////
      // 1st Detector //
      //////////////////
  
      if(i==1 && fStripsEv[1][j] > 0) {
        if(use_gui==1 && pas1==0) gui << i+1 ;
        if(pas1==0) trig++;
	//	det1.push_back(j-2);
	det1.push_back(j);
        if(use_gui==1) gui << " " << j+1-(0) ;
        pas1=1;
      } 
         
      //////////////////
      // 2nd Detector //
      //////////////////

      if(i==2 && pas1==1 && fStripsEv[2][j] > 0) {
        if(use_gui==1 && pas2==0) gui << " " << endl;
        if(use_gui==1 && pas2==0) gui << i+1 ;
        if(pas2==0) trig++;
	//	det2.push_back(j-2);
	det2.push_back(j);
        if(use_gui==1) gui << " " << j+1-(0) ;
	pas2=1;
      }	 

      //////////////////     
      // 3rd Detector //
      //////////////////

      if(i==3 && pas1==1 && pas2==1 && fStripsEv[3][j] > 0) {
        if(use_gui==1 && pas3==0) gui << " " << endl;
        if(use_gui==1 && pas3==0) gui << i+1 ;
        if(pas3==0) trig++;
	det3.push_back(j);
        if(use_gui==1) gui << " " << j+1 ;
	pas3=1;
      }	 
       
    }
  }

  ////////////////////////////////////////
  // Creating Tracks and X2 Calculation //
  ////////////////////////////////////////

  if(trig != 3 && runmode == 0) {
    QwOut << "Event = " << eve << " Does not Have 3/3 Hits " << QwLog::endl;
    QwOut << det1.size() << " " << det2.size() << " " << det3.size() << QwLog::endl;
    if(det1.size() > 0) QwOut << det1[0] << QwLog::endl;
    if(det2.size() > 0) QwOut << det2[0] << QwLog::endl;
    if(det3.size() > 0) QwOut << det3[0] << QwLog::endl;
  }
 
  if(trig==3 && runmode == 0) {
  
    for(size_t i=0; i<det1.size(); i++)
      {
	for(size_t j=0; j<det2.size(); j++)
	  {
	    for(size_t k=0; k<det3.size(); k++)
	      {
  	  
		track++;
		if(print_fit==1) QwOut << det1[i] << " " << det2[j] << " " << det3[k] << QwLog::endl;      
	  
		double s1 = 0;
		double sx = 0;
		double sxx = 0;
		double sy = 0;
		double sxy = 0;
		double syy = 0;
		double x[3] = {690,710,730};
		int d[3] = {det1[i],det2[j],det3[k]};
		double error = 2;          
 	   	    	  
		for(Int_t t=0; t<3; t++)
		  {
		    s1 += 1/(error*error);
		    sx += x[t]/(error*error);
		    sxx += pow(x[t]/error, 2);
		    sy += (60+4*d[t])/pow(error,2);
		    sxy += x[t]*(60+4*d[t])/pow(error,2);
		    syy += pow((60+4*d[t])/error,2);
		    //->	       sy += (d[t])/pow(error,2);
		    //->	       sxy += x[t]*d[t]/pow(error,2);
		    //->	       syy += pow((d[t])/error,2);
		  }

		double ddd = s1*sxx - sx*sx;
		double a = (1/ddd)*(sxx*sy - sx*sxy);
		double b = (1/ddd)*(s1*sxy - sx*sy);

		double fita = 0;
	        for(Int_t t=0; t<3; t++)
		  {
		    double y = 60+4*d[t];
		    //->		  double y = d[t];
		    double fitaCorrd = a + x[t]*b;
		    fita += pow((y - fitaCorrd)/error, 2);		
		  }
	       
	        if(fita<bestfita)
		  {
		    bestfita = fita;
		    besttrack = track;
		    bestplane1 = d[0];
		    bestplane2 = d[1];
		    bestplane3 = d[2];
		    besty1 = 60+4*d[0];
		    besty2 = 60+4*d[1];
		    besty3 = 60+4*d[2];
		  }

	      }
	  }
      }

       
    edet_tr_angle=atan((bestplane1-bestplane3)*200*0.0001/2);
    edet_angle=edet_tr_angle*180/3.141592;           
       
    edet_TotalNumberTracks=track;
    edet_x2=bestfita;
     
    if(print_fit==1) {
      QwOut << " Best Chi2 = " << bestfita << " and Best Track is = " << besttrack << QwLog::endl;  
      QwOut << " Best Hit in Plane1 = " << bestplane1 << " with Coord = " << besty1 << QwLog::endl; 
      QwOut << " Best Hit in Plane2 = " << bestplane2 << " with Coord = " << besty2 << QwLog::endl; 
      QwOut << " Best Hit in Plane3 = " << bestplane3 << " with Coord = " << besty3 << QwLog::endl; 
      QwOut << " Track Angle is     = " << edet_angle << QwLog::endl; 
    }
	 
    fStripsEvBest1=bestplane1;
    fStripsEvBest2=bestplane2;
    fStripsEvBest3=bestplane3;


    //////////////////////////
    /// Offset Corrections ///
    //////////////////////////

    if(edet_angle > 1) {
      bestplane2=bestplane2-2;
      bestplane1=bestplane1-2;      
    }

    if(edet_angle < 1) {
      bestplane2=bestplane2-2;
      bestplane1=bestplane1-1;      
    }
      

    //       if(bestplane2 != bestplane3) bestplane2=bestplane2+1;

    ////////////////////////////////
    /// Re-Calculation of the X2 ///
    ////////////////////////////////

    double s1 = 0;
    double sx = 0;
    double sxx = 0;
    double sy = 0;
    double sxy = 0;
    double syy = 0;
    double x[3] = {690,710,730};
    double error = 2;          
    int dc[3] = {bestplane1,bestplane2,bestplane3};

    for(Int_t t=0; t<3; t++)
      {
	s1 += 1/(error*error);
	sx += x[t]/(error*error);
	sxx += pow(x[t]/error, 2);
	sy += (60+4*dc[t])/pow(error,2);
	sxy += x[t]*(60+4*dc[t])/pow(error,2);
	syy += pow((60+4*dc[t])/error,2);
      }

    double ddd = s1*sxx - sx*sx;
    double a = (1/ddd)*(sxx*sy - sx*sxy);
    double b = (1/ddd)*(s1*sxy - sx*sy);

    double fita = 0;
    for(Int_t t=0; t<3; t++)
      {
	double y = 60+4*dc[t];
	double fitaCorrd = a + x[t]*b;
	fita += pow((y - fitaCorrd)/error, 2);		 
      }
 
    edet_x2=fita;
         
    ///////////////////////////////////
    /// Re-Calculation of the Angle ///
    ///////////////////////////////////

    fStripsEvBest1=bestplane1;
    fStripsEvBest2=bestplane2;
    fStripsEvBest3=bestplane3;
       
    edet_angle=atan((bestplane1-bestplane3)*200*0.0001/2);
    edet_angle=edet_angle*180/3.141592; 
       
       
 
    if(bestfita < 10 && edet_TotalNumberTracks < 5) {
      //         QwOut << eve << " " << fStripsEvBest1 << " " << fStripsEvBest2 << "  " << fStripsEvBest3 << " " << edet_x2 << " " << edet_angle << QwLog::endl;
    }
 
  }

  //////////////////
  ///  eff start ///
  //////////////////

  ///////////////////////
  // * PLANE 1 START * //
  ///////////////////////
  // 
  //      if(det2eff.size()==1 && det3eff.size() ==1 && det1eff.size() < 2 ) {
  //       det2eff[0]=det2eff[0]-2;
  // 	
  // 	if(det1eff.size() == 0) { 
  //       QwOut << eve << " 1 " << " 100 " << det2eff[0] << " " << det3eff[0] << QwLog::endl;
  // 	} 
  // 	
  // 	if(det1eff.size() == 1 ) {
  // 	 edet_angle=atan((det1eff[0]-det3eff[0])*200*0.0001/2);
  // 	 edet_angle=edet_angle*180/3.141592; 
  // 	   
  // 	 if(edet_angle > 1) {
  // 	  det1eff[0]=det1eff[0]-2;	
  // 	   }
  //
  // 	   if(edet_angle < 1) {
  // 	  det1eff[0]=det1eff[0]-1;     
  // 	   }
  //       QwOut << eve << " 1 " << det1eff[0] << " " << det2eff[0] << " " << det3eff[0] << QwLog::endl;
  // 	}	
  //      }
  //
  ///////////////////////
  // * PLANE 2 START * //
  ///////////////////////
  // 
  // 	if(det1eff.size()==1 && det3eff.size() ==1 && det2eff.size() < 2 ) {
  // 	
  //       edet_angle=atan((det1eff[0]-det3eff[0])*200*0.0001/2);
  //       edet_angle=edet_angle*180/3.141592; 
  // 	 
  // 	if(edet_angle > 1) {
  // 	 det1eff[0]=det1eff[0]-2;     
  // 	}
  //
  // 	if(edet_angle < 1) {
  // 	 det1eff[0]=det1eff[0]-1;     
  // 	}      
  //	
  // 	if(det2eff.size() == 0) { 
  // 	   QwOut << eve << " 2 " << det1eff[0] << " 100 " << det3eff[0] << QwLog::endl;
  // 	} 
  // 	
  // 	if(det2eff.size() == 1 ) { 
  // 	 det2eff[0]=det2eff[0]-2;	
  // 	   QwOut << eve << " 2 " << det1eff[0] << " " << det2eff[0] << " " << det3eff[0] << QwLog::endl;
  // 	}	
  // 	}
  //
  ///////////////////////
  // * PLANE 3 START * //
  ///////////////////////
  //  
  //       if(det1eff.size()==1 && det2eff.size()==1 && det3eff.size() < 2 ) {
  //       
  // 	det2eff[0]=det2eff[0]-2;
  //       
  // 	edet_angle=atan((det1eff[0]-det2eff[0])*200*0.0001/2);
  // 	edet_angle=edet_angle*180/3.141592; 
  //
  //       if(edet_angle > 1) {
  // 	det1eff[0]=det1eff[0]-2;     
  //       }
  //
  //       if(edet_angle < 1) {
  // 	det1eff[0]=det1eff[0]-1;     
  //       }      
  //
  //       
  //       if(det3eff.size() == 0) { 
  // 	QwOut << eve << " 3 " << det1eff[0] << " " << det2eff[0] << " 100 " << QwLog::endl;
  //       } 
  //       
  //       
  //       if(det3eff.size() == 1 ) { 
  // 	QwOut << eve << " 3 " << det1eff[0] << " " << det2eff[0] << " " << det3eff[0] << QwLog::endl;
  //       }       
  //
  //       }
  //
  //
  ////////////////
  /// eff stop ///
  ////////////////

  /////////////   
  // For GUI //
  /////////////

  //QwOut << eve << " " << bestplane1 << " " << bestplane2 << " " << bestplane3 << " " << bestfita << " " << track << " " << edet_angle << QwLog::endl;
   
  if(use_gui==1) {
    gui.close();    
    QwOut << " Number of Planes Fired = " << trig << " Total Number of Tracks = " << track << QwLog::endl;
    if(trig==3) {
      QwOut << " Continue ? " << QwLog::endl;
      getchar();
    } 
  }
    
  return;
}

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
}

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
}

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
      fStripsRawScal[i][j] = 0;
    }
  }
  
  fStripsEvBest1 = 1000000;
  fStripsEvBest2 = 1000000;
  fStripsEvBest3 = 1000000;
  edet_x2 = 1000000; 
  edet_TotalNumberTracks = 1000000;
  edet_angle = 1000000;
   
  fGoodEventCount = 0;
   
  // Clear all scaler channels
  for (size_t i = 0; i < fScaler.size(); i++)
    fScaler[i].ClearEventData();

  return;
}

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

    for (size_t i = 0; i < fScaler.size(); i++)
      this->fScaler[i] = input->fScaler[i];
  }
  return *this;
}

VQwSubsystem&  QwComptonElectronDetector::operator+=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
    for (Int_t i = 0; i < NPlanes; i++){
      for (Int_t j = 0; j < StripsPerPlane; j++){

	// static double edet_acum_sum[4][96];
	//	    edet_acum_sum[i][j]=fStripsRaw[i][j]+edet_acum_sum[i][j];
	//	    QwOut << " TEST2 =  "  << " i = " << i << " j = " << j << " " << fStripsRaw[i][j] << " " << edet_acum_sum[i][j] << QwLog::endl;	  
	//            fStripsRaw[i][j]=0;
	    
	    
	    
        this->fStripsRaw[i][j] += input->fStripsRaw[i][j];     
      
      }
    }

    for (size_t i = 0; i < fScaler.size(); i++)
      this->fScaler[i] += input->fScaler[i];
  }
  return *this;
}

VQwSubsystem&  QwComptonElectronDetector::operator-=  (VQwSubsystem *value)
{
  if (Compare(value)) {
    QwComptonElectronDetector* input = dynamic_cast<QwComptonElectronDetector*> (value);
    for (Int_t i = 0; i < NPlanes; i++){
      for (Int_t j = 0; j < StripsPerPlane; j++){
        this->fStripsRaw[i][j] -= input->fStripsRaw[i][j];
      }
    }

    for (size_t i = 0; i < fScaler.size(); i++)
      this->fScaler[i] -= input->fScaler[i];
  }
  return *this;
}

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
}


void  QwComptonElectronDetector::Sum(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2)) {
    *this  = value1;
    *this += value2;
  }
}

void  QwComptonElectronDetector::Difference(VQwSubsystem  *value1, VQwSubsystem  *value2)
{
  if (Compare(value1) && Compare(value2)) {
    *this  = value1;
    *this -= value2;
  }
}


void QwComptonElectronDetector::Ratio(VQwSubsystem *numer, VQwSubsystem *denom)
{
  if (Compare(numer) && Compare(denom)) {
    QwComptonElectronDetector* innumer = dynamic_cast<QwComptonElectronDetector*> (numer);
    QwComptonElectronDetector* indenom = dynamic_cast<QwComptonElectronDetector*> (denom);

    for (Int_t i = 0; i < NPlanes; i++){
      for (Int_t j = 0; j < StripsPerPlane; j++){        
	if(indenom->fStripsRaw[i][j] > 0) {
          fStripsRaw[i][j] =(innumer->fStripsRaw[i][j]/indenom->fStripsRaw[i][j]);
	}	  
	else {
	  this->fStripsRaw[i][j]=0;
	}	     
      }
    }

    for (size_t i = 0; i < fScaler.size(); i++)
      this->fScaler[i].Ratio(innumer->fScaler[i],indenom->fScaler[i]);
    //   PrintValue();
  }  
  
}

void QwComptonElectronDetector::Scale(Double_t factor)
{
  for (Int_t i = 0; i < NPlanes; i++){
    for (Int_t j = 0; j < StripsPerPlane; j++){
      this->fStripsRaw[i][j] *= factor;//!!converting Double to Int (ok now) may not be okay later!
    }
  }

  for (size_t i = 0; i < fScaler.size(); i++)
    this->fScaler[i].Scale(factor);
}

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
    if (input->fScaler.size() != fScaler.size()) {
      result = kFALSE;
    }
  }
  return result;
}

void  QwComptonElectronDetector::AccumulateRunningSum(VQwSubsystem* value)
{
  if (Compare(value)) {
    // Optional event selection...
    fGoodEventCount++;
    *this  += value;
    //    QwOut << " 1 "  << QwLog::endl;
  }
}

void  QwComptonElectronDetector::CalculateRunningAverage()
{
  if (fGoodEventCount <= 0) {
    Scale(0);
    QwOut << " Scale = 0" << QwLog::endl;
  } else {
    Scale(1.0/fGoodEventCount);
    QwOut << " Good Events =  "  << " " << fGoodEventCount << QwLog::endl;

  }
}


//*****************************************************************
/**
 * Construct the histograms
 * @param folder Folder in which the histograms will be created
 * @param prefix Prefix with information about the type of histogram
 */
void  QwComptonElectronDetector::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();

  //  Go into subdirectory if it exists
  TDirectory* eDetfolder = folder->GetDirectory("Compton_Electron");
  if (eDetfolder == 0) eDetfolder = folder->mkdir("Compton_Electron");

  //  Now create the histograms.
  TString basename = prefix + GetSubsystemName();

  eDetfolder->cd();
  for (Int_t i=0; i<NPlanes; i++){
    TString histname = Form("Compton_eDet_Accum_Raw_Plane%d",i+1);
    fHistograms1D.push_back(gQwHists.Construct1DHist(prefix+histname));//1st histogram
    histname = Form("Compton_eDet_Accum_Plane%d",i+1);
    fHistograms1D.push_back(gQwHists.Construct1DHist(prefix+histname));
    histname = Form("Compton_eDet_Evt_Raw_Plane%d",i+1);
    fHistograms1D.push_back(gQwHists.Construct1DHist(prefix+histname));//3rd histogram
    histname = Form("Compton_eDet_Evt_Plane%d",i+1);
    fHistograms1D.push_back(gQwHists.Construct1DHist(prefix+histname));
    histname = Form("Compton_eDet_Scal_Raw_Plane%d",i+1);
    fHistograms1D.push_back(gQwHists.Construct1DHist(prefix+histname));//5th histogram
  }

  TString histname = Form("Compton_eDet_Evt_Best_Plane1");
  fHistograms1D.push_back(gQwHists.Construct1DHist(histname));
  histname = Form("Compton_eDet_Evt_Best_Plane2");
  fHistograms1D.push_back(gQwHists.Construct1DHist(histname));
  histname = Form("Compton_eDet_Evt_Best_Plane3");
  fHistograms1D.push_back(gQwHists.Construct1DHist(histname));
  histname = Form("Compton_eDet_Evt_Best_x2");
  fHistograms1D.push_back(gQwHists.Construct1DHist(histname));
  histname = Form("Compton_eDet_Evt_NTracks");
  fHistograms1D.push_back(gQwHists.Construct1DHist(histname));
  histname = Form("Compton_eDet_Evt_Track_Angle");
  fHistograms1D.push_back(gQwHists.Construct1DHist(histname));
 
  for (size_t i = 0; i < fScaler.size(); i++)
    fScaler[i].ConstructHistograms(folder,prefix);

  return;
}

void  QwComptonElectronDetector::DeleteHistograms()
{
  for (size_t i=0; i<fHistograms1D.size(); i++) {
    if (fHistograms1D.at(i) != NULL) {
      delete fHistograms1D.at(i);
      fHistograms1D.at(i) =  NULL;
    }
  }

  for (size_t i = 0; i < fScaler.size(); i++)
    fScaler[i].DeleteHistograms();
}

void  QwComptonElectronDetector::FillHistograms()
{
  //  edet_cut_on_x2=7;
  //  edet_cut_on_ntracks=2;
  edet_cut_on_x2=7;
  edet_cut_on_ntracks=5;
  
  for (Int_t i = 0; i < NPlanes; i++) {
    Int_t ii = 5*i;
    //apology for the hardcoding; had to do so in keeping with DaveG's philosophy of least change.
    //the 5 corresponds to the 5 histograms that are being declared above in the 'for' loop.
    ///The order in which the histograms are filled below should be the same in which they were created above
    for (Int_t j = 0; j < StripsPerPlane; j++) {
      Int_t nthStrip = j + 1;

      if (fHistograms1D[ii] != NULL) {
  	for (Int_t n=0; n<fStripsRaw[i][j]; n++) ///Accumulation raw data
	  fHistograms1D[ii]->Fill(nthStrip);	//!!Why does any of the following 2 method not work?
      }	//      fHistograms1D[4*i]->Fill(j+1,fStripsRaw[i][j]); 
	//      fHistograms1D[4*i]->SetBinContent(j+1,fStripsRaw[i][j]);
      if (fHistograms1D[ii+1] != NULL) {
	for (Int_t n=0; n<fStrips[i][j]; n++) ///Accumulation data
	  fHistograms1D[ii+1]->Fill(nthStrip);
      }

      if (fHistograms1D[ii+2] != NULL) {
	for (Int_t n = 0; n < fStripsRawEv[i][j]; n++) ///Event raw data
	  fHistograms1D[ii+2]->Fill(nthStrip);
      }

      if (fHistograms1D[ii+3] != NULL) {
	for (Int_t n=0; n<fStripsEv[i][j]; n++) ///Event data
	  fHistograms1D[ii+3]->Fill(nthStrip);
      }
      
      if (fHistograms1D[ii+4] != NULL) {
  	for (Int_t n=0; n<fStripsRawScal[i][j]; n++){ ///v1495 Scaler raw data
	  fHistograms1D[ii+4]->Fill(nthStrip);
	}
      }
    }
  }

  if(edet_x2 < edet_cut_on_x2 && edet_TotalNumberTracks < edet_cut_on_ntracks && fStripsEvBest1 < 100  && fStripsEvBest2 < 100  && fStripsEvBest3 < 100 ) {    
    //  if(edet_x2 == edet_cut_on_x2 && edet_TotalNumberTracks < edet_cut_on_ntracks && fStripsEvBest1 < 100  && fStripsEvBest2 < 100  && fStripsEvBest3 < 100 ) {    
    //Incrementing the following histograms based on the above increase in the 1D histos used
    fHistograms1D[20]->Fill(fStripsEvBest1); // fHistograms1D[16]->Fill(fStripsEvBest1); 
    fHistograms1D[21]->Fill(fStripsEvBest2); // fHistograms1D[17]->Fill(fStripsEvBest2); 
    fHistograms1D[22]->Fill(fStripsEvBest3); // fHistograms1D[18]->Fill(fStripsEvBest3);
  }  
  fHistograms1D[23]->Fill(edet_x2); // fHistograms1D[19]->Fill(edet_x2); 
  fHistograms1D[24]->Fill(edet_TotalNumberTracks); // fHistograms1D[20]->Fill(edet_TotalNumberTracks); 
  fHistograms1D[25]->Fill(edet_angle); //  fHistograms1D[21]->Fill(edet_angle); 
   
  for (size_t i = 0; i < fScaler.size(); i++)
    fScaler[i].FillHistograms();
  
  return;
}

void  QwComptonElectronDetector::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  //  SetHistoTreeSave(prefix);
  fTreeArrayIndex = values.size();

  for (Int_t i = 1; i < NPlanes; i++) {
    //!!iteration of this loop from '1' instead of '0' is for Qweak run-1 when the first plane was inactive
    /// Note that currently the plane number in these tree-leafs correspond to the C++ counting(from 0)
    /// Event branch for this plane
    TString basename = prefix + Form("p%dRawEv",i+1);
    TString valnames = "";
    Double_t* valstart = &(values.back());
    for (Int_t j = 0; j < StripsPerPlane; j++) {
      valnames += TString(":") + prefix + Form("p%ds%dRawEv",i+1,j+1) + "/D";
      values.push_back(0.0);
    }
    valnames.Remove(0,1); // remove first ':' character
    tree->Branch(basename, valstart+1, valnames);

    /// Accumulator branch for this plane
    basename = prefix + Form("p%dRawAc",i+1);
    valnames = "";
    valstart = &(values.back());
    for (Int_t j = 0; j < StripsPerPlane; j++) {
      valnames += TString(":") + prefix + Form("p%ds%dRawAc",i+1,j+1) + "/D";
      values.push_back(0.0);
    }
    valnames.Remove(0,1); // remove first ':' character
    tree->Branch(basename, valstart+1, valnames);

    /// v1495 scaler branch for this plane
    basename = prefix + Form("p%dRawV1495Scaler",i+1);
    valnames = "";
    valstart = &(values.back());
    for (Int_t j = 0; j < StripsPerPlane; j++) {
      valnames += TString(":") + prefix + Form("p%ds%dRawV1495Scaler",i+1,j+1) + "/D";
      values.push_back(0.0);
    }
    valnames.Remove(0,1); // remove first ':' character
    tree->Branch(basename, valstart+1, valnames);
  }
  ///Notice-2: the pattern that for every plane the branch is created for all 3 datatyes of data before 
  ///...creating the same for the next plane

  for (size_t i = 0; i < fScaler.size(); i++)
    fScaler[i].ConstructBranchAndVector(tree, prefix, values);
  return;
}

void  QwComptonElectronDetector::FillTreeVector(std::vector<Double_t> &values) const
{
  ///The 'values' needs to be a Double_t due to the inner structure in the hierarchy 
  size_t index = fTreeArrayIndex;

  ///The 'values' should be filled in exactly the same order in which they were created above
  /// ...below we follow the pattern as in Notice-2
    for (Int_t i = 1; i < NPlanes; i++) {
      for (Int_t j = 0; j < StripsPerPlane; j++)
	values[index++] = fStripsRawEv[i][j];/// Event Raw
      for (Int_t j = 0; j < StripsPerPlane; j++)
	values[index++] = fStripsRaw[i][j];/// Accum Raw
      for (Int_t j = 0; j < StripsPerPlane; j++)
	values[index++] = fStripsRawScal[i][j];/// v1495 Scaler Raw
    }

  for (size_t i = 0; i < fScaler.size(); i++)
    fScaler[i].FillTreeVector(values);

  return;
}

/**
 * Construct the tree
 * @param folder Folder in which the tree will be created
 * @param prefix Prefix with information about the type of histogram
 **/
void  QwComptonElectronDetector::ConstructTree(TDirectory *folder, TString &prefix)
{
  TString basename = GetSubsystemName();
  TString vnameh;
  TString vnamet;
  folder->cd();
  fTree = new TTree("ComptonElectron", "Compton Electron Detector");
  fTree->Branch("nevents",&fTree_fNEvents,"nevents/I");
  for (Int_t i=0; i< NPlanes; i++){
    vnameh = Form("Plane%d_Evt",i);
    vnamet = Form("Plane%d_Evt/I",i);
    fComptonElectronVector.push_back(0);
    fTree->Branch(vnameh,&(fComptonElectronVector[i]),vnamet);
  }
  return;
}

/*** Delete the tree */
void  QwComptonElectronDetector::DeleteTree()
{
  delete fTree;
  return;
}

/*** Fill the tree with data */
void  QwComptonElectronDetector::FillTree()
{
  for (Int_t i=0; i< NPlanes; i++)
    fComptonElectronVector[i] = 0;

  fTree_fNEvents = GetNumberOfEvents();
  for (Int_t i=0; i< NPlanes; i++){
    for (Int_t j=0; j<StripsPerPlane; j++){
      if (fStripsEv[i][j] != 0)
	fComptonElectronVector[i] += 1;
    }
  }
  fTree->Fill();

  return;
}


//*****************************************************************
void  QwComptonElectronDetector::PrintValue() const
{
  string path = getenv_safe_string("QW_TMP") + "/" + "edet_asym";
  stringstream ss; ss << myrun;
  string str = ss.str();
  path += "_" + str + ".txt";

  static Int_t edet_count = 0;
  edet_count++;
  
  ofstream myfile;
  if(edet_count == 1) {
    QwOut << " " << QwLog::endl;
    QwOut << " Address =  " << path << QwLog::endl;
    QwOut << "Run Number = " << myrun << " Good Events = " << fGoodEventCount << QwLog::endl;    
    QwOut << " " << QwLog::endl;
    myfile.open (path.c_str());
  } 
  else { 
    myfile.open (path.c_str(),ios::app);
  }
  
  Int_t nchan =0;
  for (Int_t i=0; i<NPlanes; i++)
    nchan += fStripsRaw[i].size();
  QwOut << " there were " << nchan << " strips registered" << QwLog::endl;
  
  for (Int_t j=0; j<StripsPerPlane; j++) {
    myfile <<  j << " " << fGoodEventCount*fStripsRaw[0][j] << " " << fGoodEventCount*fStripsRaw[1][j] << " " <<
      fGoodEventCount*fStripsRaw[2][j] << " " << fGoodEventCount*fStripsRaw[3][j] << endl;
    
    //      QwOut << " " << 3 << " " << j << " " << edet_acum_sum[3][j] << QwLog::endl; 
  }
  
  //   myfile << endl;

  myfile.close();  
  return;
}

//*****************************************************************
/**
 * Make a copy of this electron detector, including all its subcomponents
 * @param source Original version
 */
void  QwComptonElectronDetector::Copy(VQwSubsystem *source)
{
  try {
    if (typeid(*source) == typeid(*this)) {
      VQwSubsystem::Copy(source);
      QwComptonElectronDetector* input =
	dynamic_cast<QwComptonElectronDetector*>(source);
      fStripsRaw.resize(input->NPlanes);
      fStripsRawEv.resize(input->NPlanes);
      fStripsRawScal.resize(input->NPlanes);
      fStrips.resize(input->NPlanes);
      fStripsEv.resize(input->NPlanes);
      for (Int_t i = 0; i < input->NPlanes; i++) {
        fStripsRaw[i].resize(StripsPerPlane);
        fStripsRawEv[i].resize(StripsPerPlane);
        fStripsRawScal[i].resize(StripsPerPlane);
        fStrips[i].resize(StripsPerPlane);
        fStripsEv[i].resize(StripsPerPlane);
        for (Int_t j = 0; j < input->StripsPerPlane; j++) {
          fStripsRaw[i][j] = input->fStripsRaw[i][j];
          fStripsRawEv[i][j] = input->fStripsRawEv[i][j];
          fStripsRawScal[i][j] = input->fStripsRawScal[i][j];
          fStrips[i][j] = input->fStrips[i][j];
          fStripsEv[i][j] = input->fStripsEv[i][j];
        }
      }

      this->fScaler.resize(input->fScaler.size());
      for (size_t i = 0; i < this->fScaler.size(); i++)
        this->fScaler[i].Copy(&(input->fScaler[i]));

    } else {
      TString loc = "Standard exception from QwComptonElectronDetector::Copy = "
	+ source->GetSubsystemName() + " "
	+ this->GetSubsystemName() + " are not of the same type";
      throw std::invalid_argument(loc.Data());
    }

  } catch (std::exception& e) {
    QwError << e.what() << QwLog::endl;
  }

  return;
}

/**
 * Make a copy of this electron detector
 * @return Copy of this electron detector
 */
VQwSubsystem*  QwComptonElectronDetector::Copy()
{
  QwComptonElectronDetector* copy = new QwComptonElectronDetector(this->GetSubsystemName() + " Copy");
  copy->Copy(this);
  return copy;
}
