//=============================================================================
// 
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**
 
   \file QwGUICorrelationPlots.h
   \author Michael Gericke
   \author Rakitha Beminiwatha
     
*/
//=============================================================================
// 
//=============================================================================
// 
//   ---------------------------
//  | Doxygen Class Information |
//  ---------------------------
/**
   \class QwGUICorrelationPlots
    
   \brief Handles the display of the correlation plots with  scandata variables.  

   The QwGUICorrelationPlots class handles the display of the correlation plots with  scandata variables.
   It implements several functions to manipulate the data and calculate certain
   basic diagnostic quantities.

   This freamework  is Gericke's original code for the main detector. 
Added by Buddhini to display the injector beamline data.

 */
//=============================================================================

#define CORRELATION_DEV_NUM          22        
#define CORRELATION_DET_TRE_NUM      2
///
/// \ingroup QwGUICorrelationPlots

#ifndef QWGUICORRELATION_H
#define QWGUICORRELATION_H


#include "QwGUISubSystem.h"

#ifndef __CINT__

#include "QwOptions.h"
#include "QwParameterFile.h"

#endif

class QwGUICorrelationPlots : public QwGUISubSystem {

  
  TGHorizontalFrame   *dTabFrame;
  TGHorizontalFrame   *dTreeFrame;
  TGHorizontalFrame   *dTreeInputFrame1;
  TGHorizontalFrame   *dTreeInputFrame2;
  //TGHorizontalFrame   *dTreeInputFrame3;
  TGVerticalFrame     *dTreeInputFrame3;
  TGHorizontalFrame   *dTreeInputFrame4;
  TGHorizontalFrame   *dTreeInputFrame5;

  TGVerticalFrame     *dControlsFrame;
  TRootEmbeddedCanvas *dCanvas;  

  TGLayoutHints       *dTabLayout; 
  TGLayoutHints       *dCnvLayout; 
  TGLayoutHints       *dSubLayout;
  TGLayoutHints       *dBtnLayout;
  TGLayoutHints       *dTreeLayout;

  TGTextButton        *dButtonCorrelationPlot;
  TGTextButton        *dButtonBCMDDPlot;

  TGListBox           *dListboxTrees;

  TGComboBox          *dComboBoxTreePrm1;
  TGComboBox          *dComboBoxTreePrm2;

  //Tree parameters labels and txt boxes
  TGLabel             *dTreeXlbl;
  TGLabel             *dTreeYlbl;
  TGLabel             *dTreeXlbl2;
  TGLabel             *dTreeYlbl2;
  TGLabel             *dTreeCutlbl;
  

  TGTextEntry         *dTreeYtxt;
  TGTextEntry         *dTreeXtxt;
  TGTextEntry         *dTreeCuttxt;

  /* //memory mapped ROOT file */
  /* TMapFile            *fMapFile;  */

  /* //!An object array to store histogram pointers -- good for use in cleanup. */
  /* TObjArray            HistArray; */
  
  /* //!An object array to store data window pointers -- good for use in cleanup. */
  /* TObjArray            DataWindowArray; */

  TPaveText *errlabel; //To print out error messages on the pad

  TH1D *PosVariation[2] ;
/*   TGraphErrors *gx; */
/*   TGraphErrors *gy; */

 

  

 

  //!This function Draws a correlation plots from the flat tree in the  Memory map file. 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void PlotCorrelation();

  //!This function Draws bcm7-8 DD plot from the rolling buffer tree in the  Memory map file. 
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none 
  void PlotBCMDD();

  

  //!This function clear the histograms/plots in the plot container. This is done everytime a new 
  //!file is opened. If the displayed plots are not saved prior to opening a new file, any changes
  //!on the plots are lost.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  //  void                 ClearData();

  //!An array that stores the ROOT names of the histograms that I chose to display for now.
  //!These are the names by which the histograms are identified within the root file.

  static const char   *CorrelationPlotsDevices[CORRELATION_DEV_NUM];
  static const char   *CorrelationPlotsTrees[CORRELATION_DET_TRE_NUM];
  UInt_t fTreeIndex;
  

 protected:

  //!Overwritten virtual function from QwGUISubSystem::MakeLayout(). This function simply adds an
  //!embeded canvas to the tab frame for this subsystem, but almost anything can be done here ...
  //!See QwGUISubSystem::MakeLayout() for more information.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  virtual void         MakeLayout();

  void                 LoadLeafLists(Short_t tree_id);
  void                 SetCorrelationParam(Short_t cmb_id, Short_t id);
  void                 SetXCutExpr();
  void                 SetYCutExpr();
  void                 SetCutExpr();
  TString              SetupCut(TString, TString, TString);

 public:
  
  QwGUICorrelationPlots(const TGWindow *p, const TGWindow *main, const TGTab *tab,
		    const char *objName, const char *mainname, UInt_t w, UInt_t h);
  ~QwGUICorrelationPlots();


  //!Overwritten virtual function from QwGUISubSystem::OnNewDataContainer(). This function retrieves
  //!four histograms from the ROOT file that is contained within the data container makes copies of
  //!them and adds them to a histogram pointer array for later plotting and cleanup.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none  
  /* virtual void        OnNewDataContainer(); */
  /* virtual void        OnObjClose(char *); */
  virtual void        OnReceiveMessage(char*);
  /* virtual void        OnRemoveThisTab(); */

  virtual Bool_t      ProcessMessage(Long_t msg, Long_t parm1, Long_t);
  /* virtual void        TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject); */

  TString fTreePrmX;//Correlation plot parameters
  TString fTreePrmY;
  TObjArray *fTreePrmAry;
  TObject * fLeaf;
  Bool_t bCUTEXPR;
  Bool_t fCMB_TREE_PX,fCMB_TREE_PY;

  TString fCutX;
  TString fCutY;
  TString fCutExpr;

  
  ClassDef(QwGUICorrelationPlots,0);
};

#endif
