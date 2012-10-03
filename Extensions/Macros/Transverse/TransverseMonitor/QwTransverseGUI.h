// B.Waidyawansa 2011-10-05 10:14:17
// File: QwTransverseGUI.h

#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGMdiDecorFrame
#include "TGMdiDecorFrame.h"
#endif
#ifndef ROOT_TG3DLine
#include "TG3DLine.h"
#endif
#ifndef ROOT_TGMdiFrame
#include "TGMdiFrame.h"
#endif
#ifndef ROOT_TGMdiMainFrame
#include "TGMdiMainFrame.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
#endif
#ifndef ROOT_TGColorDialog
#include "TGColorDialog.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
#endif
#ifndef ROOT_TGuiBldNameFrame
#include "TGuiBldNameFrame.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGShutter
#include "TGShutter.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGFontDialog
#include "TGFontDialog.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGColorSelect
#include "TGColorSelect.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGMsgBox
#include "TGMsgBox.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGListTree
#include "TGListTree.h"
#endif
#ifndef ROOT_TGuiBldGeometryFrame
#include "TGuiBldGeometryFrame.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TRootEmbeddedCanvas
#include "TRootEmbeddedCanvas.h"
#endif
#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif
#ifndef ROOT_TGInputDialog
#include "TGInputDialog.h"
#endif

#include "Riostream.h"
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TSQLStatement.h>
#include <TVector.h>
#include <TStyle.h>
#include <TROOT.h>
#include <Rtypes.h>
#include <TF1.h>
#include <TText.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TPaveStats.h>
#include <TApplication.h>
#include <stdio.h>
#include <TLatex.h>
#include <math.h>

class QwTransverseGUI : public TGMainFrame {

 private:
  TGMainFrame      *fMainFrame;
  TGCompositeFrame *fCompositeFrame,*fTab1Frame,*fTab2Frame,*fTab3Frame;
  TGFont           *ufont;         // will reflect user font changes 
  TGGC             *uGC;           // will reflect user GC changes
  ULong_t           ucolor;        // will reflect user color changes
  TGLabel          *fTitleLabel,*fLabelStartSlug,*fLabelEndSlug;
  GCValues_t        GRChanges;
  TGTextButton     *fTButtonPlot, *fTButtonExit;
  TGVertical3DLine *fVertical3DLine;
  TGTab            *fMainTab;
  TRootEmbeddedCanvas *fTab1Canvas,*fTab2Canvas,*fTab3Canvas;
  TGNumberEntry    *dStartSlug,*dEndSlug;
  TSQLServer *db;


  Double_t barvaluesin[8];
  Double_t barerrorsin[8];
  Double_t barvaluesout[8];
  Double_t barerrorsout[8];
  
  Double_t oppvaluesin[4];
  Double_t opperrorsin[4];
  Double_t oppvaluesout[4];
  Double_t opperrorsout[4];

  Int_t slug_last;
  Int_t slug_first;
  Int_t slug_range;
  Bool_t usefit1;

  
public:
  QwTransverseGUI();
  ~QwTransverseGUI(){ Cleanup();};

  Bool_t  ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void    MainFunction();
  TString data_query(TString device,Int_t slug,TString ihwp);

  TString get_sum_query(TString device,TString ihwp);
  void    get_data(TString devicelist[],Int_t size,TString ihwp,
		   Double_t value[],Double_t error[]);
  void    plot_n_fit_data(Int_t size, TString fit, Double_t valuein[],Double_t errorin[], 
		       Double_t valueout[],Double_t errorout[]);
  void    fit_octants_data(TString devicelist[],Int_t slug, TString ihwp, 
			   Double_t set_p0, Double_t *get_p0, 
			   Double_t set_p1, Double_t *get_p1,
			   Double_t set_p2, Double_t *get_p2,
			   Double_t *fit_value0,Double_t *fit_error0,
			   Double_t *fit_value1,Double_t *fit_error1);

  void plot_pv_ph_in_slugs();
  void  draw_plots(TVectorD value_in, TVectorD value_out,
		   TVectorD error_in, TVectorD error_out, 
		   TVectorD valuex_in, TVectorD errorx_in, 
		   TVectorD valuex_out, TVectorD errorx_out, 
		   Int_t ko, Int_t ki, TString type);

  ClassDef(QwTransverseGUI, 0);
  
};
