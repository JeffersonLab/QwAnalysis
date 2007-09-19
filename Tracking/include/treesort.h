#ifndef TREESORT_H
#define TREESORT_H

#include <iostream>
#include "tree.h"
#include "treesearch.h"

class treesort{

public:
treesort();
~treesort();
double doubletrack;
int good;

int rcTreeConnSort( TreeLine *head, enum ERegion region/*,
		enum EUppLow up_low, enum Etype type,
		enum Edir dir,enum Eorientation orient*/);
double chiweight( TreeLine *tl );
int connectiv( char *ca, int *array, int *isvoid, char size, int idx );
int rcCommonWires( TreeLine *line1, TreeLine *line2 );
int rcCommonWires_r3( TreeLine *line1, TreeLine *line2 );
int connectarray( char *ca, int *array, int *isvoid, char size, int idx );
void bestunconnected( char *ca, int *array, int *isvoid, double *chia,
		 int size, int idx);
int bestconnected( char *ca, int *array, int *isvoid, double *chia,
	       int size, int idx);
int globalconnectiv( char *ca, int *array, int *isvoid, int size, int idx);



int rcPartConnSort( PartTrack *head/*,
		enum EUppLow up_low, enum ERegion region, enum Etype type,
		enum Edir dir,enum Eorientation orient*/);
double ptchiweight( PartTrack *pt );
int rcPTCommonWires( PartTrack *track1, PartTrack *track2 );

private:


};


#endif
