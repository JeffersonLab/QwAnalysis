#include "QwGUITrackingSystem.h"

ClassImp(QwGUITrackingSystem);



enum QwGUITrackingSystemIndentificator {
  BA_RUN_NUMBER,
  BA_TEXTBUTTONONE,
  BA_TEXTBUTTONTWO,
  BA_EVENT_NUMBER_START, BA_EVENT_NUMBER_END, 
  BA_OUTPUTTYPE1,
  BA_COMBO,
  BA_CHKBUTTON_ONE,
  BA_LISTBOX_ON
};


QwGUITrackingSystem::QwGUITrackingSystem(const TGWindow *main, TGCompositeFrame *parent, UInt_t w, UInt_t h)
{ 

  gClient->GetColorByName("green", green);
  gClient->GetColorByName("red",   red);

  run_number = 1567;
  event_range[0] = 0;
  event_range[1] = 0;

  default_event_range[0] = 0;
  default_event_range[1] = 1000;


  for(Short_t i=0; i<HFRAME_NUM;  i++) hframe  [i] = NULL;
  for(Short_t i=0; i<GFRAME_NUM;  i++) gframe  [0] = NULL;
  for(Short_t i=0; i<G0HFRAME_NUM;i++) g0hframe[i] = NULL;
  for(Short_t i=0; i<G0VFRAME_NUM;i++) g0vframe[0] = NULL;


  for(Short_t i=0; i<NUM_ENTRY_NUM; i++) num_entry [i] = NULL;
  for(Short_t i=0; i<LABEL_NUM;     i++) num_label [i] = NULL;
  for(Short_t i=0; i<TXT_BUTTON_NUM;i++) txt_button[i] = NULL;
  for(Short_t i=0; i<CHK_BUTTON_NUM;i++) chk_button[i] = NULL;

  event_range_status  = false;
  load_rootfile_status = false;

  CreateFrame(parent,w, h);

};

QwGUITrackingSystem::~QwGUITrackingSystem()
{
  delete [] chk_button;
  delete [] txt_button;
  delete [] num_label;
  delete [] num_entry;
  delete [] hframe;
  delete [] gframe;
  delete [] g0hframe;
  delete [] g0vframe;
}


void 
QwGUITrackingSystem::CreateFrame(TGCompositeFrame *parent, UInt_t w, UInt_t h)
{
  

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
  chk_button[0] = new TGCheckButton (hframe[0], "Select Event Range", BA_CHKBUTTON_ONE);
  
  num_label[1] = new TGLabel(hframe[0], "Evt #");
  
  digital_width = 4;
  
  num_entry[1] = new TGNumberEntry(hframe[0], default_event_num, digital_width, BA_EVENT_NUMBER_START, 
				   TGNumberFormat::kNESInteger,
				   TGNumberFormat::kNEANonNegative,
				   TGNumberFormat::kNELLimitMin, event_num_min);
  num_entry[2] = new TGNumberEntry(hframe[0], default_event_num, digital_width, BA_EVENT_NUMBER_END, 
				   TGNumberFormat::kNESInteger,
				   TGNumberFormat::kNEANonNegative,
				   TGNumberFormat::kNELLimitMin, event_num_min);
  
  hframe[0] -> AddFrame(num_label[0],   new TGLayoutHints(kLHintsCenterX,0,0,4,0));
  hframe[0] -> AddFrame(num_entry[0],   new TGLayoutHints(kLHintsCenterX,0,0,0,0));
  hframe[0] -> AddFrame(chk_button[0],  new TGLayoutHints(kLHintsCenterX,4,0,4,0));
  hframe[0] -> AddFrame(num_label[1],   new TGLayoutHints(kLHintsCenterX,0,0,4,0));
  hframe[0] -> AddFrame(num_entry[1],   new TGLayoutHints(kLHintsExpandX,0,0,0,0));
  hframe[0] -> AddFrame(num_entry[2],   new TGLayoutHints(kLHintsExpandX,0,0,0,0));

  num_entry [0] -> Associate(this);
  chk_button[0] -> Associate(this);
  num_entry [1] -> Associate(this);
  num_entry [2] -> Associate(this);
  num_entry [1] -> SetState(false);
  num_entry [2] -> SetState(false);


  chk_button[0] -> SetState(kButtonUp);
  chk_button[0] -> SetToolTipText("Turn on/off Select Range");


  // hframe [1] 



  txt_button [0] = new TGTextButton(hframe[1], "Load Hit-based ROOT file",   BA_TEXTBUTTONONE);
  txt_button [0] -> SetToolTipText(" Not yet decided to be here or not.\n Just get a glimpse of what advanced Tracking GUI might be like in the future.\n Stand-alone or inside QwGUIMain? I am not sure....");
  txt_button [1] = new TGTextButton(hframe[1], "Prepare historgrams",   BA_TEXTBUTTONTWO);


  hframe[1] -> AddFrame(txt_button[0], new TGLayoutHints(kLHintsExpandX,20,1,4,1));
  hframe[1] -> AddFrame(txt_button[1], new TGLayoutHints(kLHintsExpandX,20,1,4,1));

 
  txt_button[0] -> Associate(this);
  txt_button[0] -> ChangeBackground(red);
  txt_button[1] -> Associate(this); 
  txt_button[1] -> SetEnabled(false);

 
  gframe[0] = new TGGroupFrame(parent, "ReadHits.C prototype GUI version and test TSuperCanvas", kVerticalFrame);
  parent -> AddFrame(gframe[0],  new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));

  g0hframe[1] = new TGHorizontalFrame(gframe[0]);
  gframe  [0] -> AddFrame(g0hframe[1], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));


  return;
  
};



TString 
QwGUITrackingSystem::GetFileName() 
{
  run_number       = (UInt_t) num_entry[0] -> GetNumber();
  return Form("%s/Qweak_%d.root", getenv("QW_ROOTFILES_DIR"), run_number);
}




Bool_t
QwGUITrackingSystem::SetEventRange() 
{
//   for(Short_t i=0; i<2; i++)
//     {
//       event_range[i] = (UInt_t) num_entry[i+1] -> GetNumber();
//     }


//   if( (event_range[0] ==0) && (event_range[1] ==0) )
//     {
//       event_range_status = false;  
//       range[0] = 0;
//       range[1] = total_physics_event_number;
//     }
//   else
//     {
//       event_number_range_status = true;
//       if (!(event_number_range[0] < event_number_range[1]) ) 
// 	{
// 	  event_number_range[1] = event_number_range[0] + 10;
// 	  num_entry[2]-> SetNumber(event_number_range[1]);
// 	  // force to set max range 10 larger than min range
// 	}
//       range[0] = event_number_range[0]; 
//       range[1] = event_number_range[1];
//     }

  return true;
}

void 
QwGUITrackingSystem::DefaultEventRange(Bool_t entry_status=true) 
{
  for(Short_t i=0; i<2; i++)
    {
      num_entry[i+1] -> SetNumber(default_event_range[i]);
      num_entry[i+1] -> SetState(entry_status);
    }
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
		case  BA_TEXTBUTTONONE:
		  {;;;
		    run_number  = (UInt_t) num_entry[0] -> GetNumber();
		    if(run_number%2 == 0) txt_button[0] -> ChangeBackground(green);
		    else                  txt_button[0] -> ChangeBackground(red);
		    printf("You selected RUN %d ROOT file\n", run_number);
		    printf("Is it the right one %s ?\n", GetFileName().Data());
		  };;;
		  break;
		case  BA_TEXTBUTTONTWO:
		  {;;;
		    printf("you pressed 2nd text button\n");
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
		case BA_OUTPUTTYPE1:
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
