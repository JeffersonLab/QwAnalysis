//
// C++ Interface: QwDelayLine
//
// Description:
//
//
// Author: siyuan yang <sxyang@linux-ehar>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef QWDELAYLINE_H
#define QWDELAYLINE_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <vector>
#include <utility>
using namespace std;

/**
	@author siyuan yang <sxyang@linux-ehar>
*/
class QwDelayLine
{
 public:
  QwDelayLine():Package(-1),Plane(-1),FirstWire(-1),fill(false),processed(false),Ambiguous(-1) {};
  QwDelayLine(const int pk,const int pl,const int firstwire):Package(pk),Plane(pl),FirstWire(firstwire),fill(false),processed(false),Ambiguous(false) {};

  ~QwDelayLine();
  int Package;
  int Plane;
  int FirstWire;

  bool fill;              //whether this delay line has been filled in with information
  bool processed;
  bool Ambiguous;

  vector<vector<int> > Wire;  // indexed by different hits and different wires because of the ambiguity in the same hit
  vector<pair<int,int> > Hitscount;
  vector<pair<double,double> > Windows;
  vector<double> LeftHits;
  vector<double> RightHits;
  static const double TimeStep;
  static const int  WireStep;

  void ProcessHits();
  void Wirenumber(double&);

};

#endif
