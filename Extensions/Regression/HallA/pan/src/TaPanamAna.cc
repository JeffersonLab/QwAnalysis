//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaPanamAna.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaPanamAna.cc,v 1.5 2009/08/06 18:36:03 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Online Panaming analysis.  This class derives from VaAnalysis.
//    It simply displays plots defined in the list to the screen.  
//     
//    
//
////////////////////////////////////////////////////////////////////////

#include "TaPanamAna.hh"
#include "TaEvent.hh"
#include "TaRun.hh"
#include "TaDevice.hh"
#include "TaLabelledQuantity.hh"
#include "VaPair.hh"
#define DEBUGDEV
#define DEBUGBOX
//#define LEAKING
#define ADCSTACK

ClassImp(TaPanamAna)
// Constructors/destructors/operators

TaPanamAna::TaPanamAna():VaAnalysis(),
 fMADC(0),fMBatt(0),fMBCM(0),fMBCM2(0),fMBPMINx(0),fMBPMINy(0),  
 fMBPMParx(0),fMBPMPary(0),fMBPMABx(0),fMBPMABy(0),fMBPMcavx(0),fMBPMcavy(0),
 fMLumiBack(0),fMLumiFront(0),fMDalton(0)        
{ 
  //cout<<"TaPanamAna: CREATING ANALYSIS"<<this<<endl;
  fArrayOfDataName.clear();
  fMonDev.clear();
  fMonADev.clear();
  fMDIADC.clear();
  fMDPADC.clear();
  fMDSADC.clear();
  fADC_count=0;  
  fBCM_count=0;
  fBPM_count=0;
  fDoSlice = kFALSE;
  fDoRun = kFALSE;
  fDoRoot =kFALSE;
}



TaPanamAna::~TaPanamAna()
{
  if (fMADC) {delete fMADC ;fMADC  = NULL; }
  if (fMBatt) {delete fMBatt ;fMBatt = NULL ;}
  if (fMBCM) {delete fMBCM ; fMBCM  = NULL; }
  if (fMBCM2) {delete fMBCM2 ; fMBCM2  = NULL; }
  if (fMBPMINx) {delete fMBPMINx; fMBPMINx = NULL;} 
  if (fMBPMINy) {delete fMBPMINy; fMBPMINy = NULL; } 
  if (fMBPMParx) {delete fMBPMParx; fMBPMParx = NULL;}
  if (fMBPMPary) {delete fMBPMPary; fMBPMPary = NULL; }
  if (fMBPMABx) {delete fMBPMABx; fMBPMABx = NULL; }
  if (fMBPMABy) {delete fMBPMABy; fMBPMABy = NULL; }
  if (fMBPMcavx) {delete fMBPMcavx; fMBPMcavx = NULL; }
  if (fMBPMcavy) {delete fMBPMcavy; fMBPMcavy = NULL; }
  if (fMLumiBack) {delete fMLumiBack; fMLumiBack = NULL; }
  if (fMLumiFront) {delete fMLumiFront; fMLumiFront = NULL; }
  if (fMDalton) {delete fMDalton; fMDalton = NULL;}   
  fArrayOfDataName.clear();
  fMonDev.clear();
  fMonADev.clear();
  fMDIADC.clear();
  fMDPADC.clear();
  fMDSADC.clear();
}

TaPanamAna::TaPanamAna (const TaPanamAna& copy) 
{
}


TaPanamAna& TaPanamAna::operator=( const TaPanamAna& assign) 
{ 
  return *this; 
}

string TaPanamAna::itos(Int_t i)
{
  ostringstream buffer;
  buffer<<i;
  return buffer.str();
}

vector<string>
TaPanamAna::GetHistoForListBox() const
{
  vector<string> boxlist;
  boxlist.clear();
  for (vector<TaPanamDevice*>::const_iterator i= fMonDev.begin(); i != fMonDev.end(); i++)
    {
     boxlist.push_back(string((*i)->GetSData()->GetSCHist()->GetName())); 
     boxlist.push_back(string((*i)->GetSDataRMS()->GetSCHist()->GetName())); 
     boxlist.push_back(string((*i)->GetHData()->GetName())); 
    }
  for (vector<TaPanamADevice*>::const_iterator i= fMonADev.begin(); i != fMonADev.end(); i++)
    {
     boxlist.push_back(string((*i)->GetSData()->GetSCHist()->GetName())); 
     boxlist.push_back(string((*i)->GetSDataRMS()->GetSCHist()->GetName())); 
     boxlist.push_back(string((*i)->GetSAData()->GetSCHist()->GetName())); 
     boxlist.push_back(string((*i)->GetSADataRMS()->GetSCHist()->GetName())); 
     boxlist.push_back(string((*i)->GetHData()->GetName())); 
     boxlist.push_back(string((*i)->GetHAData()->GetName())); 
    }
 
#ifdef DEBUGBOX
  for (vector<string>::const_iterator s= boxlist.begin(); s != boxlist.end(); s++)
    { 
     cout<<"boxlist name="<<*s<<endl;
    }
#endif
  return boxlist;
}

