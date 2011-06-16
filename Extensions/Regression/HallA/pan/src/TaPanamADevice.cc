//////////////////////////////////////////////////////////////////////////
//
//     HALL A C++/ROOT Parity Analyzer  Pan (Online Monitor version)           
//
//        TaPanamADevice.cc   (implementation file)
//        ^^^^^^^^^^^^^^^
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaPanamADevice.cc,v 1.3 2003/07/31 16:11:59 rsholmes Exp $
//
//////////////////////////////////////////////////////////////////////////
//
//    List of histos and stripchart for the corresponding Pan device in 
//    the datamap with extra analyzed data (asym or diff). 
//
//////////////////////////////////////////////////////////////////////////

#include "TF1.h"
#include "TPaveText.h"
#include "TaPanamADevice.hh"
#include "VaPair.hh"
#include "TaEvent.hh"
#include "TDirectory.h"

ClassImp(TaPanamADevice)

 TaPanamADevice::TaPanamADevice():TaPanamDevice(),
 fSAData(0),
 fSADataRMS(0), 
 fSADataN(0), 
 fHAData(0),
 fHADataN(0),
 fSANumOfChan(0),
 fSANumOfEvPerChan(0),
 fHAbins(0),
 fAColor(0),
 fXSAmin(0),fXSAmax(0),
  fXHAmin(0),fXHAmax(0),fWhichData(kFALSE)
{}
TaPanamADevice::TaPanamADevice(char* name, Int_t namekey, 
                               Int_t SNumChan, Int_t SNumEvperChan,
                               Int_t SANumChan, Int_t SANumEvperChan,
                               Int_t histobin,
                               Axis_t xsmin, Axis_t xsmax,
                               Axis_t xhmin, Axis_t xhmax, 
                               Int_t histoabin,  
                               Axis_t xsamin, Axis_t xsamax,
                               Axis_t xhamin, Axis_t xhamax, 
                               Int_t color, Int_t acolor, Bool_t datatype)
: TaPanamDevice(name,namekey,SNumChan,SNumEvperChan,histobin, 
                xsmin,xsmax,xhmin,xhmax,color),
 fSAData(0),
 fSADataRMS(0), 
 fSADataN(0),
 fHAData(0),
 fHADataN(0)
{
  //cout<<"TaPanamADevice: creating object ="<<this<<endl; 
  fHArray.clear();
  fSANumOfChan      = SANumChan;
  fSANumOfEvPerChan = SANumEvperChan;
  fHAbins   = histoabin;
  fXSAmin = xsamin;
  fXSAmax = xsamax;
  fXHAmin = xhamin;
  fXHAmax = xhamax;
  fAColor = acolor;
  fWhichData = datatype;
  Init();
}  

TaPanamADevice::~TaPanamADevice()
{
  //cout<<"TaPanamADevice: deleting object ="<<this<<endl; 
 cout<<"fSAData "<<fSAData<<endl;
 cout<<"fSADataRMS "<<fSADataRMS<<endl;
 cout<<"fSADataN "<<fSADataN<<endl;
 cout<<"fHAData "<<fHAData<<endl;
 cout<<"fHADataN "<<fHADataN<<endl;
//    delete fSAData; 
//    delete fSADataRMS ; 
//    delete fSADataN; 
// gDirectory->GetList()->Print();
// TH1D *h = (TH1D*) gDirectory->GetList()->FindObject("H_bcm1_Asy");
 //h->SetDirectory(0);
 //cout<<"removed histo h with name"<<h->GetName()<<" from list "<<endl; 
 //delete h;
 //delete fHADataN;
#ifdef LEAKING
 ++fLeaKDelAHisto; 
 ++fLeaKDelASC; 
 ++fLeaKDelASCN; 
 ++fLeaKDelASCRMS; 
 ++fLeaKDelAHistoN; 
#endif
}

