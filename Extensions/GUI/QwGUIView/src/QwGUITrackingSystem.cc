// Name  :   QwGUITrackingSystem.cc
// Author:   Jeong Han Lee
// email :   jhlee@jlab.org


#include "QwGUITrackingSystem.h"

ClassImp(QwGUITrackingSystem);

enum QwGUITrackingSystemIndentificator {
  BA_CLOSECANVAS, BA_HELP, BA_EXIT,
  BA_RUN_NUMBER, BA_CHWIRE_NUMBER,
  BA_TEXTBUTTONONE, BA_TEXTBUTTONTWO, BA_TEXTBUTTONTHR,  BA_TEXTBUTTONFOR,  BA_TEXTBUTTONFIV, 
  BA_TEXTBUTTONSIX,
  BA_EVENT_NUMBER_START, BA_EVENT_NUMBER_END, 
  BA_REGION_ONE,  BA_REGION_TWO,  BA_REGION_THR,
  BA_TIME, BA_RAWTIME, BA_CHANNEL, BA_WIRE,
  BA_COMBO,
  BA_CHKBUTTON_ONE,
  BA_LISTBOX_ON,
};



const char *const QwGUITrackingSystem::request_types[REQUEST_TYPE_NUM]=
  {  "Time"
   , "RawTime"
  };


const char *const QwGUITrackingSystem::select_types[SELECT_TYPE_NUM]=
  {  "channel"
   , "wire"
  };


const char *const QwGUITrackingSystem::hist1_types[HIST_ONED_NUM]=
  {"Time at channel"
   , "RawTime at channel"
   , "Time at wire"
   , "RawTime at wire"
  };




QwGUITrackingSystem::QwGUITrackingSystem(const TGWindow *main, TGCompositeFrame *parent, UInt_t w, UInt_t h)
{ 

  gClient->GetColorByName("green", green);
  gClient->GetColorByName("red",   red);


  canvas = NULL;
  progress_bar = NULL;

  run_number     = 1567;
  event_range[0] = 0;
  event_range[1] = 0;

  default_event_range[0] = 0;
  //  default_event_range[1] = 1000;
  default_event_range[1] = 1000;

  total_physics_event_number = 0;

  for(Short_t i=0;i<BOT_BUTTON_NUM;i++) b_button  [i] = NULL;
  for(Short_t i=0;i<HFRAME_NUM;    i++) hframe    [i] = NULL;
  for(Short_t i=0;i<GFRAME_NUM;    i++) gframe    [i] = NULL;
  for(Short_t i=0;i<G0HFRAME_NUM;  i++) g0hframe  [i] = NULL;
  for(Short_t i=0;i<G0VFRAME_NUM;  i++) g0vframe  [i] = NULL;
  for(Short_t i=0;i<NUM_ENTRY_NUM; i++) num_entry [i] = NULL;
  for(Short_t i=0;i<LABEL_NUM;     i++) num_label [i] = NULL;
  for(Short_t i=0;i<TXT_BUTTON_NUM;i++) txt_button[i] = NULL;
  for(Short_t i=0;i<CHK_BUTTON_NUM;i++) chk_button[i] = NULL;
  for(Short_t i=0;i<BUTTON_GRP_NUM;i++) btn_group [i] = NULL;
  for(Short_t i=0;i<RAI_BUTTON_NUM;i++) rad_button[i] = NULL;
  
  event_range_status   = true;
  load_rootfile_status = false;

  root_file       = NULL;
  qwhit_tree      = NULL;
  qwhit           = NULL;
  qwhit_container = NULL;



//   for(Short_t i=0;i<RG_ONE_HIST_NUM;i++) hist1_region1[i] = NULL;
//   for(Short_t i=0;i<RG_TWO_HIST_NUM;i++) hist1_region2[i] = NULL;
//   for(Short_t i=0;i<RG_THR_HIST_NUM;i++) hist1_region3[i] = NULL;

  for(Short_t i=0;i<RG_ONE_HIST_NUM;i++) hist2_region1[i] = NULL;
  for(Short_t i=0;i<RG_TWO_HIST_NUM;i++) hist2_region2[i] = NULL;
  for(Short_t i=0;i<RG_THR_HIST_NUM;i++) hist2_region3[i] = NULL;


  for(Short_t j=0;j<HIST_ONED_NUM;j++)
    {
      for(Short_t i=0;i<RG_ONE_HIST_NUM;i++) hist1_region1[j][i] = NULL;
      for(Short_t i=0;i<RG_TWO_HIST_NUM;i++) hist1_region2[j][i] = NULL;
      for(Short_t i=0;i<RG_THR_HIST_NUM;i++) hist1_region3[j][i] = NULL;
    }

//   for(Short_t j=0;j<HIST_TWOD_NUM;j++)
//     {
//       for(Short_t i=0;i<RG_ONE_HIST_NUM;i++) hist2_region1[j][i] = NULL;
//       for(Short_t i=0;i<RG_TWO_HIST_NUM;i++) hist2_region2[j][i] = NULL;
//       for(Short_t i=0;i<RG_THR_HIST_NUM;i++) hist2_region3[j][i] = NULL;
//     }

  d_region  = 3;
  d_request = 0;
  d_select  = 0;
  
  region_status[0]    = true;
  for(Short_t i=1;i<4;i++)  region_status[i] = false;
  direction_status[0] = true;
  for(Short_t i=1;i<7;i++)  direction_status[i] = false;
  plane_status[0]     = true;
  for(Short_t i=1;i<13;i++) plane_status[i] = false;

  CreateFrame(parent,w, h);

};

