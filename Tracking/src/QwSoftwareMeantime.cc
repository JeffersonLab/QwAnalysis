/**
 *  \file   QwSoftwareMeantime.cc
 *  \brief  
 *  \author Jeong Han Lee, jhlee@jlab.org
 *  \date   Wednesday, March 14 13:46:22 EDT 2012
 */

#include "QwSoftwareMeantime.h"

#if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
ClassImp(MeanTime);
ClassImp(MeanTimeContainer);
#endif


const Int_t MeanTime::fSoftwareMeantimeHitElementID = 3;
const Int_t MeanTime::fSoftwarePositiveHitElementID = 4;
const Int_t MeanTime::fSoftwareNegativeHitElementID = 5;
const Int_t MeanTime::fSoftwareSubtractHitElementID = 6;



MeanTime::MeanTime()
{
  
  fMeanTime      = 0.0; 
  fSubtractTime  = 0.0;

  fPositiveValue = 0.0;
  fNegativeValue = 0.0;

  fDiffHardSoftMeanTime = 0.0; // Hardware MT - Software MT
  fHardWareMeanTime     = 0.0;

  fPositiveHitId = 0;
  fNegativeHitId = 0;
  fMeanTimeId    = 0;
  
  fEventId       = 0;
  fDetectorType  = "";
  fHasValue      = false;
 
};


MeanTime::MeanTime(TString name, Long64_t ev_id, Double_t p_in, Double_t n_in, Int_t p_id, Int_t n_id)
{
  fDetectorType         = name;
  fEventId              = ev_id;
  fDiffHardSoftMeanTime = 0.0; 
  fHardWareMeanTime     = 0.0;
 

  AddPNValues(p_in, n_in, p_id, n_id);
};


MeanTime::~MeanTime()
{
};

void
MeanTime::AddPNValues(Double_t p_value, Double_t n_value, Int_t p_id, Int_t n_id)
{
  fPositiveValue = p_value;
  fNegativeValue = n_value;
  
  fPositiveHitId = p_id;
  fNegativeHitId = n_id;

  fMeanTime      = 0.5*(fPositiveValue + fNegativeValue);
  fSubtractTime  = fPositiveValue - fNegativeValue;

  fHasValue      = true;

  return;
}


void
MeanTime::Print(Bool_t on=true)
{
  if (!on) return;

  TString output = "";
  if     (fDetectorType == "MD") output += "<<";
  else if(fDetectorType == "TS") output += ">>";
  
  output += fDetectorType;
  
  if     (fDetectorType == "MD") output += fPlane;
  else if(fDetectorType == "TS") output += fPlane;
  
  output += Form("-- ID %d ------Event ", fMeanTimeId);
  output += fEventId;
  output += ": HitIndex[";
  output += fPositiveHitId;
  output += ",";
  output += fNegativeHitId;
  output += "]";
  
  if      (fDetectorType == "MD") {
    output += Form( " MDp%+9.2f MDm%+9.2f dMD%+9.2f MDsMT %+10.2f << ---------------------------- <<",
		    fPositiveValue, fNegativeValue, fSubtractTime, fMeanTime);
  }
  else if (fDetectorType == "TS") {
    output += Form( " TSp%+9.2f TSm%+9.2f dTS%+9.2f TSsMT %+10.2f TShMT %+10.2f dTSMT %+8.2f >>",
		    fPositiveValue, fNegativeValue, fSubtractTime, fMeanTime, fHardWareMeanTime, fHardWareMeanTime - fMeanTime);
  }
  
  printf("%s\n", output.Data());
  
  return;
};


Bool_t 
MeanTime::IsInTimeWindow(Double_t time_window)
{
  Bool_t status = false;
  status = ( fabs(fSubtractTime) < time_window ) ? true : false;
  return status;
};


Bool_t 
MeanTime::IsHitIDsMatch()
{
  Bool_t status = false;
  status = ( fPositiveHitId == fNegativeHitId ) ? true : false;
  return status;
};



void
MeanTime::SetHardwareMeanTime(Double_t hardware_meantime)
{
  fHardWareMeanTime     = hardware_meantime;
  fDiffHardSoftMeanTime = fHardWareMeanTime - fMeanTime;
  return;
}



