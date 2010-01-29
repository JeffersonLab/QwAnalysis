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



const char *QwGUITrackingSystem::request_types[REQUEST_TYPE_NUM]=
  {  "time"
   , "rawtime"
  };


const char *QwGUITrackingSystem::select_types[SELECT_TYPE_NUM]=
  {  "channel"
   , "wire"
  };



QwGUITrackingSystem::QwGUITrackingSystem(const TGWindow *main, TGCompositeFrame *parent, UInt_t w, UInt_t h)
{ 

  gClient->GetColorByName("green", green);
  gClient->GetColorByName("red",   red);

  run_number     = 1567;
  event_range[0] = 0;
  event_range[1] = 0;

  default_event_range[0] = 0;
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
  
  event_range_status   = false;
  load_rootfile_status = false;

  root_file       = NULL;
  qwhit_tree      = NULL;
  qwhit           = NULL;
  qwhit_container = NULL;

  for(Short_t i=0;i<RG_ONE_HIST_NUM;i++) hist1_region1[i] = NULL;
  for(Short_t i=0;i<RG_TWO_HIST_NUM;i++) hist1_region2[i] = NULL;
  for(Short_t i=0;i<RG_THR_HIST_NUM;i++) hist1_region3[i] = NULL;

  for(Short_t i=0;i<RG_ONE_HIST_NUM;i++) hist2_region1[i] = NULL;
  for(Short_t i=0;i<RG_TWO_HIST_NUM;i++) hist2_region2[i] = NULL;
  for(Short_t i=0;i<RG_THR_HIST_NUM;i++) hist2_region3[i] = NULL;

  d_region = 3;
  d_request = 0;
  d_select  = 0;

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

  delete [] hist1_region1;
  delete [] hist1_region2;
  delete [] hist1_region3;

  delete [] hist2_region1;
  delete [] hist2_region2;
  delete [] hist2_region3;

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
  Short_t default_event_num = 0;
  
  // hframe [0] 
  num_label[0]  = new TGLabel(hframe[0], "Run #");
  num_entry[0]  = new TGNumberEntry(hframe[0], run_number, digital_width, BA_RUN_NUMBER, 
				    TGNumberFormat::kNESInteger,
				    TGNumberFormat::kNEAPositive,
				    TGNumberFormat::kNELLimitMin, run_number_min);
  chk_button[0] = new TGCheckButton (hframe[0], "Event Range", BA_CHKBUTTON_ONE);
  num_label [1] = new TGLabel(hframe[0], "Evt #");
  
  digital_width = 4;
  num_entry[1] = new TGNumberEntry(hframe[0], default_event_num, digital_width, BA_EVENT_NUMBER_START, 
				   TGNumberFormat::kNESInteger,
				   TGNumberFormat::kNEANonNegative,
				   TGNumberFormat::kNELLimitMin, event_num_min);
  num_entry[2] = new TGNumberEntry(hframe[0], default_event_num, digital_width, BA_EVENT_NUMBER_END, 
				   TGNumberFormat::kNESInteger,
				   TGNumberFormat::kNEANonNegative,
				   TGNumberFormat::kNELLimitMin, event_num_min);
  
  hframe[0] -> AddFrame(num_label [0], new TGLayoutHints(kLHintsCenterX,0,0,4,0));
  hframe[0] -> AddFrame(num_entry [0], new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  hframe[0] -> AddFrame(chk_button[0], new TGLayoutHints(kLHintsCenterX,4,0,4,0));
  hframe[0] -> AddFrame(num_label [1], new TGLayoutHints(kLHintsCenterX,0,0,4,0));
  hframe[0] -> AddFrame(num_entry [1], new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  hframe[0] -> AddFrame(num_entry [2], new TGLayoutHints(kLHintsExpandX,0,0,0,0));

  num_entry [1] -> SetState(false);
  num_entry [2] -> SetState(false);

  chk_button[0] -> SetState(kButtonUp);
  chk_button[0] -> SetToolTipText("Turn on/off Select Range");


  //
  // hframe [1] 
  //
  txt_button [0] = new TGTextButton(hframe[1], "Load Hit-based ROOT file", BA_TEXTBUTTONONE);
  txt_button [1] = new TGTextButton(hframe[1], "Manipulate Histograms",    BA_TEXTBUTTONTWO);
  txt_button [2] = new TGTextButton(hframe[1], "Close the ROOT file",      BA_TEXTBUTTONTHR);

  txt_button [0] -> SetToolTipText(" Not yet decided to be here or not.\n Just get a glimpse of what advanced Tracking GUI might be like in the future.\n Stand-alone or inside QwGUIMain? I am not sure....");

  hframe[1] -> AddFrame(txt_button[0], new TGLayoutHints(kLHintsExpandX,4,1,4,1));
  hframe[1] -> AddFrame(txt_button[1], new TGLayoutHints(kLHintsExpandX,0,0,4,0));
  hframe[1] -> AddFrame(txt_button[2], new TGLayoutHints(kLHintsExpandX,0,8,4,1));

  gframe[0] = new TGGroupFrame(parent, "ReadHits.C prototype GUI version and test TSuperCanvas", kVerticalFrame);
  parent   -> AddFrame(gframe[0],  new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));

  g0hframe[0] = new TGHorizontalFrame(gframe[0]);
  g0hframe[1] = new TGHorizontalFrame(gframe[0]);
  g0hframe[2] = new TGHorizontalFrame(gframe[0]);
  gframe  [0] -> AddFrame(g0hframe[0], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));
  gframe  [0] -> AddFrame(g0hframe[1], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));
  gframe  [0] -> AddFrame(g0hframe[2], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));

  //
  // hframe[1] - g0hframe[0]
  //
  btn_group[0] = new TGButtonGroup(g0hframe[0],"Select Region", kHorizontalFrame);
  g0hframe[0] -> AddFrame(btn_group[0], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));

  rad_button[0] = new TGRadioButton(btn_group[0], "Region 1 (GEM)", BA_REGION_ONE);
  rad_button[1] = new TGRadioButton(btn_group[0], "Region 2 (HDC)", BA_REGION_TWO);
  rad_button[2] = new TGRadioButton(btn_group[0], "Region 3 (VDC)", BA_REGION_THR);

  rad_button[2] -> SetState(kButtonDown);
  btn_group[0]  -> SetLayoutHints(new TGLayoutHints(kLHintsCenterX,0,0,10,0));
   
  //
  // hframe[1] - g0hframe[1]
  //
  btn_group[1] = new TGButtonGroup  (g0hframe[1], "", kVerticalFrame);
  num_label[1] = new TGLabel        (g0hframe[1], "Histogream Per ");
  btn_group[2] = new TGButtonGroup  (g0hframe[1], "", kVerticalFrame);
  g0vframe [0] = new TGVerticalFrame(g0hframe[1]);
  
  g0hframe [1] -> AddFrame(btn_group[1], new TGLayoutHints(kLHintsCenterX,0,0,0,0));
  g0hframe [1] -> AddFrame(num_label[1], new TGLayoutHints(kLHintsCenterY,0,0,40,0));
  g0hframe [1] -> AddFrame(btn_group[2], new TGLayoutHints(kLHintsCenterX,0,0,0));
  g0hframe [1] -> AddFrame(g0vframe [0], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));
  

  rad_button[3] = new TGRadioButton(btn_group[1], "Time",    BA_TIME);
  rad_button[4] = new TGRadioButton(btn_group[1], "RawTime", BA_RAWTIME);
  rad_button[5] = new TGRadioButton(btn_group[2], "Channel", BA_CHANNEL);
  rad_button[6] = new TGRadioButton(btn_group[2], "Wire",    BA_WIRE);

  rad_button[3] -> SetState(kButtonDown);
  rad_button[5] -> SetState(kButtonDown);

  btn_group[1] -> SetLayoutHints(new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  btn_group[2] -> SetLayoutHints(new TGLayoutHints(kLHintsExpandX,0,0,0,0));

  
  ch_wire_number = 10;
  digital_width  = 4;
  num_entry[3]   = new TGNumberEntry(g0vframe[0], ch_wire_number, digital_width, BA_CHWIRE_NUMBER, 
				     TGNumberFormat::kNESInteger,
				     TGNumberFormat::kNEAPositive,
				     TGNumberFormat::kNELLimitMin, 0);
  txt_button[3]  = new TGTextButton(g0vframe[0], "PLOT",   BA_TEXTBUTTONFOR);
  g0vframe[0] -> AddFrame(num_entry[3],  new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,20,0,10,0));
  g0vframe[0] -> AddFrame(txt_button[3], new TGLayoutHints(kLHintsExpandX,20,1,4,1));


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
QwGUITrackingSystem::DefaultEventRange(Bool_t entry_status=true) 
{
  for(Short_t i=0; i<2; i++)
    {
      num_entry[i+1] -> SetNumber(default_event_range[i]);
      num_entry[i+1] -> SetState (entry_status);
    }
  event_range_status = true;
  return;
}