QwGUITrackingSystem::~QwGUITrackingSystem()
{

  delete    root_file;
  delete    qwhit_container;
  delete    qwhit_tree;
  delete    qwhit;

  delete [] chk_button;
  delete [] txt_button;
  delete [] num_label;
  delete [] num_entry;
  delete [] hframe;
  delete [] gframe;
  delete [] g0hframe;
  delete [] g0vframe;

  delete [] btn_group;
  delete [] rad_button;

  delete    bframe;
  delete [] b_button;


//   delete [] hist1_region1;
//   delete [] hist1_region2;
//   delete [] hist1_region3;

  delete [] hist2_region1;
  delete [] hist2_region2;
  delete [] hist2_region3;

 for(Short_t j=0;j<HIST_ONED_NUM;j++)
    {
      for(Short_t i=0;i<RG_ONE_HIST_NUM;i++) delete hist1_region1[j][i];
      for(Short_t i=0;i<RG_TWO_HIST_NUM;i++) delete hist1_region2[j][i];
      for(Short_t i=0;i<RG_THR_HIST_NUM;i++) delete hist1_region3[j][i];
    }

//  for(Short_t j=0;j<HIST_TWOD_NUM;j++)
//     {
//       for(Short_t i=0;i<RG_ONE_HIST_NUM;i++) hist2_region1[j][i] = NULL;
//       for(Short_t i=0;i<RG_TWO_HIST_NUM;i++) hist2_region2[j][i] = NULL;
//       for(Short_t i=0;i<RG_THR_HIST_NUM;i++) hist2_region3[j][i] = NULL;
//     }

  delete    progress_bar;
  delete    canvas;

};


