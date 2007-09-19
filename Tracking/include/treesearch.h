#ifndef TREESEARCH_H
#define TREESEARCH_H
#include <iostream>
#include "tree.h"

#ifndef TLAYERS
#define TLAYERS 6
#endif

class treesearch{

public:
char **static_pattern;
int  **static_hash;
int    static_maxlevel;
/*int    static_front;*/
int tlayers;

treesearch();
~treesearch();
void wireselection( Hit **x, Hit **X, Hit **xn, Hit**Xn, double maxdist);

void setpoint( double off,double h1, double res1,double h2, double res2,
	  	double width, unsigned bwidth, char *pa, char *pb,
	  	int *hasha, int *hashb);
int TsSetPoint(double detectorwidth, double zdistance, Hit *Ha, Hit *Hb,
	   	char *patterna, char *patternb, int  *hasha, int *hashb,
	   	unsigned binwidth);
int TsSetPoint(double detectorwidth, Hit *H,char *pattern, int  *hash,unsigned binwidth);

int exists( int *newa, int front, int back, TreeLine *treeline);

void TsSearch(shortnode *node, char *pattern[16], int  *hashpat[16],
	 	int maxlevel, int numWires);


private:
void _TsSearch(shortnode *node,int level,int offset,int row_offset,int reverse,int numWires);
void _setpoints(double posStart,double posEnd,double detectorwidth,unsigned binwidth,char *pattern,int *hash); 
void _setpoint(double position, double resolution,double detectorwidth,
	  	unsigned binwidth, char *pattern, int *hash);
};


#endif
