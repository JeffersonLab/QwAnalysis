//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan
//
//           TaPanam.cc
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaPanam.cc,v 1.7 2009/08/06 18:36:03 rsholmes Exp $
// 
////////////////////////////////////////////////////////////////////////
//
//  Graphical user interface using Pan analysing for monitoring 
//  Inspiration for layout is from ALICE testbeam monitoring, E158 and 
//  ROOT test example.   
//
////////////////////////////////////////////////////////////////////////

#define SIGNAL
#define DEBUGDCOUNT
#define DEBUGOPTION
#define REFRESH
#define CODAFILETEST
//#define ETCONNECT

#include "TaPanam.hh"
#include "TaString.hh"
#include "TaDevice.hh"

#include <sstream>

ClassImp(TaPanam)

TaPanam::TaPanam()  
  : TGMainFrame(0, 0, 0),TaThread(0)
{
 // Needed by Root for the dictionnary
 fMainTitle=0;fUtilFrame=0;
 fHistoFrame=0;fButtonFrame=0;
 fMainFrame=0;fEmCanFrame=0;
 // folders frames
 fTimeFrame=0;fTimegFrame=0;fTOptgFrame=0;
 fADCFrame=0;fADCSubFrame=0;fADCFrame0=0;fADCFrame1=0;fADCFrame2=0;fADCOptgFrame=0;
 fIADCCrate=0;fPADCCrate=0;fSADCCrate=0;
 fCurrentFrame=0;fCurrentSubFrame=0;fInjBCMFrame=0;
 fBCMFrame=0;fBCMDispFrame=0;fBCMOptFrame=0;
 fCurrentgFrame=0;fInjBCMgFrame=0;fBCMgFrame=0;fBCMDispgFrame=0;
 fPositionFrame=0;fBPMDevFrame=0;fInjBPMFrame=0;
 fBPMFrame=0;fBPMDispFrame=0;fBPMDFrame=0;fBPMOptFrame=0;
 fPositiongFrame=0;fInjBPMgFrame=0;fBPMgFrame=0;fBPMDispgFrame=0;
 fDetFrame=0;fLumiFrame=0;fDalFrame=0;fScanFrame=0;
 fLumigFrame=0;fDalgFrame=0;fScangFrame=0;
 // radio buttons
 
 fTitle=0;fL1=0;fL2=0;fL3=0;fL4=0;fL5=0;fL6=0;fL7=0;fL8=0;fL9=0;
 fLogoButton=0;
 fButton1=0;fButton2=0;fButton3=0;
 fConfigBuff=0;fCodaRunBuff=0;fRefreshBuff=0;
 fConfigText=0;fCodaRunText=0;fRefreshText=0;
 fHistoTreeView=0;
 fEmCan=0;
 fMenuBar=0;fMenuBarLayout=0;fMenuBarItemLayout=0;
 fMenuFile=0;fMenuMon=0;fMenuAna=0;
 fHistoListBox=0;fTab=0;
 fStripStyle=0;fHistoStyle=0;fGraphStyle=0;
 fBCMplot = 0; fBPMplot =0; 
}


TaPanam::TaPanam(const TGWindow *p, UInt_t w, UInt_t h)
  : TGMainFrame(p, w, h), TaThread(2)
{ 
  if (InitFlags()) 
   {
     cout<<" InitFlags() : Flags Initialized "<<endl;
     //DumpOptionState(); 
   }
  if (InitParameters()) cout<<" InitParameters() : Parameters Initialized "<<endl;
  InitGUIFrames();
  if (InitDevCheckButtons()) cout<<" InitDevCheckButtons() : Device Buttons Initialized "<<endl;
}

TaPanam::~TaPanam()
{
  delete fMenuBarLayout;
  delete fMenuBarItemLayout;
  delete fL1; delete fL2; delete fL3;delete fL4; delete fL5; delete fL6; delete fL7;
  delete fL8; delete fL9;
  delete fTimeFrame;      delete  fTimegFrame;     delete  fTOptgFrame; 
  delete fADCFrame; delete fADCSubFrame; delete  fADCFrame0;      delete  fADCFrame1; 
  delete fADCFrame2;      delete  fADCOptgFrame;
  delete fIADCCrate;      delete fPADCCrate;       delete fSADCCrate; 
  delete fCurrentFrame;   delete fCurrentSubFrame;  delete fInjBCMFrame;
  delete fBCMFrame;       delete fBCMDispFrame;    delete fBCMOptFrame;   
  delete fBCMDispFrame;   delete fBCMOptFrame;
  delete fCurrentgFrame;  delete fBCMgFrame;       delete fBCMDispgFrame;
  delete fPositionFrame;  delete fInjBPMFrame; 
  delete fBPMFrame;       delete fBPMDispFrame;    delete fBPMOptFrame; 
  delete fPositiongFrame; delete fBPMgFrame; delete fBPMDispgFrame;
  delete fDetFrame;        delete fDalFrame;          delete fLumiFrame;   delete fScanFrame; 
  delete fDalgFrame;      delete fLumigFrame;         delete fScangFrame; 
  
  delete fMenuFile;
  delete fMenuMon;
  delete fMenuAna;
  delete fMenuBar; 
}

void TaPanam::CloseWindow()
{
 TGMainFrame::CloseWindow();
 gApplication->Terminate(0);
}

Int_t TaPanam::SetConfig(UInt_t confignum)
{ // set the kind of config needed for the monitoring  
  // ADCs 
  // beam diagnostic 
  // injector CRATE only
  // parity CRATE only
  // HRS CRATE only
  // SAGDH
  // HAPPEX2 (considering current datamap 29 june 2003..) 
  // Manual
  switch (confignum)
    {
    case 0:
     ResetDevCheckButtons();
     //Injector ADC
     fIADCCheck[0]->SetState(kButtonDown);
     fIADCCheck[1]->SetState(kButtonDown);
     fIADCCheck[2]->SetState(kButtonDown);
     //parity ADC
     fADCCheck[0]->SetState(kButtonDown);
     fADCCheck[1]->SetState(kButtonDown);
     fADCCheck[2]->SetState(kButtonDown);
     fADCCheck[3]->SetState(kButtonDown);
     fADCCheck[4]->SetState(kButtonDown);
     fADCCheck[5]->SetState(kButtonDown);
     fADCCheck[6]->SetState(kButtonDown);
     fADCCheck[7]->SetState(kButtonDown);
     fADCCheck[8]->SetState(kButtonDown);
     //     fADCCheck[9]->SetState(kButtonDown);
     //spectro ADC
     fSADCCheck[0]->SetState(kButtonDown);
     fSADCCheck[1]->SetState(kButtonDown);     
     fConfigBuff->AddText(0," ADC ");    
     cout<<"Set PANAM config = 0 : ADC "<<endl;
     break;
    case 1:
      ResetDevCheckButtons();
      //BCMs
       //1,2
     fBCMCheck[0]->SetState(kButtonDown);
     fBCMCheck[1]->SetState(kButtonDown);
     fBCMCheck[2]->SetState(kButtonDown);
     fBCMCheck[3]->SetState(kButtonDown);
     //BPMs
       // Inj BPMs
     fInjBPMCheck[1]->SetState(kButtonDown);
     fInjBPMCheck[6]->SetState(kButtonDown);
       //HALLA BPMs
     fBPMCheck[0]->SetState(kButtonDown);
     fBPMCheck[1]->SetState(kButtonDown);
     fBPMCheck[2]->SetState(kButtonDown);
     fBPMCheck[3]->SetState(kButtonDown);
     fBPMCheck[4]->SetState(kButtonDown);
     //lumi
      for (Int_t i =0 ; i<6  ;i++) fLumiCheck[i]->SetState(kButtonDown);
     fConfigBuff->AddText(0," Beam ");
     cout<<"Set PANAM config = 1 : Beam Diagnostic "<<endl;     
    case 7:
     fConfigBuff->AddText(0," Manual ");
     cout<<"Set PANAM config = 7 : Manual "<<endl; 
     break;
    }
  return 1;
} 

void TaPanam::AddDevToList(TGCheckButton *thebutton, string key1, string key2)
{
 if (thebutton->GetState() == kButtonDown)
   { 
     if (fAnaMan->GetAnalysis()->GetRun()->GetDevices().IsUsed(fAnaMan->GetAnalysis()->GetRun()->GetDevices().GetKey(key1))) 
      {
	cout<<key1<<" is used and "<<flush;
       fAllDeviceList.push_back(key2);
       cout<<" added "<<key2<<" in the device list"<<endl;
      }
     else cout<<key2<<" is not used so will not appear in display "<<endl;
   }
}

void TaPanam::AddADCToList(TGCheckButton *thebutton, string key1, string key2)
{
 string  dataname;
 if (thebutton->GetState() == kButtonDown)
   { 
    if (fAnaMan->GetAnalysis()->GetRun()->GetDevices().IsUsed(fAnaMan->GetAnalysis()->GetRun()->GetDevices().GetKey(key1)))
      {
       
       for (Int_t j=0; j<maxchanadc; j++)
         {
          if (!fADCdata)
            {
             dataname = (key2 + string("_") + itos(j)).c_str();
             fAllDeviceList.push_back(dataname); 
             cout<<" added "<<dataname<<" in the device list"<<endl;
	    }
	   else
	    {
             dataname = (key2 + string("_") + itos(j) + string("_cal")).c_str();
             fAllDeviceList.push_back(dataname); 
             cout<<" added "<<dataname<<" in the device list"<<endl;
   	    }          
	  }
      }
    else cout<<key1<<" is not used so this ADC will not appear in display "<<endl;
   }
}

void TaPanam::Add2DDevToList(TGCheckButton *thebutton, string key1, string key2)
{
 string  dataname;
 if (thebutton->GetState() == kButtonDown)
   { 
    if (fAnaMan->GetAnalysis()->GetRun()->GetDevices().IsUsed(fAnaMan->GetAnalysis()->GetRun()->GetDevices().GetKey(key1)))
      {
       dataname = (key2 + string("x")).c_str();
       fAllDeviceList.push_back(dataname); 
       dataname = (key2 + string("y")).c_str();
       fAllDeviceList.push_back(dataname); 
       cout<<" added "<<dataname<<" in the device list"<<endl;
      }
    else cout<<key1<<" is not used so will not appear in display"<<endl; 
   }
}

Int_t TaPanam::SetDevList()
{
// Just need to specify first raw data name encountered fordevice to define complete device 
// (i.e. just define adc1_0 in list to ask for the display of the 4 channels of adc1)    
  // inj ADC
  AddADCToList(fIADCCheck[0],"adc8_0","adc8");
  AddADCToList(fIADCCheck[1],"adc6_0","adc6");
  AddADCToList(fIADCCheck[2],"adc16_0","adc16");
  // parity ADC
  AddADCToList(fADCCheck[0],"adc2_0","adc2");
  AddADCToList(fADCCheck[1],"adc26_0","adc26");
  AddADCToList(fADCCheck[2],"adc23_0","adc23");
  AddADCToList(fADCCheck[3],"adc22_0","adc22");
  AddADCToList(fADCCheck[4],"adc15_0","adc15");
  AddADCToList(fADCCheck[5],"adc13_0","adc13");
  AddADCToList(fADCCheck[6],"adc7_0","adc7");
  AddADCToList(fADCCheck[7],"adc12_0","adc12");
  AddADCToList(fADCCheck[8],"adc14_0","adc14");
  // HRS ADC
  AddADCToList(fSADCCheck[0],"adc29_0","adc29");
  AddADCToList(fSADCCheck[1],"adc17_0","adc17");
  // BCMs
    //inj
  AddDevToList(fInjBCMCheck[0],"bcm6r","bcm6");
  AddDevToList(fInjBCMCheck[1],"batt1","batt1");
  // parity 
  AddDevToList(fBCMCheck[0],"bcm1r","bcm1");
  AddDevToList(fBCMCheck[1],"bcm2r","bcm2");
  AddDevToList(fBCMCheck[2],"bcm3r","bcm3");
  AddDevToList(fBCMCheck[3],"bcm4r","bcm4");
  AddDevToList(fBCMCheck[4],"bcm5r","bcm5");
  AddDevToList(fBCMCheck[5],"bcmcav1r","bcmcav1");
  AddDevToList(fBCMCheck[6],"bcmcav2r","bcmcav2");
  AddDevToList(fBCMCheck[7],"bcmcav3r","bcmcav3");
  //parity batteries
  AddDevToList(fBCMCheck[8],"batt2","batt2");
  AddDevToList(fBCMCheck[9],"batt3","batt3");
  AddDevToList(fBCMCheck[10],"batt4","batt4");
  AddDevToList(fBCMCheck[11],"batt5","batt5");

  Add2DDevToList(fInjBPMCheck[0],"bpm1I02xp","bpm1I02");
  Add2DDevToList(fInjBPMCheck[1],"bpm1I04xp","bpm1I04");
  Add2DDevToList(fInjBPMCheck[2],"bpm1I06xp","bpm1I06");
  Add2DDevToList(fInjBPMCheck[3],"bpm0I02xp","bpm0I02");
  Add2DDevToList(fInjBPMCheck[4],"bpm0I02Axp","bpm0I02A");
  Add2DDevToList(fInjBPMCheck[5],"bpm0I01xp","bpm0I01");
  Add2DDevToList(fInjBPMCheck[6],"bpm0I02xp","bpm0I02");
  Add2DDevToList(fInjBPMCheck[7],"bpm0I03xp","bpm0I03");
  Add2DDevToList(fInjBPMCheck[8],"bpm0I04xp","bpm0I04");
  Add2DDevToList(fInjBPMCheck[9],"bpm0I06xp","bpm0I06");

  Add2DDevToList(fBPMCheck[0],"bpm12xp","bpm12");
  Add2DDevToList(fBPMCheck[1],"bpm10xp","bpm10");
  Add2DDevToList(fBPMCheck[2],"bpm8xp","bpm8");
  Add2DDevToList(fBPMCheck[3],"bpm4axp","bpm4a");
  Add2DDevToList(fBPMCheck[4],"bpm4bxp","bpm4b");
  Add2DDevToList(fBPMCheck[5],"bpmcav1xr","bpmcav1");
  Add2DDevToList(fBPMCheck[6],"bpmcav2xr","bpmcav2");
  Add2DDevToList(fBPMCheck[7],"bpmcav3xr","bpmcav3");
  //luminosity
  AddDevToList(fLumiCheck[0],"lumi1r","lumi1");
  AddDevToList(fLumiCheck[1],"lumi2r","lumi2");
  AddDevToList(fLumiCheck[2],"lumi3r","lumi3");
  AddDevToList(fLumiCheck[3],"lumi4r","lumi4");
  AddDevToList(fLumiCheck[4],"lumi5r","lumi5");
  AddDevToList(fLumiCheck[5],"lumi6r","lumi6");
  // daltons
  AddDevToList(fDalCheck[0],"det1r","det1");
  AddDevToList(fDalCheck[1],"det2r","det2");
  AddDevToList(fDalCheck[2],"det3r","det3");
  AddDevToList(fDalCheck[3],"det4r","det4");
 //scanners (to be updated it is dummy name right now...)
  AddDevToList(fScanCheck[0],"scan1xr","scan1");
  AddDevToList(fScanCheck[1],"scan2xr","scan2");

 return 1;
}

Int_t TaPanam::CheckOptionConfig()
{
//  if (fTOptCheck[0]->GetState() == kButtonDown) fTSCorH = kFALSE;
//  if (fTOptCheck[1]->GetState() == kButtonDown) fTSCorH = kTRUE; 

 if (fADCOptCheck[0]->GetState() == kButtonDown) fADCdata = kFALSE;
 if (fADCOptCheck[1]->GetState() == kButtonDown) fADCdata = kTRUE;
 if (fADCOptCheck[2]->GetState() == kButtonDown) fADCSCorH = kFALSE;
 if (fADCOptCheck[3]->GetState() == kButtonDown) fADCSCorH = kTRUE;

 if (fBCMDispCheck[0]->GetState() == kButtonDown) fBCMplot = 1;
 if (fBCMDispCheck[1]->GetState() == kButtonDown) fBCMplot = 2;
 if (fBCMDispCheck[2]->GetState() == kButtonDown) fBCMplot = 3;
 if (fBCMOptCheck[0]->GetState() == kButtonDown)  fBCMSCorH =kFALSE;
 if (fBCMOptCheck[1]->GetState() == kButtonDown)  fBCMSCorH =kTRUE;

 if (fBPMDispCheck[0]->GetState() == kButtonDown) fBPMplot=1;
 if (fBPMDispCheck[1]->GetState() == kButtonDown) fBPMplot=2;
 if (fBPMDispCheck[2]->GetState() == kButtonDown) fBPMplot=3;
 if (fBPMOptCheck[0]->GetState() == kButtonDown)  fBPMSCorH = kFALSE;
 if (fBPMOptCheck[1]->GetState() == kButtonDown)  fBPMSCorH = kTRUE;
 return 1;
}


