// Author : Jeong Han Lee
//          jhlee@jlab.org
//
// Date   : Tuesday, February 28 14:59:01 EST 2012
// 
//         This script is used to calculate or find good software mentime of MDs and TSs,
//         because of F1TDC multihit structure
// 
//         One needs qwroot instead of root
//         $ qwroot 
//         Qw-Root [0] .L F1TDCGoodMT.C
//         Qw-Root [1] GoodMTMultiHits(1,2,-1, 10000, 50, 20, 1, 1, filename, 102)
//
//         MD1, TS2, event 0:10000, delta MD1 < 50, delta TS2 <20, MD debug on, TS debug on, rootfile, binnumber
//         in order to save debug outputs in a file
//         Qw-Root [3] GoodMtMultiHits(1,2,-1,-1,50,20,1,1); > debug.log
//
//         GoodMTMultiHits(2,2,-1,-1, 2000, 2000, 1,1, "MDTrigger_15027.root"); > MD2_2000_TS2_2000_Run15027.log
//
//
//         0.0.2 Thursday, March  1 16:50:53 EST 2012 , jhlee
//
//               added a difference between hardware meantime and software meantime of TS
//                     in order to check this script quality.
//                     if we see always constant difference between software and hardware
//                     we might say, this scripts is good enough to use to build software meantime
//                     of MD, which doesn't have hardware meantime. 
//
//               added some boolean conditions to suppress Draw or Canvas when a given event range is small
//        0.0.3 Wednesday, March  7 17:02:51 EST 2012, jhlee
//              Added MeanTime and MeanTimeContainer class into the main analyzer, so don't need to re-define here
//                                           


//#include "F1TDCGoodMT.h"


// ClassImp(MeanTime)
// ClassImp(MeanTimeContainer)

// MeanTime::MeanTime()
// {
  
//   fMeanTime      = 0.0; 
//   fSubtractTime  = 0.0;

//   fPositiveValue = 0.0;
//   fNegativeValue = 0.0;

//   fDiffHardSoftMeanTime = 0.0; // Hardware MT - Software MT
//   fHardWareMeanTime     = 0.0;

//   fPositiveHitId = 0;
//   fNegativeHitId = 0;
//   fMeanTimeId    = 0;
  
//   fEventId       = 0;
//   fDetectorType  = "";
//   fHasValue      = false;
// };


// MeanTime::MeanTime(TString name, Long64_t ev_id, Double_t p_in, Double_t n_in, Int_t p_id, Int_t n_id)
// {
//   fDetectorType = name;
//   fEventId      = ev_id;
//   fDiffHardSoftMeanTime = 0.0; 
//   fHardWareMeanTime     = 0.0;
//   AddPNValues(p_in, n_in, p_id, n_id);

// };


// MeanTime::~MeanTime()
// {
// };

// void
// MeanTime::AddPNValues(Double_t p_value, Double_t n_value, Int_t p_id, Int_t n_id)
// {
//   fPositiveValue = p_value;
//   fNegativeValue = n_value;
  
//   fPositiveHitId = p_id;
//   fNegativeHitId = n_id;

//   fMeanTime      = 0.5*(fPositiveValue + fNegativeValue);
//   fSubtractTime  = fPositiveValue - fNegativeValue;

//   fHasValue      = true;
//   return;
// }


// void
// MeanTime::Print(Bool_t on)
// {

//   if(on) {
//     TString output = "";
//     if     (fDetectorType == "MD") output += "<<";
//     else if(fDetectorType == "TS") output += ">>";
    
//     output += fDetectorType;
    
//     if     (fDetectorType == "MD") output += "<<";
//     else if(fDetectorType == "TS") output += ">>";
    
//     output += Form("-- ID %d ------Event ", fMeanTimeId);
//     output += fEventId;
//     output += ": HitIndex[";
//     output += fPositiveHitId;
//     output += ",";
//     output += fNegativeHitId;
//     output += "]";
    
//     if      (fDetectorType == "MD") {
//       output += Form( " MDp%+9.2f MDm%+9.2f dMD%+9.2f MDsMT %+10.2f << ---------------------------- <<",
// 		      fPositiveValue, fNegativeValue, fSubtractTime, fMeanTime);
//     }
//     else if (fDetectorType == "TS") {
//       output += Form( " TSp%+9.2f TSm%+9.2f dTS%+9.2f TSsMT %+10.2f TShMT %+10.2f dTSMT %+8.2f >>",
// 		      fPositiveValue, fNegativeValue, fSubtractTime, fMeanTime, fHardWareMeanTime, fHardWareMeanTime - fMeanTime);
//     }
    
//     std::cout << output << std::endl;
//   }
//   return;
// };


// Bool_t 
// MeanTime::IsInTimeWindow(Double_t time_window)
// {
//   Bool_t status = false;
//   status = ( fabs(fSubtractTime) < time_window ) ? true : false;
//   return status;
// }


// Bool_t 
// MeanTime::IsHitIDsMatch()
// {
//   Bool_t status = false;
//   status = ( fPositiveHitId == fNegativeHitId ) ? true : false;
//   return status;
// }






// void
// MeanTime::SetHardwareMeanTime(Double_t hardware_meantime)
// {
//   fHardWareMeanTime     = hardware_meantime;
//   fDiffHardSoftMeanTime = fHardWareMeanTime - fMeanTime;
//   return;
// }



// // void 
// // MeanTime::Clear()
// // {
// //   return;
// // }





// MeanTimeContainer::MeanTimeContainer()
// {

//   fTimeWindowNs = 0.0;

//   fDetectorName = "";
  
//   fMeanTimeList = new TObjArray();
//   fMeanTimeList -> Clear();
//   fMeanTimeList -> SetOwner(kTRUE);
//   fNMeanTimes   = 0;
  
//   fEventId      = 0;
  
//   for(Int_t i=0;i<7;i++) 
//     {
//       fPositiveValue[i]         = 0.0;
//       fNegativeValue[i]         = 0.0;
//       fHardwareMeantimeValue[i] = 0.0;
//     }
  
// };



// MeanTimeContainer::MeanTimeContainer(TString name)
// {

//   fTimeWindowNs = 0.0;

//   fDetectorName = name;
  
//   fMeanTimeList = new TObjArray();
//   fMeanTimeList -> Clear();
//   fMeanTimeList -> SetOwner(kTRUE);
//   fNMeanTimes   = 0;
  
//   fEventId      = 0;
  
//   for(Int_t i=0;i<7;i++) 
//     {
//       fPositiveValue[i]         = 0.0;
//       fNegativeValue[i]         = 0.0;
//       fHardwareMeantimeValue[i] = 0.0;
//     }
  
// };


// MeanTimeContainer::~MeanTimeContainer()
// {
//   if(fMeanTimeList) delete fMeanTimeList; fMeanTimeList = NULL;
// };


// Int_t 
// MeanTimeContainer::AddMeanTime(TString name, Long64_t ev_id, Double_t p_in, Double_t n_in, Int_t p_id, Int_t n_id)
// {

//   Int_t pos = 0;
//   //  Double_t h_mt_pos = 0.0;
//   //  Double_t h_mt_neg = 0.0;
//   //  Double_t h_mt     = 0.0;

//   MeanTime *temp = NULL;
//   temp = new MeanTime(name, ev_id, p_in, n_in, p_id, n_id);

//   if(temp->IsInTimeWindow(fTimeWindowNs)) {
//     temp -> SetMeanTimeId(fNMeanTimes);
//     pos   = fMeanTimeList->AddAtFree(temp) ;
//     fNMeanTimes++;
//   }
//   else {
//     delete temp; temp = NULL;
//   }

//   return pos;
// }

// void 
// MeanTimeContainer::Add(Double_t p_value, Double_t n_value, Int_t hit_id)
// {
//   fPositiveValue[hit_id] = p_value;
//   fNegativeValue[hit_id] = n_value;

//   return;
// }


// void 
// MeanTimeContainer::Add(Double_t p_value[7], Double_t n_value[7])
// {
//   for(Int_t i=0; i<7; i++)
//     {
//       fPositiveValue[i] = p_value[i];
//       fNegativeValue[i] = n_value[i];
//     }

//   return;
// }


// void 
// MeanTimeContainer::Add(Double_t p_value[7], Double_t n_value[7], Double_t hardware_meantime[7])
// {
//   for(Int_t i=0; i<7; i++)
//     {
//       fPositiveValue[i]         = p_value[i];
//       if(p_value[i]!=0.0) fNPositive++;
//       fNegativeValue[i]         = n_value[i];
//       if(n_value[i]!=0.0) fNNegative++;
//       fHardwareMeantimeValue[i] = hardware_meantime[i];
//       if(hardware_meantime[i]!=0.0) {
// 	fNHarewareMeanTimes++;
//       }
//     }

//   //  printf("positive %d negative %d and hardware %d\n", fNPositive, fNNegative, fNHarewareMeanTimes);
//   return;
// }

// // ProcessMeanTime function is very inefficient, because
// // it uses all hit combinations, even if we don't need them.
// // Anyway, this first attempt is good enough to see
// // what I want.

// // But it would be better to add a better logic to exclude empty entries.
// // If so, we will get a bit faster script. 