void 
MeanTime::ClearEventData()
{
  fHasValue             = false;

  fMeanTime             = 0.0; 
  fSubtractTime         = 0.0;

  fDiffHardSoftMeanTime = 0.0; 
  fHardWareMeanTime     = 0.0;

  fPositiveValue        = 0.0;
  fNegativeValue        = 0.0;

  fPositiveHitId        = 0;
  fNegativeHitId        = 0;
  fMeanTimeId           = 0;
  
  fEventId              = 0;

  return;
}





MeanTimeContainer::MeanTimeContainer()
{
  // default Time Window is 2000 ns, so
  // accept all TDC data from F1TDCs
  
  fTimeWindowNs = 2000.0;

  fDetectorName = "";
  
  fMeanTimeList = new TObjArray();
  fMeanTimeList -> Clear();
  fMeanTimeList -> SetOwner(kTRUE);
  fNMeanTimes   = 0;
  
  fEventId      = 0;
  fDisableMatchHardwareMeanTime = true;

  for(Int_t i=0;i<7;i++) 
    {
      fPositiveValue[i]         = 0.0;
      fNegativeValue[i]         = 0.0;
      fHardwareMeantimeValue[i] = 0.0;
    }
  
};



MeanTimeContainer::MeanTimeContainer(TString name)
{

  fTimeWindowNs = 2000.0;

  fDetectorName = name;
  
  fMeanTimeList = new TObjArray();
  fMeanTimeList -> Clear();
  fMeanTimeList -> SetOwner(kTRUE);
  fNMeanTimes   = 0;
  
  fEventId      = 0;

  fDisableMatchHardwareMeanTime = true;

  for(Int_t i=0;i<7;i++) 
    {
      fPositiveValue[i]         = 0.0;
      fNegativeValue[i]         = 0.0;
      fHardwareMeantimeValue[i] = 0.0;
    }
  
};


MeanTimeContainer::~MeanTimeContainer()
{
  if(fMeanTimeList) delete fMeanTimeList; fMeanTimeList = NULL;
};


Int_t 
MeanTimeContainer::AddMeanTime(TString name, Long64_t ev_id, Double_t p_in, Double_t n_in, Int_t p_id, Int_t n_id)
{

  Int_t pos = 0;
  MeanTime *temp = NULL;
  temp = new MeanTime(name, ev_id, p_in, n_in, p_id, n_id);

  if(temp->IsInTimeWindow(fTimeWindowNs)) {
    temp -> SetMeanTimeId(fNMeanTimes);
    pos   = fMeanTimeList->AddAtFree(temp) ;
    fNMeanTimes++;
  }
  else {
    delete temp; temp = NULL;
  }

  return pos;
}


Int_t 
MeanTimeContainer::AddMeanTime(TString name, Int_t plane_id, Long64_t ev_id, Double_t p_in, Double_t n_in, Int_t p_id, Int_t n_id)
{

  Int_t pos = 0;
  MeanTime *temp = NULL;
  temp = new MeanTime(name, ev_id, p_in, n_in, p_id, n_id);

  if(temp->IsInTimeWindow(fTimeWindowNs)) {
    temp -> SetMeanTimeId(fNMeanTimes);
    temp -> SetPlane(plane_id);
    pos   = fMeanTimeList->AddAtFree(temp) ;
    fNMeanTimes++;
  }
  else {
    delete temp; temp = NULL;
  }

  return pos;
}


void 
MeanTimeContainer::Add(Double_t p_value, Double_t n_value, Int_t hit_id)
{
  fPositiveValue[hit_id] = p_value;
  fNegativeValue[hit_id] = n_value;

  return;
}


void 
MeanTimeContainer::Add(Double_t p_value[7], Double_t n_value[7])
{
  for(Int_t i=0; i<7; i++)
    {
      fPositiveValue[i] = p_value[i];
      fNegativeValue[i] = n_value[i];
    }

  return;
}

void 
MeanTimeContainer::Add(Int_t element, Int_t hit_number, Double_t time_ns)
{
  if(time_ns!=0.0) {
    if( element==1 ) {
      fPositiveValue[hit_number] = time_ns;
      fNPositive++;
    }
    else if (element == 2) {
      fNegativeValue[hit_number] = time_ns;
      fNNegative++;
    }
    // We don't need this to fill element == 0, because QwHit has hardware mt anyway....
    // else if (element == 0) {
    //   // do we need this? QwHit has hardware mt anyway....
    //   fHardwareMeantimeValue[hit_number] = time_ns;
    //   fNHarewareMeanTimes++;
    // }
  }

  return;
}

