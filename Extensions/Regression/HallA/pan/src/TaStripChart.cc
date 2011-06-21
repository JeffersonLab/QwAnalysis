//////////////////////////////////////////////////////////////////////////
//
//     HALL A C++/ROOT Parity Analyzer  Pan (Online Monitor version)           
//
//        TaStripChart.cc   (implementation file)
//        ^^^^^^^^^^^^^^^
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaStripChart.cc,v 1.2 2003/07/31 16:12:00 rsholmes Exp $
//
//////////////////////////////////////////////////////////////////////////
//
//   PHENIX and E158-like StripCharts for HAPPEX2/He4 adcs display. 
//
//////////////////////////////////////////////////////////////////////////

#include "TaStripChart.hh"
#include <iostream>

using namespace std;

ClassImp(TaStripChart)

TaStripChart::TaStripChart():
  fName(0),fDataName(0),fSC_hist(0),fSC_array(0),fSC_sum(0),fMaxSum(0),fSC_current_bin(0),fSC_num_of_chan(0),fSC_sum_count(0),fAbsMin(0),fAbsMax(0)
{
}

TaStripChart::TaStripChart(char* label, char* dataname, Int_t NumbofChan, Int_t NumofEvperChan, Axis_t xmin, Axis_t xmax):
  fSC_hist(0),fSC_array(0),fSC_sum(0),fSC_sum_count(0)
{
  fSC_num_of_chan=NumbofChan;
  fMaxSum = NumofEvperChan; 
  SetName(label);
  SetDataName(dataname);
  fAbsMin=xmin;
  fAbsMax=xmax;
  fSC_hist= new TH1D(label,label,fSC_num_of_chan,fAbsMin,fAbsMax);
  fSC_array = new TArrayD(fSC_num_of_chan);
  Reset();
  //  cout<<" stripchart "<<fName<<" with data "<<fDataName<<" created"<<endl;
}

TaStripChart::~TaStripChart()
{
 if (fSC_array) delete fSC_array; 
 if (fSC_hist) delete fSC_hist;
}

TaStripChart::TaStripChart(const TaStripChart& sc)
{
  fName            = sc.fName;
  fDataName        = sc.fDataName;
  fSC_num_of_chan  = sc.fSC_num_of_chan;
  fMaxSum          = sc.fMaxSum;
  fAbsMin          = sc.fAbsMin;
  fAbsMax          = sc.fAbsMax;
  fSC_current_bin  = sc.fSC_current_bin;
  fSC_sum          = sc.fSC_sum;
  fSC_sum_count    = sc.fSC_sum_count;
  if (sc.fSC_hist !=0) 
    {
     fSC_hist = new TH1D(fName,fDataName,fSC_num_of_chan,fAbsMin,fAbsMax);
     fSC_hist = sc.fSC_hist;
    }
  else fSC_hist=0;
  fSC_array = new TArrayD(fSC_num_of_chan);
  for (Int_t i=0;i<fSC_num_of_chan;i++)
    {
     fSC_array[i]=sc.fSC_array[i];
    }
}

TaStripChart &TaStripChart::operator=(const TaStripChart &sc)
{
if ( this != &sc )
    {
     if (fSC_array) delete fSC_array; 
     if (fSC_hist) delete fSC_hist;
     fName            = sc.fName;
     fDataName        = sc.fDataName;
     fSC_num_of_chan  = sc.fSC_num_of_chan;
     fMaxSum          = sc.fMaxSum;
     fAbsMin          = sc.fAbsMin;
     fAbsMax          = sc.fAbsMax;
     fSC_current_bin  = sc.fSC_current_bin;
     fSC_sum          = sc.fSC_sum;
     fSC_sum_count    = sc.fSC_sum_count;     
     fSC_array =0;
     fSC_hist  =0;
     fSC_hist = new TH1D(fName,fDataName,fSC_num_of_chan,fAbsMin,fAbsMax);
     fSC_hist = sc.fSC_hist;
     fSC_array = new TArrayD(fSC_num_of_chan);
     for (Int_t i=0;i<fSC_num_of_chan;i++)
       {
        fSC_array[i]=sc.fSC_array[i];
       }      
     return *this;
     }
 else 
   {
     cout<<"warning ! TaStripChart assignment to self !"<<endl;
     return *this;
   }
}

Int_t TaStripChart::DataSum(Double_t value)
{
  if (fSC_sum_count == fMaxSum)
    {
      fSC_sum=0;
      fSC_sum_count = 0;
    }
  fSC_sum+=value;
  fSC_sum_count++;
  if ( fSC_sum_count == fMaxSum) return 1;
  else return 0;  
}

Int_t TaStripChart::AddBinVal(const Double_t value){

 Int_t i =1;
 while (i < fSC_num_of_chan)
   {
   fSC_array->AddAt(fSC_array->At(i), i-1); 
   fSC_array->AddAt(value,fSC_num_of_chan-1);
   i++;
   }
 return 0;
}

Int_t TaStripChart::Reset(){
  fSC_array->Reset();
  fSC_sum=0;
  return 0;
}

Int_t TaStripChart::Fill(){
  Int_t i;
  for (i=0; i< fSC_num_of_chan; i++)
    fSC_hist->SetBinContent(i+1, fSC_array->At(i)); 
  //  fSC_hist->Print("range");
  return 0;
}

void TaStripChart::SCDraw(){
  fSC_hist->DrawCopy();
}

void TaStripChart::SCDrawInit(){
  fSC_hist->Draw();
}

