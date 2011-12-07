//////////////////////////////////////////////////////////////////////////
//
//     HALL A C++/ROOT Parity Analyzer  Pan (Online Monitor version)           
//
//        TaPanamDevice.cc   (implementation file)
//        ^^^^^^^^^^^^^^^
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaPanamDevice.cc,v 1.3 2003/07/31 16:11:59 rsholmes Exp $
//
//////////////////////////////////////////////////////////////////////////
//
//    List of histos and stripchart for the corresponding Pan device in 
//    the datamap. 
//
//////////////////////////////////////////////////////////////////////////

#include "TF1.h"
#include <stdio.h>
#include "TaPanamDevice.hh"
#include "VaPair.hh"
#include "TaEvent.hh"

ClassImp(TaPanamDevice)

TaPanamDevice::TaPanamDevice():
 fSData(0),
 fSDataRMS(0), 
 fHData(0),
 fTitle(0),fStat(0),
 fName(0),fDevicekey(0),
 fSNumOfChan(0),
 fSNumOfEvPerChan(0),
 fHbins(0),
 fColor(0),
 fXSmin(0),fXSmax(0),
 fXHmin(0),fXHmax(0)
{}

TaPanamDevice::TaPanamDevice(char* name, Int_t namekey,
                                 Int_t SNumChan, Int_t SNumEvperChan,
                                 Int_t histobin, 
                                 Axis_t xsmin, Axis_t xsmax, 
                                 Axis_t xhmin, Axis_t xhmax, 
                                 Int_t color):
 fSData(0),
 fSDataRMS(0), 
 fHData(0),
 fTitle(0),
 fStat(0)
{
  TH1::AddDirectory(kFALSE);
  //cout<<"TaPanamDevice: creating object ="<<this<<endl; 
  fSCArray.clear();
  fName      = name;
  fDevicekey = namekey;
  fSNumOfChan      = SNumChan;
  fSNumOfEvPerChan = SNumEvperChan;
  fHbins   = histobin;
  fXSmin = xsmin;
  fXSmax = xsmax;
  fXHmin = xhmin;
  fXHmax = xhmax;
  fColor = color;
  Init();
}

TaPanamDevice::~TaPanamDevice()
{
  //cout<<"TaPanamDevice: deleting object ="<<this<<endl;    
//     delete fSData;  
//     delete fSDataRMS; 
//     delete fHData;
//     delete fStat;
//     delete fTitle;
#ifdef LEAKING
   ++fLeakDelHisto;
   ++fLeakDelSC;
   ++fLeakDelSCRMS;
   Leaking();
#endif  
}

TaPanamDevice::TaPanamDevice(const TaPanamDevice& md)
{
  fDataVal               = md.fDataVal;
  fSCArray               = md.fSCArray;
  fName                  = md.fName;
  fDevicekey             = md.fDevicekey;
  fSNumOfChan            = md.fSNumOfChan;
  fSNumOfEvPerChan       = md.fSNumOfEvPerChan;
  fHbins                 = md.fHbins;
  fXSmin                 = md.fXSmin;
  fXSmax                 = md.fXSmax;
  fXHmin                 = md.fXHmin;
  fXHmax                 = md.fXHmax;
  fColor                 = md.fColor;
//   if(!md.fSData) 
//      {         
//       fSData   = new TaStripChart(md.fSData->GetName(),md.fSData->GetDataName(),
//                               fSNumOfChan,fSNumOfEvPerChan,
//                               fXSmin,fXSmax); 
      fSData  = md.fSData;
//      } 
//   else fSData = NULL;
//   if(md.fSDataRMS) 
//      {         
//       fSDataRMS = new TaStripChart(md.fSDataRMS->GetName(),md.fSDataRMS->GetDataName(),
//                                    fSNumOfChan,fSNumOfEvPerChan,
//                                    fXSmin,fXSmax); 
      fSDataRMS  = md.fSDataRMS;
//      } 
//   else fSDataRMS = NULL;
//   if(md.fHData) 
//      {         
//       fHData   = new TH1D(md.fHData->GetName(),md.fHData->GetTitle(),
//                           md.fHbins,fXHmin,fXHmax); 
//       cout<<" Histogram "<<fHData->GetName()<<" created"<<endl;
      fHData  = md.fHData;
//      } 
//   else fHData = NULL;
//   if (md.fStat)
//     {
      fStat = md.fStat;
//     }
//   else fStat = NULL;
//   if (md.fTitle)
//     {
      fTitle = md.fTitle;
//     }
//   else fTitle = NULL;
}

