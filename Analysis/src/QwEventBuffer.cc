#include "QwEventBuffer.h"
#include "QwOptions.h"

#include <vector>
#include <glob.h>
#include <TMath.h>

#include "THaCodaFile.h"

#ifdef __CODA_ET
#include "THaEtClient.h"
#endif

const Int_t QwEventBuffer::kRunNotSegmented = -20;
const Int_t QwEventBuffer::kNoNextDataFile  = -30;
const Int_t QwEventBuffer::kFileHandleNotConfigured  = -40;

/// This is the ASCII character array 'NULL', and is used by the
/// DAQ to indicate a known empty buffer.
const UInt_t QwEventBuffer::kNullDataWord = 0x4e554c4c;

/**
 * Defines configuration options for QwEventBuffer class using QwOptions
 * functionality.
 *
 * @param options Options object
 */
void QwEventBuffer::DefineOptions(QwOptions &options)
{
  // Define the execution options
  options.AddDefaultOptions()
    ("online", po::value<bool>()->default_value(false)->zero_tokens(),
     "use online data stream");
  options.AddDefaultOptions()
    ("run,r", po::value<string>()->default_value("0:0"),
     "run range in format #[:#]");
  options.AddDefaultOptions()
    ("chainfiles", po::value<bool>()->default_value(true)->zero_tokens(),
     "chain file segments together, do not analyze them separately");
  options.AddDefaultOptions()
    ("event,e", po::value<string>()->default_value("0:"),
     "event range in format #[:#]");
}

QwEventBuffer::QwEventBuffer():fDataFileStem("QwRun_"),
			       fDataFileExtension("log"),
			       fEvStreamMode(fEvStreamNull),
			       fEvStream(NULL),
			       fCurrentRun(-1),
			       fRunIsSegmented(kFALSE),
			       fPhysicsEventFlag(kFALSE),
			       fEvtNumber(0),
			       fNumPhysicsEvents(0)
{
  fDataDirectory = getenv("QW_DATA");
  if (fDataDirectory.Length() == 0){
    std::cerr << "ERROR:  Can't get the data directory in the QwEventBuffer creator."
	      << std::endl;
  } else if (! fDataDirectory.EndsWith("/")) {
      fDataDirectory.Append("/");
  }
};

void QwEventBuffer::ProcessOptions(QwOptions &options)
{
  fOnline     = options.GetValue<bool>("online");
  if (fOnline){
#ifndef __CODA_ET
    QwError << "Online mode will not work without the CODA libraries!"
	    << QwLog::endl;
    exit(EXIT_FAILURE);
#else
    fETHostname = getenv("HOSTNAME");
    fETSession  = getenv("SESSION");
    if (fETHostname.Length() == 0 || fETSession.Length() == 0) {
      TString tmp = "";
      if (fETHostname == NULL)
	tmp += " \"HOSTNAME\"";
      if (fETSession == NULL){
	if (tmp.Length() > 0) 
	  tmp += " and";
	tmp += " ET \"SESSION\"";
      }
      QwError << "The" << tmp
	      << " variable(s) is(are) not defined in your environment.\n"
	      << "        This is needed to run the online analysis."
	      << QwLog::endl;
      exit(EXIT_FAILURE);
    }
#endif
  }
  fRunRange   = options.GetIntValuePair("run");
  fEventRange = options.GetIntValuePair("event");
  fChainDataFiles = options.GetValue<bool>("chainfiles");
}

TString QwEventBuffer::GetRunLabel() const
{
  TString runlabel = Form("%d",fCurrentRun);
  if (fRunIsSegmented && ! fChainDataFiles){
    runlabel += Form(".%03d",*this_runsegment);
  }
  return runlabel;
};

