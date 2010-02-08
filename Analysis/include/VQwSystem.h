#ifndef __VQwSystem_h__
#define __VQwSystem_h__

// ROOT headers
#include <TROOT.h>
#include <TNamed.h>

class VQwSystem : public TNamed {

  public:
    VQwSystem (const char* name = 0): TNamed (name, "Qweak-ROOT") { };
    virtual ~VQwSystem() { };

  ClassDef(VQwSystem,1); // corresponding ClassImp in QwRoot.h
};

#endif // __VQwSystem_h__
