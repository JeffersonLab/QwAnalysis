/**********************************************************\
* File: QwASCIIEventBuffer.cc                              *
*                                                          *
* Author: Paul M. King, Rakitha Beminiwattha               *
* Time-stamp: <2009-02-22 15:40>                           *
\**********************************************************/

/*This class will act as the interface between the QTR and Track decoding
software. Mainly InitrcDETRegion, GetrcDETRegion will be called in the
QwAnalysis main routine to generate and update rcDET & rcDETRegion after
decoding CODA events in to QwHit & QwHitContainer.
The main data stuctures in the Track decoding are QwHit, QwDetectorInfo and QwHitContainer.
While Hit, Det and rcDET & rcDETRegion are for the QTR.
The QwASCIIEventBuffer will update rcDET & rcDETRegion from QwDetectorInfo & QwHitContainer data
structures.
Rakitha (04/24/2009)
*/

#include "QwASCIIEventBuffer.h"
#include "Qset.h"
#include <string>
#include <cmath>



//#define PI 3.141592653589793


//Loadchannel Map was intended to include decoding qweak.geo map file

Int_t QwASCIIEventBuffer::LoadChannelMap(const char *geomname) {



    return 0;
};



Int_t QwASCIIEventBuffer::InitrcDETRegion( std::vector< std::vector< QwDetectorInfo > > & tmp_detector_info) {

    if (DEBUG1) std::cout<<"Starting InitrcDETRegion "<<std::endl;
    fDetectorInfo=tmp_detector_info;
    DetectorCounter=0;

    for (UInt_t j=0;j<fDetectorInfo.size();j++) {
        for (UInt_t i=0;i<fDetectorInfo.at(j).size();i++) {
	  if (DEBUG1){
	    //InitrcDETRegion is called for each region to add detectors
	    std::cout<<" Region "<<fDetectorInfo.at(j).at(i).fRegion<<" ID "<<fDetectorInfo.at(j).at(i).fDetectorID<<" Detector counter "<<DetectorCounter<<" Package "<<fDetectorInfo.at(j).at(i).fPackage << " Plane " << i+1 << " Dir  " << fDetectorInfo.at(j).at(i).fDirection <<std::endl;

	  }
            AddDetector(fDetectorInfo.at(j).at(i),DetectorCounter);
            DetectorCounter++;
        }

    }


 // now rcDET is ready.
  // now linking detectors

  Qset qset;
  qset.SetNumDetectors(DetectorCounter);
  qset.LinkDetectors();
  qset.DeterminePlanes();
  if (DEBUG1) std::cout<<"Ending InitrcDETRegion "<<"Total detectors "<<DetectorCounter<<std::endl;

  return 1;
};