TaPanamADevice::TaPanamADevice(const TaPanamADevice& md):TaPanamDevice(md)
{
  fADataVal          = md.fADataVal;          
  fADataNVal          = md.fADataNVal;          
  fHArray                = md.fHArray;
  fSANumOfChan           = md.fSANumOfChan;
  fSANumOfEvPerChan      = md.fSANumOfEvPerChan;
  fHAbins                = md.fHAbins;
  fXSAmin                = md.fXSAmin;
  fXSAmax                = md.fXSAmax;
  fXHAmin                = md.fXHAmin;
  fXHAmax                = md.fXHAmax;
  fAColor                = md.fAColor;
  fColor                 = md.fAColor;
//   if(md.fSAData) 
//      {         
//       fSAData   = new TaStripChart(md.fSAData->GetName(),
//                                    md.fSAData->GetDataName(),
//                                    fSANumOfChan,fSANumOfEvPerChan,
//                                    fXSAmin,fXSAmax); 
      fSAData  = md.fSAData;
//      } 
//   else fSAData = NULL;
//   if(md.fSADataRMS) 
//      {         
//       fSADataRMS   = new TaStripChart(md.fSADataRMS->GetName(),
//                                       md.fSADataRMS->GetDataName(),
//                                       fSANumOfChan,fSANumOfEvPerChan,
//                                       fXSAmin,fXSAmax); 
      fSADataRMS  = md.fSADataRMS;
//      } 
//   else fSADataRMS = NULL;
//   if(md.fHAData) 
//      {         
//       fHAData   = new TH1D(md.fHAData->GetName(),md.fHAData->GetTitle(),
//                            md.fHAbins,fXHAmin,fXHAmax); 
      fHAData  = md.fHAData;
//      } 
//   else fHAData = NULL;
  fSADataN = md.fSADataN;
  fHADataN = md.fHADataN;
}

TaPanamADevice& TaPanamADevice::operator=(const TaPanamADevice &md)
{
if ( this != &md )
    {
     TaPanamDevice::operator=(md);
//      if(fSAData)        delete fSAData; 
//      if(fSADataRMS)    delete fSADataRMS; 
//      if(fHAData)        delete fHAData;
     fADataVal          = md.fADataVal;          
     fHArray            = md.fHArray;
     fSANumOfChan       = md.fSANumOfChan;
     fSANumOfEvPerChan  = md.fSANumOfEvPerChan;
     fHAbins            = md.fHAbins;
     fXSAmin            = md.fXSAmin;
     fXSAmax            = md.fXSAmax;
     fXHAmin            = md.fXHAmin;
     fXHAmax            = md.fXHAmax;
     fAColor            = md.fAColor;
//      if (md.fSAData) 
//        {         
//         fSAData   = new TaStripChart(md.fSAData->GetName(),md.fSAData->GetDataName(),
//                                     fSANumOfChan,fSANumOfEvPerChan,
//                                     fXSAmin,fXSAmax); 
        fSAData  = md.fSAData;
//        } 
//      else fSAData = NULL;
//      if (md.fSADataRMS) 
//        {         
//         fSADataRMS   = new TaStripChart(md.fSADataRMS->GetName(),md.fSADataRMS->GetDataName(),
//                                        fSANumOfChan,fSANumOfEvPerChan,
//                                        fXSAmin,fXSAmax); 
        fSADataRMS  = md.fSADataRMS;
//        } 
//      else fSADataRMS = NULL;
//      if(md.fHAData) 
//        {         
//         fHAData   = new TH1D(md.fHAData->GetName(),md.fHAData->GetTitle(),
//                             md.fHAbins,fXHAmax,fXHAmax); 
        fHAData  = md.fHAData;
//        } 
//      else fHAData = NULL;
     fSADataN = md.fSADataN;
     fHADataN = md.fHADataN;
     return *this;     
    }
 else
   {
     cout<<"warning ! TaPanamADevice assignment to self !"<<endl;
     return *this;
   }
}
  