Int_t QwEventBuffer::OpenNextStream()
{
  Int_t status = CODA_ERROR;
  if (fOnline) {
    /* Modify the call below for your ET system, if needed.
       OpenETStream( ET host name , $SESSION , mode)
       mode=0: wait forever
       mode=1: timeout quickly
    */
    QwMessage << "Try to open the ET station with HOSTNAME==" 
	      << fETHostname
	      << ", SESSION==" << fETSession << "."
	      << QwLog::endl;
    status = OpenETStream(fETHostname, fETSession, 0);

  } else {
    //  Try to open the next data file for the current run.
    if (fCurrentRun != -1 && !fChainDataFiles){
      status = OpenNextSegment();
    }
    if (fCurrentRun == -1){
      fCurrentRun = fRunRange.first - 1;
    }
    while (status != CODA_OK && fCurrentRun<fRunRange.second){
      fCurrentRun++;
      status = OpenDataFile(fCurrentRun);
      if (status == CODA_ERROR){
	//  The data file can't be opened.
	//  Get ready to process the next run.
	QwError << "ERROR:  Unable to find data files for run "
		<< fCurrentRun << ".  Moving to the next run.\n"
		<< QwLog::endl;
      }
    }
  }
  return status;
}

Int_t QwEventBuffer::CloseStream()
{
  QwWarning << "Starting CloseStream."
	    << QwLog::endl;
  Int_t status = kFileHandleNotConfigured;
  if (fEvStreamMode==fEvStreamFile
      && (fRunIsSegmented && !fChainDataFiles) ){
    //  The run is segmented and we are not chaining the 
    //  segments together in the event loop, so close
    //  the current segment.
    status = CloseThisSegment();
  } else if (fEvStreamMode==fEvStreamFile) {
    status = CloseDataFile();
  } else if (fEvStreamMode==fEvStreamFile){
    status = CloseETStream();
  }
  return status;
};



Int_t QwEventBuffer::GetNextEvent()
{
  //  This will return for read errors,
  //  non-physics events, and for physics
  //  events that are within the event range.
  Int_t status = CODA_OK;
  do {
    status = GetEvent();
    if (fEvtNumber > fEventRange.second){
      //  QUESTION:  Should we continue to loop once we've 
      //  reached the maximum event, to allow access to 
      //  non-physics events?
      //  For now, mock up EOF if we've reached the maximum event.
      status = EOF;
    } 
  } while (status==CODA_OK  &&
	   IsPhysicsEvent() &&
	   (fEvtNumber<fEventRange.first
	    || fEvtNumber>fEventRange.second)
	   );
  if (status==CODA_OK  && IsPhysicsEvent()) fNumPhysicsEvents++;
  return status;
};


Int_t QwEventBuffer::GetEvent()
{
  Int_t status = kFileHandleNotConfigured;
  ResetFlags();
  if (fEvStreamMode==fEvStreamFile){
    status = GetFileEvent();
  } else if (fEvStreamMode==fEvStreamET){
    status = GetEtEvent();
  }
  return status;
}

Int_t QwEventBuffer::GetFileEvent(){
  Int_t status = CODA_OK;
  //  Try to get a new event.  If the EOF occurs,
  //  and the run is segmented, try to open the
  //  next segment and read a new event; repeat
  //  if needed.
  do {
    status = fEvStream->codaRead();
    if (fChainDataFiles && status == EOF){
      CloseThisSegment();
      //  Crash out of the loop if we can't open the
      //  next segment!
      if (OpenNextSegment()!=CODA_OK) break;
    }
  } while (fChainDataFiles && status == EOF);
  if (status == CODA_OK){
    DecodeEventIDBank((UInt_t*)(fEvStream->getEvBuffer()));
  }
  return status;
};

Int_t QwEventBuffer::GetEtEvent(){
  Int_t status = CODA_OK;
  //  Do we want to have any loop here to wait for a bad
  //  read to be cleared?
  status = fEvStream->codaRead();
  if (status == CODA_OK){
    DecodeEventIDBank((UInt_t*)(fEvStream->getEvBuffer()));
  }
  return status;
};


Int_t QwEventBuffer::WriteEvent(int* buffer)
{
  Int_t status = kFileHandleNotConfigured;
  ResetFlags();
  if (fEvStreamMode==fEvStreamFile){
    status = WriteFileEvent(buffer);
  } else if (fEvStreamMode==fEvStreamET) {
    std::cout << "No support for writing to ET streams" << std::endl;
    status = CODA_ERROR;
  }
  return status;
}

Int_t QwEventBuffer::WriteFileEvent(int* buffer)
{
  Int_t status = CODA_OK;
  //  fEvStream is of inherited type THaCodaData,
  //  but codaWrite is only defined for THaCodaFile.
  status = ((THaCodaFile*)fEvStream)->codaWrite(buffer);
  return status;
};


