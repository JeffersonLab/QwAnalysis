/////////////////////////////////////////////////////////////////////////////////////
//Root Based Data Visualization and Analysis Utility Library
//
//Version 000
//
//RPlotContainer header file, declaring basic methods to contain and access
//graphs histograms etc..
/////////////////////////////////////////////////////////////////////////////////////


#ifndef RPLOTCONTAINER_H
#define RPLOTCONTAINER_H

#include <TQObject.h>
#include <TGFrame.h>
#include <TSystem.h>
#include <TString.h>
#include <TKey.h>
#include <TList.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TMultiGraph.h>
#include <TF1.h>
#include <TH1S.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TH2S.h>
#include <TH2F.h>
#include <TH2D.h>
#include <TH3S.h>
#include <TH3F.h>
#include <TH3D.h>
#include <TProfile.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <RPlotContainerDef.h>

struct PlotOptions {
  Color_t  lineColor;
  Style_t  lineStyle;
  Width_t  lineWidth;
  Color_t  markerColor;
  Style_t  markerStyle;
  Size_t   markerSize;
};

class RPlotContainer : public TQObject {

private:

  Int_t                dPlotCount;
  Int_t                dMsgcnt;        //Number of messages in queue
  char                 dMainName[NAME_STR_MAX]; 
  char                 dObjName[NAME_STR_MAX]; 
  char                 dMiscbuffer[MSG_SIZE_MAX]; //For random use
  char                 dMiscbuffer2[MSG_SIZE_MAX];
  char                 dContTitle[NAME_STR_MAX];
  char                 dMessage[MSG_QUEUE_MAX][MSG_SIZE_MAX]; //Message in queue

  PlotOptions         *dPlotOptions;

  TList               *fPlotList;
  TList               *fGraphList;  
  TList               *fGraphErList;
  TList               *fGraphAsymErList;
  TList               *fD1HistoList;
  TList               *fD2HistoList;
  TList               *fD3HistoList;
  TList               *fFuncList;
  TList               *fD1ProfileList;
  
  TMultiGraph         *fMultiGraph;

 protected:

  Bool_t               SetMessage(char *msg, 
				  char *func,
				  Int_t TS, 
				  Int_t MESSAGETYPE); //Add message to queue
  void                 FlushMessages();//Get rid of all messages in queue

  
public:

  RPlotContainer(const TGWindow *p, const TGWindow *main,
		 char * objName, char *mainname,
		 char *conttitle);
  virtual ~RPlotContainer();

  TMultiGraph         *AddMultiGraphObject(TObject *obj, const char *title, const char *type, const char *opts);
  TMultiGraph         *AddMultiGraphObject(const char* name, const char *title,
					   Int_t n, Double_t *x, Double_t *y,
					   Double_t *exl, Double_t *eyl,
					   Double_t *exh, Double_t *eyh,
					   const char *type, const char *opts);
  void                 ClearPlots();
  Int_t                GetAsymErGraphCount();
  Char_t              *GetContainerTitle() {return dContTitle;};
  Int_t                Get1DHistoCount();
  Int_t                Get1DProfileCount();
  Int_t                Get2DHistoCount();
  Int_t                GetErGraphCount();
  Int_t                GetFuncCount();
  TObject             *GetGraph(Int_t index, Char_t *type, Char_t *name = NULL);
  Int_t                GetGraphCount();
  TObject             *GetHistogram(Int_t index, const Char_t *type, const Char_t *name = NULL);
  TObject             *GetProfile(Int_t index, Char_t *type, Char_t *name = NULL);
  Char_t              *GetMessage();
  TH1D                *GetNew1DHistogram(Char_t*,Char_t*,Int_t,Double_t,Double_t);
  TH2D                *GetNew2DHistogram(Char_t*,Char_t*,Int_t,Double_t,Double_t,Int_t,Double_t,Double_t);
  TH1D                *GetNew1DHistogram(const TH1D& hist);
  TH2D                *GetNew2DHistogram(const TH2D& hist);
  TGraphAsymmErrors   *GetNewAsymErrorGraph(Char_t*, Char_t*, Int_t, Double_t*, 
					    Double_t*, Double_t*, Double_t*, 
					    Double_t*, Double_t*);
  TGraphAsymmErrors   *GetNewAsymErrorGraph(const TGraphAsymmErrors& gr);
  TGraphErrors        *GetNewErrorGraph(Char_t*, Char_t*, Int_t, Double_t*, 
					Double_t*, Double_t*, Double_t*);
  TGraphErrors        *GetNewErrorGraph(const TGraphErrors& gr);
  TF1                 *GetNewFunction(const TF1& f1);
  TGraph              *GetNewGraph(Char_t*, Char_t*, Int_t, Double_t*, Double_t*);
  TGraph              *GetNewGraph(const TGraph& gr);
  TProfile            *GetNew1DProfile(const TProfile& prof);

  Int_t                GetNewLineColor(Color_t col = 0);
  Int_t                GetNewMarkerColor(Color_t col = 0);
  TObject             *GetObject(Int_t index, Char_t *name = NULL);
  TObject             *GetPlot(Int_t index);
  Int_t                GetPlotCount();
  PlotOptions         *GetPlotOptions(){return dPlotOptions;};

  void                 RemovePlot(TObject *obj);
  void                 SetDefaultPlotOptions();
  void                 SetLineColor(Color_t col) {dPlotOptions->lineColor = col;};
  void                 SetLineStyle(Style_t sty) {dPlotOptions->lineStyle = sty;};
  void                 SetLineWidth(Width_t wid) {dPlotOptions->lineWidth = wid;};
  void                 SetMarkerColor(Color_t col){dPlotOptions->markerColor= col;};
  void                 SetMarkerStyle(Style_t sty){dPlotOptions->markerStyle= sty;};
  void                 SetMarkerSize(Size_t sze) {dPlotOptions->markerSize = sze;};
  void                 SetNewLineColor();
  void                 SetNewMarkerColor();
  void                 SetPlotOptions(PlotOptions *options);

  void                 IsClosing(char *); //Emit signal that this object is closing
  void                 SendMessageSignal(char*);//Emit signal when message is posted


  ClassDef(RPlotContainer,0);
};

#endif 