void
TaPanamADevice::Init()
{
  string dname;
  fHArray.push_back(fHData);
  if (!fWhichData)
    {
     dname = string("S_")+string(fName) + string("_Asy"); 
     fSAData    = new TaStripChart((char*)dname.c_str(),(char*)dname.c_str(),
                                   fSANumOfChan,fSANumOfEvPerChan,
                                   fXSAmin,fXSAmax);   
     fSCArray.push_back(fSAData); 
     dname = string("S_")+string(fName) + string("_Asy_RMS"); 
     fSADataRMS = new TaStripChart((char*)dname.c_str(),(char*)dname.c_str(),
                                  fSANumOfChan,fSANumOfEvPerChan,
                                  fXSAmin,fXSAmax);
     fSCArray.push_back(fSADataRMS); 
     dname = string("S_")+string(fName) + string("_AsyN"); 
     fSADataN   = new TaStripChart((char*)dname.c_str(),(char*)dname.c_str(),
                                   fSANumOfChan,fSANumOfEvPerChan,
                                   fXSAmin,fXSAmax);   
     fSCArray.push_back(fSADataN); 
     dname = string("H_")+string(fName) + string("_Asy"); 
     fHAData    = new TH1D((char*) dname.c_str(), (char*) dname.c_str(),
                            fHAbins,fXHAmin,fXHAmax); 
     fHArray.push_back(fHAData);
     dname = string("H_")+string(fName) + string("_AsyN"); 
     fHADataN   = new TH1D((char*) dname.c_str(), (char*) dname.c_str(),
                            fHAbins,fXHAmin,fXHAmax); 
     fHArray.push_back(fHADataN);
#ifdef LEAKING
 ++fLeaKNewAHisto;++fLeakNewASC;++fLeaKNewAHistoN;++fLeakNewASCN;++fLeakNewASCRMS; 
#endif
    }
  else
    {
     dname = string("S_")+string(fName) + string("_Diff"); 
     fSAData    = new TaStripChart((char*)dname.c_str(),(char*)dname.c_str(),
                                   fSANumOfChan,fSANumOfEvPerChan,
                                   fXSAmin,fXSAmax);   
     fSCArray.push_back(fSAData); 
     dname = string("S_")+string(fName) + string("_Diff_RMS"); 
     fSADataRMS = new TaStripChart((char*)dname.c_str(),(char*)dname.c_str(),
                                  fSANumOfChan,fSANumOfEvPerChan,
                                  fXSAmin,fXSAmax);
     fSCArray.push_back(fSADataRMS); 
     dname = string("S_")+string(fName) + string("_DiffN"); 
     fSADataN   = new TaStripChart((char*)dname.c_str(),(char*)dname.c_str(),
                                   fSANumOfChan,fSANumOfEvPerChan,
                                   fXSAmin,fXSAmax);   
     fSCArray.push_back(fSADataN); 
     dname = string("H_")+string(fName) + string("_Diff"); 
     fHAData    = new TH1D((char*) dname.c_str(), (char*) dname.c_str(),
                            fHAbins,fXHAmin,fXHAmax); 
     fHArray.push_back(fHAData);     
     dname = string("H_")+string(fName) + string("_DiffN"); 
     fHADataN    = new TH1D((char*) dname.c_str(), (char*) dname.c_str(),
                            fHAbins,fXHAmin,fXHAmax); 
     fHArray.push_back(fHADataN);     
#ifdef LEAKING
 ++fLeaKNewAHisto;++fLeakNewASC;++fLeaKNewAHistoN;++fLeakNewASCN;++fLeakNewASCRMS; 
#endif
    }
}

void 
TaPanamADevice::FillFromPair(VaPair& pair)
 {
   if (fDevicekey !=0)
     {
       if (!fWhichData) 
         {
	   fADataVal = pair.GetAsy(fDevicekey)*1E6;
 	  if (fSAData->DataSum(fADataVal))
            { 
 	     fSAData->AddBinVal(fSAData->GetSumNorm());
             fSAData->Fill();
            }
          fHAData->Fill(fADataVal);
	  if(fSADataRMS->DataSum(fHData->GetRMS()))
            { 
	     fSADataRMS->AddBinVal(fSDataRMS->GetSumNorm());
             fSADataRMS->Fill();
            }         
	 }
       else
        {
	  fADataVal = pair.GetDiff(fDevicekey)*1E3;
	  if (fSAData->DataSum(fADataVal))
            { 
	     fSAData->AddBinVal(fSAData->GetSumNorm());
             fSAData->Fill();
            }
          fHAData->Fill(fADataVal);
	  if (fSADataRMS->DataSum(fHAData->GetRMS()))
            { 
	      fSADataRMS->AddBinVal(fSADataRMS->GetSumNorm());
              fSADataRMS->Fill();
            }         
	}
     }

}

void 
TaPanamADevice::FillFromPair(VaPair& pair, Int_t normdev)
 {
   if (fDevicekey !=0)
     {
       if (!fWhichData) 
         {
	   fADataVal = pair.GetAsy(fDevicekey)*1E6;
	   fADataNVal = (pair.GetAsy(fDevicekey) - pair.GetAsy(normdev))*1E6;
 	  if (fSAData->DataSum(fADataVal))
            { 
 	     fSAData->AddBinVal(fSAData->GetSumNorm());
             fSAData->Fill();
            }
 	  if (fSADataN->DataSum(fADataNVal))
            { 
 	     fSADataN->AddBinVal(fSADataN->GetSumNorm());
             fSADataN->Fill();
            }
          fHAData->Fill(fADataVal);
          fHADataN->Fill(fADataNVal);
	  if(fSADataRMS->DataSum(fHData->GetRMS()))
            { 
	     fSADataRMS->AddBinVal(fSDataRMS->GetSumNorm());
             fSADataRMS->Fill();
            }         
	 }
       else
        {
	  fADataVal = pair.GetDiff(fDevicekey)*1E3;
	  fADataNVal = (pair.GetDiff(fDevicekey) - pair.GetDiff(normdev))*1E3;
	  if (fSAData->DataSum(fADataVal))
            { 
	     fSAData->AddBinVal(fSAData->GetSumNorm());
             fSAData->Fill();
            }
	  if (fSADataN->DataSum(fADataNVal))
            { 
	     fSADataN->AddBinVal(fSADataN->GetSumNorm());
             fSADataN->Fill();
            }
          fHAData->Fill(fADataVal);
          fHADataN->Fill(fADataNVal);
	  if (fSADataRMS->DataSum(fHAData->GetRMS()))
            { 
	      fSADataRMS->AddBinVal(fSADataRMS->GetSumNorm());
              fSADataRMS->Fill();
            }         
	}
     }

}