Int_t QwEventBuffer::EncodeSubsystemData(QwSubsystemArray &subsystems)
{
  // Encode the data in the elements of the subsystem array
  std::vector<UInt_t> buffer;
   subsystems.EncodeEventData(buffer);

  // Add CODA event header
  std::vector<UInt_t> header;
  header.push_back((0x0001 << 16) | (0x10 << 8) | 0xCC);
		// event type | event data type | event ID (0xCC for CODA event)
  header.push_back(4);	// size of header field
  header.push_back((0xC000 << 16) | (0x01 << 8) | 0x00);
		// bank type | bank data type (0x01 for uint32) | bank ID (0x00 for header event)
  header.push_back(++fEvtNumber); // event number (initialized to 0,
		// so increment before use to agree with CODA number)
  header.push_back(1);	// event class
  header.push_back(0);	// status summary

  // Copy the encoded event buffer into an array of integers,
  // as expected by the CODA routines.
  // Size of the event buffer in long words
  int codabuffer[header.size() + buffer.size() + 1];
  // First entry contains the buffer size
  int k = 0;
  codabuffer[k++] = header.size() + buffer.size();
  for (size_t i = 0; i < header.size(); i++)
    codabuffer[k++] = header.at(i);
  for (size_t i = 0; i < buffer.size(); i++)
    codabuffer[k++] = buffer.at(i);

  // Now write the buffer to the stream
  Int_t status = WriteEvent(codabuffer);
  // and report success or fail
  return status;
};


Int_t QwEventBuffer::EncodePrestartEvent(int runnumber, int runtype)
{
  int buffer[5];
  int localtime = (int) time(0);
  buffer[0] = 4; // length
  buffer[1] = ((kPRESTART_EVENT << 16) | (0x01 << 8) | 0xCC);
  buffer[2] = localtime;
  buffer[3] = runnumber;
  buffer[4] = runtype;
  ProcessPrestart(localtime, runnumber, runtype);
  return WriteEvent(buffer);
}
Int_t QwEventBuffer::EncodeGoEvent()
{
  int buffer[5];
  int localtime = (int) time(0);
  int eventcount = 0;
  buffer[0] = 4; // length
  buffer[1] = ((kGO_EVENT << 16) | (0x01 << 8) | 0xCC);
  buffer[2] = localtime;
  buffer[3] = 0; // (unused)
  buffer[4] = eventcount;
  ProcessGo(localtime, eventcount);
  return WriteEvent(buffer);
}
Int_t QwEventBuffer::EncodePauseEvent()
{
  int buffer[5];
  int localtime = (int) time(0);
  int eventcount = 0;
  buffer[0] = 4; // length
  buffer[1] = ((kPAUSE_EVENT << 16) | (0x01 << 8) | 0xCC);
  buffer[2] = localtime;
  buffer[3] = 0; // (unused)
  buffer[4] = eventcount;
  ProcessPause(localtime, eventcount);
  return WriteEvent(buffer);
}
Int_t QwEventBuffer::EncodeEndEvent()
{
  int buffer[5];
  int localtime = (int) time(0);
  int eventcount = 0;
  buffer[0] = 4; // length
  buffer[1] = ((kEND_EVENT << 16) | (0x01 << 8) | 0xCC);
  buffer[2] = localtime;
  buffer[3] = 0; // (unused)
  buffer[4] = eventcount;
  ProcessEnd(localtime, eventcount);
  return WriteEvent(buffer);
}


void QwEventBuffer::ResetFlags(){
};


