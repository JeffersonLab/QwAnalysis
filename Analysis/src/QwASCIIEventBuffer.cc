/**********************************************************\
* File: QwASCIIEventBuffer.cc                              *
*                                                          *
* Author: Paul M. King, Rakitha Beminiwattha               *
* Time-stamp: <2009-02-22 15:40>                           *
\**********************************************************/

#include "QwASCIIEventBuffer.h"
#include<string>


extern Det rcDET[NDetMax];
extern Det *rcDETRegion[2][3][4];


using namespace std;

#define PI 3.141592653589793


//Loadchannel Map was intended to include decoding qweak.geo map file

Int_t QwASCIIEventBuffer::LoadChannelMap(const char *geomname){

  

  return 0;
};



Int_t QwASCIIEventBuffer::GetEvent(){
  //declaring varibles used during this routine

  
  
  char line[256];
  
  int maxchar =256;

 
  int currentevtno;

  int DetectId;
  Int_t Event;
  Int_t TotalHits; 
  
 
  std::string line1;

  Double_t rPos1, Zpos, rPos2, resolution;
  Int_t wire;
  currentevtno=-2;
  while (eventf->ReadNextLine(line1)){
    
     currentevtno=0;
   
    if (fEvtNumber == 0)//we are reading the first event
      {
	std::cout<<"Reading 1st event "<<std::endl;
	Event=QwParameterFile::GetUInt(line1);
	fEvtNumber=Event;
	std::cout<<"1st event "<< fEvtNumber <<std::endl;
	if (!eventf->ReadNextLine(line1)) continue;
	DetectId=QwParameterFile::GetUInt(line1);//reading the detectorId
	std::cout<<"DET ID "<< DetectId <<std::endl;
      }
    else if (Event == fEvtNumber){//still in the same event but a different detector
      //if (eventf->ReadNextLine(line1)) continue;
      DetectId=QwParameterFile::GetUInt(line1);
      std::cout<<"Same event "<< fEvtNumber <<std::endl;
      std::cout<<"DET ID "<< DetectId <<std::endl;
    }
    
    else if (Event != fEvtNumber) {//we are starting a different event
      std::cout<<"We are done Reading"<<fEvtNumber<< " event "<<std::endl;
      //nEvent++;
      fEvtTag++;//increment the hit counter
      //We are reading one event per each GetASCIIEvent() so we are done with one event
      break;
    }
     
    //now we are reading the detector ID 
    

    //obtain the detector infomation
    up_low=rcDET[DetectId].upplow;//top or bottom detector
    region=rcDET[DetectId].region;//R1,2 or 3
    type =rcDET[DetectId].type; //type of detector
    dir==rcDET[DetectId].dir;
    resolution = rcDET[DetectId].resolution;
    
    std::cout<<"Detecotr Id : "<<DetectId<<" up-low "<<rcDET[DetectId].upplow<<" region "<<rcDET[DetectId].region<<" type "<<rcDET[DetectId].type<<" DIR "<<rcDET[DetectId].dir <<std::endl;

    //now we are going to read individual hits 
    //reading no.of hits for this event
    if (!eventf->ReadNextLine(line1))continue;
    TotalHits=QwParameterFile::GetUInt(line1);
    
    for(int cHits=0;cHits<TotalHits;cHits++){
      eventf->ReadNextLine(line1);
      wire = QwParameterFile::GetUInt(line1);
      eventf->ReadNextLine(line1);
      Zpos=atof(line1.c_str());
      eventf->ReadNextLine(line1);
      rPos1=atof(line1.c_str());
      eventf->ReadNextLine(line1);
      rPos2=atof(line1.c_str());
      std::cout<<" Wire Hit "<<wire; 
      currentHit=new QwHit(0,0,0,0,rcDET[DetectId].region,rcDET[DetectId].upplow, DetectId ,rcDET[DetectId].dir,wire,0) ; //order of parameters-> electronics stuffs are neglected, and  plane==type and data is set to zero
      currentHit->SetDriftDistance(rPos1);
      currentHit->SetSpatialResolution(resolution);
      currentHit->SetZPos(Zpos);
      fASCIIHits.push_back(*currentHit);
    }
    std::cout<<std::endl;
     std::cout<<"Finish  Wire Hits "<<std::endl; 
    //we are now complete with this eventhits 'numberhits' relating to DetectId detector.
    //next line in the file is the eventnumber
    if (!eventf->ReadNextLine(line1)){
      std::cout<<"EOF Reached"<<" Current Event "<<fEvtNumber <<std::endl;
      currentevtno=-1;
      break;
    }
    Event=QwParameterFile::GetUInt(line1);

    std::cout<<"Reading Next event "<< Event <<std::endl;
   

    

  }
  //Event always reads the event number from the ASCII file
  //fEvtNumber always  stores the Event number to be processed
  //CurrentEvent stores the current event number processing so it can be used any where in the QwASCIIEventBuffer
  //currentevtno is just a temp variable
  

  //finished reading the ASCII file so I'm still returning current event number to QwTracking while loop
  if (currentevtno == -1){
    currentevtno=fEvtNumber;
    CurrentEvent=fEvtNumber;
  }
  //when next GetEvent is executed from QwTracking while loop, since I finished reading ASCII file
  //I need to get out of the main while loop so I set currentevtno=0 and retrun it to the main routine.
  else if (currentevtno == -2){
    currentevtno=0;
    CurrentEvent=fEvtNumber;
  }
  //When I finished only reading one event and ASCII file has more event then I return current event number
  // and set fEvtNumber=Event i.e to the next event to be read
  else{
    currentevtno=fEvtNumber;
    fEvtNumber=Event;//when next time GetEvent() is called this will be the event number it will process
    CurrentEvent=Event-1;//stores the current event number
  }
  std::cout<<"Finished reading one event "<< fEvtNumber <<std::endl;
  return currentevtno;
}