TaPanamDevice& TaPanamDevice::operator=(const TaPanamDevice &md)
{
if ( this != &md )
    {
//      if(fSData)        delete fSData; 
//      if(fSDataRMS)     delete fSDataRMS; 
//      if(fHData)        delete fHData;
     fDataVal          = md.fDataVal;
     fSCArray          = md.fSCArray;
     fName             = md.fName;
     fDevicekey        = md.fDevicekey;
     fSNumOfChan       = md.fSNumOfChan;
     fSNumOfEvPerChan  = md.fSNumOfEvPerChan;
     fHbins            = md.fHbins;
     fXSmin            = md.fXSmin;
     fXSmax            = md.fXSmax;
     fXHmin            = md.fXHmin;
     fXHmax            = md.fXHmax;
     fColor            = md.fColor;
//      if (md.fSData) 
//        {         
//         fSData   = new TaStripChart(md.fSData->GetName(),md.fSData->GetDataName(),
//                                     fSNumOfChan,fSNumOfEvPerChan,
//                                     fXSmin,fXSmax); 
        fSData  = md.fSData;
//        } 
//      else fSData = NULL;
//      if (md.fSDataRMS) 
//        {         
//         fSDataRMS   = new TaStripChart(md.fSDataRMS->GetName(),md.fSDataRMS->GetDataName(),
//                                        fSNumOfChan,fSNumOfEvPerChan,
//                                        fXSmin,fXSmax); 
        fSDataRMS  = md.fSDataRMS;
//        } 
//      else fSDataRMS = NULL;
//      if(md.fHData) 
//        {         
//         fHData   = new TH1D(md.fHData->GetName(),md.fHData->GetTitle(),
//                             md.fHbins,fXHmax,fXHmax); 
        fHData  = md.fHData;
//        } 
//      else fHData = NULL;
//      if (md.fStat)
//        {
         fStat = md.fStat;
//         }
//      else fStat = NULL;
//      if (md.fTitle)
//        {
        fTitle = md.fTitle;
//        }
//      else fTitle = NULL;
     return *this;     
    }
 else
   {
     cout<<"warning ! TaPanamDevice assignment to self !"<<endl;
     return *this;
   }
}  

void 
TaPanamDevice::FillFromEvent(TaRun& run){
  if (fDevicekey !=0)
    {
      fDataVal = run.GetEvent().GetData(fDevicekey);
     if (fSData->DataSum(fDataVal))
       { 
    	fSData->AddBinVal(fSData->GetSumNorm());
        fSData->Fill();
       }
     fHData->Fill(fDataVal); 
     if (fSDataRMS->DataSum(fHData->GetRMS()))
       { 
	fSDataRMS->AddBinVal(fSDataRMS->GetSumNorm());
        fSDataRMS->Fill();
       }         
    }
}

void 
TaPanamDevice::InitSCPad(UInt_t plotidx)
{
  if (plotidx <=  (UInt_t) fSCArray.size()) 
    {
     cout<<" sc "<<fSCArray[plotidx]->GetSCHist()->GetName()<<" drawn init \n";
     fSCArray[plotidx]->SCDrawInit();
     //fTitle->Draw();
     //     fStat->Draw(); 
    }
  else cout<<" can't plot, index is bigger than array size!!! \n";
}

void 
TaPanamDevice::DisplaySC(UInt_t plotidx)
{
  if (plotidx <= (UInt_t)fSCArray.size()) 
    { 
     fSCArray[plotidx]->SCDraw();
     //     cout<<" strip of "<<fName<<" drawn \n";
    }
  else cout<<" can't plot, index is bigger than array size!!! \n";
}

