#include "Qset.h"
#include <iostream>
#include <math.h>
#include "tree.h"
#ifndef NDetMax
#define NDetMax 1010
#endif
using namespace std;
#define PI 3.141592653589793

extern Det *rcDETRegion[2][3][4];
extern Det rcDET[NDetMax];
extern EUppLow operator++(enum EUppLow &rs, int );
extern ERegion operator++(enum ERegion &rs, int );
extern Etype operator++(enum Etype &rs, int );
extern Edir operator++(enum Edir &rs, int );

/*
Qset
Date : 7/30/7
Author : Burnham Stokes */
/*! \file Qset.cc 
\brief Qset is a generic data read-in class that
reads ascii to set up the detector geometry.  

The file qweak.geo is the current geometry file.  
*/

/*! \file qweak.geo
Author : Burnham Stokes
\brief  qweak.geo is a simple ascii file used by Qset to read 
in the Qweak detector information.
*/
//____________________________________________________________
int Qset::FillDetec(char *geomfile){
	FILE *geom;
	char line[256];
	
	int maxchar = 256,i=0;
	geom = fopen(geomfile,"r");
	while(!feof(geom)){
		fgets(line,maxchar,geom);
		if(feof(geom))break;
		strcpy(rcDET[i].cName,strtok(line,"\n"));//detector name
		fgets(line,maxchar,geom);
		strcpy(rcDET[i].cType,strtok(line,"\n"));//detector Type
		fgets(line,maxchar,geom);
		rcDET[i].Zpos = atof(strtok(line,"\n"));//z position of detector center
		fgets(line,maxchar,geom);
		rcDET[i].Rot = atof(strtok(line,"\n"));//rotation of detector about the x-axis (degrees)
		rcDET[i].rRotCos = cos(rcDET[i].Rot*PI/180);
		rcDET[i].rRotSin = sin(rcDET[i].Rot*PI/180);
		fgets(line,maxchar,geom);
		rcDET[i].resolution = atof(strtok(line,"\n"));//spatial resolution 
		fgets(line,maxchar,geom);
		rcDET[i].TrackResolution = atof(strtok(line,"\n"));//track resolution
		fgets(line,maxchar,geom);
		rcDET[i].SlopeMatching = atof(strtok(line,"\n"));//front/back track segment slope matching
		fgets(line,maxchar,geom);
		switch(line[0]){//upper or lower detector
			case 'u':
				rcDET[i].upplow = w_upper;
				break;
			case 'd':
				rcDET[i].upplow = w_lower;
				break;
			default :
				rcDET[i].upplow = w_nowhere;
				break;
		}
		fgets(line,maxchar,geom);
		switch(line[0]){//detector region
			case '1':
				rcDET[i].region = r1;
				break;
			case '2':
				rcDET[i].region = r2;
				break;
			case '3':
				rcDET[i].region = r3;
				break;
		}
		fgets(line,maxchar,geom);
		switch(line[0]){//detector type
			case 'd':
				rcDET[i].type = d_drift;
				break;
			case 'g':
				rcDET[i].type = d_gem;
				break;
			case 't':
				rcDET[i].type = d_trigscint;
				break;
			case 'c':
				rcDET[i].type = d_cerenkov;
				break;
	
		}
		fgets(line,maxchar,geom);
		switch(line[0]){//plane direction
			case 'u':
				rcDET[i].dir = u_dir;
				break;
			case 'v':
				rcDET[i].dir = v_dir;
				break;
			case 'x':
				rcDET[i].dir = x_dir;
				break;
			case 'y':
				rcDET[i].dir = y_dir;
				break;
		
		}
		fgets(line,maxchar,geom);
		rcDET[i].center[0]=atof(strtok(line,"\n"));//detector center (x)
		fgets(line,maxchar,geom);
		rcDET[i].center[1]=atof(strtok(line,"\n"));//detector center (y)
		fgets(line,maxchar,geom);
		rcDET[i].width[0]=atof(strtok(line,"\n"));//width of active area (x)
		fgets(line,maxchar,geom);
		rcDET[i].width[1]=atof(strtok(line,"\n"));//width of active area (y)
		fgets(line,maxchar,geom);
		rcDET[i].width[2]=atof(strtok(line,"\n"));//width of active area (z)
		fgets(line,maxchar,geom);
		rcDET[i].WireSpacing=atof(strtok(line,"\n"));//Wire Spacing in X
		fgets(line,maxchar,geom);
		rcDET[i].PosOfFirstWire=atof(strtok(line,"\n"));//position of first wire (x) 
		fgets(line,maxchar,geom);
		rcDET[i].rCos=atof(strtok(line,"\n"));//cosine of wire orientation w.r.t. x-axis
		fgets(line,maxchar,geom);
		rcDET[i].rSin=atof(strtok(line,"\n"));//sine of wire orientation w.r.t. x-axis
		fgets(line,maxchar,geom);
		rcDET[i].NumOfWires=atoi(strtok(line,"\n"));//Number of wires
		fgets(line,maxchar,geom);
		rcDET[i].ID=atoi(strtok(line,"\n"));//detector ID number
		rcDET[i].samesearched=0;
		if(i!=rcDET[i].ID)cerr << "WARNING:DETECTOR ID MUST BE IN ORDER" << endl;
		i++;
	}
	numdetectors =i;
	LinkDetector();
	return 0;
}
//____________________________________________________________
void Qset::LinkDetector(){
/*This function strings together groups of detector elements */

	enum EUppLow up_low;
	enum ERegion region;
	enum Edir dir;
	enum Etype type;
	Det *rd,*rnd,*rwd;
  	for(int i = 0; i<=numdetectors; i++) {
		rd = &rcDET[i];
	    	if( !rd->samesearched ) { 
      			up_low = rd->upplow;     
      			dir   = rd->dir;
      			region  = rd->region;
			type = rd->type;
      			if( !rcDETRegion[up_low][region-1][dir] )
				rcDETRegion[up_low][region-1][dir] = rd;	       
      			rd->samesearched = 1;     
      			rwd = rd;      
      			/* check for the same parameters */
      			for(int l = i+1; l<=numdetectors; l++ ) {
				rnd = &rcDET[l];
				
				if(   rnd->upplow     == up_low
	      				&& rnd->type    == type
	      				&& rnd->dir == dir
	      				&& !rnd->samesearched
	      				&& rnd->region      == region  ) {
	  				rnd->samesearched = 1;
	  				rwd = (rwd->nextsame = rnd);
				}
				 
      			}
    		}
  	}
}
//____________________________________________________________
Qset::Qset(){
	numdetectors =0;
}
//____________________________________________________________