void 
QwGUITrackingSystem::CreateFrame(TGCompositeFrame *parent, UInt_t w, UInt_t h)
{
  
  UInt_t   ten_percent_height = (UInt_t) 0.1*h;
  bframe = new TGHorizontalFrame(parent, w, ten_percent_height);
  parent -> AddFrame(bframe, new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 1,1,1,1));

  
  b_button[0] = new TGTextButton(bframe, "Close Canvas", BA_CLOSECANVAS);
  b_button[1] = new TGTextButton(bframe, "Help",         BA_HELP);
  b_button[2] = new TGTextButton(bframe, "Exit",         BA_EXIT);

  bframe -> AddFrame(b_button[0], new TGLayoutHints(kLHintsExpandX, 2,2,1,1));
  bframe -> AddFrame(b_button[1], new TGLayoutHints(kLHintsExpandX, 2,2,1,1));
  bframe -> AddFrame(b_button[2], new TGLayoutHints(kLHintsExpandX, 2,2,1,1));


  
  hframe [0] = new TGHorizontalFrame(parent);
  parent  -> AddFrame(hframe[0], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,10,10,10,10));
  hframe [1] = new TGHorizontalFrame(parent);
  parent  -> AddFrame(hframe[1], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,10,10,10,10));
  
  
  Short_t digital_width     = 10;
  Short_t run_number_min    = 1;
  Short_t event_num_min     = 0;

  // hframe [0] 
  num_label [0]  = new TGLabel(hframe[0], "Run #");
  num_entry [0]  = new TGNumberEntry(hframe[0], run_number, digital_width, BA_RUN_NUMBER, 
				     TGNumberFormat::kNESInteger,
				     TGNumberFormat::kNEAPositive,
				     TGNumberFormat::kNELLimitMin, run_number_min);
  chk_button[0] = new TGCheckButton (hframe[0], "Event Range", BA_CHKBUTTON_ONE);
  num_label [1] = new TGLabel(hframe[0], "Evt #");
  
  digital_width = 4;
  num_entry [1] = new TGNumberEntry(hframe[0], default_event_range[0], digital_width, BA_EVENT_NUMBER_START, 
				    TGNumberFormat::kNESInteger,
				    TGNumberFormat::kNEANonNegative,
				    TGNumberFormat::kNELLimitMin, event_num_min);
  num_entry [2] = new TGNumberEntry(hframe[0], default_event_range[1], digital_width, BA_EVENT_NUMBER_END, 
				    TGNumberFormat::kNESInteger,
				    TGNumberFormat::kNEANonNegative,
				    TGNumberFormat::kNELLimitMin, event_num_min);
  
  hframe    [0] -> AddFrame(num_label [0], new TGLayoutHints(kLHintsCenterX,0,0,4,0));
  hframe    [0] -> AddFrame(num_entry [0], new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  hframe    [0] -> AddFrame(chk_button[0], new TGLayoutHints(kLHintsCenterX,4,0,4,0));
  hframe    [0] -> AddFrame(num_label [1], new TGLayoutHints(kLHintsCenterX,0,0,4,0));
  hframe    [0] -> AddFrame(num_entry [1], new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  hframe    [0] -> AddFrame(num_entry [2], new TGLayoutHints(kLHintsExpandX,0,0,0,0));

  //  num_entry [1] -> SetState(false);
  //  num_entry [2] -> SetState(false);

  chk_button[0] -> SetState(kButtonDown);
  chk_button[0] -> SetToolTipText("Turn on/off Select Range");


  //
  // hframe [1] 
  //
  progress_bar  = new TGHProgressBar(hframe[1], TGProgressBar::kFancy, 180);
  txt_button[0] = new TGTextButton(hframe[1], "Load Hit-based ROOT file", BA_TEXTBUTTONONE);
  txt_button[1] = new TGTextButton(hframe[1], "Manipulate Histograms",    BA_TEXTBUTTONTWO);
  txt_button[2] = new TGTextButton(hframe[1], "Close the ROOT file",      BA_TEXTBUTTONTHR);

  txt_button[0] -> SetToolTipText(" Not yet decided to be here or not.\n Just get a glimpse of what advanced Tracking GUI might be like in the future.\n Stand-alone or inside QwGUIMain? I am not sure....");

  hframe    [1] -> AddFrame(progress_bar,  new TGLayoutHints(kLHintsCenterX,4,0,0,0));
  hframe    [1] -> AddFrame(txt_button[0], new TGLayoutHints(kLHintsExpandX,0,1,4,1));
  hframe    [1] -> AddFrame(txt_button[1], new TGLayoutHints(kLHintsExpandX,0,0,4,0));
  hframe    [1] -> AddFrame(txt_button[2], new TGLayoutHints(kLHintsExpandX,0,8,4,1));

  //  progress_bar -> ShowPosition(true, false, "Read : %.0f percent");
  progress_bar -> Percent(true);
  progress_bar -> ShowPos(true);
  progress_bar -> SetBarColor("red");
  progress_bar -> SetFillType(TGProgressBar::kBlockFill);

  gframe    [0] = new TGGroupFrame(parent, "ReadHits.C prototype GUI version and test TSuperCanvas", kVerticalFrame);
  parent       -> AddFrame(gframe[0],  new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));

  g0hframe  [0] = new TGHorizontalFrame(gframe[0]);
  g0hframe  [1] = new TGHorizontalFrame(gframe[0]);
  g0hframe  [2] = new TGHorizontalFrame(gframe[0]);
  gframe    [0] -> AddFrame(g0hframe[0], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));
  gframe    [0] -> AddFrame(g0hframe[1], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));
  gframe    [0] -> AddFrame(g0hframe[2], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));

  //
  // hframe[1] - g0hframe[0]
  //
  btn_group [0] = new TGButtonGroup(g0hframe[0],"Select Region", kHorizontalFrame);
  g0hframe  [0] -> AddFrame(btn_group[0], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));

  rad_button[0] = new TGRadioButton(btn_group[0], "Region 1 (GEM)", BA_REGION_ONE);
  rad_button[1] = new TGRadioButton(btn_group[0], "Region 2 (HDC)", BA_REGION_TWO);
  rad_button[2] = new TGRadioButton(btn_group[0], "Region 3 (VDC)", BA_REGION_THR);

  rad_button[2] -> SetState(kButtonDown);
  btn_group [0] -> SetLayoutHints(new TGLayoutHints(kLHintsCenterX,0,0,10,0));
   
  //
  // hframe[1] - g0hframe[1]
  //
  btn_group [1] = new TGButtonGroup  (g0hframe[1], "", kVerticalFrame);
  num_label [1] = new TGLabel        (g0hframe[1], "Histogram with");
  btn_group [2] = new TGButtonGroup  (g0hframe[1], "", kVerticalFrame);
  g0vframe  [0] = new TGVerticalFrame(g0hframe[1]);
  
  g0hframe  [1] -> AddFrame(btn_group[1], new TGLayoutHints(kLHintsCenterX,0,0,0,0));
  g0hframe  [1] -> AddFrame(num_label[1], new TGLayoutHints(kLHintsCenterY,0,0,40,0));
  g0hframe  [1] -> AddFrame(btn_group[2], new TGLayoutHints(kLHintsCenterX,0,0,0));
  g0hframe  [1] -> AddFrame(g0vframe [0], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));
  

  rad_button[3] = new TGRadioButton(btn_group[1], "Time",    BA_TIME);
  rad_button[4] = new TGRadioButton(btn_group[1], "RawTime", BA_RAWTIME);
  rad_button[5] = new TGRadioButton(btn_group[2], "Channel", BA_CHANNEL);
  rad_button[6] = new TGRadioButton(btn_group[2], "Wire",    BA_WIRE);

  rad_button[3] -> SetState(kButtonDown);
  rad_button[5] -> SetState(kButtonDown);

  btn_group [1] -> SetLayoutHints(new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  btn_group [2] -> SetLayoutHints(new TGLayoutHints(kLHintsExpandX,0,0,0,0));

  
  ch_wire_number = 10;
  digital_width  = 4;
  num_entry [3]  = new TGNumberEntry(g0vframe[0], ch_wire_number, digital_width, BA_CHWIRE_NUMBER, 
				     TGNumberFormat::kNESInteger,
				     TGNumberFormat::kNEAPositive,
				     TGNumberFormat::kNELLimitMin, 0);
  txt_button[3]  = new TGTextButton(g0vframe[0], "PLOT",   BA_TEXTBUTTONFOR);
  g0vframe  [0] -> AddFrame(num_entry[3],  new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,20,0,10,0));
  g0vframe  [0] -> AddFrame(txt_button[3], new TGLayoutHints(kLHintsExpandX,20,1,4,1));


  //
  // g0hframe[2]
  //
  txt_button[4]  = new TGTextButton(g0hframe[2], "Event Number VS Wire Number",        BA_TEXTBUTTONFIV);
  txt_button[5]  = new TGTextButton(g0hframe[2], "Hitted Wire Number VS Event Number", BA_TEXTBUTTONSIX);
  
  g0hframe[2] -> AddFrame(txt_button[4], new TGLayoutHints(kLHintsExpandX,0,1,4,1));
  g0hframe[2] -> AddFrame(txt_button[5], new TGLayoutHints(kLHintsExpandX,0,1,4,1));

  //
  // all buttons and others are associated with "this", thus their signals are caputred by "ProcessMessage"
  //
  for(Short_t i=0;i<BOT_BUTTON_NUM;i++) b_button  [i] -> Associate(this);
  for(Short_t i=0;i<NUM_ENTRY_NUM ;i++) num_entry [i] -> Associate(this);
  for(Short_t i=0;i<TXT_BUTTON_NUM;i++) txt_button[i] -> Associate(this);
  for(Short_t i=0;i<CHK_BUTTON_NUM;i++) chk_button[i] -> Associate(this);
  for(Short_t i=0;i<RAI_BUTTON_NUM;i++) rad_button[i] -> Associate(this);

  ResetButtons(true);

  return;
  
};