Int_t TaPanam::InitFlags()
{
  fFirstAna=kFALSE;
  fPrestart=kFALSE,fStart=kFALSE,fStop=kFALSE,fEnd=kTRUE; 

  fShowIADC      = kFALSE; fShowPADC     = kFALSE; fShowSADC = kFALSE;
  fShowInjBCM    = kFALSE; fShowBatt     = kFALSE; fShowBCM  = kFALSE;
  fShowInjBPM    = kFALSE; fShowBPM      = kFALSE; fShowBPMcav = kFALSE;
  fShowLumiFront = kFALSE; fShowLumiBack = kFALSE; fShowDET  = kFALSE;

  fTSCorH = kFALSE;  fADCSCorH  = kFALSE;
  fBCMSCorH = kFALSE; fBPMSCorH= kFALSE;
  fADCdata = kFALSE; 
  fBCMplot=1; fBPMplot=1;
  fDAQConfig = 0;
  fCODARunNumber ="";
  fConfigName ="";
 
  return 1;
}

Bool_t TaPanam::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
#ifdef DEBUG
      cout << "GET_MSG = " << GET_MSG(msg) << endl;
      cout << "GET_SUBMSG = " << GET_SUBMSG(msg) << endl;
#endif 
   Int_t status;
 switch (GET_MSG(msg))
    { 
    case kC_COMMAND:
      cout<<"kC_COMMAND called"<<endl;
     switch (GET_SUBMSG(msg)) 
        {
	 case kCM_LISTBOX:
           status = HandleListBox(parm1); break;
	 case kCM_CHECKBUTTON:
	   status = HandleCheckButton(parm1); break;
         case kCM_MENUSELECT:
               //printf("Pointer over menu entry, id=%ld\n", parm1);
             break;
         case kCM_MENU:
           status = HandleMenuBar(parm1); break;
	   default:
	     break;
	} // switch GET_SUBMSG(msg)
	 default:
	   break;
    }  // siwtch GET_MSG(msg)
return kTRUE; 
}

Int_t TaPanam::HandleCheckButton(Long_t parm1)
{
 if (!fPrestart && !fStart && !fStop)
   {
  switch(parm1)
    {
      //     case 10: UpdateDevListWithButton(fTimeCheck[0],string("multipletsynch"),1);  break;
    }
  }
  return 1;
}

Int_t TaPanam::HandleMenuBar(Long_t parm1)
{
  switch(parm1)
    {
     case M_FILE_EXIT:
       CloseWindow(); break; // exit Paname
     case M_ANA_ADC:
       if (fPrestart == kFALSE || fStart == kFALSE)
	 {
	   fDAQConfig = 0;  SetConfig(fDAQConfig); fConfigName = " ADC ";   

          cout<<" DAQ CONFIG = "<< fDAQConfig <<" ADC "<<endl;
	 }
       break;
     case M_ANA_BEAM:
       if (fPrestart == kFALSE || fStart == kFALSE)
	 {
	  fDAQConfig = 1;  SetConfig(fDAQConfig); fConfigName = " BEAM ";
          cout<<" DAQ CONFIG = "<< fDAQConfig <<" BEAM "<<endl;
	 }
       break;
     case M_ANA_I:
       if (fPrestart == kFALSE || fStart == kFALSE)
	 {
	  fDAQConfig = 2;  SetConfig(fDAQConfig); fConfigName = " INJ ";
          cout<<" DAQ CONFIG = "<< fDAQConfig <<" INJECTOR CRATE "<<endl;
	 }
       break;
     case M_ANA_P:
       if (fPrestart == kFALSE || fStart == kFALSE)
	 {
	  fDAQConfig = 3; SetConfig(fDAQConfig); fConfigName = "PARITY";
          cout<<" DAQ CONFIG = "<< fDAQConfig <<" PARITY CRATE "<<endl;
	 }
       break;
     case M_ANA_S:
       if (fPrestart == kFALSE || fStart == kFALSE)
	 {
	  fDAQConfig = 4; SetConfig(fDAQConfig); fConfigName = "HRS";
          cout<<" DAQ CONFIG = "<< fDAQConfig <<" HRS Crates "<<endl;
	 }
       break;
     case M_ANA_SAGDH:
       if (fPrestart == kFALSE || fStart == kFALSE)
	 {
	  fDAQConfig = 5; SetConfig(fDAQConfig); fConfigName = "SAGDH";
          cout<<" DAQ CONFIG = "<< fDAQConfig <<" SAGDH "<<endl;
	 }
       break;
     case M_ANA_HAPPEX:
       if (fPrestart == kFALSE || fStart == kFALSE)
	 {
	  fDAQConfig = 6; SetConfig(fDAQConfig); fConfigName = "HAPPEX";
          cout<<" DAQ CONFIG = "<< fDAQConfig <<" HAPPEX "<<endl;
	 }
       break;
     case M_ANA_MAN:
       if (fPrestart == kFALSE || fStart == kFALSE)
	 {
	  fDAQConfig = 7; SetConfig(fDAQConfig); fConfigName = "MANUAL";
          cout<<" DAQ CONFIG = "<< fDAQConfig <<" MANUAL "<<endl;
	 }
       break;

     case M_MON_PRESTART:
       cout<<" ###  PRESTART ### "<<endl; 
       TThread::Kill("refreshThread"); 
       TThread::Kill("PanThread");
       RefreshThreadStop(); 
       PanThreadStop();             
       fPrestart=kTRUE;
       fEnd=kFALSE;        
       //if (SetConfig(fDAQConfig)) cout<<" config read "<<endl;
       if (CheckOptionConfig()) cout<<" CheckOptionConfig() : User Config Device list Initialized "<<endl;
       cout<<"creating new analyzer"<<flush<<endl; 
       fAnaMan = new TaAnalysisManager();               
        //DumpOptionState();           
#ifdef CODAFILETEST 
       fAnaMan->Init(4030);
#endif
#ifdef ETCONNECT 
       fAnaMan->Init();
#endif
       // now analysis initialized get the list back to initialize display...
       if (SetDevList()) cout<<" SetDevList() Used Device list Initialized "<<endl;  
        fCODARunNumber = fAnaMan->GetAnalysis()->GetRun()->GetRunNumber();
        cout<<" runnum =  "<<fCODARunNumber<<endl; 
        fCodaRunBuff->AddText(0,fCODARunNumber);  
        fAnaMan->GetAnalysis()->InitDevicesList(fAllDeviceList);
        fAnaMan->GetAnalysis()->InitMonitorDevices();    
        if (InitHistoListBox(fAnaMan->GetAnalysis()->GetHistoForListBox())) 
	cout<<" InitHistoListBox() : List Box Filled \n";              
        fPrestart=kTRUE;
        // init canvas and display 
        //if ( fPrestart) cout<<" prestart OK"<<endl;
        if (InitCanvasPads()) cout<<" InitCanvasPads() : Canvas Flock Initialized "<<endl;
        cout<<" PRESTART SUCCEEDED.......\n";
	fMenuMon->DisableEntry(M_MON_PRESTART);   // disable PRESTART
        fMenuMon->EnableEntry(M_MON_START); 
        fMenuMon->EnableEntry(M_MON_END);
        break;
     case M_MON_START:
	{
         cout<<" ### START ### "<<endl;
         fPrestart=kFALSE;
         fStart=kTRUE;
	 fMenuMon->DisableEntry(M_MON_START);   // disable START
	 fMenuMon->DisableEntry(M_MON_END);   // disable START
	 fMenuMon->EnableEntry(M_MON_STOP);     // enable STOP
         if (!PanThreadStart())
	   { 
             cout<<" START SUCCEEDED..........\n ";
             RefreshThreadStart();
            }
         else cout<<" thread not launched....\n";
          
	}
	break;
      case M_MON_STOP:
	{ 
	  cout<<" ### STOP ###\n ";            
	  TThread::Kill("refreshThread"); 
	  TThread::Kill("PanThread");
          RefreshThreadStop(); 
          PanThreadStop();             
          cout<<" STOPPED..................\n";
          fStart=kFALSE;
          fStop=kTRUE;
	  fMenuMon->DisableEntry(M_MON_STOP);     // disable STOP
	  fMenuMon->EnableEntry(M_MON_START);   // disable START
	  fMenuMon->EnableEntry(M_MON_END);   // disable START
	  fMenuMon->DisableEntry(M_MON_PRESTART);   // enable PRESTART
        } 
	break;
      case M_MON_END:
        { 
          cout<<" ### END ### \n";
          gDirectory->GetList()->Dump();
          gDirectory->DeleteAll();
          fAnaMan->End(); 
//           delete fAnaMan;
//          fAnaMan = NULL;
	  if (ClearCanvas())  cout<<" ClearCanvas()  : Canvas flock cleared"<<endl;
	  if (ClearListBox()) cout<<" ClearListBox() : ListBox Emptied"<<endl;
          if (ClearListOfDevice()) cout<<" ClearListofDevice() : Device lists cleared"<<endl;
          if (InitFlags())        cout<<" InitFlags() : Flags Reinitialized"<<endl;
	 if (InitParameters()) cout<<" InitParameters() : Parameters Initialized "<<endl;
         //if (CheckDevListConfig()) cout<<" CheckDevListConfig() : Check user device config "<<endl;
         fPrestart=kFALSE;
         fStart=kFALSE;
         fStop=kFALSE;
         fEnd=kTRUE;
         fMenuMon->DisableEntry(M_MON_START);    // disable START
         fMenuMon->DisableEntry(M_MON_STOP);     // disable STOP
	 fMenuMon->EnableEntry(M_MON_PRESTART);     // enable PRESTART
         cout<<" ENDED................................................................... "<<endl;
        }
        break;
      
      default:
	break;
    }
  fConfigBuff->AddText(0,fConfigName);
  return 1;
}

Int_t TaPanam::HandleListBox(Long_t parm1)
{
//  cout<<" kCM_LISTBOX called "<<endl;
// #ifdef LISTBOX
//  Int_t id;
//  id=fHistoListBox->GetSelected();
//  cout<<" id="<<id<<endl;
//  TGTextLBEntry* tentry;
//  tentry = (TGTextLBEntry*) (fHistoListBox->GetSelectedEntry());
//  cout<<" text of selected entry="<<tentry->GetText()->GetString()<<endl;
//  fEmCanDevPlotName = (Char_t*) tentry->GetText()->GetString();
//  Char_t* plstr; 
//  plstr =  (Char_t*) tentry->GetText()->GetString();
//  fEmCanDevName= strtok (plstr,"_");
//  //cout<<"fEmCanDevName="<<fEmCanDevName<<endl;
//  fEmCanDevName = strtok (NULL,"_");
//  if (!strncmp((Char_t*) tentry->GetText()->GetString(),"S",1)) fEmCanDevPlotType=1;
//  if (!strncmp((Char_t*) tentry->GetText()->GetString(),"H",1)) fEmCanDevPlotType=2;
//  cout<<"fEmCanDevName="<<fEmCanDevName<<endl; 
//  cout<<"fEmCanDevPlotName="<<fEmCanDevPlotName<<endl;   
//  cout<<"fEmCanDevPlotType="<<fEmCanDevPlotType<<endl;
//  fHistoListBox->AddEntrySort(fEmCanDevPlotName,id);
//  MapSubwindows();
//  Resize(GetDefaultSize()); 
//  // reloader l'entry dans la liste box.....a cause des problemes....d'affichages....
//  delete plstr;   
// #ifdef EMCANON
//   UpdateEmCanvas();
// #endif
// #endif
  return 1;
}

Int_t TaPanam::InitParameters(){

   
  fIADCNum  = 0; fPADCNum  = 0; fSADCNum  = 0;
  fInjBattNum =0; fBattNum = 0; fInjBCMNum = 0; fUnserNum = 0;
  fQuadNum = 0; fBCM12Num = 0; fBCM34Num = 0; fBCMcavNum = 0;
  fInjBPMNum = 0; fBPMNum = 0; fBPMcavNum = 0; 
  fLumiFrontNum = 0;  fLumiBackNum = 0; fDETNum = 0;
//   fIADCidx.clear(); fPADCidx.clear(); fSADCidx.clear();
//   fBCMidx.clear();fBattidx.clear(); fBCMCAVidx.clear();fBPMidx.clear();fBPMINidx.clear();
//   fBPMCAVidx.clear();fLUMIidx.clear();fDETidx.clear();
  
  fDeviceList.clear();fADeviceList.clear();fAllDeviceList.clear();
  fADCdata = kTRUE;

  return 1;
}

void TaPanam::ResetDevCheckButtons()
{
  // for ( Int_t icb = 0 ; icb <7; icb++) fTimeCheck[icb]->SetState(kButtonUp);
 for ( Int_t icb = 0 ; icb <3; icb++) 
   {
    fIADCCheck[icb]->SetState(kButtonUp);
   }
 for ( Int_t icb = 0 ; icb <9; icb++) fADCCheck[icb]->SetState(kButtonUp);
 for ( Int_t icb = 0 ; icb <2; icb++) 
   {
    fSADCCheck[icb]->SetState(kButtonUp);fScanCheck[icb]->SetState(kButtonUp);
    fInjBCMCheck[icb]->SetState(kButtonUp);
   }
 for ( Int_t icb = 0 ; icb <8; icb++) fBCMCheck[icb]->SetState(kButtonUp);
 for ( Int_t icb = 0 ; icb <10; icb++) fInjBPMCheck[icb]->SetState(kButtonUp);
 for ( Int_t icb = 0 ; icb <5; icb++) 
   {
    fBPMCheck[icb]->SetState(kButtonUp);
   }
 for ( Int_t icb = 0 ; icb <6; icb++) 
   {
    fLumiCheck[icb]->SetState(kButtonUp);
   }
 for ( Int_t icb = 0 ; icb <4; icb++) fDalCheck[icb]->SetState(kButtonUp); 
 for ( Int_t icb = 0 ; icb <2; icb++) fScanCheck[icb]->SetState(kButtonUp); 
}