// // Wednesday, February 29 09:20:10 EST 2012, jhlee

// void 
// MeanTimeContainer::ProcessMeanTime()
// {

//   Bool_t   local_debug = false;
//   Bool_t   GS_Stable_marriage_debug = false;

//   Double_t ini      = 100000.0;

//   //  Double_t mt[7]=  {ini};

//   // Bool_t zero_flag = false;
//   // Int_t count_zero = 0;

//   // for(Int_t i =0; i<7; i++) 
//   //   {
//   //     mt[i] =  0.5*(fPositiveValue[i] + fNegativeValue[i]); 
//   //     zero_flag = (mt[i] == 0.0) ? true : false;
//   //     if(zero_flag) count_zero++;
//   //   }

//   // if(local_debug) printf("count zero %d\n", count_zero);

//   // // reject when all entries are zero.
//   // if(count_zero == 5) return;
  
//   // if(count_zero == 5) {
//   //   AddMeanTime(fDetectorName, fEventId, fPositiveValue[0], fNegativeValue[0], 0, 0);
//   // }



//   Double_t nst[7][7] = {{ini}}; // [negative][positive]
//   Double_t pst[7][7] = {{ini}}; // [positive][negative]

//   Char_t *p_name[7] = {"p0", "p1", "p2", "p3", "p4", "p5", "p6"};
//   Char_t *n_name[7] = {"n0", "n1", "n2", "n3", "n4", "n5", "n6"};

//   Int_t   p =    0;
//   Int_t   n    = 0;
//   Int_t   rank = 0;

//   Int_t   n_preference_list[7][7] = {{0}};
//   Int_t   p_preference_list[7][7] = {{0}};


//   // n_preference_list must be built according to negative channel, 
  
//   // nst [negative][positive] // abs (subtract)
  

//   for(n=0; n<7; n++) 
//     {
//       for(p=0; p<7; p++) 
// 	{
// 	  nst[n][p] = fabs(fPositiveValue[p] - fNegativeValue[n]);
// 	  //
// 	  // if one of them is zero, fill st with unreasonable number ini
// 	  //
// 	  if( nst[n][p] == fabs(fPositiveValue[p]) || nst[n][p]== fabs(fNegativeValue[n]) ) {
// 	    nst[n][p] = ini;
// 	  }
// 	  if(local_debug)  printf("neg [%d,%d], nst %f \n", n,p, nst[n][p]);
//       }
//       //
//       // sort n_preference_list in lowest st at the first place [n][0].
//       //
//       TMath::Sort(7, nst[n], n_preference_list[n], false);
      
//       // if(local_debug) {
//       // 	for(j=0; j<maximum_hit_number; j++)
//       // 	  {
//       // 	    printf("Sort [%d,%d], index sorting %d st sorting %f\n", p,j, n_preference_list[p][j], nst[p][n_preference_list[p][j]]);
//       // 	  }
//       //      }
//     }

//   // p_preference_list must be built according to positive channel, 
  
//   for(p=0; p<7; p++) 
//     {
//       for(n=0; n<7; n++) 
// 	{
// 	  pst[p][n] = fabs(fPositiveValue[p] - fNegativeValue[n]);
// 	  //
// 	  // if one of them is zero, fill st with unreasonable number ini
// 	  //
// 	  if(pst[p][n] == fabs(fPositiveValue[p]) || pst[p][n]== fabs(fNegativeValue[n]) ) {
// 	    pst[p][n] = ini;
// 	  }
// 	  if(local_debug)  printf("pos [%d,%d], pst %f \n", p,n,pst[p][n]);
//       }
//       //
//       // sort n_preference_list in lowest st at the first place [n][0].
//       //
//       TMath::Sort(7, pst[p], p_preference_list[p], false);
      
//       // if(local_debug) {
//       // 	for(j=0; j<maximum_hit_number; j++)
//       // 	  {
//       // 	    printf("Sort [%d,%d], index sorting %d st sorting %f\n", p,j, n_preference_list[p][j], nst[p][n_preference_list[p][j]]);
//       // 	  }
//       //      }
//     }


//   if(local_debug) {
//     for(n=0; n<7; n++) 
//       {
//   	for(p=0; p<7; p++)
//   	  {
//   	    printf("Sort [%d,%d], index sorting %d st sorting %f\n", n,p, n_preference_list[n][p], nst[n][p]);
//   	  }
//       }
//     printf("\n");
//   }


//   if(local_debug) {
//     for(p=0; p<7; p++) 
//       {
//   	for(n=0; n<7; n++)
//   	  {
//   	    printf("Sort [%d,%d], index sorting %d st sorting %f\n", p,n, p_preference_list[p][n], nst[p][n]);
//   	  }
//       }
//     printf("\n");
//   }

//   // rank of negative channels
//   // n_rank[negative channel][rank]
//   // we don't need to have rank of positive channels,
//   // because positive propose to negative
//   //
//   Int_t n_rank[7][7] = {{0}};

//   for(n=0; n<7; n++)
//     {
//       for (rank=0; rank<7; rank++) 
//   	{
//   	  n_rank[n][n_preference_list[n][rank]] = rank;
// 	  //	  printf("n_rank[%d][%d] = %d\n", n, n_preference_list[n][rank], rank);
//   	}
//     }
  
  
//   // p seems to be man, and n seems to be woman.
   
//   Int_t fiancee      [7];  // fiancee[p] = n , n with viewpoint of p.
//   Int_t suitor       [7];  // suitor [n] = p , p with viewpoint of n.
//   Int_t decrease_rank[7]; 
//   Int_t getting_engaged = 0; 

//   // Initially all p s and all n s are free
//   // fiancee[0] = -1 means p 0 is solo

//   for(p=0; p<7; p++) 
//     {
//       decrease_rank[p] =  0; fiancee[p] = -1;
//     }
  
//   for(n=0; n<7; n++) 
//     {
//       suitor[n] = -1;
//     }
  
//   while (getting_engaged < 7)
//     {
//       // While there is a positive channel p which is free and has not proposed to every negative
//       // Chose such a positive channel p which is free or solo
//       for (p=0; fiancee[p]!=-1; p++)
//        	;

//       if(GS_Stable_marriage_debug) printf(">>> positive channel %d proposing:\n", p);

//       while (fiancee[p] == -1) {

// 	// Let n be the highest-ranked negative in the preference list of positive.
// 	// In the preference list, the first place is the highest-ranked negative number.
// 	// decrease_rank[p] = 0 means the highest-ranked negative number
	
// 	n = p_preference_list[p][decrease_rank[p]];

// 	if(GS_Stable_marriage_debug) printf(" to negative %2d", n);

// 	// decrease the rank number, which means preparing the next highest-ranked negative number
// 	// if highest-ranked negative will reject the propose of this positive p.
// 	decrease_rank[p]++;
	
// 	// If negative is free or solo then
// 	if (suitor[n] == -1) {
// 	  if(GS_Stable_marriage_debug) printf(" accepted\t(negative %2d previously free)\n", n);
// 	  suitor[n] = p; fiancee[p] = n;
// 	  getting_engaged++;
// 	}
// 	// Else negative is currently engaged to positive "suitor[n]"
// 	else {
// 	  // if negative prefer positive_old(suitor[n]) to positive
// 	  // Greater rank means the lowest-ranked pixel.
// 	  if (n_rank[n][suitor[n]] < n_rank[n][p]) {
// 	    if(GS_Stable_marriage_debug) printf(" rejected\t(negative %2d prefers %d)\n", n, suitor[n]);
// 	  }
// 	  else {
// 	    // else pixel prefer f to f_old
// 	    if(GS_Stable_marriage_debug) printf(" accepted\t(negative %2d dumps positive %d)\n", n, suitor[n]);
// 	    // f_old becomes free
// 	    fiancee[suitor[n]] = -1;
// 	    // (f,p) become engaged
// 	    suitor[n] = p; fiancee[p] = n;
// 	  }
// 	}
//       }
//     }


//   for(p=0; p<7; p++)
//     {
//       if (local_debug)printf("%s %s\n", p_name[p], n_name[fiancee[p]]);
//       if(fPositiveValue[p] !=0.0 && fNegativeValue[fiancee[p]]!=0.0) {
// 	AddMeanTime(fDetectorName, fEventId, fPositiveValue[p], fNegativeValue[fiancee[p]], p, fiancee[p]);
//       }
//     }

    

//   MatchHardwareMeanTime();
  
//   // //  Int_t exclude_j = 0;
//   // // quick and dirty way
//   // for(Int_t i=0; i<7; i++) 
//   //   {
//   //     for(Int_t j=exclude_j; j<7; j++)
//   // 	{
//   // 	  subtract = fabs(fPositiveValue[i] - fNegativeValue[j]);
//   // 	  meantime = 0.5*(fPositiveValue[i] + fNegativeValue[j]); 
	  