//------------------------------------------------------------
Int_t QwASCIIEventBuffer::OpenDataFile(const TString filename,const TString rw="R"){

  
  

  eventf = new QwParameterFile(filename);  //Open the file
  

  fEvtTag=0;
  
  fEvtNumber=0;
    
    
  return 1;  
}
//-----------------------------------------------------------

Int_t QwASCIIEventBuffer::ProcessHitContainer(QwHitContainer & qwhits){

// List of hits
  int nhits = 0;
  Hit *newhit,*hitlist;

  // Detector region/type/direction identifiers
  enum EUppLow up_low2;
  enum ERegion region2;
  enum Edir    dir2;

  Int_t plane, plane2;

  // Detector ID
  int detecID = 0;
  int firstdetec = 1;
  Det *rd = NULL;

  //  Do something to clear rd->hitbydet for all
  //  detectors.


  QwDetectorID local_id;  
  //  Loop through the QwHitContainer
  for (QwHitContainer::iterator qwhit = qwhits.begin();
       qwhit != qwhits.end(); qwhit++){
    local_id = qwhit->GetDetectorID();

    up_low = (EUppLow) local_id.fPackage;
    region = (ERegion) local_id.fRegion;
    dir    = (Edir) local_id.fDirection;
    plane  = local_id.fPlane;


    // when this is the first detector of the event
    if (firstdetec) {
      hitlist = NULL;
      newhit = NULL;
      std::cout<<"First Detector "<<std::endl;
      firstdetec = 0;
      up_low2 = up_low;
      region2 = region;
      plane2 = plane; //Note that plane contains the detector Id
      dir2 = dir;
      rd = rcDETRegion[up_low][region-1][dir];
      while (rd->ID != plane){
	rd = rd->nextsame;
      }
      std::cout<<"Detector ID " << rd->ID <<std::endl;
    } else if (up_low2 == up_low &&
	       region2 == region &&
	       plane2 == plane   &&
	       dir2 == dir) {
      //  Same plane!  Do nothing!
    } else {
      // this is not the first detector of the file
      hitlist = NULL;
      newhit = NULL;
      // compare to previous hit
      if (up_low2 == up_low &&
	  region2 == region &&
	  plane2 != plane   &&
	  dir2 == dir) {
	// like-pitched detector plane
	rd = rcDETRegion[up_low][region-1][dir];
	while (rd->ID != plane){
	  rd = rd->nextsame;
	}
	plane2 = plane;
	std::cout<<"Detector ID " << rd->ID <<std::endl;
      } else {
	// different detector plane
	rd = rcDETRegion[up_low][region-1][dir];
	while (rd->ID != plane){
	  rd = rd->nextsame;
	}
	up_low2 = up_low;
	region2 = region;
	plane2   = plane;
	dir2    = dir;
	std::cout<<"Detector ID " << rd->ID <<std::endl;
      }
    }



    newhit = (Hit*) malloc (sizeof(Hit));
    //	  // set event number
    newhit->ID = CurrentEvent;  
    // Wire number
    newhit->wire = local_id.fElement;
    //	  // Z position of wire plane (first wire for region 3)
    newhit->Zpos = qwhit->GetZPos();
    // Distance of hit from wire
    newhit->rPos1 = qwhit->GetDriftDistance();
    //	  // Placeholder for future code
    newhit->rPos2 = 0;
    // Get the spatial resolution for this hit
    newhit->Resolution = qwhit->GetSpatialResolution();
    // the hit's pointer back to the detector plane
    newhit->detec = rd;

    // Chain the hits
    newhit->next = hitlist;
    hitlist = newhit;
	  
    // Chain the hits in each detector
    newhit->nextdet = rd->hitbydet;
    rd->hitbydet = newhit;
  }

  //  Look at hits in Detector ID 0
  rd     = &(rcDET[0]);
  newhit = rd->hitbydet;
  do {
    //  Print some stuff...
    std::cout << newhit->ID <<" "
	      << newhit->wire <<" "
	      << newhit->Zpos <<" "
	      << newhit->rPos1 <<" "
	      << newhit->rPos2 <<" "
	      << newhit->Resolution <<" "
	      << newhit->detec->ID <<" "
	      // << newhit->next <<" "
// 	      << newhit->nextdet <<" "
	      << std::endl;

    newhit = newhit->next;
  }  while (newhit != NULL);



}
