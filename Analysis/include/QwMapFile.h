#ifndef __QwMAPFILE__
#define __QwMAPFILE__

#include "TMapFile.h"

class QwMapFile {

 public:
  QwMapFile();
  QwMapFile(const TString name, const TString title, Option_t *option);
  virtual ~QwMapFile();

  void Close();
  TObject* Get(const char* name, TObject* retObj = 0) { if(fMapFile){return fMapFile->Get(name,retObj);}else{return 0;}}
  TMapFile *GetMapFile() { return fMapFile; }
  void Print() { if(fMapFile) fMapFile->Print(); }
  void Update() { if(fMapFile) fMapFile->Update(); }
  void ls() { if(fMapFile) fMapFile->ls(); }
  void RemoveAll();

  static const Int_t kMapFileSize;//making it larger crashed the qwanalysis_online

 protected:
  TMapFile *fMapFile;

  ClassDef(QwMapFile,0)
};


#endif
