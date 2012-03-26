#include "QwGUICorrelationPlots.h"

//#include "TG3DLine.h"
#include "TGaxis.h"
#include "TCut.h"

#include "TGaxis.h"
#include "TColor.h"
#include "TStyle.h"
#include "TPaveLabel.h"
#include <TString.h>



ClassImp(QwGUICorrelationPlots);

//buttons
enum QwGUICorrelationPlotsIndentificator {
  BA_CORR_PLOT,
  BA_BCMDD_PLOT,
};

//text entry
enum QwGUICorrelationPlotsTextEntry {
  TXT_Y,
  TXT_X,
  TXT_CUT,
};

//List box
enum QwGUICorrelationPlotsListBox {
  LST_TREE
};

//Combo box
enum QwGUICorrelationPlotsComboBox {
  CMB_TREE_PX,
  CMB_TREE_PY
};

const char *QwGUICorrelationPlots::CorrelationPlotsTrees[CORRELATION_DET_TRE_NUM] = 
  {
    "Hel_Tree",
    "Mps_Tree"
  };


QwGUICorrelationPlots::QwGUICorrelationPlots(const TGWindow *p, const TGWindow *main, const TGTab *tab,
			       const char *objName, const char *mainname, UInt_t w, UInt_t h)
  : QwGUISubSystem(p,main,tab,objName,mainname,w,h)
{ 
  dTabFrame           = NULL;
  dControlsFrame      = NULL;
  dTreeFrame          = NULL;
  dTreeInputFrame1     = NULL;
  dTreeInputFrame2     = NULL;
  dTreeInputFrame3     = NULL;
  dTreeInputFrame4     = NULL;
  dTreeInputFrame5     = NULL;

  dCanvas             = NULL;  
  dTabLayout          = NULL;
  dCnvLayout          = NULL;
  dSubLayout          = NULL;
  dBtnLayout          = NULL;
  dButtonCorrelationPlot = NULL;
  dButtonBCMDDPlot       = NULL;

  //Tree List box
  dListboxTrees       = NULL;

  //Tree Comboboxes
  dComboBoxTreePrm1   = NULL;
  dComboBoxTreePrm2   = NULL;

  dTreeXlbl           = NULL;
  dTreeYlbl           = NULL;
  dTreeXlbl2          = NULL;
  dTreeYlbl2          = NULL;
  dTreeCutlbl           = NULL;

  dTreeYtxt           = NULL;
  dTreeXtxt           = NULL;
  dTreeCuttxt           = NULL;
   

  PosVariation[0] = NULL;
  PosVariation[1] = NULL;


  // HistArray.Clear();
  // DataWindowArray.Clear();

  bCUTEXPR=kFALSE;//default setting is to use simple X and Y cuts. If cut expr is used this will be set to true in the SetCutExpr()

  AddThisTab(this);
  

}

QwGUICorrelationPlots::~QwGUICorrelationPlots()
{
  if(dTabFrame)           delete dTabFrame;
  if(dControlsFrame)      delete dControlsFrame;
  if(dTreeFrame)          delete dTreeFrame;
  if(dTreeInputFrame1)     delete dTreeInputFrame1;
  if(dTreeInputFrame2)     delete dTreeInputFrame2;
  if(dTreeInputFrame3)     delete dTreeInputFrame3;
  if(dTreeInputFrame4)     delete dTreeInputFrame4;
  if(dTreeInputFrame5)     delete dTreeInputFrame5;

  if(dCanvas)             delete dCanvas;  
  if(dTabLayout)          delete dTabLayout;
  if(dCnvLayout)          delete dCnvLayout;
  if(dSubLayout)          delete dSubLayout;
  if(dBtnLayout)          delete dBtnLayout;

  if(dListboxTrees)       delete dListboxTrees;
  if(dButtonCorrelationPlot) delete dButtonCorrelationPlot;
  if(dButtonBCMDDPlot) delete dButtonBCMDDPlot;

  if(dComboBoxTreePrm1)   delete dComboBoxTreePrm1;
  if(dComboBoxTreePrm2)   delete dComboBoxTreePrm2;

  if(dTreeXlbl)           delete dTreeXlbl;
  if(dTreeYlbl)           delete dTreeYlbl;
  if(dTreeXlbl2)           delete dTreeXlbl2;
  if(dTreeYlbl2)           delete dTreeYlbl2;
  if(dTreeCutlbl)           delete dTreeCutlbl;

  if(dTreeYtxt)           delete dTreeYtxt;
  if(dTreeXtxt)           delete dTreeXtxt;
  if(dTreeCuttxt)           delete dTreeCuttxt;
  

  delete [] PosVariation;

  // RemoveThisTab(this);
  // IsClosing(GetName());
}

