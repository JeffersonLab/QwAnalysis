// Author : Jeong Han Lee
// Date   : Friday, January 27 15:26:23 EST 2012
// 
//         a TS, MD, and Scanner Hit and histogram plot script
//         which is written for Jeong Han Lee. So it is very
//         incovenient to run this script
//
//
//  NOTE that : a ROOT file must be opened with qwroot before loading this script
//              'tab' will show possible options of a function
//              

//         For example 
//        
//         ~$ qwroot $QW_ROOTFILES/TrackingDAQ_15144.root
//         Qw-Root [1] .L $QWANALYSIS/Extensions/Macros/Tracking/QwHitCheck.C 
//         Qw-Root [2] TSsMTPlot()
//                     
//          0.0.1 : Friday, January 27 15:26:27 EST 2012
//          0.0.2 : Tuesday, March 20 00:03:53 EDT 2012, jhlee
//                  - added more description which how to use
//                  - refined TSsMT and MDsMT functions        
// 

 gStyle->SetStatW(0.4);//   - set the width of a stat box
 gStyle->SetStatH(0.4);//   - set the width of a stat box


TCanvas c1;


void 
TSGeneralPlot()
{
 
  c1.Clear();
  c1.Divide(4,3);
  Int_t i =0;
  
  TString BName[10] = 
    {
      "ts2m_adc", "ts1m_adc", "ts2p_adc", "ts2m_adc", 
      "ts2m_sca", "ts1m_sca", "ts2p_sca", "ts2m_sca", 
      "ts1mt_sca", "ts2mt_sca"
    };
      

  TString name ="";
  TString cut = "";
 
  name = "trigscint.";
  gStyle->SetStatW(0.4);//   - set the width of a stat box
  gStyle->SetStatH(0.4);//   - set the width of a stat box
  for (i=0; i<10; i++) 
    {
      c1.cd(i+1);
      name.Clear();
      name += BName[i];
      cut  = name +"!=0";
      std::cout << i << " Name " << name << " cut " << cut << std::endl;
      event_tree->Draw(name, cut);
      gPad->Update();
    }   
  c1.Update();
  return;
}


void 
TSF1Plot(Int_t plane, Int_t element)
{
  Int_t region = 4;
  //  TCanvas c1;
  c1.Clear();
  TString name ="";
  TString cut = "";
  gStyle->SetStatW(0.4);//   - set the width of a stat box
  gStyle->SetStatH(0.4);//   - set the width of a stat box
  c1.Divide(3,1);
  //  if(region==5) {
  name = "trigscint.ts";
  name += plane;
  if (element == 1) {
    name += "p_f1";
  }
  else if( element == 2) {
    name += "m_f1";
  }
  else if (element == 0) {
    name += "mt_f1";
  }
  else if (element == 3 ) {
    name += "software_meantime";
  }
  else {
    return;
  }
  cut = name +"!=0";

  std::cout << " Name " << name << " cut " << cut << std::endl;

  
  if (! name.Contains("software")) {
    c1.cd(1);
    event_tree->Draw(name, cut);
    gPad->Update();
  }
  c1.cd(2);
  event_tree->Draw("fQwHits.fTimeNs", Form("fQwHits.fRegion==%d && fQwHits.fPlane==%d && fQwHits.fElement==%d && fQwHits.fHitNumber==0 && fQwHits.fTimeNs!=0", region, plane, element));
  gPad->Update();
  c1.cd(3);
  event_tree->Draw("fQwHits.fTimeNs", Form("fQwHits.fRegion==%d && fQwHits.fPlane==%d && fQwHits.fElement==%d && fQwHits.fTimeNs!=0", region, plane, element));
  gPad->Update();
  c1.Update();

  return;
}