Int_t TaPanam::InitDevCheckButtons(){
  // first reset them :
  ResetDevCheckButtons();
 
// manual checks:

//time
//  fTimeCheck[1]->SetState(kButtonDown);
//  fTimeCheck[2]->SetState(kButtonDown);
//  fTimeCheck[3]->SetState(kButtonDown);
//  fTimeCheck[4]->SetState(kButtonDown);
//Injector ADC
  //fIADCCheck[0]->SetState(kButtonDown);
  //fIADCCheck[1]->SetState(kButtonDown);
  //fIADCCheck[2]->SetState(kButtonDown);
//parity ADC
//   fADCCheck[0]->SetState(kButtonDown);
//   fADCCheck[1]->SetState(kButtonDown);
//   fADCCheck[2]->SetState(kButtonDown);
//   fADCCheck[3]->SetState(kButtonDown);
//   fADCCheck[4]->SetState(kButtonDown);
//   fADCCheck[5]->SetState(kButtonDown);
//   fADCCheck[6]->SetState(kButtonDown);
//   fADCCheck[7]->SetState(kButtonDown);
//   fADCCheck[8]->SetState(kButtonDown);
//   fADCCheck[9]->SetState(kButtonDown);
//spectro ADC
//   fSADCCheck[0]->SetState(kButtonDown);
//   fSADCCheck[1]->SetState(kButtonDown);
//BCMs
//1,2
// fBCMCheck[0]->SetState(kButtonDown);
// fBCMCheck[1]->SetState(kButtonDown);
 // 3,4
  // fBCMCheck[2]->SetState(kButtonDown);
  // fBCMCheck[3]->SetState(kButtonDown);
 //unser
 //fBCMCheck[4]->SetState(kButtonDown);
 //injector
 //fBCMCheck[5]->SetState(kButtonDown);
 //batteries
 //fBCMCheck[6]->SetState(kButtonDown);
 //fBCMCheck[7]->SetState(kButtonDown);
 //BPMs
 // Inj BPMs
 // 1I0 family
//  fInjBPMCheck[0]->SetState(kButtonDown);
//    fInjBPMCheck[1]->SetState(kButtonDown);
//  fInjBPMCheck[2]->SetState(kButtonDown);
 // 0I0 family
//  fInjBPMCheck[3]->SetState(kButtonDown);
//  fInjBPMCheck[4]->SetState(kButtonDown);
 // I0 family
//  fInjBPMCheck[5]->SetState(kButtonDown);
//  fInjBPMCheck[6]->SetState(kButtonDown);
//  fInjBPMCheck[7]->SetState(kButtonDown);
//  fInjBPMCheck[8]->SetState(kButtonDown);
//  fInjBPMCheck[9]->SetState(kButtonDown);
// HALLA BPMs
// fBPMCheck[0]->SetState(kButtonDown);
// fBPMCheck[1]->SetState(kButtonDown);
// fBPMCheck[2]->SetState(kButtonDown);
// fBPMCheck[3]->SetState(kButtonDown);
// fBPMCheck[4]->SetState(kButtonDown);
 //daltons
 //for (Int_t i =0 ; i<4  ;i++) fDalCheck[i]->SetState(kButtonDown);
//lumi
  //for (Int_t i =0 ; i<6  ;i++) fLumiCheck[i]->SetState(kButtonDown);
  return 1;
}

string 
TaPanam::itos(Int_t i)
{
  ostringstream buffer;
  buffer<<i;
  return buffer.str();
}


void TaPanam::DisplayThis(TGCheckButton *thebutton, Bool_t showcan)
{
  if (thebutton->IsDown()) 
    {
      if (thebutton->GetState() == kButtonDown) showcan = kTRUE;
      else showcan = kFALSE; 
    }
}

Int_t TaPanam::InitHistoListBox(vector<string> thelist)
{// display the list of "sub" histos and stripcharts contained
 // in each TaPAnamDevice object in the List box of the main GUI frame. 
  Int_t k=1;
  if (ClearListBox() && thelist.size() !=0)
    {
     for ( vector<string>::const_iterator i=thelist.begin(); i!=thelist.end();i++)
        { 
	  //	  cout<<" add entry to list box string="<<*i<<" char*="<<(*i).c_str()<<"with number :"<<k<<endl;
         fHistoListBox->AddEntry((char*)(*i).c_str(),k);
         k++;
        }
     MapSubwindows();
     fHistoListBox->Layout();
     Resize(1000,500);
     MapWindow();
     return 1;
    }
  else 
    {
     cout<<"TaPanam::InitHistoListBox() nothing initialized, vector size null \n";
     return 0;
    }
  return 0;
}

Int_t TaPanam::InitListOfAllPlots()
{
 for (vector<string>::const_iterator i= fDeviceList.begin(); i!= fDeviceList.end(); i++) 
   fAllDeviceList.push_back(*i);
 for (vector<string>::const_iterator i= fADeviceList.begin(); i!= fADeviceList.end(); i++) 
   fAllDeviceList.push_back(*i);
 cout<<" = = =  Dump ALL DEVICE LIST = = = "<<endl;        
 for (vector<string>::const_iterator i= fAllDeviceList.begin(); i!= fAllDeviceList.end(); i++)
    cout<<*i<<endl;
 return 1; 
}

void TaPanam::SetNumOfPads()
{
 for (UInt_t i = 0; i< fAllDeviceList.size(); i++)
   {
     if (!strncmp((fAllDeviceList[i]).c_str(),"adc8",5) || 
         !strncmp((fAllDeviceList[i]).c_str(),"adc6",5) || 
         !strncmp((fAllDeviceList[i]).c_str(),"adc16",5))
       {
	 fIADCNum++; fShowIADC = kTRUE;
       }   
     else if (!strncmp((fAllDeviceList[i]).c_str(),"adc17",5) || 
              !strncmp((fAllDeviceList[i]).c_str(),"adc29",5))
       {
        fSADCNum++; fShowSADC = kTRUE;
       }       
     else if (!strncmp((fAllDeviceList[i]).c_str(),"adc",3)) 
       {
        fPADCNum++; fShowPADC = kTRUE;     
       }
     if (!strncmp((fAllDeviceList[i]).c_str(),"bcm1",5) || 
         !strncmp((fAllDeviceList[i]).c_str(),"bcm2",5))
       {
	 fBCM12Num =1; fShowBCM = kTRUE; 
       }
     if (!strncmp((fAllDeviceList[i]).c_str(),"bcm3",5) || 
         !strncmp((fAllDeviceList[i]).c_str(),"bcm4",5))
       {
	 fBCM34Num =1; fShowBCM = kTRUE;
       } 
     if (!strncmp((fAllDeviceList[i]).c_str(),"bcm5",5))
       {
	fUnserNum =1; fShowBCM = kTRUE;
       } 
     if (!strncmp((fAllDeviceList[i]).c_str(),"bcm6",5))
       {
	fInjBCMNum =1; fShowInjBCM = kTRUE;
       } 
     if (!strncmp((fAllDeviceList[i]).c_str(),"bcmca",5))
       {
	 fBCMcavNum =1; fShowBCM = kTRUE;
       } 
     if (!strncmp((fAllDeviceList[i]).c_str(),"batt",4))
       {
	 fBattNum = 1; fShowBatt = kTRUE;
       } 
//      if (!strncmp((fAllDeviceList[i]).c_str(),"qpd",3))
//        {
// 	 fQuadNum =1; fShowInjBCM = kTRUE;
//        } 
     if (!strncmp((fAllDeviceList[i]).c_str(),"bpm1I",5) || 
         !strncmp((fAllDeviceList[i]).c_str(),"bpm0I",5)) 
       {
        fInjBPMNum =1; fShowInjBPM = kTRUE;
       }
     if (!strncmp((fAllDeviceList[i]).c_str(),"bpmca",5))
       {
        fBPMcavNum =1; fShowBPMcav = kTRUE;
       }
     if (!strncmp((fAllDeviceList[i]).c_str(),"bpm4",4))
       {
        fBPMNum = 1; fShowBPM = kTRUE;
       }
     else if (!strncmp((fAllDeviceList[i]).c_str(),"bpm",3))
       {
        fBPMNum = 2; fShowBPM = kTRUE;
       }
     if (!strncmp((fAllDeviceList[i]).c_str(),"lumi1",5) || 
         !strncmp((fAllDeviceList[i]).c_str(),"lumi2",5) || 
         !strncmp((fAllDeviceList[i]).c_str(),"lumi3",5) || 
         !strncmp((fAllDeviceList[i]).c_str(),"lumi4",5)) 
       {
        fLumiBackNum =1; fShowLumiBack = kTRUE;   
       }
     if (!strncmp((fAllDeviceList[i]).c_str(),"lumi5",5) || 
         !strncmp((fAllDeviceList[i]).c_str(),"lumi6",5)) 
       {
        fLumiFrontNum =1; fShowLumiFront = kTRUE;   
       }
     if (!strncmp((fAllDeviceList[i]).c_str(),"det",3)) 
       {
        fDETNum++; fShowDET = kTRUE;
       }
   }
#ifdef DEBUGDCOUNT
//   cout<<" fTimeNum="<<fTimeNum<<" fIADCNum="<<fIADCNum<<endl;
//   cout<<" fPADCNum="<<fPADCNum<<" fSADCNum="<<fSADCNum<<endl;
//   cout<<" fBCMNum="<<fBCMNum<<" fBCMCAVNum="<<fBCMCAVNum<<endl;
//   cout<<" fBPMINNum="<<fBPMINNum<<" fBPMNum="<<fBPMNum<<endl;
//   cout<<" fBPMCAVNum="<<fBPMCAVNum<<endl;
//   cout<<" fLUMNum="<<fLUMNum<<" fDETNum="<<fDETNum<<endl;

//   cout<<"Time idx size ="<<fTimeidx.size()<<endl;  
//   cout<<"IADC idx size ="<<fIADCidx.size()<<endl;
//   cout<<"PADC idx size ="<<fPADCidx.size()<<endl;
//   cout<<"SADC idx size ="<<fSADCidx.size()<<endl;
//   cout<<"BCM  idx size ="<<fBCMidx.size()<<endl;
//   cout<<"BPMIN idxsize ="<<fBPMidx.size()<<endl;
//   cout<<"BPM idx size ="<<fBPMINidx.size()<<endl;
//   cout<<"BCMCAV idx size ="<<fBCMCAVidx.size()<<endl;
//   cout<<"BPMCAV idx size ="<<fBPMCAVidx.size()<<endl;
//   cout<<"LUMI idx   vector  size ="<<fLUMIidx.size()<<endl;
//   cout<<"DET idx   vector  size ="<<fDETidx.size()<<endl;

#endif

}

void TaPanam::DumpOptionState()
{
  if (fFirstAna) cout <<"fFirstAna = T "; else cout<<"fFirstAna = F \n\n";

  if (fShowIADC) cout <<" fShowIADC = T "; else cout<<" fShowIADC = F";
  if (fShowPADC) cout <<" fShowPADC = T "; else cout<<" fShowPADC = F";
  if (fShowSADC) cout <<" fShowSADC = T "; else cout<<" fShowSADC = F ";
  if (fShowInjBCM) cout <<" fShowInjBCM = T "; else cout<<" fShowInjBCM = F ";
  if (fShowBatt) cout <<" fShowBatt = T "; else cout<<" fShowBatt = F ";
  if (fShowBCM) cout <<" fShowBCM = T ";   else cout<<" fShowBCM = F ";

  if (fShowInjBPM) cout <<"fShowInjBPM = T ";else cout<<"fShowInjBPM = F ";
  if (fShowBPM) cout <<" fShowBPM = T ";else cout<<" fShowBPM = F ";
  if (fShowBPMcav) cout <<" fShowBPMcav = T ";else cout<<" fShowBPMcav = F ";
   if (fShowLumiBack) cout <<" fShowLumiBack = T \n\n";else cout<<" fShowLumiBack = F \n\n";
   if (fShowLumiFront) cout <<" fShowLumiFront = T \n\n";else cout<<" fShowLumiFront = F \n\n";
   if (fShowDET) cout <<" fShowDET = T \n\n";else cout<<" fShowDET = F \n\n";

  if (fTSCorH) cout <<" fTSCorH = T ";else cout<<"fTSCorH = F ";
  if (fADCSCorH) cout <<" fADCSCorH = T ";else cout<<" fADCSCorH = F ";
  if (fBCMSCorH) cout <<" fBCMSCorH = T ";else cout<<" fBCMSCorH = F ";
  if (fBPMSCorH) cout <<" fBPMSCorH = T \n\n";else cout<<" fBPMSCorH = F \n\n";

  if (fADCdata) cout <<" fADCdata = T ";else cout<<" fADCdata = F ";

  cout <<" fBCMplot = "<<fBCMplot;
  cout <<" fBPMplot = "<<fBPMplot<<" \n \n ";

  if (fPrestart) cout <<" fPrestart = T ";else cout<<" fPrestart = F ";
  if (fStart) cout    <<" fStart = T ";else cout<<" fStart = F ";
  if (fStop) cout     <<" fStop = T ";else cout<<" fStop = F ";
  if (fEnd) cout      <<" fEnd = T \n\n";else cout<<" fEnd = F \n\n";   

}

void TaPanam::CanvasSafeDelete(TCanvas* can, char* name )
{
  TObject* obj = (gROOT->GetListOfCanvases()->FindObject(name));
  if (obj == (TObject*) can)
  if (can) delete can;
  can = NULL;
}


Int_t TaPanam::UpdateCanvasFlock()
{
  if (fShowIADC)
   {
    fIADCCan =  new TCanvas("IADC", "Injector DAQ ADCs",1,1,100,100);
    fIADCRMSCan =  new TCanvas("IADCRMS", "Injector DAQ ADCs RMS",1,1,100,100);
   }
 else if (!fShowIADC && gROOT->GetListOfCanvases()->FindObject("IADC"))
   {
     cout<<"already defined canvas IADC/IADCRMS deleting"<<endl;
     CanvasSafeDelete(fIADCCan,"IADC");
     CanvasSafeDelete(fIADCRMSCan,"IADCRMS");
   }
  else cout<<"No canvas IADC/IADCRMS created"<<endl;
 if (fShowPADC)
   {
    fPADCCan =  new TCanvas("PADC", "Parity DAQ ADCs",100,1,300,300);
    fPADCCan =  new TCanvas("PADCRMS", "Parity DAQ ADCs RMS",100,1,300,300);
   }
else if (!fShowPADC && gROOT->GetListOfCanvases()->FindObject("PADC"))
   {
     cout<<"already defined canvas PADC/PADCRMS deleting"<<endl;
     CanvasSafeDelete(fPADCCan,"IADC");
     CanvasSafeDelete(fPADCRMSCan,"IADCRMS");
   }
  else cout<<"No canvas PADC/PADCRMS created"<<endl;
 if (fShowSADC)
   {
    fSADCCan =  new TCanvas("SADC", "HRS DAQ ADCs",400,1,100,100);
    fSADCCan =  new TCanvas("SADCRMS", "HRS DAQ ADCs RMS",400,1,100,100);
   }
 else if (!fShowSADC && gROOT->GetListOfCanvases()->FindObject("SADC"))
   {
     cout<<"already defined canvas SADC/SADCRMS deleting"<<endl;
     CanvasSafeDelete(fSADCCan,"SADC");
     CanvasSafeDelete(fSADCRMSCan,"SADCRMS");
   }
  else cout<<"No canvas SADC/SADCRMS created"<<endl;
 if (fShowInjBCM)
    fInjBCMCan =  new TCanvas("InjBCM", "Injector current monitor ",620,1,600,600);
 else if (!fShowInjBCM && gROOT->GetListOfCanvases()->FindObject("InjBCM"))
   {
     cout<<"already defined canvas InjBCM deleting"<<endl;
     CanvasSafeDelete(fInjBCMCan,"InjBCM");
   }
 else cout<<"No canvas InjBCM created"<<endl;
 if (fShowBatt)
    fBattCan =  new TCanvas("Batt", "Batt photodiodes",1,1,100,100);
 else if (!fShowBatt && gROOT->GetListOfCanvases()->FindObject("Batt"))
   {
     cout<<"already defined canvas Batt, deleting"<<endl;
     CanvasSafeDelete(fBattCan,"Batt");
   }
 else cout<<"No canvas Batt created"<<endl;
 if (fShowBCM)
    fBCMCan =  new TCanvas("BCM", "Beam Current",620,1,300,300);
 else if (!fShowBCM && gROOT->GetListOfCanvases()->FindObject("BCM"))
   {
     cout<<"already defined canvas BCM deleting"<<endl;
     CanvasSafeDelete(fBCMCan,"BCM");
   }
 else cout<<"No canvas BCM created"<<endl;
 if (fShowInjBPM)
    fInjBPMCan =  new TCanvas("InjBPM", "Injector BPMs ",1,1,100,100);
 else if (!fShowInjBPM && gROOT->GetListOfCanvases()->FindObject("InjBPM"))
   {
     cout<<"already defined canvas InjBPM, deleting"<<endl;
     CanvasSafeDelete(fInjBPMCan,"InjBPM");
   }
 else cout<<"No canvas InjBPM created"<<endl;

 if (fShowBPM)
    fBPMCan =  new TCanvas("BPM", "HALL A Stripline BPMs",1,20,500,500);
 else if (!fShowBPM && gROOT->GetListOfCanvases()->FindObject("BPM"))
   {
     cout<<"already defined canvas BPM deleting"<<endl;
     CanvasSafeDelete(fBPMCan,"BPM");
   }
 else cout<<"No canvas BPM created"<<endl;
 if (fShowBPMcav)
    fBPMcavCan =  new TCanvas("BPMcav", "HALL A position cavity monitors",1,20,500,500);
 else if (!fShowBPMcav && gROOT->GetListOfCanvases()->FindObject("BPMcav"))
   {
     cout<<"already defined canvas BPMcav deleting"<<endl;
     CanvasSafeDelete(fBPMCan,"BPMcav");
   }
 else cout<<"No canvas BPMcav created"<<endl;

 if (fShowLumiFront) fLumiFrontCan =  new TCanvas("LumiFront", "Front luminosity detector ",1,1,600,500);
 else if (!fShowLumiFront && gROOT->GetListOfCanvases()->FindObject("LumiFront"))
   {
     cout<<"already defined canvas LumiFront deleting"<<endl;
     CanvasSafeDelete(fLumiFrontCan,"LumiFront");
   }
 else cout<<"No canvas LumiFront created"<<endl;

 if (fShowLumiBack) fLumiBackCan =  new TCanvas("LumiBack", "Back luminosity detector ",1,1,600,500);
 else if (!fShowLumiBack && gROOT->GetListOfCanvases()->FindObject("LumiBack"))
   {
     cout<<"already defined canvas LumiBack deleting"<<endl;
     CanvasSafeDelete(fLumiBackCan,"LumiBack");
   }
 else cout<<"No canvas LumiBack created"<<endl;

 if (fShowDET) fDETCan =  new TCanvas("DET", "The Daltons",1,1,600,500);
 else if (!fShowDET && gROOT->GetListOfCanvases()->FindObject("DET"))
   {
     cout<<"already defined canvas DET deleting"<<endl;
     CanvasSafeDelete(fDETCan,"DET");
   }
 else cout<<"No canvas DET created"<<endl;
 return 1;
}