void QwEventBuffer::DecodeEventIDBank(UInt_t *buffer)
{
  UInt_t local_datatype;
  UInt_t local_eventtype;

  fPhysicsEventFlag = kFALSE;

  if ( buffer[0] == 0 ){
    /*****************************************************************
     *  This buffer is empty.                                        *
     *****************************************************************/
    SetEventLength(1);     //  Pretend that there is one word.
    SetWordsSoFar(1);      //  Mark that we've read the word already.
    SetEventType(0);
    fEvtTag       = 0;
    fBankDataType = 0;
    fIDBankNum    = 0;
    fEvtNumber    = 0;
    fEvtClass     = 0;
    fStatSum      = 0;
  } else {
    /*****************************************************************
     *  This buffer contains data; fill the event ID parameters.     *
     *****************************************************************/
    //  First word is the number of long-words in the buffer.
    SetEventLength(buffer[0]+1);

    // Second word contains the event type, for CODA events.
    fEvtTag   = (buffer[1] & 0xFFFF0000) >> 16;  // (bits(31-16));
    local_datatype = (buffer[1] & 0xFF00) >> 8;  // (bits(15-8));
    fIDBankNum = (buffer[1] & 0xFF);             // (bits(7-0));
    if ( fIDBankNum == 0xCC) {
      //  This is a CODA event bank; the event type is equal to
      //  the event tag.
      local_eventtype = fEvtTag;
      SetEventType(local_eventtype);
      fBankDataType = local_datatype;

      if (local_eventtype>=0 && local_eventtype<=15) {
        //  This is a physics event; record the event number, event
        //  classification, and status summary.
        fEvtNumber = buffer[4];
        fEvtClass  = buffer[5];
        fStatSum   = buffer[6];
	fPhysicsEventFlag = kTRUE;
        //  Now skip to the first ROC data bank.
        SetWordsSoFar(7);
      } else {
        //  This is not a physics event, but is still in the CODA
        //  event format.  The first two words have been examined.
        fEvtNumber = 0;
        fEvtClass  = 0;
        fStatSum   = 0;
        SetWordsSoFar(2);
	//  Run this event through the Control event processing.
	//  If it is not a control event, nothing will happen.
	ProcessControlEvent(fEvtType, &buffer[fWordsSoFar]);
      }
    } else {
      //  This is not an event in the CODA event bank format,
      //  but it still follows the CEBAF common event format.
      //  Arbitrarily set the event type to "fEvtTag".
      //  The first two words have been examined.
      SetEventType(fEvtTag);
      fEvtNumber = 0;
      fEvtClass  = 0;
      fStatSum   = 0;
      SetWordsSoFar(2);
    }
  }
  QwDebug << Form("Length: %d; Tag: 0x%x; Bank ID num: 0x%x; ",
		  fEvtLength, fEvtTag, fIDBankNum)
	  << Form("Evt type: 0x%x; Evt number %d; Evt Class 0x%.8x; ",
		  fEvtType, fEvtNumber, fEvtClass)
	  << Form("Status Summary: 0x%.8x; Words so far %d",
		  fStatSum, fWordsSoFar)
	  << QwLog::endl;
};


Bool_t QwEventBuffer::FillSubsystemConfigurationData(QwSubsystemArray &subsystems)
{
  ///  Passes the data for the configuration events into each subsystem
  ///  object.  Each object is responsible for recognizing the configuration
  ///  data which it ought to decode.
  ///  NOTE TO DAQ PROGRAMMERS:
  ///      The configuration event for a ROC must have the same
  ///      subbank structure as the physics events for that ROC.
  Bool_t okay = kTRUE;
  UInt_t rocnum = fEvtType - 0x90;
  std::cerr << "QwEventBuffer::FillSubsystemConfigurationData:  "
	    << "Found configuration event for ROC"
	    << rocnum
	    << std::endl;
  //  Loop through the data buffer in this event.
  UInt_t *localbuff = (UInt_t*)(fEvStream->getEvBuffer());
  while ((okay = DecodeSubbankHeader(&localbuff[fWordsSoFar]))){
    //  If this bank has further subbanks, restart the loop.
    if (fSubbankType == 0x10) continue;
    //  If this bank only contains the word 'NULL' then skip
    //  this bank.
    if (fFragLength==1 && localbuff[fWordsSoFar]==kNullDataWord){
      fWordsSoFar += fFragLength;
      continue;
    }

    //  Subsystems may be configured to accept data in formats
    //  other than 32 bit integer (banktype==1), but the
    //  bank type is not provided.  Subsystems must be able
    //  to process their data knowing only the ROC and bank tags.
    //
    //  After trying the data in each subsystem, bump the
    //  fWordsSoFar to move to the next bank.
    subsystems.ProcessConfigurationBuffer(rocnum, fSubbankTag,
					  &localbuff[fWordsSoFar],
					  fFragLength);
    fWordsSoFar += fFragLength;
    QwDebug << "QwEventBuffer::FillSubsystemConfigurationData:  "
	    << "Ending loop: fWordsSoFar=="<<fWordsSoFar
	    <<QwLog::endl;
  }
  return okay;
};