void 
MeanTimeContainer::Add(Double_t p_value[7], Double_t n_value[7], Double_t hardware_meantime[7])
{
  for(Int_t i=0; i<7; i++)
    {
      fPositiveValue[i] = p_value[i];
      if(p_value[i]!=0.0) {
	fNPositive++;
      }

      fNegativeValue[i] = n_value[i];
      if(n_value[i]!=0.0) {
	fNNegative++;
      }
      
      fHardwareMeantimeValue[i] = hardware_meantime[i];
      if(hardware_meantime[i]!=0.0) {
	fNHarewareMeanTimes++;
      }
    }

  SetDisableMatchHardwareMeantime(false);

  //  printf("positive %d negative %d and hardware %d\n", fNPositive, fNNegative, fNHarewareMeanTimes);
  return;
}


// ProcessMeanTime function is very inefficient, because
// it uses all hit combinations, even if we don't need them.
// Anyway, this first attempt is good enough to see
// what I want.

// But it would be better to add a better logic to exclude empty entries.
// If so, we will get a bit faster script. 

// Wednesday, February 29 09:20:10 EST 2012, jhlee

void 
MeanTimeContainer::ProcessMeanTime()
{
  
  if (fNPositive == 1 && fNNegative == 1) {
    AddMeanTime(fDetectorName, fPlane, fEventId, fPositiveValue[0], fNegativeValue[0], 0,0);
  }
  else {
    

    Bool_t   local_debug = false;
    Bool_t   GS_Stable_marriage_debug = false;

    Double_t ini      = 100000.0;

    Double_t nst[7][7] = {{ini}}; // [negative][positive]
    Double_t pst[7][7] = {{ini}}; // [positive][negative]

    //    TString p_name[7] = {"p0", "p1", "p2", "p3", "p4", "p5", "p6"};
    //    TString n_name[7] = {"n0", "n1", "n2", "n3", "n4", "n5", "n6"};

    Int_t   p =    0;
    Int_t   n    = 0;
    Int_t   rank = 0;

    Int_t   n_preference_list[7][7] = {{0}};
    Int_t   p_preference_list[7][7] = {{0}};


    // n_preference_list must be built according to negative channel, 
  
    // nst [negative][positive] // abs (subtract)
  

    for(n=0; n<7; n++) 
      {
	for(p=0; p<7; p++) 
	  {
	    nst[n][p] = fabs(fPositiveValue[p] - fNegativeValue[n]);
	    //
	    // if one of them is zero, fill st with unreasonable number ini
	    //
	    if( nst[n][p] == fabs(fPositiveValue[p]) || nst[n][p]== fabs(fNegativeValue[n]) ) {
	      nst[n][p] = ini;
	    }
	    if(local_debug)  printf("neg [%d,%d], nst %f \n", n,p, nst[n][p]);
	  }
	//
	// sort n_preference_list in lowest st at the first place [n][0].
	//
	TMath::Sort(7, nst[n], n_preference_list[n], false);
      
	// if(local_debug) {
	// 	for(j=0; j<maximum_hit_number; j++)
	// 	  {
	// 	    printf("Sort [%d,%d], index sorting %d st sorting %f\n", p,j, n_preference_list[p][j], nst[p][n_preference_list[p][j]]);
	// 	  }
	//      }
      }

    // p_preference_list must be built according to positive channel, 
  
    for(p=0; p<7; p++) 
      {
	for(n=0; n<7; n++) 
	  {
	    pst[p][n] = fabs(fPositiveValue[p] - fNegativeValue[n]);
	    //
	    // if one of them is zero, fill st with unreasonable number ini
	    //
	    if(pst[p][n] == fabs(fPositiveValue[p]) || pst[p][n]== fabs(fNegativeValue[n]) ) {
	      pst[p][n] = ini;
	    }
	    if(local_debug)  printf("pos [%d,%d], pst %f \n", p,n,pst[p][n]);
	  }
	//
	// sort n_preference_list in lowest st at the first place [n][0].
	//
	TMath::Sort(7, pst[p], p_preference_list[p], false);
      
	// if(local_debug) {
	// 	for(j=0; j<maximum_hit_number; j++)
	// 	  {
	// 	    printf("Sort [%d,%d], index sorting %d st sorting %f\n", p,j, n_preference_list[p][j], nst[p][n_preference_list[p][j]]);
	// 	  }
	//      }
      }


    if(local_debug) {
      for(n=0; n<7; n++) 
	{
	  for(p=0; p<7; p++)
	    {
	      printf("Sort [%d,%d], index sorting %d st sorting %f\n", n,p, n_preference_list[n][p], nst[n][p]);
	    }
	}
      printf("\n");
    }


    if(local_debug) {
      for(p=0; p<7; p++) 
	{
	  for(n=0; n<7; n++)
	    {
	      printf("Sort [%d,%d], index sorting %d st sorting %f\n", p,n, p_preference_list[p][n], nst[p][n]);
	    }
	}
      printf("\n");
    }

    // rank of negative channels
    // n_rank[negative channel][rank]
    // we don't need to have rank of positive channels,
    // because positive propose to negative
    //
    Int_t n_rank[7][7] = {{0}};

    for(n=0; n<7; n++)
      {
	for (rank=0; rank<7; rank++) 
	  {
	    n_rank[n][n_preference_list[n][rank]] = rank;
	    //	  printf("n_rank[%d][%d] = %d\n", n, n_preference_list[n][rank], rank);
	  }
      }
  
  
    // p seems to be man, and n seems to be woman.
   
    Int_t fiancee      [7];  // fiancee[p] = n , n with viewpoint of p.
    Int_t suitor       [7];  // suitor [n] = p , p with viewpoint of n.
    Int_t decrease_rank[7]; 
    Int_t getting_engaged = 0; 

    // Initially all p s and all n s are free
    // fiancee[0] = -1 means p 0 is solo

    for(p=0; p<7; p++) 
      {
	decrease_rank[p] =  0; fiancee[p] = -1;
      }
  
    for(n=0; n<7; n++) 
      {
	suitor[n] = -1;
      }
  
    while (getting_engaged < 7)
      {
	// While there is a positive channel p which is free and has not proposed to every negative
	// Chose such a positive channel p which is free or solo
	for (p=0; fiancee[p]!=-1; p++)
	  ;

	if(GS_Stable_marriage_debug) printf(">>> positive channel %d proposing:\n", p);

	while (fiancee[p] == -1) {

	  // Let n be the highest-ranked negative in the preference list of positive.
	  // In the preference list, the first place is the highest-ranked negative number.
	  // decrease_rank[p] = 0 means the highest-ranked negative number
	
	  n = p_preference_list[p][decrease_rank[p]];

	  if(GS_Stable_marriage_debug) printf(" to negative %2d", n);

	  // decrease the rank number, which means preparing the next highest-ranked negative number
	  // if highest-ranked negative will reject the propose of this positive p.
	  decrease_rank[p]++;
	
	  // If negative is free or solo then
	  if (suitor[n] == -1) {
	    if(GS_Stable_marriage_debug) printf(" accepted\t(negative %2d previously free)\n", n);
	    suitor[n] = p; fiancee[p] = n;
	    getting_engaged++;
	  }
	  // Else negative is currently engaged to positive "suitor[n]"
	  else {
	    // if negative prefer positive_old(suitor[n]) to positive
	    // Greater rank means the lowest-ranked pixel.
	    if (n_rank[n][suitor[n]] < n_rank[n][p]) {
	      if(GS_Stable_marriage_debug) printf(" rejected\t(negative %2d prefers %d)\n", n, suitor[n]);
	    }
	    else {
	      // else pixel prefer f to f_old
	      if(GS_Stable_marriage_debug) printf(" accepted\t(negative %2d dumps positive %d)\n", n, suitor[n]);
	      // f_old becomes free
	      fiancee[suitor[n]] = -1;
	      // (f,p) become engaged
	      suitor[n] = p; fiancee[p] = n;
	    }
	  }
	}
      }


    for(p=0; p<7; p++)
      {
	//	if (local_debug) printf("%s %s\n", p_name[p].Data(), n_name[fiancee[p]].Data());
	if(fPositiveValue[p] !=0.0 && fNegativeValue[fiancee[p]]!=0.0) {
	  AddMeanTime(fDetectorName, fPlane, fEventId, fPositiveValue[p], fNegativeValue[fiancee[p]], p, fiancee[p]);
	}
      }

    
    MatchHardwareMeanTime();
  
    // //  Int_t exclude_j = 0;
    // // quick and dirty way
    // for(Int_t i=0; i<7; i++) 
    //   {
    //     for(Int_t j=exclude_j; j<7; j++)
    // 	{
    // 	  subtract = fabs(fPositiveValue[i] - fNegativeValue[j]);
    // 	  meantime = 0.5*(fPositiveValue[i] + fNegativeValue[j]); 
	  
    // 	  if ( subtract > fTimeWindowNs ) continue; // reject a meantime out of a reasonable time window
    // 	  if ( meantime == 0.0          ) continue; // skip  when PositiveValue and NegativeValue are zero
    // 	  if ( (fabs(fPositiveValue[i])==subtract) || (fabs(fNegativeValue[j])==subtract)) continue; // skip to calculate a meantime when one of values is zero. 
    // 	  // if(fDetectorName == "MD") {
    // 	  //   printf("[%d,%d] P %8.2f N %8.2f, subtract %8.2f meantime %8.2f\n", 
    // 	  // 	   i, j, fPositiveValue[i], fNegativeValue[j], subtract, meantime);
    // 	  // }
    // 	  AddMeanTime(fDetectorName, fEventId, fPositiveValue[i], fNegativeValue[j], i,j);
    // 	  exclude_j = j;
    // 	  // if p[0] has its pair m[3], so we can ignore m[0], m[1], and m[2] in next i loop,
    //         // p[1] > p[0] and p[1] - m[0] > p[0] - m[0] > fTimeWindowNs  yes
    // 	  //                 p[1] - m[1] > p[0] - m[1] > fTimeWindowNs  yes
    // 	  //                 p[1] - m[2] > p[0] - m[2] > fTimeWindowNs  yes

    // 	  // And there is one addition case, we should care ....
    // 	  //                 p[1] - m[3] > p[0] - m[3] < fTimeWindowNs ... hmm... I am not sure....
    // 	  //                     alpha  +  p[0] - m[3] < fTimeWindowNs ?
    // 	  //                     alpha  +  p[0] - m[3] > fTimeWindowNs ?
 
    // 	  // p[0]- m[4] could be < fTimeWindowNs also... hmm...
    // 	  //	  break; 
    // 	}
      
    //   }
  
  }

  return;
}