//   // 	  if ( subtract > fTimeWindowNs ) continue; // reject a meantime out of a reasonable time window
//   // 	  if ( meantime == 0.0          ) continue; // skip  when PositiveValue and NegativeValue are zero
//   // 	  if ( (fabs(fPositiveValue[i])==subtract) || (fabs(fNegativeValue[j])==subtract)) continue; // skip to calculate a meantime when one of values is zero. 
//   // 	  // if(fDetectorName == "MD") {
//   // 	  //   printf("[%d,%d] P %8.2f N %8.2f, subtract %8.2f meantime %8.2f\n", 
//   // 	  // 	   i, j, fPositiveValue[i], fNegativeValue[j], subtract, meantime);
//   // 	  // }
//   // 	  AddMeanTime(fDetectorName, fEventId, fPositiveValue[i], fNegativeValue[j], i,j);
//   // 	  exclude_j = j;
//   // 	  // if p[0] has its pair m[3], so we can ignore m[0], m[1], and m[2] in next i loop,
//   //         // p[1] > p[0] and p[1] - m[0] > p[0] - m[0] > fTimeWindowNs  yes
//   // 	  //                 p[1] - m[1] > p[0] - m[1] > fTimeWindowNs  yes
//   // 	  //                 p[1] - m[2] > p[0] - m[2] > fTimeWindowNs  yes

//   // 	  // And there is one addition case, we should care ....
//   // 	  //                 p[1] - m[3] > p[0] - m[3] < fTimeWindowNs ... hmm... I am not sure....
//   // 	  //                     alpha  +  p[0] - m[3] < fTimeWindowNs ?
//   // 	  //                     alpha  +  p[0] - m[3] > fTimeWindowNs ?
 
//   // 	  // p[0]- m[4] could be < fTimeWindowNs also... hmm...
//   // 	  //	  break; 
//   // 	}
      
//   //   }
  

//   return;
// }

// void
// MeanTimeContainer::Print(Bool_t on)
// {
//   if(on) {
//     TObjArrayIter next(fMeanTimeList);
//     TObject* obj = NULL;
//     MeanTime * mean_time  = NULL;
    
    
//     while ( (obj = next()) )
//       {
// 	mean_time = (MeanTime *) obj;
// 	mean_time->Print();
//       }
//   }
  
//   return;
// };



// MeanTime*
// MeanTimeContainer::GetMeanTimeObject(Int_t index)
// {

  
//   Int_t mt_index = 0;
//   TObjArrayIter next(fMeanTimeList);
//   TObject* obj = NULL;
//   MeanTime * mean_time  = NULL;


//   while ( (obj = next()) )
//     {
//       mean_time = (MeanTime *) obj;
//       mt_index = mean_time->GetMeanTimeId();
//       //   printf("index %d and mt_index %d\n", index, mt_index);
//       if( mt_index == index ) {
// 	//	printf("can we see\n");
// 	return mean_time;
//       }
//     }

//   return NULL;
// };


// void
// MeanTimeContainer::MatchHardwareMeanTime()
// {
//   Int_t mt_index = 0;


//   TObjArrayIter next(fMeanTimeList);
//   TObject* obj = NULL;
//   MeanTime * mean_time  = NULL;

//   Double_t h_mt     = 0.0;

//   //  Int_t mt_pos_id = 0;
//   //  Int_t mt_neg_id = 0;
//   Double_t h_mt_pos = 0.0;
//   Double_t h_mt_neg = 0.0;

//   if( IsHarewareMatchSoftware() ) {
//     while ( (obj = next()) )
//       {
// 	mean_time = (MeanTime *) obj;
// 	mt_index = mean_time->GetMeanTimeId();
// 	h_mt = fHardwareMeantimeValue[mt_index];
// 	mean_time->SetHardwareMeanTime(h_mt);
//       }
//   }
//   else {
 
//     if(fNPositive < fNHarewareMeanTimes && fNNegative < fNHarewareMeanTimes) {

//       while ( (obj = next()) )
// 	{
// 	  mean_time = (MeanTime *) obj;

// 	  Double_t s_mt = 0.0;

// 	  s_mt     = mean_time -> GetMeanTime();
// 	  mt_index = mean_time->GetMeanTimeId();
	  
// 	  Int_t idx = mt_index;
// 	  while(1) 
// 	    {
// 	      h_mt = fHardwareMeantimeValue[idx];
// 	      //	      printf("%d %f %f \n", idx, s_mt, h_mt);
// 	      if ( fabs(s_mt - h_mt) <50 ) {
// 		break;
// 	      }
// 	      idx++;
	      
// 	    }
// 	  mean_time->SetHardwareMeanTime(h_mt);

// 	}
//     }
//     else if (fNPositive == 2 && fNNegative == 1 && fNHarewareMeanTimes == 2){

//       // extream ugly... only one meantime because of "if condition"
//       while ( (obj = next()) )
// 	{
// 	  mean_time = (MeanTime *) obj;
// 	  Double_t s_mt = 0.0;
// 	  s_mt = mean_time -> GetMeanTime();
// 	  h_mt = fHardwareMeantimeValue[0];
// 	  if ( fabs(s_mt - h_mt) < 50 ) {
// 	    mean_time->SetHardwareMeanTime(h_mt);
// 	  }
// 	  else {
// 	    mean_time->SetHardwareMeanTime(fHardwareMeantimeValue[1]);
// 	  }
// 	}
      
//     }
//     else {
//       while ( (obj = next()) )
// 	{
// 	  mean_time = (MeanTime *) obj;
// 	  Int_t negative_id = mean_time -> GetNegativeHitId();
// 	  Int_t positive_id = mean_time -> GetPositiveHitId();
	  
// 	  h_mt_pos = fHardwareMeantimeValue[positive_id];
// 	  h_mt_neg = fHardwareMeantimeValue[negative_id];
	  
// 	  if (h_mt_neg!=0.0) {
// 	    h_mt = h_mt_neg;
// 	  }
// 	  else {
// 	    // check whether TShMT[positive_id] is zero or not
// 	    // if it exists, use it as TShMT
// 	    if(h_mt_pos!=0.0) {
// 	      h_mt = h_mt_pos;
// 	    }
// 	    else {
// 	      h_mt = 0.0;
// 	    }
// 	  }
// 	  mean_time->SetHardwareMeanTime(h_mt);
// 	}
//     }
//   }

  
//   return;
// }

// Double_t
// MeanTimeContainer::GetMeanTime(Int_t index)
// {

//   Int_t mt_index = 0;
//   TObjArrayIter next(fMeanTimeList);
//   TObject* obj = NULL;
//   MeanTime * mean_time  = NULL;


//   while ( (obj = next()) )
//     {
//       mean_time = (MeanTime *) obj;
//       mt_index = mean_time->GetMeanTimeId();
//       if( mt_index == index ) {
// 	return mean_time->GetMeanTime();
//       }
//     }

//   return 0.0;
// };

// Bool_t  
// MeanTimeContainer::IsHarewareMatchSoftware()
// {
//   Bool_t status = false;
//   status = ( fNHarewareMeanTimes == fNMeanTimes ) ? true : false;
//   return status;
// }



// void 
// MeanTimeContainer::Clear()
// {
//   // for(Int_t i=0;i<7;i++) 
//   //   {
//   //     fMeanTime[i].Clear();
//   //   }
//   return;
// };




void
Style() 
{
  gStyle->SetHistMinimumZero(kTRUE);
  gStyle->SetPalette(1,0);
  gROOT->ForceStyle();
  return;
}


