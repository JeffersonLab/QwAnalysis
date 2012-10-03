/* 
   ####################################################
   
   Qweak Transverse Polartization Monitoring Tool
   
   ####################################################
   Mainframe macro generated from application: /home/buddhini/Software/root-5-27-02/bin/root.exe
   By ROOT version 5.27/02 on 2011-10-05 10:14:17
   Author: B. Waidyawansa (buddhini@jlab.org)
   
   This tool is intended to be used for the purpose of determining the transverse polarization in the
   electron beam during the running period of Qweak experiment (2010-2012).
   
   - It uses the REGRESSED main detector asymmetries in QWEAK RUN1 PASS3 data base.
   
   - The fit uses the -4.75 ppm transverse asymmetry for the LH2 cell measured from the 
   vertical transverse running described in Qweak-doc-1401-v3.
   
   - The canvases are saved to *.png plots with the relevent slug ranges.
    
*/

#include "QwTransverseGUI.h"

ClassImp(QwTransverseGUI);


TString quartz_bar_sum[8]=
  {
    "qwk_md1barsum","qwk_md2barsum","qwk_md3barsum","qwk_md4barsum",
   "qwk_md5barsum","qwk_md6barsum","qwk_md7barsum","qwk_md8barsum"
  };

TString opposite_quartz_bar_sum[4]=
  {
  "qwk_md1_qwk_md5","qwk_md2_qwk_md6","qwk_md3_qwk_md7","qwk_md4_qwk_md8"
  };


enum QwTransverseGUIIdentificator {
  BA_PLOT,
  BA_EXIT,
  N_START,
  N_END,
  TAB1,
  TAB2,
  TAB3
};