void  QwASCIIEventBuffer::GetrcDETRegion(QwHitContainer &HitList, Int_t event_no)
{
  //  List of hits
  //    int nhits = 0;
  QwHit *newhit  = NULL;
  QwHit *hitlist = NULL;

  // Detector region/type/direction identifiers
  EQwDetectorPackage package2;
  EQwRegionID        region2;
  EQwDirectionID     dir2;

  package2 = kPackageNull;
  region2  = kRegionIDNull;
  dir2     = kDirectionNull;

  Int_t detectorId1 = 0;
  Int_t detectorId2 = 0;

  // Detector ID
  //    int detecID = 0;
  int firstdetec = 1;
  Det *rd        = NULL;

  //  Do something to clear rd->hitbydet for all detectors.


  QwDetectorID local_id;

  //  Loop through the QwHitContainer
  for (QwHitContainer::iterator qwhit = HitList.begin();
       qwhit != HitList.end(); qwhit++) {

    local_id = qwhit->GetDetectorID();
    package  = (EQwDetectorPackage) local_id.fPackage;
    region   = (EQwRegionID)        local_id.fRegion;
    dir      = (EQwDirectionID)     local_id.fDirection;

    detectorId1 = fDetectorInfo.at(local_id.fPackage).at(local_id.fPlane-1).fDetectorID;


    // when this is the first detector of the event
    if (firstdetec) {
      //if (DEBUG1) std::cout<<"First Detector "<<std::endl;
      hitlist     = NULL;
      newhit      = NULL;
      firstdetec  = 0;
      package2    = package;
      region2     = region;
      detectorId2 = detectorId1;
      dir2        = dir;
      rd          = rcDETRegion[package][region][dir];
      while (rd->ID != detectorId1) {
	rd = rd->nextsame;
      }
      //if (DEBUG1) std::cout<<"Detector ID " << rd->ID <<std::endl;
    } else if (package2 == package &&
	       region2 == region &&
	       detectorId2==detectorId1   &&
	       dir2 == dir) {
      //  Same plane!  Do nothing!
    } else {
      // this is not the first detector of the file
      hitlist = NULL;
      newhit = NULL;
      // compare to previous hit
      if (package2 == package &&
	  region2 == region &&
	  detectorId2!=detectorId1   &&
	  dir2 == dir) {
	// like-pitched detector plane
	rd = rcDETRegion[package][region][dir];
	while (rd->ID != detectorId1) {
	  rd = rd->nextsame;
	}
	detectorId2=detectorId1;
	//if (DEBUG1) std::cout<<"Detector ID " << rd->ID <<std::endl;
      } else {
	// different detector plane
	rd = rcDETRegion[package][region][dir];
	while (rd->ID != detectorId1) {
	  rd = rd->nextsame;
	}
	package2 = package;
	region2 = region;
	detectorId2=detectorId1;
	dir2    = dir;
	//if (DEBUG1) std::cout<<"Detector ID " << rd->ID <<std::endl;
      }
    }



    newhit = new QwHit;
    assert(newhit);
    //newhit = (QwHit*) malloc (sizeof(QwHit));

    // Set event number (obsolete since r377, wdconinc)
    //newhit->ID = event_no; //set the current event number

    // Wire number (obsolete since r377, wdconinc)
    //newhit->wire = local_id.fElement;

    // Z position of wire plane (first wire for region 3)
    //newhit->Zpos = qwhit->GetZPos();
    //newhit->SetZPos(qwhit->GetZPos());
    // Distance of hit from wire
    //newhit->rPos1 = qwhit->GetDriftDistance();
    newhit->SetDriftDistance(qwhit->GetDriftDistance());
    //	  // Placeholder for future code
    newhit->rPos2 = 0;
    // Get the spatial resolution for this hit
    newhit->SetSpatialResolution(qwhit->GetSpatialResolution());

    // the hit's pointer back to the detector plane
    //newhit->detec = rd; // (obsolete since r377, wdconinc)

    // Chain the hits
    newhit->next = hitlist;
    hitlist = newhit;

    // Chain the hits in each detector
    newhit->nextdet = rd->hitbydet;
    rd->hitbydet = newhit;
  }

};

void QwASCIIEventBuffer::AddDetector(QwDetectorInfo qwDetector, Int_t i) {
    //if (DEBUG1) std::cout<<" Region "<<qwDetector.Region<<" ID "<<qwDetector.DetectorId<<" Detector counter "<<i<<" Package "<<qwDetector.package<<std::endl;

    if (qwDetector.fRegion ==2)
        rcDET[i].sName="HDC";
    else if (qwDetector.fRegion ==3)
        rcDET[i].sName="VDC";
    rcDET[i].sType = qwDetector.fType;
    rcDET[i].Zpos = qwDetector.GetZPosition();
    rcDET[i].Rot = qwDetector.GetDetectorRotation();
    rcDET[i].resolution = qwDetector.GetSpatialResolution();
    rcDET[i].TrackResolution = qwDetector.GetTrackResolution();
    rcDET[i].SlopeMatching = qwDetector.GetSlopeMatching();

    rcDET[i].package = (EQwDetectorPackage)qwDetector.fPackage;
    rcDET[i].region = (EQwRegionID)qwDetector.fRegion;
    rcDET[i].type = (EQwDetectorType)qwDetector.fType;
    rcDET[i].dir  = (EQwDirectionID)qwDetector.fDirection;

    rcDET[i].center[0] = qwDetector.GetXPosition();
    rcDET[i].center[1] = qwDetector.GetYPosition();

    rcDET[i].width[0] = qwDetector.GetActiveWidthX();
    rcDET[i].width[1] = qwDetector.GetActiveWidthY();
    rcDET[i].width[2] = qwDetector.GetActiveWidthZ();

    rcDET[i].WireSpacing = qwDetector.GetElementSpacing();
    rcDET[i].PosOfFirstWire = qwDetector.GetElementOffset();
    rcDET[i].rCos = qwDetector.GetElementAngleCos();
    rcDET[i].rSin = qwDetector.GetElementAngleSin();
    rcDET[i].NumOfWires = qwDetector.GetNumberOfElements();
    rcDET[i].ID = qwDetector.fDetectorID;
    rcDET[i].index = i;
    rcDET[i].samesearched = 0;

    rcDET[i].SetActive();
};

