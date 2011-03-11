#ifndef QWGUIDATAWINDOW_H
#define QWGUIDATAWINDOW_H

#include "RSDataWindow.h"
#include "QwGUIPlotOptionsDialog.h"

#define   M_PLOT_ATTR 1150
#define   M_PLOT_AVG  1151

class QwGUIDataWindow : public RSDataWindow {

 private:
  char                 dMiscbuffer[MSG_SIZE_MAX]; //For random use
  char                 dMiscbuffer2[MSG_SIZE_MAX];

  Int_t                dStaticChannel;

  ENDataType               dDataType;
  DataOptions              dOptions;  
  QwGUIPlotOptionsDialog  *dOptionsDlg;

  void            SetDataType(ENDataType type){dDataType = type;};

  TObject             *dStaticPlotObject;

 protected:

public:
  QwGUIDataWindow(const TGWindow *p, const TGWindow *main, 
		  const char * objName, const char *mainname,
		  const char *datatitle, ERPlotTypes type,
		  ENDataType dtype,UInt_t w, UInt_t h);
  ~QwGUIDataWindow();

  virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void           UpdatePlot(char *obj);
  void           OnObjClose(char *obj);
  void           OnReceiveMessage(char *obj){};
  ENDataType     GetDataType(){return dDataType;};
  Int_t          GetPlotChannel();
  Int_t          GetStaticChannel() {return dStaticChannel;};
  Int_t          GetStartingMacroPulse();
  Int_t          GetMacroPulseRange();
  Bool_t         AverageMacroPulses();
  Bool_t         IntegrateMacroPulses();
  Bool_t         NormalizeToBeam();
  Bool_t         SubtractPedestals();
  Bool_t         SubtractBackgrounds();
  void           SetChannel(Int_t chan);
  void           SetStaticData(TObject *obj = NULL, Int_t chan = -1);

  
  ClassDef(QwGUIDataWindow,0);
};

#endif