void
MeanTimeContainer::Print(Bool_t on)
{
  if (!on) return;

  printf("%s fNPositive %d fNNegative %d, fNMeanTimes %d fNHarewareMeanTimes %d\n", 
	 fDetectorName.Data(), fNPositive, fNNegative, fNMeanTimes, fNHarewareMeanTimes);
  
  TObjArrayIter next(fMeanTimeList);
  TObject* obj = NULL;
  MeanTime * mean_time  = NULL;
  
  while ( (obj = next()) )
    {
      mean_time = (MeanTime *) obj;
      mean_time->Print();
    }
  
  return;
};



MeanTime*
MeanTimeContainer::GetMeanTimeObject(Int_t index)
{
  
  Int_t mt_index = 0;
  TObjArrayIter next(fMeanTimeList);
  TObject* obj = NULL;
  MeanTime * mean_time  = NULL;
  
  while ( (obj = next()) )
    {
      mean_time = (MeanTime *) obj;
      mt_index = mean_time->GetMeanTimeId();
      //   printf("index %d and mt_index %d\n", index, mt_index);
      if( mt_index == index ) {
	//	printf("can we see\n");
	return mean_time;
      }
    }
  
  return NULL;
};


void
MeanTimeContainer::MatchHardwareMeanTime()
{
  // we don't use this function into the main analyzer, it was used in F1TDCGoodMT.C script
  // to check the reliability of a method to build software meantime for MD 
  // See  https://qweak.jlab.org/elog/Detector/58
  // Wednesday, March 14 13:57:37 EDT 2012, jhlee

  if( fDisableMatchHardwareMeanTime ) {
    //    printf("%s  disable .....\n\n\n", fDetectorName.Data());
    return;
  }
  // else {
  //   printf("%s enable ....\n\n\n", fDetectorName.Data());
  // }
 
  Int_t mt_index = 0;
  
  TObjArrayIter next(fMeanTimeList);
  TObject* obj = NULL;
  MeanTime * mean_time  = NULL;
  
  Double_t h_mt     = 0.0;
  Double_t h_mt_pos = 0.0;
  Double_t h_mt_neg = 0.0;

  if( IsHarewareMatchSoftware() ) {
    while ( (obj = next()) )
      {
	mean_time = (MeanTime *) obj;
	mt_index = mean_time->GetMeanTimeId();
	h_mt = fHardwareMeantimeValue[mt_index];
	mean_time->SetHardwareMeanTime(h_mt);
      }
  }
  else {
 
    if(fNPositive < fNHarewareMeanTimes && fNNegative < fNHarewareMeanTimes) {

      while ( (obj = next()) )
	{
	  mean_time = (MeanTime *) obj;

	  Double_t s_mt = 0.0;

	  s_mt     = mean_time -> GetMeanTime();
	  mt_index = mean_time->GetMeanTimeId();
	  
	  Int_t idx = mt_index;

	  while(1) 
	    {
	      h_mt = fHardwareMeantimeValue[idx];
	      //	      printf("%d %f %f \n", idx, s_mt, h_mt);
	      if ( fabs(s_mt - h_mt) <50 ) {
		break;
	      }
	      idx++;
	      
	    }
	  mean_time->SetHardwareMeanTime(h_mt);

	}
    }
    else if (fNPositive == 2 && fNNegative == 1 && fNHarewareMeanTimes == 2){

      // ugly... only one meantime because of "if condition"
      while ( (obj = next()) )
	{
	  mean_time = (MeanTime *) obj;
	  Double_t s_mt = 0.0;
	  s_mt = mean_time -> GetMeanTime();
	  h_mt = fHardwareMeantimeValue[0];
	  if ( fabs(s_mt - h_mt) < 50 ) {
	    mean_time->SetHardwareMeanTime(h_mt);
	  }
	  else {
	    mean_time->SetHardwareMeanTime(fHardwareMeantimeValue[1]);
	  }
	}
      
    }
    else {
      while ( (obj = next()) )
	{
	  mean_time = (MeanTime *) obj;
	  Int_t negative_id = mean_time -> GetNegativeHitId();
	  Int_t positive_id = mean_time -> GetPositiveHitId();
	  
	  h_mt_pos = fHardwareMeantimeValue[positive_id];
	  h_mt_neg = fHardwareMeantimeValue[negative_id];
	  
	  if (h_mt_neg!=0.0) {
	    h_mt = h_mt_neg;
	  }
	  else {
	    // check whether TShMT[positive_id] is zero or not
	    // if it exists, use it as TShMT
	    if(h_mt_pos!=0.0) {
	      h_mt = h_mt_pos;
	    }
	    else {
	      h_mt = 0.0;
	    }
	  }
	  mean_time->SetHardwareMeanTime(h_mt);
	}
    }
  }

  
  return;
}