void
QwGUITrackingSystem::ManipulateHistograms()
{
  ResetProgressBar();
  SetEventRange();



  Int_t    nhit        = 0;
  UInt_t    ev_i       = 0; 
  Int_t    hit_i       = 0;

  Int_t    crate       = 0;
  Int_t    module      = 0;
  Int_t    channel     = 0;
  Int_t    hitnumber   = 0;
  Int_t    hitnumber_r = 0;
  
  Short_t  region      = 0;
  Short_t  package     = 0;
  Short_t  direction   = 0;
  Int_t    plane       = 0;
  Int_t    element      = 0; //  Trace # for R1; wire # for R2 & R3; PMT # for others

  Double_t rawtime  = 0.0;
  Double_t time     = 0.0;
  Double_t time_resolution    = 0.0;
  Double_t distance           = 0.0;
  Double_t position           = 0.0;
  Double_t residual           = 0.0;
  Double_t z_position         = 0.0;
  Double_t r_position         = 0.0;
  Double_t phi_position       = 0.0;
  Double_t spatial_resolution = 0.0;
  Double_t track_resolution   = 0.0;
  
  //   Short_t  tmp         = 0;
   
  
  Double_t offset      = -0.5;
  
  Int_t    xbinN[3]    = {0, 0, 0};
  Int_t    ybinN[3]    = {RG_ONE_WIRE_NUM, RG_TWO_WIRE_NUM, RG_THR_WIRE_NUM}; // more than the number of wires

  Int_t    wire_axis_range[3][2];
  Int_t    event_axis_range[3][2];

  Int_t    select_number = 0;
  select_number =  num_entry[3] -> GetNumber();
   
  for(Short_t i=0; i<3; i++)
    {
      wire_axis_range[i][0] = offset;
      wire_axis_range[i][1] = ybinN[i]+offset;
      
      event_axis_range[i][0] = event_range[0]+offset;
      event_axis_range[i][1] = event_range[1]+offset;

      xbinN[i] = event_axis_range[i][1] -  event_axis_range[i][0];

      region = i+1;
      if( region==1 ) 
	{

	  for(Short_t j=0; j<RG_ONE_HIST_NUM; j++)
	    {
	      direction = j+1;
	      sprintf(text_buffer, "H1RG%dPL%d", region, plane);
	      CheckOldObject(text_buffer);
	      hist1_region1[4][j]=  new TH1D(text_buffer,
					     Form("Number of Events per Wire of RG %d DIR %d", region, direction), 
					     ybinN[i], wire_axis_range[i][0], wire_axis_range[i][1]);

	      sprintf(text_buffer, "H2RG%dPL%d", region, plane);
	      CheckOldObject(text_buffer);
	      hist2_region1[j]=  new TH2I(text_buffer,
					  Form("Number of Events per Wire of RG %d DIR %d", region, direction), 
					  xbinN[i], event_axis_range[i][0], event_axis_range[i][1],
					  ybinN[i], wire_axis_range [i][0], wire_axis_range [i][1]);

	      for(Short_t k=0; k<HIST_ONED_NUM; k++)
		{
		  sprintf(text_buffer, "RG%dDR%d_%d", region, direction, k);
		  CheckOldObject(text_buffer);
		  hist1_region1[k][j]=  new TH1D(text_buffer,
						 Form("%s-%d  RG %d DIR %d", hist1_types[k], select_number, region, direction), 
						 BINNUMBER, 0, 0); 
		  hist1_region1[k][j] -> SetDefaultBufferSize(BUFFERSIZE);
		  //		  printf("TH1D %s\n", text_buffer);
		}
	    }
	}
      if( region==2 )
	{
	  for(Short_t j=0; j<RG_TWO_HIST_NUM; j++)
	    {
	      plane = j+1; 
	      sprintf(text_buffer, "H1RG%dPL%d", region, plane);
	      CheckOldObject(text_buffer);
	      hist1_region2[4][j]=  new TH1D(text_buffer,
					     Form("Number of Events per Wire of RG %d PL %d", region, plane), 
					     ybinN[i], wire_axis_range[i][0], wire_axis_range[i][1]);
	      sprintf(text_buffer, "H2RG%dPL%d", region, plane);
	      CheckOldObject(text_buffer);
	      hist2_region2[j]=  new TH2I(text_buffer,
					  Form("Number of Events per Wire of RG %d PL %d", region, plane), 
					  xbinN[i], event_axis_range[i][0], event_axis_range[i][1],
					  ybinN[i], wire_axis_range [i][0], wire_axis_range [i][1]);

	      for(Short_t k=0; k<HIST_ONED_NUM; k++)
		{

		  sprintf(text_buffer, "RG%dPL%d_%d", region, plane, k);
		  CheckOldObject(text_buffer);
		  hist1_region2[k][j]=  new TH1D(text_buffer,
						 Form("%s-%d  RG %d PL %d", hist1_types[k], select_number, region, plane), 
						 BINNUMBER, 0, 0); 
		  hist1_region2[k][j] -> SetDefaultBufferSize(BUFFERSIZE);
		  //		  printf("TH1D %s\n", text_buffer);
		}
	       
	    }
	}
      
      if( region==3 ) 
	{
	  for(Short_t j=0; j<RG_THR_HIST_NUM; j++)
	    {
	      plane = j+1;
	      sprintf(text_buffer, "H1RG%dPL%d", region, plane);
	      CheckOldObject(text_buffer);
 	      hist1_region3[4][j]=  new TH1D(text_buffer,
					     Form("Number of Events per Wire of RG %d PL %d", region, plane), 
					     ybinN[i], wire_axis_range[i][0], wire_axis_range[i][1]);
	      sprintf(text_buffer, "H2RG%dPL%d", region, plane);
	      CheckOldObject(text_buffer);
	      hist2_region2[j]=  new TH2I(text_buffer,
					  Form("Number of Events per Wire of RG %d PL %d", region, plane), 
					  xbinN[i], event_axis_range[i][0], event_axis_range[i][1],
					  ybinN[i], wire_axis_range [i][0], wire_axis_range [i][1]);

	      for(Short_t k=0; k<HIST_ONED_NUM; k++)
		{
		  sprintf(text_buffer, "RG%dPL%d_%d", region, plane, k);
		  CheckOldObject(text_buffer);
		  hist1_region3[k][j]=  new TH1D(text_buffer,
						 Form("%s-%d  RG %d PL %d", hist1_types[k], select_number, region, plane), 
						 BINNUMBER, 0, 0); 
		  hist1_region3[k][j] -> SetDefaultBufferSize(BUFFERSIZE);
		  //		  printf("TH1D %s\n", text_buffer);
		}
	    }
	}
    }
       



//   region = 0;
//   plane  = 0;
//   direction = 0;

 

  for(ev_i=event_range[0]; ev_i<event_range[1]; ev_i++)
    {
      qwhit_tree -> GetEntry(ev_i);
      nhit = qwhit_container->GetSize();
      //      printf("*** Event %10d\n", ev_i);
      //      printf("    --- %d hits (by QwHitRootContainer)\n", nhit);

      for(hit_i=0; hit_i<nhit; hit_i++)
	{
	  qwhit   = (QwHit*) qwhit_container->GetHit(hit_i);

	  channel = qwhit->GetChannel();
	  element = qwhit->GetElement();
	  time    = qwhit->GetTime();
	  rawtime = qwhit->GetRawTime();

	  region  = qwhit->GetRegion();
	  plane   = qwhit->GetPlane();
	  direction = qwhit->GetDirection();

	  if(region == 1) 
	    {
	      hist1_region1[4][direction-1] -> Fill(element);
	      //	      hist2_region1[direction-1]    -> Fill(ev_i, element);
	    }
	  if(region == 2) 
	    {
	      hist1_region2[4][plane-1] -> Fill(element);
	      //	      hist2_region2[plane-1]    -> Fill(ev_i, element);
	    }
	  if(region == 3) 
	    {
	      hist1_region3[4][plane-1] -> Fill(element);
	      //	      hist2_region3[plane-1]    -> Fill(ev_i, element);
	    }



	  if(region ==1)
	    {
	    }
	  else
	    {
	      if (select_number == channel)
		{
		  if(region == 2) 
		    {
		      hist1_region2[0][plane-1] -> Fill(time);
		      hist1_region2[1][plane-1] -> Fill(rawtime);
		    }
		  if(region == 3)
		    {
		      hist1_region3[0][plane-1] -> Fill(time);
		      hist1_region3[1][plane-1] -> Fill(rawtime);
		    }
		}
 	      if (select_number == element)
 		{
 		  if(region == 2) 
 		    {
 		      hist1_region2[2][plane-1] -> Fill(time);
 		      hist1_region2[3][plane-1] -> Fill(rawtime);
 		    }
 		  if(region == 3)
 		    {
 		      hist1_region3[2][plane-1] -> Fill(time);
 		      hist1_region3[3][plane-1] -> Fill(rawtime);
 		    }
 		}
	    }
	  qwhit->Clear();
	}
      qwhit_container->Clear();
      Float_t inc = 100.0/(Double_t)event_range[1];
      if(ev_i > (Int_t) 90/inc) progress_bar -> SetBarColor("lightblue");
      progress_bar -> Increment(inc);
    }
  
  txt_button[1] -> SetEnabled(false);


  return;
};


