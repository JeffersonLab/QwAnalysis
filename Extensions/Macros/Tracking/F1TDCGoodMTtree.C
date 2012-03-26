// Author : Jeong Han Lee
//          jhlee@jlab.org
//
// Date   : Tuesday, February 28 15:09:13 EST 2012
// 
//         This script is the same as F1TDCGoodMT.C, but is develping for further studies.
//         It dosen't show anything after run, it will save all information in an additional
//         Root file which will has good meantime branches, and QwEvents. So
//         it will be easy to apply various cuts among MD, TS, and VDC
//       
//         NOTE that this is developing version, not working one. 
// 
//         One needs qwroot instead of root
//         $ qwroot 
//         Qw-Root [0] .L F1TDCGoodMT.C
//         Qw-Root [1] GoodMTMultiHits(1,2,-1, 10000, 50, 20, 1, 1, filename, 102)
//
//         MD1, TS2, event 0:10000, delta MD1 < 50, delta TS2 <20, MD debug on, TS debug on, rootfile, binnumber
//         in order to save debug outputs in a file
//         Qw-Root [3] GoodMtMultiHits(1,2,-1,-1,50,20,1,1); > debug.log



#include "F1TDCGoodMT.h"

ClassImp(MeanTime)
ClassImp(MeanTimeContainer)


typedef struct {
  Double_t hit0;
  Double_t hit1;
  Double_t hit2;
  Double_t hit3;
  Double_t hit4;
  Double_t hit5;
  Double_t hit6;
  Char_t  *name;
} HITS;


MeanTime::MeanTime()
{
  
  fMeanTime      = 0.0; 
  fSubtractTime  = 0.0;

  fPositiveValue = 0.0;
  fNegativeValue = 0.0;

  fPositiveHitId = 0;
  fNegativeHitId = 0;
  fMeanTimeId    = 0;
  
  fEventId       = 0;
  fDetectorType  = "";
  fHasValue      = false;
};


