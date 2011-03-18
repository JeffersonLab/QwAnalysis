#include "QwRTGUISubSystem.h"


QwRTGUISubSystem(const TGWindow *main, TGCompositeFrame *parent, UInt_t w, UInt_t h)
{ 

  dHistoReset = 0;
  dHistoAccum = 0;
  dHistoPause = 0;
  dMapFile    = NULL;

  dMapFileFlag = false;

  CreateFrame(parent,w, h);
 

}

QwRTGUISubSystem::~QwRTGUISubSystem()
{

}



void 
QwRTGUISubSystem::SetMapFile(TMapFile *file)
{
  if(file) {
    dMapFile = file;
    dMapFileFlag = true;
    std::cout << "Subsystem " << std::setw(22) << this->GetName()
	      << " now can access the map file at the address "
	      << dMapFile << std::endl;
  }
  else {
    dMapFileFlag = false;
  }
  return;
};



void 
QwRTGUISubSystem::SummaryHist(TH1 *in)
{

  Double_t out[4] = {0.0};
  Double_t test   = 0.0;

         
  out[0] = in -> GetMean();
  out[1] = in -> GetMeanError();
  out[2] = in -> GetRMS();
  out[3] = in -> GetRMSError();

  Int_t entries = 0;
  entries = in->GetEntries();
  if(entries == 0) test = 0.0;
  else             test = out[2]/TMath::Sqrt(entries);

  printf("%sName%s", BOLD, NORMAL);
  printf("%22s", in->GetName());
  printf("  %sMean%s%s", BOLD, NORMAL, " : ");
  printf("[%s%+4.2e%s +- %s%+4.2e%s]", RED, out[0], NORMAL, BLUE, out[1], NORMAL);
  printf("  %sSD%s%s", BOLD, NORMAL, " : ");
  printf("[%s%+4.2e%s +- %s%+4.2e%s]", RED, out[2], NORMAL, GREEN, out[3], NORMAL);
  printf(" %sRMS/Sqrt(N)%s %s%+4.2e%s \n", BOLD, NORMAL, BLUE, test, NORMAL);
  
  return;
};