void QwGUICorrelationPlots::MakeLayout()
{

  SetCleanup(kDeepCleanup);

  dTabFrame= new TGHorizontalFrame(this);  
  AddFrame(dTabFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 5, 5));


  dControlsFrame = new TGVerticalFrame(this);
  dTabFrame->AddFrame(dControlsFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandY, 5, 5, 5, 5));

  

  
  // TGVertical3DLine *separator = new TGVertical3DLine(this);
  // dTabFrame->AddFrame(separator, new TGLayoutHints(kLHintsRight | kLHintsExpandY));

  dCanvas   = new TRootEmbeddedCanvas("pC", dTabFrame,200, 200); 
  dTabFrame->AddFrame(dCanvas, new TGLayoutHints( kLHintsLeft | kLHintsExpandY | kLHintsExpandX, 2, 2, 2, 2));

 
  


  dBtnLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);

  //Add frames to insert lists, combos, labels and texts
  dTreeInputFrame1= new TGHorizontalFrame(dControlsFrame,50,100);
  dControlsFrame->AddFrame(dTreeInputFrame1, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));
  dTreeInputFrame2= new TGHorizontalFrame(dControlsFrame,50,100);
  dControlsFrame->AddFrame(dTreeInputFrame2, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));
  dTreeInputFrame3= new TGVerticalFrame(dControlsFrame,50,100);
  dControlsFrame->AddFrame(dTreeInputFrame3, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));
  dTreeInputFrame4= new TGHorizontalFrame(dControlsFrame,50,100);
  dControlsFrame->AddFrame(dTreeInputFrame4, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));
  dTreeInputFrame5= new TGHorizontalFrame(dControlsFrame,50,100);
  dControlsFrame->AddFrame(dTreeInputFrame5, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));

  //Add two labels
  dTreeYlbl =new TGLabel(dTreeInputFrame1,"Y CUT");
  dTreeYlbl->Resize(10,20);//w,h
  dTreeXlbl =new TGLabel(dTreeInputFrame2,"X CUT");
  dTreeXlbl->Resize(10,20);//w,h
  dTreeCutlbl =new TGLabel(dTreeInputFrame3,"Cut Expr");
  dTreeCutlbl->Resize(10,20);//w,h
  
 
 
  //Add two text boxes
  dTreeYtxt = new TGTextEntry(dTreeInputFrame1,"",TXT_Y);
  dTreeXtxt = new TGTextEntry(dTreeInputFrame2,"",TXT_X);
  dTreeCuttxt = new TGTextEntry(dTreeInputFrame3,"", TXT_CUT);
  //dTreeCuttxt->Resize(100,20);//w,h
  
  dTreeInputFrame1->AddFrame(dTreeYlbl,new TGLayoutHints( kLHintsNormal | kLHintsLeft , 10, 10, 5, 5));
  dTreeInputFrame2->AddFrame(dTreeXlbl,new TGLayoutHints( kLHintsNormal | kLHintsLeft , 10, 10, 5, 5));
  dTreeInputFrame3->AddFrame(dTreeCutlbl,new TGLayoutHints( kLHintsNormal | kLHintsTop , 10, 10, 5, 5));

  dTreeInputFrame1->AddFrame(dTreeYtxt,new TGLayoutHints( kLHintsExpandX | kLHintsRight , 10, 10, 5, 5));
  dTreeInputFrame2->AddFrame(dTreeXtxt,new TGLayoutHints( kLHintsExpandX | kLHintsRight , 10, 10, 5, 5));
  dTreeInputFrame3->AddFrame(dTreeCuttxt,new TGLayoutHints( kLHintsExpandX | kLHintsBottom , 10, 10, 5, 5));

  //Add two combo boxes
  dComboBoxTreePrm1=new TGComboBox(dTreeInputFrame4,CMB_TREE_PX);
  dComboBoxTreePrm1->Resize(50,20);//To make it better looking
  dComboBoxTreePrm2=new TGComboBox(dTreeInputFrame5,CMB_TREE_PY);
  dComboBoxTreePrm2->Resize(50,20);//To make it better looking
  dComboBoxTreePrm1->SetEnabled(kFALSE);
  dComboBoxTreePrm2->SetEnabled(kFALSE);

   //Add two labels
  dTreeYlbl2 =new TGLabel(dTreeInputFrame4,"X");
  dTreeXlbl2 =new TGLabel(dTreeInputFrame5,"Y");

  dTreeLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop | kLHintsRight, 10, 10, 5, 5);
  dTreeInputFrame4->AddFrame(dComboBoxTreePrm1, dTreeLayout);
  dTreeInputFrame4->AddFrame(dTreeYlbl2, new TGLayoutHints( kLHintsNormal| kLHintsLeft , 10, 10, 5, 5));
  dTreeInputFrame5->AddFrame(dComboBoxTreePrm2, dTreeLayout);
  dTreeInputFrame5->AddFrame(dTreeXlbl2, new TGLayoutHints( kLHintsNormal | kLHintsLeft , 10, 10, 5, 5));

  
  //for the tree list box and other GUI compenents
  dTreeFrame= new TGHorizontalFrame(dControlsFrame,50,100); //w,h
  dControlsFrame->AddFrame(dTreeFrame, new TGLayoutHints(kLHintsRight | kLHintsExpandX, 5, 5, 5, 5));

  //Tree parameters

  //ListBox
  dListboxTrees = new TGListBox(dTreeFrame, LST_TREE);
  dListboxTrees->Resize(50,50);//To make it better looking
  //Add two trees in to the list box
  dListboxTrees->AddEntry("Pattern",0);  
  dListboxTrees->AddEntry("Event",1);
  fTreeIndex=0;

  dTreeLayout = new TGLayoutHints( kLHintsExpandX | kLHintsTop , 10, 10, 5, 5);
  dTreeFrame->AddFrame(dListboxTrees,dTreeLayout);

  dButtonCorrelationPlot = new TGTextButton(dTreeFrame, "&Plot", BA_CORR_PLOT);
  dButtonCorrelationPlot->SetEnabled(kFALSE);
  dTreeFrame->AddFrame(dButtonCorrelationPlot, dBtnLayout);

  dButtonBCMDDPlot= new TGTextButton(dControlsFrame, "Plot-BCMDD", BA_BCMDD_PLOT);
  dControlsFrame->AddFrame(dButtonBCMDDPlot, dBtnLayout);

  dButtonCorrelationPlot -> Associate(this);
  dButtonBCMDDPlot -> Associate(this);
  dListboxTrees -> Associate(this);

  dComboBoxTreePrm1 -> Associate(this);
  dComboBoxTreePrm2 -> Associate(this);

  dTreeYtxt -> Associate(this);
  dTreeXtxt -> Associate(this);
  dTreeCuttxt -> Associate(this);
  
 
  dCanvas->GetCanvas()->SetBorderMode(0);
  // dCanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)",
  // 				"QwGUICorrelationPlots",
  // 				this,"TabEvent(Int_t,Int_t,Int_t,TObject*)");