void
GoodMTMultiHits(Int_t md_plane, Int_t ts_plane, 
		Int_t event_number_start=-1, 
		Int_t event_number_end=-1,
		Int_t MD_MT_time_window_ns = 50,
		Int_t TS_MT_time_window_ns = 20,
		Bool_t ts_debug = false, Bool_t md_debug = false, 
		Bool_t canvas_on=true,
		TString filename="14955.root", 
		Int_t nbin=102, 
		Int_t time_shift_ns=0) {
  
  Bool_t local_debug = false;

  Style();

  TString path = gSystem->Getenv("QW_ROOTFILES");
  
  if (path.IsNull()) { 
    printf("$QW_ROOTFILES is not defined, please define them first\n");
    return NULL;
  }

  path += "/"; //  just in case...
  path += filename;
  
  TFile afile(path);
  if (afile.IsZombie()) { 
    printf("Error opening file\n"); 
    return NULL;
  }

  TString output_filename="MT";

  TString canvas_name = "";
  
  output_filename += "_MD";
  output_filename += md_plane;
  output_filename += "_";
  output_filename += MD_MT_time_window_ns;
  output_filename += "_TS";
  output_filename += ts_plane;
  output_filename += "_";
  output_filename += TS_MT_time_window_ns;

  canvas_name     = output_filename;

  output_filename += "_";
  output_filename += filename;

  
  if(canvas_on) 
    {
      TCanvas *c1 = new TCanvas(canvas_name, 
				Form("%s Stacked Histograms", canvas_name.Data()),
				100,
				100,
				1024,
				612);
    }
  
  TString name ="";
  

  TObjArray HistList(0);

  THStack *mt_hs[6];

  
  mt_hs[0] = new THStack(Form("MD%dsMTHitStack", md_plane),
			 Form("MD%dsMT F1TDC Multi-Hit Contribution Histogram", md_plane)
			 );
  mt_hs[1] = new THStack(Form("TS%dsMTHitStack", ts_plane),
			 Form("TS%dsMT F1TDC Multi-Hit Contribution Histogram", ts_plane)
			 );
  mt_hs[2] = new THStack(Form("TS%dhMTHitStack", ts_plane),
			 Form("TS%dhMT F1TDC Multi-Hit Contribution Histogram", ts_plane)
			 );

  mt_hs[3] = new THStack(Form("dMD%dpmHitStack", md_plane),
			 Form("MD%p - MD%dm F1TDC Multi-Hit Contribution Histogram", md_plane, md_plane)
			 );
  mt_hs[4] = new THStack(Form("dTS%dpmHitStack", ts_plane),
			 Form("TS%dp - TS%dm F1TDC Multi-Hit Contribution Histogram", ts_plane, ts_plane)
			 );
  mt_hs[5] = new THStack(Form("dTS%dMTsHitStack", ts_plane),
			 Form("TS%dhMT -TS%dsMT F1TDC Multi-Hit Contribution Histogram", ts_plane, ts_plane)
			 );
  
  
  for(Int_t idx=0; idx<6; idx++)
    {
      HistList.Add(mt_hs[idx]);
    }
  
  TH1D* mdhist[7];
  TH1D* tshist[7];
  TH1D* tsmthist[7];

  TH1D* dmdhist[7];
  TH1D* dtshist[7];
  TH1D* dtsmthist[7];

  //
  for(Int_t idx=0; idx <7;idx++)
    {// 2200, 1100, 550
      mdhist[idx]   = new TH1D(Form("MDsMT_hitid_%d", idx), Form("MD sMT HitId %d", idx), nbin, -950+time_shift_ns, 1400+time_shift_ns);
      tshist[idx]   = new TH1D(Form("TSsMT_hitid_%d", idx), Form("TS sMT HitId %d", idx), nbin, -950+time_shift_ns, 1400+time_shift_ns);
      tsmthist[idx] = new TH1D(Form("TShMT_hitid_%d", idx), Form("TS hMT HitId %d", idx), nbin, -950+time_shift_ns, 1400+time_shift_ns);
   
      dmdhist[idx]   = new TH1D(Form("dMDpm_hitid_%d", idx), Form("MDp - MDm  HitId %d", idx),    nbin, -2000+time_shift_ns, 2000+time_shift_ns);
      dtshist[idx]   = new TH1D(Form("dTSpm_hitid_%d", idx), Form("TSp - TSm  HitId %d", idx),    nbin, -2000+time_shift_ns, 2000+time_shift_ns);
      dtsmthist[idx] = new TH1D(Form("dTSMT_hitid_%d", idx), Form("TShMT - TSsMT HitId %d", idx), nbin, -2000+time_shift_ns, 2000+time_shift_ns);

      // To see data with 0.001ns bin width https://qweak.jlab.org/elog/Detector/58
      //      dtsmthist[idx] = new TH1D(Form("dTSMT_hitid_%d", idx), Form("TShMT - TSsMT HitId %d", idx), 20000, 35, 55); // 10/100000 = 1/10000 : 0.001 ns
      HistList.Add(mdhist[idx]);
      HistList.Add(tshist[idx]);
      HistList.Add(tsmthist[idx]);

      HistList.Add(dmdhist[idx]);
      HistList.Add(dtshist[idx]);
      HistList.Add(dtsmthist[idx]);
      
    } 
  


  TTree*   event_tree  = (TTree*) afile.Get("event_tree");
  TBranch* ev_branch   = event_tree->GetBranch("events");
  QwEvent* qw_event    = 0; 
  QwHit*   qwhit       = 0;

  Long64_t num_entries = event_tree->GetEntries();
  //  Long64_t ini_event   = 0;
  Long64_t localEntry  = 0;
  Int_t    nhit        = 0;


  ev_branch -> SetAddress(&qw_event);
 

  if (event_number_start ==-1) {
    event_number_start = 0;
  }
  if (event_number_end != -1) {
    num_entries = event_number_end;
  }

  Double_t ini = 0.0;

  MeanTimeContainer *mt_container = NULL;
  
  // MD time window is selected from Rob's talk
  // Typical event collects light for ~40ns
  // loose time window 50 ns
  // https://qweak.jlab.org/doc-private/ShowDocument?docid=1234
  // 50
  //  Int_t MD_MT_time_window_ns = 50;

  // TS time window is selected from Katherine's report 
  // 9.36 ns / 160 cm, so 13.75 ns / 220 cm 
  // at x =190cm, 11.19 ns/ 160 cm, so 15.39 +- 1.25 ns
  // * strict window should be 17 ns
  // * loose  window  20 ns
  //https://qweak.jlab.org/doc-private/ShowDocument?docid=1220
  // Int_t TS_MT_time_window_ns = 20;



  for (Long64_t i=event_number_start; i<num_entries; i++)
    {//;
      localEntry = event_tree->LoadTree(i);
      event_tree->GetEntry(localEntry);
      
      if(i % 1000 == 0) {
	printf(" Total events %d events process so far : %d\n", num_entries, i);
      }
      
      //    printf("\n");
      nhit = qw_event->GetNumberOfHits();
      
      

      Double_t tsp[7]  = {ini};
      Double_t tsm[7]  = {ini};
      Double_t tsmt[7] = {ini};

      Double_t mdp[7]  = {ini};
      Double_t mdm[7]  = {ini};

      Int_t fregion    = 0;
      Int_t fplane     = 0;
      Int_t felement   = 0;
      Int_t fhitnumber = 0;

      Double_t timens  = 0.0;
      
      Int_t ts_max_hit = 0;
      Int_t md_max_hit = 0;
    
      for(Int_t hit_idx=0; hit_idx<nhit; hit_idx++) 
	{//;;

	  qwhit   = qw_event->GetHit(hit_idx);
	  fregion = qwhit->GetRegion();

	  // Trigger Scintillator is Region 4 in QwHit
	  if (fregion == 4 ) { 
	    fplane     = qwhit->GetPlane();
	    if (fplane != ts_plane)   continue;
	    else {
	      felement   = qwhit->GetElement();
	      fhitnumber = qwhit->GetHitNumber(); 
	      
	      if (fhitnumber > ts_max_hit ) ts_max_hit = fhitnumber;
	      timens =  qwhit->GetTimeNs() +time_shift_ns;
	      if     (felement == 1) tsp[fhitnumber] = timens;
	      else if(felement == 2) tsm[fhitnumber] = timens;
	      else if(felement == 0) {
		tsmt[fhitnumber] = timens;
		tsmthist[fhitnumber] -> Fill(timens);
	      }
	    }
	  }
	  // Main Detector is Region 5 in QwHit
	  else if (fregion == 5 ) {
	    fplane     = qwhit->GetPlane();
	    if (fplane != md_plane)   continue;
	    else {
	      felement   = qwhit->GetElement();
	      fhitnumber = qwhit->GetHitNumber(); 
	      timens =  qwhit->GetTimeNs() +time_shift_ns;
	      if(fhitnumber > md_max_hit ) md_max_hit = fhitnumber;

	      if     (felement == 1) mdp[fhitnumber] = timens;
	      else if(felement == 2) mdm[fhitnumber] = timens;
	    }
	    
	  }
	  else {
	    continue;
	  }
	}//;;

       

      Int_t ts_max_hit_range = ts_max_hit+1;

      for(Int_t idx=0; idx<ts_max_hit_range; idx++) 
	//      for(Int_t idx=0; idx<7; idx++) 
	{//;;

	  if (tsp[idx] == ini && tsm[idx] == ini && tsmt[idx] == ini) continue;

	  Double_t software_mt_ts  = 0.0;
	  Double_t software_del_ts = 0.0;

	  if(ts_debug) {
	    printf(">>TS>> NHits%4d in Event %8d: HitOrder[%d,%d] TSp%+9.2f TSm%+9.2f", 
		   nhit, i, ts_max_hit,  idx,tsp[idx], tsm[idx]);
	  }

	  if (tsp[idx]!=ini && tsm[idx]!=ini) {
	    software_mt_ts  = 0.5*(tsp[idx]+tsm[idx]);
	    software_del_ts = tsp[idx] - tsm[idx];
	    if (ts_debug) printf(" dTS%+9.2f TSsMT %+10.2f", software_del_ts, software_mt_ts);
	  }
	  else {
	    software_mt_ts = 0.0;
	    if (ts_debug) printf(" >>---------- >>--------------");
	  }

	 
	  if(software_mt_ts!=0.0) {
	    
	    if (ts_debug )  {
	      printf(" TShMT %+10.2f dTSMT %+8.2f >>\n", tsmt[idx], tsmt[idx]-software_mt_ts);
	    }
	  }
	  else {

	    if (ts_debug )  {
	      printf(" TShMT %+10.2f >>------------ >>\n", tsmt[idx]);
	    }
	  }
	}//;;



      mt_container = new MeanTimeContainer("TS");
      mt_container -> SetEventId(i);
      mt_container -> SetTimeWindow(TS_MT_time_window_ns);
      mt_container -> Add(tsp, tsm, tsmt);
      //   mt_container -> SetEnableMatchHardwareMeantime(false);
      mt_container -> ProcessMeanTime();


      MeanTime* ts_mt_time= NULL;

      for(Int_t ts_size=0; ts_size < mt_container->SoftwareMTSize(); ts_size++)
	{
	  ts_mt_time = mt_container->GetMeanTimeObject(ts_size);
	  ts_mt_time -> Print(local_debug);
	  tshist [ts_size]   -> Fill(ts_mt_time->GetMeanTime());
	  dtshist[ts_size]   -> Fill(ts_mt_time->GetSubtractTime());
	  dtsmthist[ts_size] -> Fill(ts_mt_time->GetDiffHardSoftMeanTime());
	  
	}

      delete mt_container; mt_container = NULL;

      
      Int_t md_max_hit_range = md_max_hit+1;

      for(Int_t idx=0; idx<md_max_hit_range; idx++) 
      	{//;;

	  if (mdp[idx] == ini && mdm[idx] == ini) continue;

      	  Double_t software_mt_md  = 0.0;
      	  Double_t software_del_md = 0.0;

      	  if (md_debug) {
	    printf("<<MD<< NHits%4d in Event %8d: HitOrder[%d,%d] MDp%+9.2f MDm%+9.2f", 
		   nhit, i, md_max_hit, idx, mdp[idx], mdm[idx]);
	  }

      	  if (mdp[idx]!=ini && mdm[idx]!=ini) {
      	    //	    printf("event %18d id %d mdp %+8.2f mdm%+8.2f mdsmt ", i, idx, mdp[idx], mdm[idx]);
      	    software_mt_md  = 0.5*(mdp[idx]+mdm[idx]);
      	    software_del_md = mdp[idx]-mdm[idx];
      	    if (md_debug) printf(" dMD%+9.2f MDsMT %+10.2f", software_del_md, software_mt_md);
      	  }
      	  else {
      	    if(md_debug)  printf(" <<---------- <<--------------");

      	  }
      	  if (md_debug)   printf(" <<-------------- <<------------ <<\n");

      	}//;;


   
      mt_container = new MeanTimeContainer("MD");
      mt_container -> SetEventId(i);
      mt_container -> SetTimeWindow(MD_MT_time_window_ns);
      mt_container -> Add(mdp, mdm);
      mt_container -> ProcessMeanTime();

      MeanTime* md_mt_time= NULL;

      for(Int_t md_size=0; md_size < mt_container->SoftwareMTSize(); md_size++)
	{
	  md_mt_time = mt_container->GetMeanTimeObject(md_size);
	  md_mt_time -> Print(local_debug);
	  mdhist [md_size] -> Fill(md_mt_time->GetMeanTime());
	  dmdhist[md_size] -> Fill(md_mt_time->GetSubtractTime());
	  
	}

      delete mt_container; mt_container = NULL;

    }//; Long64_t i=0; i<num_entries....
  
  
  for(Int_t idx=0; idx <7;idx++)
    {
      mdhist  [idx] -> SetDirectory(0);
      mdhist  [idx] -> SetMarkerStyle(21);
      mt_hs     [0] -> Add(mdhist[idx]);   
   
      tshist  [idx] -> SetDirectory(0);
      tshist  [idx] -> SetMarkerStyle(21);
      mt_hs     [1] -> Add(tshist[idx]);

      tsmthist[idx] -> SetDirectory(0);
      tsmthist[idx] -> SetMarkerStyle(21);
      mt_hs     [2] -> Add(tsmthist[idx]);

      dmdhist [idx] -> SetDirectory(0);
      dmdhist [idx] -> SetMarkerStyle(21);
      mt_hs     [3] -> Add(dmdhist[idx]);   
   
      dtshist [idx] -> SetDirectory(0);
      dtshist [idx] -> SetMarkerStyle(21);
      mt_hs     [4] -> Add(dtshist[idx]);

      dtsmthist[idx] -> SetDirectory(0);
      dtsmthist[idx] -> SetMarkerStyle(21);
      mt_hs      [5] -> Add(dtsmthist[idx]);

    }

  afile.Close();


  // Int_t color[7]= {2,3,4,6,7,5,43};

  // for (Int_t idx = 0; i<7; i++) 
  //   {
  //     //      color[idx] = gROOT->GetColor(int_color[idx]);

  // mdhist[idx] -> SetFillColor((Color_t) color[idx]);
  // mdhist[idx] -> SetMarkerColor((Color_t) color[idx]);
      
  // tshist[idx] -> SetFillColor((Color_t) color[idx]);
  // tshist[idx] -> SetMarkerColor((Color_t) color[idx]);
      
  // tsmthist[idx] -> SetFillColor((Color_t) color[idx]);
  // tsmthist[idx] -> SetMarkerColor((Color_t) color[idx]);

  // dmdhist[idx] -> SetFillColor((Color_t) color[idx]);
  // dmdhist[idx] -> SetMarkerColor((Color_t) color[idx]);
      
  // dtshist[idx] -> SetFillColor((Color_t) color[idx]);
  // dtshist[idx] -> SetMarkerColor((Color_t) color[idx]);
      
  // dtsmthist[idx] -> SetFillColor((Color_t) color[idx]);
  // dtsmthist[idx] -> SetMarkerColor((Color_t) color[idx]);
    
  //    }

  
  Bool_t md_plot_flag   = (mdhist  [0]->GetEntries()!=0) ? true : false;
  Bool_t ts_plot_flag   = (tshist  [0]->GetEntries()!=0) ? true : false;
  Bool_t tsmt_plot_flag = (tsmthist[0]->GetEntries()!=0) ? true : false;
  
  
  if (md_plot_flag || ts_plot_flag || tsmt_plot_flag) 
    {
      
      mdhist[0] -> SetFillColor(2);
      mdhist[0] -> SetMarkerColor(2);
      
      tshist[0] -> SetFillColor(2);
      tshist[0] -> SetMarkerColor(2);
      
      tsmthist[0] -> SetFillColor(2);
      tsmthist[0] -> SetMarkerColor(2);
      
      dmdhist[0] -> SetFillColor(2);
      dmdhist[0] -> SetMarkerColor(2);
      
      dtshist[0] -> SetFillColor(2);
      dtshist[0] -> SetMarkerColor(2);
      
      dtsmthist[0] -> SetFillColor(2);
      dtsmthist[0] -> SetMarkerColor(2);
      
      
      mdhist[1] -> SetFillColor(3);
      mdhist[1] -> SetMarkerColor(3);
      
      tshist[1] -> SetFillColor(3);
      tshist[1] -> SetMarkerColor(3);
      
      tsmthist[1] -> SetFillColor(3);
      tsmthist[1] -> SetMarkerColor(3);
      
      dmdhist[1] -> SetFillColor(3);
      dmdhist[1] -> SetMarkerColor(3);
      
      dtshist[1] -> SetFillColor(3);
      dtshist[1] -> SetMarkerColor(3);
      
      dtsmthist[1] -> SetFillColor(3);
      dtsmthist[1] -> SetMarkerColor(3);
      
      
      
      mdhist[2] -> SetFillColor(4);
      mdhist[2] -> SetMarkerColor(4);
      
      tshist[2] -> SetFillColor(4);
      tshist[2] -> SetMarkerColor(4);
      
      tsmthist[2] -> SetFillColor(4);
      tsmthist[2] -> SetMarkerColor(4);
      
      dmdhist[2] -> SetFillColor(4);
      dmdhist[2] -> SetMarkerColor(4);
      
      dtshist[2] -> SetFillColor(4);
      dtshist[2] -> SetMarkerColor(4);
      
      dtsmthist[2] -> SetFillColor(4);
      dtsmthist[2] -> SetMarkerColor(4);
      
      
      
      
      mdhist[3] -> SetFillColor(6);
      mdhist[3] -> SetMarkerColor(6);
      
      tshist[3] -> SetFillColor(6);
      tshist[3] -> SetMarkerColor(6);
      
      tsmthist[3] -> SetFillColor(6);
      tsmthist[3] -> SetMarkerColor(6);
      
      dmdhist[3] -> SetFillColor(6);
      dmdhist[3] -> SetMarkerColor(6);
      
      dtshist[3] -> SetFillColor(6);
      dtshist[3] -> SetMarkerColor(6);
      
      dtsmthist[3] -> SetFillColor(6);
      dtsmthist[3] -> SetMarkerColor(6);
      
      
      
      
      mdhist[4] -> SetFillColor(7);
      mdhist[4] -> SetMarkerColor(7);
      
      tshist[4] -> SetFillColor(7);
      tshist[4] -> SetMarkerColor(7);
      
      tsmthist[4] -> SetFillColor(7);
      tsmthist[4] -> SetMarkerColor(7);
      
      dmdhist[4] -> SetFillColor(7);
      dmdhist[4] -> SetMarkerColor(7);
      
      dtshist[4] -> SetFillColor(7);
      dtshist[4] -> SetMarkerColor(7);
      
      dtsmthist[4] -> SetFillColor(7);
      dtsmthist[4] -> SetMarkerColor(7);
      
      

      
      mdhist[5] -> SetFillColor(5);
      mdhist[5] -> SetMarkerColor(5);
      
      tshist[5] -> SetFillColor(5);
      tshist[5] -> SetMarkerColor(5);
  
      tsmthist[5] -> SetFillColor(5);
      tsmthist[5] -> SetMarkerColor(5);
      
      dmdhist[5] -> SetFillColor(5);
      dmdhist[5] -> SetMarkerColor(5);
      
      dtshist[5] -> SetFillColor(5);
      dtshist[5] -> SetMarkerColor(5);
      
      dtsmthist[5] -> SetFillColor(5);
      dtsmthist[5] -> SetMarkerColor(5);
      
      
      
      
      
      
      mdhist[6] -> SetFillColor(43);
      mdhist[6] -> SetMarkerColor(43);
  
      tshist[6] -> SetFillColor(43);
      tshist[6] -> SetMarkerColor(43);
      
      tsmthist[6] -> SetFillColor(43);
      tsmthist[6] -> SetMarkerColor(43);
      
      dmdhist[6] -> SetFillColor(43);
      dmdhist[6] -> SetMarkerColor(43);
      
      dtshist[6] -> SetFillColor(43);
      dtshist[6] -> SetMarkerColor(43);
      
      dtsmthist[6] -> SetFillColor(43);
      dtsmthist[6] -> SetMarkerColor(43);




      if (canvas_on)
	{
	  TLegend *leg = NULL;
  
	  c1->Divide(3,2);
	  if(md_plot_flag) {

	    c1->cd(1);
 
	    gPad->SetGridy();
	    gPad->SetLogy();
	    mt_hs[0] -> Draw("goff");
	    mt_hs[0] -> GetXaxis() -> SetTitle("Time[ns]");
	    mt_hs[0] -> Draw();
	    leg = new TLegend(0.6,0.7,0.99,0.92,NULL,"brNDC");
    
	    leg->SetNColumns(2);
	    leg->SetBorderSize(1);
	    leg->SetTextFont(62);
	    leg->SetLineColor(1);
	    leg->SetLineStyle(1);
	    leg->SetLineWidth(1);
	    leg->SetFillColor(0);
	    leg->SetFillStyle(1001);
    
	    //   leg->SetHeader("Stack Histogram");
	    leg->AddEntry(mdhist[0],"1st Hits","f");
	    leg->AddEntry(mdhist[1],"2nd Hits","f");
	    leg->AddEntry(mdhist[2],"3rd Hits","f");
	    leg->AddEntry(mdhist[3],"4th Hits","f"); 
	    leg->AddEntry(mdhist[4],"5th Hits","f");
	    leg->AddEntry(mdhist[5],"6th Hits","f");
	    leg->AddEntry(mdhist[6],"7th Hits","f");
	    leg->Draw();
	    gPad->Update();
 
 
	    c1->cd(4);
	    gPad->SetGridy();
	    gPad->SetLogy();
	    mt_hs[0]->Draw("nostack");
	    // leg->SetHeader("Non Stack Histogram");
	    // leg->Draw();
	    gPad->Update();
	  }//if(md_plot_flag) {

	  if(ts_plot_flag) {

	    c1->cd(2);
	    gPad->SetGridy();
	    gPad->SetLogy();
	    mt_hs[1] -> Draw("goff");
	    mt_hs[1] -> GetXaxis() -> SetTitle("Time[ns]");
	    mt_hs[1] -> Draw();
	    // TLegend *leg2 = new TLegend(0.6,0.7,0.99,0.92,NULL,"brNDC");
	    // leg2->SetHeader("Stack Histogram");
	    // leg2->SetNColumns(2);
	    // leg2->AddEntry(tshist[0],"1st Hits","f");
	    // leg2->AddEntry(tshist[1],"2nd Hits","f");
	    // leg2->AddEntry(tshist[2],"3rd Hits","f");
	    // leg2->AddEntry(tshist[3],"4th Hits","f"); 
	    // leg2->AddEntry(tshist[4],"5th Hits","f");
	    // leg2->AddEntry(tshist[5],"6th Hits","f");
	    // leg2->AddEntry(tshist[6],"7th Hits","f");
	    // leg2->Draw();
	    gPad->Update();

	    c1->cd(5);
	    gPad->SetGridy();
	    gPad->SetLogy();
	    mt_hs[1]->Draw("nostack");
	    // leg2->SetHeader("Non Stack Histogram");
	    // leg2->Draw();
	    gPad->Update();
	    c1-> Modified();
	    c1-> Update();

	  }
  
	  if(tsmt_plot_flag) {
	    c1->cd(3);
	    gPad->SetGridy();
	    gPad->SetLogy();
	    mt_hs[2] -> Draw("goff");
	    mt_hs[2] -> GetXaxis() -> SetTitle("Time[ns]");
	    mt_hs[2] -> Draw();
	    // TLegend *leg3 = new TLegend(0.6,0.7,0.99,0.92,NULL,"brNDC");
	    // leg3->SetHeader("Stack Histogram");
	    // leg3->SetNColumns(2);
	    // leg3->AddEntry(tshist[0],"1st Hits","f");
	    // leg3->AddEntry(tshist[1],"2nd Hits","f");
	    // leg3->AddEntry(tshist[2],"3rd Hits","f");
	    // leg3->AddEntry(tshist[3],"4th Hits","f"); 
	    // leg3->AddEntry(tshist[4],"5th Hits","f");
	    // leg3->AddEntry(tshist[5],"6th Hits","f");
	    // leg3->AddEntry(tshist[6],"7th Hits","f");
	    // leg3->Draw();
	    gPad->Update();
	    
	    c1->cd(6);
	    gPad->SetGridy();
	    gPad->SetLogy();
	    mt_hs[2]->Draw("nostack");
	    // leg3->SetHeader("Non Stack Histogram");
	    // leg3->Draw();
	    gPad->Update();
	    
	  }
	  
	  c1-> Modified();
	  c1-> Update();

	}

    }
  
  //  if(md_plot_flag && ts_plot_flag && tsmt_plot_flag) 
  //    {
      TFile f(Form("%s", output_filename.Data()),"recreate");
      HistList.Write();
      //    }
  
	
      
    

  return;
}