void 
MDGeneralPlot()
{
  //  TCanvas c1;
  c1.Clear();
  c1.Divide(8,5);
  Int_t i =0;
  
  TString BName[40] = 
    {
      "md1m_adc", "md2m_adc", "md3m_adc", "md4m_adc", "md5m_adc", "md6m_adc", "md7m_adc", "md8m_adc",
      "md1p_adc", "md2p_adc", "md3p_adc", "md4p_adc", "md5p_adc", "md6p_adc", "md7p_adc", "md8p_adc",
      "md1m_sca", "md2m_sca", "md3m_sca", "md4m_sca", "md5m_sca", "md6m_sca", "md7m_sca", "md8m_sca",
      "md1p_sca", "md2p_sca", "md3p_sca", "md4p_sca", "md5p_sca", "md6p_sca", "md7p_sca", "md8p_sca",
      "md1bar_sca", "md2bar_sca", "md3bar_sca", "md4bar_sca", "md5bar_sca", "md6bar_sca", "md7bar_sca", "md8bar_sca"
    };
      

  TString name ="";
  TString cut = "";
 
  name = "maindet.";
  gStyle->SetStatW(0.6);//   - set the width of a stat box
  gStyle->SetStatH(0.6);//   - set the width of a stat box
  for (i=0; i<40; i++) 
    {
      c1.cd(i+1);
      name.Clear();
      name += BName[i];
      cut  = name +"!=0";
      std::cout << i << " Name " << name << " cut " << cut << std::endl;
      event_tree->Draw(name, cut);
      gPad->Update();
    }   
  c1.Update();
  return;
}



void 
MDF1Plot(Int_t plane, Int_t element)
{
  //  TCanvas c1;
  c1.Clear();
  TString name ="";
  TString cut = "";
 
  c1.Divide(3,1);
  //  if(region==5) {
  name = "maindet.md";
  name += plane;
  if (element == 1) {
    name += "p_f1";
  }
  else if( element == 2) {
    name += "m_f1";
  }
  else if (element == 3 ) {
    name += "software_meantime";
  }
  else {
    return;
  }

  cut = name +"!=0";

  
  printf("\"%s\":%s\n", name.Data(), cut.Data());
  c1.cd(1);
  if (! name.Contains("software")) {
    event_tree->Draw(name.Data(), cut.Data());
  }
  gPad->Update();
  c1.cd(2);
  event_tree->Draw("fQwHits.fTimeNs", Form("fQwHits.fRegion==5 && fQwHits.fPlane==%d && fQwHits.fElement==%d && fQwHits.fHitNumber==0  && fQwHits.fTimeNs!=0", plane, element));
  gPad->Update();
  c1.cd(3);
  event_tree->Draw("fQwHits.fTimeNs", Form("fQwHits.fRegion==5 && fQwHits.fPlane==%d && fQwHits.fElement==%d  && fQwHits.fTimeNs!=0", plane, element));
  gPad->Update();
  c1.Update();

  return;
}

// TCanvas c1
// c1.Divide(3,1)

// c1.cd(1)
// event_tree->Draw("maindet.md1m_f1", "maindet.md1m_f1!=0")
// c1.cd(2)
// event_tree->Draw("fQwHits.fTimeNs", "fQwHits.fRegion==5 && fQwHits.fPlane==1 && fQwHits.fElement==2 && fQwHits.fHitNumber==0")
// c1.cd(3)
// event_tree->Draw("fQwHits.fTimeNs", "fQwHits.fRegion==5 && fQwHits.fPlane==1 && fQwHits.fElement==2")
// c1.Update()



void 
ScannerGeneralPlot()
{
  //  TCanvas c1;
  c1.Clear();

  c1.Divide(6,5);
  Int_t i =0;
  
  TString ScannerBName[30] = {"PowSupply_VQWK",    "PositionX_VQWK",    "PositionY_VQWK", "FrontSCA",         "BackSCA", 
			      "CoincidenceSCA",    "FrontADC",          "BackADC",        "FrontTDC",         "BackTDC", 
			      "PositionX_QDC",     "PositionY_QDC",     "front_adc_raw",  "back__adc_raw",    "pos_x_adc_raw", 
			      "pos_y_adc_raw",     "front_f1_raw",      "back__f1_raw",   "coinc_f1_raw",     "ref_t_f1_raw",
			      "front_sca_raw",     "back__sca_raw",     "coinc_sca_raw",  "randm_sca_raw",    "bg_chrlt_sca_raw", 
			      "bg_wilbr_sca_raw",  "phase_monitor_raw", "pos_x_vqwk_raw", "pos_y_vqwk_raw",   "power_vqwk_raw"};
  TString name ="";
  TString cut = "";
 
  name = "scanner.";
  gStyle->SetStatW(0.4);//   - set the width of a stat box
  gStyle->SetStatH(0.4);//   - set the width of a stat box
  for (i=0; i<30; i++) 
    {
      c1.cd(i+1);
      name.Clear();
      name += ScannerBName[i];
      cut  = name +"!=0";
      std::cout << i << " Name " << name << " cut " << cut << std::endl;
      event_tree->Draw(name, cut);
      gPad->Update();
    }   
  c1.Update();
  return;
}



