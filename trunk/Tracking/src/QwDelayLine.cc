//
// C++ Implementation: QwDelayLine
//
// Description:
//
//
// Author: siyuan yang <sxyang@linux-ehar>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "QwDelayLine.h"

#include "QwParameterFile.h"

const Double_t QwDelayLine::kTimeStep=2.6;
const Int_t    QwDelayLine::kWireStep=8;

void QwDelayLine::Wirenumber(Double_t& time){
  Double_t f1tdc_resolution_ns = 0.116312881651642913;
  Int_t temp = 0;
  vector<Int_t> tempwire;
  Int_t Guess = (Int_t) (f1tdc_resolution_ns*(time-(Windows.at( 0 ).first+Windows.at( 0 ).second) /2)/kTimeStep);
  // there's two conditions:
  // first, that the value of time-average value of the first window can not be smaller than -2.6;
  // second, the right bound of the last window minus the left bound of the first window should be
  // smaller than Windows.size()*kTimeStep(because we must require that Guess shoulde be smaller than
  // the Windows.size(),otherwise, the first command below Window.at(Guess) will be out of boundary!!)

  if(Guess >= (Int_t) Windows.size()) Guess = (Int_t) Windows.size() - 1;
  if(time < Windows.at(Guess).second)
    {
      for(Int_t i= Guess; i >= 0 ; --i)
	{
	  if(time < Windows.at( i ).second && time >= Windows.at( i ).first)
	    {
	      temp=fFirstWire+i*kWireStep;
	      tempwire.push_back ( temp );
	      Wire.push_back ( tempwire );
	      Ambiguous=false;
	      break;
	    }
	  else if(time < Windows.at( i ).first && time >= Windows.at( i-1 ).second)
	    {
	      temp= fFirstWire + i*kWireStep;
	      tempwire.push_back(temp);
	      tempwire.push_back ( temp-kWireStep );
	      Wire.push_back ( tempwire );
	      Ambiguous=true;
	      break;
	    }

	}
    }
  else
    {
      for(size_t i=Guess+1;i<Windows.size();i++)
	{
	  if (time < Windows.at( i ).second && time >= Windows.at( i ).first)
	    {
	      temp=fFirstWire+i*kWireStep;
	      tempwire.push_back(temp);
	      Wire.push_back(tempwire);
	      Ambiguous=false;
	      break;
	    }
	  else if(time < Windows.at( i ).first && time >= Windows.at( i-1 ).second)
	    {
	      temp= fFirstWire + i*kWireStep;
	      tempwire.push_back(temp);
	      tempwire.push_back(temp-kWireStep);
	      Wire.push_back(tempwire);
	      Ambiguous=true;
	      break;
	    }
	}
    }
}

void QwDelayLine::ProcessHits(Bool_t k){
  //  The loop over the hits is assumming that both lists are ordered forwards in time.
  Double_t delta_t=0;
  Int_t hitscount=LeftHits.size();
  Int_t hitscount_R=RightHits.size();
  Int_t wincount=Windows.size();
  //Int_t first_match=0;

  if(hitscount ==0 || hitscount_R ==0){
  }// do nothing
  else{
    for(Int_t i=0;i<hitscount;i++)
      {
	//for(Int_t j=first_match;j<hitscount_R;j++)
	for(Int_t j=0;j<hitscount_R;j++)
	  {
	    if(k==false)
	      delta_t=-(LeftHits.at( i )-RightHits.at( j ));
	    else delta_t= LeftHits.at( i )-RightHits.at( j );

	    if(delta_t < Windows.at ( 0 ).first)
	      {
		//	std::cout << " delta_t is unphysical,continue" << std::endl;
		continue;
	      }
	    else if(delta_t > Windows.at ( wincount-1 ).second)
	      {
		//	std::cout << " delta_t is unphysical,continue" << std::endl;
		continue;
	      }
	    else
	      {
		Wirenumber(delta_t);
		std::pair<Int_t,Int_t> hitspair(i,j);
		Hitscount.push_back (hitspair);
		//first_match = j+1;
	      }
	  }
      }
  }
  Processed = true;
}




QwDelayLine::~QwDelayLine()
{
}

