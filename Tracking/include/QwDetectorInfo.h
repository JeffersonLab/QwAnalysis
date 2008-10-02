#ifndef QWDETECTORINFO_H
#define QWDETECTORINFO_H

#include<vector>

#include "QwTypes.h"

class QwDetectorInfo{
  ///
  ///  Tracking detector information class.  This will be used in an array
  ///  indexed by the package, plane, and channel, 
  ///
public:
  


	//identification info for readout channels. Filled at load time.
	int fCrate; //ROC number
	int fModule; //F1TDC slot number or module index
	int fChannel; //channel number


	//reference channel index in list of reference channels (most prob. filled at load time)
	int fReferenceChannelIndex;
	

	//list of active hits by absolute hit number from QwHit array. filled for each event; cleared after each event.
	std::vector<int> fHitID;


	void SetElectronics(int crt,int mdl,int chn)
	  {
	    fCrate=crt;
	    fModule=mdl;
	    fChannel=chn;
	    fReferenceChannelIndex=1;		
	  }

	Bool_t IsHit()
	  {
	    return (!fHitID.empty());
	  }
	
	Int_t GetNumHits(){return fHitID.size();};
	
	void ClearHits()
	  {
	    if (!fHitID.empty())
	      fHitID.clear();
	  }

	void PushHit(int time)
	  {
	    fHitID.push_back(time);
	  }
};

#endif