void 
ScannerF1Plot(Int_t element)
{
  //  TCanvas c1;
  c1.Clear();
  TString name ="";
  TString cut = "";
 
  Int_t region = 6;
  Int_t plane = 0;

  c1.Divide(3,1);
  //  if(region==5) {
  name = "scanner.";
  //  name += plane;
  if (element == 1) {
    name += "front_f1_raw";
  }
  else if( element == 2) {
    name += "back__f1_raw";
  }
  else if( element == 0) {
    name += "coinc_f1_raw";
  }
  else {
    return;
  }
  cut = name +"!=0";

  
  printf("\"%s\":%s\n", name.Data(), cut.Data());
  c1.cd(1);
  event_tree->Draw(name.Data(), cut.Data());
  gPad->Update();
  c1.cd(2);
  event_tree->Draw("fQwHits.fTimeNs", Form("fQwHits.fRegion==%d && fQwHits.fElement==%d && fQwHits.fPlane==0 &&fQwHits.fHitNumber==0  && fQwHits.fTimeNs!=0", region,  element));
  gPad->Update();
  c1.cd(3);
  event_tree->Draw("fQwHits.fTimeNs", Form("fQwHits.fRegion==%d  && fQwHits.fPlane==0&& fQwHits.fElement==%d  && fQwHits.fTimeNs!=0", region, element));
  gPad->Update();
  c1.Update();

  return;
}


void 
VDC_F1_TimePlot()
{
  c1.Clear();

  c1.Divide(4,1);
  c1.cd(1);
  event_tree->Draw("fQwHits.fTimeNs","fQwHits.fRegion==3&&fQwHits.fPackage==1&&(fQwHits.fPlane==1||fQwHits.fPlane==2)"); // Vader
  gPad->Update();
  c1.cd(2);
  event_tree->Draw("fQwHits.fTimeNs","fQwHits.fRegion==3&&fQwHits.fPackage==1&&(fQwHits.fPlane==3||fQwHits.fPlane==4)"); // Leia
  gPad->Update();
  c1.cd(3);
  event_tree->Draw("fQwHits.fTimeNs","fQwHits.fRegion==3&&fQwHits.fPackage==2&&(fQwHits.fPlane==1||fQwHits.fPlane==2)"); // Yoda
  gPad->Update();
  c1.cd(4);
  event_tree->Draw("fQwHits.fTimeNs","fQwHits.fRegion==3&&fQwHits.fPackage==2&&(fQwHits.fPlane==3||fQwHits.fPlane==4)"); // Han
  gPad->Update();

  return;
}




void 
VDC_F1_VaderTimePlot()
{
  c1.Clear();

  c1.Divide(4,2);
  c1.cd(1);
  event_tree->Draw("fQwHits.fTimeNs","fQwHits.fRegion==3&&fQwHits.fPackage==1&&(fQwHits.fPlane==1||fQwHits.fPlane==2)"); // Vader
  gPad->Update();
  c1.cd(2);
  event_tree->Draw("fQwHits.fTimeNs","fQwHits.fRegion==3&&fQwHits.fPackage==1&&(fQwHits.fPlane==1||fQwHits.fPlane==2) && fQwHits.fHitNumber==0"); // Vader
  gPad->Update();
  c1.cd(3);
  event_tree->Draw("fQwHits.fTimeNs","fQwHits.fRegion==3&&fQwHits.fPackage==1&&(fQwHits.fPlane==1||fQwHits.fPlane==2) && fQwHits.fHitNumber==1"); // Vader
  gPad->Update();
  c1.cd(4);
  event_tree->Draw("fQwHits.fTimeNs","fQwHits.fRegion==3&&fQwHits.fPackage==1&&(fQwHits.fPlane==1||fQwHits.fPlane==2) && fQwHits.fHitNumber==2"); // Vader
  gPad->Update();
  c1.cd(5);
  event_tree->Draw("fQwHits.fTimeNs","fQwHits.fRegion==3&&fQwHits.fPackage==1&&(fQwHits.fPlane==1||fQwHits.fPlane==2) && fQwHits.fHitNumber==3"); // Vader
  gPad->Update();

  c1.cd(6);
  event_tree->Draw("fQwHits.fTimeNs","fQwHits.fRegion==3&&fQwHits.fPackage==1&&(fQwHits.fPlane==1||fQwHits.fPlane==2) && fQwHits.fHitNumber==4"); // Vader
  gPad->Update();
  c1.cd(7);
  event_tree->Draw("fQwHits.fTimeNs","fQwHits.fRegion==3&&fQwHits.fPackage==1&&(fQwHits.fPlane==1||fQwHits.fPlane==2) && fQwHits.fHitNumber==5"); // Vader
  gPad->Update();
  c1.cd(8);
  event_tree->Draw("fQwHits.fTimeNs","fQwHits.fRegion==3&&fQwHits.fPackage==1&&(fQwHits.fPlane==1||fQwHits.fPlane==2) && fQwHits.fHitNumber==6"); // Vader
  gPad->Update();
 
  

  return;
}



