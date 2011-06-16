//////////////////////////////////////////////////////////////////////////
//
//     HALL A C++/ROOT Parity Analyzer  Pan (Online Monitor version)           
//
//        TaPanamMultiDevice.cc   (implementation file)
//        ^^^^^^^^^^^^^^^^^^^^^
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaPanamMultiDevice.cc,v 1.2 2003/07/31 16:11:59 rsholmes Exp $
//
//////////////////////////////////////////////////////////////////////////
//
//    List of PanamDevice object. 
// 
//
//////////////////////////////////////////////////////////////////////////

#include "TaPanamMultiDevice.hh"

ClassImp(TaPanamMultiDevice)
 
TaPanamMultiDevice::TaPanamMultiDevice():
 fStkStrip(0),   
 fStkStripRMS(0),   
 fStkHisto(0),   
 fStkAStrip(0),   
 fStkAStripRMS(0),   
 fStkAHisto(0),
 fPDev(0),fPADev(0)
{
 fMyPD.clear();
 fMyPAD.clear();
 fColor.clear();
}

TaPanamMultiDevice::TaPanamMultiDevice(Char_t* name):
 fStkStrip(0),   
 fStkStripRMS(0),   
 fStkHisto(0),   
 fStkAStrip(0),   
 fStkAStripRMS(0),   
 fStkAHisto(0),  
 fPDev(0),fPADev(0)
{
 cout<<"TaPanamMultiDevice: creating  object ="<<this<<endl; 
 fMyPD.clear();
 fMyPAD.clear();
 fColor.clear();
 fName = name;
}

TaPanamMultiDevice::~TaPanamMultiDevice()
{ 
 cout<<"TaPanamMultiDevice: Deleting object ="<<this<<endl; 

 fMyPD.clear();
 fMyPAD.clear(); 
 fColor.clear();
 if (fStkStrip) delete fStkStrip; 
 if (fStkStripRMS) delete fStkStripRMS; 
 if (fStkHisto) delete fStkHisto; 
 if (fStkAStrip) delete fStkAStrip; 
 if (fStkAStripRMS) delete fStkAStripRMS; 
 if (fStkAHisto) delete fStkAHisto; 
}

TaPanamMultiDevice::TaPanamMultiDevice(const TaPanamMultiDevice& md)
{
  fName=md.fName;
  fColor=md.fColor;
  fPD=md.fPD;
  fPAD=md.fPAD; 
  fMyPD=md.fMyPD;
  fMyPAD=md.fMyPAD;
  fS=md.fS;
  fStkStrip=md.fStkStrip; 
  fStkStripRMS=md.fStkStripRMS; 
  fStkHisto=md.fStkHisto; 
  fStkAStrip=md.fStkAStrip;
  fStkAStripRMS=md.fStkAStripRMS;
  fStkAHisto=md.fStkAHisto; 
  fPDev=md.fPDev;
  fPADev=md.fPADev;
}

TaPanamMultiDevice& TaPanamMultiDevice::operator=(const TaPanamMultiDevice &md)
{
if ( this != &md )
    {
     fName=md.fName;
     fColor=md.fColor;
     fPD=md.fPD;
     fPAD=md.fPAD; 
     fMyPD=md.fMyPD;
     fMyPAD=md.fMyPAD;
     fS=md.fS;
     fStkStrip=md.fStkStrip; 
     fStkStripRMS=md.fStkStripRMS; 
     fStkHisto=md.fStkHisto; 
     fStkAStrip=md.fStkAStrip;
     fStkAStripRMS=md.fStkAStripRMS;
     fStkAHisto=md.fStkAHisto; 
     fPDev=md.fPDev;
     fPADev=md.fPADev;
    return *this;     
     }
 else
   {
     cout<<"warning ! TaPanamMultiDevice assignment to self !"<<endl;
     return *this;
   }
}
void
TaPanamMultiDevice::InsertDevice(TaPanamDevice* thedev)
{ 
 fMyPD.push_back(thedev);
}

void 
TaPanamMultiDevice::InsertADevice(TaPanamADevice* theAdev)
{
 fMyPAD.push_back(theAdev);
}

void 
TaPanamMultiDevice::CopyDev(vector<TaPanamDevice*> vdev)
{
  fMyPD.clear();
  fMyPD = vdev;
}

void 
TaPanamMultiDevice::CopyADev(vector<TaPanamADevice*> vadev)
{
  fMyPAD.clear();
  fMyPAD = vadev;
}

Int_t TaPanamMultiDevice::TypeOfDev()
{
 Int_t thedevtype;
 if (fMyPD.size() > 0 ) thedevtype = 0; 
 else if (fMyPAD.size() >0 ) thedevtype = 1;
 else thedevtype = -1; 
 return thedevtype;
} 