void
QwGUITrackingSystem::PlotDraw()
{

  Int_t  region    = 0;
  Int_t  plane     = 0;

  canvas = new TSuperCanvas("rawtime_per_tdc_canvas","time per tdc channel",10,10,1200,800);
//   //  canvas = CheckCanvas(this, canvas, "test", false);
  canvas -> SetWindowSize(1024,648);
 
   
  TLatex *region_tex = NULL;
  region_tex = new TLatex();
  region_tex -> SetTextSize(0.05);
  region_tex -> SetTextColor(kRed);
  region_tex -> SetTextAlign(22);
  
  Short_t padnumber = 0;

  region = GetRegion();
  
  Int_t type = 0;
  if(d_request == 0) //Time
    {
      if(d_select == 0) type = 0; // time at channel
      if(d_select == 1) type = 2; // time at wire
    }
  if(d_request == 1)
    {
      if(d_select == 0) type = 1;  // rawtime at channel
      if(d_select == 1) type = 3; //  rawtime at wire
    }

  if(region == 1)
    {
      region_tex -> DrawLatex(0.5,0.5, "NO Region 1 data was in the ROOT file");
    }

  if(region == 2)
    {
      canvas -> Divide(4,3);
      for(Short_t j=0; j<12; j++)
	{
	  plane     = j+1;
	  padnumber = plane;
	  canvas -> cd(padnumber);
	  // 		  gStyle -> SetOptStat(0);
	  if( hist1_region2[type][plane-1]->GetEntries() != 0)
	    {
	      hist1_region2[type][plane-1] -> SetLineColor(kRed);
	      hist1_region2[type][plane-1] -> GetXaxis()-> SetTitle(Form("%s", request_types[d_request]));
	      hist1_region2[type][plane-1] -> GetYaxis()-> SetTitle("Number of Events");
	      hist1_region2[type][plane-1] -> Draw();
	    }
	  else
	    {
	      region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
	    }
	}
    }
  if( region == 3)
    {
      canvas -> Divide(2,1);
      for(Short_t j=0; j<2; j++)
	{
	  plane     = j+1;
	  padnumber = plane;
	  //	      printf("region 3 pad %d \n", padnumber);
	  canvas -> cd(padnumber);
	  if( hist1_region3[type][plane-1]->GetEntries() != 0)
	    {
	 
	      hist1_region3[type][plane-1] -> SetLineColor(kRed);
	      hist1_region3[type][plane-1] -> GetXaxis()-> SetTitle(Form("%s", request_types[d_request]));
	      hist1_region3[type][plane-1] -> GetYaxis()-> SetTitle("Number of Events");
	      hist1_region3[type][plane-1] -> Draw();
	    }
	  else
	    {
	      region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
	    }
	}

      //   printf("%s\n",Form("%s per %s %d  RG %d PL %d", request_types[d_request], select_types[d_select], get_number, region, plane));
      //   qwhit_tree -> Dump();
      //   qwhit_tree ->Draw("hits.fQwHits.fRawTime", "hits.fQwHits.fElement==4")  ;
    } 
  canvas -> Modified();
  canvas -> Update();
  
  return;
};