void 
QwGUITrackingSystem::ResetEventRange(Bool_t entry_status=false) 
{
  for(Short_t i=0; i<2; i++)
    {
      event_range[i] = 0;
      num_entry[i+1] -> SetNumber(event_range[i]);
      num_entry[i+1] -> SetState(entry_status);
    }
  chk_button[0] -> SetState(kButtonUp);
  event_range_status = false;
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
QwGUITrackingSystem::ManipulateHistograms()
{
  SetEventRange();


 //  for(Short_t i=0;i<RG_ONE_HIST_NUM;i++) 
//     {
//       if(hist1_region1[i]) 
// 	{
// 	  delete hist1_region1[i];
// 	  hist1_region1[i] = NULL;
// 	}
//     }
//   for(Short_t i=0;i<RG_TWO_HIST_NUM;i++) 
//     {
//       if(hist1_region2[i]) 
// 	{
// 	  delete hist1_region2[i];
// 	  hist1_region2[i] = NULL;
// 	}
//     }
//   for(Short_t i=0;i<RG_THR_HIST_NUM;i++) 
//     {
//       if(hist1_region3[i]) 
// 	{
// 	  delete hist1_region3[i];
// 	  hist1_region3[i] = NULL;
// 	}
//     }

  UInt_t get_number = 0;


//   Int_t    nhit        = 0;
//   Int_t    ev_i        = 0; 
//   Int_t    hit_i       = 0;
          
//   Double_t tdc_rawtime = 0.0;
//   Double_t tdc_time    = 0.0;

//   UInt_t   tdc_chan    = 0;
//   UInt_t   wire_number = 0;
  Short_t  region      = 0;
//   Short_t  wire        = 0;
//   Short_t  package     = 0;
  Short_t  direction   = 0;
  Short_t  plane       = 0;
//   Short_t  tmp         = 0;

//   Double_t offset      = -0.5;
//   Int_t    xbinN[3]    = {0, 0, 0};
//   Int_t    ybinN[3]    = {RG_ONE_WIRE_NUM, RG_TWO_WIRE_NUM, RG_THR_WIRE_NUM}; // more than the number of wires
      
  get_number = (UInt_t) num_entry[3] -> GetNumber();

  
  for(Short_t i=0; i<3; i++)
    {
      region          = i+1;
      
      if( region==1 ) 
	{
	  for(Short_t j=0; j<RG_ONE_HIST_NUM; j++)
	    {
	      direction = j+1;
	      hist1_region1[j]=  new TH1D(Form("RG%dDIR%d", region, direction), 
					  Form("%s per %s %d  RG %d DIR %d", request_types[d_request], select_types[d_select], get_number, region, direction), 
					  BINNUMBER, 0, 0); 
	      hist1_region1[j] -> SetDefaultBufferSize(BUFFERSIZE);
	    }
	}
      printf("%s\n",Form("%s per %s %d  RG %d DIR %d", request_types[d_request], select_types[d_select], get_number, region, direction));

      if( region==2 )
	{
	  for(Short_t j=0; j<RG_TWO_HIST_NUM; j++)
	    {
	      plane = j+1; 
	      hist1_region2[j]=  new TH1D(Form("RG%dPL%d", region, plane), 
					  Form("%s per %s %d  RG %d DIR %d", request_types[d_request], select_types[d_select], get_number, region, direction), 
					  BINNUMBER, 0, 0); 
	      hist1_region2[j] -> SetDefaultBufferSize(BUFFERSIZE);
	    }
	}
      
      if( region==3 ) 
	{
	  for(Short_t j=0; j<RG_THR_HIST_NUM; j++)
	    {
	      plane = j+1; 
	      hist1_region3[j]=  new TH1D(Form("RG%dPL%d", region, plane), 
					  Form("%s per %s %d  RG %d DIR %d", request_types[d_request], select_types[d_select], get_number, region, direction), 
					  BINNUMBER, 0, 0); 
	      hist1_region3[j] -> SetDefaultBufferSize(BUFFERSIZE);
	    }
	}
    }
  
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
		    printf("there is no canvas yes\n");
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
		    ResetEventRange();
		    ResetButtons(true);
		    CloseRootFile();
		  };;;
		  break;
		case   BA_TEXTBUTTONFOR:
		  {;;;
		    printf("you pressed PLOT button\n");
		  };;;
		  break;
		case   BA_TEXTBUTTONFIV:
		  {;;;
		    printf("you pressed 5 button\n");
		  };;;
		  break;
		case   BA_TEXTBUTTONSIX:
		  {;;;
		    printf("you pressed 6 button\n");
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
		  };;; 
		  break;
		case BA_CHWIRE_NUMBER:
		  {;;;
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