//  //  TCanvas *mc = dCanvas->GetCanvas();
// //   mc->Divide( 2, 4);

//   Int_t wid = dCanvas->GetCanvasWindowId();
//   //  TCanvas *super_canvas = new TCanvas("", 10, 10, wid);
//   QwGUISuperCanvas *mc = new QwGUISuperCanvas("", 10,10, wid);
//   dCanvas->AdoptCanvas(mc);
  //  super_canvas -> Divide(2,4);


}

void QwGUICorrelationPlots::OnReceiveMessage(char *obj)
{

}

// void QwGUICorrelationPlots::OnObjClose(char *obj)
// {
// //   if(!strcmp(obj,"dROOTFile")){
// // //     printf("Called QwGUICorrelationPlots::OnObjClose\n");

// //     dROOTCont = NULL;
// //   }
// }


// void QwGUICorrelationPlots::OnNewDataContainer()
// {


// };

// void QwGUICorrelationPlots::OnRemoveThisTab()
// {

// };

// void QwGUICorrelationPlots::ClearData()
// {

//   //  TObject *obj;
//   //   TIter next(HistArray.MakeIterator());
//   //   obj = next();
//   //   while(obj){    
//   //     delete obj;
//   //     obj = next();
//   //   }
//   HistArray.Clear();//Clear();
// }