void 
TaPanamAna::InitMonitorDevices()
{  
 fMBatt = new TaPanamMultiDevice("batteries");
 fMBCM = new TaPanamMultiDevice("BCMs");
 fMBCM2 = new TaPanamMultiDevice("BCM2s");
 fMBCMcav = new TaPanamMultiDevice("BCMcav");
 fMBPMINx = new TaPanamMultiDevice("BPMINx");
 fMBPMINy = new TaPanamMultiDevice("BPMINy");
 fMBPMParx = new TaPanamMultiDevice("BPMParx");
 fMBPMPary = new TaPanamMultiDevice("BPMPary");
 fMBPMABx = new TaPanamMultiDevice("BPMABx");
 fMBPMABy = new TaPanamMultiDevice("BPMABy");
 fMBPMcavx = new TaPanamMultiDevice("BPMcavx");
 fMBPMcavy = new TaPanamMultiDevice("BPMcavy");
 fMLumiBack = new TaPanamMultiDevice("Lumisback");
 fMLumiFront = new TaPanamMultiDevice("Lumisfront");
 fMDalton = new TaPanamMultiDevice("Daltons");
 // special treatment for ADC stacks    
 vector<TaPanamADevice*>      fADC;
 // string::size_type cmatch;
 
 fLastADCName="";
  
 cout<<"------------------------Entering histos initialization------------------------"<<endl;

#ifdef DEBUGDEV
  cout<<" fMondev size= "<<fMonDev.size()<<endl;
  cout<<" *** Name of device and plots  "<<endl;
#endif  
  // clear vectors
  if ( fMonDev.size() != 0)  fMonDev.clear();
  if ( fMonADev.size() != 0)  fMonADev.clear();  
  if ( fADC.size() != 0)  fADC.clear();

  for (vector<string>::const_iterator ci= fArrayOfDataName.begin(); 
                                      ci!= fArrayOfDataName.end(); 
                                      ci++)
     {
//       if (!strncmp((*ci).c_str(),"heli",4) || !strncmp((*ci).c_str(),"pair",4)  || 
//           !strncmp((*ci).c_str(),"mult",4) || !strncmp((*ci).c_str(),"times",5)) 
//         {
//          TaPanamDevice* md = new TaPanamDevice((char*)(*ci).c_str(),fRun->GetKey((char*)(*ci).c_str()),
//                                                striptime,60,100,0.,(Float_t) striptime,-2,2,0);
//          fMonDev.push_back(md);
//          fMSignal->InsertDevice(md);
//          cout<<"(*ci)"<<(*ci)<<endl;
// 	}
//       if (!strncmp((*ci).c_str(),"v2f",3)) 
//         {
//          TaPanamDevice* md = new TaPanamDevice((char*)(*ci).c_str(),fRun->GetKey((char*)(*ci).c_str()),
//                                                striptime,60,100,0.,(Float_t) striptime,-2,2,0);
//          fMonDev.push_back(md);
//          fMClock->InsertDevice(md);
// 	}
      if (!strncmp((*ci).c_str(),"adc",3)) 
        {
         TaPanamADevice* md = new TaPanamADevice((char*)(*ci).c_str(),
                                                 fRun->GetKey((char*)(*ci).c_str()),
                                                 striptime,60,striptime,60,
                                                 655,
                                                 0.,(Float_t) striptime,
                                                 0.,65535.,
                                                 600,
                                                 0.,(Float_t) striptime,
                                                 -500,500,
                                                 0,2,kFALSE);
         fMonADev.push_back(md);
	 fADC.push_back(md);
	}
     if (!strncmp((*ci).c_str(),"bcm",3)  || !strncmp((*ci).c_str(),"det",3) || 
         !strncmp((*ci).c_str(),"bcmcav",6) || !strncmp((*ci).c_str(),"bat",3)) 
        {
         TaPanamADevice* md = new TaPanamADevice((char*)(*ci).c_str(),
                                                 fRun->GetKey((char*)(*ci).c_str()),
                                                 striptime,60,striptime,60,
                                                 100,
                                                 0.,(Float_t) striptime,
                                                 0.,65535.,
                                                 200,
                                                 0.,(Float_t) striptime,
                                                 -5000,5000,
                                                 0,2,kFALSE);
         fMonADev.push_back(md);
         if (!strncmp((md->GetName()),"bcm1",4) || !strncmp((md->GetName()),"bcm2",4)) fMBCM->InsertADevice(md);
         if (!strncmp((md->GetName()),"bcm3",4) || !strncmp((md->GetName()),"bcm4",4)) fMBCM2->InsertADevice(md);
         if (!strncmp((md->GetName()),"bcmcav",6)) fMBCMcav->InsertADevice(md);   
         if (!strncmp((md->GetName()),"bat",3)) fMBatt->InsertADevice(md);
         if (!strncmp((md->GetName()),"det",3)) fMDalton->InsertADevice(md);
	}
      if (!strncmp((*ci).c_str(),"lumi",4)) 
        {
         TaPanamADevice* md = new TaPanamADevice((char*)(*ci).c_str(),
                                                 fRun->GetKey((char*)(*ci).c_str()),
                                                 striptime,60,striptime,60,
                                                 100,
                                                 0.,(Float_t) striptime,
                                                 0.,65535.,
                                                 200,
                                                 0.,(Float_t) striptime,
                                                 -10000,10000,
                                                 0,2,kFALSE);
         fMonADev.push_back(md);
         if (!strncmp((md->GetName()),"lumi1",5) || 
             !strncmp((md->GetName()),"lumi2",5) ||
             !strncmp((md->GetName()),"lumi3",5) ||
             !strncmp((md->GetName()),"lumi4",5) ) fMLumiBack->InsertADevice(md);
         else fMLumiFront->InsertADevice(md);
	}
      if (!strncmp((*ci).c_str(),"bpm1I",5) || !strncmp((*ci).c_str(),"bpm0I",5) || !strncmp((*ci).c_str(),"bpm0L",5) ) 
        {
         TaPanamADevice* md = new TaPanamADevice((char*)(*ci).c_str(),
                                                 fRun->GetKey((char*)(*ci).c_str()),
                                                 striptime,60,striptime,60,
                                                 100,
                                                 0.,(Float_t) striptime,
                                                 0.,65535.,
                                                 200,
                                                 0.,(Float_t) striptime,
                                                 -500,500,
                                                 0,2,kTRUE);
          fMonADev.push_back(md);
	  if ((*ci).find("x",0) != string::npos) fMBPMINx->InsertADevice(md);
	  if ((*ci).find("y",0) != string::npos) fMBPMINy->InsertADevice(md);
	}
      else if (!strncmp((*ci).c_str(),"bpmca",5)) 
        {
         TaPanamADevice* md = new TaPanamADevice((char*)(*ci).c_str(),
                                                 fRun->GetKey((char*)(*ci).c_str()),
                                                 striptime,60,striptime,60,
                                                 100,
                                                 0.,(Float_t) striptime,
                                                 0.,65535.,
                                                 200,
                                                 0.,(Float_t) striptime,
                                                 -500,500,
                                                 0,2,kTRUE);
         fMonADev.push_back(md);
	  if ((*ci).find("x",0) != string::npos) fMBPMcavx->InsertADevice(md);
	  if ((*ci).find("y",0) != string::npos) fMBPMcavy->InsertADevice(md);
	}
      else if (!strncmp((*ci).c_str(),"bpm4",4)) 
        {
         TaPanamADevice* md = new TaPanamADevice((char*)(*ci).c_str(),
                                                 fRun->GetKey((char*)(*ci).c_str()),
                                                 striptime,60,striptime,60,
                                                 100,
                                                 0.,(Float_t) striptime,
                                                 0.,65535.,
                                                 200,
                                                 0.,(Float_t) striptime,
                                                 -500,500,
                                                 0,2,kTRUE);
         fMonADev.push_back(md);
         if ((*ci).find("x",0) != string::npos) fMBPMABx->InsertADevice(md);
         if ((*ci).find("y",0) != string::npos) fMBPMABy->InsertADevice(md);
	}
      else if (!strncmp((*ci).c_str(),"bpm",3)) 
        {
         TaPanamADevice* md = new TaPanamADevice((char*)(*ci).c_str(),
                                                 fRun->GetKey((char*)(*ci).c_str()),
                                                 striptime,60,striptime,60,
                                                 100,
                                                 0.,(Float_t) striptime,
                                                 0.,65535.,
                                                 200,
                                                 0.,(Float_t) striptime,
                                                 -500,500,
                                                 0,2,kTRUE);
         fMonADev.push_back(md);
         if ((*ci).find("x",0) !=string::npos) fMBPMParx->InsertADevice(md);
         if ((*ci).find("y",0) !=string::npos) fMBPMPary->InsertADevice(md);
	}
      }

#ifdef DEBUGDEV

  cout<<" === === === debug monitor list === ==== ==="<<endl; 
 for (vector<TaPanamDevice*>::const_iterator md  = fMonDev.begin(); md!= fMonDev.end(); md++)
   {
    cout<<"\ndevice name="<<(*md)->GetName()<<" string="<<string((*md)->GetName())<<endl;
    cout<<"fSData name="<<(*md)->GetSData()->GetName()<<" string="<<
          string((*md)->GetSData()->GetName())<<endl;
    cout<<"fSData TH1D name="<<(*md)->GetSData()->GetSCHist()->GetName()<<" string="<<
          string((*md)->GetSData()->GetSCHist()->GetName())<<endl;
    cout<<"fSDataRMS name="<<(*md)->GetSDataRMS()->GetName()<<" string="<<string((*md)->GetSDataRMS()->GetName())<<endl;
    cout<<"fSDataRMS TH1D name="<<(*md)->GetSDataRMS()->GetSCHist()->GetName()<<" string="<<string((*md)->GetSDataRMS()->GetSCHist()->GetName())<<endl;
    cout<<"fHData name="<<(*md)->GetHData()->GetName()<<" string="<<string((*md)->GetHData()->GetName())<<endl;
   }

 for (vector<TaPanamADevice*>::const_iterator md  = fMonADev.begin(); 
                                      md!= fMonADev.end(); 
                                      md++)
   {
    cout<<"\ndevice name="<<(*md)->GetName()<<" string="<<string((*md)->GetName())<<endl;
    cout<<"fSData name="<<(*md)->GetSData()->GetName()<<" string="<<
          string((*md)->GetSData()->GetName())<<endl;
    cout<<"fSData TH1D name="<<(*md)->GetSData()->GetSCHist()->GetName()<<" string="<<
    cout<<"fSAData name="<<(*md)->GetSAData()->GetName()<<" string="<<
          string((*md)->GetSAData()->GetName())<<endl;
    cout<<"fSAData TH1D name="<<(*md)->GetSAData()->GetSCHist()->GetName()<<" string="<<
          string((*md)->GetSAData()->GetSCHist()->GetName())<<endl;
    cout<<"fSDataRMS name="<<(*md)->GetSDataRMS()->GetName()<<" string="<<
          string((*md)->GetSDataRMS()->GetName())<<endl;
    cout<<"fSDataRMS TH1D name="<<(*md)->GetSDataRMS()->GetSCHist()->GetName()<<" string="<<
          string((*md)->GetSDataRMS()->GetSCHist()->GetName())<<endl;
    cout<<"fSADataRMS name="<<(*md)->GetSADataRMS()->GetName()<<" string="<<
          string((*md)->GetSADataRMS()->GetName())<<endl;
    cout<<"fSADataRMS TH1D name="<<(*md)->GetSADataRMS()->GetSCHist()->GetName()<<" string="<<
          string((*md)->GetSADataRMS()->GetSCHist()->GetName())<<endl;
    cout<<"fHData name="<<(*md)->GetHData()->GetName()<<" string="<<
          string((*md)->GetHData()->GetName())<<endl;
    cout<<"fHAData name="<<(*md)->GetHAData()->GetName()<<" string="<<
          string((*md)->GetHAData()->GetName())<<endl;
  }

 cout<<"=== === ===  Device debug complete === === ==="<<endl;

#endif  

 //------------------------------ start init Multidevice ------------------------------------------

//   if (fMSignal->TypeOfDev() < 0 ) 
//    {
//      delete fMSignal; fMSignal = NULL;
//    } 
//   else 
//    {
//     fMSignal->InitStacks(); 
//     fMSignal->InitStats();
//    } 

//   if (fMClock->TypeOfDev() < 0 ) {delete fMClock; fMClock = NULL;}
//   else 
//    {
//     fMClock->InitStacks();
//     fMClock->InitStats();
//    }
  if (fMBatt->TypeOfDev() < 0 ) {delete fMBatt; fMBatt = NULL;} 
  else
   { 
    fMBatt->InitStacks();
    fMBatt->InitStats();
   }
  if (fMBCM->TypeOfDev() < 0 ) {delete fMBCM;  fMBCM = NULL;}
  else 
   {
    fMBCM->InitStacks();
    fMBCM->InitStats();
   } 
  if (fMBCM2->TypeOfDev() < 0 ) {delete fMBCM2;  fMBCM2 = NULL;}
  else 
   {
    fMBCM2->InitStacks();
    fMBCM2->InitStats();
   } 

  if (fMBPMINx->TypeOfDev() < 0 ) 
   {
    delete fMBPMINx; fMBPMINx = NULL;
    delete fMBPMINy; fMBPMINy = NULL;
   } 
  else 
   {
    fMBPMINx->InitStacks();
    fMBPMINx->InitStats();
    fMBPMINy->InitStacks(); 
    fMBPMINy->InitStats();
   } 
  if (fMBPMParx->TypeOfDev() < 0 ) 
   {
    delete fMBPMParx; fMBPMParx = NULL;
    delete fMBPMPary; fMBPMPary = NULL; 
   } 
  else 
   {
    fMBPMParx->InitStacks(); 
    fMBPMParx->DumpInfo(); 
    fMBPMParx->InitStats();
    fMBPMPary->InitStacks();
    fMBPMPary->DumpInfo();
    fMBPMPary->InitStats();
   } 
  if (fMBPMABx->TypeOfDev() < 0 ) 
    {
     delete fMBPMABx; fMBPMABx = NULL;
     delete fMBPMABy; fMBPMABy = NULL;
    } 
  else 
    {
     fMBPMABx->InitStacks();
     fMBPMABx->InitStats();
     fMBPMABy->InitStacks(); 
     fMBPMABy->InitStats();
    }   
  if (fMBPMcavx->TypeOfDev() < 0 ) 
    {
     delete fMBPMcavx; fMBPMcavx = NULL;
     delete fMBPMcavy; fMBPMcavy = NULL;
    } 
  else 
    {
     fMBPMcavx->InitStacks();
     fMBPMcavx->InitStats();
     fMBPMcavy->InitStacks(); 
     fMBPMcavy->InitStats();
    }   
  if (fMLumiBack->TypeOfDev() < 0 ) 
    {
     delete fMLumiBack; 
     fMLumiBack = NULL;
    } 
  else
   {
    fMLumiBack->InitStacks();
    fMLumiBack->InitStats();
   } 
  if (fMLumiFront->TypeOfDev() < 0 ) 
    {
     delete fMLumiFront; 
     fMLumiFront = NULL;
    } 
  else
   {
    fMLumiFront->InitStacks();
    fMLumiFront->InitStats();
   } 

  if (fMDalton->TypeOfDev() < 0 )
    {
     delete fMDalton;fMDalton = NULL;
    } 
  else 
    {
     fMDalton->InitStacks();
     fMDalton->InitStats();
    } 
  
 Char_t* devname;
 Char_t* splitstr;
 Char_t* channum;
 Int_t   numchan=0; 

 if (fADC.size() > 0) 
   {   

    for (vector<TaPanamADevice*>::const_iterator didx= fADC.begin(); didx != fADC.end(); didx++)
      {
       splitstr= (Char_t*) (*didx)->GetName();
       //cout<<"ADC init  splitstr="<<splitstr<<endl;
       devname=strtok(splitstr,"_");
       channum= strtok(NULL,"_ ");
       //cout<<"ADC init  devname="<<devname<<endl;
       //cout<<"last adc name "<<fLastADCName<<endl;   
       //cout<<"ADC channum="<<channum<<endl;
       numchan=atoi(channum);
       if (strcmp(devname,fLastADCName))
         {
	  fLastADCName=devname; 
          fMADC = new TaPanamMultiDevice(devname);
          fMADC->InsertADevice(*didx);
          if (!strncmp(devname,"adc8",5) || !strncmp(devname,"adc6",5) || !strncmp(devname,"adc16",5))
           fMDIADC.push_back(fMADC);
           else if (!strncmp(devname,"adc17",5) || !strncmp(devname,"adc29",5))
           fMDSADC.push_back(fMADC);    
          else if (!strncmp(devname,"adc",3))    
           fMDPADC.push_back(fMADC);
	 }
       else
	 {
          fMADC->InsertADevice(*didx);
	 } 
       
      }
    
    cout<<" --------- Init & debug ADC multidevice ------------"<<endl;
    cout<<" injector crate "<<endl;
    for(vector<TaPanamMultiDevice*>::const_iterator mdidx= fMDIADC.begin(); mdidx != fMDIADC.end(); mdidx++)
      {
        (*mdidx)->InitStacks();(*mdidx)->InitStats();
	cout<<(*mdidx)->GetName()<<endl;        
      }
    cout<<" parity crate"<<endl;
    for(vector<TaPanamMultiDevice*>::const_iterator mdidx= fMDPADC.begin(); mdidx != fMDPADC.end(); mdidx++)
      {
        (*mdidx)->InitStacks();(*mdidx)->InitStats();
	cout<<(*mdidx)->GetName()<<endl;        
      }
    cout<<" HRS crate "<<endl;
    for(vector<TaPanamMultiDevice*>::const_iterator mdidx= fMDSADC.begin(); mdidx != fMDSADC.end(); mdidx++)
      {
       (*mdidx)->InitStacks();(*mdidx)->InitStats();
	cout<<(*mdidx)->GetName()<<endl;        
      }
    cout<<" --------------------------------------------"<<endl; 
   }

 cout<<"---------------------Initialization complete ! ----------------\n"<<endl;
}


