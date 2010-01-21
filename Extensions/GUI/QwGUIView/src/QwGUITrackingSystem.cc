#include "QwGUITrackingSystem.h"

ClassImp(QwGUITrackingSystem);



enum QwGUITrackingSystemIndentificator {
  BA_RUN_NUMBER,
  BA_TEXTBUTTONONE,
  BA_TEXTBUTTONTWO,
  BA_EVENT_NUMBER_START, BA_EVENT_NUMBER_END, 
  BA_OUTPUTTYPE1,
  BA_COMBO,
  BA_DEBUGONE,
  BA_LISTBOX_ON
};


QwGUITrackingSystem::QwGUITrackingSystem(const TGWindow *main, TGCompositeFrame *parent, UInt_t w, UInt_t h)
{ 

  gClient->GetColorByName("green", green);
  gClient->GetColorByName("red",   red);

  run_number = 1567;

  for(Short_t i=0; i<2;i++)  hframe [i] = NULL;
  gframe [0] = NULL;
  for(Short_t i=0; i<2;i++)  g0hframe[i] = NULL;
  g0vframe[0] = NULL;


  for(Short_t i=0; i<3;i++) num_entry[i] = NULL;
  for(Short_t i=0; i<2;i++) num_label[i] = NULL;
  for(Short_t i=0; i<2;i++) txt_button[i] = NULL;

  CreateFrame(parent,w, h);

};

QwGUITrackingSystem::~QwGUITrackingSystem()
{
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



  Short_t digital_width   = 10;
  Short_t run_number_min  = 1077;

  
  hframe [0] = new TGHorizontalFrame(parent);
  parent  -> AddFrame(hframe[0], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,10,10,10,10));
  hframe [1] = new TGHorizontalFrame(parent);
  parent  -> AddFrame(hframe[1], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,10,10,10,10));


  // hframe [0] 
  num_label[0]  = new TGLabel(hframe[0], "Run #");
  num_entry[0]  = new TGNumberEntry(hframe[0], run_number, digital_width, BA_RUN_NUMBER, 
				    TGNumberFormat::kNESInteger,
				    TGNumberFormat::kNEAPositive,
				    TGNumberFormat::kNELLimitMin, run_number_min);
  txt_button [0] = new TGTextButton(hframe[0], "Load Hit-based ROOT file",   BA_TEXTBUTTONONE);
  txt_button [0] -> SetToolTipText(" Not yet decided to be here or not.\n Just get a glimpse of what advanced Tracking GUI might be like in the future.\n Stand-alone or inside QwGUIMain? I am not sure....");

  
  hframe[0] -> AddFrame(num_label[0],  new TGLayoutHints(kLHintsExpandX,0, 0,4,0));
  hframe[0] -> AddFrame(num_entry[0],  new TGLayoutHints(kLHintsExpandX,0, 0,4,0));
  hframe[0] -> AddFrame(txt_button[0], new TGLayoutHints(kLHintsExpandX,20,1,4,1));

  num_entry  [0] -> Associate(this);
  txt_button [0] -> Associate(this);
  txt_button [0] -> ChangeBackground(red);

  // hframe [1] 


  Short_t event_num_min     = 0;
  Short_t default_event_num = 0;
    
  digital_width = 4;

  num_label[1] = new TGLabel(hframe[1], "Physics Event #");
  num_entry[1] = new TGNumberEntry(hframe[1], default_event_num, digital_width, BA_EVENT_NUMBER_START, 
				   TGNumberFormat::kNESInteger,
				   TGNumberFormat::kNEANonNegative,
				   TGNumberFormat::kNELLimitMin, event_num_min);
  num_entry[2] = new TGNumberEntry(hframe[1], default_event_num, digital_width, BA_EVENT_NUMBER_END, 
				   TGNumberFormat::kNESInteger,
				   TGNumberFormat::kNEANonNegative,
				   TGNumberFormat::kNELLimitMin, event_num_min);
//   txt_button[1] = new TGTextButton(hframe[1], "Reset Event #",   BA_TEXTBUTTONTWO);
  
  hframe[1] -> AddFrame(num_label[1],  new TGLayoutHints(kLHintsExpandX,0, 0,4,0));
  hframe[1] -> AddFrame(num_entry[1],  new TGLayoutHints(kLHintsExpandX,0, 0,4,0));
  hframe[1] -> AddFrame(num_entry[2],  new TGLayoutHints(kLHintsExpandX,0, 0,4,0));
//   hframe[1] -> AddFrame(txt_button[1], new TGLayoutHints(kLHintsExpandX,10,1,4,1));
  
  num_entry  [1] -> Associate(this);
  num_entry  [1] -> SetState(false);
  num_entry  [2] -> Associate(this);
  num_entry  [2] -> SetState(false);
  //  num_label  [1] -> SetToolTipText("This Event is a Physics Event of CODA buffer");
//   txt_button [1] -> SetToolTipText("Reset Event number from a beginning..");
//   //  txt_button [2] -> SetToolTipText("Maybe next version.....");
//   txt_button [1] -> Associate(this);
  //  txt_button [2] -> SetEnabled(false);
  



  gframe[0] = new TGGroupFrame(parent, "ReadHits.C prototype GUI version and test TSuperCanvas", kVerticalFrame);
  parent -> AddFrame(gframe[0],  new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));

  g0hframe[1] = new TGHorizontalFrame(gframe[0]);
  gframe  [0] -> AddFrame(g0hframe[1], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));


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
		case  BA_TEXTBUTTONONE:
		  {;;;
		    run_number  = (UInt_t) num_entry[0] -> GetNumber();
		    if(run_number%2 == 0) txt_button[0] -> ChangeBackground(green);
		    else                  txt_button[0] -> ChangeBackground(red);
		    printf("You selected RUN %d ROOT file\n", run_number);
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
		case  BA_DEBUGONE:
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
