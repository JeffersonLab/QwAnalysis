#ifndef QWGUIDATABASECONTAINER_H
#define QWGUIDATABASECONTAINER_H

#include "RDataContainer.h"
#include "QwGUIDatabaseSelectionDialog.h"


#ifndef ROOTCINTMODE
#include "QwDatabase.h"
#include "QwOptions.h"
#else
class QwDatabase;
#endif

class QwGUIDatabaseContainer : public RDataContainer {

 private:

  const TGWindow *dParent;
  const TGWindow *dMain;
  QwDatabase *dDatabase;
  

  Char_t dMiscbuffer[MSG_SIZE_MAX];
  Char_t dMiscbuffer2[MSG_SIZE_MAX];

 protected:

public:
  QwGUIDatabaseContainer(const TGWindow *p, const TGWindow *main, 
		 const char *objName, const char *ownername, const char *dataname, 
		 ERFileModes mode, ERFileTypes type);

  virtual ~QwGUIDatabaseContainer();

  int OpenDatabase();
  void CloseDatabase();
  Bool_t Connect(){if(dDatabase) return dDatabase->Connect(); return kFalse;};
#ifndef ROOTCINTMODE
  mysqlpp::Query Query(const char* str){ if(dDatabase) return dDatabase->Query(str); return 0;};
  mysqlpp::Query Query(){ if(dDatabase) return dDatabase->Query(); return 0;};
#endif
  void Disconnect() {if(dDatabase) dDatabase->Disconnect();};
  
  ClassDef(QwGUIDatabaseContainer,0);
};

#endif