Int_t QwASCIIEventBuffer::GetEvent()
{
  //declaring varibles used during this routine

  //    char line[256];
  //    int maxchar = 256;

  Int_t currentevtno = 0;

  //    Int_t Event;
  Int_t TotalHits = 0;


  std::string line1;

  Double_t rPos1, Zpos, rPos2, resolution;
  Int_t wire;

  //To keep the track on when to break the  while (eventf->ReadNextLine(line1)) loop
  currentevtno=1;


  if (eventf->IsEOF())//at the end of  the last event the next GetEvent() call will terminate the QwTracking while loop by setting currentevtno=0.
    currentevtno=0;

  if (fASCIIHits.size()>0)
    fASCIIHits.clear();//std::cout<<" Vector not empty "<<std::endl;


  while (eventf->ReadNextLine(line1)) {

    if (fEvtNumber == 0) { //we are reading the first event
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
    } else if (CurrentEvent == fEvtNumber) {//still in the same event but a different detector
      //eventf->ReadNextLine(line1);
      DetectId=QwParameterFile::GetUInt(line1);
      //std::cout<<" Event "<< fEvtNumber <<std::endl;
      //std::cout<<"DET ID "<< DetectId <<std::endl;

      //If routine is in a middle of same event increment it
      currentevtno++;

      //now we are going to read individual hits
      //reading no.of hits for this event
      eventf->ReadNextLine(line1);
    } else if (CurrentEvent != fEvtNumber) {//we are starting a different event
      if (currentevtno>1) {//greater than 1 means I have finished analyzing one whole event so break from the while loop
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
    package    = rcDET[DetectId].package;//top or bottom detector
    region     = rcDET[DetectId].region;//R1,2 or 3
    type       = rcDET[DetectId].type; //type of detector
    dir        = rcDET[DetectId].dir;
    resolution = rcDET[DetectId].resolution;

    std::cout<<"Detector Id : "<<DetectId<<" package "<<rcDET[DetectId].package<<" region "<<rcDET[DetectId].region<<" type "<<rcDET[DetectId].type<<" DIR "<<rcDET[DetectId].dir <<std::endl;



    TotalHits=QwParameterFile::GetUInt(line1);
    //std::cout<<" Total Hits "<<TotalHits<<std::endl;


    for (int cHits=0;cHits<TotalHits;cHits++) {
      eventf->ReadNextLine(line1);
      wire = QwParameterFile::GetUInt(line1);
      if (wire > 281) wire -= 281;
      eventf->ReadNextLine(line1);
      Zpos=atof(line1.c_str());
      eventf->ReadNextLine(line1);
      rPos1=atof(line1.c_str());
      eventf->ReadNextLine(line1);
      rPos2=atof(line1.c_str());
      //std::cout<<" Wire Hit "<<wire;

      //NOTE: In this QwHit object, plane is set to the DetectorId

      currentHit=new QwHit(0,0,0,0,rcDET[DetectId].region,rcDET[DetectId].package, rcDET[DetectId].plane ,rcDET[DetectId].dir,wire,0) ; //order of parameters-> electronics stuffs are neglected, and  plane=DetectId and data is set to zero
      currentHit->SetDriftDistance(rPos1);
      currentHit->SetSpatialResolution(resolution);
      //currentHit->SetZPos(Zpos);
      fASCIIHits.push_back(*currentHit);
    }
    //std::cout<<"Size of the Vector "<<fASCIIHits.size()<<std::endl;

    //std::cout<<"Finish  Wire Hits "<<std::endl;
    //we are now complete with this event having  'numberhits' no.of hits  relating to DetectId detector.
    //next line in the file is the eventnumber
    if (!eventf->ReadNextLine(line1)) {
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
Int_t QwASCIIEventBuffer::OpenDataFile(const TString filename,const TString rw="R") {




    eventf = new QwParameterFile(filename);  //Open the file


    fEvtTag=0;

    fEvtNumber=0;


    return 1;
}
//-----------------------------------------------------------

Int_t QwASCIIEventBuffer::ProcessHitContainer(QwHitContainer & qwhits)
{

  // List of hits
  //    int nhits = 0;
  QwHit *newhit  = NULL;
  QwHit *hitlist = NULL;

  // Detector region/type/direction identifiers
  EQwDetectorPackage package2;
  EQwRegionID        region2;
  EQwDirectionID     dir2;

  package2 = kPackageNull;
  region2  = kRegionIDNull;
  dir2     = kDirectionNull;

  Int_t plane  = 0;
  Int_t plane2 = 0;

  // Detector ID
  //    int detecID = 0;
  int firstdetec = 1;
  Det *rd        = NULL;

  //  Do something to clear rd->hitbydet for all
  //  detectors.


  QwDetectorID local_id;
  //  Loop through the QwHitContainer
  for (QwHitContainer::iterator qwhit = qwhits.begin(); qwhit != qwhits.end(); qwhit++) {
    local_id = qwhit->GetDetectorID();

    package = (EQwDetectorPackage) local_id.fPackage;
    region  = (EQwRegionID) local_id.fRegion;
    dir     = (EQwDirectionID) local_id.fDirection;
    plane   = local_id.fPlane;


    // when this is the first detector of the event
    if (firstdetec) {
      hitlist = NULL;
      newhit  = NULL;
      std::cout<<"First Detector "<<std::endl;
      firstdetec = 0;
      package2 = package;
      region2 = region;
      plane2 = plane; //Note that plane contains the detector Id
      dir2 = dir;
      rd = rcDETRegion[package][region][dir];
      while (rd->plane != plane) {
	rd = rd->nextsame;
      }
      if (DEBUG1) std::cout << "Detector plane " << rd->plane << std::endl;
    } else if (package2 == package &&
	       region2  == region  &&
	       plane2   == plane   &&
	       dir2     == dir) {
      //  Same plane!  Do nothing!
    } else {
      // this is not the first detector of the file
      hitlist = NULL;
      newhit  = NULL;
      // compare to previous hit
      if (package2 == package &&
	  region2  == region  &&
	  plane2   != plane   &&
	  dir2     == dir) {
	// like-pitched detector plane
	rd = rcDETRegion[package][region][dir];
	while (rd->plane != plane) {
	  rd = rd->nextsame;
	}
	plane2 = plane;
	if (DEBUG1) std::cout << "Detector plane " << rd->plane << std::endl;
      } else {
	// different detector plane
	rd = rcDETRegion[package][region][dir];
	while (rd->plane != plane) {
	  rd = rd->nextsame;
	}
	package2 = package;
	region2  = region;
	plane2   = plane;
	dir2     = dir;
	if (DEBUG1) std::cout << "Detector plane " << rd->plane << std::endl;
      }
    }


    newhit = new QwHit;
    assert(newhit);
    //newhit = (QwHit*) malloc (sizeof(QwHit));

    // Set event number (obsolete since r377, wdconinc)
    //newhit->ID = fEvtNumber;

    // Wire number
    newhit->SetElement(local_id.fElement);

    // Z position of wire plane (first wire for region 3)
    // newhit->Zpos = qwhit->GetZPos();
    //newhit->SetZPos(qwhit->GetZPos());
    // Distance of hit from wire
    //newhit->rPos1 = qwhit->GetDriftDistance();
    newhit->SetDriftDistance(qwhit->GetDriftDistance());
    //	  // Placeholder for future code
    newhit->rPos2 = 0;
    // Get the spatial resolution for this hit
    // newhit->Resolution = qwhit->GetSpatialResolution();
    newhit->SetSpatialResolution(qwhit->GetSpatialResolution());
    // the hit's pointer back to the detector plane
    //newhit->detec = rd; // (obsolete since r377, wdconinc)

    // Chain the hits
    newhit->next = hitlist;
    hitlist = newhit;

    // Chain the hits in each detector
    newhit->nextdet = rd->hitbydet;
    rd->hitbydet = newhit;
  }

  return 0;
};