void
TaPanamAna::FillEventPlots()
{ 
  for (vector<TaPanamDevice*>::const_iterator i  = fMonDev.begin(); 
                                              i != fMonDev.end(); 
                                              i++) 
      (*i)->FillFromEvent(*fRun);
  
  for (vector<TaPanamADevice*>::const_iterator i  = fMonADev.begin(); 
                                              i != fMonADev.end(); 
                                              i++) 
      (*i)->FillFromEvent(*fRun);  
}

void 
TaPanamAna::FillPairPlots()
{
for (vector<TaPanamADevice*>::const_iterator i  = fMonADev.begin(); 
                                             i != fMonADev.end(); 
                                             i++)
       (*i)->FillFromPair(*fPair);
// if bcm1 -> norm by bcm2
// if bcm3 -> norm by bcm4
// if det or lumi ->norm by fCurmon
// if bpm4Ax -> norm by bpm4Bx to get diff deltax
// if bpm4Ay -> norm by bpm4By to get diff deltay
 
}

void TaPanamAna::InitDevicePad(Int_t devidx, UInt_t plotidx, Bool_t choice, UInt_t optionfit)
{
  if (!choice) {
    //    cout<<"initscpad"<<endl;
   fMonDev[devidx]->InitSCPad(plotidx);
  }
 else fMonDev[devidx]->DrawHPad(optionfit);
}