QwTransverseGUI::QwTransverseGUI()
{

  SetCleanup(kDeepCleanup);


  // main frame
  fMainFrame = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
  fMainFrame->SetLayoutBroken(kTRUE);

   // composite frame
  fCompositeFrame = new TGCompositeFrame(fMainFrame,1000,800,kVerticalFrame);
  fCompositeFrame->SetLayoutBroken(kTRUE);


  // font changes
  ufont = gClient->GetFont("-*-bitstream charter-bold-r-*-*- 0-*-*-*-*-*-*-*");

  // graphics context changes
  GRChanges.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
  gClient->GetColorByName("#000099",GRChanges.fForeground);
  gClient->GetColorByName("#c0c0c0",GRChanges.fBackground);
  GRChanges.fFillStyle = kFillSolid;
  GRChanges.fFont = ufont->GetFontHandle();
  GRChanges.fGraphicsExposures = kFALSE;
  uGC = gClient->GetGC(&GRChanges, kTRUE);

  // color changes
  gClient->GetColorByName("#ffffff",ucolor);

  // Main label
  fTitleLabel = new TGLabel(fCompositeFrame,"Qweak Transverse Polarization Monitor",uGC->GetGC(),
			    ufont->GetFontStruct(),kChildFrame,ucolor);
  fTitleLabel->SetTextJustify(36);
  fTitleLabel->SetMargins(0,0,0,0);
  fTitleLabel->SetWrapLength(-1);
  fCompositeFrame->AddFrame(fTitleLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fTitleLabel->MoveResize(16,8,970,32);

  // Buttons
  fTButtonPlot = new TGTextButton(fCompositeFrame,"&Plot", BA_PLOT);
  fTButtonPlot->SetTextJustify(36);
  fTButtonPlot->SetMargins(0,0,0,0);
  fTButtonPlot->SetWrapLength(-1);
  fTButtonPlot->Resize(112,32);
  fCompositeFrame->AddFrame(fTButtonPlot, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fTButtonPlot->MoveResize(858,260,112,32);
  fTButtonPlot->Connect("Pressed()", "QwTransverseGUI", this, "MainFunction()");

  fTButtonExit = new TGTextButton(fCompositeFrame,"&Exit",BA_EXIT);
  fTButtonExit->SetTextJustify(36);
  fTButtonExit->SetMargins(0,0,0,0);
  fTButtonExit->SetWrapLength(-1);
  fTButtonExit->Resize(112,32);
  fCompositeFrame->AddFrame(fTButtonExit, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fTButtonExit->MoveResize(858,740,112,32);
  fTButtonExit->Connect("Pressed()", "TApplication", gApplication, "Terminate()");

  // Number entries
  dStartSlug    = new TGNumberEntry(fCompositeFrame, 40, 5, N_START, 
				    TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);
  fCompositeFrame->AddFrame(dStartSlug, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  dStartSlug->MoveResize(858,130,112,22);

  fLabelStartSlug = new TGLabel(fCompositeFrame,"Starting Slug");
  fLabelStartSlug ->SetTextJustify(36);
  fLabelStartSlug ->SetMargins(0,0,0,0);
  fLabelStartSlug ->SetWrapLength(-1);
  fCompositeFrame->AddFrame(fLabelStartSlug , new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fLabelStartSlug ->MoveResize(858,100,112,16);
  dEndSlug    = new TGNumberEntry(fCompositeFrame, 119, 5, N_END, 
				  TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative);


  fCompositeFrame->AddFrame(dEndSlug, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  dEndSlug->MoveResize(858,210,112,22);
  dEndSlug-> Associate(this);

  fLabelEndSlug = new TGLabel(fCompositeFrame,"End Slug");
  fLabelEndSlug->SetTextJustify(36);
  fLabelEndSlug->SetMargins(0,0,0,0);
  fLabelEndSlug->SetWrapLength(-1);
  fCompositeFrame->AddFrame(fLabelEndSlug, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fLabelEndSlug->MoveResize(858,180,112,16);

  //Vertical line
  fVertical3DLine = new TGVertical3DLine(fCompositeFrame,8,465);
  fCompositeFrame->AddFrame(fVertical3DLine, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fVertical3DLine->MoveResize(584,48,8,465);
  
  // tab widget
  fMainTab = new TGTab(fCompositeFrame,820,730);

  // container of "Tab1"
  fTab1Frame = fMainTab->AddTab("Octant Fit for P_T and P_V");
  fTab1Frame->SetLayoutManager(new TGVerticalLayout(fTab1Frame));
  
  // embedded canvas
  fTab1Canvas = new TRootEmbeddedCanvas(0,fTab1Frame,818,728 );
  Int_t wfTab1Canvas = fTab1Canvas->GetCanvasWindowId();
  TCanvas *cTab1Canvas = new TCanvas("mc1", 10, 10, wfTab1Canvas);
  fTab1Canvas->AdoptCanvas(cTab1Canvas);
  fTab1Frame->AddFrame(fTab1Canvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  
  // container of "Tab2"
  fTab2Frame = fMainTab->AddTab("Opposite Bar Sum");
  fTab2Frame->SetLayoutManager(new TGVerticalLayout(fTab2Frame));

  // embedded canvas
  fTab2Canvas = new TRootEmbeddedCanvas(0,fTab2Frame,818,728);
  Int_t wfTab2Canvas = fTab2Canvas->GetCanvasWindowId();
  TCanvas *cTab2Canvas = new TCanvas("mc2", 10, 10, wfTab2Canvas);
  fTab2Canvas->AdoptCanvas(cTab2Canvas);
  fTab2Frame->AddFrame(fTab2Canvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  // container of "Tab3"
  fTab3Frame = fMainTab->AddTab("Variation of P_T and P_V");
  fTab3Frame->SetLayoutManager(new TGVerticalLayout(fTab3Frame));
  
  // embedded canvas
  fTab3Canvas = new TRootEmbeddedCanvas(0,fTab3Frame,818,728 );
  Int_t wfTab3Canvas = fTab3Canvas->GetCanvasWindowId();
  TCanvas *cTab3Canvas = new TCanvas("mc1", 10, 10, wfTab3Canvas);
  fTab3Canvas->AdoptCanvas(cTab3Canvas);
  fTab3Frame->AddFrame(fTab3Canvas, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  fMainTab->SetTab(0);

  fMainTab->Resize(fMainTab->GetDefaultSize());
  fCompositeFrame->AddFrame(fMainTab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fMainTab->MoveResize(8,48,820,730);

  fMainFrame->AddFrame(fCompositeFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  fCompositeFrame->MoveResize(0,0,1000,800);

  fMainFrame->SetMWMHints(kMWMDecorAll,kMWMFuncAll,kMWMInputModeless);
  fMainFrame->MapSubwindows();
  
  fMainFrame->Resize(fMainFrame->GetDefaultSize());
  fMainFrame->MapWindow();
  fMainFrame->Resize(1000,800);
  SetWindowName("Qweak Transverse Polarization Monitor");
  MapRaised();    


  slug_last = 0;
  slug_first = 0;
  slug_range = 9;  
  usefit1 = false;
}

Bool_t QwTransverseGUI::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
  // Process events generated by the buttons in the frame.
  
  switch (GET_MSG(msg)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {
    case kC_TEXTENTRY:
      switch(parm1)
	{
	case  N_START:
	  slug_first = dStartSlug->GetIntNumber();
	  break;
	case  N_END:
	  slug_last  = dStartSlug->GetIntNumber();
	  break;
	}
    case kCM_TAB:
  //     switch(parm1)
// 	{
      std::cout<<tab id<<std::endl; 
// 	}

    default:
      break;
    }	
  default:
    break;
  }
  return kTRUE;
}



/*the main function*/
void QwTransverseGUI::MainFunction()
{

  // Fit and stat parameters
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0000000);
  gStyle->SetStatY(0.99);
  gStyle->SetStatX(0.99);
  gStyle->SetStatW(0.10);
  gStyle->SetStatH(0.3);

  
  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.18);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadRightMargin(0.22);
  gStyle->SetPadLeftMargin(0.08);

  
  // histo parameters
  gStyle->SetTitleYOffset(1.6);
  gStyle->SetTitleXOffset(1.6);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.05,"x");
  gStyle->SetTitleSize(0.05,"y");
  gStyle->SetTitleX(0.1);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.09);
  gDirectory->Delete("*");

  
  /*connect to the data base*/
  // db = TSQLServer::Connect("mysql://cdaql6.jlab.org/qw_fall2010_20101204","qweak", "QweakQweak");
  // db = TSQLServer::Connect("mysql://127.0.0.1/qw_run1_pass3","qweak", "QweakQweak");
  db = TSQLServer::Connect("mysql://qweakdb.jlab.org/qw_run1_pass3","qweak", "QweakQweak");


  if(db)
    printf("Server info: %s\n", db->ServerInfo());
  else
    exit(1);
  
 
 /*Clear the other arrays*/
  for(Int_t i =0;i<8;i++){
    barvaluesin[i]  = 0.0;
    barvaluesout[i] = 0.0;
    barerrorsin[i]  = 0.0;
    barerrorsout[i] = 0.0;
   }

  slug_first = dStartSlug->GetIntNumber();
  slug_last = dEndSlug->GetIntNumber();
  slug_range = slug_last-slug_first;


  /*Get data from database*/
  std::cout<<"IHWP 'in'.."<<std::endl;
  get_data(quartz_bar_sum,8,"in",barvaluesin,barerrorsin);
  get_data(opposite_quartz_bar_sum,4,"in",oppvaluesin,opperrorsin);

  std::cout<<"IHWP 'out'.."<<std::endl;
  get_data(quartz_bar_sum,8,"out",barvaluesout,barerrorsout);
  get_data(opposite_quartz_bar_sum,4,"out",oppvaluesout,opperrorsout);

  TCanvas *mc1        = NULL;
  mc1 = fTab1Canvas->GetCanvas();
  mc1->Clear();

  /*Fit octants*/
  plot_n_fit_data(8, "cosfit",  barvaluesin,barerrorsin, barvaluesout,barerrorsout);
  mc1->Update();
  if(usefit1)
    mc1->Print(Form("transverse_monitor_slugs_all_fit1_%i_%i_plots.png",slug_first,slug_last));
  else
    mc1->Print(Form("transverse_monitor_slugs_all_%i_%i_plots.png",slug_first,slug_last));


  /*Fit opposite bar sums*/
  TCanvas *mc2        = NULL;
  mc2 = fTab2Canvas->GetCanvas();
  mc2->Clear();
  plot_n_fit_data(4, "pol0",  oppvaluesin,opperrorsin, oppvaluesout,opperrorsout);
  mc2->Update();
  mc2->Print(Form("transverse_monitor_slugs_opposite_%i_%i_plots.png",slug_first,slug_last));


  /*Plot variation over slugs*/
  TCanvas *mc3        = NULL;
  mc3 = fTab3Canvas->GetCanvas();
  mc3->Clear();
  plot_pv_ph_in_slugs();

  std::cout<<"Done! \n";
  db->Close();
  delete db;

}



/*A function to create the mysql query to grab the runlet average of a md detector*/
TString QwTransverseGUI::data_query(TString device,Int_t slug,TString ihwp){

  Bool_t ldebug = true;

  TString datatable = "md_data_view";
 
  TString output = " sum( distinct("+datatable+".value/(POWER("
    +datatable+".error,2))))/sum( distinct(1/(POWER("
    +datatable+".error,2)))), SQRT(1/SUM(distinct(1/(POWER("
    +datatable+".error,2)))))";
  
 
  TString slug_cut   = Form("%s.slug = %i",
			   datatable.Data(),slug);

  TString run_quality =  Form("(%s.run_quality_id = '1') ",
			   datatable.Data());

  TString good_for =  Form("(%s.good_for_id = '1' or %s.good_for_id = '1,3')",
			      datatable.Data(),datatable.Data());

  TString regression = Form("%s.slope_calculation = 'off' and %s.slope_correction = 'on' ",
			    datatable.Data(),datatable.Data()); 

  /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
  TString query =" SELECT " +output+ " FROM "+datatable+" , slow_controls_settings WHERE "
    +datatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +datatable+".detector = '"+device+"' AND "+datatable+".subblock = 0 AND "
    +datatable+".measurement_type = 'a' AND target_position = 'HYDROGEN-CELL' AND "
    +slug_cut+" AND "+regression+" AND "+run_quality+" AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for+" AND "
    +datatable+".error != 0; ";

  if(ldebug) std::cout<<query<<std::endl;
  return query;
}

/*A function to get data from the database for the octants*/
void QwTransverseGUI::get_data(TString devicelist[],Int_t size,TString ihwp,
			       Double_t value[], Double_t error[]){
  
  Bool_t ldebug = true;

  TString query;
  TSQLStatement* stmt = NULL;
  
  std::cout<<"Extracting average asymmetries from slug "<<slug_first<<" to "<<slug_last<<std::endl;

  for(Int_t i=0 ; i<size ;i++){

    if(ldebug) printf("Getting data for %10s ihwp %5s ", devicelist[i].Data(), ihwp.Data());

    query = get_sum_query(Form("%s",devicelist[i].Data()),ihwp);
    
    stmt = db->Statement(query,100);
    if(!stmt)  {
      db->Close();
      exit(1);
    }
    /* process statement */
    if (stmt->Process()) {
      /* store result of statement in buffer*/
      stmt->StoreResult();
      while (stmt->NextResultRow()) {
	value[i] = (Double_t)(stmt->GetDouble(0))*1e6; // convert to  ppm
	error[i] = (Double_t)(stmt->GetDouble(1))*1e6; // ppm
	if(ldebug) printf(" value = %16.8lf +- %10.8lf [ppm]\n", value[i], error[i]);
      }
    }
    delete stmt;    
  }
}


/*A function to create the mysql query*/
TString QwTransverseGUI::get_sum_query(TString device,TString ihwp){

  Bool_t ldebug = false;

  TString datatable = "md_data_view";
 
  TString output = " sum( distinct("+datatable+".value/(POWER("
    +datatable+".error,2))))/sum( distinct(1/(POWER("
    +datatable+".error,2)))), SQRT(1/SUM(distinct(1/(POWER("
    +datatable+".error,2)))))";
  
  TString slug_cut   = Form("(%s.slug >= %i and %s.slug <= %i)",
			  datatable.Data(),slug_first,datatable.Data(),slug_last);

  TString run_quality =  Form("(%s.run_quality_id = '1') ",
			   datatable.Data());

  TString good_for =  Form("(%s.good_for_id = '1' or %s.good_for_id = '1,3')",
			      datatable.Data(),datatable.Data());

  TString regression = Form("%s.slope_calculation = 'off' and %s.slope_correction = 'on' ",
			    datatable.Data(),datatable.Data()); 
       
  /*Select md asymmetries for LH2 from parity, production that are good/suspect*/
  TString query =" SELECT " +output+ " FROM "+datatable+", slow_controls_settings WHERE "
    +datatable+".runlet_id =  slow_controls_settings.runlet_id AND "
    +datatable+".detector = '"+device+"' AND "+datatable+".subblock = 0 AND "
    +datatable+".measurement_type = 'a' AND target_position = 'HYDROGEN-CELL' AND "
    +slug_cut+" AND "+regression+" AND "+run_quality+" AND "
    +" slow_helicity_plate= '"+ihwp+"' AND "+good_for+" AND "
    +datatable+".error != 0; ";

  if(ldebug) std::cout<<query<<std::endl;
  return query;
}



void QwTransverseGUI::plot_n_fit_data(const Int_t size, TString fit, 
				      Double_t valuesin[],Double_t errorsin[], 
				      Double_t valuesout[],Double_t errorsout[]){

  TF1 *cosfit;
  TF1 *cosfit_diff;
  TString usefit="";
  Double_t phase_low;
  Double_t phase_up;

  Double_t x[size];
  Double_t errx[size];
  Double_t valuesum[size];
  Double_t valueerror[size];
  Double_t valuediff[size];
  Double_t errordiff[size];

  /*set X location and clear the other arrays*/
  for(Int_t i =0;i<size;i++){
    x[i]         = i+1;
    errx[i]      = 0.0;
    valuesum[i]  = 0.0;
    valueerror[i]= 0.0;
    valuediff[i] = 0.0;
    errordiff[i] = 0.0;
  }
  
  TPad* pad = (TPad*)(gPad->GetMother());
  pad->cd();
  pad->Clear();
  
  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(20);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  TString title;

  if(fit == "cosfit"){
    if(usefit1)
      title = Form("LH2: Regressed Main detector asymmetries from slugs %i to %i: Fit (-4.75)*p0*sin(p1)*[sin(p2)*cos(phi) - cos(p2)*sin(phi)]+ p3",slug_first,slug_last);
    else
      title = Form("LH2: Regressed Main detector asymmetries from slugs %i to %i: Fit (-4.75)*(p0*cos(phi) - p1*sin(phi))+ p2",slug_first,slug_last);
  }
  if(fit == "pol0")
    title = Form("LH2: Regressed slug averages of Opposite Main detector bar sum asymmetries from slugs %i to %i: Fit p0",slug_first,slug_last);


  TString text = Form(title);
  TText*t1 = new TText(0.06,0.3,text);
  t1->SetTextSize(0.35);
  t1->Draw();
  
  pad2->cd();
  pad2->Divide(1,2);
  
  pad2->cd(1);


  /*cosine fit is for the octants and constant fit is for the opposite octants*/
  if(fit == "pol0"){
    usefit = "pol0";
  }  
  if(fit =="cosfit"){

    usefit = "cosfit";
    // Define the cosine fit
    //    cosfit = new TF1("cosfit","[0]*-4.75*cos((pi/180)*(45*(x-1)+[1])) +[2]",1,8);
    
    if(usefit1){
      cosfit = new TF1("cosfit"," (-4.75)*[0]*sin((pi/180)*[1])*(sin((pi/180)*[2])*cos((pi/180)*(45*(x-1))) - cos((pi/180)*[2])*sin((pi/180)*(45*(x-1))))+ [3]",1,8);
      cosfit->SetParLimits(0,0,99999);
      cosfit->SetParLimits(1,0,99999);
      cosfit->SetParLimits(1,-180,180);
      cosfit->SetParLimits(2,-180,180);
    } 
    else {
      cosfit = new TF1("cosfit","(-4.75)*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))))+ [2]",1,8);
    }
  }

  /*Draw IN values*/
  TGraphErrors* grp_in  = new TGraphErrors(size,x,valuesin,errx,errorsin);
  grp_in ->SetMarkerSize(0.8);
  grp_in ->SetMarkerStyle(21);
  grp_in ->SetMarkerColor(kBlue);
  grp_in ->SetLineColor(kBlue);
  grp_in->Fit(usefit,"EM");
  TF1* fit1 = grp_in->GetFunction(usefit);

  fit1->DrawCopy("same");
  fit1->SetLineColor(kBlue);
 

  if(fit =="cosfit"){

    // Define the cosine fit for out. Use the p0, p1 and p2 for initialization and limits of this fit.
    // We dont't want the phase to change with the IHWP. The sign of the amplitude is the onlything
    // that should change with IHWP. But we will allow a +-90 degree leverage for the phase to see 
    // if fo some reason, changing the IHWP has changed the verticle to horizontal transverse
    // or vise versa.
    
    if(usefit1){
      cosfit = new TF1("cosfit"," (-4.75)*[0]*sin((pi/180)*[1])*(sin((pi/180)*[2])*cos((pi/180)*(45*(x-1))) - cos((pi/180)*[2])*sin((pi/180)*(45*(x-1))))+ [3]",1,8);

      cosfit->SetParLimits(0,-99999,0);
      cosfit->SetParameter(0,-(fit1->GetParameter(0)));
      cosfit->SetParameter(1, (fit1->GetParameter(1)));
      cosfit->SetParameter(2, (fit1->GetParameter(2)));

      phase_low = fit1->GetParameter(1)-90;
      phase_up  = fit1->GetParameter(1)+90;
      cosfit->SetParLimits(1,phase_low,phase_up);
      phase_low = fit1->GetParameter(2)-90;
      phase_up  = fit1->GetParameter(2)+90;
      cosfit->SetParLimits(2,phase_low,phase_up);
    } 
    else {
      cosfit = new TF1("cosfit","(-4.75)*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))))+ [2]",1,8);
      cosfit->SetParameter(0,-(fit1->GetParameter(0)));
      cosfit->SetParameter(1,-(fit1->GetParameter(1)));
      cosfit->SetParameter(2,-(fit1->GetParameter(2)));

    }
    
  }


   /*Draw OUT values*/
  TGraphErrors* grp_out  = new TGraphErrors(size,x,valuesout,errx,errorsout);
  grp_out ->SetMarkerSize(0.8);
  grp_out ->SetMarkerStyle(21);
  grp_out ->SetMarkerColor(kRed);
  grp_out ->SetLineColor(kRed);

  grp_out->Fit(usefit,"EM");
  TF1* fit2 = grp_out->GetFunction(usefit);
  fit2->DrawCopy("same");
  fit2->SetLineColor(kRed);

  /* Draw IN+OUT values */
  for(Int_t i =0;i<size;i++){
    valuesum[i]=(valuesin[i]+valuesout[i])/2;
    valueerror[i]= sqrt(pow(errorsin[i],2)+pow(errorsout[i],2))/2;
  }

  TGraphErrors* grp_sum  = new TGraphErrors(size,x,valuesum,errx,valueerror);
  grp_sum ->SetMarkerSize(0.8);
  grp_sum ->SetMarkerStyle(21);
  grp_sum ->SetMarkerColor(kGreen-2);
  grp_sum ->SetLineColor(kGreen-2);

  grp_sum->Fit("pol0");
  TF1* fit3 = grp_sum->GetFunction("pol0");
  fit3->DrawCopy("same");
  fit3->SetLineColor(kGreen-2);

  TMultiGraph * grp = new TMultiGraph();
  grp->Add(grp_in);
  grp->Add(grp_out);
  grp->Add(grp_sum);
  grp->Draw("AP");

  grp->SetTitle("");
  grp->GetXaxis()->SetTitle("Octant");
  grp->GetXaxis()->CenterTitle();
  grp->GetYaxis()->SetTitle("Asymmetry (ppm)");
  grp->GetYaxis()->CenterTitle();
  grp->GetYaxis()->SetTitleOffset(0.7);
  grp->GetXaxis()->SetTitleOffset(0.8);

  TLegend *legend = new TLegend(0.1,0.83,0.2,0.99,"","brNDC");
  legend->AddEntry(grp_in, "IHWP-IN", "p");
  legend->AddEntry(grp_out, "IHWP-OUT", "p");
  legend->AddEntry(grp_sum, "(IN+OUT)/2", "p");
  legend->SetFillColor(0);
  legend->Draw("");

 

  TPaveStats *stats1 = (TPaveStats*)grp_in->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats2 = (TPaveStats*)grp_out->GetListOfFunctions()->FindObject("stats");
  TPaveStats *stats3 = (TPaveStats*)grp_sum->GetListOfFunctions()->FindObject("stats");
  stats1->SetTextColor(kBlue);
  stats1->SetFillColor(kWhite); 
  stats2->SetTextColor(kRed);
  stats2->SetFillColor(kWhite); 
  stats3->SetTextColor(kGreen-2);
  stats3->SetFillColor(kWhite); 
  stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); 
  stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);
  stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); 
  stats2->SetY1NDC(0.4);stats2->SetY2NDC(0.65);
  stats3->SetX1NDC(0.8); stats2->SetX2NDC(0.99); 
  stats3->SetY1NDC(0.1);stats3->SetY2NDC(0.35);

  pad2->cd(2);

  /* Draw IN-OUT weighted average values */
  for(Int_t i =0;i<size;i++){
    valuediff[i]=((valuesin[i]/pow(errorsin[i],2)) - (valuesout[i]/pow(errorsout[i],2))) /((1.0/pow(errorsin[i],2)) + (1.0/pow(errorsout[i],2)));
    errordiff[i]= sqrt(1.0/((1.0/(pow(errorsin[i],2)))+(1.0/pow(errorsout[i],2))));
  }

  TGraphErrors* grp_diff  = new TGraphErrors(size,x,valuediff,errx,errordiff);
  grp_diff ->SetMarkerSize(0.8);
  grp_diff ->SetMarkerStyle(21);
  grp_diff ->SetMarkerColor(kMagenta-2);
  grp_diff ->SetLineColor(kMagenta-2);

  if(usefit == "cosfit"){
    usefit = "cosfit_diff";
    if(usefit1){
      cosfit_diff = new TF1("cosfit_diff"," (-4.75)*[0]*sin((pi/180)*[1])*(sin((pi/180)*[2])*cos((pi/180)*(45*(x-1))) - cos((pi/180)*[2])*sin((pi/180)*(45*(x-1))))+ [3]",1,8);
      cosfit_diff->SetParLimits(0,0,99999);
      cosfit_diff->SetParLimits(1,0,99999);
      cosfit_diff->SetParLimits(1,-180,180);
      cosfit_diff->SetParLimits(2,-180,180);
    } 
    else {
      cosfit_diff = new TF1("cosfit_diff","(-4.75)*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))))+ [2]",1,8);
    }
  } 
  else usefit = "pol0";

  grp_diff->Fit(usefit,"EM");
  grp_diff->SetTitle("Graph of IN-OUT");
  TF1* fit4 = grp_diff->GetFunction(usefit);
  if(fit4 != NULL){
    fit4->DrawCopy("same");
    fit4->SetLineColor(kMagenta-2);
  }

  grp_diff->Draw("AP");

//   if(usefit == "cosfit_diff" & !usefit1){
//     /*Draw the individual X, Y parts*/
//     TF1 * Ah = new TF1("Ah", Form("-4.75*%f*cos((pi/180)*(45*(x-1))) + %f",(fit4->GetParameter(0)),(fit4->GetParameter(2))),1,8);
//     Ah->SetLineColor(kBlack);
//     Ah->SetLineStyle(7);
//     Ah->Draw("same");

//     TF1 * Av = new TF1("Av", Form("4.75*%f*sin((pi/180)*(45*(x-1))) + %f",(fit4->GetParameter(1)),(fit4->GetParameter(2))),1,8);
//     Av->SetLineColor(kBlack);
//     Av->SetLineStyle(7);
//     Av->Draw("same");
//   }

  pad->Modified();
  pad->Update();
}


void QwTransverseGUI::plot_pv_ph_in_slugs(){

  TVectorD x_i;
  TVectorD xerr_i;
  TVectorD x_o;
  TVectorD xerr_o;
  TVectorD p0_i;
  TVectorD p0_ie;
  TVectorD p0_o;
  TVectorD p0_oe;
  TVectorD p1_i;
  TVectorD p1_ie;
  TVectorD p1_o;
  TVectorD p1_oe;
  Double_t fit_value0;
  Double_t fit_error0;
  Double_t fit_value1;
  Double_t fit_error1;
  Double_t get_p0;
  Double_t set_p0=0.0;
  Double_t get_p1=0.0;
  Double_t set_p1=0.0;
  Double_t get_p2=0.0;
  Double_t set_p2=0.0;
  Double_t dummy=0;
  Int_t ki =0;
  Int_t ko =0;

  for(Int_t i=slug_first;i<slug_last+1;i++){
    fit_octants_data(quartz_bar_sum,i,"in",set_p0, &get_p0,set_p1, &get_p1,set_p2, &get_p2,
		     &fit_value0,&fit_error0,&fit_value1,&fit_error1);
    
    if(fit_value0 == 0 && fit_error0 == 0 && fit_value1 == 0 && fit_error1 == 0) {
      /*do nothing. The slug number is not in the selected list*/
    } else {
      x_i.ResizeTo(ki+1);    x_i.operator()(ki)=i;
      xerr_i.ResizeTo(ki+1); xerr_i.operator()(ki)=0.0;
      p0_i.ResizeTo(ki+1);   p0_i.operator()(ki)=fit_value0*100;
      p0_ie.ResizeTo(ki+1);  p0_ie.operator()(ki)=fit_error0*100;
      p1_i.ResizeTo(ki+1);   p1_i.operator()(ki)=fit_value1*100;
      p1_ie.ResizeTo(ki+1);  p1_ie.operator()(ki)=fit_error1*100;

      ki++;
    }
    fit_value0 = 0.0; 
    fit_error0 = 0.0;
    fit_value1 = 0.0;
    fit_error1 = 0.0;

    fit_octants_data(quartz_bar_sum,i,"out",get_p0,&dummy,get_p1,&dummy,get_p2,&dummy,
		     &fit_value0,&fit_error0,&fit_value1,&fit_error1);

    if(fit_value0 == 0 && fit_error0 == 0 && fit_value1 == 0 && fit_error1 == 0) {
      /*do nothing. The slug number is not in the selected list*/
    } else {
      x_o.ResizeTo(ko+1);    x_o.operator()(ko)=i;
      xerr_o.ResizeTo(ko+1); xerr_o.operator()(ko)=0.0;
      p0_o.ResizeTo(ko+1);   p0_o.operator()(ko)=fit_value0*100;
      p0_oe.ResizeTo(ko+1);  p0_oe.operator()(ko)=fit_error0*100;
      p1_o.ResizeTo(ko+1);   p1_o.operator()(ko)=fit_value1*100;
      p1_oe.ResizeTo(ko+1);  p1_oe.operator()(ko)=fit_error1*100;
      ko++;
    }
  }
 
  if(ko==0 && ki==0) {std::cout<<"No data!"<<std::endl;
    TString text2 = "Unable to find regressed values for the given slug range.";
    TText*t2 = new TText(0.06,0.5,text2);
    t2->SetTextSize(0.05);
    t2->Draw();
  }
  else{
    
    /*Draw p1 values*/
    /*Create a canvas*/
    TString fit = "-4.74*( P_{V}*cos#phi -  P_{H}*sin#phi ) + constant";
    TString title = Form("Variation of transverse polarization in slugs %i to %i from fit %s",
			 slug_first,slug_last,fit.Data());

    TPad* pad = (TPad*)(gPad->GetMother());
    pad->cd();
    
    TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
    TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
    pad1->SetFillColor(20);
    pad1->Draw();
    pad2->Draw();
    
    
    pad1->cd();
    TLatex*t1 = new TLatex(0.1,0.5,title);
    t1->SetTextSize(0.4);
    t1->Draw();
    
    pad2->cd();
    pad2->Divide(1,2);

    pad2->cd(1);
    draw_plots(p0_i, p0_o,p0_ie, p0_oe, x_i, xerr_i,x_o, xerr_o,ko,ki,"Y");

    pad2->cd(2);
    draw_plots(p1_i, p1_o,p1_ie, p1_oe, x_i, xerr_i,x_o, xerr_o,ko,ki,"X");
 
    pad->Modified();
    pad->Update();
    pad->Print(Form("transverse_in_slugs_%i_%i.png",slug_first,slug_last));
    pad->Print(Form("transverse_in_slugs_%i_%i.C",slug_first,slug_last));

  }
}

/*A function to get data from the database for the octants, fit them using the 
  transverse fit and output P_V and P_H*/
void  QwTransverseGUI::fit_octants_data(TString devicelist[],Int_t slug, TString ihwp, 
					Double_t set_p0, Double_t *get_p0, 
					Double_t set_p1, Double_t *get_p1,
					Double_t set_p2, Double_t *get_p2,
					Double_t *fit_value0,Double_t *fit_error0,
					Double_t *fit_value1,Double_t *fit_error1){
  
  Bool_t ldebug = false;
  Bool_t empty  = false;
  TString query;
  TSQLStatement* stmt = NULL;
  Double_t value[8];
  Double_t error[8];
  Double_t x[8];
  Double_t xerr[8];

 /*set X location and clear the other arrays*/
  for(Int_t i =0;i<8;i++){
    x[i]         = i+1;
    xerr[i]      = 0.0;
    value[i]     = 0.0;
    error[i]     = 0.0;
  }
  std::cout<<"Extracting average asymmetries from slug "<<slug<<std::endl;
  for(Int_t i=0 ; i<8 ;i++){
    if(ldebug) printf("Getting data for %10s ", devicelist[i].Data());
    
    query = data_query(Form("%s",devicelist[i].Data()),slug,ihwp);
    
    stmt = db->Statement(query,100);
    if(!stmt)  {
      db->Close();
      exit(1);
    }
    /* process statement */
    if (stmt->Process()) {
      /* store result of statement in buffer*/
      stmt->StoreResult();
      while (stmt->NextResultRow()) {
	value[i] = (Double_t)(stmt->GetDouble(0))*1e6; // convert to  ppm
	error[i] = (Double_t)(stmt->GetDouble(1))*1e6; // ppm
	if(ldebug) printf(" value = %3.3f +- %3.3f [ppm]\n", value[i], error[i]);
	if((error[i]==0) or (stmt->IsNull(0))) {
	  empty = true;
	  break;
	}
      }
    }
    delete stmt;    
  }

  /* Now fit the results if they are not empty */
  /* Define the cosine fit. Use the p0, p1 and p2 for initialization and limits of this fit.
     We dont't want the phase to change with the IHWP. The sign of the amplitude is the onlything
     that should change with IHWP. But we will allow a +-90 degree leverage for the phase to see 
     if fo some reason, changing the IHWP has changed the verticle to horizontal transverse
     or vise versa.
  */
  TF1 * cosfit = new TF1("cosfit","(-4.75)*([0]*cos((pi/180)*(45*(x-1))) - [1]*sin((pi/180)*(45*(x-1))))+ [2]",1,8);


  /*Without loss of generality, I picked IHWP IN as the first fit. So then I need to
    use the results from that case to initialize IHWP OUT case. Since I use the same function 
    in both cases, I have to use and if statement to check the ihwp state.*/
  if(ihwp == "out"){
    cosfit->SetParameter(2,-set_p2);
    cosfit->SetParameter(0,set_p0);
    cosfit->SetParameter(1,set_p1);
  } 


  if(!empty){
    if(ldebug) std::cout<<"Fit data"<<std::endl;
    

    TGraphErrors* grp1  = new TGraphErrors(8,x,value,xerr,error);
    grp1->Fit("cosfit","QVLEM");
    TF1* fit1 = grp1->GetFunction("cosfit");
    if(fit1==NULL){
      std::cout<<"Empty fit."<<std::endl;
    } else {
      
      /*Get the fit parameters from the IHWP IN fit to initialize IHWP OUT case*/
      if(ihwp == "in") {
	*get_p0 = fit1->GetParameter(0);
	*get_p2 = fit1->GetParameter(1);
	*get_p1 = fit1->GetParameter(2);
      }
      
      *fit_value0 = fit1->GetParameter(0); //p0
      *fit_error0 = fit1->GetParError(0); //p0 error
      *fit_value1 = fit1->GetParameter(1); //p1
      *fit_error1 = fit1->GetParError(1); //p1 error

      if(ldebug) {
	std::cout<<"p0 =" <<*fit_value0<< "+-" <<*fit_error0
		 <<" p1 =" <<*fit_value1<< "+-" <<*fit_error1 << std::endl;
      }
      
    }
  }

}



 void  QwTransverseGUI::draw_plots(TVectorD value_in, TVectorD value_out,
		 TVectorD error_in, TVectorD error_out, 
		 TVectorD valuex_in, TVectorD errorx_in, 
		 TVectorD valuex_out, TVectorD errorx_out, 
		 Int_t ko, Int_t ki, TString type){



   TString title;
   TString ytitle;

   if(type == "X") {
     title = "P_{H}";
     ytitle = "P_{H} (%)";
   }
   if(type == "Y") {
     title = "P_{V}";
     ytitle = "P_{V} (%)";
   }


   TMultiGraph * grp = new TMultiGraph();
   TLegend *legend = new TLegend(0.1,0.83,0.2,0.99,"","brNDC");
   TGraphErrors* grp_out = NULL;
   TGraphErrors* grp_in = NULL;
   TPaveStats *stats1;
   TPaveStats *stats2;
   

   if(ko!=0){
      grp_out  = new TGraphErrors(valuex_out,value_out,errorx_out,error_out);
      grp_out ->SetMarkerSize(0.8);
      grp_out ->SetMarkerStyle(21);
      grp_out ->SetMarkerColor(kRed);
      grp_out ->SetLineColor(kRed);
      grp_out ->Fit("pol0");
      TF1* fit1 = grp_out->GetFunction("pol0");
      fit1->SetParNames(title);
      if(fit1==NULL) {
	std::cout<<"Fit is empty for OUT data"<<std::endl;
	exit(1);
      }
      fit1->DrawCopy("same");
      fit1->SetLineColor(kRed);
      fit1->SetLineStyle(2);
      grp->Add(grp_out);
      legend->AddEntry(grp_out, "IHWP-OUT", "p");
    

    if(ki!=0){
      grp_in  = new TGraphErrors(valuex_in,value_in,errorx_in,error_in);
      grp_in ->SetMarkerSize(0.8);
      grp_in ->SetMarkerStyle(21);
      grp_in ->SetMarkerColor(kBlue);
      grp_in ->SetLineColor(kBlue);
      grp_in ->Fit("pol0");    
      TF1* fit2 = grp_in->GetFunction("pol0");
      fit2->SetParNames(title);

      if(fit2==NULL) {
	std::cout<<"Fit is empty for IN data"<<std::endl;
	exit(1);
      }
      fit2->DrawCopy("same");
      fit2->SetLineColor(kBlue);
      fit2->SetLineStyle(2);
      grp->Add(grp_in);
      legend->AddEntry(grp_in, "IHWP-IN", "p");
    }

    if(ko!=0 or ki!=0) {

      grp->Draw("AP");
      grp->GetXaxis()->SetTitle("slug number");
      grp->GetYaxis()->SetTitle(ytitle);
      grp->GetYaxis()->CenterTitle();
      grp->GetXaxis()->CenterTitle();
      legend->SetFillColor(0);
      legend->Draw();
     }

    if(ki!=0){
      stats1 = (TPaveStats*)grp_in->GetListOfFunctions()->FindObject("stats");
      stats1->SetTextColor(kBlue);
      stats1->SetFillColor(kWhite); 
      stats1->SetX1NDC(0.8); stats1->SetX2NDC(0.99); 
      stats1->SetY1NDC(0.7);stats1->SetY2NDC(0.95);
    }

    if(ko!=0){
      stats2 = (TPaveStats*)grp_out->GetListOfFunctions()->FindObject("stats");
      stats2->SetTextColor(kRed);
      stats2->SetFillColor(kWhite); 
      stats2->SetX1NDC(0.8); stats2->SetX2NDC(0.99); 
      stats2->SetY1NDC(0.4);stats2->SetY2NDC(0.65);
    }
   }
}