void QwGUICorrelationPlots::PlotCorrelation()
{

  TTree *tree=NULL;
  TString tree_name=CorrelationPlotsTrees[fTreeIndex];
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  Char_t drw[100];
  TCut  cut;
  TCut  cutX;
  TCut  cutY;
  TString PrmY;
  TString PrmX;

  //  This way we can prevent fTreePrmY or fTreePrmX changing during the loop and somtines crash when one parameter has value form 1 tree while the other has value form the other tree.
  PrmY=fTreePrmY;
  PrmX=fTreePrmX;


  
  //sprintf(fCut,"%s%s",fTreePrmY.Data(),fTreePrmY.Data());
  //printf("Cut Expr%s \n ",fCutExpr.Data());
  //dTreeCuttxt->SetText(fCut);


  printf("Correlation plot of %s vs %s \n",PrmY.Data(),PrmX.Data());
  // cut = PrmX+fCutX;

  // sprintf(cut,"%s%s",PrmX.Data(),fCutX.Data());
  // sprintf(cut,"%s%s",PrmY.Data(),fCutY.Data());
  // //  printf("Cut %s \n ",cut);
  while (1){ 
    tree= (TTree *)dMapFile->Get(tree_name);
    
    mc->Clear();
    mc->Divide(1,1);

    mc->cd(1);

    if (tree==NULL){
      printf("No Such tree object exist!\n");
      break;
    }

    cutX = PrmX + fCutX;
    cutY = PrmY + fCutY;

    if (tree->FindLeaf(PrmX) && tree->FindLeaf(PrmY)){ 
           
      sprintf(drw,"%s:%s",PrmY.Data(),PrmX.Data());  
      if (bCUTEXPR)
	cut = fCutExpr;
      else{
	if (fCutY!="" && fCutX!="")
	  cut = cutX + "&&" + cutY;
	//	  sprintf(cut,"%s%s && %s%s",PrmX.Data(),fCutX.Data(),PrmY.Data(),fCutY.Data());
	else if (fCutX=="" && fCutY!="")
	  cut = cutY;
	//	  sprintf(cut,"%s%s",PrmY.Data(),fCutY.Data());
	else if (fCutY=="" && fCutX!="")
	  cut = cutX;
	//	  sprintf(cut,"%s%s",PrmX.Data(),fCutX.Data());
	else
	  cut = "";
      }
      if (tree->Draw(drw,cut)<0){
	printf("Bad numerical expression \n");
	break;
      }
      
  
    }
    else{
      printf("Unable to find one or two leafs \n");
    }

    gPad->Update();
     
    mc->Modified();
    mc->Update();
     
     
    gSystem->Sleep(2000);
    if (gSystem->ProcessEvents()){
      break;
    }
    delete tree;
  }
  

  return;
};