void 
TaPanamDevice::DrawHPad(UInt_t optionfit)
{
 fHData->Draw();
 if ( optionfit ) 
   {
    fHData->Fit("gaus");
    fHData->GetFunction("gaus")->SetLineColor(fHData->GetLineColor());
   } 
}

void 
TaPanamDevice::InitStat(Double_t  x1, Double_t y1, Double_t x2, Double_t y2,Int_t textfont=22, 
                        Double_t textsize=0.04148)
{
 fStat = new TPaveText(x1,y1,x2,y2,"brNDC");
 fStat->SetBorderSize(1);  fStat->SetFillColor(10);
 fStat->SetTextAlign(12);  fStat->SetTextFont(textfont);
 fStat->SetTextSize(0.04148);
}

void 
TaPanamDevice::UpdateStat()
{

 fStat->Clear();
 fSigmaAverage = (fHData->GetRMS())/(sqrt(fHData->GetEntries()));
 sprintf(fMeanp,"<%s>= %2.2f",fHData->GetName(),fHData->GetMean());
 sprintf(fSigAvep," +- %2.2f",fSigmaAverage);
 strcat(fMeanp,fSigAvep);
 sprintf(fRMSp," rms  = %2.2f", fHData->GetRMS());
 fStat->SetTextColor(fHData->GetLineColor());
 fStat->AddText(fMeanp);
 fStat->AddText(fRMSp);
 fStat->Draw(); 
}

void TaPanamDevice::SetTiltle()
{
 fTitle = new TPaveText(0.076555,0.901015,0.578947,0.951777,"blNDC");
 fTitle->SetName("title");
 fTitle->SetBorderSize(0);
 fTitle->SetFillColor(10);
 fTitle->AddText(fHData->GetName());
 fTitle->SetTextColor(fHData->GetLineColor()); 
 fTitle->Draw();
}

void
TaPanamDevice::Init()
{
 string dname;
 dname = string("S_")+string(fName); 
 fSData    = new TaStripChart((char*)dname.c_str(),(char*)dname.c_str(),
                              fSNumOfChan,fSNumOfEvPerChan,
                              fXSmin,fXSmax);   
 fSCArray.push_back(fSData); 

 dname = string("S_")+string(fName) + string("_RMS");
 fSDataRMS = new TaStripChart((char*)dname.c_str(),(char*)dname.c_str(),
                              fSNumOfChan,fSNumOfEvPerChan,
                              fXSmin,fXSmax);
 fSCArray.push_back(fSDataRMS); 

 dname = string("H_")+string(fName); 
 fHData    = new TH1D((char*) dname.c_str(), (char*) dname.c_str(),
                      fHbins,fXHmin,fXHmax); 

#ifdef LEAKING
   ++fLeakNewHisto;
   ++fLeakNewSC;
   ++fLeakNewSCRMS;
#endif  
}

TH1D*
TaPanamDevice::GetPlot(char* const plotname, Int_t const plottype) const 
{
  TH1D* theplot=NULL;
  if (plottype ==1)
    {
      for (vector<TaStripChart*>::const_iterator sc=fSCArray.begin();sc!=fSCArray.end();sc++)
	{
	  if ((*sc)->GetSCHist()->GetName() == plotname) 
            {
             theplot = (*sc)->GetSCHist();  
	    }
          if (theplot!=NULL) break;
        }
    }
  if(plottype ==2) theplot = fHData;   
  return theplot;
}
#ifdef LEAKING
void TaPanamDevice::Leaking()
{
  // Check for memory leaks in VaAnalysis
  cout << "Memory leak test for TaPanamDevice:" << endl;
  cout << "Histo new " << fLeaKNewHisto
       << " del " << fLeakDelHisto
       << " diff " <<fLeakNewHisto-fLeakDelHisto << endl;
  cout << "SC  new " << fLeakNewSC
       << " del " << fLeakDelSC
       << " diff " <<fLeakNewSC-fLeakDelSC << endl;
  cout << "SCRMS  new " << fLeakNewSCRMS
       << " del " << fLeakDelSCRMS
       << " diff " <<fLeakNewSCRMS-fLeakDelSCRMS << endl;
}
#endif