Bool_t QwEventBuffer::FillSubsystemData(QwSubsystemArray &subsystems){
  //
  Bool_t okay = kTRUE;
  //  Clear the old event information from the subsystems.
  subsystems.ClearEventData();

  //  Loop through the data buffer in this event.
  UInt_t *localbuff = (UInt_t*)(fEvStream->getEvBuffer());
  while ((okay = DecodeSubbankHeader(&localbuff[fWordsSoFar]))){
    //  If this bank has further subbanks, restart the loop.
    if (fSubbankType == 0x10) continue;
    //  If this bank only contains the word 'NULL' then skip
    //  this bank.
    if (fFragLength==1 && localbuff[fWordsSoFar]==kNullDataWord){
      fWordsSoFar += fFragLength;
      continue;
    }

    QwDebug << "QwEventBuffer::FillSubsystemData:  "
	    << "Beginning loop: fWordsSoFar=="<<fWordsSoFar
	    <<QwLog::endl;
    
    //  Loop through the subsystems and try to store the data
    //  from this bank in each subsystem.
    //
    //  Subsystems may be configured to accept data in formats
    //  other than 32 bit integer (banktype==1), but the
    //  bank type is not provided.  Subsystems must be able
    //  to process their data knowing only the ROC and bank tags.
    //
    //  After trying the data in each subsystem, bump the
    //  fWordsSoFar to move to the next bank.
    QwDebug << "ProcessEventBuffer: ROC="<<fROC<<", SubbankTag="<< fSubbankTag
	    <<", FragLength="<<fFragLength <<QwLog::endl;
    
    subsystems.ProcessEvBuffer(fROC, fSubbankTag,
			       &localbuff[fWordsSoFar],
			       fFragLength);
    fWordsSoFar += fFragLength;
    QwDebug << "QwEventBuffer::FillSubsystemData:  "
	    << "Ending loop: fWordsSoFar=="<<fWordsSoFar
	    <<QwLog::endl;
  }
  return okay;
};


Bool_t QwEventBuffer::DecodeSubbankHeader(UInt_t *buffer){
  //  This function will decode the header information from
  //  either a ROC bank or a subbank.  It will also bump
  //  fWordsSoFar to be referring to the first word of
  //  the subbank's data.
  //
  //  NOTE TO DAQ PROGRAMMERS:
  //      All internal subbank tags MUST be defined to
  //      be greater than 31.
  Bool_t okay = kTRUE;
  if (fWordsSoFar == fEvtLength){
    //  We have reached the end of this event.
    okay = kFALSE;
  } else {
    fFragLength   = buffer[0] - 1;  // This is the number of words in the data block
    fSubbankTag   = (buffer[1]&0xFFFF0000)>>16; // Bits 16-31
    fSubbankType  = (buffer[1]&0xFF00)>>8;      // Bits 8-15
    fSubbankNum   = (buffer[1]&0xFF);           // Bits 0-7
    if (fSubbankTag<=31){
      //  Subbank tags between 0 and 31 indicate this is
      //  a ROC bank.
      fROC        = fSubbankTag;
      fSubbankTag = 0;
    }
    if (fWordsSoFar+2+fFragLength > fEvtLength){
      //  Trouble, because we'll have too many words!
      std::cerr << "fWordsSoFar+2+fFragLength=="<<fWordsSoFar+2+fFragLength
		<< " and fEvtLength==" << fEvtLength
		<< QwLog::endl;
      okay = kFALSE;
    }
    fWordsSoFar   += 2;
  }
  return okay;
};


const TString&  QwEventBuffer::DataFile(const UInt_t run, const Short_t seg = -1)
{
  TString basename = fDataFileStem + Form("%ld.",run) + fDataFileExtension;
  if(seg == -1){
    fDataFile = fDataDirectory + basename;
  } else {
    fDataFile = fDataDirectory + basename + Form(".%d",seg);
  }
  return fDataFile;
}


