#include "Qevent.h"
#include <iostream>
#include <math.h>

#ifndef NDetMax
#define NDetMax 1010
#endif
#ifndef NMaxHits 
#define NMaxHits 1000
#endif

using namespace std;

extern Det *rcDETRegion[2][3][4];
extern Det rcDET[NDetMax];
extern EUppLow operator++(enum EUppLow &rs, int );
extern ERegion operator++(enum ERegion &rs, int );
extern Etype operator++(enum Etype &rs, int );
extern Edir operator++(enum Edir &rs, int );

/*____________________________________________________________
Qevent
Date : 7/30/7
Author : Burnham Stokes
Description : Qevent is a generic data read-in class that
reads ascii to generate events.  The file 'qweak.event' is an
example event file.  The file 'template.event' has 
documentation to describe the data in 'qweak.event'.
____________________________________________________________*/


//____________________________________________________________
Qevent::Qevent(){
	numevents=0;
	hitlist = 0;
}
//____________________________________________________________
int Qevent::FillHits(char *eventfile){
	char line[256];
	Hit *newhit;
	int nhits =0;
	int maxchar = 256,i=0;
	//Detector region/type/direction identifiers
	enum EUppLow up_low;
	enum ERegion region;
	enum Edir dir;
	enum Etype type;

	int nevent=0,nevents=0;
	int detecID=0;
	
	int changedetec,firstdetec = 1;
	Det *rd;
	events = fopen(eventfile,"r");

	while(!feof(events)){
		fgets(line,maxchar,events);
		if(feof(events))break;
		nevent = atoi(strtok(line,"\n"));// event number
		nevents++;
			


		fgets(line,maxchar,events);
		//detecID is an integer identifying which detector is being read in
		detecID = atoi(strtok(line,"\n"));

		//Obtain the detector information from rcDET which is set up by Qset
		up_low = rcDET[detecID].upplow;//The 'top' or 'bottom' detector
		region = rcDET[detecID].region;
		dir = rcDET[detecID].dir;// the wire direction for the case of drift chambers
		type = rcDET[detecID].type;// the detector type 


		//String the different directions/layers in each detector
		if(firstdetec){
			rd = rcDETRegion[up_low][region-1][dir];
			firstdetec = 0;
			changedetec = detecID;
		}
		else if(detecID!=changedetec){
			rd = rd->nextsame;
			changedetec = detecID;
		}
		


		//number of hits
		fgets(line,maxchar,events);
		nhits = atoi(strtok(line,"\n"));
		for(i =0;i<nhits;i++){
			newhit = (Hit *)malloc( sizeof(Hit)) ;
			newhit->ID = nevent;//event number
			fgets(line,maxchar,events);
			newhit->wire = atoi(strtok(line,"\n"));//wire number
			fgets(line,maxchar,events);
			newhit->Zpos = atof(strtok(line,"\n"));//Z position of wire plane(first wire for region 3)
			fgets(line,maxchar,events);
			newhit->rPos1 = atof(strtok(line,"\n"));//distance of hit from wire
			fgets(line,maxchar,events);
			newhit->rPos2 = atof(strtok(line,"\n"));//placeholder for future code

			newhit->detec = rd;//the hit's pointer back to the detector plane

			newhit->next = hitlist;//chain the hits
			hitlist = newhit;

			//chain the hits in each detector
			newhit->nextdet = rd->hitbydet;
			rd->hitbydet = newhit;	
		}
	}
	
	return 1;
}

//____________________________________________________________