void
QwGUITrackingSystem::PlotEventVsWire()
{

  Int_t  region    = 0;
  Int_t  plane     = 0;

  canvas = new TSuperCanvas("rawtime_per_tdc_canvas","time per tdc channel",10,10,1200,800);
//   //  canvas = CheckCanvas(this, canvas, "test", false);
  canvas -> SetWindowSize(1024,648);
 
   
  TLatex *region_tex = NULL;
  region_tex = new TLatex();
  region_tex -> SetTextSize(0.05);
  region_tex -> SetTextColor(kRed);
  region_tex -> SetTextAlign(22);
  
  Short_t padnumber = 0;

  region = GetRegion();
  
  if(region == 1)
    {
      region_tex -> DrawLatex(0.5,0.5, "NO Region 1 data was in the ROOT file");
    }

  if(region == 2)
    {
      canvas -> Divide(4,3);
      for(Short_t j=0; j<12; j++)
	{
	  plane     = j+1;
	  padnumber = plane;
	  canvas -> cd(padnumber);
	  // 		  gStyle -> SetOptStat(0);
	  if( hist1_region2[4][plane-1]->GetEntries() != 0)
	    {
	      hist1_region2[4][plane-1] -> SetLineColor(kRed);
	      hist1_region2[4][plane-1] -> GetXaxis()-> SetTitle("Wire #");
	      hist1_region2[4][plane-1] -> GetYaxis()-> SetTitle("Number of Events");
	      hist1_region2[4][plane-1] -> Draw();
	    }
	  else
	    {
	      region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
	    }
	}
    }
  if( region == 3)
    {
      canvas -> Divide(2,1);
      for(Short_t j=0; j<2; j++)
	{
	  plane     = j+1;
	  padnumber = plane;
	  //	      printf("region 3 pad %d \n", padnumber);
	  canvas -> cd(padnumber);
	  if( hist1_region3[4][plane-1]->GetEntries() != 0)
	    {
	      hist1_region3[4][plane-1] -> SetLineColor(kRed);
	      hist1_region3[4][plane-1] -> GetXaxis()-> SetTitle("Wire #");
	      hist1_region3[4][plane-1] -> GetYaxis()-> SetTitle("Number of Events");
	      hist1_region3[4][plane-1] -> Draw();
	    }
	  else
	    {
	      region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
	    }
	}
    } 
  canvas -> Modified();
  canvas -> Update();
  
  return;
};

void
QwGUITrackingSystem::PlotHittedWireVsEvent()
{

  Int_t  region    = 0;
  Int_t  plane     = 0;

  canvas = new TSuperCanvas("rawtime_per_tdc_canvas","time per tdc channel",10,10,1200,800);
//   //  canvas = CheckCanvas(this, canvas, "test", false);
  canvas -> SetWindowSize(1024,648);
 
   
  TLatex *region_tex = NULL;
  region_tex = new TLatex();
  region_tex -> SetTextSize(0.05);
  region_tex -> SetTextColor(kRed);
  region_tex -> SetTextAlign(22);
  
  Short_t padnumber = 0;

  region = GetRegion();
  
  if(region == 1)
    {
      region_tex -> DrawLatex(0.5,0.5, "NO Region 1 data was in the ROOT file");
    }

  if(region == 2)
    {
      canvas -> Divide(4,3);
      for(Short_t j=0; j<12; j++)
	{
	  plane     = j+1;
	  padnumber = plane;
	  canvas -> cd(padnumber);
	  // 		  gStyle -> SetOptStat(0);
	  if( hist1_region2[4][plane-1]->GetEntries() != 0)
	    {
	      hist1_region2[4][plane-1] -> SetLineColor(kRed);
	      hist1_region2[4][plane-1] -> GetXaxis()-> SetTitle("Wire #");
	      hist1_region2[4][plane-1] -> GetYaxis()-> SetTitle("Number of Events");
	      hist1_region2[4][plane-1] -> Draw();
	    }
	  else
	    {
	      region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
	    }
	}
    }
  if( region == 3)
    {
      canvas -> Divide(2,1);
      for(Short_t j=0; j<2; j++)
	{
	  plane     = j+1;
	  padnumber = plane;
	  //	      printf("region 3 pad %d \n", padnumber);
	  canvas -> cd(padnumber);
	  if( hist1_region3[4][plane-1]->GetEntries() != 0)
	    {
	      hist1_region3[4][plane-1] -> SetLineColor(kRed);
	      hist1_region3[4][plane-1] -> GetXaxis()-> SetTitle("Wire #");
	      hist1_region3[4][plane-1] -> GetYaxis()-> SetTitle("Number of Events");
	      hist1_region3[4][plane-1] -> Draw();
	    }
	  else
	    {
	      region_tex -> DrawLatex(0.5,0.5,Form("RG %1d PL %1d : NO data", region, plane));
	    }
	}
    } 
  canvas -> Modified();
  canvas -> Update();
  
  return;
};