void TaPanamAna::InitADevicePad(Int_t devidx, UInt_t plotidx, Bool_t choice, UInt_t optionfit)
{
 if (!choice) fMonADev[devidx]->InitSCPad(plotidx);
 else fMonADev[devidx]->DrawHPad(plotidx,optionfit);
}

void TaPanamAna::DisplayDevice(Int_t devidx,UInt_t plotidx, Bool_t choice, UInt_t optionfit)
{
 if ( !choice) fMonDev[devidx]->DisplaySC(plotidx);
 else fMonDev[devidx]->DrawHPad(optionfit);
}

void TaPanamAna::DisplayADevice(Int_t devidx,UInt_t plotidx, Bool_t choice, UInt_t optionfit)
{
 if ( !choice) fMonADev[devidx]->DisplaySC(plotidx);
 else fMonADev[devidx]->DrawHPad(plotidx,optionfit);
}

void TaPanamAna::DrawMDStack(UInt_t stacknum, Int_t thestack, Int_t optstat)
{
  switch(stacknum)
    {
//     case 0:
//      fMSignal->DrawStack(thestack,optstat);
//       break;
//     case 1:
//      fMClock->DrawStack(thestack,optstat);
//       break;
    case 0:
     fMBatt->DrawStack(thestack,optstat);
      break;
    case 1:
      fMBCM->DrawStack(thestack,optstat);
      break;
    case 2:
      fMBCM2->DrawStack(thestack,optstat);
      break;
    case 3:
      fMBCMcav->DrawStack(thestack,optstat);
      break;
    case 4:
     fMBPMINx->DrawStack(thestack,optstat);
      break;
    case 5:
     fMBPMINy->DrawStack(thestack,optstat);
      break;
    case 6:
     fMBPMParx->DrawStack(thestack,optstat);
      break;
    case 7:
     fMBPMPary->DrawStack(thestack,optstat);
      break;
    case 8:
     fMBPMABx->DrawStack(thestack,optstat);
      break;
    case 9:
     fMBPMABy->DrawStack(thestack,optstat);
      break;
    case 10:
     fMLumiBack->DrawStack(thestack,optstat);
      break;
    case 11:
     fMLumiFront->DrawStack(thestack,optstat);
      break;
    case 12:
     fMDalton->DrawStack(thestack,optstat);
      break;
    case 13:
      fMBPMcavx->DrawStack(thestack,optstat);
     break;
    case 14:
     fMBPMcavy->DrawStack(thestack,optstat);
     break; 
  }
}