void HT_test()
{
  // These event numbers are selected to good match between software and hardware meantime
  // in order to develop a better logic to handle *all* possible cases.


  static const Int_t MAXNUMBER = 17;
  UInt_t event_number[MAXNUMBER] = 
    {
      26686, 
      73779, 
      161312, 
      123058, 
      151860, 
      205353, 
      130266, 
      1191062,
      1047585,
      1905451,
      1847953,
      1802703,
      2007625,
      1802703,
      27071, // extreme case.. :-(
      125499,
      1903348
    };
  
  for (Int_t i=0; i< MAXNUMBER; i++)
    {
      printf("--------------- Event %d\n", event_number[i]);
      GoodMTMultiHits(1,2, event_number[i], event_number[i]+1, 50, 20, 1, 1, 0);
      printf("\n");
    }
}


// These are wrong/good matches between TSsMT and TShMT
//
// >>TS>> NHits  10 in Event    26686: HitOrder[1,0] TSp  -172.14 TSm  -194.48 dTS   +22.33 TSsMT    -183.31 TShMT    -137.13 dTSMT   +46.18 >>
// >>TS>> NHits  10 in Event    26686: HitOrder[1,1] TSp  +412.68 TSm  +401.28 dTS   +11.40 TSsMT    +406.98 TShMT    +453.27 dTSMT   +46.29 >>
// >>TS>>-- ID 0 ------Event    26686: HitIndex[1,1] TSp  +412.68 TSm  +401.28 dTS   +11.40 TSsMT    +406.98 TShMT    -137.13 dTSMT  -544.11 >>
// Good
// >>TS>>-- ID 0 ------Event    26686: HitIndex[1,1] TSp  +412.68 TSm  +401.28 dTS   +11.40 TSsMT    +406.98 TShMT    +453.27 dTSMT   +46.29 >>


