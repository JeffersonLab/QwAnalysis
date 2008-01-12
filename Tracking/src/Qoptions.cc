#include "Qoptions.h"
#include <iostream>
#include <math.h>
#include "tree.h"
#ifndef NDetMax
#define NDetMax 1010
#endif
using namespace std;

extern Det *rcDETRegion[2][3][4];
extern Det rcDET[NDetMax];
extern Options opt;
extern EUppLow operator++(enum EUppLow &rs, int );
extern ERegion operator++(enum ERegion &rs, int );
extern Etype operator++(enum Etype &rs, int );
extern Edir operator++(enum Edir &rs, int );

/*____________________________________________________________
Qoptions
Date : 7/31/7
Author : Burnham Stokes
*//*! \file Qoptions.cc 
\brief Qoptions is a generic ascii read-in program for handling
program options.

This program currently only manages a few things, but it is intended
that Qoptions will be greatly expanded in the future. 
*/
/*! \class Qoptions \brief Qoptions Qoptions is a generic ascii read-in program for handling
program options.*/
//____________________________________________________________
void Qoptions::Get(char *optionsfile){
	FILE *options;
	char line[256];
	char *word;
	EUppLow upplow;
	ERegion region;
	Etype type;
	//Edir dir;
	
	int maxlevels;	

	int maxchar = 256;
	options = fopen(optionsfile,"r");
	while(!feof(options)){
		fgets(line,maxchar,options);//first line is a placeholder
		if(feof(options))break;
		word = strtok(line," ");
		if(*word == 'u' || *word == 'd'){//detector or general
			switch(*word){//upper or lower detector
				case 'u':
					upplow = w_upper;
					break;
				case 'd':
					upplow = w_lower;
					break;
				default :
					upplow = w_nowhere;
					break;
			}
			word = strtok(NULL," ");
			switch(*word){//detector region
				case '1':
					region = r1;
					break;
				case '2':
					region = r2;
					break;
				case '3':
					region = r3;
					break;
			}
			word = strtok(NULL," ");
			switch(*word){//detector type
				case 'd':
					type = d_drift;
					break;
				case 'g':
					type = d_gem;
					break;
				case 't':
					type = d_trigscint;
					break;
				case 'c':
					type = d_cerenkov;
					break;
	
			}
	
			word = strtok(NULL," ");
			maxlevels = atoi(word);
  			opt.levels[upplow][region-1][type]=maxlevels;
		}
		else{
			switch(*word){
				case '1':
					opt.showEventPattern = 1;
					break;
				case '0':
					opt.showEventPattern = 0;
					break;
			}
			word = strtok(NULL," ");
			switch(*word){
				case '1':
					opt.showMatchingPatterns = 1;
					break;
				case '0':
					opt.showMatchingPatterns = 0;
					break;
			}
			word = strtok(NULL," ");
			switch(*word){
				case '1':
					opt.showMatchedPattern = 1;
					break;
				case '0':
					opt.showMatchedPattern = 0;
					break;
			}
			
		}	
	}
	return;
}
//____________________________________________________________
Qoptions::Qoptions(){
	
}
//____________________________________________________________