void 
TaPanamMultiDevice::DrawStack(Int_t whichstack,Int_t optstat)
{
  //cout<<" fS size "<<fS.size()<<endl;
  //cout<<"fS[whichstack] ="<<fS[whichstack]<<endl;
  //if (fS.size() > 0) fS[whichstack]->Print();
 if (fS.size() > 0) fS[whichstack]->Draw("nostack");
 if (whichstack >= 2) 
  {
    cout<<"update stats"<<endl; 
   UpdateStats(1);
   cout<<"updated ADEV"<<endl; 
  }
 else  
 { 
  UpdateStats(0);
 }
}

void 
TaPanamMultiDevice::InitColors(Int_t devsize)
{
  fColor.clear();
  switch (devsize)
    {
    case 1:
      fColor.push_back(4);
      break;
    case 2:
      fColor.push_back(9);
      fColor.push_back(6);
      break;
    case 3:
      fColor.push_back(4);
      fColor.push_back(5);
      fColor.push_back(6);
      break;
    case 4:
      fColor.push_back(2);
      fColor.push_back(6);
      fColor.push_back(4);
      fColor.push_back(7);
      break;
    case 5:
      fColor.push_back(2);
      fColor.push_back(4);
      fColor.push_back(5);
      fColor.push_back(6);
      fColor.push_back(7);
      break;
    case 6:
      fColor.push_back(2);
      fColor.push_back(3);
      fColor.push_back(4);
      fColor.push_back(5);
      fColor.push_back(6);
      fColor.push_back(7);
      break;
    case 7:
      fColor.push_back(2);
      fColor.push_back(3);
      fColor.push_back(4);
      fColor.push_back(5);
      fColor.push_back(6);
      fColor.push_back(7);
      fColor.push_back(8);
      break;
    case 8:
      fColor.push_back(2);
      fColor.push_back(3);
      fColor.push_back(4);
      fColor.push_back(5);
      fColor.push_back(6);
      fColor.push_back(7);
      fColor.push_back(8);
      fColor.push_back(9);
      break;
    case 9:
      fColor.push_back(2);
      fColor.push_back(3);
      fColor.push_back(4);
      fColor.push_back(5);
      fColor.push_back(6);
      fColor.push_back(7);
      fColor.push_back(8);
      fColor.push_back(9);
      fColor.push_back(30);
      break;
    }

} 

Int_t
TaPanamMultiDevice::InitStacks()
{
  cout<<" Initialization of STACK named "<<fName<<endl;
  string dname;
  string suffix;
  Int_t colnum =0;
  TaPanamADevice *adev;
  if (fMyPD.size()) 
    {
     InitColors(fMyPD.size());      
     dname = string("SST_")+string(fName); 
     fStkStrip = new THStack((char*) dname.c_str(),(char*) dname.c_str());
     dname = string("SST_RMS_")+string(fName); 
     fStkStripRMS = new THStack((char*) dname.c_str(),(char*) dname.c_str());
     dname = string("HST_")+string(fName); 
     fStkHisto = new THStack((char*) dname.c_str(),(char*) dname.c_str());     
     for (vector<TaPanamDevice*>::const_iterator i  = fMyPD.begin(); 
                                              i != fMyPD.end(); 
                                              i++) 
       {
	(*i)->GetSData()->GetSCHist()->SetLineColor(fColor[colnum]);
	(*i)->GetSDataRMS()->GetSCHist()->SetLineColor(fColor[colnum]);
	(*i)->GetHData()->SetLineColor(fColor[colnum]);
        fStkStrip->Add((*i)->GetSData()->GetSCHist());
        fStkStrip->Add((*i)->GetSDataRMS()->GetSCHist());
        fStkHisto->Add((*i)->GetHData());
        colnum++;
       }
     fS.push_back(fStkStrip); 
     fS.push_back(fStkStripRMS); 
     fS.push_back(fStkHisto); 
    }
  if (fMyPAD.size())
    {
     InitColors(fMyPAD.size());      
     adev = fMyPAD[0];
     dname = string("SST_")+string(fName); 
     fStkStrip = new THStack((char*) dname.c_str(),(char*) dname.c_str());
     dname = string("SST_RMS_")+string(fName); 
     fStkStripRMS = new THStack((char*) dname.c_str(),(char*) dname.c_str());
     dname = string("SST_")+string(fName); 
     fStkHisto = new THStack((char*) dname.c_str(),(char*) dname.c_str());
     //     cout<<adev->GetHAData()->GetName()<<endl;
     //     cout<<" result strncmp _Asy for naming stack : "<<strncmp(adev->GetHAData()->GetName(),"_Asy",4)<<endl; 
     //     cout<<" result strncmp _Diff for naming stack : "<<strncmp(adev->GetHAData()->GetName(),"_Diff",4)<<endl; 
     if ( !strncmp(adev->GetHAData()->GetName(),"Asy",3)) suffix = string("_ASY");
     else suffix = string("_Diff");
     dname = string("SSTasy_")+string(fName)+ suffix; 
     fStkAStrip = new THStack((char*) dname.c_str(),(char*) dname.c_str());
     dname = string("SSTasy_RMS")+string(fName)+ suffix; 
     fStkAStripRMS = new THStack((char*) dname.c_str(),(char*) dname.c_str());
     dname = string("HSTasy_")+string(fName)+ suffix; 
     fStkAHisto = new THStack((char*) dname.c_str(),(char*) dname.c_str());  
     for (vector<TaPanamADevice*>::const_iterator i  = fMyPAD.begin(); 
                                              i != fMyPAD.end(); 
                                              i++) 
       {        
	(*i)->GetSData()->GetSCHist()->SetLineColor(fColor[colnum]);
	(*i)->GetSDataRMS()->GetSCHist()->SetLineColor(fColor[colnum]);
	(*i)->GetHData()->SetLineColor(fColor[colnum]);
	(*i)->GetSAData()->GetSCHist()->SetLineColor(fColor[colnum]);
	(*i)->GetSADataRMS()->GetSCHist()->SetLineColor(fColor[colnum]);
	(*i)->GetHAData()->SetLineColor(fColor[colnum]);
        fStkStrip->Add((*i)->GetSData()->GetSCHist());
        fStkStripRMS->Add((*i)->GetSDataRMS()->GetSCHist());
        fStkHisto->Add((*i)->GetHData());
        fStkAStrip->Add((*i)->GetSAData()->GetSCHist());
        fStkAStripRMS->Add((*i)->GetSADataRMS()->GetSCHist());
        fStkAHisto->Add((*i)->GetHAData());
	colnum++;        
       }
     fS.push_back(fStkStrip); 
     fS.push_back(fStkStripRMS); 
     fS.push_back(fStkHisto); 
     fS.push_back(fStkAStrip); 
     fS.push_back(fStkAStripRMS); 
     fS.push_back(fStkAHisto); 
    }
  return 0;
}