// >>TS>> NHits  63 in Event    73779: HitOrder[1,0] TSp  -433.50 TSm  -461.18 dTS   +27.68 TSsMT    -447.34 TShMT    -401.16 dTSMT   +46.18 >>
// >>TS>> NHits  63 in Event    73779: HitOrder[1,1] TSp  -181.33 TSm  -185.17 dTS    +3.84 TSsMT    -183.25 TShMT    -137.02 dTSMT   +46.23 >>
// >>TS>>-- ID 0 ------Event    73779: HitIndex[1,1] TSp  -181.33 TSm  -185.17 dTS    +3.84 TSsMT    -183.25 TShMT    -401.16 dTSMT  -217.91 >>
// Good
// >>TS>>-- ID 0 ------Event    73779: HitIndex[1,1] TSp  -181.33 TSm  -185.17 dTS    +3.84 TSsMT    -183.25 TShMT    -137.02 dTSMT   +46.23 >>


// >>TS>> NHits  11 in Event   161312: HitOrder[1,0] TSp  -184.24 TSm  -672.17 dTS  +487.93 TSsMT    -428.21 TShMT    -622.39 dTSMT  -194.18 >>
// >>TS>> NHits  11 in Event   161312: HitOrder[1,1] TSp    +0.00 TSm  -182.49 >>---------- >>-------------- TShMT    -137.25 >>------------ >>
// >>TS>>-- ID 0 ------Event   161312: HitIndex[0,1] TSp  -184.24 TSm  -182.49 dTS    -1.74 TSsMT    -183.37 TShMT    -622.39 dTSMT  -439.02 >>
// Good
// >>TS>>-- ID 0 ------Event   161312: HitIndex[0,1] TSp  -184.24 TSm  -182.49 dTS    -1.74 TSsMT    -183.37 TShMT    -137.25 dTSMT   +46.12 >>



