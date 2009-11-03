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
///
/// \ingroup QwTrackingAnl
class QwDelayLine
{
 public:
  QwDelayLine():Package(-1),Plane(-1),Direction(-1),FirstWire(-1),t0_max(0),fill(false),processed(false),Ambiguous(-1),change_max(false) {};
  QwDelayLine(const int pk,const int pl,const int dir,const int firstwire):Package(pk),Plane(pl),Direction(dir),FirstWire(firstwire),t0_max(0),fill(false),processed(false),Ambiguous(false),change_max(false) {};

  ~QwDelayLine();
  int Package;
  int Plane;
  int Direction;
  int FirstWire;
  double t0_max;


  bool fill;              //whether this delay line has been filled in with information
  bool processed;
  bool Ambiguous;
  bool change_max;

  vector<vector<int> > Wire;  // indexed by different hits and different wires because of the ambiguity in the same hit
  vector<pair<int,int> > Hitscount;
  vector<pair<double,double> > Windows;
  vector<double> LeftHits;
  vector<double> RightHits;
  static const double TimeStep;
  static const int  WireStep;

  void ProcessHits(bool k);
  void Wirenumber(double&);

};

#endif