void QwGUICorrelationPlots::PlotBCMDD(){
  TTree *tree=NULL;
  TString tree_name=CorrelationPlotsTrees[fTreeIndex];
  TCanvas *mc = NULL;
  mc = dCanvas->GetCanvas();
  Char_t drw[100];
  TCut  cut;
  TCut  cutX;
  TCut  cutY;
  TString PrmY;
  TString PrmX;


  //stats
  gStyle->SetOptTitle(1);
  gStyle->SetStatH(0.4);
  gStyle->SetStatW(0.3);     
  gStyle->SetOptStat("eMr");//with error on the mean

  // histo parameters
  gStyle->SetTitleX(0.3);
  gStyle->SetTitleW(0.35);
  gStyle->SetTitleSize(0.03);
  gStyle->SetTitleOffset(2);
  gStyle->SetTitleColor(kBlack);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.08);

  gStyle->SetPadBottomMargin(0.2);
  gStyle->SetHistFillColor(41);

  while (1){ 
    tree= (TTree *)dMapFile->Get(tree_name);
    
    mc->Clear();
    mc->Divide(1,1);

    mc->cd(1);
    gPad->SetLogy(1);

    if (tree==NULL){
      printf("No Such tree object exist!\n");
      break;
    }

    if (tree->Draw("(asym_qwk_bcm7.hw_sum - asym_qwk_bcm8.hw_sum)*1e+6","ErrorFlag==0")<0){
	printf("Bad numerical expression \n");
	break;
    }

    ((TH1F *)(mc->FindObject("htemp")))->SetTitle("BCM-DD (7-8)");
    ((TH1F *)(mc->FindObject("htemp")))->GetXaxis()->SetTitle("ppm");
    
    gPad->Update();
     
    mc->Modified();
    mc->Update();
     
     
    gSystem->Sleep(2000);
    if (gSystem->ProcessEvents()){
      break;
    }
    delete tree;
  }
  

  return;  
};
  

// void QwGUICorrelationPlots::TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject)
// {
//   /*
//   if(event == kButton1Double){
//     Int_t pad = dCanvas->GetCanvas()->GetSelectedPad()->GetNumber();
    
//     if(pad > 0 && pad <= CORRELATION_DEV_NUM)
//       {
// 	RSDataWindow *dMiscWindow = new RSDataWindow(GetParent(), this,
// 						     GetNewWindowName(),"QwGUICorrelationPlots",
// 						     HistArray[pad-1]->GetTitle(), PT_HISTO_1D,600,400);
// 	if(!dMiscWindow){
// 	  return;
// 	}
// 	DataWindowArray.Add(dMiscWindow);
// 	dMiscWindow->SetPlotTitle((char*)HistArray[pad-1]->GetTitle());
// 	dMiscWindow->DrawData(*((TH1D*)HistArray[pad-1]));
// 	SetLogMessage(Form("Looking at %s\n",(char*)HistArray[pad-1]->GetTitle()),kTrue);

// 	return;
//       }
//   }
//   */
// }


Bool_t QwGUICorrelationPlots::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  if(dMapFileFlag){
  // Process events generated by the object in the frame.
  
  switch (GET_MSG(msg))
    {
    case kC_TEXTENTRY:
      switch (GET_SUBMSG(msg)) 
	{
	case kTE_ENTER:
	  switch (parm1) 
	    {//case values must be in same order as they are defined in the enum
	    case TXT_Y:	   
	      //printf("TXT_Y %s \n",dTreeYtxt->GetText());
	      SetYCutExpr();
	      break;
	    case TXT_X:
	      //printf("TXT_X %s \n",dTreeXtxt->GetText());
	      SetXCutExpr();
	      break;
	    case TXT_CUT:
	      //printf("TXT_CUT %s \n",dTreeCuttxt->GetText());
	      SetCutExpr();
	      break;
	    default:
	      break;
	    }
	default:
	  break;
	}
      
    case kC_COMMAND:
      //    if(dROOTCont){
	switch (GET_SUBMSG(msg))
	  {
	  case kCM_BUTTON:
	    {
	      switch(parm1)
		{
		case BA_CORR_PLOT:
		  //printf(" PLOT CORR \n ");
		  PlotCorrelation();
		  break;
		case BA_BCMDD_PLOT:
		  //printf(" PLOT CORR \n ");
		  PlotBCMDD();
		  break;

		}
	      
	      break;
	    }
	  case kCM_COMBOBOX:
	    {
	      switch (parm1) {
	      case CMB_TREE_PX:
		//printf("Combo box index %ld index %ld selected \n", parm1,parm2);
		fCMB_TREE_PX=kTRUE;
		SetCorrelationParam(CMB_TREE_PX,parm2);
		break;
	      case CMB_TREE_PY:
		//printf("Combo box index %ld index %ld selected\n", parm1,parm2);
		fCMB_TREE_PY=kTRUE;
		SetCorrelationParam(CMB_TREE_PY,parm2);
		break;
	      }
	    }
	    break;
	    
	  case kCM_MENUSELECT:
	    break;
	  case kCM_LISTBOX:
	    //List box will generate kC_COMMAND, kCM_LISTBOX, listbox id(parm1), item id(parm2) .
	    //printf("List box index %ld index %ld selected\n", parm1,parm2);
	    //Set the list box index to the fTreeIndex
	    if (parm1==LST_TREE)
	    fTreeIndex=parm2;
	    LoadLeafLists(fTreeIndex);
	    break;
	    
	default:
	  //printf("text");
	  break;
	}
      // }
      // else{
      // 	std::cout<<"Please load the map file to view data. \n";
      // }
      default:
	break;  //dTreeYlbl =new TGLabel(dTreeInputFrame1,"Y CUT");
    }
  }
  return kTRUE;
}