// >>TS>> NHits  20 in Event   123058: HitOrder[1,0] TSp  -169.47 TSm  -197.15 dTS   +27.68 TSsMT    -183.31 TShMT    -137.13 dTSMT   +46.18 >>
// >>TS>> NHits  20 in Event   123058: HitOrder[1,1] TSp   +76.30 TSm   +61.65 dTS   +14.66 TSsMT     +68.97 TShMT    +115.15 dTSMT   +46.18 >>
// >>TS>>-- ID 0 ------Event   123058: HitIndex[1,1] TSp   +76.30 TSm   +61.65 dTS   +14.66 TSsMT     +68.97 TShMT    -137.13 dTSMT  -206.11 >>
// Good
// >>TS>>-- ID 0 ------Event   123058: HitIndex[1,1] TSp   +76.30 TSm   +61.65 dTS   +14.66 TSsMT     +68.97 TShMT    +115.15 dTSMT   +46.18 >>



// >>TS>> NHits  30 in Event   151860: HitOrder[1,0] TSp  -173.19 TSm  -193.78 dTS   +20.59 TSsMT    -183.48 TShMT    -137.25 dTSMT   +46.23 >>
// >>TS>> NHits  30 in Event   151860: HitOrder[1,1] TSp  +153.30 TSm  +151.32 dTS    +1.98 TSsMT    +152.31 TShMT    +198.78 dTSMT   +46.47 >>
// >>TS>>-- ID 0 ------Event   151860: HitIndex[1,1] TSp  +153.30 TSm  +151.32 dTS    +1.98 TSsMT    +152.31 TShMT    -137.25 dTSMT  -289.56 >>
// Good
// >>TS>>-- ID 0 ------Event   151860: HitIndex[1,1] TSp  +153.30 TSm  +151.32 dTS    +1.98 TSsMT    +152.31 TShMT    +198.78 dTSMT   +46.47 >>


// >>TS>> NHits  13 in Event   205353: HitOrder[3,0] TSp  -183.77 TSm  -183.19 dTS    -0.58 TSsMT    -183.48 TShMT    -137.25 dTSMT   +46.23 >>
// >>TS>> NHits  13 in Event   205353: HitOrder[3,1] TSp  +172.26 TSm  +203.31 dTS   -31.06 TSsMT    +187.79 TShMT    +233.79 dTSMT   +46.00 >>
// >>TS>> NHits  13 in Event   205353: HitOrder[3,2] TSp  +951.21 TSm  +950.97 dTS    +0.23 TSsMT    +951.09 TShMT    +997.50 dTSMT   +46.41 >>
// >>TS>> NHits  13 in Event   205353: HitOrder[3,3] TSp    +0.00 TSm +1176.97 >>---------- >>-------------- TShMT      +0.00 >>------------ >>
// >>TS>>-- ID 0 ------Event   205353: HitIndex[0,0] TSp  -183.77 TSm  -183.19 dTS    -0.58 TSsMT    -183.48 TShMT    -137.25 dTSMT   +46.23 >>
// >>TS>>-- ID 1 ------Event   205353: HitIndex[2,2] TSp  +951.21 TSm  +950.97 dTS    +0.23 TSsMT    +951.09 TShMT    +233.79 dTSMT  -717.30 >>
// Good
// >>TS>>-- ID 0 ------Event   205353: HitIndex[0,0] TSp  -183.77 TSm  -183.19 dTS    -0.58 TSsMT    -183.48 TShMT    -137.25 dTSMT   +46.23 >>
// >>TS>>-- ID 1 ------Event   205353: HitIndex[2,2] TSp  +951.21 TSm  +950.97 dTS    +0.23 TSsMT    +951.09 TShMT    +997.50 dTSMT   +46.41 >>



// >>TS>> NHits  39 in Event   130266: HitOrder[1,0] TSp  +181.33 TSm  -207.27 dTS  +388.60 TSsMT     -12.97 TShMT    -137.02 dTSMT  -124.05 >>
// >>TS>> NHits  39 in Event   130266: HitOrder[1,1] TSp    +0.00 TSm  +187.61 >>---------- >>-------------- TShMT    +230.88 >>------------ >>
// >>TS>>-- ID 0 ------Event   130266: HitIndex[0,1] TSp  +181.33 TSm  +187.61 dTS    -6.28 TSsMT    +184.47 TShMT    -137.02 dTSMT  -321.49 >>
// Good
// >>TS>>-- ID 0 ------Event   130266: HitIndex[0,1] TSp  +181.33 TSm  +187.61 dTS    -6.28 TSsMT    +184.47 TShMT    +230.88 dTSMT   +46.41 >>

// >>TS>> NHits  26 in Event  1191062: HitOrder[3,0] TSp  -549.00 TSm  -559.12 dTS   +10.12 TSsMT    -554.06 TShMT    -507.82 dTSMT   +46.23 >>
// >>TS>> NHits  26 in Event  1191062: HitOrder[3,1] TSp  -192.61 TSm  -174.47 dTS   -18.14 TSsMT    -183.54 TShMT    -137.25 dTSMT   +46.29 >>
// >>TS>> NHits  26 in Event  1191062: HitOrder[3,2] TSp  +585.87 TSm  +267.87 dTS  +318.00 TSsMT    +426.87 TShMT    +626.46 dTSMT  +199.59 >>
// >>TS>> NHits  26 in Event  1191062: HitOrder[3,3] TSp    +0.00 TSm  +574.59 >>---------- >>-------------- TShMT      +0.00 >>------------ >>
// >>TS>>-- ID 0 ------Event  1191062: HitIndex[0,0] TSp  -549.00 TSm  -559.12 dTS   +10.12 TSsMT    -554.06 TShMT    -507.82 dTSMT   +46.23 >>
// >>TS>>-- ID 1 ------Event  1191062: HitIndex[1,1] TSp  -192.61 TSm  -174.47 dTS   -18.14 TSsMT    -183.54 TShMT    -137.25 dTSMT   +46.29 >>
// >>TS>>-- ID 2 ------Event  1191062: HitIndex[2,3] TSp  +585.87 TSm  +574.59 dTS   +11.28 TSsMT    +580.23 TShMT      +0.00 dTSMT  -580.23 >>
// Good
// >>TS>>-- ID 0 ------Event  1191062: HitIndex[0,0] TSp  -549.00 TSm  -559.12 dTS   +10.12 TSsMT    -554.06 TShMT    -507.82 dTSMT   +46.23 >>
// >>TS>>-- ID 1 ------Event  1191062: HitIndex[1,1] TSp  -192.61 TSm  -174.47 dTS   -18.14 TSsMT    -183.54 TShMT    -137.25 dTSMT   +46.29 >>
// >>TS>>-- ID 2 ------Event  1191062: HitIndex[2,3] TSp  +585.87 TSm  +574.59 dTS   +11.28 TSsMT    +580.23 TShMT    +626.46 dTSMT   +46.23 >>


// >>TS>> NHits  30 in Event  1047585: HitOrder[5,0] TSp  -551.32 TSm  -558.19 dTS    +6.86 TSsMT    -554.75 TShMT    -508.40 dTSMT   +46.35 >>
// >>TS>> NHits  30 in Event  1047585: HitOrder[5,1] TSp  -188.19 TSm  -178.54 dTS    -9.65 TSsMT    -183.37 TShMT    -137.02 dTSMT   +46.35 >>
// >>TS>> NHits  30 in Event  1047585: HitOrder[5,2] TSp   +45.13 TSm   +63.62 dTS   -18.49 TSsMT     +54.38 TShMT    +100.73 dTSMT   +46.35 >>
// >>TS>> NHits  30 in Event  1047585: HitOrder[5,3] TSp  +406.86 TSm  +209.25 dTS  +197.62 TSsMT    +308.05 TShMT    +629.60 dTSMT  +321.55 >>
// >>TS>> NHits  30 in Event  1047585: HitOrder[5,4] TSp  +590.99 TSm  +575.75 dTS   +15.24 TSsMT    +583.37 TShMT      +0.00 dTSMT  -583.37 >>
// >>TS>> NHits  30 in Event  1047585: HitOrder[5,5] TSp    +0.00 TSm  +957.14 >>---------- >>-------------- TShMT      +0.00 >>------------ >>
// >>TS>>-- ID 0 ------Event  1047585: HitIndex[0,0] TSp  -551.32 TSm  -558.19 dTS    +6.86 TSsMT    -554.75 TShMT    -508.40 dTSMT   +46.35 >>
// >>TS>>-- ID 1 ------Event  1047585: HitIndex[1,1] TSp  -188.19 TSm  -178.54 dTS    -9.65 TSsMT    -183.37 TShMT    -137.02 dTSMT   +46.35 >>
// >>TS>>-- ID 2 ------Event  1047585: HitIndex[2,2] TSp   +45.13 TSm   +63.62 dTS   -18.49 TSsMT     +54.38 TShMT    +100.73 dTSMT   +46.35 >>
// >>TS>>-- ID 3 ------Event  1047585: HitIndex[4,4] TSp  +590.99 TSm  +575.75 dTS   +15.24 TSsMT    +583.37 TShMT      +0.00 dTSMT  -583.37 >>
// Good
// >>TS>>-- ID 0 ------Event  1047585: HitIndex[0,0] TSp  -551.32 TSm  -558.19 dTS    +6.86 TSsMT    -554.75 TShMT    -508.40 dTSMT   +46.35 >>
// >>TS>>-- ID 1 ------Event  1047585: HitIndex[1,1] TSp  -188.19 TSm  -178.54 dTS    -9.65 TSsMT    -183.37 TShMT    -137.02 dTSMT   +46.35 >>
// >>TS>>-- ID 2 ------Event  1047585: HitIndex[2,2] TSp   +45.13 TSm   +63.62 dTS   -18.49 TSsMT     +54.38 TShMT    +100.73 dTSMT   +46.35 >>
// >>TS>>-- ID 3 ------Event  1047585: HitIndex[4,4] TSp  +590.99 TSm  +575.75 dTS   +15.24 TSsMT    +583.37 TShMT    +629.60 dTSMT   +46.23 >>