void 
TaPanamMultiDevice::InitStats()
{
  Int_t numoftag = 0;
  Int_t devnum = 0;
  if (!TypeOfDev()) 
   {
    numoftag = fMyPD.size();
    for (vector<TaPanamDevice*>::const_iterator i  = fMyPD.begin(); 
                                                i  != fMyPD.end(); 
                                                i++) 
      {
       (*i)->InitStat(0.6, 0.9 - 0.1*devnum, 0.95, 1 - 0.1*devnum,22,0.06);
       devnum++;
      }
   }
  else
   { 
    numoftag = fMyPAD.size();
    for (vector<TaPanamADevice*>::const_iterator i  = fMyPAD.begin(); 
                                                i  != fMyPAD.end(); 
                                                i++) 
      {
       (*i)->InitStat(0.4, 0.9 - 0.1*devnum, 0.95, 1 - 0.1*devnum,22,0.06);
       (*i)->InitAStat(0.4, 0.9 - 0.1*devnum, 0.95, 1 - 0.1*devnum,22,0.06);
       devnum++;
     }
   }
} 

void 
TaPanamMultiDevice::UpdateStats( UInt_t datatype )
{
  Int_t numoftag = 0;
  if (!TypeOfDev() && (datatype == 0 || datatype == 2)) 
   {
     cout<<" updating Dev data stats"<<flush<<endl;
    numoftag = fMyPD.size();
    for (vector<TaPanamDevice*>::const_iterator i  = fMyPD.begin(); 
                                                i  != fMyPD.end(); 
                                                i++) 
	(*i)->UpdateStat();
   }
  if (TypeOfDev() && (datatype == 1 || datatype == 2))   
   { 
    cout<<" updating ADev data stats"<<flush<<endl;
    numoftag = fMyPAD.size();
    cout<<" numortag="<<numoftag<<flush<<endl;
    for (vector<TaPanamADevice*>::const_iterator i  = fMyPAD.begin(); 
                                                i  != fMyPAD.end(); 
                                                i++) 
	(*i)->UpdateAStat();
    cout<<"done..."<<flush<<endl;
   }
}

// void 
// TaPanamMultiDevice::Fill(vector<TPanamDevice*> devlist, string devname)
// {
//   //copy devices 
//  for (vector<TaPanamDevice*>::const_iterator i  = devlist.begin(); 
//                                               i != devlist.end(); 
//                                               i++)  
//    {
//      if (!strncmp((*i).c_str(),dvname,3
//    }
// }
void 
TaPanamMultiDevice::DumpInfo()
{
  cout<<" ### Dump info MultiDevice named "<<GetName()<<endl;
  cout<<"fStkStrip name ="<<fStkStrip->GetName()<<endl;
  fStkStrip->Print();
  cout<<"fStkHisto name ="<<fStkHisto->GetName()<<endl;
  fStkHisto->Print();
  if (fStkAStrip)
    {
     cout<<"fStkAStrip name ="<<fStkAStrip->GetName()<<endl;
     fStkAStrip->Print();
    }
  if (fStkAHisto)
    {
  cout<<"fStkAHisto name ="<<fStkAHisto->GetName()<<endl;
  fStkAHisto->Print();
    }  
}