void QwGUICorrelationPlots::LoadLeafLists(Short_t tree_id){
  TTree * tree;
  TString tree_name=CorrelationPlotsTrees[tree_id];
  TObjArray * leaf_list;
  TObject * leaf;
  TString leafname;
  tree= (TTree *)dMapFile->Get(CorrelationPlotsTrees[tree_id]);
  if (tree==NULL){
    printf("Tree objects not loaded yet. Waiting for first map file update! \n");
    return;
  }
  leaf_list=tree->GetListOfLeaves();
  fTreePrmAry=tree->GetListOfLeaves();
  
  printf("Tree %s No.of Leafs %d \n",CorrelationPlotsTrees[tree_id],leaf_list->GetEntries());
  dComboBoxTreePrm1->RemoveAll();
  dComboBoxTreePrm2->RemoveAll();
  for(Int_t i=0;i<leaf_list->GetEntries();i++){
    leaf=leaf_list->At(i);
    dComboBoxTreePrm1->AddEntry(leaf->GetName(),i);
    dComboBoxTreePrm2->AddEntry(leaf->GetName(),i);
    //printf("Leaf %s \n",leaf->GetName());
  }
  dComboBoxTreePrm1->SetEnabled(kTRUE);
  dComboBoxTreePrm2->SetEnabled(kTRUE);
  dComboBoxTreePrm1->SortByName();
  dComboBoxTreePrm2->SortByName();

};


void QwGUICorrelationPlots::SetCorrelationParam(Short_t cmb_id, Short_t id){
  fLeaf=fTreePrmAry->At(id);
  if (cmb_id==CMB_TREE_PX){
    fTreePrmX=fLeaf->GetName();
  }else if (cmb_id==CMB_TREE_PY){
    fTreePrmY=fLeaf->GetName();
  }else{
    fLeaf=fTreePrmAry->At(0);
    fTreePrmX=fLeaf->GetName();
    fTreePrmY=fLeaf->GetName();
  }
  if (fCMB_TREE_PX && fCMB_TREE_PY)//Only two parameters are selected plot, buttone is enabled
    dButtonCorrelationPlot->SetEnabled(kTRUE);
};


void QwGUICorrelationPlots::SetXCutExpr(){
  
  TString tstr;
  fCutX=dTreeXtxt->GetText();
  printf("CUT_X %s \n",fCutX.Data());
  bCUTEXPR=kFALSE;
};
void QwGUICorrelationPlots::SetYCutExpr(){
  fCutY=dTreeYtxt->GetText();
  printf("CUT_Y %s \n",fCutY.Data());
  bCUTEXPR=kFALSE;
};
void QwGUICorrelationPlots::SetCutExpr(){
  fCutExpr=dTreeCuttxt->GetText();
  if (fCutExpr!=""){
    bCUTEXPR=kTRUE;//use this as the cut only this true 
  }
};

TString  QwGUICorrelationPlots::SetupCut(TString cutx, TString cuty, TString cutexpr){
  TString scut;

  return scut;
  
};