// >>TS>> NHits  13 in Event  1905451: HitOrder[2,0] TSp  -182.49 TSm  -185.05 dTS    +2.56 TSsMT    -183.77 TShMT    -137.37 dTSMT   +46.41 >>
// >>TS>> NHits  13 in Event  1905451: HitOrder[2,1] TSp  +952.60 TSm  +757.55 dTS  +195.06 TSsMT    +855.07 TShMT    +997.62 dTSMT  +142.54 >>
// >>TS>> NHits  13 in Event  1905451: HitOrder[2,2] TSp    +0.00 TSm  +949.93 >>---------- >>-------------- TShMT      +0.00 >>------------ >>
// >>TS>>-- ID 0 ------Event  1905451: HitIndex[0,0] TSp  -182.49 TSm  -185.05 dTS    +2.56 TSsMT    -183.77 TShMT    -137.37 dTSMT   +46.41 >>
// >>TS>>-- ID 1 ------Event  1905451: HitIndex[1,2] TSp  +952.60 TSm  +949.93 dTS    +2.68 TSsMT    +951.26 TShMT      +0.00 dTSMT  -951.26 >>
// Good
// >>TS>>-- ID 0 ------Event  1905451: HitIndex[0,0] TSp  -182.49 TSm  -185.05 dTS    +2.56 TSsMT    -183.77 TShMT    -137.37 dTSMT   +46.41 >>
// >>TS>>-- ID 1 ------Event  1905451: HitIndex[1,2] TSp  +952.60 TSm  +949.93 dTS    +2.68 TSsMT    +951.26 TShMT    +997.62 dTSMT   +46.35 >>




// >>TS>> NHits  16 in Event  1847953: HitOrder[3,0] TSp  -547.48 TSm  -559.46 dTS   +11.98 TSsMT    -553.47 TShMT    -507.12 dTSMT   +46.35 >>
// >>TS>> NHits  16 in Event  1847953: HitOrder[3,1] TSp  -191.45 TSm  -175.40 dTS   -16.05 TSsMT    -183.43 TShMT    -137.25 dTSMT   +46.18 >>
// >>TS>> NHits  16 in Event  1847953: HitOrder[3,2] TSp  +587.26 TSm  +208.67 dTS  +378.60 TSsMT    +397.96 TShMT    +627.28 dTSMT  +229.31 >>
// >>TS>> NHits  16 in Event  1847953: HitOrder[3,3] TSp    +0.00 TSm  +574.59 >>---------- >>-------------- TShMT      +0.00 >>------------ >>
// >>TS>>-- ID 0 ------Event  1847953: HitIndex[0,0] TSp  -547.48 TSm  -559.46 dTS   +11.98 TSsMT    -553.47 TShMT    -507.12 dTSMT   +46.35 >>
// >>TS>>-- ID 1 ------Event  1847953: HitIndex[1,1] TSp  -191.45 TSm  -175.40 dTS   -16.05 TSsMT    -183.43 TShMT    -137.25 dTSMT   +46.18 >>
// >>TS>>-- ID 2 ------Event  1847953: HitIndex[2,3] TSp  +587.26 TSm  +574.59 dTS   +12.68 TSsMT    +580.92 TShMT      +0.00 dTSMT  -580.92 >>
// Good
// >>TS>>-- ID 0 ------Event  1847953: HitIndex[0,0] TSp  -547.48 TSm  -559.46 dTS   +11.98 TSsMT    -553.47 TShMT    -507.12 dTSMT   +46.35 >>
// >>TS>>-- ID 1 ------Event  1847953: HitIndex[1,1] TSp  -191.45 TSm  -175.40 dTS   -16.05 TSsMT    -183.43 TShMT    -137.25 dTSMT   +46.18 >>
// >>TS>>-- ID 2 ------Event  1847953: HitIndex[2,3] TSp  +587.26 TSm  +574.59 dTS   +12.68 TSsMT    +580.92 TShMT    +627.28 dTSMT   +46.35 >>


// >>TS>> NHits   5 in Event  2007625: HitOrder[1,0] TSp  -184.24 TSm  -644.72 dTS  +460.48 TSsMT    -414.48 TShMT    -137.25 dTSMT  +277.23 >>
// >>TS>> NHits   5 in Event  2007625: HitOrder[1,1] TSp    +0.00 TSm  -182.73 >>---------- >>-------------- TShMT      +0.00 >>------------ >>
// >>TS>>-- ID 0 ------Event  2007625: HitIndex[0,1] TSp  -184.24 TSm  -182.73 dTS    -1.51 TSsMT    -183.48 TShMT      +0.00 dTSMT  +183.48 >>
// Good
// >>TS>>-- ID 0 ------Event  2007625: HitIndex[0,1] TSp  -184.24 TSm  -182.73 dTS    -1.51 TSsMT    -183.48 TShMT    -137.25 dTSMT   +46.23 >>


// >>TS>> NHits  13 in Event  1802703: HitOrder[2,0] TSp  -182.15 TSm  -710.09 dTS  +527.94 TSsMT    -446.12 TShMT    -136.90 dTSMT  +309.22 >>
// >>TS>> NHits  13 in Event  1802703: HitOrder[2,1] TSp  +496.42 TSm  -184.36 dTS  +680.78 TSsMT    +156.03 TShMT    +540.27 dTSMT  +384.24 >>
// >>TS>> NHits  13 in Event  1802703: HitOrder[2,2] TSp    +0.00 TSm  +491.65 >>---------- >>-------------- TShMT      +0.00 >>------------ >>
// >>TS>>-- ID 0 ------Event  1802703: HitIndex[0,1] TSp  -182.15 TSm  -184.36 dTS    +2.21 TSsMT    -183.25 TShMT    +540.27 dTSMT  +723.52 >>
// >>TS>>-- ID 1 ------Event  1802703: HitIndex[1,2] TSp  +496.42 TSm  +491.65 dTS    +4.77 TSsMT    +494.04 TShMT      +0.00 dTSMT  -494.04 >>
// Good 
// >>TS>>-- ID 0 ------Event  1802703: HitIndex[0,1] TSp  -182.15 TSm  -184.36 dTS    +2.21 TSsMT    -183.25 TShMT    -136.90 dTSMT   +46.35 >>
// >>TS>>-- ID 1 ------Event  1802703: HitIndex[1,2] TSp  +496.42 TSm  +491.65 dTS    +4.77 TSsMT    +494.04 TShMT    +540.27 dTSMT   +46.23 >>


//>>TS>> NHits   6 in Event    27071: HitOrder[1,0] TSp  -179.70 TSm  -187.38 dTS    +7.68 TSsMT    -183.54 TShMT    -773.48 dTSMT  -589.94 >>
//>>TS>> NHits   6 in Event    27071: HitOrder[1,1] TSp    +0.00 TSm    +0.00 >>---------- >>-------------- TShMT    -137.25 >>------------ >>
//>>TS>>-- ID 0 ------Event    27071: HitIndex[0,0] TSp  -179.70 TSm  -187.38 dTS    +7.68 TSsMT    -183.54 TShMT    -773.48 dTSMT  -589.94 >>


//>>TS>> NHits  14 in Event   125499: HitOrder[1,0] TSp  -186.45 TSm  -181.10 dTS    -5.35 TSsMT    -183.77 TShMT    -757.78 dTSMT  -574.00 >>
//>>TS>> NHits  14 in Event   125499: HitOrder[1,1] TSp    -0.12 TSm    +0.00 >>---------- >>-------------- TShMT    -137.37 >>------------ >>
//>>TS>>-- ID 0 ------Event   125499: HitIndex[0,0] TSp  -186.45 TSm  -181.10 dTS    -5.35 TSsMT    -183.77 TShMT    -757.78 dTSMT  -574.00 >>