void 
TaPanamADevice::DrawHPad(UInt_t plotidx, UInt_t optionfit )
{
  if (plotidx <= (UInt_t)fHArray.size())
    { 
     fHArray[plotidx]->Draw();
     if ( optionfit ) 
       {
        fHArray[plotidx]->Fit("gaus");
        fHArray[plotidx]->GetFunction("gaus")->SetLineColor(fHArray[plotidx]->GetLineColor());
       } 
    }  
  else cout<<" can't plot, index is bigger than array size!!! \n";
}

void 
TaPanamADevice::InitAStat(Double_t  x1, Double_t y1, Double_t x2, Double_t y2,Int_t textfont=22, 
                        Double_t textsize=0.04148)
{
 fAStat = new TPaveText(x1,y1,x2,y2,"brNDC");
 fAStat->SetBorderSize(1);  fAStat->SetFillColor(10);
 fAStat->SetTextAlign(12);  fAStat->SetTextFont(textfont);
 fAStat->SetTextSize(textsize);
 fANStat = new TPaveText(x1,y1,x2,y2,"brNDC");
 fANStat->SetBorderSize(1);  fAStat->SetFillColor(10);
 fANStat->SetTextAlign(12);  fAStat->SetTextFont(textfont);
 fANStat->SetTextSize(textsize);
}



void 
TaPanamADevice::UpdateAStat()
{ 
  fAStat->Clear();
  //string gdname = string(GetName());
  fAStat->SetTextColor(fHAData->GetLineColor());
  if ( fHAData->GetEntries() > 0) fASigmaAverage = (fHAData->GetRMS())/(sqrt(fHAData->GetEntries()));
  sprintf(fAMeanp,"<%s>= %2.2f",fHAData->GetName(),fHAData->GetMean());
  sprintf(fASigAvep," +- %2.2f",fASigmaAverage);
  strcat(fAMeanp,fASigAvep);
  sprintf(fARMSp," rms  = %2.2f", fHAData->GetRMS());
  fAStat->AddText(fAMeanp);
  fAStat->AddText(fARMSp); 
  fAStat->Draw(); 
}

void 
TaPanamADevice::UpdateANStat()
{ 
  fANStat->Clear();
  //string gdname = string(GetName());
  fANStat->SetTextColor(fHADataN->GetLineColor());
  if ( fHADataN->GetEntries() > 0) fASigmaAverage = (fHADataN->GetRMS())/(sqrt(fHADataN->GetEntries()));
  sprintf(fAMeanp,"<%s>= %2.2f",fHADataN->GetName(),fHADataN->GetMean());
  sprintf(fASigAvep," +- %2.2f",fASigmaAverage);
  strcat(fAMeanp,fASigAvep);
  sprintf(fARMSp," rms  = %2.2f", fHADataN->GetRMS());
  fANStat->AddText(fAMeanp);
  fANStat->AddText(fARMSp); 
  fANStat->Draw(); 
}

#ifdef LEAKING
void TaPanamADevice::Leaking()
{
  // Check for memory leaks in VaAnalysis
  cout << "Memory leak test for TaPanamADevice:" << endl;
  cout << "Histo new " << fLeaKNewAHisto
       << " del " << fLeakDelAHisto
       << " diff " <<fLeakNewAHisto-fLeakDelAHisto << endl;
  cout << "SC  new " << fLeakNewASC
       << " del " << fLeakDelASC
       << " diff " <<fLeakNewASC-fLeakDelASC << endl;
  cout << "SCRMS  new " << fLeakNewASCRMS
       << " del " << fLeakDelASCRMS
       << " diff " <<fLeakNewASCRMS-fLeakDelASCRMS << endl;
  cout << "HistoN  new " << fLeakNewAHistoN
       << " del " << fLeakDelASHistoN
       << " diff " <<fLeakNewAHistoN-fLeakDelAHistoN << endl;
  cout << "SCN  new " << fLeakNewASCN
       << " del " << fLeakDelASCN
       << " diff " <<fLeakNewASCN-fLeakDelASCN << endl;
}
#endif
