/**********************************************************\
 * File: QwSciFiDetector.cc                                *
 *                                                         *
 * Author: Jeong Han Lee                                   *
 * Date:  Sunday, January 15 17:22:07 EST 2012             *
\**********************************************************/

#include "QwSciFiDetector.h"
#include "QwParameterFile.h"

// Register this subsystem with the factory
RegisterSubsystemFactory(QwSciFiDetector);


QwSciFiDetector::QwSciFiDetector(const TString& name)
: VQwSubsystem(name),VQwSubsystemTracking(name)
{
  ClearAllBankRegistrations();
}

QwSciFiDetector::~QwSciFiDetector()
{
}


Int_t 
QwSciFiDetector::LoadChannelMap( TString mapfile )
{
  
  Bool_t local_debug = false;

  if(local_debug) printf("\n------------- R1 LoadChannelMap %s\n", mapfile.Data());

  TString varname;
  TString varvalue;
  UInt_t  value   = 0;
  UInt_t  chan    = 0;
  UInt_t  number  = 0;
  UInt_t  type    = 0;
   UInt_t DIRMODE = 0;

  Int_t roc=0;

  EQwDetectorPackage package = kPackageNull;
  //  EQwDirectionID   direction = kDirectionNull;


  QwParameterFile mapstr(mapfile.Data());  //Open the file
  fDetectorMaps.insert(mapstr.GetParamFileNameContents());

  while (mapstr.ReadNextLine()){
    
    mapstr.TrimComment('!');   // Remove everything after a '!' character.
    mapstr.TrimWhitespace();   // Get rid of leading and trailing spaces.

    if (mapstr.LineIsEmpty())  continue;

    if (mapstr.HasVariablePair("=",varname,varvalue)){
       //  This is a declaration line.  Decode it.
      varname.ToLower();
      value = QwParameterFile::GetUInt(varvalue);
      if (value ==0){
	value = atol(varvalue.Data());
      }

      //    Int_t value = atoi(varvalue.Data());
      if (varname=="roc") {
	RegisterROCNumber(roc,0);
	DIRMODE=0;
      } 
      else if (varname=="bank"){
	RegisterSubbank(value);
	DIRMODE=0;
      }
      else if (varname=="pkg") {
	package=(EQwDetectorPackage)value;
	DIRMODE=0;
      }
      else if (varname=="slot") {
	//	RegisterSlotNumber(value);
	DIRMODE=0;
      } 
      else if (varname=="scaler"){
	DIRMODE=1;
      }
      printf("DIRMODE %d  varname %s and value %d\n", DIRMODE, varname.Data(), value);
    }
    else {
  
      chan   = mapstr.GetTypedNextToken<Int_t>();
      number = mapstr.GetTypedNextToken<Int_t>();
      type   = mapstr.GetTypedNextToken<Int_t>();
      
      //      printf(" Chan %4d  FiberN %4d, FiberType %d\n", chan, number,type);
      
    } 
  }
  
  if(local_debug) printf("\n------------- R1 LoadChannelMap End%s\n\n", mapfile.Data());
  

  return 0;
};

Int_t 
QwSciFiDetector::LoadInputParameters(TString mapfile)
{
  return 0;
};

Int_t 
QwSciFiDetector::LoadGeometryDefinition(TString mapfile)
{
  return 0;
}

void 
QwSciFiDetector::ClearEventData()
{
  return;
};

Int_t 
QwSciFiDetector::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  return 0;
};

Int_t 
QwSciFiDetector::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words)
{

  return 0;
}

void
QwSciFiDetector::ProcessEvent()
{
  return;
};

void  
QwSciFiDetector::FillListOfHits(QwHitContainer& hitlist)
{
  return;
};

void  
QwSciFiDetector::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  return;
};

void 
QwSciFiDetector::FillHistograms()
{
  return;
};

void  
QwSciFiDetector::DeleteHistograms()
{

  return;
};
