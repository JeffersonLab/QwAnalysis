#include "QwGUITrackingSystem.h"

ClassImp(QwGUITrackingSystem);



enum QwGUITrackingSystemIndentificator {
  BA_RUN_NUMBER,
  BA_TEXTBUTTONONE,
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

  gframe [0] = NULL;
  for(Short_t i=0; i<2;i++)  g0hframe[i] = NULL;
  g0vframe[0] = NULL;


  num_entry[0] = NULL;
  num_label[0] = NULL;
  txt_button[0] = NULL;

  CreateFrame(parent,w, h);

};

QwGUITrackingSystem::~QwGUITrackingSystem()
{
  delete [] txt_button;
  delete [] num_label;
  delete [] num_entry;
  delete [] gframe;
  delete [] g0hframe;
  delete [] g0vframe;
}


void 
QwGUITrackingSystem::CreateFrame(TGCompositeFrame *parent, UInt_t w, UInt_t h)
{

  gframe[0] = new TGGroupFrame(parent, "ReadHits.C prototype GUI version and test TSuperCanvas", kVerticalFrame);
  parent -> AddFrame(gframe[0],  new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));

  g0hframe[0] = new TGHorizontalFrame(gframe[0]);
  gframe  [0] -> AddFrame(g0hframe[0], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));
  g0hframe[1] = new TGHorizontalFrame(gframe[0]);
  gframe  [0] -> AddFrame(g0hframe[1], new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,0,0,0,0));

  Short_t digital_width   = 10;
  Short_t run_number_min  = 1077;

  num_label[0]  = new TGLabel(g0hframe[0], "Run #");
  num_entry[0]  = new TGNumberEntry(g0hframe[0], run_number, digital_width, BA_RUN_NUMBER, 
				    TGNumberFormat::kNESInteger,
				    TGNumberFormat::kNEAPositive,
				    TGNumberFormat::kNELLimitMin, run_number_min);
  txt_button [0] = new TGTextButton(g0hframe[0], "Load Hit-based ROOT file",   BA_TEXTBUTTONONE);
  txt_button [0] -> SetToolTipText(" Not yet decided to be here or not.\n Just get a glimpse of what advanced Tracking GUI might be like in the future.\n Stand-alone or inside QwGUIMain? I am not sure....");

  
  g0hframe[0] -> AddFrame(num_label[0],  new TGLayoutHints(kLHintsExpandX,0, 0,4,0));
  g0hframe[0] -> AddFrame(num_entry[0],  new TGLayoutHints(kLHintsExpandX,0, 0,4,0));
  g0hframe[0] -> AddFrame(txt_button[0], new TGLayoutHints(kLHintsExpandX,20,1,4,1));
  
  num_entry  [0] -> Associate(this);
  txt_button [0] -> Associate(this);
  txt_button [0] -> ChangeBackground(red);
  

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
