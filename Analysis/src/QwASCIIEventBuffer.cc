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

  
  Int_t Event;
  Int_t TotalHits; 
  
 
  std::string line1;

  Double_t rPos1, Zpos, rPos2, resolution;
  Int_t wire;

  //To keep the track on when to break the  while (eventf->ReadNextLine(line1)) loop 
  currentevtno=1;
  

  if (eventf->IsEOF())//at the end of  the last event the next GetEvent() call will terminate the QwTracking while loop by setting currentevtno=0.
   currentevtno=0; 

  if (fASCIIHits.size()>0)
    fASCIIHits.clear();//std::cout<<" Vector not empty "<<std::endl;
    

  while (eventf->ReadNextLine(line1)){
    
    
   
    if (fEvtNumber == 0)//we are reading the first event
      {
	//std::cout<<"Reading 1st event "<<std::endl;
	CurrentEvent=QwParameterFile::GetUInt(line1);	
	//std::cout<<"1st event "<< CurrentEvent <<std::endl;
	eventf->ReadNextLine(line1);
	DetectId=QwParameterFile::GetUInt(line1);//reading the detectorId
	//std::cout<<"DET ID "<< DetectId <<std::endl;

	//if routine is  starting a new event increment it
	currentevtno++;

	//now we are going to read individual hits 
	//reading no.of hits for this event
	eventf->ReadNextLine(line1);
      }
    else if (CurrentEvent == fEvtNumber){//still in the same event but a different detector
      //eventf->ReadNextLine(line1);
      DetectId=QwParameterFile::GetUInt(line1);
      //std::cout<<" Event "<< fEvtNumber <<std::endl;
      //std::cout<<"DET ID "<< DetectId <<std::endl;

      //If routine is in a middle of same event increment it
      currentevtno++;

      //now we are going to read individual hits 
      //reading no.of hits for this event
      eventf->ReadNextLine(line1);
    }    
    else if (CurrentEvent != fEvtNumber) {//we are starting a different event
      if (currentevtno>1){//greater than 1 means I have finished analyzing one whole event so break from the while loop
	//std::cout<<" We are done Reading "<<fEvtNumber<< " event "<<std::endl;
	//save the DetId for next GetEvent call
	 DetectId=QwParameterFile::GetUInt(line1);
	//nEvent++;
	fEvtTag++;//increment the hit counter
	//We are reading one event per each GetASCIIEvent() so we are done with one event      
	break;
      }
      //else {//equal to 1 mean I'm starting a new event
      //std::cout<<" Event "<< CurrentEvent <<std::endl;
      //std::cout<<"DET ID "<< DetectId <<std::endl;
      //}
    }
     fEvtNumber=CurrentEvent;
    
    
    
    
    //obtain the detector infomation
    up_low=rcDET[DetectId].upplow;//top or bottom detector
    region=rcDET[DetectId].region;//R1,2 or 3
    type =rcDET[DetectId].type; //type of detector
    dir==rcDET[DetectId].dir;
    resolution = rcDET[DetectId].resolution;
    
    std::cout<<"Detecotr Id : "<<DetectId<<" up-low "<<rcDET[DetectId].upplow<<" region "<<rcDET[DetectId].region<<" type "<<rcDET[DetectId].type<<" DIR "<<rcDET[DetectId].dir <<std::endl;

    
      
    TotalHits=QwParameterFile::GetUInt(line1);
    //std::cout<<" Total Hits "<<TotalHits<<std::endl;
    
    
    for(int cHits=0;cHits<TotalHits;cHits++){
      eventf->ReadNextLine(line1);
      wire = QwParameterFile::GetUInt(line1);
      eventf->ReadNextLine(line1);
      Zpos=atof(line1.c_str());
      eventf->ReadNextLine(line1);
      rPos1=atof(line1.c_str());
      eventf->ReadNextLine(line1);
      rPos2=atof(line1.c_str());
      //std::cout<<" Wire Hit "<<wire; 
      currentHit=new QwHit(0,0,0,0,rcDET[DetectId].region,rcDET[DetectId].upplow, DetectId ,rcDET[DetectId].dir,wire,0) ; //order of parameters-> electronics stuffs are neglected, and  plane=DetectId and data is set to zero
      currentHit->SetDriftDistance(rPos1);
      currentHit->SetSpatialResolution(resolution);
      currentHit->SetZPos(Zpos);
      fASCIIHits.push_back(*currentHit);
    }
    //std::cout<<"Size of the Vector "<<fASCIIHits.size()<<std::endl;
    
    //std::cout<<"Finish  Wire Hits "<<std::endl; 
    //we are now complete with this event having  'numberhits' no.of hits  relating to DetectId detector.
    //next line in the file is the eventnumber
    if (!eventf->ReadNextLine(line1)){
      //std::cout<<" EOF Reached "<<" Current Event "<<fEvtNumber <<std::endl;      
      break;
    }
    CurrentEvent=QwParameterFile::GetUInt(line1);

    //std::cout<<"Reading Next event "<< CurrentEvent <<std::endl;
   
    
    

  }  

 
  std::cout<<" Finished reading current event "<< fEvtNumber<<" Next Event "<<CurrentEvent <<" state "<<currentevtno<<std::endl;
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
    newhit->ID = fEvtNumber;  
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
  /*
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
  */



}