void TaPanam::ResetIdxVector()
{
  ;fIADCidx.clear();
  fPADCidx.clear();fSADCidx.clear();
  fBCMidx.clear();fBCMcavidx.clear();
  fBPMidx.clear();fInjBPMidx.clear();
  fBPMcavidx.clear();fLUMIBackidx.clear();fLUMIFrontidx.clear();
  fDETidx.clear();
}

void TaPanam::UpdateCanvasWithDevice(Char_t * title, vector<Int_t> idxvect, Int_t devtype, Bool_t disptype)
{
  TCanvas* currentcan;
  if ((gROOT->GetListOfCanvases()->FindObject(title))!=NULL)
    { 
      //      cout<<"found canvas named="<<title<<flush<<endl;
      currentcan = (TCanvas*)(gROOT->GetListOfCanvases()->FindObject(title));
      if (fPrestart == kTRUE)
	{
	  switch (devtype)
             {
	     case 0:
              	 if (!disptype)
	           {
		    fStripStyle->cd();
                    currentcan->Divide((UInt_t(idxvect.size()))); 
                    currentcan->UseCurrentStyle();
                    currentcan->cd();
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
	              currentcan->cd(idx+1);
                      fAnaMan->GetAnalysis()->InitDevicePad(idxvect[idx],0,disptype,0);
	             }
		   }
		  else 
		   {
		    fHistoStyle->cd();
                    currentcan->Divide(idxvect.size()); 
                    currentcan->UseCurrentStyle();
                    currentcan->cd();
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
	              currentcan->cd(idx+1);
                      fAnaMan->GetAnalysis()->InitDevicePad(idxvect[idx],0,disptype,0);
	             }
		   }               
	       break;
	    case 1:
	      //	      cout<<" case 1 :DEV type device.... vector size="<<idxvect.size()<<flush<<endl; 
              	 if (!disptype)
	           {
		    fStripStyle->cd();
                    currentcan->Divide((UInt_t(idxvect.size()*2))); 
                    currentcan->UseCurrentStyle();
                    currentcan->cd();
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
	              currentcan->cd(idx*2+1);
                      fAnaMan->GetAnalysis()->InitDevicePad(idxvect[idx],0,disptype,0);
	              currentcan->cd(idx*2+2);
                      fAnaMan->GetAnalysis()->InitDevicePad(idxvect[idx],1,disptype,0);
	             }
		   }
		  else 
		   {
		    fHistoStyle->cd();
                    currentcan->UseCurrentStyle();
                    currentcan->Divide(idxvect.size()); 
                    currentcan->cd();
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
	              currentcan->cd(idx+1);
                      fAnaMan->GetAnalysis()->InitDevicePad(idxvect[idx],0,disptype,0);
	             }
		   }
	       break;
	   case 2: 
	     //	     cout<<" case 2 : ADEV type device... vector size="<<idxvect.size()<<flush<<endl; 
              	 if (!disptype)
	           {
		    fStripStyle->cd();
                    currentcan->Divide(4,idxvect.size()); 
                    currentcan->UseCurrentStyle();
                    currentcan->cd();
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
	              currentcan->cd(idx*4+1);
                      fAnaMan->GetAnalysis()->InitADevicePad(idxvect[idx],0,disptype,0);
	              currentcan->cd(idx*4+2);
                      fAnaMan->GetAnalysis()->InitADevicePad(idxvect[idx],1,disptype,0);
	              currentcan->cd(idx*4+3);
                      fAnaMan->GetAnalysis()->InitADevicePad(idxvect[idx],2,disptype,0);
	              currentcan->cd(idx*4+4);
                      fAnaMan->GetAnalysis()->InitADevicePad(idxvect[idx],3,disptype,0);
	             }
		   }
		  else 
		   {
		    fHistoStyle->cd();
                    currentcan->UseCurrentStyle();
                    currentcan->Divide(2,idxvect.size()); 
                    currentcan->cd();
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
	              currentcan->cd(idx*2+1);
                      fAnaMan->GetAnalysis()->InitADevicePad(idxvect[idx],0,disptype,0);
	              currentcan->cd(idx*2+2);
                      fAnaMan->GetAnalysis()->InitADevicePad(idxvect[idx],1,disptype,0);
	             }
		   }
	       break;
	  case 3: 
              	 if (!disptype)
	           {
		    fStripStyle->cd();
                    currentcan->UseCurrentStyle();
                    currentcan->Divide(2,idxvect.size()); 
                    currentcan->cd();
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
	              currentcan->cd(idx*2+1);
                      fAnaMan->GetAnalysis()->InitADevicePad(idxvect[idx],2,disptype,0);
	              currentcan->cd(idx*2+2);
                      fAnaMan->GetAnalysis()->InitADevicePad(idxvect[idx],3,disptype,0);
	             }
		   }
		  else 
		   {
		    fHistoStyle->cd();
                    currentcan->UseCurrentStyle();
                    currentcan->Divide(idxvect.size()); 
                    currentcan->cd();
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
	              currentcan->cd(idx+1);
                      fAnaMan->GetAnalysis()->InitADevicePad(idxvect[idx],1,disptype,0);
	             }
		   }
	       break;
              
	    break;

	   } // switch
	}// prestart cond
      else if(fStart)
	{
	 switch (devtype)
            {
	    case 0:
              	 if (!disptype)
	           {
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
		      fStripStyle->cd();
                      currentcan->UseCurrentStyle();
	              currentcan->cd(idx+1);
                      fAnaMan->GetAnalysis()->DisplayDevice(idxvect[idx],0,disptype,0);
	             }
                    currentcan->Update();	             
		   }
		  else 
		   {
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
		      fHistoStyle->cd();
                      currentcan->UseCurrentStyle();
	              currentcan->cd(idx+1);
                      fAnaMan->GetAnalysis()->DisplayDevice(idxvect[idx],0,disptype,0);
	             }
                    currentcan->Update();	             
		   }             
	      break;
	    case 1:
              	 if (!disptype)
	           {
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
		      fStripStyle->cd();
                      currentcan->UseCurrentStyle();
	              currentcan->cd(idx*2+1);
                      fAnaMan->GetAnalysis()->DisplayDevice(idxvect[idx],0,disptype,0);
	              currentcan->cd(idx*2+2);
                      fAnaMan->GetAnalysis()->DisplayDevice(idxvect[idx],1,disptype,0);
	             }
                    currentcan->Update();	             
		   }
		  else 
		   {
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
		      fHistoStyle->cd();
                      currentcan->UseCurrentStyle();
	              currentcan->cd(idx+1);
                      fAnaMan->GetAnalysis()->DisplayDevice(idxvect[idx],0,disptype,0);
	             }
                    currentcan->Update();	             
		   }
	       break;
	   case 2: 
              	 if (!disptype)
	           {
		    fStripStyle->cd();
                    currentcan->UseCurrentStyle();
                    currentcan->cd();
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
	              currentcan->cd(idx*4+1);
                      fAnaMan->GetAnalysis()->DisplayADevice(idxvect[idx],0,disptype,0);
	              currentcan->cd(idx*4+2);
                      fAnaMan->GetAnalysis()->DisplayADevice(idxvect[idx],1,disptype,0);
	              currentcan->cd(idx*4+3);
                      fAnaMan->GetAnalysis()->DisplayADevice(idxvect[idx],2,disptype,0);
	              currentcan->cd(idx*4+4);
                      fAnaMan->GetAnalysis()->DisplayADevice(idxvect[idx],3,disptype,0);
                      currentcan->Update();	             
	             }
		   }
		  else 
		   {
		    fHistoStyle->cd();
                    currentcan->UseCurrentStyle();
                    currentcan->cd();
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
	              currentcan->cd(idx*2+1);
                      fAnaMan->GetAnalysis()->DisplayADevice(idxvect[idx],0,disptype,0);
	              currentcan->cd(idx*2+2);
                      fAnaMan->GetAnalysis()->DisplayADevice(idxvect[idx],1,disptype,0);
                      currentcan->Update();	             
                     }
		   }
	       break;
	  case 3: 
              	 if (!disptype)
	           {
		    fStripStyle->cd();
                    currentcan->UseCurrentStyle();
                    currentcan->cd();
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
	              currentcan->cd(idx*2+1);
                      fAnaMan->GetAnalysis()->DisplayADevice(idxvect[idx],2,disptype,0);
	              currentcan->cd(idx*2+2);
                      fAnaMan->GetAnalysis()->DisplayADevice(idxvect[idx],3,disptype,0);
                      currentcan->Update();	             
	             }
		   }
		  else 
		   {
                    currentcan->cd();
		    fHistoStyle->cd();
                    currentcan->UseCurrentStyle();
                    for ( UInt_t idx = 0; idx < idxvect.size(); idx++)
	             {
	              currentcan->cd(idx+1);
                      fAnaMan->GetAnalysis()->DisplayADevice(idxvect[idx],1,disptype,0);
                      currentcan->Update();	             
                     }
		   }
	    break;
	     } // switch
	  } // start
      else if (!fPrestart) cout <<" Not prestart..."<<endl;
      else if (!fStart) cout <<" Not start..."<<endl;
    }
}



void TaPanam::UpdateIADCStack(Bool_t disptype)
{
 TCanvas* currentcan;
 UInt_t candiv=(UInt_t) fIADCNum/4;
 if (gROOT->GetListOfCanvases()->FindObject("IADC") != NULL)
   { 
    currentcan = (TCanvas*)(gROOT->GetListOfCanvases()->FindObject("IADC"));
    currentcan->cd();
    if ( fPrestart == kTRUE )
      {          
       currentcan->Divide(1,candiv);
       for ( UInt_t idx = 0; idx < candiv; idx++)
	 {
	  currentcan->cd(idx+1);
          if ( !disptype )
	    {
             fStripStyle->cd();
             currentcan->UseCurrentStyle();
             fAnaMan->GetAnalysis()->DrawADCStack(0,idx,0,0);
	    }
          else
            {
	     fHistoStyle->cd();
             currentcan->UseCurrentStyle();
             fAnaMan->GetAnalysis()->DrawADCStack(0,idx,2,0);
            }  
         }
       currentcan->Update(); 
	}
      if ( fStart == kTRUE )
        {          
         for ( UInt_t idx = 0; idx < candiv; idx++)
	   {
	    currentcan->cd(idx+1);
            if ( !disptype )
	      {
               fStripStyle->cd();
               currentcan->UseCurrentStyle();
               fAnaMan->GetAnalysis()->DrawADCStack(0,idx,0,1);
	      }
            else
	      {
	       fHistoStyle->cd();
               currentcan->UseCurrentStyle();
               fAnaMan->GetAnalysis()->DrawADCStack(0,idx,2,0);
	      }  
	 }
      currentcan->Update(); 
      }
   }
}

void TaPanam::UpdateIADCRMSStack()
{
 TCanvas* currentcan;
 UInt_t candiv=(UInt_t) fIADCNum/4;
 if (gROOT->GetListOfCanvases()->FindObject("IADCRMS") != NULL)
   { 
    currentcan = (TCanvas*)(gROOT->GetListOfCanvases()->FindObject("IADCRMS"));
    currentcan->cd();
    if ( fPrestart == kTRUE )
      {          
       currentcan->Divide(1,candiv);
       for ( UInt_t idx = 0; idx < candiv; idx++)
	 {
	  currentcan->cd(idx+1);
          fStripStyle->cd();
          currentcan->UseCurrentStyle();
          fAnaMan->GetAnalysis()->DrawADCStack(0,idx,1,0);
         }
       currentcan->Update(); 
	}
      if ( fStart == kTRUE )
        {          
         for ( UInt_t idx = 0; idx < candiv; idx++)
	   {
	    currentcan->cd(idx+1);
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawADCStack(0,idx,1,1);
	 }
      currentcan->Update(); 
      }
   }
}

void TaPanam::UpdatePADCStack(Bool_t disptype)
  {
 TCanvas* currentcan;
 UInt_t candiv = (UInt_t) fPADCNum/4; 
   if (gROOT->GetListOfCanvases()->FindObject("PADC") != NULL)
     { 
      currentcan = (TCanvas*)(gROOT->GetListOfCanvases()->FindObject("PADC"));
      currentcan->cd();
      if ( fPrestart == kTRUE )
        {          
         if (candiv <= 3 ) currentcan->Divide(1,candiv);
         if (candiv > 3 && candiv <= 6 ) currentcan->Divide(3,2);
         if (candiv >6 && candiv <= 9 )  currentcan->Divide(3,3);                           
         for ( UInt_t idx = 0; idx < candiv; idx++)
	   {
	    currentcan->cd(idx+1);
            if ( !disptype )
	      {
               fStripStyle->cd();
               currentcan->UseCurrentStyle();
               fAnaMan->GetAnalysis()->DrawADCStack(1,idx,0,0);
	      }
            else
	      {
	       fHistoStyle->cd();
               currentcan->UseCurrentStyle();
               fAnaMan->GetAnalysis()->DrawADCStack(1,idx,2,0);
	      }  
	   }
        currentcan->Update(); 
	}
      if ( fStart == kTRUE )
        {          
         for ( UInt_t idx = 0; idx < candiv; idx++)
	   {
	    currentcan->cd(idx+1);
            if ( !disptype )
	      {
               fStripStyle->cd();
               currentcan->UseCurrentStyle();
               fAnaMan->GetAnalysis()->DrawADCStack(1,idx,0,1);
	      }
            else
	      {
	       fHistoStyle->cd();
               currentcan->UseCurrentStyle();
               fAnaMan->GetAnalysis()->DrawADCStack(1,idx,2,0);
	      }  
	   }
        currentcan->Update(); 
	}
     }

  }

void TaPanam::UpdatePADCRMSStack()
  {
 TCanvas* currentcan;
 UInt_t candiv = (UInt_t) fPADCNum/4; 
   if (gROOT->GetListOfCanvases()->FindObject("PADCRMS") != NULL)
     { 
      currentcan = (TCanvas*)(gROOT->GetListOfCanvases()->FindObject("PADCRMS"));
      currentcan->cd();
      if ( fPrestart == kTRUE )
        {          
         if (candiv <= 3 ) currentcan->Divide(1,candiv);
         if (candiv > 3 && candiv <= 6 ) currentcan->Divide(3,2);
         if (candiv >6 && candiv <= 9 )  currentcan->Divide(3,3);                           
         for ( UInt_t idx = 0; idx < candiv; idx++)
	   {
	    currentcan->cd(idx+1);
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawADCStack(1,idx,1,0);
	   }
        currentcan->Update(); 
	}
      if ( fStart == kTRUE )
        {          
         for ( UInt_t idx = 0; idx < candiv; idx++)
	   {
	    currentcan->cd(idx+1);
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawADCStack(1,idx,1,1);
	   }
        currentcan->Update(); 
	}
     }

  }