Bool_t QwEventBuffer::DataFileIsSegmented()
{
  glob_t globbuf;

  TString searchpath;
  TString scanvalue;
  Int_t   local_segment;

  std::vector<Int_t> tmp_segments;
  std::vector<Int_t> local_index;

  /*  Clear and set up the fRunSegments vector. */
  tmp_segments.clear();
  fRunSegments.clear();
  fRunSegments.resize(0);
  fRunIsSegmented = kFALSE;

  searchpath = fDataFile;
  glob(searchpath.Data(), GLOB_ERR, NULL, &globbuf);

  if (globbuf.gl_pathc == 1){
    /* The base file name exists.                               *
     * Do not look for file segments.                           */
    fRunIsSegmented = kFALSE;

  } else {
    /* The base file name does not exist.                       *
     * Look for file segments.                                  */
    std::cerr << "WARN: File " << fDataFile << " does not exist!\n"
	      << "      Trying to find run segments for run "
	      << fRunNumber << "...  ";

    searchpath.Append(".[0-9]*");
    glob(searchpath.Data(), GLOB_ERR, NULL, &globbuf);

    if (globbuf.gl_pathc == 0){
      /* There are no file segments and no base file            *
       * Produce and error message and exit.                    */
      std::cerr << "\n      There are no file segments either!!" << std::endl;

      // This could mean a single gzipped file!
      fRunIsSegmented = kFALSE;

    } else {
      /* There are file segments.                               *
       * Determine the segment numbers and fill fRunSegments    *
       * to indicate the existing file segments.                */

      std::cerr << "OK" << std::endl;
      scanvalue = fDataFile + ".%d";

      /*  Get the list of segment numbers in file listing       *
       *  order.                                                */
      for (size_t iloop=0;iloop<globbuf.gl_pathc;++iloop){
        /*  Extract the segment numbers from the file name.     */
        sscanf(globbuf.gl_pathv[iloop], scanvalue.Data(), &local_segment);
        tmp_segments.push_back(local_segment);
      }
      local_index.resize(tmp_segments.size(),0);
      /*  Get the list of segments sorted numerically in        *
       *  increasing order.                                     */
      TMath::Sort(static_cast<int>(tmp_segments.size()),&(tmp_segments[0]),&(local_index[0]),
                  kFALSE);
      /*  Put the segments into numerical order in              *
       *  fRunSegments.                                         */
      QwMessage << "      Found the segment(s): ";
      for (size_t iloop=0; iloop<tmp_segments.size(); ++iloop){
        local_segment = tmp_segments[local_index[iloop]];
        fRunSegments.push_back(local_segment);
        if (iloop==0){
          QwMessage << local_segment ;
        } else {
          QwMessage << ", " << local_segment ;
        }
      }
      QwMessage << "." << std::endl;
      this_runsegment = fRunSegments.begin();

      fRunIsSegmented = kTRUE;
    }
  }
  globfree(&globbuf);
  return fRunIsSegmented;
}

//------------------------------------------------------------

Int_t QwEventBuffer::CloseThisSegment()
{
  Int_t status = kFileHandleNotConfigured;
  Int_t last_runsegment;
  if (fRunIsSegmented){
    last_runsegment = *this_runsegment;
    this_runsegment++;
    if (this_runsegment <= fRunSegments.end()){
      QwMessage << "Closing run segment " << last_runsegment <<"."
		<< QwLog::endl;
      status = CloseDataFile();
    }
  } else {
    //  Don't try to close a nonsegmented file; we will explicitly
    //  use CloseDataFile() later.
  }
  return status;
}

//------------------------------------------------------------

Int_t QwEventBuffer::OpenNextSegment()
{
  Int_t status;

  if (! fRunIsSegmented){
    /*  We are processing a non-segmented run.            *
     *  We should not have entered this routine, but      *
     *  since we are here, don't do anything.             */
    status = kRunNotSegmented;

  } else if (fRunSegments.size()==0){
    /*  There are actually no file segments located.      *
     *  Return "kNoNextDataFile", but don't print an      *
     *  error message.                                    */
    status = kNoNextDataFile;

  } else if (this_runsegment >= fRunSegments.begin() &&
      this_runsegment <  fRunSegments.end() ) {
    QwMessage << "Trying to open run segment " << *this_runsegment <<std::endl;
    status = OpenDataFile(DataFile(fRunNumber,*this_runsegment),"R");

  } else if (this_runsegment ==  fRunSegments.end() ) {
    /*  We have reached the last run segment. */
    QwMessage << "There are no run segments remaining."<<std::endl;
    status = kNoNextDataFile;

  } else {
    std::cerr << "QwEventBuffer::OpenNextSegment(): Unrecognized error" << std::endl;
    status = CODA_ERROR;
  }
  return status;
}