void 
TSsMTPlot(Int_t element=3)
{
  Int_t region = 4;
  //  TCanvas c1;
  c1.Clear();
  TString name ="";
  TString cut = "";
  gStyle->SetStatW(0.4);//   - set the width of a stat box
  gStyle->SetStatH(0.4);//   - set the width of a stat box
  c1.Divide(4,4);

  //  plane = 1;
  for(Int_t idx=0;idx<7; idx++)
    {
      c1.cd(idx+1);
      event_tree->Draw("fQwHits.fTimeNs", Form("fQwHits.fRegion==%d && fQwHits.fPlane==%d && fQwHits.fElement==%d && fQwHits.fHitNumber==%d && fQwHits.fTimeNs!=0", region, 1, element, idx));
      gPad->Update();
    }

  //  plane = 2;
  for(Int_t idx=0;idx<7; idx++)
    {
      c1.cd(idx+9);
      event_tree->Draw("fQwHits.fTimeNs", Form("fQwHits.fRegion==%d && fQwHits.fPlane==%d && fQwHits.fElement==%d && fQwHits.fHitNumber==%d && fQwHits.fTimeNs!=0", region, 2, element, idx));
      gPad->Update();
    }



  c1.Update();

  return;
}



void 
MDsMTPlot(Int_t plane)
{
  Int_t region = 5;
  //  TCanvas c1;
  c1.Clear();
  TString name ="";
  TString cut = "";
  gStyle->SetStatW(0.4);//   - set the width of a stat box
  gStyle->SetStatH(0.4);//   - set the width of a stat box
  c1.Divide(4,2);


  for(Int_t idx=0;idx<7; idx++)
    {
      c1.cd(idx+1);
      event_tree->Draw("fQwHits.fTimeNs", Form("fQwHits.fRegion==%d && fQwHits.fPlane==%d && fQwHits.fElement==3 && fQwHits.fHitNumber==%d && fQwHits.fTimeNs!=0", region, plane, idx));
      gPad->Update();
    }

  c1.Update();

  return;
}




// TCanvas c1;

// c1.Divide(3,1);
// c1.cd(1);
// event_tree->Draw("fQwHits.fTimeNs", "fQwHits.fRegion==6  && fQwHits.fElement==0 && fQwHits.fHitNumber==0");
// c1.cd(2);
// event_tree->Draw("fQwHits.fTimeNs", "fQwHits.fRegion==6  && fQwHits.fElement==0 && fQwHits.fHitNumber==1");
// c1.cd(3);
// event_tree->Draw("fQwHits.fTimeNs", "fQwHits.fRegion==6  && fQwHits.fElement==0 && fQwHits.fHitNumber==2");


// TCanvas c1;
// c1.Divide(3,2);
// c1.cd(1);
// event_tree->Draw("scanner.FrontTDC", "scanner.FrontTDC!=0");
// c1.cd(2);
// event_tree->Draw("scanner.BackTDC", "scanner.BackTDC!=0");

// c1.cd(4);
// event_tree->Draw("scanner.front_f1_raw", "scanner.front_f1_raw!=0");
// c1.cd(5);
// event_tree->Draw("scanner.back__f1_raw", "scanner.back__f1_raw!=0");
// c1.cd(6);
// event_tree->Draw("scanner.coinc_f1_raw", "scanner.coinc_f1_raw!=0");

