//=============================================================================
//
//   ---------------------------
//  | Doxygen File Information |
//  ---------------------------
/**

   \file QwGUIHistories.h
   \author Michael Gericke

*/
//=============================================================================
//
//=============================================================================
//
//   ---------------------------
//  | Doxygen Class Information |
//  ---------------------------
/**
   \class QwGUIHistories

   \brief Handles the display of the slow controls data.

   The QwGUIHistories class handles the display of histories of various quantities.

 */
//=============================================================================

///
/// \ingroup QwGUIMain

#ifndef QWGUIHISTORIES_H
#define QWGUIHISTORIES_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <map>
using std::map;
using std::vector;

#include <TG3DLine.h>
#include <TRootEmbeddedCanvas.h>
#include <TRootCanvas.h>
#include <TMath.h>
#include <TH1D.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TLeaf.h>
#include <TVirtualFFT.h>
#include <TPaveText.h>
#include <TFrame.h>
#include "QwGUISubSystem.h"
#include "QwGUIDataWindow.h"
#include "RNumberEntryDialog.h"

class QwGUIHistories : public QwGUISubSystem
{

  TGVerticalFrame        *dTabFrame;
  TRootEmbeddedCanvas    *dCanvas;

  vector <TGraphErrors*>  PlotArray;  //this is the history plot array
  vector <TBox*>          BoxArray;  //this is the history plot array

  RDataContainer           *HistoryData;

  TString                   HistoryFile;
  Bool_t                    fOpen;
  Bool_t                    StartNew;
  Int_t                     dCurrentRun;
  Int_t                     OpenHistory();
  void                      FillPlots();
  void                      WriteHistory();

  //!An object array to store data window pointers -- good for use in cleanup.
  TObjArray            DataWindowArray;
  
  void                 SetSelectedDataWindow(Int_t ind) {dSelectedDataWindow = ind;};
  void                 RemoveSelectedDataWindow() {dSelectedDataWindow = -1;};
  QwGUIDataWindow     *GetSelectedDataWindow();  

  //!A dioalog for number entry ...
  RNumberEntryDialog   *dNumberEntryDlg;

  Int_t                 GetFilenameFromDialog(char *file, const char *ext,
					      ERFileStatus status, Bool_t kNotify,
					      const char *notifytext);
  
  //!This function fills histograms and graphs with the chosen data from the current file.
  //!
  //!Parameters:
  //! - none
  //!
  //!Return value: none

  void                 NewDataInit();
  
  Int_t                dSelectedDataWindow;
  
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

    void                 CleanUp();

  public:

    QwGUIHistories(const TGWindow *p, const TGWindow *main, const TGTab *tab,
                 const char *objName, const char *mainname, UInt_t w, UInt_t h);
    ~QwGUIHistories();


    //!Overwritten virtual function from QwGUISubSystem::OnNewDataContainer(). This function retrieves
    //! histograms from the ROOT file that is contained within the data container makes copies of
    //!them and adds them to a histogram pointer array for later plotting and cleanup.
    //!
    //!Parameters:
    //! - none
    //!
    //!Return value: none
    virtual void        OnNewDataContainer(RDataContainer *cont);

    virtual void        OnObjClose(char *);
    virtual void        OnReceiveMessage(char*);
    virtual void        OnRemoveThisTab();
    void                OnUpdatePlot(char *);

    virtual Bool_t      ProcessMessage(Long_t msg, Long_t parm1, Long_t);
    virtual void        TabEvent(Int_t event, Int_t x, Int_t y, TObject* selobject);

    //!This function plots histograms and graphs of the data in the current file, in the main canvas.
    //!
    //!Parameters:
    //! - a vector of plots from which to extract the new history data for the current run
    //!
    //!Return value: none
    void                 PlotData(vector <TH1F*> plots, vector <TBox*> boxes, Int_t Run);

    Int_t                GetRunNumber() {return dCurrentRun;}

    virtual void         MakeHCLogEntry(); 
    
  private:



    ClassDef(QwGUIHistories,0);
  };

#endif