MeanTime::MeanTime(TString name, Long64_t ev_id, Double_t p_in, Double_t n_in, Int_t p_id, Int_t n_id)
{
  fDetectorType = name;
  fEventId      = ev_id;
  AddPNValues(p_in, n_in, p_id, n_id);

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
MeanTime::Print()
{

  TString output = "";
  if     (fDetectorType == "MD") output += "<<";
  else if(fDetectorType == "TS") output += ">>";

  output += fDetectorType;
  
  if     (fDetectorType == "MD") output += "<<";
  else if(fDetectorType == "TS") output += ">>";

  output += Form("-- ID %d ------Event %8d:", fMeanTimeId, fEventId);
  output += " HitIndex[";
  output += fPositiveHitId;
  output += ",";
  output += fNegativeHitId;
  output += "]";

  if      (fDetectorType == "MD") {
    output += Form( " MDp%+9.2f MDm%+9.2f dMD%+9.2f MDsMT %+10.2f",
		    fPositiveValue, fNegativeValue, fSubtractTime, fMeanTime);
  }
  else if (fDetectorType == "TS") {
    output += Form( " TSp%+9.2f TSm%+9.2f dTS%+9.2f TSsMT %+10.2f",
		    fPositiveValue, fNegativeValue, fSubtractTime, fMeanTime);
  }

  std::cout << output << std::endl;
  return;
};


Bool_t 
MeanTime::IsInTimeWindow(Double_t time_window)
{
  Bool_t status = false;
  status = ( fabs(fSubtractTime) < time_window ) ? true : false;
  return status;
}


void 
MeanTime::Clear()
{
  return;
}


MeanTimeContainer::MeanTimeContainer(TString name)
{

  fTimeWindowNs = 0.0;

  fDetectorName = name;
  
  fMeanTimeList = new TObjArray();
  fMeanTimeList -> Clear();
  fMeanTimeList -> SetOwner(kTRUE);
  fNMeanTimes   = 0;
  
  fEventId      = 0;
  
  for(Int_t i=0;i<7;i++) 
    {
      fPositiveValue[i] = 0.0;
      fNegativeValue[i] = 0.0;
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
    temp->SetMeanTimeId(fNMeanTimes);
    pos = fMeanTimeList->AddAtFree(temp) ;
    //  temp->Print();
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
MeanTimeContainer::ProcessMeanTime()
{
  //  MD 791 event...
  
  Bool_t   local_debug = false;
  Bool_t   GS_Stable_marriage_debug = false;

  Double_t ini      = 100000.0;

  Double_t mt[7]=  {ini};

  // Bool_t zero_flag = false;
  // Int_t count_zero = 0;

  // for(Int_t i =0; i<7; i++) 
  //   {
  //     mt[i] =  0.5*(fPositiveValue[i] + fNegativeValue[i]); 
  //     zero_flag = (mt[i] == 0.0) ? true : false;
  //     if(zero_flag) count_zero++;
  //   }

  // if(local_debug) printf("count zero %d\n", count_zero);

  // // reject when all entries are zero.
  // if(count_zero == 5) return;
  
  // if(count_zero == 5) {
  //   AddMeanTime(fDetectorName, fEventId, fPositiveValue[0], fNegativeValue[0], 0, 0);
  // }



  Double_t nst[7][7] = {{ini}}; // [negative][positive]
  Double_t pst[7][7] = {{ini}}; // [positive][negative]

  Char_t *p_name[7] = {"p0", "p1", "p2", "p3", "p4", "p5", "p6"};
  Char_t *n_name[7] = {"n0", "n1", "n2", "n3", "n4", "n5", "n6"};

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
      // While there is a positive channel p which is free and has not proposed to every pixel
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
	  // if pixel prefer f_old(suitor[p]) to f 
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
      if (local_debug)printf("%s %s\n", p_name[p], n_name[fiancee[p]]);
      //     if ( subtract > fTimeWindowNs ) continue; // reject a meantime out of a reasonable time window
      if(fPositiveValue[p] !=0.0 && fNegativeValue[fiancee[p]]!=0.0) {
	AddMeanTime(fDetectorName, fEventId, fPositiveValue[p], fNegativeValue[fiancee[p]], p, fiancee[p]);
      }
    }

    
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
  

  return;
}

void
MeanTimeContainer::Print()
{
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


void 
MeanTimeContainer::Clear()
{
  // for(Int_t i=0;i<7;i++) 
  //   {
  //     fMeanTime[i].Clear();
  //   }
  return;
};


void
GoodMTMultiHitsTree(Int_t md_plane, Int_t ts_plane, 
		    Int_t event_number_start=-1, 
		    Int_t event_number_end=-1,
		    Int_t MD_MT_time_window_ns = 50,
		    Int_t TS_MT_time_window_ns = 20,
		    Bool_t ts_debug = false, Bool_t md_debug = false,
		    TString filename="14955.root",
		    Int_t nbin=102, 
		    Int_t time_shift_ns=0)
{

  //  Style();
  Bool_t verbose = true;
  // Bool_t ts_debug = false;
  // Bool_t md_debug = true;

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


  // TCanvas *c1 = new TCanvas("GoodMTMultiHits","GoodMtMultiHits Stacked hists",100,100,1080,800);

  TString name ="";
  

  // TObjArray HistList(0);

  // THStack *mt_hs[6];

  
  // mt_hs[0] = new THStack(Form("MD%dsMTHitStack", md_plane),
  // 		      Form("MD%dsMT F1TDC Multi-Hit Contribution Histogram", md_plane)
  // 		      );
  // mt_hs[1] = new THStack(Form("TS%dsMTHitStack", ts_plane),
  // 		      Form("TS%dsMT F1TDC Multi-Hit Contribution Histogram", ts_plane)
  // 		      );
  // mt_hs[2] = new THStack(Form("TS%dhMTHitStack", ts_plane),
  // 		      Form("TS%dhMT F1TDC Multi-Hit Contribution Histogram", ts_plane)
  // 		      );

  // mt_hs[3] = new THStack(Form("dMD%dpmHitStack", md_plane),
  // 			 Form("MD%p - MD%dm F1TDC Multi-Hit Contribution Histogram", md_plane, md_plane)
  // 		      );
  // mt_hs[4] = new THStack(Form("dTS%dpmHitStack", ts_plane),
  // 			 Form("TS%dp - TS%dm F1TDC Multi-Hit Contribution Histogram", ts_plane, ts_plane)
  // 		      );
  // mt_hs[5] = new THStack(Form("dTS%dMTsHitStack", ts_plane),
  // 			 Form("TS%dhMT -TS%dsMT F1TDC Multi-Hit Contribution Histogram", ts_plane, ts_plane)
  // 			 );
  
  
  // for(Int_t idx=0; idx<6; idx++)
  //   {
  //     HistList.Add(mt_hs[idx]);
  //   }
  
  // TH1D* mdhist[7];
  // TH1D* tshist[7];
  // TH1D* tsmthist[7];

  // TH1D* dmdhist[7];
  // TH1D* dtshist[7];
  // TH1D* dtsmthist[7];

  // //
  // for(Int_t idx=0; idx <7;idx++)
  //   {// 2200, 1100, 550
  //     mdhist[idx]   = new TH1D(Form("MDsMT_hitid_%d", idx), Form("MD sMT HitId %d", idx), nbin, -950+time_shift_ns, 1400+time_shift_ns);
  //     tshist[idx]   = new TH1D(Form("TSsMT_hitid_%d", idx), Form("TS sMT HitId %d", idx), nbin, -950+time_shift_ns, 1400+time_shift_ns);
  //     tsmthist[idx] = new TH1D(Form("TShMT_hitid_%d", idx), Form("TS hMT HitId %d", idx), nbin, -950+time_shift_ns, 1400+time_shift_ns);
   
  //     dmdhist[idx]   = new TH1D(Form("dMDpm_hitid_%d", idx), Form("MDp - MDm  HitId %d", idx),    nbin, -2000+time_shift_ns, 2000+time_shift_ns);
  //     dtshist[idx]   = new TH1D(Form("dTSpm_hitid_%d", idx), Form("TSp - TSm  HitId %d", idx),    nbin, -2000+time_shift_ns, 2000+time_shift_ns);
  //     dtsmthist[idx] = new TH1D(Form("dTSMT_hitid_%d", idx), Form("TShMT - TSsMT HitId %d", idx), nbin, -2000+time_shift_ns, 2000+time_shift_ns);

  //     HistList.Add(mdhist[idx]);
  //     HistList.Add(tshist[idx]);
  //     HistList.Add(tsmthist[idx]);

  //     HistList.Add(dmdhist[idx]);
  //     HistList.Add(dtshist[idx]);
  //     HistList.Add(dtsmthist[idx]);
      
  //   } 
  


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

  MeanTimeContainer *mt_container = 0;
  
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

  TString output_filename="GoodMTMultiHitsTree";
 
  output_filename += "_md";
  output_filename += md_plane;
					       
  output_filename += "_ts";
  output_filename += ts_plane;
  
  //  output_filename += "_";
  output_filename += "_MD_";
  output_filename += MD_MT_time_window_ns;
  output_filename += "_TS_";
  output_filename += TS_MT_time_window_ns;
  output_filename += "_";
  output_filename += filename;

  TFile outfile(Form("%s", output_filename.Data()),"recreate");


  TTree t1("t1", "tree test");
  HITS b_mdp;
  HITS b_mdm;
  // HITS tsp;
  // HITS tsm;
  // HITS tsmt;

  t1.Branch("mdp", &b_mdp, "0:1:2:3:4:5:6:MDp");
  t1.Branch("mdm", &b_mdm, "0:1:2:3:4:5:6:MDm");
  // t1.Branch("tsp", &tsp, "0:1:2:3:4:5:6:TSp");
  // t1.Branch("tsm", &tsm, "0:1:2:3:4:5:6:TSm");
  // t1.Branch("tsmt", &tsmt, "0:1:2:3:4:5:6:TSmt");


  Double_t tsp[7]  = {ini};
  Double_t tsm[7]  = {ini};
  Double_t tsmt[7] = {ini};
  
  Double_t mdp[7]  = {ini};
  Double_t mdm[7]  = {ini};


 
  afile.cd();

  for (Long64_t i=event_number_start; i<num_entries; i++)
    {//;
      localEntry = event_tree->LoadTree(i);
      event_tree->GetEntry(localEntry);
      
      if(i % 1000 == 0) {
	printf(" Total events %d events process so far : %d\n", num_entries, i);
      }
      
      //    printf("\n");
      nhit = qw_event->GetNumberOfHits();
      
      

      // Double_t tsp[7]  = {ini};
      // Double_t tsm[7]  = {ini};
      // Double_t tsmt[7] = {ini};

      // Double_t mdp[7]  = {ini};
      // Double_t mdm[7]  = {ini};

      Int_t fregion    = 0;
      Int_t fplane     = 0;
      Int_t felement   = 0;
      Int_t fhitnumber = 0;

      Double_t timens  = 0.0;
      
      Int_t ts_max_hit = 0;
      Int_t md_max_hit = 0;
    
      for(Int_t hit_idx=0; hit_idx<nhit; hit_idx++) 
	{//;;
	  qwhit = qw_event->GetHit(hit_idx);

	  fregion    = qwhit->GetRegion();

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
		//		tsmthist[fhitnumber] -> Fill(timens);
	      }
	    }
	  }
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

      outfile.cd();
      b_mdp.hit0 = mdp[0];
      b_mdp.hit1 = mdp[1];
      b_mdp.hit2 = mdp[2];
      b_mdp.hit3 = mdp[3];
      b_mdp.hit4 = mdp[4];
      b_mdp.hit5 = mdp[5];
      b_mdp.hit6 = mdp[6];
      
      t1.Fill();

      afile.cd();
      Int_t ts_max_hit_range = ts_max_hit+1;

      for(Int_t idx=0; idx<ts_max_hit_range; idx++) 
	{//;;


	  if (tsp[idx] == ini && tsm[idx] == ini) continue;
	  Double_t software_mt_ts = 0.0;
	  Double_t software_del_ts = 0.0;
	  if(ts_debug) {
	    printf(">>TS>> NHits%4d in Event %8d: HitOrder[%d,%d] TSp%+9.2f TSm%+9.2f", 
		   nhit, i, ts_max_hit,  idx,tsp[idx], tsm[idx]);
	  }
	  if (tsp[idx]!=ini && tsm[idx]!=ini) {
	    software_mt_ts  = 0.5*(tsp[idx]+tsm[idx]);
	    software_del_ts = tsp[idx] - tsm[idx];
	    if (ts_debug) printf(" dTS%+9.2f TSsMT %+10.2f", software_del_ts, software_mt_ts);
	    //	    tshist[idx]  -> Fill(software_mt_ts);
	    //	    dtshist[idx] -> Fill(software_del_ts);
	  }
	  else {
	    software_mt_ts = 0.0;
	    if (ts_debug) printf(" >>---------- >>--------------");
	  }

	 
	  if(software_mt_ts!=0.0) {
	    //	    dtsmthist[idx] -> Fill(tsmt[idx]-software_mt_ts);
	    if (ts_debug )  {
	      printf(" TSMT %+8.2f dTSMT %+8.2f >>\n", tsmt[idx], tsmt[idx]-software_mt_ts);
	    }
	  }
	  else {
	    if (ts_debug )  {
	      printf(" TSMT %+8.2f >>------------ >>\n", tsmt[idx]);
	    }
	  }
	  // if (tsp[idx]!=ini && tsm[idx]!=ini) {

	  
	  // }
	}//;;



      mt_container = new MeanTimeContainer("TS");
      mt_container -> SetEventId(i);
      mt_container -> SetTimeWindow(TS_MT_time_window_ns);
      mt_container -> Add(tsp, tsm);
      mt_container -> ProcessMeanTime();


      MeanTime* ts_mt_time= NULL;

      for(Int_t ts_size=0; ts_size < mt_container->Size(); ts_size++)
	{
	  ts_mt_time = mt_container->GetMeanTimeObject(ts_size);
	  ts_mt_time -> Print();
	  //	  tshist[ts_size]  -> Fill(ts_mt_time->GetMeanTime());
	  //	  dtshist[ts_size] -> Fill(ts_mt_time->GetSubtractTime());
	  
	}


      delete mt_container; mt_container = NULL;



      Bool_t md_zero_suppress = false;

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

	    //   	    mdhist[idx]  -> Fill(software_mt_md);
	    //      	    dmdhist[idx] -> Fill(software_del_md);
      	    if (md_debug) printf(" dMD%+9.2f MDsMT %+10.2f", software_del_md, software_mt_md);
      	  }
      	  else {
      	    if(md_debug)  printf(" <<---------- <<--------------");

      	  }
      	  if (md_debug) printf(" <<----------- <<------------ <<\n");

      	}//;;


   
      mt_container = new MeanTimeContainer("MD");
      mt_container -> SetEventId(i);
      mt_container -> SetTimeWindow(MD_MT_time_window_ns);
      mt_container -> Add(mdp, mdm);
      mt_container -> ProcessMeanTime();

      MeanTime* md_mt_time= NULL;

      for(Int_t md_size=0; md_size < mt_container->Size(); md_size++)
	{
	  md_mt_time = mt_container->GetMeanTimeObject(md_size);
	  md_mt_time -> Print();
	  //	  mdhist[md_size]  -> Fill(md_mt_time->GetMeanTime());
	  //	  dmdhist[md_size] -> Fill(md_mt_time->GetSubtractTime());
	  
	}
      delete mt_container; mt_container = NULL;
      //    	}//;; for(Int_t hit_idx=0; 
    }//; Long64_t i=0; i<num_entries....
  
  afile.Close();


  outfile.cd();
  outfile.Write();

  // for(Int_t idx=0; idx <7;idx++)
  //   {
  //     mdhist[idx] -> SetDirectory(0);
  //     mdhist[idx] -> SetMarkerStyle(21);
  //     mt_hs[0]->Add(mdhist[idx]);   
   
  //     tshist[idx] -> SetDirectory(0);
  //     tshist[idx] -> SetMarkerStyle(21);
  //     mt_hs[1]->Add(tshist[idx]);

  //     tsmthist[idx] -> SetDirectory(0);
  //     tsmthist[idx] -> SetMarkerStyle(21);
  //     mt_hs[2]->Add(tsmthist[idx]);

  //     dmdhist[idx] -> SetDirectory(0);
  //     dmdhist[idx] -> SetMarkerStyle(21);
  //     mt_hs[3]->Add(dmdhist[idx]);   
   
  //     dtshist[idx] -> SetDirectory(0);
  //     dtshist[idx] -> SetMarkerStyle(21);
  //     mt_hs[4]->Add(dtshist[idx]);

  //     dtsmthist[idx] -> SetDirectory(0);
  //     dtsmthist[idx] -> SetMarkerStyle(21);
  //     mt_hs[5]->Add(dtsmthist[idx]);

  //   }





  // // Int_t color[7]= {2,3,4,6,7,5,43};

  // // for (Int_t idx = 0; i<7; i++) 
  // //   {
  // //     //      color[idx] = gROOT->GetColor(int_color[idx]);

  //     // mdhist[idx] -> SetFillColor((Color_t) color[idx]);
  //     // mdhist[idx] -> SetMarkerColor((Color_t) color[idx]);
      
  //     // tshist[idx] -> SetFillColor((Color_t) color[idx]);
  //     // tshist[idx] -> SetMarkerColor((Color_t) color[idx]);
      
  //     // tsmthist[idx] -> SetFillColor((Color_t) color[idx]);
  //     // tsmthist[idx] -> SetMarkerColor((Color_t) color[idx]);

  //     // dmdhist[idx] -> SetFillColor((Color_t) color[idx]);
  //     // dmdhist[idx] -> SetMarkerColor((Color_t) color[idx]);
      
  //     // dtshist[idx] -> SetFillColor((Color_t) color[idx]);
  //     // dtshist[idx] -> SetMarkerColor((Color_t) color[idx]);
      
  //     // dtsmthist[idx] -> SetFillColor((Color_t) color[idx]);
  //     // dtsmthist[idx] -> SetMarkerColor((Color_t) color[idx]);
    
  // //    }


  // mdhist[0] -> SetFillColor(2);
  // mdhist[0] -> SetMarkerColor(2);
  
  // tshist[0] -> SetFillColor(2);
  // tshist[0] -> SetMarkerColor(2);
  
  // tsmthist[0] -> SetFillColor(2);
  // tsmthist[0] -> SetMarkerColor(2);
  
  // dmdhist[0] -> SetFillColor(2);
  // dmdhist[0] -> SetMarkerColor(2);
  
  // dtshist[0] -> SetFillColor(2);
  // dtshist[0] -> SetMarkerColor(2);
  
  // dtsmthist[0] -> SetFillColor(2);
  // dtsmthist[0] -> SetMarkerColor(2);


  // mdhist[1] -> SetFillColor(3);
  // mdhist[1] -> SetMarkerColor(3);
  
  // tshist[1] -> SetFillColor(3);
  // tshist[1] -> SetMarkerColor(3);
  
  // tsmthist[1] -> SetFillColor(3);
  // tsmthist[1] -> SetMarkerColor(3);
  
  // dmdhist[1] -> SetFillColor(3);
  // dmdhist[1] -> SetMarkerColor(3);
  
  // dtshist[1] -> SetFillColor(3);
  // dtshist[1] -> SetMarkerColor(3);
  
  // dtsmthist[1] -> SetFillColor(3);
  // dtsmthist[1] -> SetMarkerColor(3);



  // mdhist[2] -> SetFillColor(4);
  // mdhist[2] -> SetMarkerColor(4);
  
  // tshist[2] -> SetFillColor(4);
  // tshist[2] -> SetMarkerColor(4);
  
  // tsmthist[2] -> SetFillColor(4);
  // tsmthist[2] -> SetMarkerColor(4);
  
  // dmdhist[2] -> SetFillColor(4);
  // dmdhist[2] -> SetMarkerColor(4);
  
  // dtshist[2] -> SetFillColor(4);
  // dtshist[2] -> SetMarkerColor(4);
  
  // dtsmthist[2] -> SetFillColor(4);
  // dtsmthist[2] -> SetMarkerColor(4);




  // mdhist[3] -> SetFillColor(6);
  // mdhist[3] -> SetMarkerColor(6);
  
  // tshist[3] -> SetFillColor(6);
  // tshist[3] -> SetMarkerColor(6);
  
  // tsmthist[3] -> SetFillColor(6);
  // tsmthist[3] -> SetMarkerColor(6);
  
  // dmdhist[3] -> SetFillColor(6);
  // dmdhist[3] -> SetMarkerColor(6);
  
  // dtshist[3] -> SetFillColor(6);
  // dtshist[3] -> SetMarkerColor(6);
  
  // dtsmthist[3] -> SetFillColor(6);
  // dtsmthist[3] -> SetMarkerColor(6);




  // mdhist[4] -> SetFillColor(7);
  // mdhist[4] -> SetMarkerColor(7);
  
  // tshist[4] -> SetFillColor(7);
  // tshist[4] -> SetMarkerColor(7);
  
  // tsmthist[4] -> SetFillColor(7);
  // tsmthist[4] -> SetMarkerColor(7);
  
  // dmdhist[4] -> SetFillColor(7);
  // dmdhist[4] -> SetMarkerColor(7);
  
  // dtshist[4] -> SetFillColor(7);
  // dtshist[4] -> SetMarkerColor(7);
  
  // dtsmthist[4] -> SetFillColor(7);
  // dtsmthist[4] -> SetMarkerColor(7);




  // mdhist[5] -> SetFillColor(5);
  // mdhist[5] -> SetMarkerColor(5);
  
  // tshist[5] -> SetFillColor(5);
  // tshist[5] -> SetMarkerColor(5);
  
  // tsmthist[5] -> SetFillColor(5);
  // tsmthist[5] -> SetMarkerColor(5);
  
  // dmdhist[5] -> SetFillColor(5);
  // dmdhist[5] -> SetMarkerColor(5);
  
  // dtshist[5] -> SetFillColor(5);
  // dtshist[5] -> SetMarkerColor(5);
  
  // dtsmthist[5] -> SetFillColor(5);
  // dtsmthist[5] -> SetMarkerColor(5);






  // mdhist[6] -> SetFillColor(43);
  // mdhist[6] -> SetMarkerColor(43);
  
  // tshist[6] -> SetFillColor(43);
  // tshist[6] -> SetMarkerColor(43);
  
  // tsmthist[6] -> SetFillColor(43);
  // tsmthist[6] -> SetMarkerColor(43);
  
  // dmdhist[6] -> SetFillColor(43);
  // dmdhist[6] -> SetMarkerColor(43);
  
  // dtshist[6] -> SetFillColor(43);
  // dtshist[6] -> SetMarkerColor(43);
  
  // dtsmthist[6] -> SetFillColor(43);
  // dtsmthist[6] -> SetMarkerColor(43);




  // TLegend *leg = NULL;
  
  // c1->Divide(3,2);
  // if(mt_hs[0]) {
  //   c1->cd(1);
 
  //   gPad->SetGridy();
  //   gPad->SetLogy();
  //   mt_hs[0] -> Draw("goff");
  //   mt_hs[0] -> GetXaxis() -> SetTitle("Time[ns]");
  //   mt_hs[0] -> Draw();
  //   leg = new TLegend(0.6,0.7,0.99,0.92,NULL,"brNDC");
    
  //   leg->SetNColumns(2);
  //   leg->SetBorderSize(1);
  //   leg->SetTextFont(62);
  //   leg->SetLineColor(1);
  //   leg->SetLineStyle(1);
  //   leg->SetLineWidth(1);
  //   leg->SetFillColor(0);
  //   leg->SetFillStyle(1001);
    
  //   leg->SetHeader("Stack Histogram");
  //   leg->AddEntry(mdhist[0],"1st Hits","f");
  //   leg->AddEntry(mdhist[1],"2nd Hits","f");
  //   leg->AddEntry(mdhist[2],"3rd Hits","f");
  //   leg->AddEntry(mdhist[3],"4th Hits","f"); 
  //   leg->AddEntry(mdhist[4],"5th Hits","f");
  //   leg->AddEntry(mdhist[5],"6th Hits","f");
  //   leg->AddEntry(mdhist[6],"7th Hits","f");
  //   leg->Draw();
  //   gPad->Update();
 
 
  //   c1->cd(4);
  //   gPad->SetGridy();
  //   gPad->SetLogy();
  //   mt_hs[0]->Draw("nostack");
  //   leg->SetHeader("Non Stack Histogram");
  //   leg->Draw();
  //   gPad->Update();

  // }

  // if(mt_hs[1]) {
  //   c1->cd(2);
  //   gPad->SetGridy();
  //   gPad->SetLogy();
  //   mt_hs[1] -> Draw("goff");
  //   mt_hs[1] -> GetXaxis() -> SetTitle("Time[ns]");
  //   mt_hs[1] -> Draw();
  //   TLegend *leg2 = new TLegend(0.6,0.7,0.99,0.92,NULL,"brNDC");
  //   leg2->SetHeader("Stack Histogram");
  //   leg2->SetNColumns(2);
  //   leg2->AddEntry(tshist[0],"1st Hits","f");
  //   leg2->AddEntry(tshist[1],"2nd Hits","f");
  //   leg2->AddEntry(tshist[2],"3rd Hits","f");
  //   leg2->AddEntry(tshist[3],"4th Hits","f"); 
  //   leg2->AddEntry(tshist[4],"5th Hits","f");
  //   leg2->AddEntry(tshist[5],"6th Hits","f");
  //   leg2->AddEntry(tshist[6],"7th Hits","f");
  //   leg2->Draw();
  //   gPad->Update();

  //   c1->cd(5);
  //   gPad->SetGridy();
  //   gPad->SetLogy();
  //   mt_hs[1]->Draw("nostack");
  //   leg2->SetHeader("Non Stack Histogram");
  //   leg2->Draw();
  //   gPad->Update();
  //   c1-> Modified();
  //   c1-> Update();

  // }
  
  // if(mt_hs[2]) {
  //   c1->cd(3);
  //   gPad->SetGridy();
  //   gPad->SetLogy();
  //   mt_hs[2] -> Draw("goff");
  //   mt_hs[2] -> GetXaxis() -> SetTitle("Time[ns]");
  //   mt_hs[2] -> Draw();
  //   TLegend *leg3 = new TLegend(0.6,0.7,0.99,0.92,NULL,"brNDC");
  //   leg3->SetHeader("Stack Histogram");
  //   leg3->SetNColumns(2);
  //   leg3->AddEntry(tshist[0],"1st Hits","f");
  //   leg3->AddEntry(tshist[1],"2nd Hits","f");
  //   leg3->AddEntry(tshist[2],"3rd Hits","f");
  //   leg3->AddEntry(tshist[3],"4th Hits","f"); 
  //   leg3->AddEntry(tshist[4],"5th Hits","f");
  //   leg3->AddEntry(tshist[5],"6th Hits","f");
  //   leg3->AddEntry(tshist[6],"7th Hits","f");
  //   leg3->Draw();
  //   gPad->Update();

  //   c1->cd(6);
  //   gPad->SetGridy();
  //   gPad->SetLogy();
  //   mt_hs[2]->Draw("nostack");
  //   leg3->SetHeader("Non Stack Histogram");
  //   leg3->Draw();
  //   gPad->Update();

  // }
  // c1-> Modified();
  // c1-> Update();



  return;
};