void TaPanamAna::DrawADCStack( Int_t adccrate, UInt_t adcidx, Int_t thestack, Int_t optstat)
{
  switch (adccrate)
    {
    case 0:
      if (fMDIADC.size() > adcidx ) fMDIADC[adcidx]->DrawStack(thestack,optstat);
      //else cout<<" TaPanamAna::DrawADCStack(): ERROR fMDIADC.size > adcidx"<<flush<<endl;
      break;
    case 1:
      if (fMDPADC.size() > adcidx ) fMDPADC[adcidx]->DrawStack(thestack,optstat);
      else cout<<" TaPanamAna::DrawADCStack(): ERROR fMDPADC.size > adcidx"<<flush<<endl;
      break;
    case 2:
      if (fMDSADC.size() > adcidx ) {
         fMDSADC[adcidx]->DrawStack(thestack,optstat);
      }
      else cout<<" TaPanamAna::DrawADCStack(): ERROR fMDSADC.size > adcidx"<<flush<<endl;
      break;
    }
}



void
TaPanamAna::EventAnalysis()
{
  // Event analysis.
  // For now we hard code four results, the values of bcm1 and bcm2
  // (raw and corrected).  A somewhat more intelligent analysis 
  // should be concocted eventually.

  fEvt->AddResult ( TaLabelledQuantity ( "bcm1 raw",
					 fEvt->GetData(IBCM1R), 
					 "chan" ) );
  fEvt->AddResult ( TaLabelledQuantity ( "bcm1",
					 fEvt->GetData(IBCM1), 
					 "chan" ) );
  fEvt->AddResult ( TaLabelledQuantity ( "bcm2 raw",
					 fEvt->GetData(IBCM2R), 
					 "chan" ) );
  fEvt->AddResult ( TaLabelledQuantity ( "bcm2",
					 fEvt->GetData(IBCM2), 
					 "chan" ) );
  FillEventPlots();       
}

