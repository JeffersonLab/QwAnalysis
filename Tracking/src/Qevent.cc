#include "Qevent.h"

#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cstring>

#ifndef NDetMax
# define NDetMax 1010
#endif

#ifndef NMaxHits
# define NMaxHits 1000
#endif

using namespace std;

extern Det* rcDETRegion[2][3][4];
extern Det  rcDET[NDetMax];
extern EUppLow operator++(enum EUppLow &rs, int );
extern ERegion operator++(enum ERegion &rs, int );
extern Etype operator++(enum Etype &rs, int );
extern Edir operator++(enum Edir &rs, int );

/*____________________________________________________________
Qevent
Date : 7/30/7
Author : Burnham Stokes*/
/*! \file Qevent.cc
\brief Qevent is a generic data read-in class.

Description : Qevent is a generic data read-in class that
reads ascii to generate events.  The file 'qweak.event' is an
example event file.  The file 'template.event' has
documentation to describe the data in 'qweak.event'.
*/


//____________________________________________________________
Qevent::Qevent(){
	cout << "Qevent object created" << endl;
	numevents=0;
	hitlist = 0;
	nevent = 0;
        newevent = 0;
	firstevent = 1;
}
//____________________________________________________________
int Qevent::Open(const char *eventfile){
  events = fopen(eventfile,"r");
  if (events) return 1;
  else return 0;

}
//____________________________________________________________
int Qevent::GetEvent(){
	hitlist = NULL;
	char line[256];
	Hit *newhit;
	int nhits =0;
	int maxchar = 256,i=0;
	//Detector region/type/direction identifiers
	enum EUppLow up_low,up_low2;
	enum ERegion region,region2;
	enum Edir dir,dir2;
	enum Etype type,type2;

	int nevents=0;
	int detecID=0;

	int firstdetec = 1;
	Det *rd;
	if(feof(events))return 0;
	while(!feof(events) && newevent == nevent){
		if(firstdetec && firstevent){
			fgets(line,maxchar,events);
			nevent = atoi(strtok(line,"\n"));// event number
			firstevent--;
			nevents++;
		}
		fgets(line,maxchar,events);
		//detecID is an integer identifying which detector is being read in
		detecID = atoi(strtok(line,"\n"));

		//Obtain the detector information from rcDET which is set up by Qset
		up_low = rcDET[detecID].upplow;//The 'top' or 'bottom' detector
		region = rcDET[detecID].region;
		dir = rcDET[detecID].dir;// the wire direction for the case of drift chambers
		type = rcDET[detecID].type;// the detector type

		if(firstdetec){
			firstdetec = 0;
			up_low2 = up_low;
			region2 = region;
			type2 = type;
			dir2 = dir;
			rd = rcDETRegion[up_low][region-1][dir];
		}
		else{
			hitlist = NULL;
			newhit = NULL;
			if(up_low2 == up_low &&
			   region2 == region &&
			   type2 == type &&
			   dir2 == dir){
				rd = rd->nextsame;
				//cerr << "went to next" << endl;
			}
			else{
				rd = rcDETRegion[up_low][region-1][dir];
				up_low2 = up_low;
				region2 = region;
				type2 = type;
				dir2 = dir;
			}
		}
		rd->hitbydet = 0;
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
			newhit->Resolution = rd->resolution;//get the spatial resolution for this hit
			newhit->detec = rd;//the hit's pointer back to the detector plane

			newhit->next = hitlist;//chain the hits
			hitlist = newhit;

			//chain the hits in each detector
			newhit->nextdet = rd->hitbydet;
			rd->hitbydet = newhit;
		}
		fgets(line,maxchar,events);
		newevent = atoi(strtok(line,"\n"));// event number
	}
	nevent = newevent;
	return nevent-1;
}

//____________________________________________________________
