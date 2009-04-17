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

#include "DelayLine.h"

const double TimeStep=2.6;
const int  WireStep=8;

void DelayLine::Wirenumber(double& time){
 int temp=0;
 vector<int> tempwire;
 int Guess=int((time-(Windows.at( 0 ).first+Windows.at( 0 ).second) /2)/TimeStep);          //there's two conditions:first,that the value of time-average value of the first window can not be smaller than -2.6;second, the right bound of the last window minus the left bound of the first window should be smaller than Windows.size()*TimeStep(because we must require that Guess shoulde be smaller than the Windows.size(),otherwise, the first command below Window.at(Guess) will be out of boundary!!)
 if(time < Windows.at(Guess).second)
 {
  for(int i= Guess; i >= 0 ; --i)
   {
	if(time < Windows.at( i ).second && time >= Windows.at( i ).first)
	{
		temp=FirstWire+i*WireStep;
		tempwire.push_back ( temp );
		Wire.push_back ( tempwire );
		Ambiguous=false;
		break;
	}
	else if(time < Windows.at( i ).first && time >= Windows.at( i-1 ).second)
	{
		temp= FirstWire + i*WireStep;
		tempwire.push_back(temp);
		tempwire.push_back ( temp-WireStep );
		Wire.push_back ( tempwire );
		Ambiguous=true;
		break;
	}

   } 
 }
  else
	{
		for(int i=Guess+1;i<Windows.size();i++)
		{
			if (time < Windows.at( i ).second && time >= Windows.at( i ).first)
			{
				temp=FirstWire+i*WireStep;
				tempwire.push_back(temp);
				Wire.push_back(tempwire);
				Ambiguous=false;
				break;
			}
			else if(time < Windows.at( i ).first && time >= Windows.at( i-1 ).second)
			{
				temp= FirstWire + i*WireStep;
				tempwire.push_back(temp);
				tempwire.push_back(temp-WireStep);
				Wire.push_back(tempwire);
				Ambiguous=true;
				break;
			}
		}
	}
}

void DelayLine::ProcessHits(){
	//  The loop over the hits is assumming that both lists are ordered forwards in time.
 int hitscount=LeftHits.size();
 int hitscount_R=RightHits.size();
 int wincount=Windows.size();
 int first_match = 0;
 for(int i=0;i<hitscount;i++)
  {
	for(int j=first_match;j<hitscount_R;j++)
	{
		double delta_t=LeftHits.at( i )-RightHits.at( j );
			
		if(delta_t < Windows.at ( 0 ).first)
		{
			std::cout << " delta_t is unphysical,continue" << std::endl;
			break;
		}
		else if(delta_t > Windows.at ( wincount-1 ).second)
		{
			std::cout << " delta_t is unphysical,continue" << std::endl;
			continue;
		}
		else
		{
			Wirenumber(delta_t);
			pair<int,int> hitspair(i,j);
			Hitscount.push_back (hitspair);
			first_match = j+1;
		}
	}
 }
	processed = true;
}



DelayLine::~DelayLine()
{
}