void 
TaPanamAna::PairAnalysis()
{ 
  // Pair analysis
  // All we have here is a call to AutoPairAna.
  AutoPairAna();
  FillPairPlots();
}


void
TaPanamAna::InitChanLists ()
{
}

void
TaPanamAna::InitDevicesList(vector<string> arrayofname)
{
  cout<<"\n ---- Copy of the dataname wanted for monitoring ----\n"<<endl;
  fArrayOfDataName = arrayofname;
  cout<<"copied"<<endl;
}

TaPanamDevice* 
TaPanamAna::GetPanamDevice(Char_t* const devname) const
{
   TaPanamDevice* thedev;
   for (vector<TaPanamDevice*>::const_iterator i= fMonDev.begin(); i != fMonDev.end(); i++)
     {
       if ((Char_t*) (*i)->GetName() == devname) { thedev=(*i);} 
     }
   return thedev;
}

TaPanamADevice* 
TaPanamAna::GetPanamADevice(Char_t* const devname) const
{
   TaPanamADevice* theAdev;
   for (vector<TaPanamADevice*>::const_iterator i= fMonADev.begin(); i != fMonADev.end(); i++)
     {
       if ((Char_t*) (*i)->GetName() == devname) { theAdev=(*i);} 
     }
   return theAdev;
}