Bool_t 
QwGUITrackingSystem::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  //  bool status = 0;
  //std::cout << " TychoAnalysisProcessMessage::: msg :" << msg << ", parm1: " << ", parm2: " << parm2 << std::endl;
  switch (GET_MSG(msg)) 
    {
    case kC_COMMAND:
      {;
	switch (GET_SUBMSG(msg))
	  {
	  case kCM_BUTTON: 
	    {;;
	      switch(parm1)  
		{
		case  BA_CLOSECANVAS:
		  {;;;
		    CheckCanvas(this, canvas, "c", true);
		  };;;
		  break;
		case  BA_HELP:
		  {;;;
		    printf("May the force be with you\n");
		  };;;
		  break;
		case  BA_EXIT:
		  {;;;
		    printf("Can subframe close the main program?\n");
		  };;;
		  break;
		case  BA_TEXTBUTTONONE:
		  {;;;
		    run_number  = (UInt_t) num_entry[0] -> GetNumber();
		    printf("You selected RUN %d ROOT file\n", run_number);
		    //		    printf("Is it the right one %s ?\n", GetFileName().Data());

		    OpenRootFile();
		  };;;
		  break;
		case  BA_TEXTBUTTONTWO:
		  {;;;
		    EnableButtons(false);
		    ManipulateHistograms();
		  };;;
		  break;
		case   BA_TEXTBUTTONTHR:
		  {;;;

		    ResetButtons(true);
		    CloseRootFile();
		  };;;
		  break;
		case   BA_TEXTBUTTONFOR:
		  {;;;
		    PlotDraw();
		  };;;
		  break;
		case   BA_TEXTBUTTONFIV:
		  {;;;
		    PlotEventVsWire();
		  };;;
		  break;
		case   BA_TEXTBUTTONSIX:
		  {;;;
		    PlotHittedWireVsEvent();
		  };;;
		  break;
		default:
		  {;;;
		  };;;
		  break;
		}
	    };;
	    break;
	  case kCM_RADIOBUTTON: // case kCM_RADIOBUTTON:
	    {;;
	      switch (parm1) 
		{
		case BA_REGION_ONE:
		  {;;;
		    SetRegion(1);
		  };;;
		  break;
		case BA_REGION_TWO:
		  {;;;
		    SetRegion(2);
		  };;;
		  break;
		case BA_REGION_THR:
		  {;;;
		    SetRegion(3);
		  };;;
		  break;
		case BA_TIME:
		  {;;;
		    SetRequest(0);
		  };;;
		  break;
		case BA_RAWTIME:
		  {;;;
		    SetRequest(1);
		  };;;
		  break;
		case BA_CHANNEL:
		  {;;;
		    SetSelect(0);
		  };;;
		  break;
		case BA_WIRE:
		  {;;;
		    SetSelect(1);
		  };;;
		  break;
		default:
		  {;;;
		  };;;
		  break;
		}
	    };;
	    break;    
	  case kCM_COMBOBOX:// CASE 2 in the "GET_SUBMSG(msg) SWITCH"
	    {;;	  
	      switch (parm2) 
		{
		case BA_COMBO:
		  {;;;
		  };;;
		  break;
		default:
		  {;;;
		  };;;
		  break;
		}
	    };;
	    break;    
	  case kCM_CHECKBUTTON:
	    {;;	  
	      switch(parm1)  
		{
		case  BA_CHKBUTTON_ONE:
		  {;;;
		    event_range_status  = chk_button[0] -> GetState();
		    ResetButtons(false);
		    if(event_range_status)
		      {
			DefaultEventRange();
		      }
		    else
		      {
			ResetEventRange();
		      }
		  };;;
		  break;
		default:
		  {;;;
		  };;;
		  break;
		}
	    };;
	    break;
	  case kCM_LISTBOX:
	    {;;	  
	      switch(parm1)  
		{
		case BA_LISTBOX_ON:
		  {;;;
		  };;;
		  break;
		default:
		  {;;;
		  };;;
		  break;
		}
	    };;
	    break;
	  default:
	    {;;
	    };;
	    break;
	  }
      };
      break;
      
    case kC_TEXTENTRY: 
      {;
	switch (GET_SUBMSG(msg))
	  {
	  case kTE_TEXTCHANGED:
	    {;;
	      switch (parm1)
		{
		case BA_RUN_NUMBER:
		  {;;;
		    ResetButtons(true);
		    if(root_file) CloseRootFile();
		  };;; 
		  break;
		case BA_CHWIRE_NUMBER:
		  {;;;
		    ResetButtons(true);
		  };;; 
		  break;
		default:
		  {;;;
		  };;;
		  break;
		}
	    };;
	  default:
	    {;;
	    };;
	    break;
	  }
      };
      break;
      
    default:
      {;
      };
      break;
    }
  
  return kTRUE;
};