void TaPanam::UpdateSADCStack(Bool_t disptype)
{
 TCanvas* currentcan;
 UInt_t candiv = (UInt_t) fSADCNum/4;
 if (gROOT->GetListOfCanvases()->FindObject("SADC") != NULL)
   { 
    currentcan = (TCanvas*)(gROOT->GetListOfCanvases()->FindObject("SADC"));
       cout<<"found canvas named=SADC"<<flush<<endl;
     currentcan->cd();
    if ( fPrestart == kTRUE )
      {          
       currentcan->Divide(1,candiv);
         for ( UInt_t idx = 0; idx < candiv; idx++)
	   {
	    currentcan->cd(idx+1);
            if ( !disptype )
	      {
               fStripStyle->cd();
               currentcan->UseCurrentStyle();
               fAnaMan->GetAnalysis()->DrawADCStack(2,idx,0,0);
	      }
            else
	      {
	       fHistoStyle->cd();
               currentcan->UseCurrentStyle();
               fAnaMan->GetAnalysis()->DrawADCStack(2,idx,2,1);
	      }  
	   }
        currentcan->Update(); 
	}
      if ( fStart == kTRUE )
        {          
         for ( UInt_t idx = 0; idx < candiv; idx++)
	   {
	    currentcan->cd(idx+1);
            if ( !disptype )
	      {
               fStripStyle->cd();
               currentcan->UseCurrentStyle();
               fAnaMan->GetAnalysis()->DrawADCStack(2,idx,0,1);
	      }
            else
	      {
	       fHistoStyle->cd();
               currentcan->UseCurrentStyle();
               fAnaMan->GetAnalysis()->DrawADCStack(2,idx,2,1);
	      }  
	   }
        currentcan->Update(); 
	}
     }
}

void TaPanam::UpdateSADCRMSStack()
{
 TCanvas* currentcan;
 UInt_t candiv = (UInt_t) fSADCNum/4;
 if (gROOT->GetListOfCanvases()->FindObject("SADCRMS") != NULL)
   { 
    currentcan = (TCanvas*)(gROOT->GetListOfCanvases()->FindObject("SADCRMS"));
     currentcan->cd();
    if ( fPrestart == kTRUE )
      {          
       currentcan->Divide(1,candiv);
         for ( UInt_t idx = 0; idx < candiv; idx++)
	   {
	    currentcan->cd(idx+1);
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawADCStack(2,idx,1,0);
	   }
        currentcan->Update(); 
	}
      if ( fStart == kTRUE )
        {          
         for ( UInt_t idx = 0; idx < candiv; idx++)
	   {
	    currentcan->cd(idx+1);
               fStripStyle->cd();
               currentcan->UseCurrentStyle();
               fAnaMan->GetAnalysis()->DrawADCStack(2,idx,1,1);
	   }
        currentcan->Update(); 
	}
     }
}

void TaPanam::UpdateBattStack(Bool_t disptype)
{
 TCanvas* currentcan;
 // UInt_t candiv = fBattNum;
   if (gROOT->GetListOfCanvases()->FindObject("Batt") != NULL)
     { 
      currentcan = (TCanvas*)(gROOT->GetListOfCanvases()->FindObject("Batt"));
      currentcan->cd();
      //      cout<<"found canvas named=Batt"<<flush<<endl;
      if ( fPrestart == kTRUE )
        {
         currentcan->Divide(1,fBattNum);
	 currentcan->cd(1);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(0,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(0,2,0);
	   }  
	 currentcan->cd(2);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(0,5,0);
        currentcan->Update(); 
	}
      if ( fStart == kTRUE )
        {          
	 currentcan->cd(1);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(0,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(0,2,0);
	   }  
	 currentcan->cd(2);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(0,5,1);  
         currentcan->Update(); 
	}
     }
}