//------------------------------------------------------------
//call this routine if we've selected the run segment by hand
Int_t QwEventBuffer::OpenDataFile(UInt_t current_run, Short_t seg)
{
  fRunNumber = current_run;

  fRunSegments.clear();
  fRunIsSegmented = kTRUE;

  fRunSegments.push_back(seg);
  this_runsegment = fRunSegments.begin();
  return OpenNextSegment();
};

//------------------------------------------------------------
//call this routine if the run is not segmented
Int_t QwEventBuffer::OpenDataFile(UInt_t current_run, const TString rw)
{
  Int_t status;
  fRunNumber = current_run;
  DataFile(fRunNumber);
  if (DataFileIsSegmented()){
    status = OpenNextSegment();
  } else {
    status = OpenDataFile(DataFile(fRunNumber),rw);
  }
  return status;
};



//------------------------------------------------------------
Int_t QwEventBuffer::OpenDataFile(const TString filename, const TString rw)
{
  if (fEvStreamMode==fEvStreamNull){
    QwDebug << "QwEventBuffer::OpenDataFile:  File handle doesn't exist.\n"
	    << "                              Try to open a new file handle!"
	    << QwLog::endl;
    fEvStream = new THaCodaFile();
    fEvStreamMode = fEvStreamFile;
  } else if (fEvStreamMode!=fEvStreamFile){
    QwError << "QwEventBuffer::OpenDataFile:  The stream is not configured as an input\n"
	    << "                              file stream!  Can't deal with this!\n"
	    << QwLog::endl;
    exit(1);
  }
  fDataFile = filename;

  if (rw.Contains("w",TString::kIgnoreCase)) {
    // If we open a file for write access, let's suppose
    // we've given the path we want to use.
    QwMessage << "Opening data file:  " << fDataFile << std::endl;
  } else {
    //  Let's try to find the data file for read access.
    glob_t globbuf;
    glob(fDataFile.Data(), GLOB_ERR, NULL, &globbuf);
    if (globbuf.gl_pathc == 0){
      //  Can't find the file; try in the "fDataDirectory".
      fDataFile = fDataDirectory + filename;
      glob(fDataFile.Data(), GLOB_ERR, NULL, &globbuf);
    }
    if (globbuf.gl_pathc == 0){
      //  Can't find the file; try gzipped.
      fDataFile = filename + ".gz";
      glob(fDataFile.Data(), GLOB_ERR, NULL, &globbuf);
    }
    if (globbuf.gl_pathc == 0){
      //  Can't find the file; try gzipped in the "fDataDirectory".
      fDataFile = fDataDirectory + filename + ".gz";
      glob(fDataFile.Data(), GLOB_ERR, NULL, &globbuf);
    }
    if (globbuf.gl_pathc == 1){
      QwMessage << "Opening data file:  " << fDataFile << std::endl;
    } else {
      fDataFile = filename;
      std::cerr << "Unable to find "
		<< filename.Data()  << " or "
		<< (fDataDirectory + filename).Data()  << std::endl;
    }
    globfree(&globbuf);
  }
  return fEvStream->codaOpen(fDataFile, rw);
};


//------------------------------------------------------------
Int_t QwEventBuffer::CloseDataFile()
{
  Int_t status = kFileHandleNotConfigured;
  if (fEvStreamMode==fEvStreamFile){
    status = fEvStream->codaClose();
  }
  return status;
}

//------------------------------------------------------------
Int_t QwEventBuffer::OpenETStream(TString computer, TString session, int mode)
{
  if (fEvStreamMode==fEvStreamNull){
#ifdef __CODA_ET
    fEvStream = new THaEtClient(computer, session, mode);
    fEvStreamMode = fEvStreamET;
#endif
  }
  return 0;
}

//------------------------------------------------------------
Int_t QwEventBuffer::CloseETStream()
{
  Int_t status = kFileHandleNotConfigured;
  if (fEvStreamMode==fEvStreamFile){
    status = fEvStream->codaClose();
  }
  return status;
}
