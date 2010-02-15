/*------------------------------------------------------------------------*//*!

 \file TSuperCanvas.h
 \ingroup Extensions

 \author	Juergen Diefenbach <defi@kph.uni-mainz.de>
 \author	Jeong Han Lee      <jhlee@jlab.org>

 \date		2010-01-12 17:25:57

 \brief        Implementation of ROOT "super" canvas.


*//*-------------------------------------------------------------------------*/

//  
// They're meant to supplement some "missing" methods, which are very useful in everyday live,
// but anyhow for some reasons not implemented in original ROOT.
//


// This TSuperCanvas was degined for the PVA4 experiment at MAMI, Germany. 
// The second author (Lee) obtained permission from the PVA4 collaboration
// in order to use and modify this code for the Qweak experiment at JLab. 


// Select a pad with middle mouse button (wheel)
//
// *) "g" : grid on/off
// *) "m" : zoom on/off
// *) "l" : y log scale on/off

// *) "f"      : FFT on with a selection of TH1 histogram
//               open a new canvas of FFT result.
//    "ctrl+f" : close the result canvas of FFT

// *) measure a distance between two points (A, B)  on a selected pad,
//    Shift + mouse 1 (left) down (click) on A and + click on B
//    The distance will be dumped on "xterminal" as the following example:
//    dist. meas.: dX = 1.20e+01 dY = -6.90e+00 dist = 1.39e+01

// *) "x"       : projection X of TH2 histogram
//                open a new canvas of this projection result
//    "ctrl+x"  : close the result canvas of the projection
// *) "y"       : projection Y of TH2 histogram
//                open a new canvas of this projection result
//    "ctrl+y"  : close the result canvas of the projection


// *) more ....



// History:
// 0.0.0     2009/12/27   * create TSuperCanvas class based on the orignal 
//                          Juergen's  TSuperClass class
// 0.0.1     2010/01/12   * introduce a new canvas for a FFT result and 
//                          Projection X,Y results
//                        * introduce SetCurrentPad() in order to
//                          select an active pad according to the mouse
//                          position
// 0.0.3     2010/02/01   * some bugs were fixed
//                        * comment out several methods which are
//                          ill-defined. 
// 0.0.3     2010/02/01   * some bugs were fixed
//                        * comment out several methods which are
//                          ill-defined. 


#ifndef _TSUPERCANVAS_H_
#define _TSUPERCANVAS_H_
#include "TROOT.h"
#include "TGFrame.h"
#include "TGNumberEntry.h"
#include "TGDoubleSlider.h"
#include "TGLabel.h"
#include "TGTableLayout.h"
#include "TCanvas.h"
#include "TButton.h"
#include "TClass.h"
#include "TH1.h"
#include "TH2.h"
#include "TLine.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TVirtualX.h"

// funny colors: http://www.faqs.org/docs/abs/HTML/colorizing.html
//           or: http://linuxgazette.net/issue65/padala.html
#define RED       "\E[31m"
#define GREEN     "\E[32m"
#define BLUE      "\E[34m"
#define BOLD      "\033[1m"
#define BOLDRED   "\E[31m\033[1m"
#define BOLDGREEN "\E[32m\033[1m"
#define BOLDBLUE  "\E[34m\033[1m"
#define BACKRED   "\E[31m\033[7m"
#define BACKGREEN "\E[32m\033[7m"
#define BACKBLUE  "\E[34m\033[7m"
#define NORMAL    "\033[0m"


class TSuperCanvas : public TCanvas
{

 public:
  TSuperCanvas(const Char_t* name, const Char_t* title, Int_t ww, Int_t wh, Float_t menumargin=0.0);
  TSuperCanvas(const Char_t* name, const Char_t* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh, Float_t menumargin=0.0);
  virtual ~TSuperCanvas();

  virtual void Initialize();
  virtual void ClearPointers();
  virtual void Divide(Int_t nx=1, Int_t ny=1, Float_t xmargin=0.01, Float_t ymargin=0.01, Int_t color=0);

  //  virtual void Divide(Int_t nx=1, Int_t ny=1, Float_t xmargin=0.01, Float_t ymargin=0.01, Int_t color=0, Float_t menumargin=-1.0);

  void MakeMenu(TSuperCanvas **me, Float_t menumargin);

  virtual void HandleInput(EEventType event, Int_t px, Int_t py);
  virtual void Measure(Int_t px, Int_t py);

 
  void SetMenuMargin(Float_t menumargin) {menu_margin = menumargin;};
  void SliceTH2(Int_t px, Int_t py);
  void DiscreteFourierTransform(Int_t px, Int_t py);
  void MagnifyPad();
  void PrintPad(TVirtualPad* pad);

  
 private:
  
  void Configure();
  
  Float_t  menu_margin;
  Int_t    meas_status;
  Float_t  meas_x1, meas_x2, meas_y1, meas_y2;
/*   Float_t  peakpos[10]; */
/*   TLine   *peakline[10]; */
  
  Double_t SamplingRate, ScrollRate;

  TVirtualPad *MaximizedPad;
  Double_t MaxmizedPad_xlow;
  Double_t MaxmizedPad_ylow;
  Double_t MaxmizedPad_xup;
  Double_t MaxmizedPad_yup;
  
  //TButton *closebutton, *crossbutton, *eventbutton, *toolbutton, *logxbutton, *logybutton;

  void SetBackBeforePad(TPad* cpad);
  void SetCurrentPad(Bool_t debug=false);

  ClassDef(TSuperCanvas,1);
};

#endif