void TaPanam::UpdateBCMStack(Bool_t disptype)
{
 TCanvas* currentcan;
 if (gROOT->GetListOfCanvases()->FindObject("BCM") != NULL)
     { 
      currentcan = (TCanvas*)(gROOT->GetListOfCanvases()->FindObject("BCM"));
      currentcan->cd();
      cout<<"found canvas named=BCM"<<flush<<endl;
      cout<<"disptype="<<disptype<<endl;
      if ( fPrestart == kTRUE )
        {
         if (fBCMplot == 1) currentcan->Divide(fBCM12Num + fBCM34Num + fBCMcavNum + fBattNum, fBCMplot);
         if (fBCMplot > 1) currentcan->Divide(fBCM12Num + fBCM34Num+ fBCMcavNum + fBattNum, 2);
         if ( !disptype )
	   {
            fStripStyle->cd();
            if (fBCM12Num) { 
	      currentcan->cd(fBCM12Num);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(1,0,0);
	    } 
            if (fBCM34Num) { 
	      currentcan->cd(fBCM12Num + fBCM34Num);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(2,0,0);
	    } 
            if (fBCMcavNum) { 
	      currentcan->cd(fBCM12Num + fBCM34Num+ fBCMcavNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(3,0,0);
	    } 
            if (fBattNum) { 
	      currentcan->cd(fBCM12Num + fBCM34Num+ fBCMcavNum + fBattNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(0,0,0);
	    } 
            currentcan->Update(); 	   
           }
         else
	   {
	    fHistoStyle->cd();
            if (fBCM12Num) { 
	      currentcan->cd(fBCM12Num);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(1,2,0);
	    } 
            if (fBCM34Num) { 
	      currentcan->cd(fBCM12Num + fBCM34Num);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(2,2,0);
	    } 
            if (fBCMcavNum) { 
	      currentcan->cd(fBCM12Num + fBCM34Num+ fBCMcavNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(3,2,0);
	    } 
            if (fBattNum) { 
	      currentcan->cd(fBCM12Num + fBCM34Num+ fBCMcavNum + fBattNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(0,2,0);
	    } 
            currentcan->Update(); 	   
	   } 
         if (fBCMplot > 1) 
           {
	     //fHistoStyle->cd();
            if (fBCM12Num) { 
	      currentcan->cd(2*fBCM12Num + fBCM34Num + fBCMcavNum + fBattNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(1,5,1);
	    } 
            if (fBCM34Num) { 
	      currentcan->cd(2*fBCM12Num + 2*fBCM34Num + fBCMcavNum + fBattNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(2,5,1);
	    } 
            if (fBCMcavNum) { 
	      currentcan->cd(2*fBCM12Num + 2*fBCM34Num + 2*fBCMcavNum + fBattNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(3,5,1);
	    } 
            if (fBattNum) { 
	      currentcan->cd(2*fBCM12Num + 2*fBCM34Num+ 2*fBCMcavNum + 2*fBattNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(0,5,1);
	    } 
            currentcan->Update(); 	   
	   }
	}
      if ( fStart == kTRUE )
        {          
         if ( !disptype )
	   {
            fStripStyle->cd();
            if (fBCM12Num) { 
	      currentcan->cd(fBCM12Num);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(1,0,0);
	    } 
            if (fBCM34Num) { 
	      currentcan->cd(fBCM12Num + fBCM34Num);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(2,0,0);
	    } 
            if (fBCMcavNum) { 
	      currentcan->cd(fBCM12Num + fBCM34Num+ fBCMcavNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(3,0,0);
	    } 
            if (fBattNum) { 
	      currentcan->cd(fBCM12Num + fBCM34Num+ fBCMcavNum + fBattNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(0,0,0);
	    } 
            currentcan->Update(); 	   
	   }
         else
	   {
	    fHistoStyle->cd();
            if (fBCM12Num) { 
	      currentcan->cd(fBCM12Num);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(1,2,0);
	    } 
            if (fBCM34Num) { 
	      currentcan->cd(fBCM12Num + fBCM34Num);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(2,2,0);
	    } 
            if (fBCMcavNum) { 
	      currentcan->cd(fBCM12Num + fBCM34Num+ fBCMcavNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(3,2,0);
	    } 
            if (fBattNum) { 
	      currentcan->cd(fBCM12Num + fBCM34Num+ fBCMcavNum + fBattNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(0,1,0);
	    } 
            currentcan->Update(); 	   
	   } 
         if (fBCMplot > 1) 
           {
	     //fHistoStyle->cd();
            if (fBCM12Num) { 
	      currentcan->cd(2*fBCM12Num + fBCM34Num + fBCMcavNum + fBattNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(1,5,1);
	    } 
            if (fBCM34Num) { 
	      currentcan->cd(2*fBCM12Num + 2*fBCM34Num + fBCMcavNum + fBattNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(2,5,1);
	    } 
            if (fBCMcavNum) { 
	      currentcan->cd(2*fBCM12Num + 2*fBCM34Num + 2*fBCMcavNum + fBattNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(3,5,1);
	    } 
            if (fBattNum) { 
	      currentcan->cd(2*fBCM12Num + 2*fBCM34Num+ 2*fBCMcavNum + 2*fBattNum);
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(0,5,1);
	    } 
            currentcan->Update(); 	   
	   }
	}
     }
}

void TaPanam::UpdateInjBPMStack(Bool_t disptype)
{
 TCanvas* currentcan;
   if (gROOT->GetListOfCanvases()->FindObject("InjBPM") != NULL)
     { 
      currentcan = (TCanvas*)(gROOT->GetListOfCanvases()->FindObject("InjBPM"));
            cout<<"found canvas named=InjBPM"<<flush<<endl;
      currentcan->cd();
      if ( fPrestart == kTRUE )
        {
         currentcan->Divide(2,2);
	 currentcan->cd(1);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(4,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(4,2,0);
	   }  
	 currentcan->cd(2);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(5,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(5,2,0);
	   }  
	 currentcan->cd(3);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(4,5,0);
         currentcan->Update(); 
	 currentcan->cd(4);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(5,5,0);
         currentcan->Update(); 
	}
      if ( fStart == kTRUE )
        {          
	 currentcan->cd(1);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(4,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(4,2,0);
	   }  
	 currentcan->cd(2);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(5,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(5,2,0);
	   }  
	 currentcan->cd(3);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(4,5,0);
         currentcan->Update(); 
	 currentcan->cd(4);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(5,5,0);
         currentcan->Update(); 
	}
     }
}

void TaPanam::UpdateBPMStack(Bool_t disptype)
{
  //cout<<"entering UpdateBPMStack"<<endl;
 TCanvas* currentcan;
   if (gROOT->GetListOfCanvases()->FindObject("BPM") != NULL)
     { 
      currentcan = (TCanvas*)(gROOT->GetListOfCanvases()->FindObject("BPM"));        
       cout<<"found canvas named=BPM"<<flush<<endl;
      currentcan->cd();
      if ( fPrestart == kTRUE )
        {
         currentcan->Divide(4,2);
	 currentcan->cd(1);         
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(6,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(6,2,0);
	   } 
	 currentcan->cd(2);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(7,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(7,2,0);
	   } 
	 currentcan->cd(3);        
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(6,5,0);
	  currentcan->cd(4);
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(7,5,0);
            currentcan->cd(5);
            if ( !disptype )
              {
               fStripStyle->cd();
               currentcan->UseCurrentStyle();
               fAnaMan->GetAnalysis()->DrawMDStack(8,0,0);
	      }
            else
	     {
	      fHistoStyle->cd();
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(8,2,0);
	     }  
	   currentcan->cd(6);
           if ( !disptype )
	    {
              fStripStyle->cd();
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(9,0,0);
	     }
           else
	    {
	      fHistoStyle->cd();
              currentcan->UseCurrentStyle();
              fAnaMan->GetAnalysis()->DrawMDStack(9,2,0);
	     }  
	 currentcan->cd(7);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(8,5,0);
	 currentcan->cd(8);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(9,5,0);
         currentcan->Update();            
	}
      if ( fStart == kTRUE )
        {          
	 currentcan->cd(1);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            cout<<" start refreshing BPM SC "<<endl;  
            fAnaMan->GetAnalysis()->DrawMDStack(6,0,0);
            cout<<" Refreshed BPM SC "<<endl;  
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
	    fAnaMan->GetAnalysis()->DrawMDStack(6,2,0);
	   }  
	 currentcan->cd(2);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(7,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(7,2,0);
	   }  
	 currentcan->cd(3);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
	 fAnaMan->GetAnalysis()->DrawMDStack(6,5,0);
	 currentcan->cd(4);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(7,5,0);
	 currentcan->cd(5);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(8,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(8,2,0);
	   }  
	 currentcan->cd(6);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(9,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(9,2,0);
	   }  
	 currentcan->cd(7);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(8,5,0);
	 currentcan->cd(8);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(9,5,0);
         currentcan->Update(); 
	}
     }
}

void TaPanam::UpdateBPMcavStack(Bool_t disptype)
{
 TCanvas* currentcan;
   if (gROOT->GetListOfCanvases()->FindObject("BPMcav") != NULL)
     { 
      currentcan = (TCanvas*)(gROOT->GetListOfCanvases()->FindObject("BPMcav"));
            cout<<"found canvas named=BPMcav"<<flush<<endl;
      currentcan->cd();
      if ( fPrestart == kTRUE )
        {
         currentcan->Divide(2,2);
	 currentcan->cd(1);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(13,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(14,2,0);
	   }  
	 currentcan->cd(2);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(13,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(14,2,0);
	   }  
	 currentcan->cd(3);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(13,5,0);
         currentcan->Update(); 
	 currentcan->cd(4);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(14,5,0);
         currentcan->Update(); 
	}
      if ( fStart == kTRUE )
        {          
	 currentcan->cd(1);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(13,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(14,2,0);
	   }  
	 currentcan->cd(2);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(13,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(14,2,0);
	   }  
	 currentcan->cd(3);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(13,5,0);
         currentcan->Update(); 
	 currentcan->cd(4);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(14,5,0);
         currentcan->Update(); 
	}
     }
}

void 
TaPanam::UpdateLumiBackStack()
{
 Bool_t disptype = kFALSE;
 TCanvas* currentcan;
   if (gROOT->GetListOfCanvases()->FindObject("LumiBack") != NULL)
     { 
      currentcan = (TCanvas*)(gROOT->GetListOfCanvases()->FindObject("LumiBack"));
      currentcan->cd();
      cout<<"found canvas named=Lumiback"<<flush<<endl;
      if ( fPrestart == kTRUE )
        {
         currentcan->Divide(2,1);
	 currentcan->cd(1);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(10,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(10,2,0);
	   }           
	 currentcan->cd(2);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(10,5,0);
	 currentcan->cd(3);
         currentcan->UseCurrentStyle();
	 currentcan->cd(4);
         currentcan->UseCurrentStyle();
         currentcan->Update(); 
	}
      if ( fStart == kTRUE )
        {
	 currentcan->cd(1);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(10,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(10,2,0);
	   }           
	 currentcan->cd(2);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(10,5,0);
	 currentcan->cd(3);
         currentcan->UseCurrentStyle();
	 currentcan->cd(4);
         currentcan->UseCurrentStyle();
         currentcan->Update(); 
	}
     }
}

void
TaPanam::UpdateLumiFrontStack()
{
  Bool_t disptype = kFALSE;
 TCanvas* currentcan;
   if (gROOT->GetListOfCanvases()->FindObject("LumiFront") != NULL)
     { 
      currentcan = (TCanvas*)(gROOT->GetListOfCanvases()->FindObject("LumiFront"));
      currentcan->cd();
      //      cout<<"found canvas named=LUMI"<<flush<<endl;
      if ( fPrestart == kTRUE )
        {
         currentcan->Divide(2,1);
	 currentcan->cd(1);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(11,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(11,2,0);
	   }           
	 currentcan->cd(2);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(11,5,0);
	 currentcan->cd(3);
         currentcan->UseCurrentStyle();
	 currentcan->cd(4);
         currentcan->UseCurrentStyle();
         currentcan->Update(); 
	}
      if ( fStart == kTRUE )
        {
	 currentcan->cd(1);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(11,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(11,2,0);
	   }           
	 currentcan->cd(2);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(11,5,0);
	 currentcan->cd(3);
         currentcan->UseCurrentStyle();
	 currentcan->cd(4);
         currentcan->UseCurrentStyle();
         currentcan->Update(); 
	}
     }
}

void 
TaPanam::UpdateDaltonStack()
{
 Bool_t disptype = kFALSE;
 TCanvas* currentcan;
   if (gROOT->GetListOfCanvases()->FindObject("DET") != NULL)
     { 
      currentcan = (TCanvas*)(gROOT->GetListOfCanvases()->FindObject("DET"));
      currentcan->cd();
      //      cout<<"found canvas named=DET"<<flush<<endl;
      if ( fPrestart == kTRUE )
        {
         currentcan->Divide(2,1);
	 currentcan->cd(1);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(12,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(12,2,0);
	   }           
	 currentcan->cd(2);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(12,5,0);
	 currentcan->cd(3);
	 currentcan->cd(4);
         currentcan->Update(); 
	}
      if ( fStart == kTRUE )
        {
	 currentcan->cd(1);
         if ( !disptype )
	   {
            fStripStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(12,0,0);
	   }
         else
	   {
	    fHistoStyle->cd();
            currentcan->UseCurrentStyle();
            fAnaMan->GetAnalysis()->DrawMDStack(12,2,0);
	   }           
	 currentcan->cd(2);
	 fHistoStyle->cd();
         currentcan->UseCurrentStyle();
         fAnaMan->GetAnalysis()->DrawMDStack(12,5,0);
	 currentcan->cd(3);
	 currentcan->cd(4);
         currentcan->Update(); 
	}
     }
}
 

Int_t TaPanam::InitCanvasPads()
{
 SetNumOfPads();
 UpdateCanvasFlock();
 SetStyles();
 UpdateIADCStack(fADCSCorH);
 UpdateIADCRMSStack();
 UpdatePADCStack(fADCSCorH);
 UpdatePADCRMSStack();
 UpdateSADCStack(fADCSCorH);
 UpdateSADCRMSStack();
 UpdateBCMStack(fBCMSCorH);
 UpdateBattStack(fBCMSCorH);
 UpdateInjBPMStack(fBPMSCorH);
 UpdateBPMStack(fBPMSCorH);
 UpdateBPMcavStack(fBPMSCorH);
 UpdateLumiBackStack();
 UpdateLumiFrontStack();
 UpdateDaltonStack();
// commented example to display simple device ( not Multi )
//UpdateCanvas("PADC",fPADCidx,2,fADCSCorH); // 2 for Adev !!!
//   InitADCStacks();
//   UpdateCanvas("BCM",fBCMidx,fBCMplot,fBCMSCorH); 
//   UpdateCanvas("BCMCAV",fBCMCAVidx,fBCMplot,fBCMSCorH); 
//   UpdateCanvas("BPMIN",fBPMINidx,fBPMplot,fBPMSCorH); 
//   UpdateCanvas("BPM",fBPMidx,fBPMplot,fBPMSCorH); 
//   UpdateCanvas("BPMCAV",fBPMCAVidx,fBPMplot,fBPMSCorH);
//   UpdateCanFDBK(); 
//   UpdateCanDITH();
//   UpdateCanvas("LUMI",fLUMIidx,3,kFALSE); 
 return 1;
}

Int_t TaPanam::UpdateCanvasPads()
{
 UpdateIADCStack(fADCSCorH);
 UpdateIADCRMSStack();
 UpdatePADCStack(fADCSCorH);
 UpdatePADCRMSStack();
 UpdateSADCStack(fADCSCorH);
 UpdateSADCRMSStack();
 UpdateBattStack(fBCMSCorH);
 UpdateBCMStack(fBCMSCorH);
 UpdateInjBPMStack(fBPMSCorH);
 UpdateBPMStack(fBPMSCorH);
 UpdateBPMcavStack(fBPMSCorH);
 UpdateLumiBackStack();
 UpdateLumiFrontStack();
 UpdateDaltonStack();
// commented example to display simple device ( not Multi )

//  UpdateCanvas("TimeSignal",fTimeidx,0,fTSCorH);  
//  UpdateADCStacks();
//  UpdateCanvas("IADC",fPADCidx,2,fADCSCorH);
//  UpdateCanvas("PADC",fPADCidx,2,fADCSCorH);
//  UpdateCanvas("SADC",fPADCidx,2,fADCSCorH);
//  UpdateCanvas("BCM",fBCMidx,fBPMplot,fBCMSCorH); 
//  UpdateCanvas("BPM",fBPMidx,fBPMplot,fBPMSCorH); 
//  UpdateCanFDBK(); 
//  UpdateCanDITH(); 
//  UpdateCanvas("LUMI",fLUMIidx,3,kFALSE); 
  return 1;
}

void TaPanam::UpdateEmCanvas()
{
//  if (fEmCanDevPlotType) fEmCanToUpdate = kTRUE;
//  if (fEmCanToUpdate) 
//    {
//     fEmCan->GetCanvas()->cd();
//     cout<<" cd on Embedded canvas"<<endl;
//     //    (fAnaMan->GetAnalysis()->GetMonitorDevice(fEmCanDevName))->GetPlot(fEmCanDevPlotName,fEmCanDevPlotType)->Draw();
//     fEmCan->GetCanvas()->Update();
//    }
}

Int_t TaPanam::ClearCanvas()
{
  if ((fIADCCan!=NULL) && gROOT->GetListOfCanvases()->FindObject("IADC")) 
     CanvasSafeDelete(fIADCCan,"IADC");
  if ((fIADCRMSCan!=NULL) && gROOT->GetListOfCanvases()->FindObject("IADCRMS")) 
     CanvasSafeDelete(fIADCRMSCan,"IADCRMS");
  if ((fPADCCan!=NULL) && gROOT->GetListOfCanvases()->FindObject("PADC")) 
     CanvasSafeDelete(fPADCCan,"PADC");
  if ((fPADCRMSCan!=NULL) && gROOT->GetListOfCanvases()->FindObject("PADCRMS")) 
     CanvasSafeDelete(fPADCRMSCan,"PADCRMS");
  if ((fSADCCan!=NULL) && gROOT->GetListOfCanvases()->FindObject("SADC")) 
     CanvasSafeDelete(fSADCCan,"SADC");
  if ((fSADCRMSCan!=NULL) && gROOT->GetListOfCanvases()->FindObject("SADCRMS")) 
     CanvasSafeDelete(fSADCRMSCan,"SADCRMS");
   if ((fBattCan!=NULL) && gROOT->GetListOfCanvases()->FindObject("Batt")) 
     CanvasSafeDelete(fBattCan,"Batt");
  if ((fInjBCMCan!=NULL) && gROOT->GetListOfCanvases()->FindObject("InjBCM")) 
     CanvasSafeDelete(fInjBCMCan,"InjBCM");
  if ((fBCMCan!=NULL) && gROOT->GetListOfCanvases()->FindObject("BCM")) 
     CanvasSafeDelete(fBCMCan,"BCM");
  if ((fInjBPMCan!=NULL) && gROOT->GetListOfCanvases()->FindObject("InjBPM")) 
     CanvasSafeDelete(fInjBPMCan,"InjBPM");
  if ((fBPMCan!=NULL) && gROOT->GetListOfCanvases()->FindObject("BPM")) 
     CanvasSafeDelete(fBPMCan,"BPM");
  if ((fBPMcavCan!=NULL) && gROOT->GetListOfCanvases()->FindObject("BPMcav")) 
     CanvasSafeDelete(fBPMcavCan,"BPMcav");
   if ((fLumiFrontCan!=NULL) && gROOT->GetListOfCanvases()->FindObject("LumiFront")) 
     CanvasSafeDelete(fLumiFrontCan,"LumiFront");
   if ((fLumiBackCan!=NULL) && gROOT->GetListOfCanvases()->FindObject("LumiBack")) 
     CanvasSafeDelete(fLumiBackCan,"LumiBack");
  if ((fDETCan!=NULL) && gROOT->GetListOfCanvases()->FindObject("DET")) 
     CanvasSafeDelete(fDETCan,"DET");
  return 1;
}
Int_t TaPanam::ClearListBox()
{  
  Int_t j=1;
  //fHistoListBox->Cleanup();
   while (fHistoListBox->GetSelection(j) != kFALSE )
     {
       j++;
     }
     fHistoListBox->RemoveEntries(0,j);
     fHistoListBox->Layout();
     MapSubwindows();
     Resize(950,550);
     MapWindow();
 return 1;
}

Int_t TaPanam::ClearListOfDevice()
{  
  fDeviceList.clear();
  fADeviceList.clear();
  fAllDeviceList.clear();
 return 1;
}

void TaPanam::InitGUIFrames(){
  ////////////////     Menu bar  //////////////////////////////////////////

  //   define layout parameters for menu bar  
  fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
				     0, 0, 1, 1);  
  fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
     // File menu
  fMenuFile = new TGPopupMenu(fClient->GetRoot());
//   fMenuFile->AddEntry("&Open Root file...", M_FILE_OPEN);
//   fMenuFile->AddEntry("Edit Pan &MySQL Data Base",M_FILE_EDITDB);
//   fMenuFile->AddSeparator();
//   fMenuFile->AddEntry("&Save all Canvas as ps file", M_FILE_SAVE);
//   fMenuFile->AddEntry("&Save current as eps file", M_FILE_SAVE1);
//   fMenuFile->AddEntry("&Save current as gif file", M_FILE_SAVE2);
//   fMenuFile->AddEntry("&Print all Canvas",M_FILE_PRINT);
//   fMenuFile->AddEntry("&Print current Canvas",M_FILE_PRINT1);
  fMenuFile->AddSeparator();
  fMenuFile->AddEntry("E&xit", M_FILE_EXIT);
      // Monitoring menu
  fMenuAna =  new TGPopupMenu(fClient->GetRoot());
  fMenuAna->AddEntry(" ADC Status      ", M_ANA_ADC);
  fMenuAna->AddEntry(" Beam Diagnostic ", M_ANA_BEAM);
//   fMenuAna->AddEntry(" Injector Crate  ", M_ANA_I);
//   fMenuAna->AddEntry(" parity Crate    ", M_ANA_P);
//   fMenuAna->AddEntry(" HRS Crates      ", M_ANA_S);
//   fMenuAna->AddEntry(" SAGDH prod      ", M_ANA_SAGDH);
//   fMenuAna->AddEntry(" HAPPEX prod     ", M_ANA_HAPPEX);
//   fMenuAna->AddEntry(" manual          ", M_ANA_MAN);

  fMenuMon = new TGPopupMenu(fClient->GetRoot());
  fMenuMon->AddEntry("Prestart ", M_MON_PRESTART);
  fMenuMon->AddEntry("Start ", M_MON_START);
  fMenuMon->DisableEntry(M_MON_START);
  fMenuMon->AddEntry("Stop", M_MON_STOP);
  fMenuMon->DisableEntry(M_MON_STOP);
  fMenuMon->AddEntry("End ", M_MON_END);
  fMenuMon->DisableEntry(M_MON_END);
  fMenuMon->AddSeparator();
  fMenuMon->AddEntry("Reset all plots",M_MON_HISTOALLRAZ);
  fMenuMon->DisableEntry(M_MON_HISTOALLRAZ);
       // Analysis Launch Menu
//   fMenuAna->AddEntry("Prompt Analysis", M_ANA_PROMPT);
//   fMenuMon->DisableEntry(M_ANA_PROMPT);
//   fMenuAna->AddEntry("Regression Analysis", M_ANA_REGRESS);
//   fMenuMon->DisableEntry(M_ANA_REGRESS);
//   fMenuAna->AddEntry("Beam Modulation analysis", M_ANA_BEAMMOD);   
//  fMenuMon->DisableEntry(M_ANA_BEAMMOD);
     // Menu button messages are handled by the main frame (i.e. "this")
     // ProcessMessage() method.
  fMenuFile->Associate(this);
  fMenuMon->Associate(this);
  fMenuAna->Associate(this);

  fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
  fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
  fMenuBar->AddPopup("&Configuration", fMenuAna, fMenuBarItemLayout);
  fMenuBar->AddPopup("&Monitoring", fMenuMon, fMenuBarItemLayout);
  AddFrame(fMenuBar, fMenuBarLayout);
  //////////////////////  TITLE FRAME   //////////////////////////////////////////
   
  //  define layout parameters
   fL1  = new TGLayoutHints(kLHintsTop | kLHintsLeft,0,0,0,0);
   fL2  = new TGLayoutHints(kLHintsCenterX ,20,10,20,10);
   fL5  = new TGLayoutHints(kLHintsTop | kLHintsLeft,2,2,2,2);
   fL3  = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandY  ,5,5,2,2);
   fL4  = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX |
			   kLHintsExpandY, 2, 2, 2, 2);
   fL6  = new TGLayoutHints(kLHintsCenterX ,20,10,20,10);
   fL7  = new TGLayoutHints(kLHintsCenterX,0,0,0,0);
   fL8  = new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,2,2,2,2);
   fL9  =  new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,2, 2, 2, 2);
   //LOGO+TITLE
   fMainTitle = new TGCompositeFrame(this, 50, 100,kHorizontalFrame | kLHintsExpandX );
   //fLogoButton = new TGPictureButton(fMainTitle,gClient->GetPicture("HAPPEX2.xpm"));    
   FontStruct_t labelfont = gClient->GetFontByName("-*-times-bold-r-*-*-24-*-*-*-*-*-*-*");
   GCValues_t   gval;
   gval.fMask = kGCForeground | kGCFont;
   gval.fFont = gVirtualX->GetFontHandle(labelfont);
   gClient->GetColorByName("orange", gval.fForeground);
   GContext_t fTextGC = gVirtualX->CreateGC(gClient->GetRoot()->GetId(), &gval);
   fTitle = new TGLabel(fMainTitle,"     PANAM    ",fTextGC,labelfont);
   fMainTitle->AddFrame(fTitle,fL2);
   AddFrame(fMainTitle,fL1);
   //////////////////  MAIN FRAME   //////////////////////////////////////////////

     // Main frame
   fMainFrame = new TGCompositeFrame(this,100,100,kHorizontalFrame | kRaisedFrame);
   AddFrame(fMainFrame,fL4);

   /////////////////////  UTILITY FRAME ///////////////////////////////////////////////
   fUtilFrame = new TGCompositeFrame(fMainFrame,100,100,kVerticalFrame);
   fMainFrame->AddFrame(fUtilFrame,fL3);
   // Button box
   fButtonFrame = new TGCompositeFrame(fUtilFrame,100,100,kHorizontalFrame);
    
   fConfigBuff = new TGTextBuffer(6); 
   fConfigBuff->AddText(0,"NONE");
   fConfigText = new TGTextEntry(fButtonFrame,fConfigBuff,200);
   fConfigText->Associate(this);
   
   fCodaRunBuff = new TGTextBuffer(4); 
   fCodaRunBuff->AddText(0,"    ");
   fCodaRunText = new TGTextEntry(fButtonFrame,fCodaRunBuff,201);
   
   //fRefreshBuff = new TGTextBuffer(6); 
   //fRefreshBuff->AddText(0,"2");
   //fRefreshText = new TGTextEntry(fButtonFrame,fRefreshBuff,202);
   
   fButtonFrame->AddFrame(fTextLab = new TGLabel(fButtonFrame, new TGHotString("DAQ Config ")));
   fButtonFrame->AddFrame(fConfigText);
   fButtonFrame->AddFrame(fTextLab = new TGLabel(fButtonFrame, new TGHotString("CODA Run  ")));
   fButtonFrame->AddFrame(fCodaRunText);
//    fButtonFrame->AddFrame(fTextLab = new TGLabel(fButtonFrame, new TGHotString("Refresh time ")));
//    fButtonFrame->AddFrame(fRefreshText);

   
  // Some buttons
  //   fButton1 = new TGTextButton(fButtonFrame,"Plot on new Canvas",1);
  //   fButton1->Associate(this);
  //   fButtonFrame->AddFrame(fButton1,fL5);
  //  fButton2 = new TGTextButton(fButtonFrame,"sleep",2);
  //  fButton2->Associate(this);
  //  fButtonFrame->AddFrame(fButton2,fL5);
  //  fButton3 = new TGTextButton(fButtonFrame,"",3);
  //  fButton3->Associate(this);
  //  fButtonFrame->AddFrame(fButton3,fL5);
   fUtilFrame->AddFrame(fButtonFrame,fL5);

  fHistoTreeView = new TGCanvas(fUtilFrame, 300, 100,kSunkenFrame | kDoubleBorder);
  fHistoListBox = new TGListBox(fHistoTreeView->GetViewPort(),10, 10, kHorizontalFrame);
  fHistoListBox->Associate(this);
  //fHistoListBox->AddEntry("List of plots initialized ... ",1);
  fHistoTreeView->SetContainer(fHistoListBox);
  fUtilFrame->AddFrame(fHistoTreeView,fL3);

  /////////////////  Draw folder with canvas and checkbox options ////  
  fTab = new TGTab(fMainFrame,300,300);
  fTab->Associate(this);
 
  // set time/helicity signal displaying pairsynch and helicity stripchart
  //  fHistoFrame = fTab->AddTab("Time Signals");
  //  fTab->Associate(this);
//   fTimeFrame    = new TGCompositeFrame(fHistoFrame, 60, 60, kVerticalFrame);    
//   fTimegFrame   = new TGGroupFrame(fTimeFrame,new TGString(" Signals "));
//   fTimeCheck[0] = new TGCheckButton(fTimegFrame, " Multipletsynch ", 10);
//   fTimeCheck[1] = new TGCheckButton(fTimegFrame, " Pairsynch     ", 11);
//   fTimeCheck[2] = new TGCheckButton(fTimegFrame, " Helicitity    ", 12);
//   fTimeCheck[3] = new TGCheckButton(fTimegFrame, " Timeslot      ", 13);
//   fTimeCheck[4] = new TGCheckButton(fTimegFrame, " v2fclock0      ", 14);
//   fTimeCheck[5] = new TGCheckButton(fTimegFrame, " v2fclock1      ", 15);
//   fTimeCheck[6] = new TGCheckButton(fTimegFrame, " v2fclock2      ", 16);
//   fTimeCheck[0]->Associate(this);
//   fTimeCheck[1]->Associate(this);
//   fTimeCheck[2]->Associate(this);
//   fTimeCheck[3]->Associate(this);
//   fTimeCheck[4]->Associate(this);
//   fTimeCheck[5]->Associate(this);
//   fTimeCheck[6]->Associate(this);
//   for (Int_t i =0 ; i< 7 ;i++) fTimegFrame->AddFrame(fTimeCheck[i],fL4);
//   fTOptgFrame =   new TGGroupFrame(fTimeFrame,new TGString(" Display Options ")); 
//   fTOptCheck[0] = new TGCheckButton(fTOptgFrame, " StripChart     ",17);
//   fTOptCheck[1] = new TGCheckButton(fTOptgFrame, " Histogram      ",18);
//   fTOptCheck[0]->Associate(this);
//   fTOptCheck[1]->Associate(this);
//   //set default 
//   fTOptCheck[0]->SetState(kButtonDown); 
//   for (Int_t i =0 ; i< 2 ;i++) fTOptgFrame->AddFrame(fTOptCheck[i],fL4);
//   fTimeFrame->AddFrame(fTimegFrame,fL2);
//   fTimeFrame->AddFrame(fTOptgFrame,fL2);
//   fHistoFrame->AddFrame(fTimeFrame,fL2);
//   //  cout<<"time..."<<endl<<flush;  


  fHistoFrame = fTab->AddTab("ADCs");
  fTab->Associate(this);
  fADCFrame = new TGCompositeFrame(fHistoFrame, 60, 60, kVerticalFrame);
  fADCSubFrame = new TGCompositeFrame(fADCFrame, 60, 60, kHorizontalFrame);
  fIADCCrate = new TGGroupFrame(fADCSubFrame,new TGString(" Injector DAQ "));  
  fADCFrame0 = new TGCompositeFrame(fIADCCrate, 60, 60, kVerticalFrame);
  fIADCCheck[0] = new TGCheckButton(fADCFrame0, " ADC8  ",20); 
  fIADCCheck[1] = new TGCheckButton(fADCFrame0, " ADC6  ",21); 
  fIADCCheck[2] = new TGCheckButton(fADCFrame0, " ADC16 ",22);  
  fIADCCheck[0]->Associate(this);
  fIADCCheck[1]->Associate(this);
  fIADCCheck[2]->Associate(this);
  for (Int_t i =0 ; i<3  ;i++) fADCFrame0->AddFrame(fIADCCheck[i],fL4);
  fIADCCrate->AddFrame(fADCFrame0,fL2);
  fPADCCrate = new TGGroupFrame(fADCSubFrame,new TGString(" Parity DAQ "));  
  fADCFrame1 = new TGCompositeFrame(fPADCCrate, 60, 60, kVerticalFrame);
  fADCCheck[0] = new TGCheckButton(fADCFrame1, " ADC2   ",23); 
  fADCCheck[1] = new TGCheckButton(fADCFrame1, " ADC26   ",24); 
  fADCCheck[2] = new TGCheckButton(fADCFrame1, " ADC23   ",25); 
  fADCCheck[3] = new TGCheckButton(fADCFrame1, " ADC22   ",26); 
  fADCCheck[4] = new TGCheckButton(fADCFrame1, " ADC15   ",27); 
  fADCCheck[5] = new TGCheckButton(fADCFrame1, " ADC13   ",28); 
  fADCCheck[6] = new TGCheckButton(fADCFrame1, " ADC7   ",29); 
  fADCCheck[7] = new TGCheckButton(fADCFrame1, " ADC12   ",30); 
  fADCCheck[8] = new TGCheckButton(fADCFrame1, " ADC14   ",31);  
  fADCCheck[0]->Associate(this);
  fADCCheck[1]->Associate(this);
  fADCCheck[2]->Associate(this);
  fADCCheck[3]->Associate(this);
  fADCCheck[4]->Associate(this);
  fADCCheck[5]->Associate(this);
  fADCCheck[6]->Associate(this);
  fADCCheck[7]->Associate(this);
  fADCCheck[8]->Associate(this);
  for (Int_t i =0 ; i<9  ;i++) fADCFrame1->AddFrame(fADCCheck[i],fL4);
  fPADCCrate->AddFrame(fADCFrame1,fL2);
  fSADCCrate = new TGGroupFrame(fADCSubFrame,new TGString(" HRS DAQs "));  
  fADCFrame2 = new TGCompositeFrame(fSADCCrate, 60, 60, kVerticalFrame);
  fSADCCheck[0] = new TGCheckButton(fADCFrame2, " ADC29  (Right HRS)",32); 
  fSADCCheck[1] = new TGCheckButton(fADCFrame2, " ADC17  (Left HRS",33); 
  fSADCCheck[0]->Associate(this);
  fSADCCheck[1]->Associate(this);
  for (Int_t i =0 ; i<2  ;i++) fADCFrame2->AddFrame(fSADCCheck[i],fL4);
  fSADCCrate->AddFrame(fADCFrame2,fL2);

  fADCOptgFrame   = new TGGroupFrame(fADCFrame,new TGString(" Display Options ")); 
  fADCOptCheck[0] = new TGCheckButton(fADCOptgFrame, " Raw Data        ",51);
  fADCOptCheck[1] = new TGCheckButton(fADCOptgFrame, " Calibrated Data",52);
  fADCOptCheck[2] = new TGCheckButton(fADCOptgFrame, " Data stripchart    ",53);
  fADCOptCheck[3] = new TGCheckButton(fADCOptgFrame, " Data Histos      ",54);
  fADCOptCheck[0]->Associate(this);
  fADCOptCheck[1]->Associate(this);
  fADCOptCheck[2]->Associate(this);
  fADCOptCheck[3]->Associate(this);
  //set default
  fADCOptCheck[1]->SetState(kButtonDown); 
  fADCOptCheck[2]->SetState(kButtonDown); 
  for (Int_t i =0 ; i< 4 ;i++) fADCOptgFrame->AddFrame(fADCOptCheck[i],fL8);
  fADCSubFrame->AddFrame(fIADCCrate);  
  fADCSubFrame->AddFrame(fPADCCrate);  
  fADCSubFrame->AddFrame(fSADCCrate);    
  fADCFrame->AddFrame(fADCSubFrame);  
  fADCFrame->AddFrame(fADCOptgFrame);  
  fHistoFrame->AddFrame(fADCFrame,fL2);
  //  cout<<"adcs..."<<endl<<flush;  
  
  fHistoFrame = fTab->AddTab("BEAM Current");
  fTab->Associate(this);
  fCurrentFrame    = new TGCompositeFrame(fHistoFrame, 60, 60, kVerticalFrame);
  fCurrentSubFrame = new TGCompositeFrame(fCurrentFrame, 60, 60, kHorizontalFrame);
  fInjBCMgFrame    = new TGGroupFrame(fCurrentSubFrame,new TGString(" Injector "));  
  fInjBCMFrame     = new TGCompositeFrame(fInjBCMgFrame, 60, 60, kVerticalFrame);  
  fInjBCMCheck[0]  = new TGCheckButton(fInjBCMFrame, " bcm ",60);
  fInjBCMCheck[1]  = new TGCheckButton(fInjBCMFrame, " Battery ",61);
  fInjBCMCheck[0]->Associate(this);
  fInjBCMCheck[1]->Associate(this);
  for (Int_t i = 0 ; i<2  ;i++) fInjBCMFrame->AddFrame(fInjBCMCheck[i],fL3);
  fInjBCMgFrame->AddFrame(fInjBCMFrame,fL3);
  fCurrentSubFrame->AddFrame(fInjBCMgFrame);
  fBCMgFrame     = new TGGroupFrame(fCurrentSubFrame,new TGString(" HALL A "));  
  fBCMFrame      = new TGCompositeFrame(fBCMgFrame, 60, 60, kVerticalFrame);
  fBCMCheck[0]   = new TGCheckButton(fBCMFrame, " BCM1 Low G ",42);
  fBCMCheck[1]   = new TGCheckButton(fBCMFrame, " BCM2 Low G ",43);
  fBCMCheck[2]   = new TGCheckButton(fBCMFrame, " BCM1 High G ",44);
  fBCMCheck[3]   = new TGCheckButton(fBCMFrame, " BCM2 High G ",45);
  fBCMCheck[4]   = new TGCheckButton(fBCMFrame, " Unser       ",46);
  fBCMCheck[5]   = new TGCheckButton(fBCMFrame, " cavity1     ",47);
  fBCMCheck[6]   = new TGCheckButton(fBCMFrame, " cavity2     ",48);
  fBCMCheck[7]   = new TGCheckButton(fBCMFrame, " cavity3     ",49);
  fBCMCheck[8]   = new TGCheckButton(fBCMFrame, " CH Batt1    ",50);
  fBCMCheck[9]   = new TGCheckButton(fBCMFrame, " CH Batt2    ",51);
  fBCMCheck[10]  = new TGCheckButton(fBCMFrame, " RHRS Batt   ",52);
  fBCMCheck[11]  = new TGCheckButton(fBCMFrame, " LHRS Batt   ",53);
  fBCMCheck[0]->Associate(this);
  fBCMCheck[1]->Associate(this);
  fBCMCheck[2]->Associate(this);
  fBCMCheck[3]->Associate(this);
  fBCMCheck[4]->Associate(this);
  fBCMCheck[5]->Associate(this);
  fBCMCheck[6]->Associate(this);
  fBCMCheck[7]->Associate(this);
  for (Int_t i =0 ; i<12  ;i++) fBCMFrame->AddFrame(fBCMCheck[i],fL3);
  fBCMgFrame->AddFrame(fBCMFrame,fL3);
  fCurrentSubFrame->AddFrame(fBCMgFrame);
  fBCMDispgFrame    = new TGGroupFrame(fCurrentFrame,new TGString(" Options "));  
  fBCMDispFrame     = new TGCompositeFrame(fBCMDispgFrame, 60, 60, kVerticalFrame);
  fBCMDispCheck[0]  = new TGCheckButton(fBCMDispFrame, " Data        ",54);
  fBCMDispCheck[1]  = new TGCheckButton(fBCMDispFrame, " Asymmetries ",55);
  fBCMDispCheck[2]  = new TGCheckButton(fBCMDispFrame, " All         ",56);
  fBCMDispCheck[0]->Associate(this);
  fBCMDispCheck[1]->Associate(this);
  fBCMDispCheck[2]->Associate(this);
  fBCMDispCheck[1]->SetState(kButtonDown);
  for (Int_t i =0 ; i<3  ;i++) fBCMDispFrame->AddFrame(fBCMDispCheck[i],fL4);
  fBCMDispgFrame->AddFrame(fBCMDispFrame,fL3);  
  fBCMOptFrame     = new TGCompositeFrame(fBCMDispgFrame, 60, 60, kVerticalFrame);
  fBCMOptCheck[0]  = new TGCheckButton(fBCMOptFrame, " Data stripchart ",57);
  fBCMOptCheck[1]  = new TGCheckButton(fBCMOptFrame, " Data histograms",58);
  fBCMOptCheck[0]->Associate(this);
  fBCMOptCheck[1]->Associate(this);
  fBCMOptCheck[0]->SetState(kButtonDown);
  for (Int_t i =0 ; i<2  ;i++) fBCMOptFrame->AddFrame(fBCMOptCheck[i],fL4);
  fBCMDispgFrame->AddFrame(fBCMOptFrame,fL3);
  fCurrentFrame->AddFrame(fCurrentSubFrame ,fL2);      
  fCurrentFrame->AddFrame(fBCMDispgFrame ,fL2);      
  fHistoFrame->AddFrame(fCurrentFrame,fL2);
  cout<<"BCMs..."<<endl<<flush;  
  
  fHistoFrame = fTab->AddTab("Beam Position");
  fTab->Associate(this);


  fPositionFrame  = new TGCompositeFrame(fHistoFrame, 60, 60, kVerticalFrame);
  fBPMDevFrame  = new TGCompositeFrame(fPositionFrame , 60, 60, kHorizontalFrame);
   
  fInjBPMgFrame    = new TGGroupFrame(fBPMDevFrame,new TGString(" Injector "));
  fInjBPMFrame     = new TGCompositeFrame(fInjBPMgFrame, 60, 60, kVerticalFrame);
  fInjBPMCheck[0]  = new TGCheckButton(fInjBPMFrame, " BPM1I02  100KeV",70);
  fInjBPMCheck[1]  = new TGCheckButton(fInjBPMFrame, " BPM1I04  100KeV",71);
  fInjBPMCheck[2]  = new TGCheckButton(fInjBPMFrame, " BPM1I06  100KeV",72);
  fInjBPMCheck[3]  = new TGCheckButton(fInjBPMFrame, " BPM0I02  100KeV",73);
  fInjBPMCheck[4]  = new TGCheckButton(fInjBPMFrame, " BPM0I02A 100KeV",74);
  fInjBPMCheck[5]  = new TGCheckButton(fInjBPMFrame, " BPM0L01 5MeV",75);
  fInjBPMCheck[6]  = new TGCheckButton(fInjBPMFrame, " BPM0L02 5MeV",76);
  fInjBPMCheck[7]  = new TGCheckButton(fInjBPMFrame, " BPM0L03 5MeV",77);
  fInjBPMCheck[8]  = new TGCheckButton(fInjBPMFrame, " BPM0L04 5MeV",78);
  fInjBPMCheck[9]  = new TGCheckButton(fInjBPMFrame, " BPM0L06 5MeV",79);
  fInjBPMCheck[0]->Associate(this);
  fInjBPMCheck[1]->Associate(this);
  fInjBPMCheck[2]->Associate(this);
  fInjBPMCheck[3]->Associate(this);
  fInjBPMCheck[4]->Associate(this);
  fInjBPMCheck[5]->Associate(this);
  fInjBPMCheck[6]->Associate(this);
  fInjBPMCheck[7]->Associate(this);
  fInjBPMCheck[8]->Associate(this);
  fInjBPMCheck[9]->Associate(this);
  for (Int_t i =0 ; i<10  ;i++) fInjBPMFrame->AddFrame(fInjBPMCheck[i],fL3);
  fInjBPMgFrame->AddFrame(fInjBPMFrame,fL3);
  //  cout<<"BPMIN..."<<endl<<flush;  

  fBPMgFrame = new TGGroupFrame(fBPMDevFrame,new TGString(" Hall A  "));
  
  fBPMFrame     = new TGCompositeFrame(fBPMgFrame, 60, 60, kVerticalFrame);
  fBPMCheck[0]  = new TGCheckButton(fBPMFrame, " BPM12 ",90);
  fBPMCheck[1]  = new TGCheckButton(fBPMFrame, " BPM10 ",91);
  fBPMCheck[2]  = new TGCheckButton(fBPMFrame, " BPM8  ",92);
  fBPMCheck[3]  = new TGCheckButton(fBPMFrame, " BPM4A ",93);
  fBPMCheck[4]  = new TGCheckButton(fBPMFrame, " BPM4B ",94);
  fBPMCheck[5]  = new TGCheckButton(fBPMFrame, " Cavity1",95);
  fBPMCheck[6]  = new TGCheckButton(fBPMFrame, " Cavity2",96);
  fBPMCheck[7]  = new TGCheckButton(fBPMFrame, " Cavity3",97);
  fBPMCheck[0]->Associate(this);
  fBPMCheck[1]->Associate(this);
  fBPMCheck[2]->Associate(this);
  fBPMCheck[3]->Associate(this);
  fBPMCheck[4]->Associate(this);
  fBPMCheck[5]->Associate(this);
  fBPMCheck[6]->Associate(this);
  fBPMCheck[7]->Associate(this);
  for (Int_t i =0 ; i<8  ;i++) fBPMFrame->AddFrame(fBPMCheck[i],fL3);
  fBPMgFrame->AddFrame(fBPMFrame,fL3);

  fBPMDispgFrame = new TGGroupFrame(fPositionFrame,new TGString("Options  "));
  fBPMDFrame  = new TGCompositeFrame(fBPMDispgFrame, 60, 60, kHorizontalFrame); 
  fBPMDispFrame = new TGCompositeFrame(fBPMDFrame, 60, 60, kVerticalFrame);
  fBPMDispCheck[0]  = new TGCheckButton(fBPMDispFrame, " Data  ",110);
  fBPMDispCheck[1]  = new TGCheckButton(fBPMDispFrame, " Differences  ",111);
  fBPMDispCheck[2]  = new TGCheckButton(fBPMDispFrame, " all  ",112);
  fBPMDispCheck[0]->Associate(this);
  fBPMDispCheck[1]->Associate(this);
  fBPMDispCheck[2]->Associate(this);
  fBPMDispCheck[1]->SetState(kButtonDown);
  for (Int_t i =0 ; i<3  ;i++) fBPMDispFrame->AddFrame(fBPMDispCheck[i],fL3);
  fBPMDFrame->AddFrame(fBPMDispFrame,fL2);

  fBPMOptFrame  = new TGCompositeFrame(fBPMDFrame, 60, 60, kVerticalFrame);
  fBPMOptCheck[0]  = new TGCheckButton(fBPMOptFrame, " Stripchart Data ",113);
  fBPMOptCheck[1]  = new TGCheckButton(fBPMOptFrame, " Histogram  Data ",114);
  fBPMOptCheck[0]->Associate(this);
  fBPMOptCheck[1]->Associate(this);
  fBPMOptCheck[1]->SetState(kButtonDown);  
  for (Int_t i =0 ; i<2  ;i++) fBPMOptFrame->AddFrame(fBPMOptCheck[i],fL3);
  fBPMDFrame->AddFrame(fBPMOptFrame,fL2);
  fBPMDispgFrame-> AddFrame(fBPMDFrame,fL2); 
  fBPMDevFrame->AddFrame(fInjBPMgFrame,fL2); 
  fBPMDevFrame->AddFrame(fBPMgFrame,fL2); 
  fPositionFrame->AddFrame(fBPMDevFrame,fL2);      
  fPositionFrame->AddFrame(fBPMDispgFrame,fL2);      
  fHistoFrame->AddFrame(fPositionFrame,fL2);
  cout<<"BPM..."<<endl<<flush;  
  
    cout<<"Others..."<<endl<<flush;  
  
  fHistoFrame = fTab->AddTab("Detectors");
  fTab->Associate(this);
 
  fDetFrame     = new TGCompositeFrame(fHistoFrame, 60, 60, kHorizontalFrame);
  fLumigFrame    = new TGGroupFrame(fDetFrame,new TGString(" Luminosity "));
  fLumiFrame     = new TGCompositeFrame(fLumigFrame, 60, 60, kVerticalFrame);
  fLumiCheck[0]  = new TGCheckButton(fLumiFrame, " LUMI1         ",114);    
  fLumiCheck[1]  = new TGCheckButton(fLumiFrame, " LUMI2         ",115);    
  fLumiCheck[2]  = new TGCheckButton(fLumiFrame, " LUMI3         ",116);    
  fLumiCheck[3]  = new TGCheckButton(fLumiFrame, " LUMI4         ",117); 
  fLumiCheck[4]  = new TGCheckButton(fLumiFrame, " LUMI5 (front) ",118); 
  fLumiCheck[5]  = new TGCheckButton(fLumiFrame, " LUMI6 (front) ",119); 
  for (Int_t i =0 ; i<6  ;i++) 
    {
     fLumiCheck[i]->Associate(this);
     fLumiFrame ->AddFrame(fLumiCheck[i],fL2);
    }
  fLumigFrame->AddFrame(fLumiFrame ,fL2);
  
  fDalgFrame    = new TGGroupFrame(fDetFrame,new TGString(" The Daltons "));
  fDalFrame     = new TGCompositeFrame(fDalgFrame, 60, 60, kVerticalFrame);
  fDalCheck[0]  = new TGCheckButton(fDalFrame, "-Joe-     DET1 RHRS H ",110);    
  fDalCheck[1]  = new TGCheckButton(fDalFrame, "-Jack-    DET2 RHRS V ",111);    
  fDalCheck[2]  = new TGCheckButton(fDalFrame, "-William- DET3 LHRS H ",112);    
  fDalCheck[3]  = new TGCheckButton(fDalFrame, "-Averell- DET4 LHRS V ",113); 
  for (Int_t i =0 ; i<4  ;i++) 
    {
     fDalCheck[i]->Associate(this);
     fDalFrame ->AddFrame(fDalCheck[i],fL2);
    }
  fDalgFrame->AddFrame(fDalFrame ,fL2);
 
  fScangFrame    = new TGGroupFrame(fDetFrame,new TGString(" Scanners "));
  fScanFrame     = new TGCompositeFrame(fScangFrame, 60, 60, kVerticalFrame);
  fScanCheck[0]  = new TGCheckButton(fScanFrame, "RHRS scanner ",110);    
  fScanCheck[1]  = new TGCheckButton(fScanFrame, "LHRS scanner ",111);    
  for (Int_t i =0 ; i<2  ;i++) 
    {
     fScanCheck[i]->Associate(this);
     fScanFrame ->AddFrame(fScanCheck[i],fL2);
    }
  fScangFrame->AddFrame(fScanFrame ,fL2);  
  fDetFrame->AddFrame(fLumigFrame,fL2); 
  fDetFrame->AddFrame(fDalgFrame,fL2); 
  fDetFrame->AddFrame(fScangFrame,fL2); 
  fHistoFrame->AddFrame(fDetFrame,fL2);
  cout<<" Detectors ..."<<endl<<flush;  
       
  //  fHistoFrame = fTab->AddTab("Beam Control");
  //  fTab->Associate(this);
//     fControlFrame = new TGCompositeFrame(fHistoFrame, 60, 60, kVerticalFrame);
//     fFeedbackFrame  = new TGCompositeFrame(fControlFrame, 60, 60, kHorizontalFrame);
//     fFCurgFrame   = new TGGroupFrame(fFeedbackFrame,new TGString("Current Feedback"));    
//     fFCurFrame    = new TGCompositeFrame(fFCurgFrame, 60, 60, kHorizontalFrame);
//     fFCurCheck[0]  = new TGCheckButton(fFCurFrame, "PITA",100);
//     fFCurCheck[1]  = new TGCheckButton(fFCurFrame, "IA Cell",101);  
//     for (Int_t i =0 ; i<2  ;i++) 
//       {
//        fFCurCheck[i]->Associate(this);
//        fFCurFrame->AddFrame(fFCurCheck[i],fL3);
//       }
//     fFCurgFrame->AddFrame(fFCurFrame,fL3);
//     fFPosgFrame   = new TGGroupFrame(fFeedbackFrame,new TGString("Position Feedback"));    
//     fFPosFrame    = new TGCompositeFrame(fFPosgFrame, 60, 60, kHorizontalFrame);
//     fFPosCheck    = new TGCheckButton(fFPosFrame, "PZT",102);
//     fFPosCheck->Associate(this);
//     fFPosFrame->AddFrame(fFPosCheck,fL3);
//     fFPosgFrame->AddFrame(fFPosFrame,fL3);    
//     fFeedbackFrame->AddFrame(fFCurgFrame,fL2);
//     fFeedbackFrame->AddFrame(fFPosgFrame,fL2);

//     fDITHgFrame   = new TGGroupFrame(fControlFrame,new TGString("Beam Dithering"));    
//     fDITHFrame    = new TGCompositeFrame(fDITHgFrame, 60, 60, kHorizontalFrame);
//     fDITHCheck[0]    = new TGCheckButton(fDITHFrame, "Position",103);
//     fDITHCheck[1]    = new TGCheckButton(fDITHFrame, "Energy",104);
//     fDITHCheck[0]->Associate(this);
//     fDITHCheck[1]->Associate(this);
//     fDITHFrame->AddFrame(fDITHCheck[0],fL3);
//     fDITHFrame->AddFrame(fDITHCheck[1],fL3);
//     fDITHgFrame->AddFrame(fDITHFrame,fL3); 
           
//    fControlFrame->AddFrame(fFeedbackFrame,fL2);
//     fControlFrame->AddFrame(fDITHgFrame,fL2);
//    fHistoFrame->AddFrame(fControlFrame,fL2);

//  fHistoFrame = fTab->AddTab("Single PLOT");
//  fTab->Associate(this);
//   TGCompositeFrame *fEmCanFrame = new TGCompositeFrame(fHistoFrame, 60, 60, kHorizontalFrame);
//   fEmCan = new TRootEmbeddedCanvas("EmCan", fEmCanFrame, 300, 300);
//   fEmCanFrame->AddFrame(fEmCan,fL4);
//   fHistoFrame->AddFrame(fEmCanFrame,fL4);
  fMainFrame->AddFrame(fTab,fL4);
  //fEmCan->GetCanvas()->Divide(1,2);
  
   //----------------------------------------------------------------------//
     // Display interface windows on screen
  SetWindowName("PANAM");
  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow(); 
  //-----------------------------------------------------------------------//
  cout<<"InitGUIFrames() : PANAM GUI initialized..."<<endl;
}

Int_t
TaPanam::ThreadIsRunning()
{
  if (!fPanThreadRun) return 0;
  else return 1;
}


void
TaPanam::DoPan()
{ // launch Pan analysis ( run analysis for the moment )
  // try initialize analysis manager out of the thread...  
  fAnaMan->Process();
  cout<<" killing refresh thread"<<endl;
  TThread::Kill("refreshThread"); 
  gSystem->Sleep(1000);
  RefreshThreadStop();   
  fAnaMan->End();
  cout<<" deleting fAnaMan object "<<endl; 
  delete fAnaMan;
  fAnaMan = NULL;
  cout<<"fAnaMan ptr :"<<fAnaMan<<endl;
  fMenuMon->DisableEntry(M_MON_STOP);     // disable STOP
  fMenuMon->DisableEntry(M_MON_START);   // disable START
  fMenuMon->EnableEntry(M_MON_PRESTART);   // enable PRESTART
  fMenuMon->EnableEntry(M_MON_END);   // enable PRESTART
  //   just a test....
  //  cout<<" Pan analysis"<<endl;
  //  gSystem->Sleep(500);   
}
void 
TaPanam::DoRefreshing()
{
#ifdef REFRESH
  cout<<"Refresh..."<<endl;
  // wait for 2 seconds of data
  if (RefreshCanvas()) cout<<" RefreshCanvas() : Canvases REFRESHED..."<<flush<<endl;
  gSystem->Sleep(2000);
#endif
}

Int_t TaPanam::RefreshCanvas()
{
  UpdateCanvasPads();
 return 1;
}

void TaPanam::SetStyles()
{
     fStripStyle = new TStyle("STRIPCHART_STYLE","STRIPCHART_STYLE");
     fStripStyle->SetOptDate(0);
     fStripStyle->SetOptStat(0);
     fStripStyle->SetOptLogy(0);
     //     fStripStyle->SetOptTitle(1);
     fStripStyle->SetFrameBorderMode(0);
     fStripStyle->SetFrameBorderSize(0);
     fStripStyle->SetCanvasColor(10);
     fStripStyle->SetPadColor(10);
     //fStripStyle->SetFrameLineWidth(10);
  
     // pads parameters
     fStripStyle->SetGridColor(15);
     //fStripStyle->SetPadColor(12);
     fStripStyle->SetPadBorderMode(0);
     fStripStyle->SetPadBorderSize(0); 
     fStripStyle->SetPadLeftMargin(0.25);
     fStripStyle->SetPadGridX(1);
     fStripStyle->SetPadGridY(1);
     fStripStyle->SetFuncWidth(1);
     fStripStyle->SetTitleColor(10);
     fStripStyle->SetLabelSize(0.07,"x");
     fStripStyle->SetLabelSize(0.07,"y");
     fStripStyle->SetTitleFont(42);
     fStripStyle->SetTitleH(0.09);
     fStripStyle->SetTitleW(0.6);
     fStripStyle->SetTitleX(0.1);
     fStripStyle->SetTitleBorderSize(0);
     fStripStyle->SetFrameFillColor(10);

  fHistoStyle = new TStyle("HISTOGRAMS_STYLE","HISTOGRAMS_STYLE");
  fHistoStyle->SetOptDate(0);
  fHistoStyle->SetOptStat(111111);
  fHistoStyle->SetOptLogy(0);
  fHistoStyle->SetOptTitle(0);
  fHistoStyle->SetFrameBorderSize(0);
  fHistoStyle->SetCanvasColor(10);
  fHistoStyle->SetFrameFillColor(10);
  // pads parameters  
  fHistoStyle->SetFuncWidth(2);
  fHistoStyle->SetPadColor(10);
  fHistoStyle->SetPadBorderMode(0);
  fHistoStyle->SetPadBorderSize(0);
  fHistoStyle->SetPadGridX(0);
  fHistoStyle->SetPadGridY(0);
  fHistoStyle->SetFuncWidth(1);
  fHistoStyle->SetTitleColor(10);
  fHistoStyle->SetLabelSize(0.05,"x");
  fHistoStyle->SetLabelSize(0.05,"y");
  fHistoStyle->SetTitleFont(42);
  fHistoStyle->SetTitleH(0.09);
  fHistoStyle->SetTitleW(0.5);
  fHistoStyle->SetTitleX(0.1);
  fHistoStyle->SetTitleBorderSize(1);
  fHistoStyle->SetOptFit(111111);

//   fGraphStyle = new TStyle("GRAPH_STYLE","GRAPH_STYLE");
//   fGraphStyle->SetOptStat(0);
//   fGraphStyle->SetOptLogx(0);
//   fGraphStyle->SetOptLogy(0);
//   fGraphStyle->SetPadGridX(0);
//   fGraphStyle->SetPadGridY(0);
//   fGraphStyle->SetFrameBorderMode(0);
//   fGraphStyle->SetFrameBorderSize(0);
//   fGraphStyle->SetPadBorderMode(0);
//   fGraphStyle->SetPadBorderSize(0);
//   fGraphStyle->SetCanvasColor(10);
//   fGraphStyle->SetFrameFillColor(10);
//   fGraphStyle->SetPalette(9,0);
//   fGraphStyle->SetFuncColor(2);
//   fGraphStyle->SetFuncWidth(2);
//   fGraphStyle->SetLabelSize(0.10,"x");
//   fGraphStyle->SetLabelSize(0.07,"y");
//   fGraphStyle->SetTitleFont(42);
//   fGraphStyle->SetTitleH(0.09);
//   fGraphStyle->SetTitleW(0.8);
//   fGraphStyle->SetTitleX(0.1);
//   fGraphStyle->SetTitleBorderSize(1);
//   fGraphStyle->SetOptFit(0);

}