Double_t
MeanTimeContainer::GetMeanTime(Int_t index)
{

  Int_t mt_index = 0;
  TObjArrayIter next(fMeanTimeList);
  TObject* obj = NULL;
  MeanTime * mean_time  = NULL;


  while ( (obj = next()) )
    {
      mean_time = (MeanTime *) obj;
      mt_index = mean_time->GetMeanTimeId();
      if( mt_index == index ) {
	return mean_time->GetMeanTime();
      }
    }

  return 0.0;
};

Bool_t  
MeanTimeContainer::IsHarewareMatchSoftware()
{
  Bool_t status = false;
  status = ( fNHarewareMeanTimes == fNMeanTimes ) ? true : false;
  return status;
}



void 
MeanTimeContainer::ClearEventData()
{

  fMeanTimeList-> Clear();

  fNMeanTimes         = 0;
  fEventId            = 0;

  fNPositive          = 0;
  fNNegative          = 0;
  fNHarewareMeanTimes = 0;

  for(Int_t i=0;i<7;i++) 
    {
      fPositiveValue[i]         = 0.0;
      fNegativeValue[i]         = 0.0;
      fHardwareMeantimeValue[i] = 0.0;
    }

  TObjArrayIter next(fMeanTimeList);
  TObject* obj = NULL;
  MeanTime * mean_time  = NULL;

  while ( (obj = next()) )
    {
      mean_time = (MeanTime *) obj;
      mean_time->ClearEventData();
    }

  return;
};