//
//
// private functions
//
//
void 
QwGUITrackingSystem::CheckOldObject(Char_t* name)
{
  TObject *obj;
  obj = (TObject*) gROOT->FindObject(name); 
  if(obj) 
    {
      delete obj; 
      obj = NULL;
    }
}


void
QwGUITrackingSystem::SetEventRange() 
{
  for(Short_t i=0; i<2; i++)
    {
      event_range[i] = (UInt_t) num_entry[i+1] -> GetNumber();
    }
  
  if( (event_range[0] ==0) && (event_range[1] ==0) )
    {
      event_range[0] = 0;
      event_range[1] = total_physics_event_number;
    }
  else
    {
      if (!(event_range[0] < event_range[1]) ) 
 	{
 	  event_range[1] = event_range[0] + 10;
 	  num_entry  [2]-> SetNumber(event_range[1]);
	  printf("Your selected range is not allowed. And ");
 	  // force to set max range 10 larger than min range
 	}
    }
  printf("Event Range [%d,%d] is used to do your request.\n", event_range[0], event_range[1]);

  event_range_status = true;
  return;
}

void 
QwGUITrackingSystem::DefaultEventRange() 
{
  event_range_status = true;
  for(Short_t i=0; i<2; i++)
    {
      num_entry[i+1] -> SetNumber(default_event_range[i]);
      num_entry[i+1] -> SetState (event_range_status);
    }
  chk_button[0] ->  SetState(kButtonDown);
  return;
}


void 
QwGUITrackingSystem::ResetEventRange() 
{
  event_range_status = true;
  for(Short_t i=0; i<2; i++)
    {
      event_range[i] = 0;
      num_entry[i+1] -> SetNumber(0);
      num_entry[i+1] -> SetState(event_range_status);
    }
  chk_button[0] -> SetState(kButtonUp);

  return;
}



Bool_t
QwGUITrackingSystem::OpenRootFile()
{
 
  run_number = (UInt_t) num_entry[0] -> GetNumber();
  root_file   = new TFile( Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"), run_number) );
  
  if (root_file->IsZombie()) 
    {
      printf("Error opening file.\n"); 
      ResetButtons(true);
      return false;
    }
  else
    {
      qwhit_tree  = (TTree*) root_file->Get("tree");
      qwhit_tree -> SetBranchAddress("hits", &qwhit_container);
      total_physics_event_number = qwhit_tree -> GetEntries();

      EnableButtons(true);

      return true;

    }
};


void
QwGUITrackingSystem::CloseRootFile()
{
  ResetProgressBar();
  DefaultEventRange();
  if (qwhit_container)
    {
      qwhit_container -> Clear();
      delete qwhit_container; 
      qwhit_container = NULL;
    }
  if (qwhit_tree)
    {
      delete qwhit_tree;
      qwhit_tree = NULL;
    }

  if( root_file -> IsOpen() ) root_file->Close();
  delete root_file; root_file = NULL;
  printf("The opened ROOT file is closed\n");
  return;
};




void 
QwGUITrackingSystem::ResetButtons(Bool_t main_buttons)
{
  if(main_buttons)
    {
      txt_button[0] -> SetEnabled(true);
      txt_button[0] -> ChangeBackground(red);
      for(Short_t i=1; i< TXT_BUTTON_NUM; i++) txt_button[i] -> SetEnabled(false);
    }
  else
    {
      for(Short_t i=3; i< TXT_BUTTON_NUM; i++) txt_button[i] -> SetEnabled(false);
    }
  return;
}



void 
QwGUITrackingSystem::EnableButtons(Bool_t main_buttons)
{
  if(main_buttons)
    {
      txt_button[0] -> SetEnabled(false);
      txt_button[0] -> ChangeBackground(green);
      for(Short_t i=1; i< TXT_BUTTON_NUM-3; i++) txt_button[i] -> SetEnabled(true);
    }
  else
    {
      for(Short_t i=3; i< TXT_BUTTON_NUM; i++) txt_button[i] -> SetEnabled(true);
    }
  return;
}


void
QwGUITrackingSystem::ResetProgressBar()
{
  progress_bar -> Reset();
  progress_bar -> SetBarColor("red");
  return;
}




TSuperCanvas *QwGUITrackingSystem::CheckCanvas(const TGWindow *main, TSuperCanvas *temp_canvas, const char* name, bool close_status)
{
  TSeqCollection *canvas_collection = gROOT->GetListOfCanvases();
  
  if ( !(strcmp(name,"c"))  ) 
    {
      TCanvas *obj;
      TIter next(canvas_collection, true);
      obj = (TCanvas*) next();
      while(obj){    
	obj->Close();
	delete obj;
	obj = (TCanvas*) next();
      }
      canvas_collection->Clear();
      return NULL;
    }
  else
    {
      temp_canvas = (TSuperCanvas*)canvas_collection->FindObject(name);
      //      gStyle -> SetCanvasBorderSize(0);
      //   gStyle -> SetCanvasBorderMode(0);
      //   gStyle -> SetCanvasColor(kWhite);
      //   gStyle -> SetPadBorderMode(1);
      //   gStyle -> SetPadBorderSize(0);
      //   gStyle -> SetFrameBorderMode(0);  // Strange red boarder on right and botton axis
      //   gStyle -> SetTitleFillColor(kWhite);
      if(!close_status)
	{
	  if(!temp_canvas) temp_canvas =  new TSuperCanvas(name, Form("%s canvas", name), 600,480);
	  else             temp_canvas -> Clear();
	  canvas_collection->Clear();
	  return temp_canvas;
	}
      else
	{
	  if(temp_canvas)  temp_canvas -> Close();
	  canvas_collection->Clear();
	  return NULL;
	}
    }
}
